/*
 ****************************************************************
 *								*
 *			regcmp.c				*
 *								*
 *	Compilador de expressões regulares			*
 *								*
 *	Versão	1.0.0, de 08.01.87				*
 *		3.0.0, de 23.06.95				*
 *								*
 *	Módulo: regcmp						*
 *		libc/etc					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 ****************************************************************
 *	Definições e variáveis globais				*
 ****************************************************************
 */
typedef enum { false, true }	bool;

static const char	*_end_match;	/* aponta o primeiro caracter */
					/* na linha após a expressão */
entry char		*_loc1;

/*
 ******	Tipos de Elementos no Automato **************************
 */
#define  C_UNIQUE	0x01		/* Caracter Normal */
#define  C_SET		0x02		/* Um conjunto de caracteres */
#define  C_ANY		0x03		/* Qualquer caracter */
#define  E_BEGIN	0x04		/* Inicio de uma Expressão */
#define  E_END		0x05		/* Fim de uma Expressão */
#define  E_TABLE	0x06		/* Inicio de uma tabela C_SET */

#define  ELEM(c)	(c & 0x0F)

/*
 ******	Atributos para os elementos no Automato *****************
 */
#define  A_FIRST	0x10	/* Ocorrencia do elemento no inicio */
#define  A_LAST		0x20	/*			  no fim */
#define  A_REPEAT	0x40	/* Repetição zero ou mais vezes */
#define  A_RANGE	0x80	/* Repetição entre limites */

#define  ATRIB(c)	((c) & 0xF0)

/*
 ******	Conjuntos de Caracteres *********************************
 */
#define  TBSIZE		(256)
#define  MASK(n)	(1 << (n-1))

#define  INCLUDE(ch, bit)	(set_table[ch] |= MASK(bit))
#define  EXCLUDE(ch, bit)	(set_table[ch] &= ~MASK(bit))
#define  PRESENT(ch, mask)	((set_table[ch] & mask) != 0)

/*
 ******	Protótipos de funções ***********************************
 */
static bool	match (const char *, const char *, const char *, int);

/*
 ****************************************************************
 *	Compila Expressão Regular em um Automato Finito		*
 ****************************************************************
 */
char *
regcmp (const char *expression)
{
	register const char	*ep;	/* aponta caracter corrente */
	register char		*cp;	/* aponta codigo corrente */
	register char		*cq;	/* aponta ultimo codigo */
	register char		*cs;	/* aponta inicio do codigo da exp. atual */
	char			*code;	/* Area para o codigo gerado */

	register int		ch;

	register int		size;	/* tamanho necessario p/ o codigo */
	register int		ntabs;	/* No. de tabelas C_SET */
	register int		nbits;	/* No. de bits C_SET ainda disponiveis */

	register int		nsets;
	char			*set_table;

	/*
	 ******	Calcula tamanho necessario p/ o codigo gerado ***
	 */
	size = 3;	/* E_BEGIN, E_END, '\0' */
	ntabs = 0;
	nbits = 0;

	for (ep = &expression[0]; *ep; ep++)
	{
#ifdef	DEBUG
		printf ("caracter: '%c'\n", *ep);
#endif	DEBUG
		switch (*ep)
		{
		    case '\\':
			ep++;
			goto case_default;

		    case '^':
			if (ep > expression)
				goto case_default;
			break;

		    case '$':
			if (*(ep+1) != '\0')
				goto case_default;
			break;

		    case '[':
			if (nbits == 0)
			{
				ntabs++;
				nbits = 8;
			}

			nbits--; size += 2;

			while (*ep && *ep != ']' && *(ep-1) != '\\')
				ep++;

			if (*ep == '\0')
			{
				fputs ("regcmp: Falta ']'\n", stderr);
				return (NULL);
			}
			break;

		    case '.':
			size += 1;
			break;

		    case '*':
			if (ep == expression)
				goto case_default;
			break;

		    case '+':
			if (ep == expression)
				size += 2;
			else
				goto case_default;
			break;

		    case_default:
		    default:
			size += 2;
			break;

		}	/* switch (*ep) */

#ifdef	DEBUG
		printf ("Size:%d, Nbits:%d, Ntabs:%d\n\n", size, nbits, ntabs);
#endif	DEBUG

	}	/* for (*ep) */

	size += TBSIZE * ntabs;

	/*
	 *	Aloca memoria para o Automato
	 */
	if ((code = malloc (size)) == NULL)
		return (NULL);

	/*
	 **********************	Compilação **********************
	 */
	nsets = 0;
	set_table = NULL;

	cs = &code[0];

	/*
	 *	Coloca as Tabelas no inicio do Codigo
	 */
	while (ntabs -- > 0)
	{
		*cs = E_TABLE;

		if (set_table == NULL)
			set_table = cs +1;

		for (cp = cs +1; cp < cs + TBSIZE +1; cp++)
			*cp = 0;

		cs += TBSIZE +1;
	}

	/*
	 *	Inicia Geração do Automato
	 */
	*cs = E_BEGIN;
	cq = NULL;	/* cq == NULL => inicio de uma expressão */
	cp = cs +1;

	/*
	 *	Percorre a Expressão Regular e Gera seu Automato correspondente
	 */
	for (ep = &expression[0]; *ep; ep++)
	{
#ifdef	DEBUG
		printf ("caracter: '%c'\n", *ep);
#endif	DEBUG
		switch (*ep)
		{
		    case '\\':
			cq = cp;
			*cp++ = C_UNIQUE;
			*cp++ = *++ep;
			break;

		    case '^':
			if (cq == NULL)
				*cs |= A_FIRST;
			else
				goto case_normal;
			break;

		    case '$':
			if (*(ep+1) == '\0')
				*cs |= A_LAST;
			else
				goto case_normal;
			break;

		    case '[':
			cq = cp;

			if (nsets == 8)
			{
				set_table += TBSIZE +1;
				nsets = 1;
			}
			else
			{
				nsets++;
			}

			if (*(ep+1) == '^')
				goto case_neg;

			while (*++ep != ']')
			{
				if
				(	*ep != '\\' && *(ep+1) == '-' &&
					*(ep+2) >= *ep && *(ep+2) != ']'
				)
				{
					for (ch = *ep; ch <= *(ep+2); ch++)
						INCLUDE (ch, nsets);
					ep++;
					ep++;
				}
				else
				{
					if (*ep == '\\')
						ep++;
					INCLUDE (*ep, nsets);
				}
			}

			*cp++ = C_SET;
			*cp++ = MASK (nsets);
			break;

		    case_neg:
			for (ch = 0; ch <= 127; ch++)
				INCLUDE (ch, nsets);

			ep++;

			while (*++ep != ']')
			{
				if
				(	*ep != '\\' && *(ep+1) == '-' &&
					*(ep+2) >= *ep && *(ep+2) != ']'
				)
				{
					for (ch = *ep; ch <= *(ep+2); ch++)
						EXCLUDE (ch, nsets);
					ep += 2;
				}
				else
				{
					if (*ep == '\\')
						ep++;
					EXCLUDE (*ep, nsets);
				}
			}

			*cp++ = C_SET;
			*cp++ = MASK (nsets);
			break;

		    case '.':
			cq = cp;
			*cp++ = C_ANY;
			break;

		    case '*':
			if (cq != NULL && (ATRIB(*cq) & A_REPEAT) == 0)
				*cq |= A_REPEAT;
			else
				goto case_normal;
			break;

		    case '+':
			if (cq != NULL && (ATRIB(*cq) & A_REPEAT) == 0)
			{
				*cp++ = ELEM(*cq) | A_REPEAT;

				if (ELEM(*cq) == C_ANY)
				{
					cq = cp -1;
				}
				else
				{
					*cp++ = *(cq+1);
					cq = cp -2;
				}
			}
			else
			{
				goto case_normal;
			}
			break;

		case_normal:
		    default:
			cq = cp;
			*cp++ = C_UNIQUE;
			*cp++ = *ep;
			break;

		}	/* switch (*ep) */

	}	/* for (*ep) */

	*cp++ = E_END;
	*cp = '\0';

	return (code);

}	/* end regcmp */

/*
 ****************************************************************
 *	Executa Automato correspondente a uma Expressão Regular	*
 ****************************************************************
 */
static const char *
re_execute (register const char *expr, const char *line, register const char *start)
{
	const char	*set_table;
	char		nsets;
	bool		first, last;
	register bool	common_case;
	register char	first_char;

	/*
	 *	Avanca Tabelas
	 */
	if (ELEM(*expr) == E_TABLE)
	{
		set_table = expr +1;
		expr = expr + TBSIZE + 1;

		while (ELEM(*expr) == E_TABLE)
			expr += TBSIZE + 1;

		nsets = 8;
	}
	else
	{
		set_table = NULL;
		nsets = 0;
	}

	/*
	 *	Pequena verificação do formato do Automato
	 */
	if (ELEM(*expr) != E_BEGIN)
	{
		fprintf (stderr, "re_exec: Automato invalido\n");
		return (NULL);
	}

	/*
	 *	Atributos exclusivos da Expressão Mae
	 */
	first = ATRIB (*expr) & A_FIRST;
	last =  ATRIB (*expr) & A_LAST;

	/*
	 *	Expressão no inicio da linha
	 */
	if (first)
	{
		if (start != line)
			return (NULL);

		if (match (expr, line, set_table, nsets))
		{
			if (!last || *_end_match == '\0')
				return (start);
		}

		return (NULL);
	}

	/*
	 *	Otimização
	 */
	common_case = ELEM (*(expr+1)) == C_UNIQUE
			&& (ATRIB (*(expr+1)) & A_REPEAT) == 0;

	first_char = common_case ? *(expr+2) : '\0';

	/*
	 *	Percorre line, a partir de start,
	 *	até que o automato possa ser executado
	 *	ou que line termine
	 */
	for (/* vazio */; *start; start++)
	{
		/*
		 *	Otimização
		 */
		if (common_case)
		{
			if ((start = strchr (start, first_char)) == NULL)
				return (NULL);
		}

		/*
		 *	Tenta executar o Automato
		 */
		if (match (expr, start, set_table, nsets))
		{
			if (!last || *_end_match == '\0')
				return (start);
		}
	}

	return (NULL);

}	/* end re_execute */

/*
 ****************************************************************
 *	Ponto de Entrada para Execução do Automato.		*
 ****************************************************************
 */
char *
regex (register const char *expr, register const char *line, register char *mp)
{
	register const char	*p;

	_loc1 = (char *)re_execute (expr, line, line);

	if (_loc1 != NULL)
	{
		for (p = _loc1; p < _end_match; p++)
			*mp++ = *p;

		*mp = '\0';

		return ((char *)_end_match);
	}
	else
	{
		return (NULL);
	}

}	/* end regex */

/*
 ****************************************************************
 *	Verifica a continuação da execução do automato		*
 ****************************************************************
 */
static bool	
match (register const char *ep, register const char *lp, const char *set_table, int nsets)
{
	register char	ch;
	register const char	*bp;

	/*
	 *	Executa (recursivamente) o Automato a partir de start
	 */
	for (/* vazio */; *ep; ep++) switch (*ep)
	{
	    case E_BEGIN:
	    case E_BEGIN | A_FIRST:
	    case E_BEGIN | A_FIRST | A_LAST:
	    case E_BEGIN | A_LAST:
		break;

	    case E_END:
		break;

	    case C_UNIQUE:
		if (*lp == *++ep)
			lp++;
		else
			return (false);
		break;

	    case C_UNIQUE | A_REPEAT:
		ch = *++ep;
		bp = lp;

		while (*lp == ch)
			lp++;

		goto case_repeat;

	    case C_SET:
		if (--nsets < 0)
		{
			set_table += TBSIZE +1;
			nsets = 7;
		}

		ch = *++ep;

		if (PRESENT(*lp, ch))
			lp++;
		else
			return (false);
		break;

	    case C_SET | A_REPEAT:
		if (--nsets < 0)
		{
			set_table += TBSIZE +1;
			nsets = 7;
		}

		ch = *++ep;
		bp = lp;

		while (PRESENT(*lp, ch))
			lp++;

		goto case_repeat;

	    case C_ANY:
		if (*lp)
			lp++;
		else
			return (false);
		break;

	    case C_ANY | A_REPEAT:
		bp = lp;

		while (*lp)
			lp++;

		goto case_repeat;

	    case_repeat:
		ep++;

		for (/* vazio */; lp >= bp; lp-- )
		{
			if (match (ep, lp, set_table, nsets))
				return (true);
		}
		return (false);

	    default:
		fprintf (stderr, "re: match: Automato invalido\n");
		return (false);

	}	/* for (*ep)  switch (*ep) */

	/*
	 *	O Automato executou completamente
	 *	Indica o final da execução em _end_match
	 */
	_end_match = lp;
	return (true);

}	/* end match */
