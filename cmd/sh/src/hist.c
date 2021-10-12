/*
 ****************************************************************
 *								*
 *			hist.c					*
 *								*
 *	Funções para a manipulação do histórico			*
 *								*
 *	Versão	3.2.2, de 15.09.99				*
 *		4.4.0, de 21.11.02				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>
#include	<sys/syscall.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<termio.h>

#include	"../h/common.h"

/*
 ****************************************************************
 *	Estrutura básica do histórico				*
 ****************************************************************
 */
#define	NOHIST	(HIST *)0

typedef struct
{
	char	*h_line;	/* Linha do histórico */
	int	h_len;		/* Número de caracteres do comando */

}	HIST;

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry	char	HISTMAX[10];	/* Cadeia com o valor de "hist_max" */

entry	int	hist_max,	/* Tamanho máximo do histórico */
		hist_sz,	/* Tamanho efetivo do histórico */
		last_cmd;	/* Número do último comando inserido */

entry	HIST	*history,	/* Início do histórico */
		*end_history,	/* Fim do histórico */
		*histp;		/* Local da próxima inserção */

/*
 ****************************************************************
 *	Insere um comando novo no histórico			*
 ****************************************************************
 */
int
put_history (const char *cmd)
{
	HIST		*hp = histp;
	char		*new_line;
	int		len;

	if (cmd == NOSTR || cmd[0] == '\0')
		return (-1);

	if ((hp = histp) >= end_history)	/* Circularmente */
		hp = history;

	/*
	 *	Verifica se é preciso alocar uma linha maior.
	 */
	if (hp->h_len < (len = strlen (cmd)))
	{
		if ((new_line = malloc (len + 1)) == NOSTR)
			return (-1);

		if (hp->h_line != NOSTR)
			free (hp->h_line);

		hp->h_line = new_line;
		hp->h_len  = len;
	}

	/*
	 *	Insere o novo comando.
	 */
	memmove (hp->h_line, cmd, len + 1);

	histp = hp + 1;

	++last_cmd;

	if (hist_sz < hist_max)
		hist_sz++;

	return (0);

}	/* end put_history */

/*
 ****************************************************************
 *	Obtém um comando, dado seu número			*
 ****************************************************************
 */
char *
get_history (int cmd)
{
	HIST		*hp;

	if (cmd > last_cmd || cmd <= last_cmd - hist_sz)
		return (NOSTR);

	if ((hp = histp + (cmd - last_cmd - 1)) < history)
		hp += hist_max;

	return (hp->h_line);

}	/* end get_history */

/*
 ****************************************************************
 *	Mostra o histórico					*
 ****************************************************************
 */
void
show_history (FILE *fp)
{
	HIST		*hp;
	int		n = last_cmd - hist_sz + 1;

	if (hist_sz >= hist_max)
	{
		for (hp = histp; hp < end_history; hp++)
			fprintf (fp, "\t%3d:\t%s\n", n++, hp->h_line);
	}

	for (hp = history; hp < histp; hp++)
		fprintf (fp, "\t%3d:\t%s\n", n++, hp->h_line);

}	/* end show_history */

/*
 ****************************************************************
 *	Altera o tamanho do histórico				*
 ****************************************************************
 *
 *	Retorna o novo tamanho máximo.
 */
int
set_history_size (int new_hist_max)
{
	HIST		*new_hist, *hp1, *hp2;
	int		sz1, sz2;

	/*
	 *	Uma pequena consistência.
	 */
	if (new_hist_max <= 0 || new_hist_max == hist_max)
		return (hist_max);

	/*
	 *	Aloca o novo histórico zerado.
	 */
	if ((new_hist = calloc (new_hist_max, sizeof (HIST))) == NOHIST)
		return (hist_max);

	if (new_hist_max > hist_max)
	{
		/*
		 *	O histórico aumenta de tamanho.
		 */
		hp1 = (hist_sz >= hist_max) ? histp : end_history;
		hp2 = history;
	}
	else
	{
		/*
		 *	O histórico diminui de tamanho.
		 */
		hp1 = end_history;

		if ((hp2 = histp - new_hist_max) < history)
		{
			if (hist_sz >= hist_max)
				hp1 = hp2 + hist_max;

			hp2 = history;
		}
	}

	/*
	 *	Copia o que é necessário do antigo para o novo.
	 */
	if ((sz1 = end_history - hp1) > 0)
		memmove (new_hist, hp1, sz1 * sizeof (HIST));

	if ((sz2 = histp - hp2) > 0)
		memmove (new_hist + sz1, hp2, sz2 * sizeof (HIST));

	/*
	 *	Novo histórico.
	 */
	if (history != NOHIST)
		free (history);

	hist_max    = new_hist_max;
	hist_sz     = sz1 + sz2;

	history     = new_hist;
	end_history = history + hist_max;
	histp       = new_hist + hist_sz;

	snprintf (HISTMAX, sizeof (HISTMAX), "%d", hist_max);

	return (hist_max);

}	/* end set_history_size */

/*
 ****************************************************************
 *	Seleciona um comando do histórico			*
 ****************************************************************
 */
char *
select_command_from_history (int *selection)
{
	TERMIO		oldtermio, termio;
	int		first_cmd, ncmd, c, len, limit, size;
	char		*cmdp, *dots;

	if (last_cmd <= 0)
		{ error ("O histórico está vazio"); return (NOSTR); }

	/*
	 *	Coloca o terminal em modo RAW.
	 */
	if (ioctl (0, TCGETS, &oldtermio) < 0)
		return (NOSTR);

	if ((limit = oldtermio.t_ncol - 25) <= 0)
		return (NOSTR);

	termio          = oldtermio;
	termio.c_lflag &= ~(ECHO|ICANON|ISIG);
	termio.c_oflag &= ~OPOST;
	termio.t_min = 1;

	if (ioctl (0, TCSETNR, &termio) < 0)
		return (NOSTR);

	/*
	 *	Limita o número de comandos ao tamanho da tela.
	 */
	size = termio.t_nline - 2;

	if (size > hist_sz)
		size = hist_sz;

	/*
	 *	Mostra o histórico.
	 */
	first_cmd = last_cmd - size + 1;

	for (ncmd = first_cmd; ncmd <= last_cmd; ncmd++)
	{
		if ((cmdp = get_history (ncmd)) != NOSTR)
		{
			dots = "";

			if ((len = strlen (cmdp)) > limit)
				len = limit, dots = "...";

			fprintf
			(	stderr,
				"\t%3d:    %-*.*s%s\r\n",
				ncmd, len, len, cmdp, dots
			);
		}
	}

	ncmd = last_cmd;
	fprintf (stderr, "\e[A");

	/*
	 *	Lê os comandos, movimentando a barra de seleção.
	 */
	for (EVER)
	{
		dots = "";

		if ((len = strlen (cmdp = get_history (ncmd))) > limit)
			len = limit, dots = "...";

		/*
		 *	Mostra o comando em destaque.
		 */
		fprintf
		(	stderr,
			"\r\t\e[7m%3d:    %-*.*s%s\e[0m",
			ncmd, len, len, cmdp, dots
		);

		c = getchar ();

		/*
		 *	Apaga o destaque.
		 */
		fprintf
		(	stderr,
			"\r\t%3d:    %-*.*s%s",
			ncmd, len, len, cmdp, dots
		);

		/*
		 *	Analisa o que foi teclado.
		 */
		switch (c)
		{
		    case '\e':
			if (getchar () != '[')
				goto fim;

			/*
			 *	Processa as setas.
			 */
			switch (getchar ())
			{
			    up_arrow:
			    case 'A':
				if (ncmd > first_cmd)
				{
					cmdp = get_history (--ncmd);
					fprintf (stderr, "\e[A");
				}
				else
				{
					putc ('\007', stderr);
				}
				break;

			    down_arrow:
			    case 'B':
				if (ncmd < last_cmd)
				{
					cmdp = get_history (++ncmd);
					fprintf (stderr, "\e[B");
				}
				else
				{
					putc ('\007', stderr);
				}
				break;

			    case 'C':
				c = 'x';
				goto fim;

			    case 'D':
				c = 'e';
				goto fim;

			    default:
				putc ('\007', stderr);
				break;
			}
			break;

		    case 'j':
			goto down_arrow;

		    case 'k':
			goto up_arrow;

		    case '\r':
		    case 'e':
		    case 'q':
		    case 'x':
			goto fim;

		    default:
			putc ('\007', stderr);
			break;

		}	/* end switch (caractere lido) */

	}	/* end for (lendo e processando comandos) */

	/*
	 *	Retorna ao estado anterior.
	 */
    fim:
	while (ncmd++ < last_cmd)
		putc ('\n', stderr);

	fprintf (stderr, "\r\n");
 
	ioctl (0, TCSETNR, &oldtermio);

	if (selection != (int *)NULL)
		*selection = c;

	return (cmdp);

}	/* end select_command_from_history */

#ifdef	DEBUG
void
main (void)
{
	char	line[100], *cp;
	int	n;

	if (set_history_size (5) < 0)
	{
		printf ("Não consegui alocar o histórico\n");
		exit (1);
	}

	while (fgets (line, sizeof (line), stdin) != NOSTR)
	{
		switch (line[0])
		{
		    case 'h':
			show_history (stdout);
			break;

		    case 'p':
			put_history (line + 2);
			show_history (stdout);
			break;

		    case 'g':
			n = atoi (line + 1);
			cp = get_history (n);

			printf ("Comando %d: <%s>\n", n, cp == NOSTR ? "não existe" : cp);
			break;

		    case 't':
			n = atoi (line + 1);
			if (set_history_size (n) < 0)
				printf ("Não consegui mudar o tamanho\n");
			else
				show_history (stdout);
		}
	}

}
#endif
