/*
 ****************************************************************
 *								*
 *			allocate.c				*
 *								*
 *	Allocate and clear storage for bison			*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL

extern void done();

/*
 ****************************************************************
 *	Aloca memória						*
 ****************************************************************
 */
char *
mallocate (unsigned n)
{
  char *block;

  /* Avoid uncertainty about what an arg of 0 will do.  */
  if (n == 0)
    n = 1;
  block = calloc(n,1);
  if (block == NOSTR)
    {
      fprintf(stderr, "%s: memory exhausted\n", pgname);
      done(1);
    }

  return (block);
}
