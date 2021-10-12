/*
 ****************************************************************
 *								*
 *			free.c					*
 *								*
 *	Liberação de memória alocada dinâmicamente		*
 *								*
 *	Versão	1.0.0, de 17.07.87				*
 *		3.0.0, de 19.12.94				*
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

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	DEBUG

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
 *	Liberação de memória alocada dinâmicamente		*
 ****************************************************************
 */
void
free (void *area)
{
	STORE	*p = area;

	/*
	 *	Consistência inicial
	 */
	if (p == NOSTORE)
		{ fprintf (stderr, "%s (free): Endereço NULO\n", pgname); return; }

	if (p < _malloc_base || p > _malloc_top)
		{ fprintf (stderr, "%s (free): Endereço inválido\n", pgname); return; }

	_malloc_search = --p;

	if (!IS_BUSY (p->m_status))
		fprintf (stderr, "%s (free): Reliberação de um bloco\n", pgname);

	CLR_BUSY (p->m_status);

	ASSERT (p->m_ptr > _malloc_search && p->m_ptr <= _malloc_top);

}	/* end free */
