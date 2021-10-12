/*
 ****************************************************************
 *								*
 *			nullable.c				*
 *								*
 *	Part of the bison parser generator			*
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

/* set up nullable, a vector saying which nonterminals can expand into the null string.
   nullable[i - ntokens] is nonzero if symbol i can do so.  */

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/types.h"
#include "../h/gram.h"
#include "../h/new.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
char *nullable;


void
set_nullable (void)
{
  short *r;
  short *s1;
  short *s2;
  int ruleno;
  int symbol;
  shorts *p;

  short *squeue;
  short *rcount;
  shorts **rsets;
  shorts *relts;
  char any_tokens;
  short *r1;

#ifdef	TRACE
  fprintf(stderr, "Entering set_nullable");
#endif

  nullable = NEW2(nvars, char) - ntokens;

  squeue = NEW2(nvars, short);
  s1 = s2 = squeue;

  rcount = NEW2(nrules + 1, short);
  rsets = NEW2(nvars, shorts *) - ntokens;
  /* This is said to be more elements than we actually use.
     Supposedly nitems - nrules is enough.
     But why take the risk?  */
  relts = NEW2(nitems + nvars + 1, shorts);
  p = relts;

  r = ritem;
  while (*r)
    {
      if (*r < 0)
	{
	  symbol = rlhs[-(*r++)];
	  if (symbol >= 0 && !nullable[symbol])
	    {
	      nullable[symbol] = 1;
	      *s2++ = symbol;
	    }
	}
      else
	{
	  r1 = r;
	  any_tokens = 0;
	  for (symbol = *r++; symbol > 0; symbol = *r++)
	    {
	      if (ISTOKEN(symbol))
		any_tokens = 1;
	    }

	  if (!any_tokens)
	    {
	      ruleno = -symbol;
	      r = r1;
	      for (symbol = *r++; symbol > 0; symbol = *r++)
		{
		  rcount[ruleno]++;
		  p->next = rsets[symbol];
		  p->value = ruleno;
		  rsets[symbol] = p;
		  p++;
		}
	    }
	}
    }

  while (s1 < s2)
    {
      p = rsets[*s1++];
      while (p)
	{
	  ruleno = p->value;
	  p = p->next;
	  if (--rcount[ruleno] == 0)
	    {
	      symbol = rlhs[ruleno];
	      if (symbol >= 0 && !nullable[symbol])
		{
		  nullable[symbol] = 1;
		  *s2++ = symbol;
		}
	    }
	}
    }

  FREE(squeue);
  FREE(rcount);
  FREE(rsets + ntokens);
  FREE(relts);
}


#if (0)	/*************************************/
void
free_nullable (void)
{
  FREE(nullable + ntokens);
}
#endif	/*************************************/
