/*
 ****************************************************************
 *								*
 *			cmd.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 30.07.97				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <time.h>
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/edusr.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry jmp_buf		prompt_point;	/* Estado para dar o prompt */

entry int		received_intr;	/* Veio um <^C> */

entry int		ignore_intr;	/* Diferente de zero, ignora <^C> */

entry const char	*cmd_nm;	/* Nome do comando em execução */

/*
 ****** Indicadores do comandos	*********************************
 */
entry int	cmd_iflag;	/* Interativo */
entry int	cmd_vflag;	/* Verboso */
entry int	cmd_fflag;	/* Força */
entry int	cmd_lflag;	/* Longo */
entry int	cmd_dflag;	/* Debug */
entry int	cmd_dotflag;	/* Não ignora ".old" */
entry int	cmd_cflag;	/* Comparação */
entry int	cmd_uflag;	/* Atualização */
entry int	cmd_aflag;	/* Todas as entradas */
entry int	cmd_sflag;	/* Somente o total */

/*
 ******	Tabela dos comandos *************************************
 */
typedef struct tab
{
	char	c_name[12];		/* Nome do comando */
	void	(*c_function) (int, const char *[]); /* Função que trata o comando */

}	CMDTB;

const CMDTB	cmdtab[] =
{
	"c",		do_cota,
	"cota",		do_cota,
	"data",		do_expir,
	"e",		do_expir,
	"exit",		do_quit,
	"expir",	do_expir,
	"g",		do_mkgrp,
	"help",		do_help,
	"home",		do_home,
	"mg",		do_mkgrp,
	"mkgrp",	do_mkgrp,
	"mkusr",	do_mkusr,
	"mu",		do_mkusr,
	"pg",		do_prgrp,
	"prgrp",	do_prgrp,
	"prusr",	do_prusr,
	"pu",		do_prusr,
	"q",		do_quit,
	"quit",		do_quit,
	"quota",	do_cota,
	"rg",		do_rmgrp,
	"rmgrp",	do_rmgrp,
	"rmusr",	do_rmusr,
	"ru",		do_rmusr,
	"set",		do_set,
	"shell",	do_shell,
	"u",		do_mkusr,
	"?",		do_help,
	""
};

/*
 ******	Variáveis do histórico **********************************
 */
#define	NHIST	20

typedef struct
{
	int	h_no;		/* O número */
	char	*h_cmd;		/* A linha do comando */

}	HIST;

entry HIST	hist[NHIST];	/* O histórico */

entry HIST	*hist_ptr = &hist[0]; /* Ponteiro para próxima entrada a usar */

entry int	hist_no = 1;	/* O número do próximo comando */

/*
 ****************************************************************
 *	Identificador de Comandos				*
 ****************************************************************
 */
void
exec_cmd (const char *first_cmd)
{
	const CMDTB	*tp;
	char		*cp;
	HIST		*hp;
	int		argc;
	char		c;
	char		line[BLSZ];
	const char	*argv[10];

	/*
	 *	Lê e processa os comandos
	 */
	signal (SIGINT, on_intr);

	setjmp (prompt_point);

    prompt:
	for (EVER)
	{
		ignore_intr = 1;	/* No momento, sempre ignora */
		received_intr = 0;

		if (first_cmd != NOSTR)
		{
			strcpy (line, first_cmd);
			first_cmd = NOSTR;
		}
		else
		{
			printf ("edusr [%d]> ", hist_no);

			fflush (stdout);

			errno = NOERR;

			if (gets (line) == NOSTR)
			{
				if (errno == EINTR)
					continue;

				strcpy (line, "quit");
			}
		}

		/*
		 *	Procura o nome do comando
		 */
	    parse:
		if (!stdin_is_a_tty)
			printf ("%s\n", line);

		for (cp = line; cp[0] == ' ' || cp[0] == '\t'; cp++)
			/* vazio */;

		if (cp[0] == '\0')
			continue;

		/*
		 *	Verifica se é um comando do histórico
		 */
		c = '*';

		if   (cp[0] == 'h')
			c = cp[1];
		elif ((cp[0] == 'e' || cp[0] == 'x') && cp[1] == 'h')
			c = cp[2];

		if (c == ' ' || c == '\t' || c == '\0')
		{
			switch (cp[0])
			{
			    case 'h':
				print_hist ();
				continue;

			    case 'x':
			    case 'e':
				if (exec_hist (cp + 2, cp[0], line) >= 0)
					goto parse;
				continue;

			}	/* end switch */
		}

		/*
		 *	Insere no histórico
		 */
		hp = hist_ptr;

		if (hp->h_cmd != NOSTR)
			free (hp->h_cmd);

		if ((hp->h_cmd = malloc (strlen (cp) + 1)) == NOSTR)
			error ("$exec_cmd: Memória esgotada");

		strcpy (hp->h_cmd, cp);
		hp->h_no = hist_no;

		hist_no++;

		if (++hist_ptr >= &hist[NHIST])
			hist_ptr = &hist[0];

		/*
		 *	Verifica se é um comando para o "sh" local
		 */
		if (cp[0] == '!')
			{ exec_local_cmd (cp + 1); continue; }

		cp = strtok (cp, " \t");

		cmd_nm = argv[0] = cp;	/* O nome do comando */

		/*
		 *	Procura o comando na tabela
		 */
 		for (tp = cmdtab; /* teste abaixo */; tp++)
		{
			if (tp->c_name[0] == '\0')
			{
				printf ("Comando \"%s\" desconhecido\n", cp);
				goto prompt;
			}

			if (streq (cp, tp->c_name))
				break;
		}

		/*
		 *	Retira os argumentos
		 */
		for (argc = 1; (cp = strtok (NOSTR, " \t")) != NOSTR; argc++)
			argv[argc] = cp;

		if (argv[1] != NOSTR && streq (argv[1], "?"))
			argv[1] = "-H";

		argv[argc] = NOSTR;	/* Final dos argumentos */

		/*
		 *	Inicializa os indicadores
		 */
		cmd_iflag = 0;		/* Interativo */
		cmd_vflag = 0;		/* Verboso */
		cmd_fflag = 0;		/* Força */
		cmd_lflag = 0;		/* Longo */
		cmd_dflag = 0;		/* Debug */
		cmd_dotflag = 0;	/* Não ignora ".old" */
		cmd_cflag = 0;		/* Comparação */
		cmd_uflag = 0;		/* Atualização */
		cmd_aflag = 0;		/* Todas as entradas */
		cmd_sflag = 0;		/* Somente o total */

		cmd_optinit++;

		(*tp->c_function) (argc, argv);

	}	/* end for (EVER) */

}	/* end exec_cmd */

/*
 ****************************************************************
 *	Imprime o histórico					*
 ****************************************************************
 */
void
print_hist (void)
{
	int		i;
	HIST		*hp;

	for (i = 0, hp = hist_ptr; i < NHIST; i++, hp++)
	{
		if (hp >= &hist[NHIST])
			hp = &hist[0];

		if (hp->h_cmd == NOSTR)
			continue;

		printf ("%4d: %s\n", hp->h_no, hp->h_cmd);
	}

}	/* end print_hist */

/*
 ****************************************************************
 *	Executa um comando do histórico				*
 ****************************************************************
 */
int
exec_hist (const char *cp, int function, char *line)
{
	int		no;
	HIST		*hp;
	const char	*str;
	char		prompt[12], area[512];

	while (cp[0] == ' ' || cp[0] == '\t')
		 cp++;

	if   (cp[0] == '\0')
	{
		no = hist_no - 1;
	}
	elif ((no = strtol (cp, &str, 0)) <= 0 || *str != '\0')
	{
		printf
		(	"%s: Número de histórico inválido: \"%s\"\n",
			pgname, cp
		);
		return (-1);
	}

	for (hp = &hist[0]; /* abaixo */; hp++)
	{
		if (hp >= &hist[NHIST])
		{
			printf
			(	"%s: Número ausente do histórico: \"%d\"\n",
				pgname, no
			);
			return (-1);
		}

		if (hp->h_cmd == NOSTR)
			continue;

		if (hp->h_no == no)
			break;
	}

	/*
	 *	Verifica qual a função
	 */
	if (function == 'x')
	{
		strcpy (line, hp->h_cmd);
		printf ("%s\n", hp->h_cmd);
	}
	else	/* function == 'e' */
	{
		sprintf (prompt, "%d: ", no);

		if (edit (prompt, hp->h_cmd, area) < 0)
			return (-1);

		fprintf (stderr, "Deseja executar o comando ? (n): ");

		if (askyesno () <= 0)
			return (-1);

		strcpy (line, area);
	}

	return (0);

}	/* end exec_hist */

/*
 ****************************************************************
 *	Executa um comando local				*
 ****************************************************************
 */
void
exec_local_cmd (char *line)
{
	char		*cp;
	const char	*dir;
	char		path[128];

	/*
	 *	Pula os brancos iniciais
	 */
	for (cp = line; cp[0] == ' ' || cp[0] == '\t'; cp++)
		/* vazio */;

	if (cp[0] == '\0')
		return;

	/*
	 *	Analisa especialmente o comando "cd"
	 */
	if (cp[0] == 'c' && cp[1] == 'd' && (cp[2] == ' ' || cp[2] == '\t' || cp[2] == '\0'))
	{
		for (cp += 2; /* abaixo */; cp++)
		{
			if (*cp == ' ' || *cp == '\t')
				continue;

			if (*cp == '\0')
				dir = getenv ("HOME");
			else
				dir = env_var_scan (path, cp);

			break;
		}

		if (dir == NOSTR)
			dir = ".";

		if (chdir (dir) < 0)
		{
			fprintf
			(	stderr,
				"Diretório inválido: \"%s\" (%s)\n",
				path, strerror (errno)
			);
		}

		return;

	}	/* end if (linha é "cd ...") */

	/*
	 *	Outros comandos
	 */
	system (line);

}	/* end exec_local_cmd */

/*
 ****************************************************************
 *	Substitui variáveis "$..." do ambiente em um caminho	*
 ****************************************************************
 */
char *
env_var_scan (char *output_path, char *input_path)
{
	const char	*path, *env;
	int		slash_missing = 0;

	output_path[0] = '\0';

	for
	(	path = strfield (input_path, '/');
		path != NOSTR;
		path = strfield (NOSTR, '/')
	)
	{
		if (slash_missing)
			strcat (output_path, "/");
		else
			slash_missing++;

		if (path[0] == '$' && (env = getenv (path + 1)) != NOSTR)
			strcat (output_path, env);
		else
			strcat (output_path, path);
	}

	return (output_path);

}	/* end env_var_scan */

/*
 ****************************************************************
 *	Tratamento de interrupções ("saída elegante")		*
 ****************************************************************
 */
void
on_intr (int signo, ...)
{
	signal (SIGINT, on_intr);

	received_intr++;

	if (ignore_intr)
		return;

	longjmp (prompt_point, 1);

}	/* end on_intr */

/*
 ****************************************************************
 *	Comando "help"						*
 ****************************************************************
 */
void
do_help (int argc, const char *argv[])
{
	const CMDTB	*tp;
	int		i;

	/*
	 *	Imprime os comandos disponíveis
	 */
	printf ("%s - %s\n\n", pgname, pgversion);

	printf ("Comandos disponíveis:");

	for (i = 0, tp = cmdtab; tp->c_name[0] != '\0'; tp++)
	{
		if (i % 5 == 0)
			printf ("\n    ");

		printf (" %-14.14s", tp->c_name);

		i++;
	}

	printf
	(	"\n\n"
		"Comandos do histórico: \"h\", \"eh\" e \"xh\" (análogos ao \"sh\")\n"
	);

	printf
	(	"\n"
		"Para obter informações sobre o comando <cmd>, use "
		"\"<cmd> -H\" ou \"<cmd> ?\"\n"
	);

#if (0)	/*******************************************************/
	printf
	(	"\n"
		"Para obter maiores detalhes, use \"man %s\"\n",
		pgname
	);
#endif	/*******************************************************/

}	/* end do_help */
