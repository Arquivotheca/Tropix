/*
 ****************************************************************
 *								*
 *			ldshlib.c				*
 *								*
 *	Carrega/descarrega uma biblioteca compartilhada		*
 *								*
 *	Versão	3.2.3, de 02.12.99				*
 *		4.3.0, de 21.06.02				*
 *								*
 *	Módulo: ldshlib						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/region.h>
#include <sys/shlib.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.3.0, de 21.06.02";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;			/* Verbose */
entry int	sflag;			/* Silencioso */

/*
 ****** Protótipos de funções ***********************************
 */
void		print_shlib_tb (void);
void		help (void);

/*
 ****************************************************************
 *	Carrega/descarrega uma biblioteca compartilhada		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, index;
	const char	*str, *lib_nm, *unload_str = NOSTR;
	char		long_lib_nm[64];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "u:svH")) != EOF)
	{
		switch (opt)
		{
		    case 'u':			/* Descarrega */
			unload_str = optarg;
			break;

		    case 's':			/* Silencioso */
			sflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se é para descarregar
	 */
	if (unload_str != NOSTR)
	{
		index = strtol (unload_str, &str, 0);

		if (*str != '\0')
			error ("$Índice \"%s\" inválido", unload_str);

		if (shlib (SHLIB_UNLOAD, index) < 0)
			error ("$*Não consegui descarregar a biblioteca %d", index);

		return (0);
	}

	/*
	 *	Imprime a tabela das bibliotecas carregadas
	 */
	if ((lib_nm = argv[0]) == NOSTR)
		{ print_shlib_tb (); return (0); }

	/*
	 *	Compõe o nome da biblioteca
	 */
	if (lib_nm[0] == '-' && lib_nm[1] == 'l')
	{
		strcpy (long_lib_nm, "/usr/lib/lib");
		strncat (long_lib_nm, lib_nm + 2, sizeof (long_lib_nm));

		if   (access (long_lib_nm + 4, F_OK) >= 0)
		{
			/* "/lib/lib..." existe */

			lib_nm = long_lib_nm + 4;
		}
		elif (access (long_lib_nm, F_OK) >= 0)
		{
			/* "/usr/lib/lib..." existe */

			lib_nm = long_lib_nm;
		}
		else
		{
			/* Tentando "/usr/xwin/lib/lib..." */

			strcpy (long_lib_nm, "/usr/xwin/lib/lib");
			strncat (long_lib_nm, lib_nm + 2, sizeof (long_lib_nm));
			lib_nm = long_lib_nm;
		}
	}

	if (shlib (SHLIB_LOAD, lib_nm) < 0)
	{
		if (sflag && errno == EBUSY)
			return (-1);

		error ("$*Não consegui carregar a biblioteca \"%s\"", lib_nm);

		return (-1);
	}

	return (0);

}	/* end ldshlib */

/*
 ****************************************************************
 *	Imprime dados sobre as bibliotecas carregadas		*
 ****************************************************************
 */
void
print_shlib_tb (void)
{
	SHLIB		local_shlib_tb[NSHLIB];
	const SHLIB	*shp = local_shlib_tb;
	int		index;

	if (shlib (SHLIB_TABLE, local_shlib_tb) < 0)
		error ("$*Não consegui obter a tabela de bibliotecas compartilhadas");

	printf
	(	"I  NOME           --------- DATA --------- DEP HSEQ"
	);

	if (vflag)
		printf ("   TEXT VA    DATA VA    VA MASK");
	else
		printf ("   TEXT   DATA    BSS  SYMTB  NSYM NTENT");

	printf ("\n\n");

	for (index = 0; index < NSHLIB; index++, shp++)
	{
		if (shp->sh_name[0] == '\0')
			continue;

		printf
		(	"%d  %-14.14s %-24.24s  %02X %3d",
			index, shp->sh_name, btime (&shp->sh_time),
			shp->sh_dep_mask, shp->sh_hash_seq
		);

		if (vflag)
		{
			printf
			(	"   %P  %P  %P",
				PGTOBY (shp->sh_tvaddr), PGTOBY (shp->sh_dvaddr),
				shp->sh_vaddr_mask
			);
		}
		else
		{
			printf
			(	" %7d %6d %6d %6d  %4d  %4.2f",
				shp->sh_tsize, shp->sh_dsize, shp->sh_bsize, shp->sh_ssize,
				shp->sh_sym_count,
				(double)(shp->sh_sym_count + shp->sh_hash_col) / shp->sh_sym_count
			);
		}

		printf ("\n");
	}

}	/* end print_shlib_tb */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - carrega/descarrega uma biblioteca compartilhada\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-sv] [-u <índice>] [<biblioteca>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-u: Descarrega a biblioteca com o <índice> dado\n"
		"\t-s: Não imprime mensagem de erro caso a biblioteca já esteja carregada\n"
		"\t-v: Verboso\n"
		"\nObs.:\tA biblioteca pode ser dada na forma abreviada \"-l...\"\n"
	);

	exit (2);

}	/* end help */
