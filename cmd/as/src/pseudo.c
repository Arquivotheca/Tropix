/*
 ****************************************************************
 *								*
 *			pseudo.c				*
 *								*
 *	Analisa pseudo-instruções				*
 *								*
 *	Versão	1.0.0, de 26.03.86				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Rotinas:						*
 *		pseudo1		pseudo2		ifpseudo	*
 *		explst1		explst2		align		*
 *		nonabsexp       nonrealexp			*
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
#include <a.out.h>
#include <string.h>
#include <time.h>
#include <wsdb.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/tabs.h"
#include "../h/expr.h"
#include "../h/op_code.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Macro para Arredondamento dos endereços			*
 ****************************************************************
 */
entry int	size_shift_vec[] =
{
	0,
	0,	/* RBYTE */
	1,	/* RWORD */
	0,
	2	/* RLONG */
};

#define	ROUND(a,b)	(((a) + (b)) & ~(b))

/*
 ****************************************************************
 *	Analisa Pseudo-Instruções - Primeiro Passo		*
 ****************************************************************
 */
void
pseudo1 (ulong group)
{
	SYMTB		*sp;
	char		*str;
	EXP		exp;
	long		nlc;

	switch (group)
	{
		/*
		 *	.text	(1)
		 */
	    case 6:
		section = TEXT;
		lc = &lctext;
		symblk++;
		break;

		/*
		 *	.const	(1)
		 */
	    case 28:
		section = CONST;
		lc = &lcconst;
		symblk++;
		break;

		/*
		 *	.data	(1)
		 */
	    case 7:
		section = DATA;
		lc = &lcdata;
		symblk++;
		break;

		/*
		 *	.bss	(1)
		 */
	    case 2:
		section = BSS;
		lc = &lcbss;
		symblk++;
		break;

		/*
		 *	.org	(1)
		 */
	    case 19:
		gettoken ();
		expression (&exp);

		if (exp.ex_type != section)
		{
			msg
			(	ERRO, EXPMARK,
				"Expressão errada, indefinida, "
				"ou não se refere à seção corrente",
				NOSTR, &exp
			);
		}
		elif (exp.ex_value < *lc)
		{
			msg
			(	ERRO, EXPMARK,
				"Não podemos retroceder na seção",
				NOSTR, &exp
			);
		}
		else
		{
			*lc = exp.ex_value;

			if (section == TEXT)
				put_org_sdi (*lc);
		}

		break;

		/*
		 *	.align	(1)
		 */
	    case 18:
		gettoken (); 	expression (&exp);

		if (exp.ex_type != ABS)
			{ nonabsexp (&exp); break; }

		nlc = *lc;

		switch (exp.ex_value)
		{
		    case 1:
			break;

		    case 2:
			nlc = ROUND (nlc, 1);
			break;

		    case 4:
			nlc = ROUND (nlc, 3);
			break;

		    default:
			msg
			(
				ERRO, EXPMARK,
				"Valor inválido para alinhamento",
				NOSTR, &exp
			);
		}

		if (section == TEXT)
			put_align_sdi (nlc, exp.ex_value - 1, nlc - *lc);

		*lc = nlc;

		break;

		/*
		 *	.even	(1)
		 */
	    case 8:
		nlc = *lc;

		nlc = ROUND (nlc, 1);

		if (section == TEXT)
			put_align_sdi (nlc, 1, nlc - *lc);

		*lc = nlc;

		break;

		/*
		 *	.common	(1)
		 */
	    case 9:
		if (gettoken () != ID)
		{
			msg (ERRO, SHORTMARK, "Esperava um identificador");
			break;
		}

		sp = putsym (token->l_name, token->l_len, S_SEARCH);

		if (sp->s.s_type != UNDEF)
		{
			msg (ADVR, LONGMARK, "Símbolo redefinido");
			break;
		}

		sp->s.s_flags |= S_EXTERN|S_LBL;

		if (gettoken () != CM)
		{
			msg (ERRO, SHORTMARK, "Esperava uma vírgula");
			break;
		}

		gettoken ();

		expression (&exp);

		if (exp.ex_type != ABS)
		{
			nonabsexp (&exp);
			sp->s.s_value = 4;
		}
		elif (exp.ex_value == 0)
		{
			msg
			(	ADVR, EXPMARK,
				"Expressão não pode valer zero",
				NOSTR, &exp
			);
			sp->s.s_value = 0;
		}
		else
		{
			sp->s.s_value = exp.ex_value;
		}

		break;

		/*
		 *	.global	(1)
		 */
	    case 10:
		for (EVER)
		{
			if (gettoken () != ID)
			{
				msg
				(	ERRO, SHORTMARK,
					"Esperava um identificador"
				);
				break;
			}

			sp = putsym (token->l_name, token->l_len, S_SEARCH);

			sp->s.s_flags |= S_EXTERN;

			if (gettoken () != CM)
				break;
		}

		break;

		/*
		 *	.byte	(1)
		 */
	    case 3:
		explst1 (RBYTE);
		break;

		/*
		 *	.word	(1)
		 */
	    case 5:
		explst1 (RWORD);
		break;

		/*
		 *	.long	(1)
		 */
	    case 4:
		explst1 (RLONG);
		break;

		/*
		 *	.ascii e .iso	(1)
		 */
	    case 12:
	    case 24:
		if (gettoken () != STRING)
		{
			msg (ERRO, SHORTMARK, "Esperava uma cadeia");
			break;
		}

		*lc += token->l_len;

		break;

		/*
		 *	.asciz e .isoz	(1)
		 */
	    case 13:
	    case 25:
		if (gettoken () != STRING)
		{
			msg (ERRO, SHORTMARK, "Esperava uma cadeia");
			break;
		}

		*lc += token->l_len + 1;

		break;

#if (0)	/*******************************************************/
		/*
		 *	.extd	(1)
		 */
	    case 21:
		gettoken (); expression (&exp);

		if (exp.ex_type != REAL && exp.ex_type != ABS)
			nonrealexp (&exp);

		*lc = align (RLONG) + sizeof (extd);
		break;
#endif	/*******************************************************/

		/*
		 *	.double	(1)
		 */
	    case 22:
		gettoken (); expression (&exp);

		if (exp.ex_type != REAL && exp.ex_type != ABS)
			nonrealexp (&exp);

		if (token->l_major != EOL)
		{
			msg
			(	ADVR, SHORTMARK,
				"No momento aceitamos apenas uma constante real"
			);
		}

		*lc = align (RLONG) + sizeof (double);
		break;

		/*
		 *	.float	(1)
		 */
	    case 23:
		gettoken (); expression (&exp);

		if (exp.ex_type != REAL && exp.ex_type != ABS)
			nonrealexp (&exp);

		if (token->l_major != EOL)
		{
			msg
			(	ADVR, SHORTMARK,
				"No momento aceitamos apenas uma constante real"
			);
		}

		*lc = align (RLONG) + sizeof (float);
		break;

		/*
		 *	.time	(1)
		 */
	    case 20:
		*lc = align (RLONG) + sizeof (time_t);
		break;

		/*
		 *	.dsb	(1)
		 */
	    case 14:
		defspace (RBYTE);
		break;

		/*
		 *	.dsw	(1)
		 */
	    case 15:
		defspace (RWORD);
		break;

		/*
		 *	.dsl	(1)
		 */
	    case 16:
		defspace (RLONG);
		break;

		/*
		 *	.page	(1)
		 */
	    case 17:
		break;

		/*
		 *	.asm	(1)
		 */
	    case 29:
		break;

		/*
		 *	.line	(1)
		 */
	    case 26:
		gettoken ();

		if (token->l_major == STRING)
		{
			char		*cp;

			cp = strrchr (token->l_str, '/');

			if (cp == NOSTR)
				cp = token->l_str;
			else
				cp++;

			sp = putsym (cp, strlen (cp), S_DEFINE);

			if (sp->s.s_type == UNDEF)
			{
				sp->s.s_value = lctext;
				sp->s.s_flags = S_EXTERN;
				sp->s.s_type  = SFILE;
			}
		}
		else
		{
			expression (&exp);

			if (exp.ex_type != ABS)
				nonabsexp (&exp);
		}

		break;

		/*
		 *	.syment	(1)
		 */
	    case 27:
		if (gettoken () != CTE)
			msg (ERRO, LONGMARK, "Constante vírgula");
		if (gettoken () != CM)
			msg (ERRO, SHORTMARK, "Falta vírgula");
		if (gettoken () != STRING)
			msg (ERRO, LONGMARK, "Cadeia esperada");
		if (gettoken () != CM)
			msg (ERRO, SHORTMARK, "Falta vírgula");
		if (gettoken () != CTE)
			msg (ERRO, LONGMARK, "Constante vírgula");
		if (gettoken () != CM)
			msg (ERRO, SHORTMARK, "Falta vírgula");
		if (gettoken () != CTE)
			msg (ERRO, LONGMARK, "Constante vírgula");
		if (gettoken () != CM)
			msg (ERRO, SHORTMARK, "Falta vírgula");
		if   (gettoken () == OP && token->l_minor == N_MINUS)
			gettoken ();
		if (token->l_major != CTE)
			msg (ERRO, LONGMARK, "Constante vírgula");
		break;

		/*
		 *	.mode16	(1)
		 */
	    case 30:
		seg16 = 1;
		break;

		/*
		 *	.mode32	(1)
		 */
	    case 31:
		seg16 = 0;
		break;

		/*
		 *	.if	(1)
		 */
	    case 35:
		pseudo_if (1);
		break;

		/*
		 *	.ifz	(1)
		 */
	    case 36:
		pseudo_if (0);
		break;

		/*
		 *	.ifdef	(1)
		 */
	    case 37:
		pseudo_ifdef (1);
		break;

		/*
		 *	.ifndef	(1)
		 */
	    case 38:
		pseudo_ifdef (0);
		break;

		/*
		 *	.elif	(1)
		 */
	    case 39:
		pseudo_elif (1);
		break;

		/*
		 *	.elifz	(1)
		 */
	    case 40:
		pseudo_elif (0);
		break;

		/*
		 *	.eldef	(1)
		 */
	    case 41:
		pseudo_eldef (1);
		break;

		/*
		 *	.elndef	(1)
		 */
	    case 42:
		pseudo_eldef (0);
		break;

		/*
		 *	.else	(1)
		 */
	    case 43:
		pseudo_else ();
		break;

		/*
		 *	.endif	(1)
		 */
	    case 44:
		pseudo_endif ();
		break;

		/*
		 *	.warn	(1) e 	.error	(1)
		 */
	    case 50:
	    case 51:
		gettoken ();

		str = token->l_str;

		exp.ex_value = 0;

		if (latoken->l_major == CM)
			{ gettoken (); gettoken (); expression (&exp); }

		msg
		(	group == 50 ? ADVR : ERRO, NOMARK,
			str, exp.ex_value
		);

		break;

	    default:
		msg
		(	ERRO, NOMARK,
			"Valor inválido de pseudo: %d",
			group
		);
		break;

	}	/* end switch */

}	/* end pseudo1 */

/*
 ****************************************************************
 *	Analisa Pseudo-Instruções - Segundo Passo		*
 ****************************************************************
 */
void
pseudo2 (ulong group)
{
	char		*pt;
	long		val = 0;
	short		i;
	EXP		exp;
	SYMTB		*sp, **link_place;
	LNOTB		*lp;
	time_t		tempo;
	char		c;

	switch (group)
	{
		/*
		 *	.text	(2)
		 */
	    case 6:
		section = TEXT;
		lc = &lctext;
		fseek (aout, sizeof (HEADER) + lctext, SEEK_SET);
		sectoff = lctext;
		symblk++;
		break;

		/*
		 *	.const	(2)
		 */
	    case 28:
		section = CONST;
		lc = &lcconst;
		fseek (aout, sizeof (HEADER) + lcconst, SEEK_SET);
		sectoff = lcconst;
		symblk++;
		break;

		/*
		 *	.data	(2)
		 */
	    case 7:
		section = DATA;
		lc = &lcdata;
		fseek (aout, sizeof (HEADER) + lcdata, SEEK_SET);
		sectoff = lcdata - csize - tsize;
		symblk++;
		break;

		/*
		 *	.bss	(2)
		 */
	    case 2:
		section = BSS;
		lc = &lcbss;
		symblk++;
		break;

		/*
		 *	.org	(2)
		 */
	    case 19:
		gettoken ();
		expression (&exp);

		if (exp.ex_value < *lc)
		{
			msg
			(	ERRO, EXPMARK,
				"Não podemos retroceder na seção",
				NOSTR, &exp
			);
		}

		val = exp.ex_value - *lc;
		*lc += val;

		if (section != BSS && val != 0)
		{
			fseek (aout, sizeof (HEADER) + (*lc), SEEK_SET);
			sectoff += val;
		}

		break;

		/*
		 *	.align	(2)
		 */
	    case 18:
		gettoken (); 	expression (&exp);

#if (0)	/*******************************************************/
		if (section == TEXT)
			break;
#endif	/*******************************************************/

		switch (exp.ex_value)
		{
		    case 1:
			val = 0;
			break;

		    case 2:
			val = *lc & 1;
			break;

		    case 4:
			val = ROUND (*lc, 3) - *lc;
			break;

		    default:
			msg
			(	ADVR, EXPMARK,
				"Valor inválido para alinhamento: %d",
				exp.ex_value, &exp
			);
			break;

		}	/* end switch */

		*lc += val;

		if (section != BSS && val != 0)
		{
			fseek (aout, sizeof (HEADER) + (*lc), SEEK_SET);
			sectoff += val;
		}

		break;

		/*
		 *	.even	(2)
		 */
	    case 8:
#if (0)	/*******************************************************/
		if (section == TEXT)
			break;
#endif	/*******************************************************/

		if (*lc & 1)
		{
			*lc += 1;

			if (section != BSS)
			{
	
				fseek (aout, sizeof (HEADER) + (*lc), SEEK_SET);
				sectoff += 1;
			}
		}

		break;

		/*
		 *	.common	(2)
		 */
	    case 9:
		break;

		/*
		 *	.global	(2)
		 */
	    case 10:
		break;

		/*
		 *	.byte	(2)
		 */
	    case 3:
		explst2 (RBYTE);
		break;

		/*
		 *	.word	(2)
		 */
	    case 5:
		explst2 (RWORD);
		break;

		/*
		 *	.long	(2)
		 */
	    case 4:
		explst2 (RLONG);
		break;

		/*
		 *	.ascii e .iso	(2)
		 */
	    case 12:
	    case 24:
		gettoken ();
		pt  = token->l_str;
		val = i = token->l_len;

		while (i-- > 0)
			putB (*pt++);

		*lc += val;
		break;

		/*
		 *	.asciz e .isoz	(2)
		 */
	    case 13:
	    case 25:
#ifdef	TESTPHASE
		if (labtb[0])
			testphase ();
#endif
		gettoken ();
		pt  = token->l_str;
		i   = token->l_len;
		val = i + 1;

		while (i-- > 0)
			putB (*pt++);

		putB ('\0');

		*lc += val;
		break;

#if (0)	/*******************************************************/
		/*
		 *	.extd (2)
		 */
	    case 21:
		*lc = align (RLONG);

#ifdef	TESTPHASE
		if (labtb[0])
			testphase ();
#endif
		gettoken (); expression (&exp);

		if (exp.ex_type == ABS)
			exp.ex_rval = (extd)exp.ex_value;

		*lc += putextd (exp.ex_rval);
		break;
#endif	/*******************************************************/

		/*
		 *	.double (2)
		 */
	    case 22:
		*lc = align (RLONG);
#ifdef	TESTPHASE
		if (labtb[0])
			testphase ();
#endif
		gettoken (); expression (&exp);

#if (0)	/*******************************************************/
		if (exp.ex_type == ABS)
			exp.ex_rval = exp.ex_value;
#endif	/*******************************************************/

		*lc += putdouble (exp.ex_rval);
		break;

		/*
		 *	.float	(2)
		 */
	    case 23:
		*lc = align (RLONG);
#ifdef	TESTPHASE
		if (labtb[0])
			testphase ();
#endif
		gettoken (); expression (&exp);

#if (0)	/*******************************************************/
		if (exp.ex_type == ABS)
			exp.ex_rval = exp.ex_value;
#endif	/*******************************************************/

		*lc += putfloat (exp.ex_rval);
		break;

		/*
		 *	.time	(2)
		 */
	    case 20:
		gettoken ();
		pt = token->l_end + 1; c = pt[0]; pt[0] = '\0';
		time (&tempo);
		if (token->l_major == EOL)
			val = tempo;
		else
			val = strtotime (token->l_begin, localtime (&tempo));
		pt[0] = c; 
		if (val < 0)
			msg (ERRO, LONGMARK, "Data inválida");
		*lc = align (RLONG) + putL (val);
		break;

		/*
		 *	.dsb	(2)
		 */
	    case 14:
		defspace (RBYTE);
		break;

		/*
		 *	.dsw	(2)
		 */
	    case 15:
		defspace (RWORD);
		break;

		/*
		 *	.dsl	(2)
		 */
	    case 16:
		defspace (RLONG);
		break;

		/*
		 *	.page	(2)
		 */
	    case 17:
		if (lflag)
			fprintf (list, "\f");
		break;

		/*
		 *	.asm	(2)
		 */
	    case 29:
		break;

		/*
		 *	.line	(2)
		 */
	    case 26:
		gettoken ();

		if (token->l_major == STRING)
		{
			char		*cp;

			cp = strrchr (token->l_str, '/');

			if (cp == NOSTR)
				cp = token->l_str;
			else
				cp++;

			sp = id_hash (cp, strlen (cp), &link_place);

			/*
			 *	Gera a Entrada apontando para o
			 *	Símbolo SFILE
			 */
			lp = putlno ();
			lp->l.ll_addr = sp->s_index;
			lp->l.ll_line = 0;
		}
		else
		{
			/*
			 *	Gera a Entrada apontando para a Linha
			 */
			expression (&exp);

			lp = putlno ();

			lp->l.ll_line = exp.ex_value;
			lp->l.ll_addr = lctext;
		}
		break;

		/*
		 *	.syment	(2)
		 */
	    case 27:
		{
			DBTB		*dp;

			dp = putdbtb ();

			gettoken ();
			if ((val = token->l_ival) != N_VARIABLE)
				dp->db.d_class = val + C_MOE;

			gettoken ();
			gettoken ();
			strscpy (dp->db.d_name, token->l_str, sizeof (dp->db.d_name));

			gettoken ();
			gettoken ();

			if (val == N_DIMENSIONS)
			{
				dp->db.d.d_dim[0] = token->l_ival;

				gettoken ();
				gettoken ();
				dp->db.d.d_dim[1] = token->l_ival;

				gettoken ();
				gettoken ();
				dp->db.d.d_dim[2] = token->l_ival;
			}
			else
			{
				if   (val == N_VARIABLE)
				{
				    dp->db.d_class = token->l_ival;
				    if (dp->db.d_class == C_UNDEF)
					dp->db.d_class = C_EOS;
				}
				elif (val == N_EXTENSION)
				{
				    dp->db.d.dbent.d_value = token->l_ival;
				}

				gettoken ();
				gettoken ();
				dp->db.d.dbent.d_type = token->l_ival;

				gettoken ();
				gettoken ();
				if (token->l_minor == N_MINUS)
				{
					gettoken ();
					token->l_ival = -token->l_ival;
				}

				if (val != N_EXTENSION)
				    dp->db.d.dbent.d_value = token->l_ival;
			}
		}

		break;

		/*
		 *	.mode16	(2)
		 */
	    case 30:
		seg16 = 1;
		break;

		/*
		 *	.mode32	(2)
		 */
	    case 31:
		seg16 = 0;
		break;

		/*
		 *	.if	(2)
		 */
	    case 35:
		pseudo_if (1);
		break;

		/*
		 *	.ifz	(2)
		 */
	    case 36:
		pseudo_if (0);
		break;

		/*
		 *	.ifdef	(2)
		 */
	    case 37:
		pseudo_ifdef (1);
		break;

		/*
		 *	.ifndef	(2)
		 */
	    case 38:
		pseudo_ifdef (0);
		break;

		/*
		 *	.elif	(2)
		 */
	    case 39:
		pseudo_elif (1);
		break;

		/*
		 *	.elifz	(2)
		 */
	    case 40:
		pseudo_elif (0);
		break;

		/*
		 *	.eldef	(2)
		 */
	    case 41:
		pseudo_eldef (1);
		break;

		/*
		 *	.elndef	(2)
		 */
	    case 42:
		pseudo_eldef (0);
		break;

		/*
		 *	.else	(2)
		 */
	    case 43:
		pseudo_else ();
		break;

		/*
		 *	.endif	(2)
		 */
	    case 44:
		pseudo_endif ();
		break;

		/*
		 *	.warn	(2) e 	.error	(2)
		 */
	    case 50:
	    case 51:
		break;

	    default:
		msg
		(	ERRO, NOMARK,
			"Valor inválido de pseudo: %d",
			group
		);
		break;

	}	/* end switch */

}	/* end pseudo2 */

/*
 ****************************************************************
 *	Analisa Lista de Expressões - Passo 1			*
 ****************************************************************
 */
void
explst1 (int rsz)
{
	long		sz, size;
	EXP		exp;

	/*
	 *	Alinha para a vizinhanca desejada
	 */
	*lc = align (rsz);

	sz = size = (rsz >> RSZSHIFT);
#if (0)	/*******************************************************/
	sz = size = 1 << (rsz >> RSZSHIFT);
#endif	/*******************************************************/

	gettoken ();
	expression (&exp);

	if (exp.ex_type == REAL)
	{
		msg
		(	ERRO, EXPMARK,
			"Expressão não pode ser real", 0, &exp
		);
	}

	while (token->l_major == CM)
	{
		gettoken ();
		expression (&exp);

		if (exp.ex_type == REAL)
		{
			msg
			(	ERRO, EXPMARK,
				"Expressão não pode ser real", 0, &exp
			);
		}

		sz += size;
	}

	*lc += sz;

}	/* end explst1 */

/*
 ****************************************************************
 *	Analisa Lista de Expressões - Passo 2			*
 ****************************************************************
 */
void
explst2 (int size)
{
	long		sz;
	EXP		exp;

	*lc = align (size);

#ifdef	TESTPHASE
	if (labtb[0])
		testphase ();
#endif
	gettoken ();
	expression (&exp);
	sz = reloc (&exp, size);

	while (token->l_major == CM)
	{
		gettoken ();
		expression (&exp);
		sz += reloc (&exp, size);
	}

	*lc += sz;

}	/* end explst2 */

/*
 ****************************************************************
 *	Provê o Alinhamento dos Últimos Rótulos			*
 ****************************************************************
 */
int
align (int boundary)
{
	long		nlc = 0;
	int		align_mask = 0;
	SYMTB		**sp, **endl;

	nlc = *lc;

	switch (boundary)
	{
	    case RBYTE:
		return (nlc);

	    case RWORD:
		nlc = ROUND (nlc, 1);
		align_mask = 1;
		break;

	    case RLONG:
		nlc = ROUND (nlc, 3);
		align_mask = 3;
		break;

	    default:
		msg (COMP, NOMARK, "Align: Alinhamento errado");
	}

	if (pass == 1)
	{
		/*
		 *	Se necessário, alinha os rótulos que
		 *	antecedem imediatamente a pseudo-instrução
		 */
		if (nlc != *lc)
		{
			endl = labp;

			for (sp = labtb; sp < endl; sp++)
				(*sp)->s.s_value = nlc;
		}

		if (section == TEXT)
			put_align_sdi (nlc, align_mask, nlc - *lc);
	}
	else /*	(pass == 2) */
	{
		/*
		 *	Não sendo BSS, pula o número de bytes
		 *	que for necessario no a.out
		 */
		if (section != BSS)
		{
			fseek (aout, sizeof (HEADER) + nlc, SEEK_SET);
			sectoff += nlc - *lc;
		}
	}

	/*
	 *	Esta rotina não altera diretamente *lc
	 */
	return (nlc);

}	/* end align */

/*
 ****************************************************************
 *	Emite um diagnóstico sobre uma expressão errada		*
 ****************************************************************
 */
void
nonabsexp (EXP *ep)
{
	switch (ep->ex_type)
	{
	    case UNDEF:
		msg
		(	ERRO, EXPMARK,
			"Expressão contém variável(is) indefinida(s)", 0, ep
		);
		break;

	    case ERR:
		break;

	    case REAL:
		msg
		(	ERRO, EXPMARK,
			"Expressão não pode ter valor real", 0, ep
		);
		break;

	    default:
		msg
		(	ERRO, EXPMARK,
			"Expressão deve ser absoluta", 0, ep
		);
	}

}	/* end nonabsexp */

/*
 ****************************************************************
 *	Opcodes referenciados na geração de código		*
 ****************************************************************
 */
void
nonrealexp (EXP *ep)
{
	switch (ep->ex_type)
	{
	    case UNDEF:
		msg
		(	ERRO, EXPMARK,
			"Expressão indefinida", 0, ep
		);
		break;

	    case ERR:
		break;

	    default:
		msg
		(	ERRO, EXPMARK,
			"Expressão deve ser absoluta (inteira ou real)",
			0, ep
		);
	}

}	/* end nonrealexp */

/*
 ****************************************************************
 *	Reserva Espaço para Variáveis				*
 ****************************************************************
 */
void
defspace (int size)
{
	int		shift;
	long		val;
	EXP		exp;

	shift = size_shift_vec[size >> RSZSHIFT];
#if (0)	/*******************************************************/
	shift = size >> RSZSHIFT;
#endif	/*******************************************************/

	gettoken ();
	expression (&exp);

	if (pass == 1)
	{
		if (exp.ex_type != ABS)
		{
			nonabsexp (&exp);
		}
		elif (exp.ex_value <= 0)
		{
			msg (ERRO, EXPMARK, "Valor inválido",	NOSTR, &exp);
		}
		else
		{
			*lc = align (size) + (exp.ex_value << shift);
		}
	}
	else
	{
		*lc = align (size);

		if (lflag)
			prntloc (*lc);

		val = exp.ex_value << shift;
		*lc += val;

		if (section != BSS)
		{
			fseek (aout, sizeof (HEADER) + (*lc), SEEK_SET);
			sectoff += val;
		}
	}

}	/* end defspace */
