/*
 ****************************************************************
 *								*
 *			walk.c					*
 *								*
 * 	Caminha em árvores do sistema de arquivos		*
 *								*
 *	Versão	1.0.0, de 28.09.86				*
 *		4.2.0, de 25.04.02				*
 *								*
 *	Módulo: walk						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 25.04.02";

#define	EVER	;;
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	NPAT	20	/* No. maximo de Padrões */

/*
 ******	Variáveis globais ***************************************
 */
entry int	rflag;		/* Arquivos regulares */
entry int	dflag;		/* Diretórios  */
entry int	bflag;		/* Arquivos especiais de blocos */
entry int	cflag;		/* Arquivos especiais de caracteres */
entry int	lflag;		/* Elos simbólicos */
entry int	fflag;		/* FIFOs  */
entry int	mflag;		/* Prefixo */
entry int	Lflag;		/* Evite LINKS flag */
entry int	dotflag;	/* Não ignora ".old" ... */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */
entry int	vflag;		/* Verbose */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Os padrões **********************************************
 */
entry const char *inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int	inc_pati;

entry const char *exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int	exc_pati;

/*
 ******	Protótipos de funções ***********************************
 */
int		accept (const char *, STAT *, int);
void		put_s_option (void);
int		put_p_option (char *, const char *[], int *);
int		pattern_accept (const char *);
void		process (const char *, STAT *);
int		proclink (STAT *);
void		help (void);

/*
 ****************************************************************
 *	Estruturas						*
 ****************************************************************
 */
typedef struct litem	LITEM;

struct litem
{
	dev_t	l_dev;		/* Dispositivo */
	ino_t	l_ino;		/* No. do Inode */
	LITEM	*l_next;	/* o Proximo */
};

entry LITEM	*linklist;	/* Comeco da Lista */

/*
 ****************************************************************
 * 	Caminha em árvores do sistema de arquivos		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "rdbclfamL.Nsp:P:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'r':		/* Arquivos regulares */
			rflag++;
			continue;

		    case 'd':		/* Diretórios */
			dflag++;
			continue;

		    case 'b':		/* Especial de blocos */
			bflag++;
			continue;

		    case 'c':		/* Especial de caracteres */
			cflag++;
			continue;

		    case 'l':		/* Elos Simbólicos */
			lflag++;
			continue;

		    case 'f':		/* FIFOs */
			fflag++;
			continue;

		    case 'a':		/* Todos tipos */
			rflag++;
			dflag++;
			bflag++;
			cflag++;
			fflag++;
			continue;

		    case 'm':			/* Prefixo */
			mflag++;
			break;

		    case 'L':			/* evite LINKS */
			Lflag++;
			break;

		    case '.':			/* Não ignora ".old" ... */
			dotflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 's':			/* Padrões = 	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':		/* Padrões de inclusão */
			put_p_option ((char *)optarg, inc_pat, &inc_pati);
			break;

		    case 'P':		/* Padrões de exclusão */
			put_p_option ((char *)optarg, exc_pat, &exc_pati);
			break;

		    case 'v':		/* verbose flag */
			vflag++;
			continue;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fputc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */ 

	/*
	 *	Ajustes
	 */
	argv += optind;

	if (rflag + dflag + bflag + cflag + lflag + fflag == 0)
		{ rflag++; bflag++; cflag++; lflag++; fflag++; }

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
		{ error ("Os argumentos supérfluos serão ignorados"); *argv = NOSTR; }

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR) 		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
			ftw (area, accept);
	}
	else 				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
			ftw (*argv, accept);
	}

	return (0);

}	/* end walk */

/*
 ****************************************************************
 *	Processa um nome 					*
 ****************************************************************
 */
int
accept (const char *file_nm, STAT *sp, int status)
{
	if   (status == FTW_NS)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (0);
	}
	elif (status == FTW_DNR)
	{
		error ("*Não consegui percorrer \"%s\"", file_nm);
	}

	/*
	 *	Seleciona os tipos de arquivos
	 */
	switch (sp->st_mode & S_IFMT)
	{
	    case S_IFREG:
		if (!rflag)
			return (0);
		break;

	    case S_IFDIR:	/* Diretórios ".old": pula toda subárvore */
		if (!dotflag)
		{
			if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
				return (1);
		}

		if (!dflag)
			return (0);
		break;

	    case S_IFIFO:
		if (!fflag)
			return (0);
		break;

	    case S_IFBLK:
		if (!bflag)
			return (0);
		break;

	    case S_IFCHR:
		if (!cflag)
			return (0);
		break;

	    case S_IFLNK:
		if (!lflag)
			return (0);
		break;

	    default:
		error ("Tipo inválido de arquivo: \"%s\"", file_nm);
		return (0);

	}	/* end switch */


	/*
	 *	Analisa os padrões
	 */
	if (pattern_accept (file_nm))
		process (file_nm, sp);

	return (0);

}	/* end accept */

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
			printf
			(	"%s: No momento somente aceitamos "
				XSTR (NPAT) " padrões\n\n",
				pgname
			);
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
 *	Processa O arquivo					*
 ****************************************************************
 */
void
process (const char *file_nm, STAT *sp)
{

	if (Lflag)
	{
		if (proclink (sp) > 0)
			return;
	}

	if (mflag)
	{
		printf ("%c  ", *(modetostr (sp->st_mode)));

		if (S_ISREG (sp->st_mode))
			printf ("%7d  ", sp->st_size);
		else
			printf ("         ");
	}

	printf
	(	"%s%s%s\n",
		color_vector[(sp->st_mode & S_IFMT) >> 12],
		file_nm,
		color_vector[MC_DEF]
	);

}	/* end process */

/*
 ****************************************************************
 *	Processa LINKs						*
 ****************************************************************
 */
int
proclink (STAT *sp)
{
	LITEM		*lp;

	if (sp->st_nlink == 1)
		return (0);

	for (lp = linklist; lp != (LITEM *)NULL; lp = lp->l_next)
	{
		if (lp->l_dev == sp->st_dev && lp->l_ino == sp->st_ino)
			return (1);
	}

	if ((lp = malloc (sizeof (LITEM))) == (LITEM *)NULL)
	{
		error ("Não obtive memória para item de elo físico");
	}
	else
	{
		lp->l_dev = sp->st_dev;
		lp->l_ino = sp->st_ino;

		lp->l_next = linklist;
		linklist = lp;
	}

	return (0);

}	/* end proclink */

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
	 	"%s - caminha em árvores do sistema de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [rdbclfamL.N] [<árvore> ...]\n\n"
		"\tValores de <PAD>: [-s] [-p \"<p>\"] [-P \"<P>\"]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Sem nenhuma das opções \"-rdbclfa\" lista todos os tipos "
			"\n\t    de arquivos exceto diretórios\n"
		"\t-r: Lista arquivos regulares\n"
		"\t-d: Lista diretórios\n"
		"\t-b: Lista arquivos especiais de bloco (estruturados)\n"
		"\t-c: Lista arquivos especiais de caracteres (não estruturados)\n"
		"\t-l: Lista elos simbólicos\n"
		"\t-f: Lista FIFOs\n"
		"\t-a: Lista todos os tipos de arquivos (equivalente a \"-rdbcf\")\n"
		"\t-m: Imprime um prefixo com o tipo do arquivo e seu tamanho\n"
	);
	fprintf
	(	stderr,
		"\t-L: Não repete arquivos com mais de um elo físico\n"
		"\t-.: Lista também arquivos ocultos (\".old\" ...)\n"
		"\t-N: Lê os nomes das <árvore>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados <árvore>s\n"
		"\t-s: Adiciona os padrões dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);
	exit (2);

}	/* end help */
