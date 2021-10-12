/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
 *								*
 *	Versão	1.0.0, de 17.02.86				*
 *		4.1.0, de 25.08.01				*
 *								*
 *	Rotinas:						*
 *		gettoken	getline		getch		*
 *		getstr		getnum		getid		*
 *		getescape					*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/msg.h"
#include "../h/id_sym_tb.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis do Analisador Lexico				*
 ****************************************************************
 */
entry TOKEN	tok1,
		tok2;			/* Buffers para Token e Look Ahead */
entry TOKEN	*token   = &tok1,
		*latoken = &tok2;	/* Token Atual e Look Ahead */
entry char	line[LINESZ];		/* Buffer para a linha */
entry char	*nextp;			/* Ponteiro para o proximo caractere */
entry short	lineprinted;		/* Indica se a linha já foi impressa */

/*
 ****************************************************************
 *	Macros Locais						*
 ****************************************************************
 */
#define	RETURN(m,n)	{ latoken->l_minor = (n);	\
			  latoken->l_major = (m);	\
			  return (token->l_major); }

/*
 ****************************************************************
 *	Analisador Lexico - Modulo Principal			*
 ****************************************************************
 */
MAJOR
gettoken (void)
{
	const UNSYM	*up;
	char		*lp, c;
	TOKEN		*aux;

	aux = token; token = latoken; latoken = aux;

	lp = nextp;

	/*
	 *	Pula os separadores iniciais
	 */
    again:
	while (CATEG (lp) == SEPAR)
		lp++;

	/*
	 *	Analisa a categoria do caractere
	 */
	latoken->l_begin = latoken->l_end = lp;
	nextp = lp + 1;

	switch (CATEG (lp))
	{
	    case INV:
		if (*lp == '\0')
			{ lp[1] = '\0'; nextp = lp; RETURN (EOL, N_NULL); }

		msg
		(	ADVR, MYMARK,
			"Caractere Invalido: será ignorado",
			NOSTR, lp, lp
		);
		lp++;
		goto again;

	    case LETTER:
		return (getid ());

	    case DIGIT:
		return (getnum ());

	    case APOST:
		return (getch ());

	    case QUOTE:
		return (getstr ());

	    /*
	     *	Simbolos unarios
	     */
	    case USYM:
		up = &unsym[CINDEX (lp)];
		RETURN (up->u_major, up->u_minor);

	    /*
	     *	Simbolos binarios
	     */
	    case BSYM:
		switch (CINDEX (lp))
		{
		    case 0:			/* '/' */
			if (*nextp == '*')
			{
				for (lp = nextp; *lp != '\0'; lp++)
					/* vazio */;

				nextp = lp;

				RETURN (EOL, N_NULL);
			}

			RETURN (OP, N_DIV);

		    case 1:			/* ':' */
			c = *nextp;

			if   (c == 'b' || c == 'B')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SIZE, N_BYTE);
			}
			elif (c == 'w' || c == 'W')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SIZE, N_WORD);
			}
			elif (c == 'l' || c == 'L')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SIZE, N_LONG);
			}

			RETURN (CL, N_NULL);

		    case 2:			/* '<' */
			if   (*nextp == '<')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_LSHIFT);
			}
			elif (*nextp == '=')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_LE);
			}

			RETURN (OP, N_LT);

		    case 3:			/* '>' */
			if   (*nextp == '>')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_RSHIFT);
			}
			elif (*nextp == '=')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_GE);
			}

			RETURN (OP, N_GT);

		    case 4:			/* '@' */
			c = *nextp;

			if   (c == 'b' || c == 'B')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SCALE, N_BYTE);
			}
			elif (c == 'w' || c == 'W')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SCALE, N_WORD);
			}
			elif (c == 'l' || c == 'L')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SCALE, N_LONG);
			}
			elif (c == 'q' || c == 'Q')
			{
				latoken->l_end++;
				nextp++;
				RETURN (SCALE, N_QUAD);
			}

			RETURN (AT, N_NULL);

		    case 5:			/* '|' */
			if   (*nextp == '*')
			{
				for (lp = nextp; *lp != '\0'; lp++)
					;

				nextp = lp;

				RETURN (EOL, N_NULL);
			}
			elif (*nextp == '|')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_OROR);
			}

			RETURN (OP, N_OR);

		    case 6:			/* '=' */
			if (*nextp == '=')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_EQ);
			}

			RETURN (EQ, N_NULL);

		    case 7:			/* '!' */
			if (*nextp == '=')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_NE);
			}

			RETURN (OP, N_NOTNOT);

		    case 8:			/* '&' */
			if (*nextp == '&')
			{
				latoken->l_end++;
				nextp++;
				RETURN (OP, N_ANDAND);
			}

			RETURN (OP, N_AND);

		    default:
			msg
			(	COMP, NOMARK,
				"Índice binário inválido: %d", CINDEX (lp)
			);

			return (0);

		}	/* end switch (CINDEX (lp)) */

		/*
		 *	Êrro
		 */
	    default:
		msg (COMP, NOMARK, "Categoria inválida: %02X", CATEG (lp));
		return (0);

	}	/* end switch */

}	/* end gettoken */

/*
 ****************************************************************
 *	Le uma Linha do Arquivo fonte				*
 ****************************************************************
 */
int
getline (void)
{
	char		c;

	c = ' ';

	do
	{
		if (c != ' ' && lflag && pass == 2)
			prntln ();

		if (fngets (line, LINESZ, src) == NOSTR)
			return (NIL);

		lineprinted = 0;
		token->l_line++;
		latoken->l_line++;
	}
	while ((c = line[0]) == '\0' || c == '*' || c == '|');

	nextp = line;

	gettoken ();

	return (0);

}	/* end getline */

/*
 ****************************************************************
 *	Analisa cadeias						*
 ****************************************************************
 */
MAJOR
getstr (void)
{
	char		*lp, *bp;
	const char	*end_area;
	char		c;
	int		offset;
	char		*nc;
	TOKEN		*tp = latoken;

	/*
	 *	Prólogo
	 */
	lp = nextp;

	bp = tp->l_area; end_area = bp + tp->l_area_sz;

	/*
	 *	Retira e converte a cadeia
	 */
	for (EVER)
	{
		if (bp >= end_area)
		{
			offset = bp - tp->l_area;

			tp->l_area_sz += 64;

			if ((tp->l_area = realloc (tp->l_area, tp->l_area_sz)) == NOSTR)
				error (NOSTR);

			end_area = tp->l_area + tp->l_area_sz;

			bp = tp->l_area + offset;
		}

		if   ((c = *lp) == '"' || c == '\0')
			{ *bp++ = '\0'; break; }
		if   (c == '\\')
			{ *bp++ = getescape (++lp, &nc); lp = nc; }
		else
			{ *bp++ = c; lp++; }
	}

	/*
	 *	Epílogo
	 */
	if (c == '\0')
	{
		tp->l_end = lp - 1;
		msg (ERRO, LALONGMARK, "Cadeia não terminada");
	}

	tp->l_end = lp++; nextp = lp;
	
	tp->l_str = tp->l_area;
	tp->l_len = bp - tp->l_area - 1;

	RETURN (STRING, N_NULL);

}	/* end getstr */

/*
 ****************************************************************
 *	Analisa Constantes ou Simbolos Locais			*
 ****************************************************************
 */
MAJOR
getnum (void)
{
	char		*lp;
	ushort		ind, categ;
	long		base, n;
	static char	local_symbol_nm[16];

	base = 10;
	lp = latoken->l_begin;

	/*
	 *	Determina a base
	 */
	if (*lp == '0')
	{
		lp++;
		if (*lp == 'x' || *lp == 'X')
		{
			base = 16;
			lp++;
		}
		else
		{
			base = 8;
		}
	}

	n = 0;

	categ = CATEG (lp); ind = CINDEX (lp);

	/*
	 *	Calcula o valor da constante
	 */
	switch (base)
	{
	    case 8:
		while (categ == DIGIT && ind < 8)
		{
			n = (n << 3) + ind;
			lp++;
			categ = CATEG (lp);
			ind = CINDEX (lp);
		}

		if (*lp == 'e' || *lp == 'E' || *lp == '.')
			return (getreal ());

		break;

	    case 10:
		while (categ == DIGIT)
		{
			n = (n << 3) + (n << 1) + ind;
			lp++;
			categ = CATEG (lp);
			ind = CINDEX (lp);
		}

		if (*lp == 'e' || *lp == 'E' || *lp == '.')
			return (getreal ());

		break;

	    case 16:
		while (categ == DIGIT || categ == LETTER && ind != 0)
		{
			n = (n << 4) + ind;
			lp++;
			categ = CATEG (lp);
			ind = CINDEX (lp);
		}

		break;

	}

	/*
	 *	Simbolo local
	 */
	if (*lp == '$')
	{
		latoken->l_end = lp++; nextp = lp;

		if (base > 10)
			msg (ERRO, LALONGMARK, "Simbolo Local Inválido");

		latoken->l_name = local_symbol_nm;

		sprintf (latoken->l_name, "$%03d.%d", symblk, n);
		latoken->l_len = strlen (latoken->l_name);

		RETURN (LID, N_NULL);
	}

	nextp = lp--; latoken->l_end = lp;

	latoken->l_ival = n;

	RETURN (CTE, N_NULL);

}	/* end getnum */

/*
 ****************************************************************
 *	Analisa constantes reais				*
 ****************************************************************
 */
MAJOR
getreal (void)
{
	char		*ptr;

	latoken->l_rval = strtod (latoken->l_begin, (const char **)&ptr);
	nextp = ptr--;
	latoken->l_end = ptr;

	RETURN (RCTE, N_NULL);

}	/* end getreal */

/*
 ****************************************************************
 *	Analisa Identificadores					*
 ****************************************************************
 */
MAJOR
getid (void)
{
	char		*lp;
	int		len;
	TOKEN		*tp = latoken;

	/*
	 *	Acha o comprimento do identificador
	 */
	lp = tp->l_begin + 1;

	while (CATEG (lp) == LETTER || CATEG (lp) == DIGIT)
		lp++;

	len = lp - tp->l_begin;

	/*
	 *	Verifica se a área é de bom tamanho
	 */
	if (tp->l_area_sz < len + 1)
	{
		tp->l_area_sz = (len + 1 + 63) & ~63;

		if ((tp->l_area = realloc (tp->l_area, tp->l_area_sz)) == NOSTR)
			error (NOSTR);
	}

	/*
	 *	Epílogo
	 */
	tp->l_name = tp->l_area;

	nextp = lp--; tp->l_end = lp;

	if (len == 1 && tp->l_begin[0] == '.')
		RETURN (LOCCNT, N_NULL);

	memmove (tp->l_name, tp->l_begin, len); tp->l_name[len] = '\0';
	tp->l_len = len;

	RETURN (ID, N_NULL);

}	/* end getid */

/*
 ****************************************************************
 *	Analisa um Caractere 'x'				*
 ****************************************************************
 */
MAJOR
getch (void)
{
	char		*lp;
	int		v;
	char		*nc;

	lp = nextp;

	if (*lp == '\\')
	{
		v = getescape (++lp, &nc);
		lp = nc;
	}
	else
	{
		v = *lp;
		lp++;
	}
	
	latoken->l_ival = v;

	if (*lp != '\'')
	{
		latoken->l_begin = latoken->l_end = lp;
		msg (ERRO, LASHORTMARK, "Faltando o apóstrofe");
	}
	else
	{
		latoken->l_end = lp;
		lp++;
	}

	nextp = lp;

	RETURN (CTE, N_NULL);

}	/* end getch */

/*
 ****************************************************************
 *	Analisa Sequencias de Escape				*
 ****************************************************************
 */
int
getescape (char *lp, char **nlp)
{
	int		ret;
	int		categ, ind;

	/*
	 *	"lp" aponta para o caractere após o '\'
	 *
	 *	Número octal
	 */
	categ = CATEG (lp);
	ind = CINDEX (lp);

	if (categ == DIGIT && ind < 8)
	{
		ret = ind;

		lp++;
		categ = CATEG (lp);
		ind = CINDEX (lp);

		if (categ == DIGIT && ind < 8)
		{
			ret = (ret << 3) + ind;

			lp++;
			categ = CATEG (lp);
			ind = CINDEX (lp);

			if (categ == DIGIT && ind < 8)
			{
				ret = (ret << 3) + ind;
				lp++;
			}
		}

		*nlp = lp;
		return (ret);
	}

	/*
	 *	Não é octal
	 */
	switch (*lp++)
	{
	    case 'a':
		ret = 7;
		break;

	    case 'b':
		ret = '\b';
		break;

	    case 'e':
		ret = 0x1B;
		break;

	    case 'f':
		ret = '\f';
		break;

	    case 'n':
		ret = '\n';
		break;

	    case 'r':
		ret = '\r';
		break;

	    case 't':
		ret = '\t';
		break;

	    case 'v':
		ret = 0x0B;
		break;

		/*
		 *	Hexadecimal
		 */
	    case 'x':
		categ = CATEG (lp);
		ind = CINDEX (lp);

		if (categ == DIGIT || categ == LETTER && ind != 0)
		{
			ret = ind;			/* Dig. 1 */

			lp++;
			categ = CATEG (lp);
			ind = CINDEX (lp);

			if (categ == DIGIT || categ == LETTER && ind != 0)
			{
				ret = (ret << 4) + ind;	/* Dig. 2 */

				lp++;
#ifdef	THREE_HEX_DIGITs
				categ = CATEG (lp);
				ind = CINDEX (lp);

				if
				(	categ == DIGIT ||
					categ == LETTER && ind != 0
				)
				{
					ret = ((ret << 4) + ind) & 0xFF; /* 3 */
					lp++;
				}
#endif	THREE_HEX_DIGITs
			}
			break;
		}
	    default:
		ret = lp[-1];
		break;

	}	/* end switch */

	*nlp = lp;

	return (ret);

}	/* end getescape */
