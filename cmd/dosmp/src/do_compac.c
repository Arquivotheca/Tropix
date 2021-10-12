/*
 ****************************************************************
 *								*
 *			do_compac.c				*
 *								*
 *	Desfragmenta os blocos que "sobraram"			*
 *								*
 *	Versão	3.1.8, de 29.04.99				*
 *		3.2.1, de 11.07.99				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Tabela de dispositivos numéricos ************************
 */
entry int	compac_search_cluster;
entry DOSSTAT	compac_z_copy;
entry char 	compac_dir_found;

/*
 ******	Protótipos de funções ***********************************
 */
int		do_compac_one_file (void);
int		compac_dir_function (const char *, DOSSTAT *, int);
void		do_compac_help (void);

/*
 ****************************************************************
 *	Altera o tamanho de um sistema de arquivos DOS		*
 ****************************************************************
 */
void
do_compac (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "vH")) >= 0)
	{
		switch (opt)
		{
		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_compac_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_compac_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_compac_help (); return; }

	/*
	 *	Procura o primeiro CLUSTER vago
	 */
	get_whole_fat ();

	/*
	 *	Compacta os diversos arquivos
	 */
	for (EVER)
	{
		if (do_compac_one_file () < 0)
			break;
	}

}	/* end do_compac */

/*
 ****************************************************************
 *	Compata um arquivo					*
 ****************************************************************
 */
int
do_compac_one_file (void)
{
	UNI		*up = &uni;
	DOSSTAT		*zp = &compac_z_copy;
	int		index, n, first_free, last_used, last_index;
	int		next_index, first_cluster;
	char		*area;

	/*
	 *	Calcula o coeficiente de compacticidade
	 */
	n = 0; last_used = 0; first_free = 0;

	for (index = 2; index < up->u_n_clusters + 2; index++)
	{
		if  (!C_ISFREE (get_fat_value (index)))
			{ n++; last_used = index; }
		elif (first_free == 0)
			first_free = index;

	}	/* end for (procurando do final para o início) */

#undef	DEBUG
#ifdef	DEBUG
	printf ("n = %d, last_used = %d, first_free = %d\n", n, last_used, first_free);
#endif	DEBUG

	if (last_used == 0)
		{ printf ("Não há CLUSTERs ocupados\n"); return (-1); }

	printf
	(	"Compacticidade = %s %%",
		editpercent (getpercent (n + 2, last_used + 1))
	);

	printf
	(
		", Espaço livre ao final = %s\n",
		cv_blocks_to_MB ((up->u_n_clusters + 2 - 1 - last_used) * up->u_sectors_per_cluster)
	);


	if (n + 2 == last_used + 1)
		return (-1);

	if (first_free == 0)
		{ printf ("Não há CLUSTERs vagos\n"); return (-1); }

	/*
	 ****** Compacta o sistema de arquivos ******************
	 */
	if (fs_status == FS_MOUNT_RO)
	{
		printf
		(	"\n%s: O sistema de arquivos \"%s\" "
			"está montado somente para leituras\n",
			cmd_nm, dev_nm
		);
		return (-1);
	}

	/*
	 *	Começa a procurar o arquivos com CLUSTERs no final
	 */
	for (index = up->u_n_clusters + 2 - 1; /* abaixo */; index--)
	{
		if (index <= first_free)
		{
			fprintf (stderr, "Não achei CLUSTERs após o primeiro livre\n");
			return (-1);
		}

		if (!C_ISFREE (get_fat_value (index)))
			{ last_index = index; break; }
	}

	/*
	 *	Procura o começo da cadeia
	 */
	for (EVER)
	{
		char		found = 0;

		for (index = up->u_n_clusters + 2 - 1; index >= 2; index--)
		{
			if (get_fat_value (index) == last_index)
				{ last_index = index; found++; break; }
		}

		if (!found)
			break;
	}

	/*
	 *	Agora, acha o nome do arquivo
	 */
	compac_search_cluster = index = last_index;
	compac_dir_found = 0;

	dos_ftw ("/", compac_dir_function);

	if (!compac_dir_found)
	{
		fprintf (stderr, "Não achei a entrada de DIR correspondente\n");
		return (0);
	}

	if (askyesno () <= 0)
		return (-1);

	/*
	 *	Reloca o primeiro CLUSTER, e muda a entrada do DIR
	 */
	area = alloca (CLUSZ);
	clus_read  (index, area, CLUSZ);
	clus_write (first_free, area, CLUSZ);

#ifdef	DEBUG
	printf ("%d => %d\n", index, first_free);
#endif	DEBUG
	if (cmd_vflag)
	{
		printf ("\rRelocando CLUSTER %8d => %8d ", index, first_free);
		fflush (stdout);
	}

	/*
	 *	Acerta o DIR e a FAT
	 */
	first_cluster = first_free;

	PUT_CLUSTER (first_cluster, zp); dos_dir_write (zp);

	next_index = get_fat_value (index);

	put_fat_value (next_index, first_free);
	put_fat_value (C_FREE, index);

	/*
	 *	Reloca os CLUSTERs restantes
	 */
	for
	(	last_index = first_free, index = next_index;
		!C_ISEOF (index);
		/* abaixo */
	)
	{
		for (first_free = 2; /* abaixo */; first_free++)
		{
			if (first_free >= up->u_n_clusters + 2)
			{
				printf ("\rNão há mais CLUSTERs livres\n");
				return (-1);
			}

			if (C_ISFREE (get_fat_value (first_free)))
				break;
		}

		/* Não vamos relocar para cima!!! */

		if (first_free >= index)
		{
#ifdef	DEBUG
			printf ("\rIria relocar o CLUSTER para cima (%d => %d)\n", index, first_free);
#endif	DEBUG
			last_index = index, index = get_fat_value (index);
			continue;
		}

	   /***	area = alloca (CLUSZ); ***/
		clus_read  (index, area, CLUSZ);
		clus_write (first_free, area, CLUSZ);

#ifdef	DEBUG
		printf ("%d => %d\n", index, first_free);
#endif	DEBUG
		if (cmd_vflag)
		{
			printf ("\rRelocando CLUSTER %8d => %8d ", index, first_free);
			fflush (stdout);
		}

		/*
		 *	Acerta a FAT
		 */
		next_index = get_fat_value (index);
	
		put_fat_value (first_free, last_index);
		put_fat_value (next_index, first_free);

		put_fat_value (C_FREE, index);

		last_index = first_free; index = next_index;

	}	/* end (percorrendo os CLUSTERs do arquivo) */

	if (cmd_vflag)
		printf ("\n");

	/*
	 *	Se for um diretório, muda o CLUSTER do "."
	 */
	if (!Z_ISDIR (zp->z_mode))
		return (0);

	if (dos_fstat (".", first_cluster, zp) < 0)
	{
		printf ("Não encontrei o \".\" do diretório\n");
		return (0);
	}

	PUT_CLUSTER (first_cluster, zp); dos_dir_write (zp);

	return (0);

}	/* end do_compac_one_file */

/*
 ****************************************************************
 *	Procura o ponteiro para o primeiro CLUSTER		*
 ****************************************************************
 */
int
compac_dir_function (const char *nm, DOSSTAT *zp, int type)
{
	if (zp->z_cluster != compac_search_cluster)
		return (0);

	fprintf
	(	stderr,
		"(%c, %d) %s? (n): ",
		file_type_edit (zp->z_mode), GET_LONG (zp->z_size), nm
	);

	memmove (&compac_z_copy, zp, sizeof (DOSSTAT));

	compac_dir_found++;

	return (-1);

}	/* end compac_dir_function */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_compac_help (void)
{
	fprintf
	(	stderr,
		"%s - compacta o sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

}	/* end do_compac_help */
