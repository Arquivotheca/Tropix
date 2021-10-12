/*
 ****************************************************************
 *								*
 *			obstack.c				*
 *								*
 *	Subroutines used implicitly by object stack macros	*
 *								*
 *	Versão	3.0.0, de 31.08.93				*
 *		3.0.0, de 31.08.93				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

/*
 ******	Inclusões de arquivos ***********************************
 */
#include "../h/obstack.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */

#define	elif	else if

/*
 *	Determine default alignment.
 */
struct fooalign {char x; double d;};

#define DEFAULT_ALIGNMENT ((char *)&((struct fooalign *) 0)->d - (char *)0)

/*
 *	If malloc were really smart, it would round addresses
 *	to DEFAULT_ALIGNMENT. But in fact it might be less smart
 *	and round addresses to as much as DEFAULT_ROUNDING.
 *	So we prepare for it to do that.
 */
union fooround {long x; double d;};

#define DEFAULT_ROUNDING (sizeof (union fooround))

/*
 *	When we copy a long block of data, this is the unit to do it with.
 *	On some machines, copying successive ints does not work;
 *	in such a case, redefine COPYING_UNIT to `long' (if that works)
 *	or `char' as a last resort.
 */
#ifndef COPYING_UNIT
#define COPYING_UNIT int
#endif

/*
 *	The non-GNU-C macros copy the obstack into this global variable
 *	to avoid multiple evaluation.
 */
OBSTACK		*_obstack;

/*
 ******	Protótipos de funções ***********************************
 */
extern void	abort (void);

/*
 ****************************************************************
 *	Initialize an obstack H for use				*
 ****************************************************************
 */
/*
 *	Specify chunk size SIZE (0 means default).
 *	Objects start on multiples of ALIGNMENT (0 means use default).
 *	CHUNKFUN is the function to use to allocate chunks,
 *	and FREEFUN the function to free them.
 */
void
_obstack_begin (OBSTACK *h, int size, int alignment,
				void * (*chunkfun) (), void (*freefun) ())
{
	register _OBSTACK_CHUNK	*chunk; /* points to new chunk */

	if (alignment == 0)
		alignment = DEFAULT_ALIGNMENT;

	/*
	 *	Default size is what GNU malloc can fit in a
	 *	4096-byte block. Pick a number small enough that
	 *	when rounded up to DEFAULT_ROUNDING it is still
	 *	smaller than 4096 - 4.
	 */
	if (size == 0)
	{
		int extra = 4;

		if (extra < DEFAULT_ROUNDING)
			extra = DEFAULT_ROUNDING;

		size = 4096 - extra;
	}

	h->chunkfun = (_OBSTACK_CHUNK * (*)())chunkfun;
	h->freefun = freefun;
	h->chunk_size = size;
	h->alignment_mask = alignment - 1;

	chunk = h->chunk = (*h->chunkfun) (h->chunk_size);
	h->next_free = h->object_base = chunk->contents;
	h->chunk_limit = chunk->limit = (char *) chunk + h->chunk_size;
	chunk->prev = 0;

}	/* end _obstack_begin */

/*
 ****************************************************************
 *	Allocate a new current chunk				*
 ****************************************************************
 */
/*
 *	Allocate a new current chunk for the obstack *H
 *	on the assumption that LENGTH bytes need to be added
 *	to the current object, or a new object of length LENGTH
 *	allocated. Copies any partial object from the end of the
 *	old chunk to the beginning of the new one.
 */
void
_obstack_newchunk (OBSTACK *h, int length)
{
	register _OBSTACK_CHUNK	*old_chunk = h->chunk;
	register _OBSTACK_CHUNK	*new_chunk;
	register long		new_size;
	register int		obj_size = h->next_free - h->object_base;
	register int 		i;
	int			already;

	/*
	 *	Compute size for new chunk.
	 */
	new_size = (obj_size + length) + (obj_size >> 3) + 100;

	if (new_size < h->chunk_size)
		new_size = h->chunk_size;

	/*
	 *	Allocate and initialize the new chunk.
	 */
	new_chunk = h->chunk = (*h->chunkfun) (new_size);
	new_chunk->prev = old_chunk;
	new_chunk->limit = h->chunk_limit = (char *)new_chunk + new_size;

	/*
	 *	Move the existing object to the new chunk.
	 *	word at a time is fast and is safe if the object
	 *	is sufficiently aligned.
	 */
	if (h->alignment_mask + 1 >= DEFAULT_ALIGNMENT)
	{
		for (i = obj_size / sizeof (COPYING_UNIT) - 1; i >= 0; i--)
		{
			((COPYING_UNIT *)new_chunk->contents)[i]
				= ((COPYING_UNIT *)h->object_base)[i];
		}

		/*
		 *	We used to copy the odd few remaining bytes
		 *	as one extra COPYING_UNIT, but that can cross
		 *	a page boundary on a machine which does not do
		 *	strict alignment for COPYING_UNITS.
		 */
		already = obj_size / sizeof (COPYING_UNIT) * sizeof (COPYING_UNIT);
	}
	else
	{
		already = 0;
	}

	/*
	 *	Copy remaining bytes one by one.
	 */
	for (i = already; i < obj_size; i++)
		new_chunk->contents[i] = h->object_base[i];

	h->object_base = new_chunk->contents;
	h->next_free = h->object_base + obj_size;

}	/* end _obstack_newchunk */

/*
 ****************************************************************
 *	Return nonzero if object OBJ has been allocated		*
 ****************************************************************
 */
/*
 *	Return nonzero if object OBJ has been allocated
 *	from obstack H. This is here for debugging.
 *	If you use it in a program, you are probably losing.
 */
int
_obstack_allocated_p (OBSTACK *h, void *obj)
{
	register _OBSTACK_CHUNK	*lp; /* below addr of any objects in this chunk */
	register _OBSTACK_CHUNK	*plp;/* point to previous chunk if any */

	lp = (h)->chunk;

	while (lp != 0 && ((void *)lp > obj || (void *)(lp)->limit < obj))
	{
		plp = lp -> prev;
		lp = plp;
	}

	return lp != 0;

}	/* end _obstack_allocated_p */

/*
 ****************************************************************
 *	Free objects in obstack					*
 ****************************************************************
 */
/*
 *	Free objects in obstack H, including OBJ and everything
 *	allocate more recently than OBJ.  If OBJ is zero,
 *	free everything in H.
 */
#undef obstack_free

void
obstack_free (OBSTACK *h, void * obj)
{
	register _OBSTACK_CHUNK	*lp; /* below addr of any objects in this chunk */
	register _OBSTACK_CHUNK	*plp;/* point to previous chunk if any */

	lp = (h)->chunk;

	while (lp != 0 && ((void *)lp > obj || (void *)(lp)->limit < obj))
	{
		plp = lp -> prev;
		(*h->freefun) (lp);
		lp = plp;
	}

	if (lp)
	{
		(h)->object_base = (h)->next_free = (char *)(obj);
		(h)->chunk_limit = lp->limit;
		(h)->chunk = lp;
	}
	elif (obj != 0) /* obj is not in any of the chunks! */
	{
		abort ();
	}

}	/* end obstack_free */

/*
 ****************************************************************
 *	Let same .o link with output of gcc and other compilers *
 ****************************************************************
 */
void
_obstack_free (OBSTACK *h, void * obj)
{
	obstack_free (h, obj);

}	/* end _obstack_free */
