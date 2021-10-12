/*
 ****************************************************************
 *								*
 *			rm.c					*
 *								*
 *	Remove arquivos						*
 *								*
 *	Versão	1.0.0, de 13.05.86				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: rm						*
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
#include <unistd.h>
#include <stat.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 02.05.02";

entry int	vflag;		/* Verbose */
entry int	fflag;		/* opção '-f' */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Protótipos de funções ***********************************
 */
int		remove (const char *);
void		help (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, ret = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "fivNH")) != EOF)
	{
		switch (opt)
		{
		    case 'f':			/* Força */
			fflag++;
			break;

		    case 'i':			/* Interativo */
			fflag = 0;
			break;

		    case 'v':			/* Verbose */
			vflag++;
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

	/*
	 *	Prepara o terminal para consultas
	 */
	if (!fflag)
	{
		if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
			error ("$*Não consegui abrir \"/dev/tty\"");

		askyesno_set (1);	/* Permite o modo "de força" */
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stderr)), (void *)NULL, &color_vector);

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
		{ error ("Os argumentos supérfluos serão ignorados"); *argv = NOSTR; }

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR)		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
			ret += remove (area);
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
			ret += remove (*argv);
	}

	return (ret);

}	/* end main */	

/*
 ****************************************************************
 *	Analisa a  remoção					*
 ****************************************************************
 */
int
remove (const char *file_nm)
{
	STAT		s;
	int		code, save_errno;

	/*
	 *	Obtém o estado da fonte
	 */
	s.st_mode = 0;

	code = lstat (file_nm, &s);

	save_errno = errno;

	/*
	 *	Normalmente, questiona o usuário sobre a remoção
	 */
	if (!fflag || vflag)
	{
		fprintf (stderr, "%c", modetostr (s.st_mode)[0]);

		if (S_ISREG (s.st_mode))
			fprintf (stderr, " %d", s.st_size);

		fprintf
		(	stderr,
			" %s%s%s",
			color_vector[(s.st_mode & S_IFMT) >> 12],
			file_nm,
			color_vector[MC_DEF]
		);

		if (code >= 0 && !fflag)
		{
			fprintf (stderr, "? (n): ");

			if (askyesno () <= 0)
				return (0);
		}
		else
		{
			fprintf (stderr, "\n");
		}
	}

	if (code < 0)
	{
		errno = save_errno;

		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		return (1);
	}

	if (S_ISDIR (s.st_mode))
	{
		error ("\"%s\" é um diretório", file_nm);
		return (1);
	}

	/*
	 *	Verifica possibilidade de remoção
	 */
	if (!fflag && !S_ISLNK (s.st_mode) && access (file_nm, W_OK) < 0)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" não tem permissão de escrita. Remove? (n): ",
			pgname, file_nm
		);

		if (askyesno () <= 0)
			return (0);
	}

	/*
	 *	Remove
	 */
	if (unlink (file_nm) < 0)
		{ error ("*Não consegui remover \"%s\"", file_nm); return (1); }

	if (s.st_nlink-- > 1)
		error ("Ainda há %d elo(s) físico(s) para \"%s\"", s.st_nlink, file_nm);

	return (0);

}	/* end remove */

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
		"%s - Remove arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-ifvN] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: Pede confirmação do usuário para a remoção "
			"de cada <arquivo>\n"
		"\t    (modo normal)\n"
		"\t-f: Remove o <arquivo> sem pedir confirmação do usuário\n"
		"\t-v: Verboso\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados <arquivo>s\n"
	);

	exit (2);

}	/* end help */
