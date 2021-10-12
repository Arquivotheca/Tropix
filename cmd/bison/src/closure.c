/*
 ****************************************************************
 *								*
 *			closure.c				*
 *								*
 *	Subroutines for bison					*
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

/* subroutines of file LR0.c.

Entry points:

  closure (items, n)

Given a vector of item numbers items, of length n,
set up ruleset and itemset to indicate what rules could be run
and which items could be accepted when those items are the active ones.

ruleset contains a bit for each rule.  closure sets the bits
for all rules which could potentially describe the next input to be read.

itemset is a vector of item numbers; itemsetend points to just beyond the end
 of the part of it that is significant.
closure places there the indices of all items which represent units of
input that could arrive next.

  initialize_closure (n)

Allocates the itemset and ruleset vectors,
and precomputes useful data so that closure can be called.
n is the number of elements to allocate for itemset.

  finalize_closure ()

Frees itemset, ruleset and internal data.

*/

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/machine.h"
#include "../h/new.h"
#include "../h/gram.h"


/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern short **derives;
extern char **tags;

void set_fderives();
void set_firsts();

extern void RTC();

short *itemset;
short *itemsetend;
static unsigned *ruleset;

/* internal data.  See comments before set_fderives and set_firsts.  */
static unsigned *fderives;
static unsigned *firsts;

/* number of words required to hold a bit for each rule */
static int rulesetsize;

/* number of words required to hold a bit for each variable */
static int varsetsize;


void
initialize_closure (int n)
{
  itemset = NEW2(n, short);

  rulesetsize = WORDSIZE(nrules + 1);
  ruleset = NEW2(rulesetsize, unsigned);

  set_fderives();
}



/* set fderives to an nvars by nrules matrix of bits
   indicating which rules can help derive the beginning of the data
   for each nonterminal.  For example, if symbol 5 can be derived as
   the sequence of symbols 8 3 20, and one of the rules for deriving
   symbol 8 is rule 4, then the [5 - ntokens, 4] bit in fderives is set.  */
void
set_fderives (void)
{
  unsigned *rrow;
  unsigned *vrow;
  int j;
  unsigned cword;
  short *rp;
  int b;

  int ruleno;
  int i;

  fderives = NEW2(nvars * rulesetsize, unsigned) - ntokens * rulesetsize;

  set_firsts();

  rrow = fderives + ntokens * rulesetsize;

  for (i = ntokens; i < nsyms; i++)
    {
      vrow = firsts + ((i - ntokens) * varsetsize);
      cword = *vrow++;
      b = 0;
      for (j = ntokens; j < nsyms; j++)
	{
	  if (cword & (1 << b))
	    {
	      rp = derives[j];
	      while ((ruleno = *rp++) > 0)
		{
		  SETBIT(rrow, ruleno);
		}
	    }

	  b++;
	  if (b >= BITS_PER_WORD && j + 1 < nsyms)
	    {
	      cword = *vrow++;
	      b = 0;
	    }
	}

      rrow += rulesetsize;
    }

#ifdef	DEBUG
  print_fderives();
#endif

  FREE(firsts);
}



/* set firsts to be an nvars by nvars bit matrix indicating which items
   can represent the beginning of the input corresponding to which other items.
   For example, if some rule expands symbol 5 into the sequence of symbols 8 3 20,
   the symbol 8 can be the beginning of the data for symbol 5,
   so the bit [8 - ntokens, 5 - ntokens] in firsts is set. */
void
set_firsts (void)
{
  unsigned *row;
/*   int done; JF unused */
  int symbol;
  short *sp;
  int rowsize;

  int i;

  varsetsize = rowsize = WORDSIZE(nvars);

  firsts = NEW2(nvars * rowsize, unsigned);

  row = firsts;
  for (i = ntokens; i < nsyms; i++)
    {
      sp = derives[i];
      while (*sp >= 0)
	{
	  symbol = ritem[rrhs[*sp++]];
	  if (ISVAR(symbol))
	    {
	      symbol -= ntokens;
	      SETBIT(row, symbol);
	    }
	}

      row += rowsize;
    }

  RTC(firsts, nvars);

#ifdef	DEBUG
  print_firsts();
#endif
}


void
closure (short *core, int n)
{
  int ruleno;
  unsigned word;
  short *csp;
  unsigned *dsp;
  unsigned *rsp;

  short *csend;
  unsigned *rsend;
  int symbol;
  int itemno;

  rsp = ruleset;
  rsend = ruleset + rulesetsize;
  csend = core + n;

  if (n == 0)
    {
      dsp = fderives + start_symbol * rulesetsize;
      while (rsp < rsend)
	*rsp++ = *dsp++;
    }
  else
    {
      while (rsp < rsend)
	*rsp++ = 0;

      csp = core;
      while (csp < csend)
	{
	  symbol = ritem[*csp++];
	  if (ISVAR(symbol))
	    {
	      dsp = fderives + symbol * rulesetsize;
	      rsp = ruleset;
	      while (rsp < rsend)
		*rsp++ |= *dsp++;
	    }
	}
    }

  ruleno = 0;
  itemsetend = itemset;
  csp = core;
  rsp = ruleset;
  while (rsp < rsend)
    {
      word = *rsp++;
      if (word == 0)
	{
	  ruleno += BITS_PER_WORD;
	}
      else
	{
	  int b;

	  for (b = 0; b < BITS_PER_WORD; b++)
	    {
	      if (word & (1 << b))
		{
		  itemno = rrhs[ruleno];
		  while (csp < csend && *csp < itemno)
		    *itemsetend++ = *csp++;
		  *itemsetend++ = itemno;
		}

	      ruleno++;
	    }
	}
    }

  while (csp < csend)
    *itemsetend++ = *csp++;

#ifdef	DEBUG
  print_closure(n);
#endif
}


void
finalize_closure (void)
{
  FREE(itemset);
  FREE(ruleset);
  FREE(fderives + ntokens * rulesetsize);
}



#ifdef	DEBUG

int
print_closure (int n)
{
  short *isp;

  printf("\n\nn = %d\n\n", n);
  for (isp = itemset; isp < itemsetend; isp++)
    printf("   %d\n", *isp);
}



int
print_firsts (void)
{
  int i;
  int j;
  unsigned *rowp;

  printf("\n\n\nFIRSTS\n\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("\n\n%s firsts\n\n", tags[i]);

      rowp = firsts + ((i - ntokens) * varsetsize);

      for (j = 0; j < nvars; j++)
	if (BITISSET (rowp, j))
	  printf("   %s\n", tags[j + ntokens]);
    }
}



int
print_fderives (void)
{
  int i;
  int j;
  unsigned *rp;

  printf("\n\n\nFDERIVES\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("\n\n%s derives\n\n", tags[i]);
      rp = fderives + i * rulesetsize;

      for (j = 0; j <= nrules; j++)
	if (BITISSET (rp, j))
	  printf("   %d\n", j);
    }

  fflush(stdout);
}

#endif
