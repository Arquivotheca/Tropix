/*
 ****************************************************************
 *								*
 *			clr.c					*
 *								*
 *	Limpa a tela do terminal				*
 *								*
 *	Versão	3.0.0, de 11.05.93				*
 *		3.0.1, de 10.04.97				*
 *								*
 *	Módulo: clr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <terminfo.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 10.04.97";

entry int	vflag;		/* Verbose */

/*
 ****** Dados de "terminfo" *************************************
 */
entry INFO	info;		/* Informações sobre o Terminal */

entry char	*c_clear;	/* Cadeia relevante de "info" */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Limpa a tela do terminal				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;

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

	/*
	 *	Pequena consistência
	 */
	argv += optind;
	argc -= optind;

	if (argc != 0)
		help ();

	/*
	 *	Obtem as características do terminal
	 */
	if (getinfo (NOSTR, &info) == 0)
		exit (1);

	/*
	 *	Obtém ponteiros para as cadeias relevantes de "info"
	 */
	c_clear = info.i_strings[s_clear];

	fputs (c_clear, stdout);

   /*** fflush (stdout) ***/

	return (0);

}	/* end clr */

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
		"%s - limpa a tela do terminal\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
