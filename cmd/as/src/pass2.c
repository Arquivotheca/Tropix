/*
 ****************************************************************
 *								*
 *			pass2.c					*
 *								*
 *	Análise sintática - segundo passo			*
 *								*
 *	Versão	1.0.0, de 13.06.86				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Rotinas:						*
 *		pass2	regular2    testphase			*
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

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/op_code.h"
#include "../h/proto.h"

/*
 ******	Variáveis externas **************************************
 */
extern	int	if_false_level;	/* No. de Condicionais FALSE aninhadas */

/*
 ****************************************************************
 *	Analise Sintática - Segundo Passo			*
 ****************************************************************
 */
void
pass2 (void)
{
	MAJOR		major;
	SYMTB		*sp, **link_place;
	OPCODE		*op;

	labtb[0] = NOSYMTB;	/* Último rótulo */

	while (getline () >= 0)
	{
		major = gettoken ();

		/*
		 *	Trata rótulos
		 */
		if (major == ID && latoken->l_major == CL)
		{
			if (if_false_level)
				goto print;

			sp = id_hash (token->l_name, token->l_len, &link_place);

			labtb[0] = sp;

			symblk++;

			if (lflag)
				prntloc (sp->s.s_value);

			gettoken ();
			gettoken ();
		}
		elif (major == LID && latoken->l_major == CL)
		{
			if (if_false_level)
				goto print;

			sp = id_hash (token->l_name, token->l_len, &link_place);

			labtb[0] = sp;

			if (lflag)
				prntloc (sp->s.s_value);

			gettoken ();
			gettoken ();
		}
		elif (major == ID && latoken->l_major == EQ)
		{
			/*
			 *	Analisa atribuições
			 */
			if (if_false_level)
				goto print;

			if (lflag)
			{
				sp = id_hash (token->l_name, token->l_len, &link_place);

				if (sp->s.s_type != REAL)
					prntvalue (sp->s.s_value);

				prntln ();
			}

			continue;

		}	/* end atribuição */

		/*
		 *	Acabaram os rótulos
		 */
		if (token->l_major == EOL)
			goto print;

		if (token->l_major != ID)
		{
			msg (ERRO, LONGMARK, "Esperava instrução ou pseudo");
			continue;
		}

		/*
		 *	Analisa a Instrução
		 */
		op = op_hash (token->l_name);

		if (if_false_level && (op->o_flags & NOSKIP) == 0)
			goto print;

		opcodeno++;

		switch (op->o_type)
		{
		    /*
		     *	    Instruções Regulares
		     */
		    case I_REGULAR:
#ifdef	TESTPHASE
			if (labtb[0])
				testphase ();
#endif
			*lc += regular2 (op);
			break;

		    /*
		     *	    Pseudo-instruções
		     */
		    case I_PSEUDO:
			pseudo2 (op->o_group);
			break;

#if (0)	/*************************************/
		    /*
		     *	    Instruções de Ponto Flutuante
		     */
		    case I_FLOAT:
#ifdef	TESTPHASE
			if (labtb[0])
				testphase ();
#endif
			*lc += float2 (op);
			break;
#endif	/*************************************/

		    /*
		     *	    Tipo desconhecido
		     */
		    default:
			msg
			(	COMP, LONGMARK,
				"Instrução de tipo %d desconhecido",
				op->o_type
			);
			break;

		}	/* end switch (op->o_type) */

		labtb[0] = NOSYMTB;

	    print:
		if (lflag)
			prntln ();

	}	/* end while (getline) */

}	/* end pass2 */

/*
 ****************************************************************
 *	Verifica se o Rótulo está "Em Fase"			*
 ****************************************************************
 */
void
testphase (void)
{
	SYMTB		*sp = labtb[0];
	int		i;

	if (nmsgs != 0)
		return;

	if (sp == NOSYMTB)
		return;

	if ((i = *lc - sp->s.s_value) != 0)
	{
		msg
		(	ADVR, NOMARK,
			"Defasagem: \"%s\", lc = %P, sp = %P (%+d)",
			sp->s.s_name, *lc, sp->s.s_value, i
		);
	}

}	/* end testphase */
