/*
 ****************************************************************
 *								*
 *			symtb.c					*
 *								*
 *	Gerência da tabela de símbolos				*
 *								*
 *	Versão	1.0.0, de 03.03.86				*
 *		4.4.0, de 17.04.04				*
 *								*
 *	Rotinas:						*
 *		putsym		symnumber	listsym		*
 *		writesymtb					*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
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
#include "../h/id_sym_tb.h"
#include "../h/as.h"
#include "../h/msg.h"
#include "../h/op_code.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis & Definições Locais				*
 ****************************************************************
 */
static	SYMTB	symb;			/* Cabeca da Lista de Simbolos */

entry SYMTB	*symhead  = &symb,	/* Inicio da Lista de Simbolos */
		*defhead = &symb,	/* Inicio da Definição de Simbolos */
		*symtail = &symb,	/* Fim da Lista de Simbolos */
		*deftail = &symb;	/* Fim da Definição de Simbolos */

extern int	op_hash_sz;	/* Tamanho da tabela HASH de operadores */

extern OPCODE	*op_hash_tb[];	/* Endereço da Tabela HASH de operadores */

extern SYMTB	*id_hash_tb[];	/* Endereço da Tabela HASH de identificadores */

/*
 ****************************************************************
 *	A função HASH de operadores				*
 ****************************************************************
 */
#undef	MK
#include "../h/op_hash.h"
 
/*
 ****************************************************************
 *	A função HASH de símbolos				*
 ****************************************************************
 */
#undef	MK
#include "../h/id_hash.h"

/*
 ****************************************************************
 *	Insere um Simbolo na Tabela				*
 ****************************************************************
 */
SYMTB *
putsym (const char *name, int len, int def)
{
	SYMTB		*sp, **link_place;
	int		sz;

	if ((sp = id_hash (name, len, &link_place)) == NOSYMTB)
	{
		sz = SYM_SIZEOF (len);

		if ((sp = malloc (sizeof (SYMTB) - sizeof (SYM) + sz)) == NOSYMTB)
			error (NOSTR);

		sp->s_link = NOSYMTB; *link_place = sp;

		sp->s.s_type    = UNDEF;
		sp->s.s_flags	= 0;
		sp->s.s_value	= 0;

		sp->s.s_nm_len	= len;
		strncpy (sp->s.s_name, name, SYM_NM_SZ (len));

		sp->s_next      = NOSYMTB;
		sp->s_def       = NOSYMTB;
		sp->s_line      = 0;

		symtail->s_next = sp;
		symtail         = sp;
	}

	if (def == S_DEFINE && sp->s.s_type == UNDEF)
	{
		sp->s_line = token->l_line;

		deftail->s_def = sp;
		deftail = sp;
	}

	return (sp);

}	/* end putsym */

/*
 ****************************************************************
 *	Numera os Simbolos na Tabela				*
 ****************************************************************
 */
void
symnumber (void)
{
	int		index;
	SYMTB		*sp;
	int		local;

	index = 0;
	local = !aflag;

	for (sp = symhead->s_next; sp != NOSYMTB; sp = sp->s_next)
	{
		/*
		 *	Não numera os simbolos locais.
		 *	Não numera os simbolos com valores reais.
		 */
		if (sp->s.s_flags & S_LOCAL || sp->s.s_type == REAL)
			continue;

		/*
		 *	Numera apenas os simbolos externos,
		 *	a menos que "aflag" esteja ligado.
		 */
		if (!local || sp->s.s_flags & S_EXTERN || sp->s.s_type == UNDEF)
			sp->s_index = index++;
	}

}	/* end symnumber */




#if (0)	/*************************************/
		if (sp->s.s_flags & S_EXTERN || sp->s.s_type == UNDEF)
		{
			/* vazio */;
		}
		elif (local)
		{
			continue;
		}

		sp->s_index = index++;
#endif	/*************************************/

/*
 ****************************************************************
 *	Imprime a Tabela de Simbolos				*
 ****************************************************************
 */
void
listsym (void)
{
	SYMTB		*sp;

	if ((sp = symhead->s_next) == NOSYMTB)
		return;

	fprintf (list, "\f\n\n\nTIPO  ------------SÍMBOLO------------");
	fprintf (list, "  LOCAL  ROT EXT  --VALOR--\n\n");

	for (/* sem inicialização */; sp != NOSYMTB; sp = sp->s_next)
	{
		switch (sp->s.s_type & ~EXTERN)
		{
		    case UNDEF:
			fprintf (list, "UND  ");
			break;

		    case REG:
			fprintf (list, "REG  ");
			break;

		    case ABS:
			fprintf (list, "ABS  ");
			break;

		    case TEXT:
			fprintf (list, "TEXT ");
			break;

		    case CONST:
			fprintf (list, "CONST");
			break;

		    case DATA:
			fprintf (list, "DATA ");
			break;

		    case BSS:
			fprintf (list, "BSS  ");
			break;

		    case SFILE:
			fprintf (list, "FILE ");
			break;

		    case REAL:
			fprintf (list, "REAL ");
			break;

		    default:
			fprintf (list, "???? ");
		}

		if (sp->s.s_flags & S_LOCAL)
		{
			fprintf
			(	list, " (%-3.3s),$%-9s       "
				"            ",
				sp->s.s_name + 1,
				sp->s.s_name + 5
			);
		}
		else
		{
			fprintf
			(	list,
				" %-32s   ",
				sp->s.s_name
			);
		}

		fprintf
		(	list, "%c     %c   %c   ", 
			sp->s.s_flags & S_LOCAL ?  'L' : ' ',
			sp->s.s_flags & S_LBL ?    'R' : ' ',
			sp->s.s_flags & S_EXTERN ? 'E' : ' '
		);

		if (sp->s.s_type == REAL)
		{
			fprintf (list, "%g\n", sp->s_rval);
		}
		else
		{
			fprintf
			(	list, "%04X %04X\n",
				(unsigned)sp->s.s_value >> 16,
				sp->s.s_value & 0xFFFF
			);
		}
	}

}	/* end listsym */

/*
 ****************************************************************
 *	Escreve a Tabela de Simbolos no a.out			*
 ****************************************************************
 */
long
writesymtb (void)
{
	SYMTB		*sp;
	long		size;
	int		local;
	int		sz;

	size  = 0;
	local = !aflag;

	/*
	 *	Posiciona o a.out apos o "text" e o "data"
	 */
	fseek (aout, sizeof (HEADER) + tsize + csize + dsize, SEEK_SET);

	for (sp = symhead->s_next; sp != NOSYMTB; sp = sp->s_next)
	{
		/*
		 *	Escreve apenas o simbolos não locais.
		 *	Não escreve os símbolos com valores reais.
		 */
		if (sp->s.s_flags & S_LOCAL || sp->s.s_type == REAL)
			continue;

		/*
		 *	Não esquece de transformar "const" em "text"
		 */
		if (sp->s.s_type == CONST)
			sp->s.s_type = TEXT;

		/*
		 *	Dos simbolos restantes, escreve apenas
		 *	os externos, exceto quando "aflag" estiver
		 *	ligado.
		 */
		if (sp->s.s_flags & S_EXTERN || sp->s.s_type == UNDEF)
		{
			sp->s.s_flags |= S_EXTERN;
			sp->s.s_type |= EXTERN;
		}
		elif (local)
		{
			continue;
		}

		/*
		 *	Escreve a entrada
		 */
		sz = SYM_SIZEOF (sp->s.s_nm_len);

		fwrite (&sp->s, 1, sz, aout);

		size += sz;
	}

	return (size);

}	/* end writesymtb */
