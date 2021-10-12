/*
 ****************************************************************
 *								*
 *			do_fsck.c				*
 *								*
 *	Verifica a consistência do sistema de arquivos		*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.8.0, de 09.07.05				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Variáveis diversas **************************************
 */
entry char	*chk_fat;	/* Ponteiro para FAT de teste */
entry int	chk_busy_clus;	/* Número de clusters livres */
entry int	chk_n_files;	/* Número de arquivos */

/*
 ******	Protótipos de funções ***********************************
 */
void		fsck_size_check (void);
int		fsck_size_function (const char *, DOSSTAT *, int);
void		fsck_free_list_check (void);
void		fsck_dir_check (void);
int		fsck_dir_function (const char *, DOSSTAT *, int);
void		fsck_cmp_fat (void);
void		fsck_cmp_sb (void);
void		fsck_cmp_info (void);
void		do_fsck_help (void);

/*
 ****************************************************************
 *	Verifica a consistência do sistema de arquivos		*
 ****************************************************************
 */
void
do_fsck (int argc, const char *argv[])
{
	const UNI	*up = &uni;
	int		opt, n, u;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "fH")) >= 0)
	{
		switch (opt)
		{
		    case 'f':			/* Força */
			cmd_fflag++;
			break;

		    case 'H':			/* Help */
			do_fsck_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_fsck_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_fsck_help (); return; }

	if (cmd_fflag && fs_status != FS_MOUNT_RW)
	{
		printf
		(	"Sistema de arquivos DOS montado apenas para "
			"leituras: opção \"-f\" ignorada\n\n"
		);
		cmd_fflag = 0;
	}

	printf ("*** Analisando \"%s\"", dev_nm);

	if (vol_nm[0])
		printf (" (%s)", vol_nm);

	chk_n_files = 0;

	printf (": %s\n\n", cv_blocks_to_KB_MB_or_GB (up->u_n_clusters * up->u_sectors_per_cluster));

	if (received_intr)
		return;

	/*
	 *	Fase  1: Verificando clusters e ciclos na FAT
	 */
	get_whole_fat ();

	chk_fat = alloca (up->u_n_clusters + 2);

	fsck_cycle_check (1 /* print */);

	if (received_intr)
		return;

	/*
	 *	Fase  2: Verificando blocos e tamanhos
	 */
	fsck_size_check ();

	if (received_intr)
		return;

	/*
	 *	Fase  3: Verificando a lista livre da FAT
	 */
	fsck_free_list_check ();

	if (received_intr)
		return;

	/*
	 *	Fase 4: Verificando os diretórios
	 */
	fsck_dir_check ();

	if (received_intr)
		return;

	/*
	 *	Fase 5: Comparando as duas FATs
	 */
	fsck_cmp_fat ();

	if (received_intr)
		return;

	/*
	 *	Fase 6: Analisando a cópia do superbloco
	 */
	fsck_cmp_sb ();

	if (received_intr)
		return;

	/*
	 *	Fase 7: Analisando o bloco de informações da FAT32
	 */
	fsck_cmp_info ();

	if (received_intr)
		return;

	/*
	 *	Estatística final
	 */
	n = up->u_n_clusters * up->u_sectors_per_cluster;
	u = chk_busy_clus * up->u_sectors_per_cluster;

	printf
	(	"\n*** %d Arquivos, %d Blocos, %d Usados (%s %%)\n",
		chk_n_files, n, u,
		editpercent (getpercent (BLTOKB (u), BLTOKB (n)))
	);

}	/* end do_fsck */

/*
 ****************************************************************
 *	Fase  1: Verificando clusters e ciclos na FAT		*
 ****************************************************************
 */
int
fsck_cycle_check (int print)
{
	const UNI	*up = &uni;
	int		index, end_index;
	int		clusno, n;
	int		errors = 0;

	if (print)
		printf ("*** Fase  1: Verificando clusters e ciclos na FAT\n");

	/*
	 *	Percorre a FAT
	 */
	memset (chk_fat, 0, up->u_n_clusters + 2);

	for
	(	index = 2, end_index = index + up->u_n_clusters;
		index < end_index;
		index++
	)
	{
		clusno = get_fat_value (index);

		if (C_ISFREE (clusno))
			continue;

		if (clusno >= 2 && clusno < up->u_n_clusters + 2)
		{
			if (++chk_fat[clusno] > 1)
			{
				if (print) printf
				(	"Cluster %d duplicado\n",
					clusno
				);
				errors++;
			}

			continue;
		}

		if (C_ISBAD_OR_EOF (clusno))
			continue;

		if (print) printf
		(	"Cluster %d inválido na FAT\n",
			clusno
		);

		errors++;

	}	/* end for (percorrendo a FAT) */

	/*
	 *	Se já encontrou erros, não realiza o segundo teste
	 */
	if (errors > 0)
		return (errors);

	/*
	 *	Verifica se não existem ciclos na FAT
	 */
	memset (chk_fat, 0, up->u_n_clusters + 2);

	for
	(	index = 2 /**** , end_index = index + up->u_n_clusters ***/;
		index < end_index;
		index++
	)
	{
		if ((clusno = get_fat_value (index)) < 2 || clusno >= up->u_n_clusters + 2)
			continue;

		if (chk_fat[clusno])
			continue;

		for (n = up->u_n_clusters; /* abaixo */; n--)
		{
			if (n <= 0)
			{
				if (print)
					printf ("A FAT tem ciclos\n");

				errors++;
				break;
			}

			if (C_ISFREE (clusno = get_fat_value (clusno)))
			{
				if (print)
					printf ("Cluster LIVRE em uma cadeia da FAT\n");

				errors++;
				break;
			}

			if (C_ISBAD (clusno))
			{
				if (print)
					printf ("Cluster RUIM em uma cadeia da FAT\n");

				errors++;
				break;
			}

		   /***	if (C_ISEOF (clusno))	***/
		   /***		break;		***/

			if (clusno < 2 || clusno >= up->u_n_clusters + 2)
				break;

			chk_fat[clusno] = 1;

		}	/* Percorrendo uma cadeia */

	}	/* Procurando ciclos */

	return (errors);

}	/* fsck_cycle_check */

/*
 ****************************************************************
 *	Fase  2: Verificando blocos e tamanhos			*
 ****************************************************************
 */
void
fsck_size_check (void)
{
	printf ("*** Fase  2: Verificando blocos e tamanhos\n");

	/*
	 *	Percorre as árvores dadas
	 */
	memset (chk_fat, 0, uni.u_n_clusters + 2);

	dos_ftw ("/", fsck_size_function);

}	/* end fsck_size_check */

/*
 ****************************************************************
 *	Função chamada por "ftw" (Fase 2)			*
 ****************************************************************
 */
int
fsck_size_function (const char *nm, DOSSTAT *zp, int type)
{
	const UNI	*up = &uni;
	int		clusno, fat_size = 0;
	int		dir_size, should_fix = 0;

	/*
	 *	Verifica se o nome é válido
	 */
	clusno = zp->z_cluster;

	if (zp->z_name[0] == ' ')
	{
		printf
		(	"Diretório \"%s\", clusno = %d: nome em branco (inválido)\n",
			nm[0] == '\0' ? "/" : nm, clusno
		);
		should_fix++;
	}

#if (0)	/*******************************************************/
	if (up->u_fat_bits != 32 && streq (nm, "/"))
		goto coda;
#endif	/*******************************************************/

	chk_n_files++;

	if (!Z_ISREG (zp->z_mode) && !Z_ISDIR (zp->z_mode))
		goto coda;

	/*
	 *	Examina o cluster inicial
	 */
	dir_size = GET_LONG (zp->z_size);

	if (clusno == 0)
	{
		if (dir_size == 0)
			goto coda;
		else
			goto bad;
	}

	/*
	 *	Examina a cadeia de clusters
	 */
	for (EVER)
	{
		if (C_ISEOF (clusno))
			break;

		if (clusno >= up->u_n_clusters + 2 || clusno < 2)
		{
			printf
			(	"%s: cluster (%d) inválido\n",
				nm, clusno
			);
			should_fix++;
		}

		fat_size += CLUSZ;

		if (chk_fat[clusno] != 0)
		{
			printf ("%s: Cluster (%d) duplicado\n", nm, clusno);
			should_fix++;
		}

		chk_fat[clusno]++;

		clusno = get_fat_value (clusno);
	}

	/*
	 *	Lembrar da raiz da FAT32
	 */
	if (up->u_fat_bits == 32)
		chk_fat[up->u_root_cluster]++;

	/*
	 *	Verifica o tamanho
	 */
	if (!Z_ISREG (zp->z_mode))
		goto coda;

	if (dir_size > fat_size || dir_size <= fat_size - CLUSZ)
	{
	    bad:
		printf
		(	"%s: Tamanho inválido: %d :: %d\n",
			nm, dir_size, fat_size
		);
		should_fix++;
	}

	/*
	 *	Verifica se deve consertar
	 */
    coda:
	if (!should_fix || fs_status != FS_MOUNT_RW)
		return (0);

	fprintf (stderr, "Remove? (n): ");

	if (!cmd_fflag)
	{
		if (askyesno () <= 0)
			return (0);
	}
	else
	{
		fprintf (stderr, "SIM\n");
	}

	/*
	 *	Zera a entrada
	 */
	dos_lfn_dir_clr (zp);

	return (0);

}	/* end fsck_size_function */

/*
 ****************************************************************
 *	Fase  3: Verificando a lista livre da FAT		*
 ****************************************************************
 */
void
fsck_free_list_check (void)
{
	int	n, index;
	int	clusno, bad_free_list = 0;

	printf ("*** Fase  3: Verificando a lista livre da FAT\n");

	/*
	 *	Verifica se as entradas livres da FAT
	 *	estão realmente livres e calcula o tamanho
	 */
	for (n = 0, index = 2; index < uni.u_n_clusters + 2; index++) 
	{
		if   (chk_fat[index] != 0)
		{
			n++;
		}
		else
		{
			clusno = get_fat_value (index);

			if (!C_ISFREE (clusno) && !C_ISBAD (clusno))
				bad_free_list++;
		}
	}

	chk_busy_clus = n;

	if (bad_free_list == 0)
		return;

	/*
	 *	A lista livre está inválida
	 */
	fprintf (stderr, "Faltam %d clusters na lista livre", bad_free_list);

	if (fs_status == FS_MOUNT_RW)
	{
		fprintf (stderr, ". Conserta? (n): ");

		if (!cmd_fflag)
		{
			if (askyesno () <= 0)
				return;
		}
		else
		{
			fprintf (stderr, "SIM\n");
		}
	}
	else
	{
		fprintf (stderr, "\n");
		return;
	}

	/*
	 *	Conserta a lista livre
	 */
	for (index = 2; index < uni.u_n_clusters + 2; index++) 
	{
		clusno = get_fat_value (index);

		if (chk_fat[index] == 0 && clusno != 0 && !C_ISBAD (clusno))
			put_fat_value (0, index);
	}

}	/* end fsck_free_list_check */

/*
 ****************************************************************
 *	Fase  4: Verificando os diretórios			*
 ****************************************************************
 */
void
fsck_dir_check (void)
{
	printf ("*** Fase  4: Verificando os diretórios\n");

	/*
	 *	Percorre as árvores dadas
	 */
	dos_ftw ("/", fsck_dir_function);

}	/* end fsck_dir_check */

/*
 ****************************************************************
 *	Função chamada por "ftw" para testar os diretórios (4)	*
 ****************************************************************
 */
int
fsck_dir_function (const char *nm, DOSSTAT *zp, int type)
{
	int		son_clusno;
	int		cluster, par_clusno;
	DOSSTAT		z;

	if (!Z_ISDIR (zp->z_mode))
		return (0);

	if (streq (nm, "/"))
		return (0);

	son_clusno = zp->z_cluster;

	/*
	 *	Procura a entrada "." no diretório
	 */
	if (dos_fstat (".", son_clusno, &z) < 0)
	{
		printf
		(	"O diretório \"%s\" não tem a entrada \".\"\n",
			nm
		);
		return (0);
	}

	if ((cluster = z.z_cluster) != son_clusno)
	{
		printf
		(	"A entrada \".\" do diretório \"%s\" tem o cluster "
			"errado: %d :: %d\n",
			nm, cluster, son_clusno
		);
		return (0);
	}

	/*
	 *	Procura a entrada ".." no diretório
	 */
	if (dos_fstat ("..", son_clusno, &z) < 0)
	{
		printf
		(	"O diretório \"%s\" não tem a entrada \"..\"\n",
			nm
		);
		return (0);
	}

	par_clusno = z.z_cluster;

	/*
	 *	Procura a entrada "." no diretório pai
	 */
	if (dos_fstat (".", par_clusno, &z) < 0)
	{
		printf
		(	"O diretório pai de \"%s\" não tem a entrada \".\"\n",
			nm
		);
		return (0);
	}

	/*
	 *	Confere o ".." deste diretório com o "." do pai
	 */
	if ((cluster = z.z_cluster) != par_clusno)
	{
		printf
		(	"A entrada \"..\" do diretório \"%s\" tem o cluster "
			"errado: %d :: %d\n",
			nm, par_clusno, cluster
		);
		return (0);
	}

	return (0);

}	/* end fsck_dir_function */

/*
 ****************************************************************
 *	Fase  5: Comparando as duas FATs			*
 ****************************************************************
 */
void
fsck_cmp_fat (void)
{
#define	NSEC		16
	UNI		*up = &uni;
	int		sectors, size, i = 0;
	long		offset1, offset2;
	char		area1[NSEC * BLSZ], area2[NSEC * BLSZ];

	/*
	 *	Verifica se tem sentido
	 */
	if (up->u_n_fats < 2)
		return;

	/*
	 *	Lê a FAT1
	 */
	printf ("*** Fase  5: Comparando as duas FATs\n");

	offset1 = BLSZ * up->u_fat1_blkno;
	offset2 = BLSZ * up->u_fat2_blkno;

	for (sectors = up->u_sectors_per_fat; sectors > 0; sectors -= NSEC)
	{
		if (sectors >= NSEC)
			size = NSEC * BLSZ;
		else
			size = sectors * BLSZ;

		lseek (dev_fd, offset1, L_SET);

		if (read (dev_fd, area1, size) != size)
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura da FAT (%s)\n",
				pgname, strerror (errno)
			);
			return;
		}

		lseek (dev_fd, offset2, L_SET);

		if (read (dev_fd, area2, size) != size)
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura da FAT (%s)\n",
				pgname, strerror (errno)
			);
			return;
		}

		if (!memeq (area1, area2, size))
		{
			printf
			(	"Diferença entre as duas FATs, bloco %d\n",
				i
			);
		}

		i += NSEC;
	}

}	/* end fsck_cmp_fat */

/*
 ****************************************************************
 *	Fase 6: Analisando a cópia do superbloco		*
 ****************************************************************
 */
void
fsck_cmp_sb (void)
{
	const UNI	*up = &uni;
	char		sb_copy[BLSZ];

	if (up->u_sb_backup == 0)
		return;

	/*
	 *	Analisando a cópia do superbloco
	 */
	printf ("*** Fase  6: Analisando a cópia do superbloco\n");

	bread (up->u_sb_backup, &sb_copy);

	if (memeq (&sb_copy, &sb, sizeof (BLSZ)))
		return;

	fprintf (stderr, "Diferença entre os dois SBs");

	/*
	 *	Verifica se deve atualizar o SB
	 */
	if (fs_status != FS_MOUNT_RW)
		{ fprintf (stderr, "\n"); return; }

	fprintf (stderr, ". Conserta? (n): ");

	if (!cmd_fflag)
	{
		if (askyesno () <= 0)
			return;
	}
	else
	{
		fprintf (stderr, "SIM\n");
	}

	bwrite (up->u_sb_backup, &sb);

}	/* end fsck_cmp_sb */

/*
 ****************************************************************
 *	Fase  7: Analisa o bloco de informações			*
 ****************************************************************
 */
void
fsck_cmp_info (void)
{
	UNI		*up = &uni;
	int		index, end_index;
	int		clusno;
	int		free_clusters = 0;
	int		first_free_cluster = -1;

	if (up->u_fs_info == 0)
		return;

	/*
	 *	Analisando o bloco de informações
	 */
	printf ("*** Fase  7: Analisando as informações da FAT32\n");

	memset (chk_fat, 0, up->u_n_clusters + 2);

	for
	(	index = 2, end_index = index + up->u_n_clusters;
		index < end_index;
		index++
	)
	{
		clusno = get_fat_value (index);

		if (!C_ISFREE (clusno))
			continue;

		free_clusters++;

		if (first_free_cluster < 0)
			first_free_cluster = index;

	}	/* end for (percorrendo a FAT) */

	/*
	 *	Verifica se o valor obtido está correto
	 */
#if (0)	/*******************************************************/
	if (up->u_infree != free_clusters)
	{
		error
		(	"Valor de \"u_infree\" incorreto: %d :: %d\n",
			free_clusters, up->u_infree 
		);
		up->u_infree = free_clusters;
	}
#endif	/*******************************************************/

	if
	(	free_clusters == up->u_infree
	/*** &&	first_free_cluster == up->u_disk_inxtfree ***/
	)
		return;

	fprintf
	(
		stderr,
		"Inconsistência: %d :: %d",
		free_clusters, up->u_infree
	);

   /***	printf ***/
   /***	(	"Inconsistência: %d :: %d\n", ***/
   /***		first_free_cluster, up->u_disk_inxtfree ***/
   /***	); ***/

	/*
	 *	Verifica se deve atualizar o bloco de informações
	 */
	if (fs_status != FS_MOUNT_RW)
	{
		fprintf (stderr, "\n");
		return;
	}

	fprintf (stderr, ". Conserta? (n): ");

	if (!cmd_fflag)
	{
		if (askyesno () <= 0)
			return;
	}
	else
	{
		fprintf (stderr, "SIM\n");
	}

	up->u_infree = free_clusters;

	update_fat32_info ();

}	/* end fsck_cmp_info */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_fsck_help (void)
{
	fprintf
	(	stderr,
		"%s - verifica a consistência do sistema de arquivos\n"
		"\nSintaxe:\n"
		"\t%s [-f]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Conserta sem consultar o usuário\n"
	);

}	/* end do_fsck_help */
