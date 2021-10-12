/*
 ****************************************************************
 *								*
 *			do_waste.c				*
 *								*
 *  Calcula o espaço desperdiçado no sistema de arquivos DOS	*
 *								*
 *	Versão	3.0.0, de 09.06.96				*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
#define BYRNDKB(x)	(((unsigned)(x) + KBSZ/2) >> KBSHIFT)

entry long	waste_bytes_used;	/* No. de bytes realmente usados */
entry long	waste_bytes_allocated;	/* No. de bytes alocados */

/*
 ******	Protótipos de funções ***********************************
 */
int	waste_function (const char *, DOSSTAT *, int);
void	do_waste_help (void);

/*
 ****************************************************************
 *	Percorre uma árvore DOS					*
 ****************************************************************
 */
void
do_waste (int argc, const char *argv[])
{
	int			opt;
	long			waste_bytes_wasted;

	/*
	 *	Pequena inicialização
	 */
	waste_bytes_used = 0; waste_bytes_allocated = 0;

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
			do_waste_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_waste_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_waste_help (); return; }

	/*
	 *	Percorre todo o sistema de arquivos
	 */
	get_whole_fat ();

	dos_ftw ("/", waste_function);

	waste_bytes_wasted = waste_bytes_allocated - waste_bytes_used;

	/*
	 *	Imprime o cabeçalho
	 */
	printf
	(	"DISPOSITIVO         "
		"ALOCADOS  USADOS  DESPER. (KB)  %%\n"
	);

	/*
	 *	Imprime o resultado
	 */
	printf ("%-20.20s", dev_nm);
	printf
	(	" %7d %7d %7d     ",
		BYRNDKB (waste_bytes_allocated),
		BYRNDKB (waste_bytes_used),
		BYRNDKB (waste_bytes_wasted)
	);

	printf
	(	" %s %%\n",
		waste_bytes_allocated == 0 ? 0 :
		editpercent (getpercent (waste_bytes_wasted, waste_bytes_allocated))
	);


}	/* end do_waste */

/*
 ****************************************************************
 *	Função chamada por "ftw"				*
 ****************************************************************
 */
int
waste_function (const char *nm, DOSSTAT *zp, int status)
{
	long		bytes_used = 0, bytes_allocated = 0;
	int		dir_clusno;
	DOSFILE		f;
	DOSDIR		d;

	if (status == DOS_FTW_NS)
	{
		printf
		(	"%s: Não encontrei o arquivo \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return (0);
	}

	/*
	 *	O diretório RAIZ é uma excessão
	 */
	if (uni.u_fat_bits != 32 && streq (nm, "/"))
		return (0);

	/*
	 *	Calcula o tamanho usado
	 */
	if   (Z_ISREG (zp->z_mode))
	{
		bytes_used = GET_LONG (zp->z_size);
	}
	elif (Z_ISDIR (zp->z_mode))
	{
		/*
		 *	Se for um diretório, le procurando o seu final
		 */
		dir_clusno = zp->z_cluster;

		if (dos_open_by_clus (&f, dir_clusno, BIG_SZ) < 0)
			return (0);

		while (dos_read (&f, &d, sizeof (DOSDIR)) > 0)
		{
			if (d.d_name[0] == '\0')
				break;

			bytes_used += sizeof (DOSDIR);
		}
	}

	/*
	 *	Calcula o tamanho alocado
	 */
	bytes_allocated = ls_get_cluster_list_size (zp->z_cluster);

	if (bytes_used > bytes_allocated)
	{
		printf
		(	"waste: ERRO INTERNO em %s (%7d > %7d)\n",
			nm, bytes_used, bytes_allocated
		);
	}

	/*
	 *	Adiciona aos totais
	 */
	waste_bytes_used      += bytes_used;
	waste_bytes_allocated += bytes_allocated;

#ifdef	DEBUG
	printf
	(	"waste: %7d %7d %s\n",
		bytes_used,
		ls_get_cluster_list_size (zp->z_cluster)),
		nm
	);
#endif	DEBUG

	return (0);

}	/* end waste_function */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_waste_help (void)
{
	fprintf
	(	stderr,
		"%s - calcula o espaço desperdiçado no sistema de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

}	/* end do_waste_help */
