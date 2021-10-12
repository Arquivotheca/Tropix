/*
 ****************************************************************
 *								*
 *			salloc.c				*
 *								*
 *	Alocação Dinâmica de Memória				*
 *								*
 *	Versão	3.2.3, de 19.12.99				*
 *		4.4.0, de 21.11.02				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Estruturas Locais					*
 ****************************************************************
 */
#define	NAREAS		16
#define	ALLOC_SHIFT	10	/* Grão: 1Kbytes */

/*
 ******	Estrutura da pilha de alocação **************************
 */
typedef	struct
{
	void	*a_area;	/* Ponteiro para a área alocada */

	ulong	a_total,	/* Tamanho total da área */
		a_free;		/* Tamanho que ainda resta disponível */

}	ALLOC;

/*
 ****************************************************************
 *	Variáveis Locais					*
 ****************************************************************
 */
static	ALLOC	area[NAREAS],		/* A posição 0 é sentinela */
		*areap = area;

/*
 ****************************************************************
 *	Aloca "nbytes"						*
 ****************************************************************
 */
void *
salloc (int byte_count)
{
	ulong		nb, sz;
	void		*cp;
	ALLOC		*ap;

	if (byte_count <= 0)
	{
		error ("salloc: pedido de %d bytes", byte_count);
		return (NOVOID);
	}

	/*
	 *	Arredonda o tamanho pedido para múltiplos de "sizeof (long)".
	 */
	nb = (byte_count + sizeof (long) - 1) & ~(sizeof (long) - 1);

#ifdef	DEBUG
	fprintf (stderr, "salloc: pedidos %d, alocados %d\n", byte_count, nb);
#endif

	ap = areap;

	/*
	 *	Se NÃO cabe na área corrente, tenta a próxima.
	 */
	if (ap->a_free < nb)
	{
		if (ap >= &area[NAREAS])
		{
			error ("É preciso aumentar NAREAS (> %d)", NAREAS);
			return (NOVOID);
		}

		ap++;
		sz = (1 + (nb >> ALLOC_SHIFT)) << ALLOC_SHIFT;

		if (ap->a_total < sz)
		{
			/* Não tem tamanho suficiente */

			if (ap->a_area != NOVOID)
				free (ap->a_area);

			if ((cp = malloc (sz)) == NOVOID)
			{
				ap->a_area  = NOVOID;
				ap->a_total = ap->a_free = 0;

				return (NOVOID);
			}

#ifdef	DEBUG
			fprintf (stderr, "salloc: alocada área[%d] de %d bytes\n", ap - area, sz);
#endif
			ap->a_area = cp;
			ap->a_free = ap->a_total = sz;
		}

		areap = ap;
	}

	/*
	 *	Aloca um pedaço na área corrente.
	 */
	ap->a_free -= nb;

	return (ap->a_area + ap->a_free);

}	/* end salloc */

/*
 ****************************************************************
 *	Libera todos os pedaços alocados			*
 ****************************************************************
 */
void
sfree (void)
{
	ALLOC		*ap;

	for (ap = areap; ap > &area[0]; ap--)
		ap->a_free = ap->a_total;

	areap = ap;

#ifdef	DEBUG
	fprintf (stderr, "sfree: liberados %d bytes\n", totalsz);
#endif

}	/* end sfree */
