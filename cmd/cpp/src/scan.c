/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
 *								*
 *	Versão	1.0.0, de 23.08.88				*
 *		4.1.0, de 22.08.01				*
 *								*
 *	Modulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
#define	COCTAL(lp)	(*lp >= '0' && *lp <= '7')

#define	CHEX(lp)	(categ[*lp] == DIGIT ||		\
			(*lp >= 'a' && *lp <= 'f') ||	\
			(*lp >= 'A' && *lp <= 'F'))

extern const char	categ[],	/* Tabela de categorias */
			cindex[];	/* Tabela de índices */

extern jmp_buf		eof_env;	/* Ponto de EOF */

/*
 ******	Variáveis do Analisador Léxico **************************
 */
entry TOKEN		token;		/* Token Corrente */

entry const char	*program,	/* Programa a ser Processado */
			*endprogram,	/* Fim do Programa */
			*nextp;		/* Ponteiro para o proximo caractere */

entry int		lineno,		/* Número da Linha Corrente */
			lineout;	/* Número da Linha na Saída */

entry int		eof_state;	/* O que fazer com EOF: 1 => pula */

/*
 ******	Macros locais *******************************************
 */
#define	RETURN1(m)	{ token.l_major = (m); return (m); }

#define	RETURN2(m,n)	{ token.l_minor = (n); \
			  token.l_major = (m); \
			  return (m); }

/*
 ******	Símbolos Unários (de um só caractere) *******************
 */
const UNSYM	unsym[]	=
{
	OP,	N_MOD,		/* % */
	LP,	N_NULL,		/* ( */
	RP,	N_NULL,		/* ) */
	OP,	N_MUL,		/* * */
	OP,	N_PLUS,		/* + */
	CM,	N_NULL,		/* , */
	OP,	N_MINUS,	/* - */
	OP,	N_XOR,		/* ^ */
	OP,	N_NOT		/* ~ */
};

/*
 ****************************************************************
 *	Ponto de Entrada do Analisador Léxico			*
 ****************************************************************
 */
MAJOR
scan (void)
{
	const UNSYM	*up;
	const char	*lp;

	/*
	 *	Pula os separadores iniciais
	 */
    again:
	lp = nextp;

	while (CATEG (lp) == SEPAR)	/* Não pula <nl> */
		lp++;

	/*
	 *	Analisa a categoria do caractere
	 */
	token.l_begin = token.l_end = lp;
	token.l_line  = lineno;
	nextp = lp + 1;

	switch (CATEG (lp))
	{
	    case INV:
		if (*lp == '\0')
			longjmp (eof_env, 1);

		err_msg
		(
			ADVR, MYMARK,
			"Caractere Inválido: \"%02X\", será ignorado",
			*lp, lp, lp
		);

		goto again;

	    case LETTER:
		return (getid ());

	    case DIGIT:
		return (getnum (lp));

	    case QUOTE:
		return (getstr ());

	    case APOST:
		return (getapost ());

		/*
		 *	Símbolos Unários
		 */
	    case USYM:
		up = &unsym[CINDEX (lp)];
		RETURN2 (up->u_major, up->u_minor);

		/*
		 *	Símbolos Binários
		 */
	    case BSYM:
		switch (CINDEX (lp))
		{
		    case 0:			/* <nl> */
			if (eof_state)
			{
				if (lp >= program && lp < endprogram)
					lineno++;
				goto again;
			}

			nextp--;
			RETURN1 (EOL);

		    case 1:			/* '!' */
			if (*nextp == '=')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_NE);
			}
			RETURN2 (OP, N_NEG);

		    case 2:			/* '&' */
			if (*nextp == '&')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_ANDAND);
			}
			RETURN2 (OP, N_AND);

		    case 3:			/* '/' */
			if (*nextp == '*')
			{
				skipcomment ();
				goto again;
			}

			RETURN2 (OP, N_DIV);

		    case 4:			/* '<' */
			if (*nextp == '<')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_LEFT);
			}
			if (*nextp == '=')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_LE);
			}
			RETURN2 (OP, N_LT);

		    case 5:			/* '=' */
			if (*nextp == '=')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_EQ);
			}
			else
			{
				err_msg
				(
					ADVR, MYMARK,
					"Será assumido operador '=='",
					0, lp, lp
				);
			}
			RETURN2 (OP, N_EQ);

		    case 6:			/* '>' */
			if (*nextp == '>')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_RIGHT);
			}
			if (*nextp == '=')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_GE);
			}
			RETURN2 (OP, N_GT);

		    case 7:			/* '|' */
			if (*nextp == '|')
			{
				token.l_end++;
				nextp++;
				RETURN2 (OP, N_OROR);
			}
			RETURN2 (OP, N_OR);

		    case 8:			/* '#' */
			if (*nextp == '#')
			{
				token.l_end++;
				nextp++;
				RETURN1 (GLUEOP);
			}
			RETURN1 (STROP);

		}	/* end case BSYM */

	    default:
		err_msg
		(
			COMP, NOMARK,
			"Categoria Inválida: %02X",
			CATEG (lp)
		);
		goto again;

	}	/* end switch (CATEG (lp)) */

}	/* end scan */

/*
 ****************************************************************
 *	Analisa Identificadores					*
 ****************************************************************
 */
MAJOR
getid (void)
{
	char		*lp;
	const char	*bp;
	char		c;
	int		new_area_len, id_len;
	static char	*area;
	static int	area_len;

	/*
	 *	Examina o identificador
	 */
	bp = lp = (char *)token.l_begin;

	do { lp++; } while ((c = CATEG (lp)) == LETTER || c == DIGIT);

	c = *lp; *lp = '\0'; id_len = lp - bp;

	/*
	 *	Verifica se a área é de bom tamanho
	 */
	if (area_len < id_len)
	{
		if ((new_area_len = id_len + 16) < 63)
			new_area_len = 63;

		if ((area = realloc (area, new_area_len + 1)) == NOSTR)
			error (NOSTR);

		token.l_name = area;

		area_len = new_area_len;
	}

	memmove (area, bp, id_len + 1);

   /***	token.l_name   = area; ***/
	token.l_nm_len = id_len;

	/*
	 *	Epílogo
	 */
	*lp = c;

	nextp = lp--;
	token.l_end = lp;

	RETURN1 (ID);

}	/* end getid */
 
/*
 ****************************************************************
 *	Analisa Cadeias						*
 ****************************************************************
 */
MAJOR
getstr (void)
{
	const char	*lp;
	char		c;

	lp = nextp;

	while ((c = *lp) != '"' && c != '\n')
	{
		if (c == '\\')
		{
			if (*++lp == '\n')
			{
				if (lp >= program && lp < endprogram)
					lineno++;
			}
		}

		lp++;
	}

	if (c == '\n')
	{
		lp--;
		token.l_end = lp;
		err_msg (ERRO, TOKMARK, "Cadeia não terminada");
	}

	token.l_end = lp++;
	nextp = lp;

	RETURN1 (STR);

}	/* end getstr */

/*
 ****************************************************************
 *	Analisa Caracteres 'xwyz'				*
 ****************************************************************
 */
MAJOR
getapost (void)
{
	const char	*lp;
	int		i;
	unsigned	c, seq;
	int		errtype;

	errtype = 0;

	seq = 0;
	lp = nextp;
	token.l_begin++;
	i = 0;

	/*
	 *	Supondo 4 bytes por inteiro
	 */
	while ((c = *lp++) != '\'' && c != '\n')
	{
		if (c == '\\')
		{
			c = getescape (lp);
			lp = nextp;
		}

		if (i++ < 4)
			seq = (seq << 8) | c;
		else
			errtype = 1;
	}

	if (c == '\n')
		errtype = 2;

	/*
	 *	c == '\'' ou '\n'
	 */
	token.l_ival = seq;

	nextp = lp;
	token.l_end = lp - 2;
	
	if   (errtype == 1)
	{
		err_msg (ADVR, TOKMARK, "Constante de caracteres muito longa");
	}
	elif (errtype == 2)
	{
		err_msg (ADVR, TOKMARK, "Constante de caracteres não terminada");
		nextp--;
	}

	RETURN1 (ICTE);

}	/* end getapost */

/*
 ****************************************************************
 *	Analisa Sequencias de Escape				*
 ****************************************************************
 */
int
getescape (const char *lp)
{
	int		c;

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

}	/* end getescape */

/*
 ****************************************************************
 *	Analisa Constantes					*
 ****************************************************************
 */
MAJOR
getnum (const char *lp)
{
	char		c;
	const char	*cp;

	token.l_ival = strtol (lp, &cp, 0);

	lp = cp;

	if ((c = lp[0]) == 'L' || c == 'l' || c == 'U' || c == 'u')
		lp++;

	nextp = lp--;
	token.l_end = lp;

	RETURN1 (ICTE);

}	/* end getnum */

/*
 ****************************************************************
 *	Pula Comentários					*
 ****************************************************************
 */
void
skipcomment (void)
{
	const char	*lp;
	char		c;

	lp = nextp + 1;

	do
	{
		if   ((c = *lp++) == '\n')
		{
			if (lp >= program && lp < endprogram)
				lineno++;
		}
		elif (c == '\0')
		{
			longjmp (eof_env, 1);
		}

	}
	while (c != '*' || *lp != '/');

	nextp = ++lp;

}	/* end skipcomment */

/*
 ****************************************************************
 *	Ignora até o Final da Linha				*
 ****************************************************************
 */
void
skipline (void)
{
	const char	*lp;
	char		c;

	/*
	 *	Esta rotina só é chamada para linhas originais dos fontes
	 */
	for (lp = nextp; /* vazio */; /* vazio */)
	{
		if   ((c = *lp++) == '\n')
			{ lineno++; break; }
		elif (c == '\\' && *lp == '\n')
			{ lineno++; lp++; putoutnl (); }
		elif (c == '/' && *lp == '*')
			{ nextp = lp; skipcomment (); lp = nextp; }

	}	/* end for */

	nextp = lp;

}	/* end skipline */

/*
 ****************************************************************
 *	Lê o Arquivo a Processar				*
 ****************************************************************
 */
int
readfile (const char *name)
{
	int		fd, sz, read_sz;
	char		*cp;
	STAT		st;

	if ((fd = open (name, O_RDONLY)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", name);
		return (-1);
	}

	if (fstat (fd, &st) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", name);
		return (-1);
	}

	sz = st.st_size;

	/*
	 *	Lê o programa
	 */
	if ((program = malloc (sz + 7)) == NOSTR)
		error (NOSTR);

	program += 3; *(char *)program++ = '\n';

	cp = (char *)program + sz - 1;

	if ((read_sz = read (fd, (void *)program, sz)) != sz)
	{
		error ("*Erro na leitura do arquivo \"%s\" (%d :: %d)", name, read_sz, sz);
		close (fd);
		return (-1);
	}

	close (fd);

	/*
	 *	Arremata o Programa.
	 */
	if (*cp != '\n')
		*++cp = '\n';
	
	*++cp = '\0';
	endprogram = cp;

	nextp = program;
	lineno = 1;

	if (dflag)
	{
		fprintf
		(	stderr,
			"readfile: \"%s\" -> %P a %P\n",
			name, program, endprogram
		);
	}

	/*
	 *	Prepara o nome do diretório do programa (para INCLUDES)
	 */
	if ((cp = strrchr (name, '/')) == NOSTR) 
	{
		incld[0] = ".";
	}
	else
	{
		*cp = '\0';

		if ((incld[0] = malloc (strlen (name) + 1)) == NOSTR)
			error (NOSTR);

		strcpy ((char *)incld[0], name);

		*cp = '/';
	}

	return (0);

}	/* end readfile */
