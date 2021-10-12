/*
 ****************************************************************
 *								*
 *			qtsort.c				*
 *								*
 *	Função de ordenação usando "quicksort" e tags		*
 *								*
 *	Versão	1.0.0, de 09.09.86				*
 *		3.0.0, de 23.06.95				*
 *								*
 *	Módulo: qtsort						*
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
 *	Definições e variáveis globais				*
 ****************************************************************
 */
#define	TROCA(i,j)	{				\
				temp  = base[i];	\
				base[i] = base[j];	\
				base[j] = temp;		\
			}

static void 	**global_base;
static int	(*global_comp) (const void *, const void *);

/*
 ******	Protótipos de funções ***********************************
 */
static void	sort (int, int);

/*
 ****************************************************************
 *	Função de ordenação usando "quicksort" e tags		*
 ****************************************************************
 */
void
qtsort (void *base, int nel, int (*comp) (const void *, const void *))
{
	if (nel < 2)
		return;

	global_base = base;	/* Na verdade "void **base" */
	global_comp = comp;

	sort (0, nel - 1);

}	/* end qtsort */
 
/*
 ****************************************************************
 *	Função Recursiva usando o Quicksort			*
 ****************************************************************
 */
static void
sort (register int e, register int d)
{
	register int 	i, j;
	register char	*temp, *p;
	register void 	**base;
	register int	(*comp) (const void *, const void *);

	comp = global_comp;
	base = global_base;

	i = e; j = d; p = base[(e + d) >> 1];

	do
	{
		while ((*comp) (base[i], p) < 0)
			i++;

		while ((*comp) (p, base[j]) < 0)
			j--;

		if (i <= j)
			{ TROCA (i, j); i++; j--; }

	}
	while (i <= j);

	/*
	 *	Mantem a pilha com tamanho mínimo
	 */
	if (j - e < d - i)
	{
		/*
		 *	Examina se a recusão vale a pena
		 */
		if (e == j - 1)
		{
			/*
			 *	Se a diferença é 1, compara logo
			 */
			if ((*comp) (base[e], base[j]) > 0)
				TROCA (e, j);
		}
		else
		{
			/*
			 *	Caso contrário chama recursivamente
			 */	
			if (e < j)
				sort (e, j);
		}

		if (i == d - 1)
		{
			if ((*comp) (base[i], base[d]) > 0)
				TROCA (i, d);
		}
		else
		{
			if (i < d)
				sort (i, d);
		}
	}
	else
	{
		if (i == d - 1)
		{
			if ((*comp) (base[i], base[d]) > 0)
				TROCA (i, d);
		}
		else
		{
			if (i < d)
				sort (i, d);
		}

		if (e == j - 1)
		{
			if ((*comp) (base[e], base[j]) > 0)
				TROCA (e, j);
		}
		else
		{
			if (e < j)
				sort (e, j);
		}
	}

}	/* end sort */
