/*
 ****************************************************************
 *								*
 *			sym.c					*
 *								*
 *	Funções relativas à tabela de símbolos			*
 *								*
 *	Versão	3.0.0, de 22.08.94				*
 *		3.0.0, de 22.08.94				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/proto.h"

#include <sys/a.out.h>

/*
 ****************************************************************
 *	Imprime a tabela de símbolos				*
 ****************************************************************
 */
void
print_symtb (SYM *tbaddr, int ssize)
{
	SYM		*sp;
	int		i = 0;
	char		buf[8];

	if (ssize == 0)
	{
		printf ("\nO núcleo não possui tabela de símbolos\n");
		return;
	}

#if (0)	/*******************************************************/
	if (ssize % sizeof (SYM))
		printf ("O tamanho da tabela não é múltiplo de uma entrada\n");
#endif	/*******************************************************/

	for (sp = tbaddr; ssize > 0; ssize -= sizeof (SYM), sp = SYM_NEXT_SYM (sp))
	{
		printf ("%P  %s\n", sp->s_value, sp->s_name);

		if (i++ >= 5)
			{ gets (buf); i = 0; }
	}

}	/* end print_symtb */
