/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Programa principal					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.4, de 05.02.98				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <curses.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/commands.h"
#include "../h/visual.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.4, de 05.02.98";

entry TEXT	*Text;		/* Texto em Edição */
entry COMM	*Comm;		/* Informações sobre o Texto em edição */
entry VIEW	*View;		/* Estado da Edição Visual */
entry char	*Search;	/* Autômato para Busca */
entry char	*Esearch;	/* Expressão regular para Busca */

entry bool	Interruption = false;	/* Houve uma interrupção */
entry char	*System;		/* Último comando para o Shell */
entry bool	Verbose = false;	/* */
entry bool	Mdebug = false;		/* */
entry bool	Debug = false;		/* */
entry const char *error_msg;		/* Mensagens de Erro */
entry char	temp_buffer[LSIZE];	/* Buffer temporário para uso geral */
entry char	Message[80];		/* Mensagem pendente */

/*
 ******	Protótipos de funções ***********************************
 */
static COMM	*newcomm (void);
static VIEW	*newview (void);
static TEXT	*newtext (void);
static void	on_signal (int, ...);

void		help (void);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		s;
	int		com;
	int		y, x;
	WINDOW		*ready;
	WINDOW		*bigwin, *smallwin;
	SIZE		size;
	const char	*text_nm;

	/*
	 *	Analise dos argumentos enviados ao programa
	 */
	while ((opt = getopt (argc, argv, "dmvBMH?")) != EOF)
	{
		switch (opt)
		{
		    case 'M':
		    case 'B':
			exit (0);

		    case 'd':
			Debug = true;
			setbuf (stderr, NOSTR);
			break;

		    case 'm':
			Mdebug = true;
			break;

		    case 'v':
			Verbose = true;
			break;

		    default:
			help ();

		}	/* end switch opt */

	}	/* end while opt != EOF */

	if (Mdebug)
		fprintf (stderr, "Testando MALLOC\n");

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se foi dado mais de um nome de arquivo
	 */
	text_nm = argv[0];

	if (argc > 1)
	{
		int	i;
		char		int_buf[8];

		for (EVER)
		{
			fprintf
			(	stderr,
				"\nForam dados os nomes de %d arquivos:\n\n",
				argc
			);

			for (i = 0; i < argc; i++)
				fprintf (stderr, "%2d: %s\n", i + 1, argv[i]);

			fprintf
			(	stderr,
				"\nIndique qual o desejado (1 a %d): ",
				argc
			);

			if ((i = atol (gets (int_buf))) > 0 && i <= argc)
			{
				text_nm = argv[i - 1];
				break;
			}

		}	/* end for (EVER) */

	}	/* end if (dado mais de um arquivo) */

	Text = newtext ();

	/*
	 *	Prepara os sinais
	 */
	signal (SIGINT,	 on_signal);
	signal (SIGQUIT, on_signal);
	signal (SIGTERM, on_signal);
	signal (SIGBUS,  on_signal);
	signal (SIGADR,  on_signal);
	signal (SIGSEGV, on_signal);

	/*
	 *	Leitura do texto novo
	 */
	if (text_nm != NOSTR)
	{
		Text->t_name = strdup (text_nm);

		if (Debug)
			fprintf (stderr, "Editando: %s\n", Text->t_name);

		switch (txt_read (Text->t_name, NOBASE, &size))
		{
		    case IO_OK:
			sprintf
			(	Message, "\"%s\": %d linhas, %d caracteres",
				Text->t_name, size.s_nl, size.s_nc
			);

			if (check (Text->t_flags, T_RDONLY))
				strcat (Message, " (só LEITURA)");

			if (dos_mode)
				strcat (Message, " (estilo DOS)");
			break;

		    case IO_NFOUND:
			txt_init ();

			sprintf
			(	Message, "\"%s\": arquivo novo",
				Text->t_name
			);
			break;

		    case IO_EMPTY:
			txt_init ();
			sprintf (Message, M_txtio, Text->t_name);
			break;

		    default:
			fprintf (stderr, "%s: ", pgname);
			fprintf (stderr, M_txtio, Text->t_name);
			putc ('\n', stderr);
			exit (1);

		}	/* switch txt_read */

	}	/* if text_nm */
	else
	{
		Text->t_name = strdup ("");
		txt_init ();
	}

	if (Debug)
	{
		fprintf (stderr, "Retornando de \"txt_read\"\n");
		getchar ();
	}

	/*
	 *	Iniciando operação com Curses
	 */
	if (incurses (pgname) == ERR)
	{
		fprintf
		(	stderr,
			"%s: Utilize o editor orientado a linhas \"ed\"\n",
			pgname
		);
#if (0)	/*******************************************************/
		fprintf
		(	stderr,
			"%s: Atribua à variável \"TERM\" o tipo de seu terminal\n",
			pgname
		);
#endif	/*******************************************************/

		exit (1);
	}

	/*
	 *	Programação da rotina de Switch
	 */
	setswitch (vis_switch);

	/*
	 *	x
	 */
	Comm = newcomm ();
	View = newview ();

	/*
	 *	Janelas
	 */
	ready = newwin (1, COLS, LINES - 2, 0);
	waddstr (ready, "Aperte uma tecla para voltar ao modo Visual");
	wrefset (ready, R_HOLD);
	wgetset (ready, G_IGXONXOFF);

	bigwin = stdwin;
	smallwin = newwin (3, COLS, LINES - 3, 0);
	wrefoff (bigwin, R_CURSOR);
	wrefoff (smallwin, R_CURSOR);

	/*
	 *	Linha corrente inicial
	 */
	Comm->c_cur = FIRST ();
	View->v_base = FIRST ();

	/*
	 *	Processo de edição
	 */
	for (EVER)
	{
		s = Visual ();

		if (s < VIS_QUIT)
			break;

		reset (Text->t_flags, T_EDITION);
		Comm->c_cur = View->v_base;

		if (s == VIS_STAY)
		{
			mvwin (ready, LINES - 1, 0);
			s = Commands (bigwin, true);
			touchwin (View->v_win);
		}
		elif (s == VIS_QUICK)
		{
		    again:
			wgetyx (View->v_win, y, x);

			if (y >= LINES - 3)
			{
				mvwin (smallwin, y - 3, 0);
				mvwin (ready, y - 1, 0);
			}
			else
			{
				mvwin (smallwin, LINES - 3, 0);
				mvwin (ready, LINES - 1, 0);
			}

			s = Commands (smallwin, false);
		}

		if (s == COM_QUIT)
			break;

		/*
		 *	Atualização da Janela Visual
		 */
		if (check (Text->t_flags, T_NEW))
		{
			pg_display (Comm->c_cur);
			reset (Text->t_flags, T_NEW|T_EDITION);
		}
		elif (check (Text->t_flags, T_EDITION))
		{
			pg_update ();
			pg_move (Comm->c_cur,
					str_nonwhite (Comm->c_cur->ch), true);
			reset (Text->t_flags, T_EDITION);
		}

		wrefresh (View->v_win);

		/*
		 *	Tecle algo
		 */
		if ((com = wgetch (ready)) == ':')
		{
			untouchwin (ready);
			goto again;
		}
		else
		{
			vis_ungetch (com);
		}
	}

	outcurses (); return (0);

}	/* end main */

/*
 ****************************************************************
 *	Criação de uma Estrutura COMM				*
 ****************************************************************
 */
static COMM *
newcomm (void)
{
	COMM		*commp;

	if ((commp = malloc (sizeof (COMM))) == NOCOMM)
		return (NOCOMM);

	commp->c_cur = NOBASE;
	commp->c_search = NOSTR;
	commp->c_ssearch = NOSTR;
	commp->c_subst = NOSTR;
	commp->c_repl = (REPL *)malloc (sizeof (REPL));

	return (commp);

}	/* end newcomm */

/*
 ****************************************************************
 *	Criação de uma Estrutura VIEW				*
 ****************************************************************
 */
static VIEW *
newview (void)
{
	VIEW		*viewp;

	if ((viewp = malloc (sizeof (VIEW))) == NOVIEW)
		return (NOVIEW);

	viewp->v_win = newwin (LINES, 2 * VIEW_WIDTH, 0, 0);
	viewp->v_toleft = true;
	setmove (viewp->v_win, vis_cursor);

	viewp->v_lines = LINES;
	viewp->v_cols = VIEW_WIDTH;
	viewp->v_map = NOBASEP;

	viewp->v_err = newwin (1, COLS, LINES - 1, 0);

	viewp->v_first = NOBASE;
	viewp->v_search = NOSTR;
	viewp->v_ssearch = NOSTR;

	return (viewp);

}	/* end newview */

/*
 ****************************************************************
 *	Criação de uma Estrutura TEXT				*
 ****************************************************************
 */
static TEXT *
newtext (void)
{
	TEXT		*textp;

	if ((textp = malloc (sizeof (TEXT))) == NOTEXT)
		return (NOTEXT);

	textp->t_root = malloc (sizeof (BASE));
	textp->t_root->ch = "<ROOT>";
	textp->t_root->pos = NOBASE;
	textp->t_root->ant = NOBASE;
	textp->t_flags = 0;
	textp->t_name = NOSTR;

	return (textp);

}	/* end newtext */

/*
 ****************************************************************
 *	Tratamento de interrupções				*
 ****************************************************************
 */
static void
on_signal (int sig, ...)
{
	static char	Savename[] = "./VI.SAVE";

	switch (sig)
	{
	    case SIGINT:
		signal (SIGINT, on_signal);
		break;

	    case SIGQUIT:
		signal (SIGQUIT, on_signal);
		break;

	    case SIGTERM:
	    case SIGBUS:
	    case SIGADR:
	    case SIGSEGV:
		outcurses ();
		txt_write (Savename, FIRST (), LAST (), false, false);

		fprintf
		(	stderr,
			"%s: arquivo \"%s\" salvo como \"%s\"\n",
			pgname, Text->t_name, Savename
		);
		exit (0);
	}

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
		"%s - editor visual de textos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [<arquivo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
