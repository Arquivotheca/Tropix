/*
 ****************************************************************
 *								*
 *			malloc.c				*
 *								*
 *	Alocação de memória					*
 *								*
 *	Versão	4.0.0, de 26.07.01				*
 *		4.0.0, de 26.07.01				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/common.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições Globais					*
 ****************************************************************
 */
entry void	*malloc_mem0;

/*
 ****************************************************************
 *	Alocação de pequenas áreas de memória			*
 ****************************************************************
 */
void *
malloc_byte (int size)
{
	void		*mem;

	/*
	 *	Arredonda
	 */
	size += 3; size &= ~3;

	/*
	 *	Subtrai o tamanho pedido
	 */
	mem = malloc_mem0;

	malloc_mem0 += size;

	return (mem);

}	/* end malloc_byte */

/*
 ****************************************************************
 *	Liberação de uma área					*
 ****************************************************************
 */
void
free_byte (void *area)
{
}	/* end free_byte */
