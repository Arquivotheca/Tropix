/*
 ****************************************************************
 *								*
 *			mknod.c					*
 *								*
 *	Cria um arquivo especial				*
 *								*
 *	Versão	1.0.0, de 09.06.86				*
 *		3.0.1, de 30.05.97				*
 *								*
 *	Módulo: mknod						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 30.05.97";

#define	elif	else if

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Cria um arquivo especial				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	const char	*str;
	long		modo = 0;
	unsigned	major, minor;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
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
	 *				     0	    1	    2	    3
	 *	Temos de ter 4 argumentos: <nome> <tipo> <major> <minor>
	 */
	if (argc != 4)
		help ();

	/*
	 *	Analisa o <tipo>
	 */
	if   (streq (argv[1], "b"))
	{
		modo = S_IFBLK | 0666;
	}
	elif (streq (argv[1], "c"))
	{
		modo = S_IFCHR | 0666;
	}
	else
	{
		fprintf
		(	stderr,
			"%s: Tipo de arquivo inválido: \"%s\"\n\n",
			pgname, argv[1]
		);
		help ();
	}

	/*
	 *	Analisa o <major>
	 */
	if ((major = strtol (argv[2], &str, 0)) > 255  ||  *str != '\0')
	{
		fprintf
		(	stderr,
			"%s: <major> de arquivo inválido: \"%s\"\n\n",
			pgname, argv[2]
		);
		help ();
	}

	/*
	 *	Analisa o <minor>
	 */
	if ((minor = strtol (argv[3], &str, 0)) > 255  ||  *str != '\0')
	{
		fprintf
		(	stderr,
			"%s: <minor> de arquivo inválido: \"%s\"\n\n",
			pgname, argv[3]
		);
		help ();
	}

	/*
	 *	Cria o arquivo
	 */
	if (mknod (argv[0], modo, (major << 8) | minor) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar \"%s\" (%s)\n",
			pgname, argv[0], strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end mknod */

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
		"%s - cria um arquivo especial\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s <nome> <tipo> <major> <minor>\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nObs.:\tO <tipo> deve ser \"b\" (blocos) ou \"c\" (caracteres)\n"
	);

	exit (2);

}	/* end help */
