/*
 ****************************************************************
 *								*
 *			warshall.c				*
 *								*
 *	Generate transitive closure of a matrix			*
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

#include "../h/common.h"
#include "../h/machine.h"


/* given n by n matrix of bits R, modify its contents
   to be the transive closure of what was given.  */

void
TC (unsigned *R, int n)
{
  int rowsize;
  unsigned mask;
  unsigned *rowj;
  unsigned *rp;
  unsigned *rend;
  unsigned *ccol;

  unsigned *relend;
  unsigned *cword;
  unsigned *rowi;

  rowsize = WORDSIZE(n) * sizeof(unsigned);
  relend = (unsigned *) ((char *) R + (n * rowsize));

  cword = R;
  mask = 1;
  rowi = R;
  while (rowi < relend)
    {
      ccol = cword;
      rowj = R;

      while (rowj < relend)
	{
	  if (*ccol & mask)
	    {
	      rp = rowi;
	      rend = (unsigned *) ((char *) rowj + rowsize);

	      while (rowj < rend)
		*rowj++ |= *rp++;
	    }
	  else
	    {
	      rowj = (unsigned *) ((char *) rowj + rowsize);
	    }

	  ccol = (unsigned *) ((char *) ccol + rowsize);
	}

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  cword++;
	}

      rowi = (unsigned *) ((char *) rowi + rowsize);
    }
}


/* Reflexive Transitive Closure.  Same as TC
   and then set all the bits on the diagonal of R.  */

void
RTC (unsigned *R, int n)
{
  int rowsize;
  unsigned mask;
  unsigned *rp;
  unsigned *relend;

  TC(R, n);

  rowsize = WORDSIZE(n) * sizeof(unsigned);
  relend = (unsigned *) ((char *) R + n*rowsize);

  mask = 1;
  rp = R;
  while (rp < relend)
    {
      *rp |= mask;

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  rp++;
	}

      rp = (unsigned *) ((char *) rp + rowsize);
    }
}
