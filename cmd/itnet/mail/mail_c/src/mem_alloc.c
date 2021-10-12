/*
 ****************************************************************
 *								*
 *			mem_alloc.c				*
 *								*
 *	Alocação dinamica de memória				*
 *								*
 *	Versão	4.0.0, de 23.08.00				*
 *		4.0.0, de 23.08.00				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Definições e variáveis globais				*
 ****************************************************************
 */
#define	INCR		4192	/* Tamanho Basico para alocação */

#define	elif	else if

#define	ROUND(x) ((x + 3) & ~3)	/* Arredondamento do tamanho */

entry void	*mem0,		/* Ponteiros para a área disponível */
		*mem1,
		*mem2;

/*
 ****************************************************************
 *	Alocação de Memória					*
 ****************************************************************
 */
void *
mem_alloc (int size)
{
	void		*addr, *status;

	size = ROUND (size);

	/*
	 *	Verifica se há espaço
	 */
	while (mem1 + size > mem2)
	{
		if   ((int)(status = sbrk (INCR)) == -1)
		{
			error (NOSTR);
		}
		elif (mem0 == NOVOID)
		{
			mem0 = mem1 = status;	/* Primeira vez */
		}
		elif (status != mem2)
		{
			error ("$mem_alloc: Recebi área NÃO adjacente");
		}

		mem2 = status + INCR;
	}

	/*
	 *	Acerta os indicadores
	 */
	addr = mem1; mem1 += size;

	return (addr);

}	/* end mem_alloc */

/*
 ****************************************************************
 *	Devolve um trecho não usado de memória			*
 ****************************************************************
 */
void
mem_free (void	*a)
{
	a = (void *)ROUND ((int)a);

	if (a >= mem0 && a <= mem1)
		{ mem1 = a; return; }

	error ("mem_free: Devolução inválida de memória: %P\n", a);

}	/* end mem_free */
