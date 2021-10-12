/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Análise léxica do "tic"					*
 *								*
 *	Versão	2.0.0, de 21.04.87				*
 *		3.0.0, de 12.07.97				*
 *								*
 *	Módulo: tic						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>

#include	"../h/scan.h"
#include	"../h/error.h"

/*
 ****************************************************************
 *	Variáveis e funções externas				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL
#define	elif	else if

extern	int	nadvr;		/* Numero de advertencias */
extern	int	nerro;		/* Numero de erros */
extern	FILE	*input;		/* Arquivo de Entrada */

/*
 ****************************************************************
 *	Variáveis Exportadas pelo Analisador Lexico:		*
 ****************************************************************
 */
entry char	*line;		/* Aponta buffer com a linha */
entry TOKEN	token;		/* Estrutura de saida do scanner */
entry int	nline;		/* Numero da linha sendo analisada */
entry bool	lineprinted;	/* A linha já foi impressa */

/*
 ****************************************************************
 *	Variáveis Locais para o Analisador Lexico:		*
 ****************************************************************
 */
static char	linebuf[LINESZ];/* Buffer para a linha lida do disco */
static char	*nextp;		/* Ponteiro para o proximo caracter */

/*
 ****************************************************************
 *	Definição de Constantes					*
 ****************************************************************
 */
#define	ESC	0x1B	/* Caractere ISO Escape */
#define	NL	0x0A	/* Caractere ISO New Line */
#define	CR	0x0D	/* Caractere ISO Carriage Return */
#define	HT	0x09	/* Caractere ISO Horizontal Tabulation */
#define	BS	0x08	/* Caractere ISO Back Space */
#define	NP	0x0C	/* Caractere ISO New Page */

/*
 ******	Protótipos de funções ***********************************
 */
bool		readline (void);
int		get_cap (char *);
int		get_number (void);
bool		get_string (char []);
bool		get_names (char[]);
void		skip (bool);

/*
 ****************************************************************
 *	Leitura de uma linha do arquivo atual de entrada (in).	*
 ****************************************************************
 */
bool
readline (void)
{
	register int	c;
	register char	*lp;
	register FILE	*rin;
	register bool	toolong;
	register bool	escape;

	rin = input;
	toolong = false;
	escape = false;
	lp = line = linebuf;

	/*
	 *	Uma linha terminada por '\\' continua na seguinte.
	 */
	while ((c = getc (rin)) >= 0)
	{
		if (c == '\n')
		{
			if (escape)
			{
				lp--;
				nline++;
				continue;
			}
			else
			{
				break;
			}
		}

		if (lp < &linebuf[LINESZ-1])
			*lp++ = c;
		else
			toolong = true;

		escape = (c == '\\');
	}
	*lp = '\0';

	/*
	 *	Verifica Erros
	 */
	if (toolong)
	{
		tic_error (ADVR, NOMARK,
		"Linha muito Grande: Excedente será Ignorado", NOSTR);
	}
	elif (c != '\n'  &&  lp == linebuf)
	{
		return (false);
	}

	nextp = line;
	token.t_lineno = ++nline;
	lineprinted = false;

#ifdef	DEBUG
	printf ("Li: \"%s\"\n", line);
#endif	DEBUG

	return (true);

}	/* end readline */

/*
 ****************************************************************
 *	Leitura do Nome de um Capacidade			*
 ****************************************************************
 */
int
get_cap (char *cap)
{
	register char	*cp;
	char		c[16];

	/*
	 *	Pula Espaços
	 */
	while (isspace (*nextp))
		nextp++;

	/*
	 *	Pula linhas em branco, Comentarios e Espaços
	 */
	while (!*nextp || *nextp == '#')
	{
		if (!readline ())
			return (0);

		while (isspace (*nextp))
			nextp++;
	}

	/*
	 *	Inícío do Token
	 */
	token.t_begin = nextp;
	token.t_end = nextp;
	token.t_lineno = nline;

	/*
	 *	Apenas os Nomes do Terminal podem vir no início de uma linha.
	 */
	if (nextp == line)
	{
		tic_error
		(	ERRO, LMARK,
			"Apenas os Nomes do Terminal "
			"podem vir no Início da linha",
			NOSTR
		);
		return (-1);
	}

	/*
	 *	Verifica a existência de algum nome.
	 */
	if (!isalpha (*nextp))
	{
		if (*nextp == ',')
		    tic_error (ADVR, SMARK, "Não está faltando um nome ?", NOSTR);
		else
		    tic_error (ERRO, LMARK, "Nome inválido para uma Capacidade", NOSTR);
		return (-1);
	}

	/*
	 *	Copia Nome para cap.
	 */
	cp = c;

	while (isalnum (*nextp))
	{
		if (cp - c >= IDSZ)
		{
			tic_error
			(	ADVR, NOMARK,
				"Nome de Capacidade muito grande", NOSTR
			);
			return (1);
		}
		*cp++ = *nextp++;
	}

	*cp = '\0';

	strscpy (cap, c, 16);

	token.t_end = nextp - 1;

#ifdef	DEBUG
	printf ("get_cap: '%s'\n", cap);
#endif	DEBUG

	return (1);

}	/* end get_cap */

/*
 ****************************************************************
 *	Leitura de um Numero (decimal)				*
 ****************************************************************
 */
int
get_number (void)
{
	register int	number;

	token.t_begin = token.t_end = nextp;
	token.t_lineno = nline;

	/*
	 *	Pula caractere '#'
	 */
	if (*nextp != '#')
	{
		tic_error (ADVR, LMARK, "Espero \"#\"", NOSTR);
		skip (false);
		return (false);
	}
	token.t_begin = ++nextp;

	/*
	 *	Calcula o número (em decimal)
	 */
	number = 0;

	while (isdigit (*nextp))
	{
		number = number * 10 + *nextp - '0';
		nextp++;
	}

	token.t_end = nextp - 1;

	/*
	 *	Pula vírgula
	 */
	skip (true);

	return (number);

}	/* end get_number */

/*
 ****************************************************************
 *	Leitura de uma Cadeia (string)				*
 ****************************************************************
 */
bool
get_string (char string[])
{
	register char	*s;
	register char	*error_msg;

	/*
	 *	Início do Token
	 */
	token.t_begin = nextp;
	token.t_end = nextp;
	token.t_lineno = nline;

	/*
	 *	Pula o caractere '='
	 */
	if (*nextp != '=')
	{
		error_msg = "Espero '='";
		goto str_error;
	}
	token.t_begin = ++nextp;

	/*
	 *	Copia a cadeia (até vírgula e pulando espaço) para string
	 */
	s = &string[0];

	while (*nextp && *nextp != ',')
	{
		token.t_end = nextp;

		if (s - string > STRSZ)
		{
			error_msg = "Cadeia muito longa";
			goto str_error;
		}

		if   (*nextp == '^')
		{
			/*
			 *	Caracteres de Controle
			 */
			nextp++;

			if (*nextp >= 0x40 && *nextp < 0x60 )
			{
				*s++ = *nextp & (~0x40);
			}
			else
			{
				error_msg = "Espero letra maiuscula";
				goto str_error;
			}
		}
		elif (*nextp == '\\')
		{
			/*
			 *	Caracteres "Escapados"
			 */
			switch (*++nextp)
			{
			    case 'E':
			    case 'e':
				*s++ = ESC;
				break;

			    case 'n':
			    case 'l':
				*s++ = NL;
				break;

			    case 'r':
				*s++ = CR;
				break;

			    case 't':
				*s++ = HT;
				break;

			    case 'b':
				*s++ = BS;
				break;

			    case 'f':
				*s++ = NP;
				break;

			    case 's':
				*s++ = ' ';
				break;

			    case '0':
				*s++ = 0x40;
				break;

			    default:
				*s++ = *nextp;
			}
		}
		elif (!isspace (*nextp))
		{
			/*
			 *	Outros Caracteres (menos espaço !)
			 */
			*s++ = *nextp;
		}

		nextp++;

	}	/* *nextp && *nextp != ',' */

	*s = '\0';

	token.t_end = nextp - 1;
	skip (true);

	return (true);

	/*
	 *	Mensagens de Erro
	 */
    str_error:
	token.t_end = nextp;

	tic_error (ERRO, LMARK, error_msg, NOSTR);

	/*
	 *	Pula até vírgula
	 */
	skip (false);

	return (false);

}	/* end get_string */

/*
 ****************************************************************
 *	Leitura dos Nomes do Terminal				*
 ****************************************************************
 */
bool	
get_names (char names[])
{
	register char	*end;

	/*
	 *	Início do Token
	 */
	token.t_begin = nextp;
	token.t_end = nextp;
	token.t_lineno = nline;

	/*
	 *	Se necessário, realiza primeira leitura
	 */
	if (nextp == NOSTR)
	{
		if (!readline ())
			return (false);
	}

	/*
	 *	Pula linhas em branco, Comentarios e Espaços
	 */
	while (!nextp || *nextp == '#')
	{
		if (!readline ())
			return (false);

		while (isspace (*nextp))
			nextp++;
	}

	/*
	 *	Os Nomes Terminal devem vir no início de uma linha.
	 */
	if (nextp != line)
		return (false);

	/*
	 *	Copia Nomes (até vírgula) para names
	 */
	if ((end = strchr (line, ',')) == NOSTR)
	{
		tic_error (ADVR, NOMARK, "Faltando vírgula", NOSTR);
		strcpy (names, line);
		nextp = (char *)"";
	}
	else
	{
		strncpy (names, line, end - line);
		nextp = end + 1;
	}

	token.t_end = nextp;

	return (true);

}	/* end get_names */

/*
 ****************************************************************
 *	Pula até vírgula					*
 ****************************************************************
 */
void
skip (register bool only_space)
{
	if (only_space)
	{
		while (isspace(*nextp))
			nextp++;
	}
	else
	{
		while (*nextp && *nextp != ',')
		{
			if (*nextp == '\\' && *(nextp + 1))
				nextp++;
			nextp++;
		}
	}

	if (*nextp == ',')
		nextp++;
	else
		tic_error (ERRO, LMARK, "Espero vírgula", NOSTR);

}	/* end skip */
