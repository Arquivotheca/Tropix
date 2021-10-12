/*
 ****************************************************************
 *								*
 *			smalloc.c				*
 *								*
 *	Alocação dinâmica rápida de pequenas áreas de memória	*
 *								*
 *	Versão	1.0.0, de 03.09.86				*
 *		3.0.0, de 06.07.95				*
 *								*
 *	Módulo: smalloc						*
 *		libc/mem					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	BLOCKSZ		(4 * 1024)
#define	BLOCKROUND(n)	(((n) + BLOCKSZ-1) & ~(BLOCKSZ-1))
#define	STORESZ		4

entry void	*_sm_base = NOVOID,	/* Area smalloc'ed */
		*_sm_next = NOVOID,	/* Proxima area smalloc'ed */
		*_sm_end  = NOVOID;	/* Fim da area disponivel no momento */

/*
 ****************************************************************
 *	Lista de Areas alocadas por malloc			*
 ****************************************************************
 */
#define		MAX_AREAS	20

static	void	*area[MAX_AREAS];
static	int	narea = 0;

/*
 ****************************************************************
 *	Aloca mais memória para "smalloc"			*
 ****************************************************************
 */
void *
_smalloc (size_t nbytes)
{
	register int	blocksz;

	blocksz = BLOCKROUND (nbytes);

	/*
	 *	Aloca uma nova AREA para a Lista
	 */
	if (narea >= MAX_AREAS || (area[narea] = malloc (blocksz)) == NOVOID)
		return (NOVOID);

	/*
	 *	Reinicializa _sm_base, _sm_next e _sm_end
	 */
	_sm_base = area[narea];
	_sm_end	 = _sm_base + blocksz;
	_sm_next = _sm_base + _LONGROUND (nbytes);

	narea++;

	return (_sm_base);

}	/* end _smalloc */

/*
 ****************************************************************
 *	Libera toda memória alocada por smalloc			*
 ****************************************************************
 */
void	
sfree (void)
{
	register void	**p;

	for (p = &area[narea-1]; p >= &area[0]; p--)
		free (p[0]);

	narea = 0;

	_sm_base = NOVOID; _sm_next = NOVOID; _sm_end  = NOVOID;

}	/* end sfree */
