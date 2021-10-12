/*
 ****************************************************************
 *								*
 *			do_mount.c				*
 *								*
 *	Monta um dispositivo com um sistema de arquivos DOS	*
 *								*
 *	Versão	3.0.0, de 17.11.93				*
 *		4.6.0, de 11.09.04				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/kcntl.h>
#include <sys/stat.h>
#include <sys/disktb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <mntent.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****** Nome da tabela ******************************************
 */
const char		fstab_nm[] = "/etc/fstab";

/*
 ****** Abre com O_LOCK ou não **********************************
 */
#define	USE_LOCK 0	/* Apenas para RO */

/*
 ******	Tabela de dispositivos numéricos ************************
 */
#define	DEV_0	"/dev/fd0"
#define	DEV_1	"/dev/fd1"
#define	DEV_2	"/dev/zip0"
#define	DEV_3	"/dev/zip1"

typedef struct disktb	DISKTB;

const char	num_dev_list[] =  "0123";

const char	num_dev_option[] =
{
	"\t-0: O <dispositivo> usado é \"" DEV_0 "\"\n"
	"\t-1: O <dispositivo> usado é \"" DEV_1 "\"\n"
	"\t-2: O <dispositivo> usado é \"" DEV_2 "\"\n"
	"\t-3: O <dispositivo> usado é \"" DEV_3 "\"\n"
};

/*
 ******	Constantes dependentes se a FAT é de 12 ou 16 bits ******
 */
entry int	BAD_BEGIN;	/* 0xFF1 ou 0xFFF1 */
entry int	EOF_BEGIN;	/* 0xFF8 ou 0xFFF8 */
entry int	STD_EOF;	/* 0xFFF ou 0xFFFF */

/*
 ******	Protótipos de funções ***********************************
 */
void		panic_uni_block (void);
void		do_mount_help (void);

/*
 ****************************************************************
 *	Monta um dispositivo com um sistema de arquivos DOS	*
 ****************************************************************
 */
void
do_mount (int argc, const char *argv[])
{
	int		opt;
	const char	*dev_str = NOSTR;
	char		dev_no_nm[4];
	DISKTB		ud;
	char		fs_partnm[32];

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "0123456789H")) >= 0)
	{
		if (opt >= '0' && opt <= '9')
		{
			if (dev_str != NOSTR)
				do_mount_help ();

			dev_no_nm[0] = '-';
			dev_no_nm[1] = opt;
			dev_no_nm[2] = '\0';

			dev_str = dev_no_nm;
		}
		else switch (opt)
		{
		    case 'H':			/* Help */
			do_mount_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mount_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if   (argc == 1)
	{
		if (dev_str != NOSTR)
			{ do_mount_help (); return; }

		dev_str = argv[0];
	}
	elif (argc > 1)
	{
		do_mount_help ();
		return;
	}

	/*
	 *	Abre o dispositivo
	 */
	if (simple_open (dev_str, GET_SHORT (sb.s_n_heads)) < 0)
		return;

	/*
	 *	Controla o tipo da partição
	 */
	if (kcntl (K_GET_DEV_TB, dev_dev, &ud) >= 0)
	{
		switch (dev_type = ud.p_type)
		{
		    case 0x00:		/* Disquete */
		    case DOS_FAT12:
		    case DOS_FAT16:
		    case DOS_FAT16_G:
		    case DOS_FAT16_L:
		    case DOS_FAT32_G:
		    case DOS_FAT32_L:
			break;

		    default:
			kcntl (K_GET_PARTNM, ud.p_type, fs_partnm);

			printf
			(	"%s: CUIDADO: O tipo da partição de \"%s\" é %s\n",
				cmd_nm, dev_nm, fs_partnm
			);
			break;
		}
	}

	/*
	 *	Monta o dispositivo
	 */
	simple_mount ();

}	/* end do_mount */

/*
 ****************************************************************
 *	Monta o dispositivo					*
 ****************************************************************
 */
int
simple_open (const char *nm, int heads)
{
	STAT		s;

	/*
	 *	Transforma alguns nomes de sistemas de arquivos
	 */
	if   (nm == NOSTR)
		nm = dev_nm;
	elif (streq (nm, "-0"))
		nm = DEV_0;
	elif (streq (nm, "-1"))
		nm = DEV_1;
	elif (streq (nm, "-2"))
		nm = DEV_2;
	elif (streq (nm, "-3"))
		nm = DEV_3;

	if (nm[0] == '\0')
	{
		printf
		(	"%s: Ainda não foi dado o nome do dispositivo DOS\n",
			cmd_nm
		);
		return (-1);
	}

	/*
	 *	Verifica se já tem um sistema de arquivos montado
	 */
   	if (fs_status > FS_CLOSED)
	{
	   /***	put_fat (); ***/

		close (dev_fd);

		if (streq (dev_nm, nm) && heads == 2)
		{
			printf
			(	"%s: Pode trocar o sistema de arquivos de \"%s\": ",
				cmd_nm, nm
			);

			getchar ();
		}
	}

   	fs_status = FS_CLOSED;
   	strcpy (dev_nm, nm);
	vol_nm[0] = '\0';
   /***	cwd_cluster = 0;	***/
	dev_fd = -1;
	dev_ro = 0;

	/*
	 *	Verifica se foi dado um diretório
	 */
	if (stat (nm, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return (-1);
	}

	if (S_ISDIR (s.st_mode))
	{
		FILE		*fp;
		int		code;
		char		fstab_dev_nm[64];
		char		fstab_dir_nm[64];
		MNTENT		mntent;

		/*
		 *	Procura no arquivo FSTAB
		 */
		if ((fp = fopen (fstab_nm, "r")) == NOFILE)
			error ("$*Não consegui abrir \"%s\"", fstab_nm);

		for (EVER)
		{
			if ((code = getmntent (fp, fstab_dev_nm, fstab_dir_nm, &mntent)) == EOF)
				error ("$Não achei \"%s\" em \"%s\"", nm, fstab_nm);

			if (code == 0)
				continue;

			if (streq (fstab_dir_nm, nm))
				break;
		}

		fclose (fp);

		strncpy (dev_nm, fstab_dev_nm, sizeof (dev_nm)); nm = dev_nm;
	}

	/*
	 *	Abre o novo dispositivo
	 */
	if (!rflag)
	{
		if ((dev_fd = open (nm, O_RDWR|O_LOCK)) < 0)
		{
			if (errno != EROFS)
			{
				fprintf
				(	stderr,
					"%s: Não consegui abrir \"%s\" (%s)\n",
					cmd_nm, nm, strerror (errno)
				);
				return (-1);
			}

#if (0)	/*******************************************************/
			fprintf
			(	stderr,
				"%s: CUIDADO: o dispositivo \"%s\" está com "
				"proteção de escrita\n"
				"%s: Montando o sistema de arquivos somente "
				"para leituras\n",
				cmd_nm, nm, cmd_nm
			);
#endif	/*******************************************************/

			dev_ro++;
		}
	}

	if (dev_fd < 0 && (dev_fd = open (nm, O_RDONLY|USE_LOCK)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return (-1);
	}

	if (fstat (dev_fd, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return (-1);
	}

	if (s.st_rdev == NODEV)		/* Em todo o caso, ... */
		s.st_rdev = 0;

	dev_mode = s.st_mode;

	switch (s.st_mode & S_IFMT)
	{
	    case S_IFREG:
	    case S_IFBLK:
		break;

	    default:
		fprintf
		(	stderr,
			"%s: O dispositivo \"%s\" não é regular nem de blocos\n",
			cmd_nm, nm
		);
		return (-1);
	}

   	fs_status = FS_OPEN;
   /***	strcpy (dev_nm, nm)	***/
   /***	vol_nm[0] = '\0';	***/
   /***	cwd_cluster = 0;	***/

	dev_dev = s.st_rdev;

	return (0);

}	/* end simple_open */

/*
 ****************************************************************
 *	Monta o dispositivo					*
 ****************************************************************
 */
void
simple_mount (void)
{
	UNI		*up = &uni;
	int		sb_error = 0;
	daddr_t		blkno;
	enum		{ BL4BLMASK = BL4SZ/BLSZ - 1 };

	/*
	 *	Consistência
	 */
   	if (fs_status != FS_OPEN)
	{
		error ("simple_mount: estado inválido: %d", fs_status);
		return;
	}

	/*
	 *	Prepara os parâmetros universais
	 */
	bread (0, &sb);

	if (get_uni_block () < 0)
		sb_error++;

	/*
	 *	Verifica se o SB está válido
	 */
	if (sb_error > 0)
	{
		printf
		(	"%s: %sdispositivo não montado\n",
			cmd_nm, "O superbloco está INVÁLIDO - "
		);

		return;
	}

	/*
	 *	Lê a(s) FAT(s)
	 */
   /***	get_fat (); ***/

	alloc_fat ();

#if (0)	/*******************************************************/
	/*
	 *	Pequena consistência da FAT
	 */
	if (fsck_cycle_check (0 /* nada imprime */) != 0)
	{
		if (sb_error == 0)
		{
			printf
			(	"%s: %suse o comando \"fsck\"\n",
				cmd_nm, "A FAT está defeituosa - "
			);
		}
		else
		{
			printf
			(	"%s: %sdispositivo não montado\n",
				cmd_nm, "A FAT está defeituosa - "
			);
		}
	}
	else
#endif	/*******************************************************/
	{
		sb_error = 0;
	}

	/*
	 *	Se ainda não tem SB válido, só com "mkfs"
	 */
	if (sb_error > 0)
		return;

	/*
	 *	Verifica se o último bloco do último CLUSTER exige ler após o sistema de arquivos
	 */
	blkno = up->u_cluster_0_blkno + (up->u_n_clusters + 2) * up->u_sectors_per_cluster;

	if ((blkno & BL4BLMASK) && (blkno & ~BL4BLMASK) + BL4BLMASK + 1 > up->u_end_fs_blkno)
	{
		int	value = get_fat_value (up->u_n_clusters + 1);

		if (!C_ISBAD (value))
		{
			fprintf (stderr, "%s: Último CLUSTER (%d) em posição inválida", cmd_nm, up->u_n_clusters + 1);

			if (rflag)
			{
				fprintf (stderr, "\n");
			}
			else
			{
				fprintf (stderr, ". Deseja excluí-lo? (n): ");

				if (askyesno () > 0)
				{
					if (value != 0)
					{
						fprintf (stderr, "O CLUSTER NÃO está VAGO\n");
					}
					else
					{
						put_fat_value (BAD_BEGIN, up->u_n_clusters + 1);
						up->u_infree--;
						update_fat32_info ();
					}
				}
			}
		}
	}

	/*
	 *	Atualiza o estado
	 */
   /***	fs_status = veja abaixo	***/
   /***	dev_nm[0] = inalterado	***/
   /***	vol_nm[0] = veja abaixo	***/
   /***	cwd_cluster = 0; ***/

	dir_walk (vol_search, uni.u_root_cluster); 

   	fs_status = FS_MOUNT_RO;

}	/* end simple_mount */

/*
 ****************************************************************
 *	Prepara os parâmetros universais			*
 ****************************************************************
 */
int
get_uni_block (void)
{
	DOSSB		*sp = &sb;
	UNI		*up = &uni;

	/*
	 *	Inicialmente examina parâmetros básicos
	 */
	memset (up, 0, sizeof (UNI));

	if (sp->s_boot_valid[0] != 0x55 || sp->s_boot_valid[1] != 0xAA)
	{
		printf
		(	"%s: Assinatura \"55AA\" inválida do superbloco DOS\n",
			cmd_nm
		);
		return (-1);
	}

	if ((up->u_end_fs_blkno = GET_SHORT (sp->s_little_size)) == 0)
		up->u_end_fs_blkno = GET_LONG (sp->s_big_size);

	if   (memeq (sp->s_32_fat_msg, "FAT32", 5))
	{
		up->u_fat_bits = 32;
	}
	elif (memeq (sp->s_16_fat_msg, "FAT16", 5))
	{
		up->u_fat_bits = 16;
	}
	elif (memeq (sp->s_16_fat_msg, "FAT12", 5))
	{
		up->u_fat_bits = 12;
	}
	else
	{
		up->u_fat_bits = 0;	/* Será decidido pelo número de CLUSTERs */
	}

	if ((up->u_sectors_per_cluster = sp->s_sectors_per_cluster) <= 0)
	{
		printf
		(	"%s: No. de setores por CLUSTER (%d) "
			"ainda não suportado\n",
			cmd_nm, up->u_sectors_per_cluster
		);
		return (-1);
	}

	if ((up->u_bytes_per_sector = GET_SHORT (sp->s_bytes_per_sector)) != BLSZ)
	{
		printf
		(	"%s: Tamanho de setor (%d) ainda não suportado\n",
			cmd_nm, up->u_bytes_per_sector
		);
		return (-1);
	}

	up->u_bytes_per_cluster = up->u_bytes_per_sector * up->u_sectors_per_cluster;

	if ((unsigned)(up->u_n_fats = sp->s_n_fats) > 2)
	{
		printf
		(	"%s: No. de FATs (%d) inválido\n",
			cmd_nm, up->u_n_fats
		);
		return (-1);
	}

	up->u_fat1_blkno = GET_SHORT (sp->s_fat_offset);

	up->u_sectors_per_track = GET_SHORT (sp->s_sectors_per_track);

	up->u_n_heads = GET_SHORT (sp->s_n_heads);

	/*
	 *	Analisa detalhes diferentes para várias FATs
	 */
	if (up->u_fat_bits == 32) 	/* FAT de 32 bits */
	{
		if
		(	GET_SHORT (sp->s_root_dir_entries) != 0 ||
			GET_SHORT (sp->s_little_size) != 0 ||
			GET_SHORT (sp->s_little_sectors_per_fat) != 0 ||
			GET_SHORT (sp->s_version) != 0
		)
		{
			printf
			(	"%s: Superbloco de FAT32 inválido\n",
				cmd_nm
			);
			return (-1);
		}

		if (sp->s_32_signature != 0x29)
		{
			printf
			(	"%s: Assinatura \"29\" inválida do superbloco DOS\n",
				cmd_nm
			);
			return (-1);
		}

		if (dev_type != DOS_FAT32_G && dev_type != DOS_FAT32_L)
		{
			printf
			(	"%s: CUIDADO - Tipo de partição (0x%02X) inválida para FAT32\n",
				cmd_nm, dev_type
			);
		}

		up->u_sectors_per_fat	= GET_LONG (sp->s_big_sectors_per_fat);

	   /***	up->u_fat1_blkno	= GET_SHORT (sp->s_fat_offset); ***/
		up->u_end_fat1_blkno	= up->u_fat1_blkno + up->u_sectors_per_fat;

		up->u_fat2_blkno	= up->u_end_fat1_blkno;
		up->u_end_fat2_blkno	= up->u_fat2_blkno;

		if (up->u_n_fats > 1)
			up->u_end_fat2_blkno += up->u_sectors_per_fat;

		up->u_root_dir_entries	= 0;
		up->u_root_dir_blkno	= up->u_end_fat2_blkno;
		up->u_end_root_dir_blkno = up->u_end_fat2_blkno;

		up->u_root_cluster	= GET_LONG (sp->s_root_cluster);
		up->u_cluster_0_blkno	= up->u_end_fat2_blkno - 2 * up->u_sectors_per_cluster;
		up->u_n_clusters	= (up->u_end_fs_blkno - up->u_end_fat2_blkno) / up->u_sectors_per_cluster;
	}
	else				/* FAT de 16/12 bits */
	{
		if (up->u_fat_bits == 16 && sp->s_16_signature != 0x29)
		{
			printf
			(	"%s: Assinatura \"29\" inválida do superbloco DOS\n",
				cmd_nm
			);
			return (-1);
		}

		if (dev_type == DOS_FAT32_G || dev_type == DOS_FAT32_L)
		{
			printf
			(	"%s: CUIDADO - Tipo de partição (0x%02X) inválida para FAT12/FAT16\n",
				cmd_nm, dev_type
			);
		}

		if
		(	GET_SHORT (sp->s_root_dir_entries) == 0 ||
			GET_SHORT (sp->s_little_sectors_per_fat) == 0
		)
		{
			printf
			(	"%s: Superbloco de FAT16/FAT12 inválido\n",
				cmd_nm
			);
			return (-1);
		}

		up->u_sectors_per_fat	= GET_SHORT (sp->s_little_sectors_per_fat);

	   /***	up->u_fat1_blkno	= GET_SHORT (sp->s_fat_offset); ***/
		up->u_end_fat1_blkno	= up->u_fat1_blkno + up->u_sectors_per_fat;

		up->u_fat2_blkno	= up->u_end_fat1_blkno;
		up->u_end_fat2_blkno	= up->u_fat2_blkno;

		if (up->u_n_fats > 1)
			up->u_end_fat2_blkno += up->u_sectors_per_fat;

		up->u_root_dir_entries	= GET_SHORT (sp->s_root_dir_entries);
		up->u_root_dir_blkno	= up->u_end_fat2_blkno;

		up->u_end_root_dir_blkno = up->u_root_dir_blkno +
				up->u_root_dir_entries * sizeof (DOSDIR) / up->u_bytes_per_sector;

		up->u_root_cluster	= 0;
		up->u_cluster_0_blkno	= up->u_end_root_dir_blkno - 2 * up->u_sectors_per_cluster;
		up->u_n_clusters	= (up->u_end_fs_blkno - up->u_end_root_dir_blkno) / up->u_sectors_per_cluster;
	}

	cwd_cluster = up->u_root_cluster;

	/*
	 *	Distingue entre FAT16 e FAT12
	 */
	if (up->u_fat_bits == 0)
	{
		if (up->u_n_clusters + 2 <= 0xFF1)
			up->u_fat_bits = 12;
		else
			up->u_fat_bits = 16;
	}

#if (0)	/*******************************************************/
	if (up->u_fat_bits == 0)
	{
		printf
		(	"%s: Não consegui descobrir o tipo de FAT (FAT32, FAT16 ou FAT12)\n",
			cmd_nm
		);
		return (-1);
	}
#endif	/*******************************************************/

	switch (up->u_fat_bits)
	{
	    case 32: 			/* FAT de 32 bits */
		BAD_BEGIN = 0x0FFFFFF1;
		EOF_BEGIN = 0x0FFFFFF8;
		STD_EOF   = 0x0FFFFFFF;

		up->u_sb_backup = GET_SHORT (sp->s_sb_backup);
		up->u_fs_info	= GET_SHORT (sp->s_fs_info);
		break;

	    case 16: 			/* FAT de 16 bits */
		BAD_BEGIN = 0xFFF1;
		EOF_BEGIN = 0xFFF8;
		STD_EOF   = 0xFFFF;

		break;

	    case 12: 			/* FAT de 12 bits */
		BAD_BEGIN = 0xFF1;
		EOF_BEGIN = 0xFF8;
		STD_EOF   = 0xFFF;
		break;
	}

	/*
	 *	Verifica se há um bloco de informações da FAT32
	 */
	if (up->u_fs_info != 0)
	{
		FSINFO		fsinfo;

		if (sizeof (FSINFO) != 2 * BLSZ)
			error ("$Tamanho de FSINFO não correto");

		bread (up->u_fs_info,	  &fsinfo);
	   /***	bread (up->u_fs_info + 1, (void *)&fsinfo + BLSZ); ***/

		if
		(	memeq (&fsinfo.fs_isig1, "RRaA", 4) &&
			memeq (&fsinfo.fs_isig2, "rrAa", 4) &&
			memeq (&fsinfo.fs_isig3, "\0\0\x55\xAA", 4)
		/*** && memeq (&fsinfo.fs_isig4, "\0\0\x55\xAA", 4) ***/
		)
		{
			up->u_disk_infree   =
			up->u_infree	    = GET_LONG (&fsinfo.fs_infree);
			up->u_disk_inxtfree =
			up->u_inxtfree	    = GET_LONG (&fsinfo.fs_inxtfree);
#ifdef	DEBUG
			printf
			(	"FSINFO: totalfree = %d, nextfree = %d\n",
				up->u_infree, up->u_inxtfree
			);
#endif	DEBUG
		}
		else
		{
			up->u_fs_info = 0;
		}
	}

	return (0);

}	/* end get_uni_block */

/*
 ****************************************************************
 *	Procura o nome do volume				*
 ****************************************************************
 */
int
vol_search (const DOSSTAT *zp)
{
	if (!Z_ISVOL (zp->z_mode))
		return (0);

	strcpy (vol_nm, zp->z_lfn_nm);

	return (-1);	/* Termina a busca */

}	/* end vol_search */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mount_help (void)
{
	fprintf
	(	stderr,
		"%s - monta um dispositivo com um sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-%s] [<dispositivo>]\n",
		cmd_nm, cmd_nm, num_dev_list
	);

	fprintf
	(	stderr,
		"\nOpções:%s",
		num_dev_option
	);

}	/* end do_mount_help */
