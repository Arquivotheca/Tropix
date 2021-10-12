/*
 ****************************************************************
 *								*
 *			chmod.c					*
 *								*
 *	Modifica a permissão de acesso de arquivos 		*
 *								*
 *	Versão	1.0.2, de 23.07.87				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: chmod						*
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
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

#define	EVER	;;
#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */
entry int	dflag;		/* Debug */

/*
 ******	Definições relativas aos modos **************************
 */
#define	USER	0x04		/* Para identificar quem */
#define	GROUP	0x02
#define	OTHERS	0x01

#define	SHIFT	3		/* Shift entre USER/GROUP/OTHERS */ 

typedef struct	{ int op; long mode; }	ACTION;

#define	ASZ	10

entry ACTION	action[ASZ],	/* Ações a tomar */
		*actionp = action;

/*
 ******	Protótipos de funções ***********************************
 */
void		mode_op_conv (const char *);
int		change_file_mode (const char *);
void		help (void);

/*
 ****************************************************************
 *	Programa de trocar o modo de arquivos			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ret = 0;
	char		*ep;
	const char	*bp;
	const char	*str;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vdNH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
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

	if (*argv == NOSTR)
		help ();

	/*
	 *	Processa os vários modos dados
	 */
	bp = *argv++;

	if (*bp >= '0' && *bp <= '9')
	{
		/*
		 *	Modo absoluto
		 */
		action->op	= '=';
		action->mode	= strtol (bp, &str, 0);

		if (*str != '\0')
		{
			error ("Modo inválido: \"%s\"\n", bp);
			help ();
		}

		actionp++;
	}
	else
	{
		/*
		 *	Modo simbólico
		 */
		while ((ep = strchr (bp, ',')) != NOSTR)
		{
			*ep = '\0';
			mode_op_conv (bp);
			bp = ep + 1;
		}

		mode_op_conv (bp);
	}

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		error ("Os argumentos supérfluos serão ignorados");
		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)
	{
		char		*area = alloca (1024);

		/*
		 *	Não foram dados argumentos: lê do "stdin"
		 */
		while (fngets (area, 1024, stdin) != NOSTR)
		{
			ret += change_file_mode (area);
		}
	}
	else
	{
		/*
		 *	Foram dados argumentos
		 */
		for (/* vazio */; *argv; argv++)
		{
			ret += change_file_mode (*argv);
		}
	}

	return (ret);

}	/* end main () */

/*
 ****************************************************************
 *	Converte uma codificação simbólica na forma interna	*
 ****************************************************************
 */
void
mode_op_conv (const char *mode_nm)
{
	const char	*cp = mode_nm;
	int		op;
	int		who = 0;
	long		permission = 0;
	long		mode = 0;

	/*
	 *	Primeira parte: "who"
	 */
	for (EVER) switch (*cp++)
	{
	    case 'u':
		who |= USER;
		break;

	    case 'g':
		who |= GROUP;
		break;

	    case 'o':
		who |= OTHERS;
		break;

	    case 'a':
		who |= (USER|GROUP|OTHERS);
		break;

	    default:
		if (who == 0)
			who |= (USER|GROUP|OTHERS);
		cp--;
		goto operator;
	}

	/*
	 *	Segunda parte: "Operator"
	 */
    operator:
	switch (op = *cp++)
	{
	    case '+':
	    case '-':
	    case '=':
		break;

	    default:
	    mode_err:
		error ("Modo inválido: \"%s\"\n", mode_nm);
		help ();
	}

	/*
	 *	Terceira parte: "Permission"
	 */
	for (EVER) switch (*cp++)
	{
	    case 'r':
		permission |= S_IREAD;
		break;

	    case 'w':
		permission |= S_IWRITE;
		break;

	    case 'x':
		permission |= S_IEXEC;
		break;

	    case 's':
		permission |= S_ISUID;
		break;

	    case 't':
		mode |= S_ISVTX;
		break;

	    case 'm':
		mode |= S_IMETX;
		break;

	    case '1':
		mode |= S_1;
		break;

	    case '2':
		mode |= S_2;
		break;

	    case '3':
		mode |= S_3;
		break;

	    case '4':
		mode |= S_4;
		break;

	    case '5':
		mode |= S_5;
		break;

	    case '6':
		mode |= S_6;
		break;

	    case '7':
		mode |= S_7;
		break;

	    case '\0':
		goto out;

	    default:
		goto mode_err;
	}

	/*
	 *	Interpreta a "permission" de acordo com "who"
	 */
    out:
	if (who & USER)
	{
		mode |= (permission & (S_IRWXU|S_ISUID));
	}

	if (who & GROUP)
	{
		mode |= ((permission & S_IRWXU) >> SHIFT);

		if (permission & S_ISUID)
			mode |= S_ISGID;
	}

	if (who & OTHERS)
	{
		mode |= ((permission & S_IRWXU) >> (2 * SHIFT));
	}

	if (dflag)
	{
		printf
		(	"op = '%c', mode = \"%s\" %8o  \"%s\"\n",
			op, modetostr (mode) + 2, mode, mode_nm
		);
	}

	if (actionp >= &action[ASZ])
		error ("$Foi dado um número excessivo de modos");

	actionp->op	= op;
	actionp->mode	= mode;

	actionp++;

}	/* end mode_op_conv */

/*
 ****************************************************************
 *	Troca o modo de um arquivo 				*
 ****************************************************************
 */
int
change_file_mode (const char *file_nm)
{
	ACTION		*ap;
	long		mode;
	long		text = 0;
	STAT		s;

	/*
	 *	Obtém o modo antigo
	 */
	if (stat (file_nm, &s) < 0)
	{
		error ("Não consegui obter o estado de \"%s\"", file_nm);
		return (1);
	}

	mode = s.st_mode;

	/*
	 *	Executa as várias ações
	 */
	for (ap = action; ap < actionp; ap++)
	{
		switch (ap->op)
		{
		    case '=':
			mode = ap->mode;
			break;

		    case '+':
			mode |= ap->mode;
			text |= ap->mode & (S_IMETX|S_ISVTX);
			break;

		    case '-':
			mode &= ~ap->mode;
			break;

		    default:
			error ("Operação inválida: '%c'\n", ap->op);

		}	/* end switch */

	}	/* end for */

	/*
	 *	Se o "sticky bit" está para a memória,
	 *	apaga o do swap, e vice-versa
	 */
	if   (text & S_IMETX)
		mode &= ~S_ISVTX;
	elif (text == S_ISVTX)
		mode &= ~S_IMETX;

	/*
	 *	Atribui o modo novo
	 */
	if (chmod (file_nm, mode) < 0)
	{
		error ("*Não consegui trocar o modo de \"%s\"", file_nm);
		return (1);
	}

	if (vflag)
	{
		printf ("\"%s\"  =>  ", modetostr (s.st_mode) + 2);
		printf ("\"%s\"\t%s\n", modetostr (mode) + 2, file_nm);
	}

	return (0);

}	/* end change_file_mode () */

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
		"%s - modifica a permissão de acesso de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vN] <modo> [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Lista os modos antigo e novo de cada <arquivo>\n"
		"\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados <arquivo>s\n"
	);
	fprintf
	(	stderr,
		"\n<modo>:"
		"\tAbsoluto:  um inteiro			(ex.: 0666)\n"
		"\tSimbólico: [<quem>] <op> [<permissão>]	(ex.: a+x)\n\n"
		"\tModos simbólicos múltiplos podem ser dados\n"
		"\tseparados por vírgulas			(ex.: u=rwx,g+x)\n" 
	);

	exit (2);

}	/* end help */
