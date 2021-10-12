/*
 ****************************************************************
 *								*
 *			cmd.c					*
 *								*
 *	Interpreta os comandos					*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.8, de 24.10.97				*
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

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

#include "../h/common.h"
#include "../h/proto.h"

/*
 ****** Variáveis locais ****************************************
 */
entry int		received_intr;	/* Veio um <^C> */

entry int		ignore_intr;	/* Diferente de zero, ignora <^C> */

entry const char	*cmd_nm;	/* Nome do comando em execução */

entry jmp_buf		prompt_point;	/* Estado para dar o prompt */

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
 ****** Variáveis dos padrões ***********************************
 */
entry const char	*inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int		inc_pati;

entry const char	*exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int		exc_pati;

/*
 ****** Mensagens comuns ****************************************
 */
const char	options_ifv[] =
{
	"\t-i: Pede a confirmação para cada <arquivo> a copiar\n"
	"\t-f: Os <arquivo>s destino existentes são removidos "
		"silenciosamente\n"
	"\t-v: Lista o nome dos <arquivo>s copiados\n"
};

const char	somewhere_expand[] =
{
	"\nObs.:\tOs meta-caracteres \"?*[]!\" são expandidos "
	"no sistema %s\n"
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
 ******	Protótipos de funções ***********************************
 */
void	print_hist (void);
int	exec_hist (const char *, int, char *);
void	edit_hist (void);
void	on_intr (int, ...);
void	do_help (int, const char *[]);

/*
 ****** Estrutura da tabela de comandos *************************
 */
typedef enum
{
	NM_DOS = 1,			/* Expande os nomes no DOS */
	NM_SOFIX			/* Expande os nomes no SOFIX */

}	CMDTB_ENUM;

typedef struct tab
{
	char	c_name[12];		/* Nome do comando */
	void	(*c_function) (int, const char *[]); 	/* Função que trata o comando */
	char	c_fs_status;		/* Estado necessário */
	char	c_nm_expand;		/* Modo de expansão dos nomes */

}	CMDTB;

/*
 ******	Tabela de comandos **************************************
 */
const CMDTB	cmdtab[] =
{
	"?",		do_help,	0,		0,
	"!",		do_help,	0,		0,	/* Para help */
	"arena",	do_arena,	0,		0,
	"ascii",	do_iso,		0,		0,
	"bin",		do_bin,		0,		0,
	"bye",		do_quit,	0,		0,
	"cat",		do_cat,		FS_MOUNT_RO,	NM_DOS,
	"cd",		do_cd,		FS_MOUNT_RO,	NM_DOS,
	"chd",		do_cd,		FS_MOUNT_RO,	NM_DOS,
	"chm",		do_chmod,	FS_MOUNT_RW,	NM_DOS,
	"chmod",	do_chmod,	FS_MOUNT_RW,	NM_DOS,
	"chsize",	do_chsize,	FS_MOUNT_RO,	NM_DOS,
	"clri",		do_clri,	FS_MOUNT_RW,	NM_DOS,
	"cmp",		do_cmp,		FS_MOUNT_RO,	NM_DOS,
	"compac",	do_compac,	FS_MOUNT_RO,	NM_DOS,
	"cmpget",	do_cmp,		FS_MOUNT_RO,	NM_DOS,
	"cmpgettree",	do_gettree,	FS_MOUNT_RO,	NM_DOS,
	"cmpput",	do_cmp,		FS_MOUNT_RO,	NM_SOFIX,
	"cmpputtree",	do_puttree,	FS_MOUNT_RW,	NM_SOFIX,
   /***	"cmptree",	.......,	.......,	..., ver abaixo ***/
	"del",		do_rm,		FS_MOUNT_RW,	NM_DOS,
	"df",		do_df,		FS_MOUNT_RO,	0,
	"diff",		do_diff,	FS_MOUNT_RO,	NM_DOS,
	"dir",		do_lc,		FS_MOUNT_RO,	NM_DOS,
	"du",		do_du,		FS_MOUNT_RO,	NM_DOS,
	"dump",		do_xd,		FS_MOUNT_RO,	NM_DOS,
	"echo",		do_echo,	FS_MOUNT_RO,	NM_DOS,
	"echoget",	do_echo,	FS_MOUNT_RO,	NM_DOS,
	"echoput",	do_echo,	0,		NM_SOFIX,
	"edbad",	do_edbad,	FS_MOUNT_RO,	0,
	"era",		do_rm,		FS_MOUNT_RW,	NM_DOS,
	"exit",		do_quit,	0,		0,
	"fsck",		do_fsck,	FS_MOUNT_RO,	0,
	"get",		do_get,		FS_MOUNT_RO,	NM_DOS,
	"gettree",	do_gettree,	FS_MOUNT_RO,	NM_DOS,
	"help",		do_help,	0,		0,
	"image",	do_bin,		0,		0,
	"iso",		do_iso,		0,		0,
	"lc",		do_lc,		FS_MOUNT_RO,	NM_DOS,
	"lfn",		do_lfn,		0,		0,
	"ls",		do_ls,		FS_MOUNT_RO,	NM_DOS,
	"md",		do_mkdir,	FS_MOUNT_RW,	0,
	"mkd",		do_mkdir,	FS_MOUNT_RW,	0,
	"mkdir",	do_mkdir,	FS_MOUNT_RW,	0,
	"mkfile",	do_mkfile,	FS_MOUNT_RW,	0,
	"mkfs",		do_mkfs,	0,		0,
	"mkv",		do_mkv,		FS_MOUNT_RW,	0,
	"mount",	do_mount,	0,		0,
	"mv",		do_mv,		FS_MOUNT_RW,	NM_DOS,
	"mvtree",	do_mvtree,	FS_MOUNT_RW,	NM_DOS,
	"param",	do_param,	FS_MOUNT_RO,	0,
	"path",		do_pwd,		FS_MOUNT_RO,	0,
	"put",		do_put,		FS_MOUNT_RW,	NM_SOFIX,
	"puttree",	do_puttree,	FS_MOUNT_RW,	NM_SOFIX,
	"pwd",		do_pwd,		FS_MOUNT_RO,	0,
	"q",		do_quit,	0,		0,
	"quit",		do_quit,	0,		0,
	"rd",		do_rmdir,	FS_MOUNT_RW,	NM_DOS,
	"ren",		do_mv,		FS_MOUNT_RW,	NM_DOS,
	"rm",		do_rm,		FS_MOUNT_RW,	NM_DOS,
	"rmd",		do_rmdir,	FS_MOUNT_RW,	NM_DOS,
	"rmdir",	do_rmdir,	FS_MOUNT_RW,	NM_DOS,
	"rmtree",	do_rmtree,	FS_MOUNT_RW,	NM_DOS,
	"rmv",		do_rm,		FS_MOUNT_RW,	NM_DOS,
	"ro",		do_ro,		FS_MOUNT_RO,	0,
	"rv",		do_rm,		FS_MOUNT_RW,	NM_DOS,
	"rw",		do_rw,		FS_MOUNT_RO,	0,
	"set",		do_stat,	0,		0,
	"seta",		do_iso,		0,		0,
	"setb",		do_bin,		0,		0,
	"show",		do_show,	FS_MOUNT_RO,	NM_DOS,
	"stat",		do_stat,	0,		0,
	"status",	do_stat,	0,		0,
	"sys",		do_sys,		0,		0,
	"tdat",		do_touch,	FS_MOUNT_RW,	NM_DOS,
	"touch",	do_touch,	FS_MOUNT_RW,	NM_DOS,
	"tree",		do_walk,	FS_MOUNT_RO,	NM_DOS,
	"type",		do_cat,		FS_MOUNT_RO,	NM_DOS,
	"umount",	do_umount,	0,		0,
	"walk",		do_walk,	FS_MOUNT_RO,	NM_DOS,
	"waste",	do_waste,	FS_MOUNT_RO,	0,
	"xd",		do_xd,		FS_MOUNT_RO,	NM_DOS,
	""
};

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

	expand_arg_free ();

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
			if (vol_nm[0])
				printf ("dosmp (%s) [%d]> ", vol_nm, hist_no);
			else
				printf ("dosmp [%d]> ", hist_no);

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
			error ("exec_cmd: Memória esgotada");

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

		/*
		 *	Caso super particular
		 */
		if (streq (cp, "cmptree")) for (EVER)
		{
			errno = NOERR;

			cp = "cmpgettree";

			fprintf (stderr, "%s: \"%s\"? (n): ", pgname, cp);

			if (askyesno () > 0)
				break;

			if (errno == EINTR)
				goto prompt;

			cp = "cmpputtree";

			fprintf (stderr, "%s: \"%s\"? (n): ", pgname, cp);

			if (askyesno () > 0)
				break;

			if (errno != 0)
				goto prompt;

		}	/* end (foi dado "cmptree") */

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

		argv[argc] = NOSTR;	/* Final dos argumentos */

		if (fs_status < tp->c_fs_status && (!argv[1] || !streq (argv[1], "-H")))
		{
			if   (fs_status == FS_MOUNT_RO)
			{
				printf
				(	"%s: O sistema de arquivos \"%s\" "
					"está montado somente para leituras\n",
					cmd_nm, dev_nm
				);
			}
			elif (fs_status == FS_OPEN)
			{
				printf
				(	"%s: O sistema de arquivos \"%s\" "
					"NÃO está montado\n",
					cmd_nm, dev_nm
				);
			}
			else 	/* fs_status == FS_CLOSED */
			{
				printf
				(	"%s: Não há dispositivo DOS disponível\n",
					cmd_nm
				);
			}
			goto prompt;
		}

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

		inc_pat[0] = NOSTR;	inc_pati = 0;
		exc_pat[0] = NOSTR;	exc_pati = 0;

		/*
		 *	Expande os argumentos e chama a função
		 */
		if   (tp->c_nm_expand == NM_SOFIX)
			sofix_nm_expand (tp->c_function, argv);
		elif (tp->c_nm_expand == NM_DOS)
			dos_nm_expand (tp->c_function, argv);
		else
			(*tp->c_function) (argc, argv);

		/*
		 *	Libera a memória alocada para os argumentos
		 */
		expand_arg_free ();

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
	char		prompt[12], area[BLSZ];

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

	/*
	 *	Imprime os comandos que podem ser executados neste estado
	 */
	if (fs_status < FS_MOUNT_RO)
	{
		printf
		(	"\n\n"
			"Comando(s) recomendado(s) no estado atual:"
			"\n    "
		);

		if   (fs_status == FS_CLOSED)
		{
			printf
			(	" mkfs          "    
				" mount         "    
			);
		}
		elif (fs_status == FS_OPEN)
		{
			printf
			(	" mkfs          "    
			);
		}
	}

	printf
	(	"\n\n"
		"Comandos do histórico: \"h\", \"eh\" e \"xh\" (análogos ao \"sh\")\n"
	);

	printf
	(	"\n"
		"Para obter informações sobre o comando <cmd>, use "
		"\"<cmd> -H\"\n"
	);

#if (0)	/*******************************************************/
	printf
	(	"\n"
		"Para obter maiores detalhes, use \"man %s\"\n",
		pgname
	);
#endif	/*******************************************************/

}	/* end do_help */

#if (0)	/*******************************************************/
			printf ("%s: \"get\" ou \"put\"? ", pgname);

			errno = NOERR;

			if (gets (line) == NOSTR && errno == EINTR)
				goto prompt;

			if   (streq (line, "get"))
				{ cp = "cmpgettree"; break; }
			elif (streq (line, "put"))
				{ cp = "cmpputtree"; break; }
#endif	/*******************************************************/
