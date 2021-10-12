/*
 ****************************************************************
 *								*
 *			ifdef.c					*
 *								*
 *	Realiza o préprocessamento apenas de "ifdef"		*
 *								*
 *	Versão	2.0.0, de 14.09.88				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: ifdef						*
 *		Utilitários Secretos				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ctype.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	LINESZ	512		/* Tamanho da linha */

#define	DEFSZ	20		/* No. máximo de definicões */
#define RESSZ	100		/* No. máximo de residuais */

#define elif	else if

entry const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

entry int		vflag;		/* Verbose */
entry int		rflag;		/* Variáveis residuais */
entry int		Nflag;		/* Nomes do <stdin> */

/*
 ******	Variáveis globais ***************************************
 */
entry char		buf[LINESZ];	/* Área de entrada/saída */

entry const char	*dir;		/* O último argumento é um diretório */

entry char		outnm_line[LINESZ]; /* Nomes de saída */

entry FILE		*infp, *outfp;	/* Ponteiros dos arquivos de entrada/saída */

entry int		defi;		/* No. de "defines" */

entry const char	*defvar[DEFSZ];	/* As variáveis definidas */

entry int		undefi;		/* No. de "undefs" */

entry const char	*undefvar[DEFSZ]; /* As variáveis indefinidas */

entry char		stack[100];	/* Stack de "ifdefs", etc ... */

entry char		*stackp = stack; /* Ponteiro do Stack */

entry int		falselevel;	/* No. de Condicionais FALSE aninhadas */

entry char		*resvar[RESSZ];	/* Variáveis residuais */

/*
 ******	Protótipos de funções ***********************************
 */
void		procfile (const char *, const char *);
int		metaline (const char *);
int		vargiven (const char *, char *, int);
void		resvarpr (void);
void		help (void);

/*
 ****************************************************************
 *	Programa de préprocessar "ifdef"			*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt;
	const char	*cp;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vrND:U:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'r':			/* Variáveis residuais */
			rflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'D':			/* Definições */
			if (defi >= DEFSZ)
				error ("$No momento somente aceitamos %d definições\n",	DEFSZ);

			defvar[defi++] = optarg;
			break;

		    case 'U':			/* Indefinições */
			if (undefi >= DEFSZ)
				error ("$No momento somente aceitamos %d indefinições\n", DEFSZ);

			undefvar[undefi++] = optarg;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();
		}

	}

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se o último argumento é um diretório
	 */
	if (argc != 0 && stat (argv[argc-1], &s) == 0 && S_ISDIR (s.st_mode))
		{ dir = argv[argc-1]; argv[argc-1] = NOSTR; }

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag)
	{
		if (argc != 1 || !dir)
			help ();

		/*
		 *	Le os argumentos do "stdin"
		 */
		while (fngets (buf, sizeof (buf), stdin) != NOSTR)
		{
			if ((cp = strrchr (buf, '/')) == NOSTR)
				cp = buf;
			else
				cp++;

			strcpy (outnm_line, dir);
			strcat (outnm_line, "/");
			strcat (outnm_line, cp);

			procfile (buf, outnm_line);
		}

		resvarpr ();

		exit (0);
	}

	/*
	 *	Foi dado o diretório
	 */
	if (dir)
	{
		if (argc < 2)
			help ();

		/*
		 *	Processa os argumentos dados
		 */
		for (/* vazio */; *argv; argv++)
		{
			if ((cp = strrchr (*argv, '/')) == NOSTR)
				cp = *argv;
			else
				cp++;

			strcpy (outnm_line, dir);
			strcat (outnm_line, "/");
			strcat (outnm_line, cp);

			procfile (*argv, outnm_line);
		}

		resvarpr ();

		exit (0);
	}

	/*
	 *	Processa o caso da saída padrão
	 */
	if (argc == 1)
	{
		procfile (argv[0], "*");

		resvarpr ();

		exit (0);
	}

	/*
	 *	Processa o caso de 2 arquivos
	 */
	if (argc == 2)
	{
		procfile (argv[0], argv[1]);

		resvarpr ();

		exit (0);
	}

	help ();

}	/* end ifdef */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
void
procfile (const char *innm, const char *outnm)
{
	if (vflag)
		fprintf (stderr, "%s:\n", innm);

	if ((infp = fopen (innm, "r")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\" (%s)\n", innm);
		return;
	}

	if   (*outnm == '*')
	{
		outfp = stdout;
	}
	elif ((outfp = fopen (outnm, "w")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\" (%s)\n", outnm);
		fclose (infp);
		return;
	}

	falselevel = 0; stackp = stack;

	while (fngets (buf, LINESZ, infp) == buf)
	{
		if   (buf[0] == '#')
		{
			if (metaline (innm))
				continue;
		}
		elif (falselevel)
		{
			continue;
		}

		fnputs (buf, outfp);
	}

	if (stackp != stack)
		error ("Faltaram \"endif's\" no arquivo \"%s\"", innm);

	fclose (infp);

	if (*outnm != '*')
		fclose (outfp);

}	/* end procfile */

/*
 ****************************************************************
 *	Analisa a diretiva de préprocessamento			*
 ****************************************************************
 */
int
metaline (const char *innm)
{
	char		*cp, *begin, *End;
	int		c;

	/*
	 *	Retorna:
	 *	  0    => Copia a linha
	 *	  <> 0 => Ignora a linha
	 */
	cp = buf + 1;

	while ((c = *cp) == ' ' || c == '\t')
		cp++;

	if   (strncmp (cp, "define", 6) == 0)
	{
		/*
		 *	"#define"
		 */
		cp += 6;

		while ((c = *cp) == ' ' || c == '\t')
			cp++;

		if ((c = *cp) != '_' && c != '$' && !isalnum (c))
		{
			fprintf
			(	stderr,
				"**** Nome de variável inválida\n"
			);
		}

		begin = cp;

		while ((c = *cp) == '_' || c == '$' || isalnum (c))
			cp++;

		End = cp;

		switch (vargiven (begin, End, 0 /* Não residual */))
		{
		    case -1:	/* Na lista de indefinidos */
			break;

		    case 1:	/* Na lista de definidos */
			break;

		    default:	/* Neutro: não está em nenhuma lista */
			return (falselevel);
		}

		/*
		 *	Elimina a linha seguinte, se é em branco
		 */
		if ((c = getc (infp)) != '\n' && c != EOF)
			ungetc (c, infp);

		return (1);
	}
	elif (strncmp (cp, "undef", 5) == 0)
	{
		/*
		 *	"#undef"
		 */
		cp += 5;

		while ((c = *cp) == ' ' || c == '\t')
			cp++;

		if ((c = *cp) != '_' && c != '$' && !isalnum (c))
		{
			fprintf
			(	stderr,
				"**** Nome de variável inválida\n"
			);
		}

		begin = cp;

		while ((c = *cp) == '_' || c == '$' || isalnum (c))
			cp++;

		End = cp;

		switch (vargiven (begin, End, 0 /* Não residual */))
		{
		    case -1:	/* Na lista de indefinidos */
			break;

		    case 1:	/* Na lista de definidos */
			break;

		    default:	/* Neutro: não está em nenhuma lista */
			return (falselevel);
		}

		/*
		 *	Elimina a linha seguinte, se é em branco
		 */
		if ((c = getc (infp)) != '\n' && c != EOF)
			ungetc (c, infp);

		return (1);
	}
	elif (strncmp (cp, "ifdef", 5) == 0)
	{
		/*
		 *	"#ifdef"
		 */
		cp += 5;

		while ((c = *cp) == ' ' || c == '\t')
			cp++;

		if ((c = *cp) != '_' && c != '$' && !isalnum (c))
		{
			fprintf
			(	stderr,
				"**** Nome de variável inválida\n"
			);
		}

		begin = cp;

		while ((c = *cp) == '_' || c == '$' || isalnum (c))
			cp++;

		End = cp;

		switch (vargiven (begin, End, 1 /* Residual */))
		{
		    case -1:	/* Na lista de indefinidos */
			*stackp++ = 'U';
			falselevel++;
			return (1);

		    case 1:	/* Na lista de definidos */
			*stackp++ = 'D';
			return (1);

		    default:	/* Neutro: não está em nenhuma lista */
			*stackp++ = 'N';
			return (falselevel);
		}
	}
	elif (strncmp (cp, "ifndef", 6) == 0)
	{
		/*
		 *	"#ifndef"
		 */
		cp += 6;

		while ((c = *cp) == ' ' || c == '\t')
			cp++;

		if ((c = *cp) != '_' && c != '$' && !isalnum (c))
		{
			fprintf
			(	stderr,
				"**** Nome de variável inválida\n"
			);
		}

		begin = cp;

		while ((c = *cp) == '_' || c == '$' || isalnum (c))
			cp++;

		End = cp;

		switch (vargiven (begin, End, 1 /* Residual */))
		{
		    case -1:	/* Na lista de indefinidos */
			*stackp++ = 'D';
			return (1);

		    case 1:	/* Na lista de definidos */
			*stackp++ = 'U';
			falselevel++;
			return (1);

		    default:	/* Neutro: não está em nenhuma lista */
			*stackp++ = 'N';
			return (falselevel);
		}
	}
	elif (strncmp (cp, "if", 2) == 0)
	{
		/*
		 *	"#if"
		 */
		cp += 2;

		while ((c = *cp) == ' ' || c == '\t')
			cp++;

		/*
		 *	Analisa o caso particular "#if (0)"
		 */
		if (strncmp (cp, "(0)", 3) == 0)
		{
			*stackp++ = 'U';
			falselevel++;
			return (1);
		}

		*stackp++ = 'N';
		return (falselevel);
	}
	elif (strncmp (cp, "else", 4) == 0)
	{
		/*
		 *	"#else"
		 */
		if (stackp <= stack)
		{
			fprintf
			(	stderr,
				"\"#else\" sem \"#if...\" no arquivo \"%s\"\n",
				innm
			);
		}

		switch (stackp[-1])
		{
		    case 'U':
			falselevel--;
			stackp[-1] = 'D';
			return (1);

		    case 'D':
			falselevel++;
			stackp[-1] = 'U';
			return (1);

		    default:	/* Neutro */
			return (falselevel);
		}
	}
	elif (strncmp (cp, "elif", 4) == 0)
	{
		/*
		 *	"#elif"
		 */
		if (stackp <= stack)
		{
			fprintf
			(	stderr,
				"\"#elif\" sem \"#if\" no arquivo \"%s\"\n",
				innm
			);
		}

		if (stackp[-1] != 'N')
		{
			fprintf
			(	stderr,
				"\"#elif\" associado a \"#ifdef\" "
				"no arquivo \"%s\"\n",
				innm
			);
		}

		return (falselevel);
	}
	elif (strncmp (cp, "endif", 5) == 0)
	{
		/*
		 *	"#endif"
		 */
		cp += 5;

		if (stackp <= stack)
		{
			fprintf
			(	stderr,
				"Excessivos \"endif's\" no arquivo \"%s\"\n",
				innm
			);
		}

		switch (*--stackp)
		{
		    case 'U':
			falselevel--;
			break;

		    case 'D':
			break;

		    default:	/* Neutro */
			return (falselevel);
		}

		/*
		 *	Elimina a linha seguinte, se é em branco
		 */
		if ((c = getc (infp)) != '\n' && c != EOF)
			ungetc (c, infp);

		return (1);
	}

	/*
	 *	Demais metacomandos
	 */
	return (falselevel);

}	/* end metaline */

/*
 ****************************************************************
 *	Verifica se a variável foi dada				*
 ****************************************************************
 */
int
vargiven (const char *begin, char *End, int flag)
{
	/*
	 *	Flag:
	 *	   0: Não residual
	 *	   1: Residual
	 *
	 *	Devolve:
	 *	  -1 => Indefinida
	 *	   0 => Variável não dada
	 *	  +1 => Definida
	 */
	int		i;
	const char	**cpp;
	char		**rpp;
	char		c;

	c = *End; *End = '\0';

	for (cpp = defvar, i = 0; i < defi; cpp++, i++)
	{
		if (streq (begin, *cpp))
			{ *End = c; return (1); }
	}

	for (cpp = undefvar, i = 0; i < undefi; cpp++, i++)
	{
		if (streq (begin, *cpp))
			{ *End = c; return (-1); }
	}

	/*
	 *	A variável não foi dada => é residual
	 */
	if (flag)
	{
		for (rpp = resvar; *rpp; rpp++)
		{
			if (streq (*rpp, begin))
				{ *End = c; return (0); }
		}

		if (rpp >= &resvar[RESSZ - 1])
			error ("$Tabela de variáveis residuais excedida");

		*rpp = strdup (begin);
	}

	*End = c; return (0);

}	/* end vargiven */

/*
 ****************************************************************
 *	Imprime as variáveis residuais				*
 ****************************************************************
 */
void
resvarpr (void)
{
	char		* const *rpp;
	int		i;

	if (!rflag)
		return;

	if (**resvar == 0)
	{
		fprintf (stderr, "Não há variáveis residuais");
	}
	else
	{
		fprintf (stderr, "\nVariáveis residuais:");

		for (rpp = resvar, i = 0; *rpp; rpp++, i++)
		{
			if (i % 5 == 0)
				fprintf (stderr, "\n     ");

			fprintf (stderr, " %-14.14s", *rpp);
		}
	}

	fprintf (stderr, "\n");

}	/* end resvarpr */

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
		"%s - realiza préprocessamentos parciais\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vr] [-D <var>] [-U <var>] <arquivo1> [<arquivo2]\n"
		"\t%s [-vr] [-D <var>] [-U <var>] <arquivo> ... <diretório>\n"
		"\t%s [-vr] [-D <var>] [-U <var>] -N <diretório>\n",
		pgname, pgversion, pgname, pgname, pgname, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-D: A variável \"var\" deve ser definida\n"
		"\t-U: A variável \"var\" deve ser indefinida\n"
		"\t-v: Verboso\n"
		"\t-r: Imprime uma lista de variáveis residuais\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
	);

	exit (2);

}	/* end help */
