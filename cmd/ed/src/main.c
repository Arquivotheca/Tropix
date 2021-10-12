/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Editor de textos orientado por linhas			*
 *								*
 *	Versão	1.0.0, de 24.10.86				*
 *		3.0.0, de 27.04.97				*
 *								*
 *	Módulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "../h/ed.h"
#include "../h/txtio.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0, de 26.04.97";

entry int	vflag;		/* Verbose */

/*
 ******	Variáveis globais ***************************************
 */
entry TEXT	Text;		/* Informações sobre o Texto em edição */
entry BASE	*Root;		/* Início da Lista com o Texto em edição */
entry BASE	*Current;	/* Posição Corrente */
entry AUTO	Search;		/* Expressão Regular p/ Busca */

entry bool	Interactive;		/* Edição no terminal */
entry bool	Interruption = false;	/* Houve uma interrupção */
entry char	temp_buffer[LSIZE];	/* Buffer temporario para uso geral */

/*
 ****** Protótipos de funções ***********************************
 */
void		on_signal (int, ...);
void		help (void);

/*
 ****************************************************************
 *	Editor de textos orientado por linhas			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Pequena inicialização
	 */
	signal (SIGINT, on_signal);
	Interactive = isatty (fileno (stdin));

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
	 *	Inicialização do Texto
	 */
	if (argv[0] != NULL)
		c_edit (argv[0]);
	else
		c_edit ("");

	Current = Root;

	for (EVER)
	{
		if (Interruption)
		{
			puts ("\nInterrupção");
			Interruption = false;
		}

		if (Interactive)
		{
			putchar (':');
			putchar (' ');
			fflush (stdout);
		}

		if (read_command ())
			command ();
		elif (!Interactive)
			c_quit ();
	}

}	/* end ed */

/*
 ****************************************************************
 *	Tratamento de Interrupções				*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	signal (SIGINT, on_signal);

	Interruption = true;

}	/* end on_signal */

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
		"%s - editor de textos orientado por linhas\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [<arquivo>]\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
