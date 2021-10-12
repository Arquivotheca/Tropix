/*
 ****************************************************************
 *								*
 *			calloc.c				*
 *								*
 *	Alocação dinâmica de memória zerada			*
 *								*
 *	Versão	1.0.0, de 27.06.86				*
 *		3.2.3, de 08.12.99				*
 *								*
 *	Módulo: malloc						*
 *		libc/mem					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <string.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ****** Definição da ARENA **************************************
 */
#define	PGSZ		4096		/* Incremento para "sbrk" */
#define	PGMASK		(PGSZ-1)	/* Máscara para "sbrk" */

#define	BUSY		1		/* O endereço é sempre par */

#define	IS_BUSY(p)	((p) &  BUSY)

#define	WITH_BUSY(p)	((STORE *)((int)(p) |  BUSY))
#define	WITHOUT_BUSY(p)	((STORE *)((int)(p) & ~BUSY))

#define	SET_BUSY(p)	((p) |=  BUSY)
#define	CLR_BUSY(p)	((p) &= ~BUSY)

typedef	union store	STORE;

union store
{
	int	m_status;		/* O bloco está ou não em uso */
	STORE	*m_ptr;			/* Aponta para o próximo bloco */
};

#define	STORE_SHIFT	2		/* LOG (2) (sizeof (STORE)) */

#define	NOSTORE		(STORE *)NULL

extern	STORE	*_malloc_base;		/* Primeiro elemento da Arena */
extern	STORE	*_malloc_top;		/* Último elemento da Arena */

extern	STORE	*_malloc_search;	/* Ptr para busca */

/*
 ****** Depuração ***********************************************
 */
#ifdef	DEBUG

#define ASSERT(expr)	if ((expr) == 0)		\
			{				\
				 _malloc_assert (__FILE__, __LINE__); \
			}

extern void	_malloc_assert (const char *, int);

#else

#define ASSERT(expr)

#endif	DEBUG

/*
 ****************************************************************
 *	Alocação dinâmica de memória zerada			*
 ****************************************************************
 */
void *
calloc (size_t elem_count, size_t elem_sz)
{
	void		*area;
	int		store_count;

	/*
	 *	Aloca e zera a área
	 */
	store_count = (elem_count * elem_sz + sizeof (STORE) - 1) >> STORE_SHIFT;

	if ((area = malloc (store_count << STORE_SHIFT)) != NOVOID)
		memsetl (area, 0, store_count);

	return (area);

}	/* end calloc */
