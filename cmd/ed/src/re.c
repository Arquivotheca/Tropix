/*
 ****************************************************************
 *								*
 *			src/re.c				*
 *								*
 *	Expressões Regulares					*
 *								*
 *	Versão	1.0.0, de 04.02.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: VI/EX/ED					*
 *		Editores de Texto				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições deste módulo					*
 ****************************************************************
 */
entry char *_end_match;	/* Primeiro caracter na linha após a expressão */

/*
 ****** Tipos de Elementos no Automato **************************
 */
#define  C_UNIQUE	0x01		/* Caracter Normal */
#define  C_SET		0x02		/* Um conjunto de caracteres */
#define  C_ANY		0x03		/* Qualquer caracter */
#define  E_BEGIN	0x04		/* Início de uma Expressão */
#define  E_END		0x05		/* Fim de uma Expressão */
#define  E_TABLE	0x06		/* Início de uma tabela C_SET */

#define  ELEM(c)	(c & 0x0F)

/*
 ******	 Atributos para os elementos no Automato ****************
 */
#define  A_FIRST	0x10	/* Ocorrencia do elemento no início */
#define  A_LAST		0x20	/*			  no fim */
#define  A_REPEAT	0x40	/* Repetição zero ou mais vezes */
#define  A_RANGE	0x80	/* Repetição entre limites */

#define  ATRIB(c)	((c) & 0xF0)

/*
 ******	Conjuntos de Caracteres *********************************
 */
#define  TBSIZE		(128)
#define  MASK(n)	(1 << (n-1))

#define  INCLUDE(ch, bit)	(set_table[ch] |= MASK(bit))
#define  EXCLUDE(ch, bit)	(set_table[ch] &= ~MASK(bit))
#define  PRESENT(ch, mask)	((set_table[ch] & mask) != 0)

/*
 ****************************************************************
 *	Compila Expressão Regular em um Automato Finito		*
 ****************************************************************
 */
bool
re_compile (const char *expression, AUTO automata)
{
	const char 	*ep; /* aponta caracter corrente */
	char		*cp;	/* aponta codigo corrente */
	char		*cq;	/* aponta ultimo codigo */
	char		*cs;	/* aponta início do codigo da exp. atual */
	char		*code;	/* Area para o codigo gerado */
	int		ch;
	int		size;	/* tamanho necessario p/ o codigo */
	int		ntabs;	/* No. de tabelas C_SET */
	int		nbits;	/* No. de bits C_SET ainda disponiveis */
	int		nsets;
	char		*set_table;

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
#endif
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
				{ ntabs++; nbits = 8; }

			nbits--;
			size += 2;

			while (*ep && *ep != ']' && *(ep-1) != '\\')
				ep++;

			if (*ep == '\0')
				{ error_msg = "Falta ']'\n"; return (false); }
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
#endif
	}	/* for (*ep) */

	size += TBSIZE * ntabs;

	/*
	 *	Verifica o tamanho do codigo.
	 */
	if (size >= AUTOSZ)
	{
		error_msg = "Expressão regular longa demais";
		return (false);
	}

	code = automata;

	/*
	 ******	Compilação **************************************
	 */
	nsets = 0; set_table = NULL; cs = &code[0];

	/*
	 *	Coloca as Tabelas no início do Codigo.
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
	 *	Inicia a Geração do Automato.
	 */
	*cs = E_BEGIN;
	cq = NULL;	/* cq == NULL => início de uma expressão */
	cp = cs +1;

	/*
	 *	Percorre a Expressão Regular e Gera seu Automato correspondente.
	 */
	for (ep = &expression[0]; *ep; ep++)
	{
#ifdef	DEBUG
		printf ("caracter: '%c'\n", *ep);
#endif
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
				if (*ep != '\\' && *(ep+1) == '-' && *(ep+2) >= *ep && *(ep+2) != ']')
				{
					for (ch = *ep; ch <= *(ep+2); ch++)
						INCLUDE (ch, nsets);

					ep += 2;
				}
				else
				{
					if (*ep == '\\')
						ep++;

					INCLUDE (*ep, nsets);
				}
			}

			*cp++ = C_SET; *cp++ = MASK (nsets);
			break;

		    case_neg:
			for (ch = 0; ch <= 127; ch++)
				INCLUDE (ch, nsets);

			ep++;

			while (*++ep != ']')
			{
				if (*ep != '\\' && *(ep+1) == '-' && *(ep+2) >= *ep && *(ep+2) != ']')
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

			*cp++ = C_SET; *cp++ = MASK (nsets);
			break;

		    case '.':
			cq = cp;
			*cp++ = C_ANY;
			break;

		    case '*':
			if (cq != NULL && (ATRIB (*cq) & A_REPEAT) == 0)
				*cq |= A_REPEAT;
			else
				goto case_normal;

			break;

		    case '+':
			if (cq != NULL && (ATRIB (*cq) & A_REPEAT) == 0)
			{
				*cp++ = ELEM (*cq) | A_REPEAT;

				if (ELEM (*cq) == C_ANY)
				{
					cq = cp -1;
				}
				else
				{
					*cp++ = *(cq+1);
					cq = cp - 2;
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

	return (true);

}	/* end re_compile */

/*
 ****************************************************************
 *	Executa Automato correspondente a uma Expressão Regular	*
 ****************************************************************
 */
const char *
re_execute (const char *expr, const char *line, const char *start)
{
	/*
	 *	expr:	Automato da Expressão Regular
	 *	line:	Linha de Texto p/ execução do Automato
	 *	start: 	Início da execução em line
	 */
	bool		again;
	const char	*set_table;
	char		nsets;
	bool		first, last;
	bool		common_case;
	char		first_char;

	/*
	 *	Avança Tabelas
	 */
	if (ELEM (*expr) == E_TABLE)
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
	 *	Pequena verificação do formato do Automato.
	 */
	if (ELEM (*expr) != E_BEGIN)
	{
		error_msg = "Automato inválido\n";
		return (NULL);
	}

	/*
	 *	Atributos exclusivos da Expressão Mae.
	 */
	first = ATRIB (*expr) & A_FIRST;
	last =  ATRIB (*expr) & A_LAST;

	/*
	 *	Expressão no início da linha.
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
	 *	Otimização.
	 */
	common_case = ELEM (*(expr+1)) == C_UNIQUE && (ATRIB (*(expr+1)) & A_REPEAT) == 0;
	first_char = common_case ? *(expr+2) : '\0';

	/*
	 *	Percorre line, a partir de start, até que o automato
	 *	possa ser executado ou que line termine.
	 */
	for (again = true; again; start++)
	{
		again = (*start != '\0');

		/*
		 *	Otimização:
		 */
		if (common_case)
		{
			if ((start = strchr (start, first_char)) == NULL)
				return (NULL);
		}

		/*
		 *	Tenta executar o Automato.
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
 *	Verifica a continuação da execução do automato.		*
 ****************************************************************
 */
bool	
match (const char *ep, const char *lp, const char *set_table, int nsets)
{
	char		ch;
	const char	*bp;

	/*
	 *	Executa (recursivamente) o Automato a partir de start.
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
		for (ep++; lp >= bp; lp--)
		{
			if (match (ep, lp, set_table, nsets))
				return (true);
		}

		return (false);

	    default:
		error_msg = "match: Automato inválido\n";
		return (false);

	}	/* for (*ep)  switch (*ep) */

	/*
	 *	O Automato executou completamente.
	 *	Indica o final da execução em _end_match.
	 */
	_end_match = (char *)lp;
	return (true);

}	/* end match */
