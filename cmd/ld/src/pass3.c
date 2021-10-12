/*
 ****************************************************************
 *								*
 *			pass3.c					*
 *								*
 *	Passo 3: Atribui valores definitivos aos símbolos	*
 *								*
 *	Versão	1.0.0, de 14.04.87				*
 *		4.3.0, de 06.07.02				*
 *								*
 *	Módulo: ld						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/common.h>
#include <sys/mmu.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Passo 3: Atribui valores definitivos aos símbolos	*
 ****************************************************************
 */
void
pass3 (void)
{
	GSYM		*gp;
	int		sz;
	off_t		value;

	/*
	 *	Define os Símbolos "_end", "_edata" e "_etext"
	 */
	if (!rflag && !bflag)
	{
		defsym ("_etext", EXTERN|TEXT, TEXT_SIZE);
		defsym ("_edata", EXTERN|DATA, DATA_SIZE);
		defsym ("_end",   EXTERN|BSS,  BSS_SIZE);
	}

	/*
	 *	Trata com especial deferencia o símbolo "start"
	 */
	if ((gp = hash ("start", sizeof ("start") - 1)) != NOGSYM)
		gp->s.s_flags |= S_REF;

	/*
	 *	Obtém o tamanho do COMMON
	 */
	if (!rflag)
	{
		for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if (!IS_COMMON (&gp->s))
				continue;

			COMMON_SIZE += ROUND (gp->s.s_value, 4);
		}
	}

	/*
	 *	Calcula as origens das várias seções
	 */
	if (TEXT_ORG == -1) 		/* Se "TEXT_ORG" não foi dado */
	{
		if (rflag || bflag)
			TEXT_ORG = 0;
		else
			TEXT_ORG = PGTOBY (USER_TEXT_PGA);
	}

	if (DATA_ORG == -1) 		/* Se "DATA_ORG" não foi dado */
	{
		if (nflag)
			DATA_ORG = PGTOBY (USER_DATA_PGA);
		else
			DATA_ORG = TEXT_ORG + TEXT_SIZE;
	}

	COMMON_ORG = DATA_ORG   + DATA_SIZE;
	BSS_ORG    = COMMON_ORG + COMMON_SIZE;

	/*
	 *	Analisa superposições
	 */
	if (TEXT_ORG + TEXT_SIZE > DATA_ORG && BSS_ORG + BSS_SIZE > TEXT_ORG)
		error ("@Superposição de TEXT e DATA/BSS");

	/*
	 *	Atribui valores definitivos aos símbolos
	 */
	for (value = COMMON_ORG, gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
	{
		switch (gp->s.s_type)
		{
		    case EXTERN|UNDEF:
			if (gp->s.s_value == 0)		/* UNDEF será detectado no passo 6 */
				break;

			if (rflag)			/* Não aloca COMMON */
				break;

			sz = gp->s.s_value;

			gp->s.s_type   = EXTERN|BSS;
			gp->s.s_value  = value;

			value += ROUND (sz, 4);
			break;

		    case EXTERN|TEXT:
			gp->s.s_value += TEXT_ORG;
			break;

		    case EXTERN|DATA:
			gp->s.s_value += DATA_ORG;
			break;

		    case EXTERN|BSS:
			gp->s.s_value += BSS_ORG;
			break;

		    default:				/* Por exemplo, ABS */
			break;

		}	/* end switch */

	}	/* end for */

	/*
	 *	Verifica Contadores de Referencias
	 */
	if (uflag)
	{
		int		unref = 0;

		for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if ((gp->s.s_flags & S_REF) == 0)
			{
				if (unref++ == 0)
					fprintf (stderr, "Símbolos nunca referenciados:\n");

				fprintf (stderr, "   %s\n", gp->s.s_name);
			}
		}
	}

	if (vflag)
	{
		if (mflag)
			putc ('\n', stdout);

		printf
		(	"Tsize = %9d, Dsize = %9d, Bsize = %9d, Csize = %9d\n",
			TEXT_SIZE, DATA_SIZE, BSS_SIZE, COMMON_SIZE
		);

		printf
		(	"Taddr = %P, Daddr = %P, Baddr = %P, Caddr = %P\n",
			TEXT_ORG, DATA_ORG, BSS_ORG, COMMON_ORG
		);
	}

}	/* end pass3 */
