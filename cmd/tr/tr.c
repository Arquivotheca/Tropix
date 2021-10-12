/*
 ****************************************************************
 *								*
 *			tr.c					*
 *								*
 *	Traduz caracteres					*
 *								*
 *	Versão	1.0.0, de 10.09.85				*
 *		3.0.0, de 12.07.97				*
 *								*
 *	Módulo: tr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 12.07.97";

#define	ISO_SZ	256

#define	elif	else if

entry int	cflag;		/* Complementa */
entry int	dflag;		/* Remove */
entry int	sflag;		/* Compacta */
entry int	vflag;		/* Verbose */

entry const char *nextp;	/* Usado com get_escape */

/*
 ****** Protótipos de funções ***********************************
 */
const char	*escape_interval_proc (const char *);
int		get_escape (const char *);
int		COCTAL (const char *);
int		CHEX (const char *);
int		CINDEX (const char *);
const char	*complement_set (const char *);
const char	*translate_prolog (const char *, const char *);
const char	*delete_prolog (const char *);
const char	*squeeze_prolog (const char *);
void		translate_exec (const char *);
void		translate_squeeze_exec (const char *, const char *);
void		delete_exec (const char *);
void		delete_squeeze_exec (const char *, const char *);
void		help (void);

/*
 ****************************************************************
 *	Traduz caracteres					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register const char	*str_1 = NOSTR, *str_2 = NOSTR;
	const char		*translate_table, *delete_table, *squeeze_table;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "cds1:2:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'c':			/* Complementa */
			cflag++;
			break;

		    case 'd':			/* Remove */
			dflag = 1;
			break;

		    case 's':			/* Compacta */
			sflag = 1;
			break;

		    case '1':			/* <cadeia1> */
			str_1 = optarg;
			break;

		    case '2':			/* <cadeia2> */
			str_2 = optarg;
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

	/*
	 *	Acerta para os argumentos
	 */
	argv += optind;
	argc -= optind;

	/*
	 *	Abre o arquivo de entrada
	 */
	if (argc > 0  &&  !streq (argv[0], "-"))
	{
		if (freopen (argv[0], "r", stdin) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[0], strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Abre o arquivo de saída
	 */
	if (argc > 1)
	{
		if (freopen (argv[1], "w", stdout) == NOFILE)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, argv[1], strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Processa o "[...]" e "\..."
	 */
	str_1 = escape_interval_proc (str_1);
	str_2 = escape_interval_proc (str_2);

	/*
	 *	Acha o complemento da <cadeia1>, se for o caso
	 */
	if (cflag)
		str_1 = complement_set (str_1);

	/*
	 *	Executa a função adequada
	 */
	switch (2 * dflag + sflag)
	{
		/*
		 *	Tradução
		 */
	    case 0:
		translate_table = translate_prolog (str_1, str_2);
		translate_exec (translate_table);
		break;

		/*
		 *	Tradução + Compactação
		 */
	    case 1:
		translate_table = translate_prolog (str_1, str_2);
		squeeze_table = squeeze_prolog (str_2);
		translate_squeeze_exec (translate_table, squeeze_table);
		break;

		/*
		 *	Remoção
		 */
	    case 2:
		delete_table = delete_prolog (str_1);
		delete_exec (delete_table);
		break;

		/*
		 *	Remoção + compactação
		 */
	    case 3:
		delete_table = delete_prolog (str_1);
		squeeze_table = squeeze_prolog (str_2);
		delete_squeeze_exec (delete_table, squeeze_table);
		break;

	}	/* end switch */

	/*
	 *	Encerra a operação
	 */
	if (ferror (stdin))
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da <entrada> (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	fflush (stdout);

	if (ferror (stdout))
	{
		fprintf
		(	stderr,
			"%s: Erro de escrita na <saída> (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end tr */

/*
 ****************************************************************
 *	Expande construções do tipo "[..]" e "\..."		*
 ****************************************************************
 */
const char *
escape_interval_proc (const char *str)
{
	register char		*list;
	register int		c, begin, End;
	register const char	*cp, *ep;
	register char		*lp;
	const char		*point;

	if (str == NOSTR)
		return (NOSTR);

	if (strchr (str, '[') == NOSTR && strchr (str, '\\') == NOSTR)
		return (str);

	if ((list = malloc (ISO_SZ + 1)) == NOSTR)
		{ fprintf (stderr, "%s: Memória esgotada\n", pgname); exit (1); }

	ep = list + ISO_SZ;

	for (lp = list, cp = str; lp < ep && (c = *cp++); /* no teste */)
	{
		switch (c)
		{
		    default: 
			*lp++ = c;
			break;

		    case '\\':
			*lp++ = get_escape (cp);	cp = nextp;
			break;

		    case '[':
			if ((begin = *cp++) == '\0')
				goto bad;

			if (begin == '\\')
				{ begin = get_escape (cp); cp = nextp; }

			switch (*cp++)
			{
			    default: 
				goto bad;
	
			    case '-':
				if ((End = *cp++) == '\0')
					goto bad;

				if (End == '\\')
					{ End = get_escape (cp); cp = nextp; }

				if (*cp++ != ']')
					goto bad;

				if (begin > End)
					goto bad;

				for (c = begin; c <= End && lp < ep; c++)
					*lp++ = c;
				break;

			    case '*':
				{ End = strtol (cp, &point, 0);	cp = point; }

				if (*cp++ != ']')
					goto bad;

				if (End == 0)
					End = ISO_SZ;

				while (End-- > 0 && lp < ep)
					*lp++ = begin;

				break;

			}	/* end switch */

		}	/* end switch */

	}	/* end for */

	*lp = '\0';

	return (list);

	/*
	 *	Construção inválida
	 */
    bad:
	fprintf
	(	stderr,
		"%s: A cadeia \"%s\" contém uma construção inválida\n",
		pgname, str
	);

	exit (1);

	return (list);		/* To make "lint" happy */

}	/* end escape_interval_proc */

/*
 ****************************************************************
 *	Analisa Sequencias de Escape				*
 ****************************************************************
 */
int
get_escape (const char *lp)
{
	register int	c;

	/*
	 *	Número octal
	 */
	if (COCTAL (lp))
	{
		c = CINDEX (lp++);

		if (COCTAL (lp))
		{
			c = (c << 3) + CINDEX (lp++);

			if (COCTAL (lp))
				c = ((c << 3) + CINDEX (lp++)) & 0xFF;
		}

		nextp = lp;

		return (c);
	}

	/*
	 *	Não é octal
	 */
	switch (*lp++)
	{
	    case 'a':
		c = 0x07;
		break;

	    case 'b':
		c = '\b';
		break;

	    case 'e':
		c = 0x1B;
		break;

	    case 'f':
		c = '\f';
		break;

	    case 'n':
		c = '\n';
		break;

	    case 'r':
		c = '\r';
		break;

	    case 't':
		c = '\t';
		break;

	    case 'v':
		c = 0x0B;
		break;

	    case 'x':
		if (CHEX (lp))
		{
			c = CINDEX (lp++);

			if (CHEX (lp))
			{
				c = (c << 4) + CINDEX (lp++);

				if (CHEX (lp))
					c = ((c << 4) + CINDEX (lp++)) & 0xFF;
			}

			nextp = lp;

			return (c);
		}
		
		/* cai através */

	    default:
		c = lp[-1];
		break;
	}

	nextp = lp;

	return (c);

}	/* end get_escape */

/*
 ****************************************************************
 *	Verifica se um algarismo é octal			*
 ****************************************************************
 */
int
COCTAL (register const char *lp)
{
	if (*lp >= '0' && *lp <= '7')
		return (1);

	return (0);

}	/* end COCTAL */

/*
 ****************************************************************
 *	Verifica se um algarismo é hexadecimal			*
 ****************************************************************
 */
int
CHEX (const char *lp)
{
	register char	c;

	if ((c = *lp) >= '0' && c <= '9')
		return (1);

	if (c >= 'a' && c <= 'f')
		return (1);

	if (c >= 'A' && c <= 'F')
		return (1);

	return (0);

}	/* end CHEX */

/*
 ****************************************************************
 *	Obtém o valor de um dígito				*
 ****************************************************************
 */
int
CINDEX (const char *lp)
{
	register char	c;

	if ((c = *lp) >= '0' && c <= '9')
		return (c - '0');

	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);

	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);

	return (0);

}	/* end CINDEX */

/*
 ****************************************************************
 *	Acha o complemento de uma cadeia			*
 ****************************************************************
 */
const char *
complement_set (const char *str_1)
{
	register char		*list;
	register int		i;
	register const char	*cp;
	register char		*lp;
	char			original_set[ISO_SZ];

	if ((list = malloc (ISO_SZ)) == NOSTR)
		{ fprintf (stderr, "%s: Memória esgotada\n", pgname); exit (1); }

	memset (original_set, 0, ISO_SZ);

	if (str_1 != NOSTR) for (cp = str_1; i = *cp++; /* no teste */)
		original_set[i] = 1;

	for (i = 1, cp = original_set + 1, lp = list; i < ISO_SZ; i++)
	{
		if (*cp++ == 0)
			*lp++ = i; 
	}

	*lp = '\0'; 

	return (list);

}	/* end complement_set */

/*
 ****************************************************************
 *	Prepara a tabela de tradução				*
 ****************************************************************
 */
const char *
translate_prolog (const char *str_1, const char *str_2)
{
	register int		i, c;
	register char		*table, *cp;
	register const char	*ep, *Cp;

	if ((table = malloc (ISO_SZ)) == NOSTR)
		{ fprintf (stderr, "%s: Memória esgotada\n", pgname); exit (1); }

	for (i = 0, cp = table; i < ISO_SZ; i++)
		*cp++ = i; 

	if (str_1 == NOSTR || str_2 == NOSTR)
		return (table);

	for (Cp = str_1, ep = str_2; (i = *Cp++) && (c = *ep++); /* no teste */)
		table[i] = c;

	return (table);

}	/* end translate_prolog */

/*
 ****************************************************************
 *	Prepara a tabela de remoção				*
 ****************************************************************
 */
const char *
delete_prolog (const char *str_1)
{
	register char		*table;
	register int		i;
	register const char	*cp;

	if ((table = malloc (ISO_SZ)) == NOSTR)
		{ fprintf (stderr, "%s: Memória esgotada\n", pgname); exit (1); }

	memset (table, 0, ISO_SZ);

	if (str_1 == NOSTR)
		return (table);

	for (cp = str_1; i = *cp++; /* no teste */)
		table[i] = 1;

	return (table);

}	/* end delete_prolog */

/*
 ****************************************************************
 *	Prepara a tabela de compactação				*
 ****************************************************************
 */
const char *
squeeze_prolog (const char *str_2)
{
	register int		i;
	register char		*table;
	register const char	*cp;

	if ((table = malloc (ISO_SZ)) == NOSTR)
		{ fprintf (stderr, "%s: Memória esgotada\n", pgname); exit (1); }

	memset (table, 0, ISO_SZ);

	if (str_2 == NOSTR)
		return (table);

	for (cp = str_2; i = *cp++; /* no teste */)
		table[i] = 1;

	return (table);

}	/* end squeeze_prolog */

/*
 ****************************************************************
 *	Apenas traduz						*
 ****************************************************************
 */
void
translate_exec (register const char *translate_table)
{
	register int	c;
	register FILE	*in_fp = stdin, *out_fp = stdout;

	while ((c = getc (in_fp)) >= 0)	/* != EOF */
	{
		c = translate_table[c];

		putc (c, out_fp);
	}

}	/* end translate_exec */

/*
 ****************************************************************
 *	Traduz e compacta					*
 ****************************************************************
 */
void
translate_squeeze_exec (const char *translate_table, const char *squeeze_table)
{
	register int	c, old_c = EOF;
	register FILE	*in_fp = stdin, *out_fp = stdout;

	while ((c = getc (in_fp)) >= 0)	/* != EOF */
	{
		c = translate_table[c];

		if (c != old_c)
		{
			putc (c, out_fp);

			if (squeeze_table[c])
				old_c = c;
			else
				old_c = EOF;
		}
	}

}	/* end translate_squeeze_exec */

/*
 ****************************************************************
 *	Apenas remove						*
 ****************************************************************
 */
void
delete_exec (const char *delete_table)
{
	register int	c;
	register FILE	*in_fp = stdin, *out_fp = stdout;

	while ((c = getc (in_fp)) >= 0)	/* != EOF */
	{
		if (!delete_table[c])
			putc (c, out_fp);
	}

}	/* end delete_exec */

/*
 ****************************************************************
 *	Remove e compacta					*
 ****************************************************************
 */
void
delete_squeeze_exec (const char *delete_table, const char *squeeze_table)
{
	register int	c, old_c = EOF;
	register FILE	*in_fp = stdin, *out_fp = stdout;

	while ((c = getc (in_fp)) >= 0)	/* != EOF */
	{
		if (delete_table[c])
			continue;

		if (c != old_c)
		{
			putc (c, out_fp);

			if (squeeze_table[c])
				old_c = c;
			else
				old_c = EOF;
		}
	}

}	/* end delete_squeeze_exec */

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
		"%s - traduz caracteres\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-cds] [-1 <cadeia1>] [-2 <cadeia2>] [<entrada> [<saída>]] \n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-c: Complementa a <cadeia1>\n"
		"\t-d: Remove os caracteres da <entrada> presentes na <cadeia1>\n"
		"\t-s: Compacta os caracteres da <saída> presentes na <cadeia2>\n"
		"\nObs.:\tSe <entrada> for \"-\", será lida a entrada padrão\n"
	);

	exit (2);

}	/* end help */
