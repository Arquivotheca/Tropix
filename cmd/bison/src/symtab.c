/*
 ****************************************************************
 *								*
 *			symtab.c				*
 *								*
 *	Symbol table manager for Bison				*
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
#include <string.h>

#include "../h/common.h"
#include "../h/new.h"
#include "../h/symtab.h"
#include "../h/gram.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
bucket **symtab;
bucket *firstsymbol;
bucket *lastsymbol;



int
hash (char *key)
{
  char *cp;
  int k;

  cp = key;
  k = 0;
  while (*cp)
    k = ((k << 1) ^ (*cp++)) & 0x3fff;

  return (k % TABSIZE);
}



char *
copys (char *s)
{
  int i;
  char *cp;
  char *result;

  i = 1;
  for (cp = s; *cp; cp++)
    i++;

  result =  mallocate((unsigned int)i);
  strcpy(result, s);
  return (result);
}


void
tabinit (void)
{
/*   int i; JF unused */

  symtab = NEW2(TABSIZE, bucket *);

  firstsymbol = (bucket *)NULL;
  lastsymbol = (bucket *)NULL;
}


bucket *
getsym (char *key)
{
  int hashval;
  bucket *bp;
  int found;

  hashval = hash(key);
  bp = symtab[hashval];

  found = 0;
  while (bp != (bucket *)NULL && found == 0)
    {
      if (strcmp(key, bp->tag) == 0)
	found = 1;
      else
	bp = bp->link;
    }

  if (found == 0)
    {
      nsyms++;

      bp = NEW(bucket);
      bp->link = symtab[hashval];
      bp->next = (bucket *)NULL;
      bp->tag = copys(key);
      bp->class = SUNKNOWN;

      if (firstsymbol == (bucket *)NULL)
	{
	  firstsymbol = bp;
	  lastsymbol = bp;
	}
      else
	{
	  lastsymbol->next = bp;
	  lastsymbol = bp;
	}

      symtab[hashval] = bp;
    }

  return (bp);
}


void
free_symtab (void)
{
  int i;
  bucket *bp,*bptmp;/* JF don't use ptr after free */

  for (i = 0; i < TABSIZE; i++)
    {
      bp = symtab[i];
      while (bp)
	{
	  bptmp = bp->link;
#if 0 /* This causes crashes because one string can appear more than once.  */
	  FREE(bp->type_name);
#endif
	  FREE(bp);
	  bp = bptmp;
	}
    }
  FREE(symtab);
}
