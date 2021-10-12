/*
 ****************************************************************
 *								*
 *			derives.c				*
 *								*
 *	Match rules with nonterminals for bison			*
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

/* set_derives finds, for each variable (nonterminal), which rules can derive it.
   It sets up the value of derives so that
   derives[i - ntokens] points to a vector of rule numbers,
   terminated with -1.  */

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/new.h"
#include "../h/types.h"
#include "../h/gram.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
short **derives;

void
set_derives (void)
{
  int i;
  int lhs;
  shorts *p;
  short *q;
  shorts **dset;
  shorts *delts;

  dset = NEW2(nvars, shorts *) - ntokens;
  delts = NEW2(nrules + 1, shorts);

  p = delts;
  for (i = nrules; i > 0; i--)
    {
      lhs = rlhs[i];
      if (lhs >= 0)
	{
	  p->next = dset[lhs];
	  p->value = i;
	  dset[lhs] = p;
	  p++;
	}
    }

  derives = NEW2(nvars, short *) - ntokens;
  q = NEW2(nvars + nrules, short);

  for (i = ntokens; i < nsyms; i++)
    {
      derives[i] = q;
      p = dset[i];
      while (p)
	{
	  *q++ = p->value;
	  p = p->next;
	}
      *q++ = -1;
    }

#ifdef	DEBUG
  print_derives();
#endif

  FREE(dset + ntokens);
  FREE(delts);
}

#if (0)	/*************************************/
void
free_derives (void)
{
  FREE(derives[ntokens]);
  FREE(derives + ntokens);
}
#endif	/*************************************/



#ifdef	DEBUG

int
print_derives (void)
{
  int i;
  short *sp;

  extern char **tags;

  printf("\n\n\nDERIVES\n\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("%s derives", tags[i]);
      for (sp = derives[i]; *sp > 0; sp++)
	{
	  printf("  %d", *sp);
	}
      putchar('\n');
    }

  putchar('\n');
}

#endif

