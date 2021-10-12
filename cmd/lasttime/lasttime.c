/*
 ****************************************************************
 *								*
 *			lasttime.c				*
 *								*
 *	Lista os nomes de arquivos modificados recentemente	*
 *								*
 *	Versão	3.0.0, de 27.12.93				*
 *		4.2.0, de 03.05.02				*
 *								*
 *	Módulo: lasttime					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ftw.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.2.0, de 03.05.02";

#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ****** Garfield ************************************************
 */
const char	garfield[] = "garfield";
entry FILE	*gfp;

/*
 ****** Dados dos arquivos mais recentes ************************
 */
#define	NOITEM	(ITEM *)NULL

typedef struct item	ITEM;

struct item
{
	time_t	i_time;		/* Data do arquivo */
	char	*i_nm;		/* Nome do arquivo */
	long	i_mode;		/* Modo do arquivo */
	ITEM	*i_next;	/* Ponteiro para o próximo */
};

entry ITEM	time_list;	/* A cabeça da lista */

#undef	SZ_CONTROL
#ifdef	SZ_CONTROL
#define 	TIME_LIST_MAX  100	/* No. máximo de elementos */
entry int	TIME_LIST_SZ = TIME_LIST_MAX;	/* No. máximo de elementos */
entry int	time_list_sz;	/* No. de elementos */
#endif	SZ_CONTROL

entry time_t	TIME_LIMIT;	/* Tempo após o qual interessa */

/*
 ****** Indicadores do comandos	*********************************
 */
entry int	vflag;		/* Verbose */
entry int	gflag;		/* Garfield */
entry int	dotflag;	/* Não ignora ".old" */

/*
 ******	Os padrões **********************************************
 */
#define	NPAT	20

entry const char *inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int	inc_pati;

entry const char *exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int	exc_pati;

/*
 ******	Protótipos de funções ***********************************
 */
void		put_s_option (void);
int		put_p_option (char *, const char *[], int *);
int		pattern_accept (const char *);
int		get_time_ftw_function (const char *, STAT *, int);
char		*path_can (char *);
void		help (void);

/*
 ****************************************************************
 *	Lista os nomes de arquivos modificados recentemente	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	ITEM		*ip;
	int		opt, i;
	const char	*tree, *str;
	const char	*days_str = NOSTR;
#ifdef	SZ_CONTROL
	const char	*max_str = NOSTR;
#endif	SZ_CONTROL
	int		difuso;
	time_t		HOJE;		/* Início do dia de hoje (GMT) */

	/*
	 *	Analisa as opções
	 */
#ifdef	SZ_CONTROL
	while ((opt = getopt (argc, argv, "g.sr:n:p:P:vH")) != EOF)
#else
	while ((opt = getopt (argc, argv, "g.sr:p:P:vH")) != EOF)
#endif	SZ_CONTROL
	{
		switch (opt)
		{
		    case 'g':			/* Garfield */
			gflag++;
			break;

		    case '.':			/* ".old" */
			dotflag++;
			break;

		    case 's':			/* Padrões = 	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'r':			/* No. de dias */
			days_str = optarg;
			break;
#ifdef	SZ_CONTROL
		    case 'n':			/* No. max. de arquivos */
			max_str = optarg;
			break;
#endif	SZ_CONTROL
		    case 'p':		/* Padrões de inclusão */
			put_p_option ((char *)optarg, inc_pat, &inc_pati);
			break;

		    case 'P':		/* Padrões de exclusão */
			put_p_option ((char *)optarg, exc_pat, &exc_pati);
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
	 *	Prepara o "garfield"
	 */
	if (gflag && (gfp = fopen (garfield, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", garfield);

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stderr)), (void *)NULL, &color_vector);

	/*
	 *	Prepara as datas para o cálculo de HOJE
	 */
	time (&HOJE);				/* Agora GMT */

	difuso = gettzmin () * 60;

	HOJE += difuso;				/* Agora local */

	HOJE =  HOJE / (3600 * 24) * (3600 * 24);

	HOJE -= difuso;				/* Zero hora GMT ajustada */

	TIME_LIMIT = HOJE;

	/*
	 *	Analisa o número de dias dado
	 */
	if (days_str != NOSTR)
	{
		if ((i = strtol (days_str, &str, 0)) <= 0 || *str)
			error ("$No. de dias inválido: \"%s\"", days_str);

		TIME_LIMIT -= (i - 1) * (3600 * 24);
	}

#ifdef	SZ_CONTROL
	/*
	 *	Analisa o tamanho máximo dado
	 */
	if (max_str != NOSTR)
	{
		if ((i = strtol (max_str, &str, 0)) <= 0 || *str)
			error ("$No. de arquivos inválido: \"%s\"", max_str);

		TIME_LIST_SZ = i;
	}
#endif	SZ_CONTROL

	/*
	 *	Percorre as árvores
	 */
	while ((tree = *argv++) != NOSTR)
		ftw (path_can ((char *)tree), get_time_ftw_function);

	/*
	 *	Escreve os dados do arquivo mais recente
	 */
	for (ip = time_list.i_next; ip != NOITEM; ip = ip->i_next)
	{
		fprintf
		(	stderr,
			"%-24.24s:\t%s%s%s\n",
			btime (&ip->i_time),
			color_vector[(ip->i_mode & S_IFMT) >> 12],
			ip->i_nm,
			color_vector[MC_DEF]
		);

		if (gflag)
			fnputs (ip->i_nm, gfp);
	}

	return (exit_code);

}	/* end lasttime */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
int
get_time_ftw_function (const char *file_nm, STAT *sp, int code)
{
	ITEM		*ip, *lp;

	/*
	 *	Identifica o <arquivo> (a partir de ftw)
	 */
	switch (code)
	{
		/*
		 ******	Diretório *******************************
		 */
	    case FTW_D:		/* Diretórios ".old": pula toda subárvore */
		if (!dotflag)
		{
			if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
				return (1);
		}

		return (0);

		/*
		 ****** Arquivo normal **************************
		 */
	    case FTW_F:
	    case FTW_SL:
		if (!pattern_accept (file_nm))
			return (0);

		/*
		 *	Verifica se é recente
		 */
		if (sp->st_mtime < TIME_LIMIT)
			return (0);

#ifdef	SZ_CONTROL
		/*
		 *	Verifica se é um dos TIME_LIST_SZ primeiros
		 */
		if
		(
			time_list_sz >= TIME_LIST_SZ &&
			time_list.i_next->i_time >= sp->st_mtime
		)
			return (0);
#endif	SZ_CONTROL

		/*
		 *	Procura o lugar na lista
		 */
		if ((ip = malloc (sizeof (ITEM))) == NOITEM)
		{
		    nomem:
			error (NOSTR);
		}

		if ((ip->i_nm = strdup (file_nm)) == NOSTR)
			goto nomem;

		ip->i_time = sp->st_mtime;
		ip->i_mode = sp->st_mode;

		for (lp = &time_list; lp->i_next != NOITEM; lp = lp->i_next)
		{
			if (sp->st_mtime <= lp->i_next->i_time)
				break;
		}

		ip->i_next = lp->i_next;
		lp->i_next = ip;

#ifdef	SZ_CONTROL
		/*
		 *	Verifica se encheu
		 */
		if (++time_list_sz > TIME_LIST_SZ)
		{
			ip = time_list.i_next; 	time_list.i_next = ip->i_next;

			free (ip->i_nm); 	free (ip);

			time_list_sz--;
		}
#endif	SZ_CONTROL

		return (0);

		/*
		 ******	Arquivo ilegível ************************
		 */
	    case FTW_NS:
	    case FTW_DNR:
		error ("*Não consegui obter o estado do arquivo \"%s\"", file_nm);
		return (0);

		/*
		 ******	Outros tipos de arquivos ****************
		 */
	    default:
		error ("Tipo de arquivo inválido: %d", code);
		return (0);

	}	/* end switch */

}	/* end get_time_ftw_function */

/*
 ****************************************************************
 *	Executa a opção "-s" (padrões de "C")			*
 ****************************************************************
 */
void
put_s_option (void)
{
	put_p_option ("*.[cshryv]", inc_pat, &inc_pati);
	put_p_option ("*[Mm]akefile", inc_pat, &inc_pati);

}	/* end put_s_option */

/*
 ****************************************************************
 *	Executa a opção "-p" (outros padrões)			*
 ****************************************************************
 */
int
put_p_option (char *cp, const char *pat_vec[], int *pati_ptr)
{
	char		c;

	for (EVER)
	{
		while ((c = *cp) == ' ' || c == '\t' )
			cp++;

		if (c == '\0')
			return (0);

		if (*pati_ptr >= NPAT)
		{
			error ("No momento somente aceitamos " XSTR (NPAT) " padrões\n");
			help ();
		}

		pat_vec[(*pati_ptr)++] = cp++;

		while ((c = *cp) != '\0' && c != ' ' && c != '\t')
			cp++;

		if (c == '\0')
			return (0);

		*cp++ = '\0';

	}	/* end for (EVER) */

}	/* end put_p_option */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Testa em primeiro lugar os pontos
	 */
	if (!dotflag)
	{
		if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
			return (0);
	}

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	/*
	 *	Finalmente testa os padrões de exclusão
	 */
	if ((pp = exc_pat)[0] != NOSTR)
	{
		for (/* vazio */; *pp != NOSTR; pp++)
		{
			if (patmatch (file_nm, *pp))
				return (0);
		}
	}

	return (1);

}	/* end pattern_accept */

/*
 ****************************************************************
 *	Normaliza um caminho					*
 ****************************************************************
 */
char *
path_can (char *path)
{
	char		*cp;

	for (cp = strend (path) - 1; /* abaixo */; /* abaixo */)
	{
		if (cp <= path)
			break;

		if   (cp[0] == '/')
			{ *cp = '\0'; cp--; }
		elif (cp[0] == '.' && cp[-1] == '/')
			{ *cp = '\0'; cp--; }
		else
			break;
	}

	while (path[0] == '.' && path[1] == '/')
		path += 2;

	return (path);

}	/* end path_can */

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
		"%s - lista os nomes de arquivos modificados recentemente\n"
		"\n%s\n"
		"\nSintaxe:\n"
#ifdef	SZ_CONTROL
		"\t%s [-g.] [-r <n>] [-n <max>] <PAD> <árvore> ...\n\n"
#else
		"\t%s [-g.] [-r <n>] <PAD> <árvore> ...\n\n"
#endif	SZ_CONTROL
		"\tValores de <PAD>: [-s] [-p \"<p>\"] [-P \"<P>\"]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-g: Gera o arquivo \"garfield\" com o nome dos arquivos\n"
		"\t-.: Processa também arquivos ocultos (\".old\" ...)\n"
		"\t-s: Seleciona arquivos através dos padrões dos fontes em \"C\"\n"
		"\t-r: Imprime os arquivos modificados nos últimos <n> dias "
			"ao invés\n\t    de um dia (hoje)\n"
#ifdef	SZ_CONTROL
		"\t-n: Imprime até <max> arquivos ao invés de "
			XSTR (TIME_LIST_MAX) "\n"
#endif	SZ_CONTROL
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	exit (2);

}	/* end help */
