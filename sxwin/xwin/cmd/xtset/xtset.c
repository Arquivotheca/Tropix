/*
 ****************************************************************
 *								*
 *			xtset.c					*
 *								*
 *	Altera cores em uma janela "xterm"			*
 *								*
 *	Versão	3.1.6, de 12.02.99				*
 *		4.4.0, de 19.03.03				*
 *								*
 *	Módulo: cmd/xtset					*
 *		XWindows - Utilitários				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 4.4.0, de 19.03.03";

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
#define	BEL		7
#define	ESC		27

entry	const char	*backcolor,
			*forecolor,
#if (0)	/*******************************************************/
			*cursorcolor,
#endif	/*******************************************************/
			*fontname;
/*
 ****************************************************************
 *	Protótipos de Funções Locais				*
 ****************************************************************
 */
static	void		help (int);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt, exitval;
	const char	*geometry = NOSTR;

	/*
	 *	Inicializa opções.
	 */
	exitval = 0;

	/*
	 *	Analisa as opções de execução.
	 */
	while ((opt = getopt (argc, argv, "Hb:f:F:g:")) != EOF)
	{
		switch (opt)
		{
		    case 'H':
			help (0);
			break;

		    case 'b':
			backcolor = optarg;
			break;

		    case 'f':
			forecolor = optarg;
			break;

		    case 'F':
			fontname = optarg;
			break;

		    case 'g':
			geometry = optarg;
			break;

		    default:
			putc ('\n', stderr);
			help (2);

		}	/* end switch (opt) */

	}	/* while ((opt = getopt (...))) */

	argv += optind;
	argc -= optind;

	if (backcolor)
		printf ("\033]49;%s\007", backcolor);

	if (forecolor)
		printf ("\033]39;%s\007", forecolor);

	if (fontname)
		printf ("\033]50;%s\007", fontname);

	if (geometry)
		printf ("\033]46;%s\007", geometry);

/***	printf ("\033[2J\033[H");	***/

	exit (exitval);

}	/* end main */

/*
 ****************************************************************
 *	Resumo de Utilizacao do Programa			*
 ****************************************************************
 */
static void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - modifica características de uma janela XTERM\n\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-b <fundo>] [-f <texto>] [-F <fonte>] [-g <ncol>x<nlin>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-b: muda a cor de fundo\n"
		"\t-f: muda a cor do texto (e do cursor)\n"
		"\t-F: muda a fonte\n"
		"\t-g: muda o tamanho da janela\n"
	);

	exit (exitval);

}	/* end help */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Emite uma mensagem de erro				*
 ****************************************************************
 */
entry void
msg (char *format, ...)
{
	va_list	ap;
	int	doexit = 0, doperror = 0;

	va_start (ap, format);

	if (*format == '$')
		format++, doexit++;

	if (*format == '*')
		format++, doperror++;

	fprintf (stderr, "%s: ", pgname);
	vfprintf (stderr, format, ap);

	if (doperror && errno != NOERR)
		fprintf (stderr, " (%s)", strerror (errno));

	va_end (ap);

	putc ('\n', stderr);

	if (doexit)
		exit (1);

}	/* end msg */
#endif	/*******************************************************/
