/*
 ****************************************************************
 *								*
 *			lalr.c					*
 *								*
 *	Compute look-ahead criteria for bison			*
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

/* Compute how to make the finite state machine deterministic;
 find which rules need lookahead in each state, and which lookahead tokens they accept.

lalr(), the entry point, builds these data structures:

goto_map, from_state and to_state 
 record each shift transition which accepts a variable (a nonterminal).
ngotos is the number of such transitions.
from_state[t] is the state number which a transition leads from
and to_state[t] is the state number it leads to.
All the transitions that accept a particular variable are grouped together and
goto_map[i - ntokens] is the index in from_state and to_state of the first of them.

consistent[s] is nonzero if no lookahead is needed to decide what to do in state s.

LAruleno is a vector which records the rules that need lookahead in various states.
The elements of LAruleno that apply to state s are those from
 lookaheads[s] through lookaheads[s+1]-1.
Each element of LAruleno is a rule number.

If lr is the length of LAruleno, then a number from 0 to lr-1 
can specify both a rule and a state where the rule might be applied.

LA is a lr by ntokens matrix of bits.
LA[l, i] is 1 if the rule LAruleno[l] is applicable in the appropriate state
 when the next token is symbol i.
If LA[l, i] and LA[l, j] are both 1 for i != j, it is a conflict.
*/

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/machine.h"
#include "../h/types.h"
#include "../h/state.h"
#include "../h/new.h"
#include "../h/gram.h"


/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern short **derives;
extern char *nullable;


int tokensetsize;
short *lookaheads;
short *LAruleno;
unsigned *LA;
short *accessing_symbol;
char *consistent;
core **state_table;
shifts **shift_table;
reductions **reduction_table;
short *goto_map;
short *from_state;
short *to_state;

short **transpose();
void set_state_table();
void set_accessing_symbol();
void set_shift_table();
void set_reduction_table();
void set_maxrhs();
void initialize_LA();
void set_goto_map();
void initialize_F();
void build_relations();
void add_lookback_edge();
void compute_FOLLOWS();
void compute_lookaheads();
void digraph();
void traverse();

extern void toomany();
extern void berror();

static int infinity;
static int maxrhs;
static int ngotos;
static unsigned *F;
static short **includes;
static shorts **lookback;
static short **R;
static short *INDEX;
static short *VERTICES;
static int top;


void
lalr (void)
{
  tokensetsize = WORDSIZE(ntokens);

  set_state_table();
  set_accessing_symbol();
  set_shift_table();
  set_reduction_table();
  set_maxrhs();
  initialize_LA();
  set_goto_map();
  initialize_F();
  build_relations();
  compute_FOLLOWS();
  compute_lookaheads();
}


void
set_state_table (void)
{
  core *sp;

  state_table = NEW2(nstates, core *);

  for (sp = first_state; sp; sp = sp->next)
    state_table[sp->number] = sp;
}


void
set_accessing_symbol (void)
{
  core *sp;

  accessing_symbol = NEW2(nstates, short);

  for (sp = first_state; sp; sp = sp->next)
    accessing_symbol[sp->number] = sp->accessing_symbol;
}


void
set_shift_table (void)
{
  shifts *sp;

  shift_table = NEW2(nstates, shifts *);

  for (sp = first_shift; sp; sp = sp->next)
    shift_table[sp->number] = sp;
}


void
set_reduction_table (void)
{
  reductions *rp;

  reduction_table = NEW2(nstates, reductions *);

  for (rp = first_reduction; rp; rp = rp->next)
    reduction_table[rp->number] = rp;
}


void
set_maxrhs (void)
{
  short *itemp;
  int length;
  int max;

  length = 0;
  max = 0;
  for (itemp = ritem; *itemp; itemp++)
    {
      if (*itemp > 0)
	{
	  length++;
	}
      else
	{
	  if (length > max) max = length;
	  length = 0;
	}
    }

  maxrhs = max;
}


void
initialize_LA (void)
{
  int i;
  int j;
  int count;
  reductions *rp;
  shifts *sp;
  short *np;

  consistent = NEW2(nstates, char);
  lookaheads = NEW2(nstates + 1, short);

  count = 0;
  for (i = 0; i < nstates; i++)
    {
      int k;

      lookaheads[i] = count;

      rp = reduction_table[i];
      sp = shift_table[i];
      if (rp && (rp->nreds > 1
          || (sp && ! ISVAR(accessing_symbol[sp->shifts[0]]))))
	count += rp->nreds;
      else
	consistent[i] = 1;

      if (sp)
	for (k = 0; k < sp->nshifts; k++)
	  {
	    if (accessing_symbol[sp->shifts[k]] == error_token_number)
	      {
		consistent[i] = 0;
		break;
	      }
	  }
    }

  lookaheads[nstates] = count;

  if (count == 0)
    {
      LA = NEW2(1 * tokensetsize, unsigned);
      LAruleno = NEW2(1, short);
      lookback = NEW2(1, shorts *);
    }
  else
    {
      LA = NEW2(count * tokensetsize, unsigned);
      LAruleno = NEW2(count, short);
      lookback = NEW2(count, shorts *);
    }

  np = LAruleno;
  for (i = 0; i < nstates; i++)
    {
      if (!consistent[i])
	{
	  if (rp = reduction_table[i])
	    for (j = 0; j < rp->nreds; j++)
	      *np++ = rp->rules[j];
	}
    }
}


void
set_goto_map (void)
{
  shifts *sp;
  int i;
  int symbol;
  int k;
  short *temp_map;
  int state2;
  int state1;

  goto_map = NEW2(nvars + 1, short) - ntokens;
  temp_map = NEW2(nvars + 1, short) - ntokens;

  ngotos = 0;
  for (sp = first_shift; sp; sp = sp->next)
    {
      for (i = sp->nshifts - 1; i >= 0; i--)
	{
	  symbol = accessing_symbol[sp->shifts[i]];

	  if (ISTOKEN(symbol)) break;

	  if (ngotos == MAXSHORT)
	    toomany("gotos");

	  ngotos++;
	  goto_map[symbol]++;
        }
    }

  k = 0;
  for (i = ntokens; i < nsyms; i++)
    {
      temp_map[i] = k;
      k += goto_map[i];
    }

  for (i = ntokens; i < nsyms; i++)
    goto_map[i] = temp_map[i];

  goto_map[nsyms] = ngotos;
  temp_map[nsyms] = ngotos;

  from_state = NEW2(ngotos, short);
  to_state = NEW2(ngotos, short);

  for (sp = first_shift; sp; sp = sp->next)
    {
      state1 = sp->number;
      for (i = sp->nshifts - 1; i >= 0; i--)
	{
	  state2 = sp->shifts[i];
	  symbol = accessing_symbol[state2];

	  if (ISTOKEN(symbol)) break;

	  k = temp_map[symbol]++;
	  from_state[k] = state1;
	  to_state[k] = state2;
	}
    }

  FREE(temp_map + ntokens);
}



/*  Map_goto maps a state/symbol pair into its numeric representation.	*/

int
map_goto (int state, int symbol)
{
  int high;
  int low;
  int middle;
  int s;

  low = goto_map[symbol];
  high = goto_map[symbol + 1] - 1;

  while (low <= high)
    {
      middle = (low + high) / 2;
      s = from_state[middle];
      if (s == state)
	return (middle);
      else if (s < state)
	low = middle + 1;
      else
	high = middle - 1;
    }

  berror("map_goto");
/* NOTREACHED */
  return 0;
}


void
initialize_F (void)
{
  int i;
  int j;
  int k;
  shifts *sp;
  short *edge;
  unsigned *rowp;
  short *rp;
  short **reads;
  int nedges;
  int stateno;
  int symbol;
  int nwords;

  nwords = ngotos * tokensetsize;
  F = NEW2(nwords, unsigned);

  reads = NEW2(ngotos, short *);
  edge = NEW2(ngotos + 1, short);
  nedges = 0;

  rowp = F;
  for (i = 0; i < ngotos; i++)
    {
      stateno = to_state[i];
      sp = shift_table[stateno];

      if (sp)
	{
	  k = sp->nshifts;

	  for (j = 0; j < k; j++)
	    {
	      symbol = accessing_symbol[sp->shifts[j]];
	      if (ISVAR(symbol))
		break;
	      SETBIT(rowp, symbol);
	    }

	  for (; j < k; j++)
	    {
	      symbol = accessing_symbol[sp->shifts[j]];
	      if (nullable[symbol])
		edge[nedges++] = map_goto(stateno, symbol);
	    }
	
	  if (nedges)
	    {
	      reads[i] = rp = NEW2(nedges + 1, short);

	      for (j = 0; j < nedges; j++)
		rp[j] = edge[j];

	      rp[nedges] = -1;
	      nedges = 0;
	    }
	}

      rowp += tokensetsize;
    }

  digraph(reads);

  for (i = 0; i < ngotos; i++)
	{ FREE(reads[i]); }

#if (0)	/*************************************/
  for (i = 0; i < ngotos; i++)
    {
      if (reads[i])
	FREE(reads[i]);
    }
#endif	/*************************************/

  FREE(reads);
  FREE(edge);
}


void
build_relations (void)
{
  int i;
  int j;
  int k;
  short *rulep;
  short *rp;
  shifts *sp;
  int length;
  int nedges;
  int Done;
  int state1;
  int stateno;
  int symbol1;
  int symbol2;
  short *shortp;
  short *edge;
  short *states;
  short **new_includes;

  includes = NEW2(ngotos, short *);
  edge = NEW2(ngotos + 1, short);
  states = NEW2(maxrhs + 1, short);

  for (i = 0; i < ngotos; i++)
    {
      nedges = 0;
      state1 = from_state[i];
      symbol1 = accessing_symbol[to_state[i]];

      for (rulep = derives[symbol1]; *rulep > 0; rulep++)
	{
	  length = 1;
	  states[0] = state1;
	  stateno = state1;

	  for (rp = ritem + rrhs[*rulep]; *rp > 0; rp++)
	    {
	      symbol2 = *rp;
	      sp = shift_table[stateno];
	      k = sp->nshifts;

	      for (j = 0; j < k; j++)
		{
		  stateno = sp->shifts[j];
		  if (accessing_symbol[stateno] == symbol2) break;
		}

	      states[length++] = stateno;
	    }

	  if (!consistent[stateno])
	    add_lookback_edge(stateno, *rulep, i);

	  length--;
	  Done = 0;
	  while (!Done)
	    {
	      Done = 1;
	      rp--;
			/* JF added rp>=ritem &&   I hope to god its right! */
	      if (rp>=ritem && ISVAR(*rp))
		{
		  stateno = states[--length];
		  edge[nedges++] = map_goto(stateno, *rp);
		  if (nullable[*rp]) Done = 0;
		}
	    }
	}

      if (nedges)
	{
	  includes[i] = shortp = NEW2(nedges + 1, short);
	  for (j = 0; j < nedges; j++)
	    shortp[j] = edge[j];
	  shortp[nedges] = -1;
	}
    }

  new_includes = transpose(includes, ngotos);

  for (i = 0; i < ngotos; i++)
      { FREE(includes[i]); }

#if (0)	/*************************************/
  for (i = 0; i < ngotos; i++)
    if (includes[i])
      FREE(includes[i]);
#endif	/*************************************/

  FREE(includes);

  includes = new_includes;

  FREE(edge);
  FREE(states);

}	/* end build_relations */


void
add_lookback_edge (int stateno, int ruleno, int gotono)
{
  int i;
  int k;
  int found;
  shorts *sp;

  i = lookaheads[stateno];
  k = lookaheads[stateno + 1];
  found = 0;
  while (!found && i < k)
    {
      if (LAruleno[i] == ruleno)
	found = 1;
      else
	i++;
    }

  if (found == 0)
    berror("add_lookback_edge");

  sp = NEW(shorts);
  sp->next = lookback[i];
  sp->value = gotono;
  lookback[i] = sp;
}



short **
transpose (short **R_arg, int n)
{
  short **new_R;
  short **temp_R;
  short *nedges;
  short *sp;
  int i;
  int k;

  nedges = NEW2(n, short);

  for (i = 0; i < n; i++)
    {
      sp = R_arg[i];
      if (sp)
	{
	  while (*sp >= 0)
	    nedges[*sp++]++;
	}
    }

  new_R = NEW2(n, short *);
  temp_R = NEW2(n, short *);

  for (i = 0; i < n; i++)
    {
      k = nedges[i];
      if (k > 0)
	{
	  sp = NEW2(k + 1, short);
	  new_R[i] = sp;
	  temp_R[i] = sp;
	  sp[k] = -1;
	}
    }

  FREE(nedges);

  for (i = 0; i < n; i++)
    {
      sp = R_arg[i];
      if (sp)
	{
	  while (*sp >= 0)
	    *temp_R[*sp++]++ = i;
	}
    }

  FREE(temp_R);

  return (new_R);
}


void
compute_FOLLOWS (void)
{
  int i;

  digraph(includes);

  for (i = 0; i < ngotos; i++)
    { FREE(includes[i]); }
#if (0)	/*************************************/
    {
      if (includes[i]) FREE(includes[i]);
    }
#endif	/*************************************/

  FREE(includes);
}


void
compute_lookaheads (void)
{
  int i;
  int n;
  unsigned *fp1;
  unsigned *fp2;
  unsigned *fp3;
  shorts *sp;
  unsigned *rowp;
/*   short *rulep; JF unused */
/*  int count; JF unused */
  shorts *sptmp;/* JF */

  rowp = LA;
  n = lookaheads[nstates];
  for (i = 0; i < n; i++)
    {
      fp3 = rowp + tokensetsize;
      for (sp = lookback[i]; sp; sp = sp->next)
	{
	  fp1 = rowp;
	  fp2 = F + tokensetsize * sp->value;
	  while (fp1 < fp3)
	    *fp1++ |= *fp2++;
	}

      rowp = fp3;
    }

  for (i = 0; i < n; i++)
    {/* JF removed ref to freed storage */
      for (sp = lookback[i]; sp; sp = sptmp)
      {
	sptmp=sp->next;
	FREE(sp);
      }
    }

  FREE(lookback);
  FREE(F);
}


void
digraph(relation)
short **relation;
{
  int i;

  infinity = ngotos + 2;
  INDEX = NEW2(ngotos + 1, short);
  VERTICES = NEW2(ngotos + 1, short);
  top = 0;

  R = relation;

  for (i = 0; i < ngotos; i++)
    INDEX[i] = 0;

  for (i = 0; i < ngotos; i++)
    {
      if (INDEX[i] == 0 && R[i])
	traverse(i);
    }

  FREE(INDEX);
  FREE(VERTICES);
}


void
traverse (int i)
{
  unsigned *fp1;
  unsigned *fp2;
  unsigned *fp3;
  int j;
  short *rp;

  int height;
  unsigned *base;

  VERTICES[++top] = i;
  INDEX[i] = height = top;

  base = F + i * tokensetsize;
  fp3 = base + tokensetsize;

  rp = R[i];
  if (rp)
    {
      while ((j = *rp++) >= 0)
	{
	  if (INDEX[j] == 0)
	    traverse(j);

	  if (INDEX[i] > INDEX[j])
	    INDEX[i] = INDEX[j];

	  fp1 = base;
	  fp2 = F + j * tokensetsize;

	  while (fp1 < fp3)
	    *fp1++ |= *fp2++;
	}
    }

  if (INDEX[i] == height)
    {
      for (;;)
	{
	  j = VERTICES[top--];
	  INDEX[j] = infinity;

	  if (i == j)
	    break;

	  fp1 = base;
	  fp2 = F + j * tokensetsize;

	  while (fp1 < fp3)
	    *fp2++ = *fp1++;
	}
    }
}
