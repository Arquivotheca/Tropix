/*
 ****************************************************************
 *								*
 *			do_du.c					*
 *								*
 *	Informa sobre o uso de disco no DOS			*
 *								*
 *	Versão	3.0.0, de 03.06.96				*
 *		3.0.0, de 09.06.96				*
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
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições e variáveis globais				*
 ****************************************************************
 */
#define	BLTOKB_INT(x)	((unsigned)(x) >> (KBSHIFT-BLSHIFT))
#define	BLTOKB_DEC(x)	(((x) & (KBSZ/BLSZ-1)) * (100 / (KBSZ/BLSZ)))

entry long	TOTAL;		/* Total geral */

const char	du_fmt[]   = "%9s  %s\n";
const char	du_title[] = "----KB---  ------NOME------\n";

/*
 ******	Item de arquivo (diretório ou não) **********************
 */
#define	NO_ITEM (ITEM *)NULL

typedef struct	item	ITEM;

struct	item
{
#define	i_nm	i_z.z_lfn_nm	/* Nome (somente um nome) */
	char	*i_path;	/* Caminho (nem sempre disponível) */
	DOSSTAT	i_z;		/* Estado (Diretório) */
	ITEM	*i_next;	/* Ponteiro para o próximo */
};

/*
 ******	Protótipos de funções ***********************************
 */
void		proc_file (const char *);
long		file_analysis (const char *, int);
char		*edit_BL_to_KB (int);
void		do_du_help (void);

/*
 ****************************************************************
 *	Informa sobre o uso de disco DOS			*
 ****************************************************************
 */
void
do_du (int argc, const char *argv[])
{
	int		opt, files = 0;

	/*
	 *	Pequena inicialização
	 */
	TOTAL = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "savdNH")) > 0)
	{
		switch (opt)
		{
		    case 's':			/* Somente total */
			if (cmd_aflag)
				{ do_du_help (); return; }
			cmd_sflag++;
			break;

		    case 'a':			/* Para cada arquivo */
			if (cmd_sflag)
				{ do_du_help (); return; }
			cmd_aflag++;
			break;

		    case 'v':			/* Verbose */
			cmd_vflag++;
			break;

		    case 'd':			/* Debug */
			cmd_dflag++;
			break;

		    case 'H':			/* Help */
			do_du_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_du_help ();
			return;

		}	/* end switch */

	}	/* end while */

	/*
	 *	Pequena consistência
	 */
	argv += cmd_optind;

	/*
	 *	Põe o cabeçalho
	 */
	printf (du_title);

	/*
	 *	Analisa as <árvores>
	 */
	if (*argv == NOSTR)
	{
		/*
		 *	Não foram dadas <árvores>
		 */
		proc_file ("."); files++;
	}
	else
	{
		/*
		 *	Foram dadas <árvores>
		 */
		for (/* vazio */; *argv; argv++)
		{
			if (received_intr)
				break;

			proc_file (*argv); files++;
		}
	}

	/*
	 *	Se foram dados mais de uma <árvore>, dá o total geral
	 */
	if (received_intr)
		return;

	if (files > 1)
		printf (du_fmt, edit_BL_to_KB (TOTAL), "TOTAL");

}	/* end du */

/*
 ****************************************************************
 *	Analisa uma <árvore>					*
 ****************************************************************
 */
void
proc_file (const char *path)
{
	long		total;

	total = file_analysis (path, 1 /* Root */);

	printf (du_fmt, edit_BL_to_KB (total), path);

	TOTAL += total;

}	/* end proc_file */

/*
 ****************************************************************
 *	Calcula o total de uma <árvore>				*
 ****************************************************************
 */
long
file_analysis (const char *path, int root)
{
	long		dir_total = 0;
	DOSSTAT		z;
	char		*nm;
	ITEM		*ip, *lp;
	int		dir_clusno;
	int		path_len = strlen (path);
	int		len, max_len = 0, z_BL_size, dir_size = 0;
	char		*cp_point;
	ITEM		file_list;
	DOSFILE		f;

	if (cmd_dflag)
		fprintf (stderr, "file_analysis (%s, %d)\n", path, root);

	/*
	 *	Verifica se veio interrupção
	 */
	if (received_intr)
		return (0);

	/*
	 *	Inicialmente, obtém o estado do arquivo
	 */
	if (dos_stat (path, &z) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, path, strerror (errno)
		);
		return (0);
	}

	z_BL_size = BYTOBL (GET_LONG (z.z_size));

	if (!Z_ISDIR (z.z_mode))
	{
		if (Z_ISREG (z.z_mode))
		{
			dir_total = z_BL_size;

			if (cmd_aflag && !root)
				printf (du_fmt, edit_BL_to_KB (dir_total), path);
		}

		return (dir_total);
	}

	/*
	 *	Lê o diretório
	 */
	dir_clusno = z.z_cluster;

	if (dos_open_by_clus (&f, dir_clusno, BIG_SZ) < 0)
		return (0);

	file_list.i_next = NO_ITEM;

	ip = alloca (sizeof (ITEM));

	dos_lfn_init (&ip->i_z); ip->i_z.z_lfn_entries = 0;

	while (dos_read (&f, &ip->i_z.z_d, sizeof (DOSDIR)) > 0)
	{
		if (ip->i_z.z_name[0] == '\0')
			break;

		dir_size += sizeof (DOSDIR);

		if (ip->i_z.z_name[0] == Z_EMPTY)
		{
			dos_lfn_reset (&ip->i_z);
			ip->i_z.z_lfn_entries = 0;
			continue;
		}

		if (ip->i_z.z_lfn_entries++ == 0)
		{
			ip->i_z.z_lfn_clusno    = f.f_clusno;
			ip->i_z.z_lfn_blkno     = f.f_blkno;
			ip->i_z.z_lfn_end_blkno = f.f_end_blkno;
			ip->i_z.z_lfn_offset    = f.f_ptr - f.f_area - sizeof (DOSDIR);
		}

		if (Z_ISLFN (ip->i_z.z_mode))
			{ dos_lfn_add (&ip->i_z); continue; }

		dos_lfn_get_nm (&ip->i_z);

		if (streq (ip->i_z.z_lfn_nm, ".") || streq (ip->i_z.z_lfn_nm, ".."))
		{
			dos_lfn_reset (&ip->i_z);
			ip->i_z.z_lfn_entries = 0;
			continue;
		}

		/*
		 *	Prepara o item.	Calcula o "offset"
		 *	(particular para DIRs)
		 */
		GET_CLUSTER (&ip->i_z);
	   	ip->i_z.z_blkno  = f.f_blkno;
	   	ip->i_z.z_offset = f.f_ptr - f.f_area - sizeof (DOSDIR);

	   /***	strcpy (ip->i_nm, ip->i_z.z_lfn_nm); ***/
	   /***	ip->i_path = ... ***/	/* Mais tarde */
	   /***	ip->i_z    = ... ***/	/* Já foi feito */
	   /***	ip->i_next = ...	/* Abaixo  */

		/*
		 *	Insere na lista
		 */
		for (lp = &file_list; lp->i_next != NO_ITEM; lp = lp->i_next)
		{
			if (strcmp (ip->i_z.z_lfn_nm, lp->i_next->i_nm) <= 0)
				break;
		}

		len = strlen (ip->i_z.z_lfn_nm);

		if (len > max_len)
			max_len = len;

		ip->i_next = lp->i_next;
		lp->i_next = ip;

		ip = alloca (sizeof (ITEM));	/* Já aloca um seguinte */

		dos_lfn_init (&ip->i_z); ip->i_z.z_lfn_entries = 0;

	}	/* end lendo o diretório */

	/*
	 *	Prepara uma área para o caminho (completo) novo
	 */
	nm = alloca (path_len + max_len + 2);

	if (path[0] == '\0' || path[0] == '.' && path[1] == '\0')
	{
		cp_point = nm;
	}
	else
	{
		strcpy (nm, path); cp_point = nm + path_len;

		if (cp_point[-1] != '/')
			*cp_point++ = '/';
	}

	/*
	 *	Percorre a lista, processando os arquivos e diretórios
	 */
	for (ip = file_list.i_next; ip != NO_ITEM; ip = ip->i_next)
	{
		if (received_intr)
			return (0);

		strcpy (cp_point, ip->i_nm);

		dir_total += file_analysis (nm, 0 /* Not root */);

	}	/* Processando arquivos regulares */

	/*
	 *	Terminou de processar o diretório
	 *	Não esquecer de contar o tamanho do próprio diretório
	 */
	z_BL_size = BYTOBL (dir_size); dir_total += z_BL_size;

#ifdef	DEBUG
	printf ("du: %s = %d\n", path, z_BL_size);
#endif	DEBUG

	if (!cmd_sflag && !root)
		printf (du_fmt, edit_BL_to_KB (dir_total), path);

	return (dir_total);

}	/* end file_analysis */

/*
 ****************************************************************
 *	Prepara o texto com o no. de KBs			*
 ****************************************************************
 */
char *
edit_BL_to_KB (int blocos)
{
	static char	area[16];

	sprintf (area, "%d.%02d", BLTOKB_INT (blocos), BLTOKB_DEC (blocos));

	return (area);

}	/* end edit_BL_to_KB */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_du_help (void)
{
	fprintf
	(	stderr,
		"%s - informa sobre o uso de disco DOS\n"
		"\nSintaxe:\n"
		"\t%s [-sa] [<árvore> ...]\n",
		cmd_nm, cmd_nm
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-s: Somente o total de cada <árvore>\n"
		"\t-a: Informação para cada arquivo\n"
		"\nObs.:\tAs opções \"-a\" e \"-s\" são mutuamente exclusivas.\n"
		"\tUma <árvore> pode também ser um arquivo normal.\n"
	);

}	/* end help */
