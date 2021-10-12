/*
 ****************************************************************
 *								*
 *			rand.c					*
 *								*
 *	Função simples para a geração de nos. aleatorios	*
 *								*
 *	Versão	1.0.0, de 15.10.86				*
 *		3.0.0, de 19.06.95				*
 *								*
 *	Módulo: rand						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
static long	seed = 1;

/*
 ****************************************************************
 *	Rotina para inicialização do gerador			*
 ****************************************************************
 */
void
srand (long a)
{
	seed = a;

}	/* end srand */

/*
 ****************************************************************
 *	Rotina para geração dos nos. aleatorios			*
 ****************************************************************
 */
long
rand (void)
{
	seed = getrand (seed); return (seed & 0x7FFFFFFF);

}	/* end rand */
