/*
 ****************************************************************
 *								*
 *			obstack.h				*
 *								*
 *	Object stack macros					*
 *								*
 *	Versão	3.0.0, de 01.10.93				*
 *		3.0.0, de 08.10.93				*
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
 ******	Explanação: *********************************************
 *	
 *	All the apparent functions defined here are macros.  The
 *	idea  is  that  you would use these pre-tested macros to
 *	solve a very specific set of problems,  and  they  would
 *	run fast. Caution: no side-effects in arguments please!!
 *	They may be evaluated MANY times!!
 *	
 *	These  macros  operate  a  stack of objects. Each object
 *	starts life small, and may grow to  maturity.  (Consider
 *	building  a  word  syllable  by syllable.) An object can
 *	move while it is growing. Once it has been "finished" it
 *	never changes address again. So the "top of  the  stack"
 *	is  typically an immature growing object, while the rest
 *	of the stack is of mature, fixed size and fixed  address
 *	objects.
 *	
 *	These  routines  grab  large  chunks  of memory, using a
 *	function you supply,  called  `obstack_chunk_alloc'.  On
 *	occasion,     they     free     chunks,    by    calling
 *	`obstack_chunk_free'. You must define them  and  declare
 *	them before using any obstack macros.
 *	
 *	Each  independent  stack  is  represented  by  a `struct
 *	obstack'. Each of the obstack macros expects  a  pointer
 *	to such a structure as the first argument.
 *	
 *	One  motivation  for  this  package  is  the  problem of
 *	growing char strings in symbol tables.  Unless  you  are
 *	"fascist  pig  with a read-only mind" [Gosper's immortal
 *	quote from HAKMEM item 154, out of  context]  you  would
 *	not  like to put any arbitrary upper limit on the length
 *	of your symbols.
 *	
 *	In practice this often means you will build  many  short
 *	symbols  and  a  few  long  symbols. At the time you are
 *	reading a symbol you don't know  how  long  it  is.  One
 *	traditional  method  is  to read a symbol into a buffer,
 *	realloc()ating the buffer every time you try to  read  a
 *	symbol  that  is  longer than the buffer. This is beaut,
 *	but you still will want to  copy  the  symbol  from  the
 *	buffer  to a more permanent symbol-table entry say about
 *	half the time.
 *	
 *	With obstacks, you can work differently. Use one obstack
 *	for all symbol names. As you read  a  symbol,  grow  the
 *	name   in  the  obstack  gradually.  When  the  name  is
 *	complete,  finalize  it.  Then,  if  the  symbol  exists
 *	already, free the newly read name.
 *	
 *	The  way we do this is to take a large chunk, allocating
 *	memory from low addresses. When  you  want  to  build  a
 *	symbol in the chunk you just add chars above the current
 *	"high  water  mark" in the chunk. When you have finished
 *	adding chars, because you got to the end of the  symbol,
 *	you  know  how  long the chars are, and you can create a
 *	new object. Mostly the chars will  not  burst  over  the
 *	highest   address   of  the  chunk,  because  you  would
 *	typically expect a chunk to be (say) 100 times  as  long
 *	as an average object.
 *	
 *	In  case  that isn't clear, when we have enough chars to
 *	make up the object, THEY ARE ALREADY CONTIGUOUS  IN  THE
 *	CHUNK (guaranteed) so we just point to it where it lies.
 *	No moving of chars is needed and this is the second win:
 *	potentially   long  strings  need  never  be  explicitly
 *	shuffled. Once an object is formed, it does  not  change
 *	its address during its lifetime.
 *	
 *	When  the chars burst over a chunk boundary, we allocate
 *	a larger chunk, and then copy the partly  formed  object
 *	from  the  end  of the old chunk to the beginning of the
 *	new larger chunk. We then carry on accreting  characters
 *	to the end of the object as we normally would.
 *	
 *	A  special  macro  is provided to add a single char at a
 *	time to  a  growing  object.  This  allows  the  use  of
 *	register  variables,  which  break the ordinary 'growth'
 *	macro.
 *	
 *	Summary:
 *		* We allocate large chunks.
 *		* We carve out one object at  a  time  from  the
 *		  current chunk.
 *		* Once carved, an object never moves.
 *		* We  are free to append data of any size to the
 *		  currently growing object.
 *		* Exactly one object is growing in an obstack at
 *		  any one time.
 *		* You can run one obstack per control block.
 *		* You may have as many control blocks as you dare.
 *		* Because of the way we do it, you can `unwind' a
 *		  obstack  back  to  a  previous  state. (You may
 *		  remove objects much as you would with a stack.)
 */	

/*
 *	Don't do the contents of this file more than once.
 */
#ifndef __OBSTACKS__
#define __OBSTACKS__

/*
 *	We use subtraction of (char *)0 instead of casting to int
 *	because on word-addressable machines a simple cast to int
 *	may ignore the byte-within-word field of the pointer.
 */
#ifndef __PTR_TO_INT
#define __PTR_TO_INT(P) ((P) - (char *)0)
#endif

#ifndef __INT_TO_PTR
#define __INT_TO_PTR(P) ((P) + (char *)0)
#endif

/*
 ******	Estruturas de controle **********************************
 */
typedef struct _obstack_chunk	_OBSTACK_CHUNK;

struct _obstack_chunk			/* Lives at front of each chunk. */
{
	char		*limit;		/* 1 past end of this chunk */
	_OBSTACK_CHUNK	*prev;		/* address of prior chunk or NULL */
	char		contents[4];	/* objects begin here */
};

typedef struct obstack		OBSTACK;

struct obstack			/* control current object in current chunk */
{
	long		chunk_size;	/* preferred size to allocate chunks in */
	_OBSTACK_CHUNK	*chunk;		/* address of current struct obstack_chunk */
	char		*object_base;	/* address of object we are building */
	char		*next_free;	/* where to add next char to current object */
	char		*chunk_limit;	/* address of char after current chunk */
	int		temp;		/* Temporary for some macros. */
	int		alignment_mask;	/* Mask of alignment for each object. */
	_OBSTACK_CHUNK	*(*chunkfun) ();/* User's fcn to allocate a chunk. */
	void		(*freefun) ();	/* User's function to free a chunk. */
};

/*
 ******	Protótipos de funções ***********************************
 */
void	obstack_init (OBSTACK *obstack);

void	*obstack_alloc (OBSTACK *obstack, int size);

void	*obstack_copy (OBSTACK *obstack, void *address, int size);
void	*obstack_copy0 (OBSTACK *obstack, void *address, int size);

void	obstack_free (OBSTACK *obstack, void *block);

void	obstack_blank (OBSTACK *obstack, int size);

void	obstack_grow (OBSTACK *obstack, void *data, int size);
void	obstack_grow0 (OBSTACK *obstack, void *data, int size);

void	obstack_1grow (OBSTACK *obstack, int data_char);
void	obstack_ptr_grow (OBSTACK *obstack, void *data);
void	obstack_int_grow (OBSTACK *obstack, int data);

void	*obstack_finish (OBSTACK *obstack);

int	obstack_object_size (OBSTACK *obstack);

int	obstack_room (OBSTACK *obstack);
void	obstack_1grow_fast (OBSTACK *obstack, int data_char);
void	obstack_ptr_grow_fast (OBSTACK *obstack, void *data);
void	obstack_int_grow_fast (OBSTACK *obstack, int data);
void	obstack_blank_fast (OBSTACK *obstack, int size);

void	*obstack_base (OBSTACK *obstack);
void	*obstack_next_free (OBSTACK *obstack);
int	obstack_alignment_mask (OBSTACK *obstack);
int	obstack_chunk_size (OBSTACK *obstack);

void	_obstack_begin (OBSTACK *h, int size, int alignment,
				void * (*chunkfun) (), void (*freefun) ());
void	_obstack_newchunk (OBSTACK *h, int length);

/*
 *	Pointer to beginning of object being allocated or
 *	to be allocated next. Note that this might not be
 *	the final address of the object because a new chunk
 *	might be needed to hold the final size.
 */
#define obstack_base(h) ((h)->object_base)

/*
 *	Size for allocating ordinary chunks.
 */
#define obstack_chunk_size(h) ((h)->chunk_size)

/*
 *	Pointer to next byte not yet allocated in current chunk.
 */
#define obstack_next_free(h)	((h)->next_free)

/*
 *	Mask specifying low bits that should be clear
 *	in address of an object.
 */
#define obstack_alignment_mask(h) ((h)->alignment_mask)

#define obstack_init(h) \
  _obstack_begin ((h), 0, 0, obstack_chunk_alloc, obstack_chunk_free)

#define obstack_begin(h, size) \
  _obstack_begin ((h), (size), 0, obstack_chunk_alloc, obstack_chunk_free)

#define obstack_1grow_fast(h,achar) (*((h)->next_free)++ = achar)

#define obstack_blank_fast(h,n) ((h)->next_free += (n))

/*
 *	The non-GNU macros copy the obstack-pointer into
 *	this global variable to avoid multiple evaluation.
 */
extern OBSTACK	*_obstack;

#define obstack_object_size(h) \
 (unsigned) (_obstack = (h), (h)->next_free - (h)->object_base)

#define obstack_room(h)		\
 (unsigned) (_obstack = (h), (h)->chunk_limit - (h)->next_free)

#define obstack_grow(h, where, length)					\
( (h)->temp = (length),							\
  (((h)->next_free + (h)->temp > (h)->chunk_limit)			\
   ? _obstack_newchunk ((h), (h)->temp) : (void)0),			\
  memmove ((h)->next_free, where, (h)->temp),				\
  (h)->next_free += (h)->temp)

#define obstack_grow0(h,where,length)					\
( (h)->temp = (length),							\
  (((h)->next_free + (h)->temp + 1 > (h)->chunk_limit)			\
   ? _obstack_newchunk ((h), (h)->temp + 1) : (void)0),			\
  memmove ((h)->next_free, where, (h)->temp),				\
  (h)->next_free += (h)->temp,						\
  *((h)->next_free)++ = 0)

#define obstack_1grow(h,datum)						\
( (((h)->next_free + 1 > (h)->chunk_limit)				\
   ? _obstack_newchunk ((h), 1) : (void)0),				\
  *((h)->next_free)++ = (datum))

#define obstack_ptr_grow(h,datum)					\
( (((h)->next_free + sizeof (char *) > (h)->chunk_limit)		\
   ? _obstack_newchunk ((h), sizeof (char *)) : (void)0),		\
  *((char **)(h)->next_free)++ = ((char *)datum))

#define obstack_int_grow(h,datum)					\
( (((h)->next_free + sizeof (int) > (h)->chunk_limit)			\
   ? _obstack_newchunk ((h), sizeof (int)) : (void)0),			\
  *((int *)(h)->next_free)++ = ((int)datum))

#define obstack_ptr_grow_fast(h, aptr)					\
			(*((char **)(h)->next_free)++ = (char *)aptr)
#define obstack_int_grow_fast(h, aint)					\
			(*((int *)(h)->next_free)++ = (int)aint)

#define obstack_blank(h,length)						\
( (h)->temp = (length),							\
  (((h)->next_free + (h)->temp > (h)->chunk_limit)			\
   ? _obstack_newchunk ((h), (h)->temp) : (void)0),			\
  (h)->next_free += (h)->temp)

#define obstack_alloc(h,length)						\
 (obstack_blank ((h), (length)), obstack_finish ((h)))

#define obstack_copy(h,where,length)					\
 (obstack_grow ((h), (where), (length)), obstack_finish ((h)))

#define obstack_copy0(h,where,length)					\
 (obstack_grow0 ((h), (where), (length)), obstack_finish ((h)))

#define obstack_finish(h)  						\
( (h)->temp = __PTR_TO_INT ((h)->object_base),				\
  (h)->next_free							\
    = __INT_TO_PTR ((__PTR_TO_INT ((h)->next_free)+(h)->alignment_mask)	\
		    & ~ ((h)->alignment_mask)),				\
  (((h)->next_free - (char *)(h)->chunk					\
    > (h)->chunk_limit - (char *)(h)->chunk)				\
   ? ((h)->next_free = (h)->chunk_limit) : (char *)0),			\
  (h)->object_base = (h)->next_free,					\
  __INT_TO_PTR ((h)->temp))

#define obstack_free(h,obj)						\
( (h)->temp = (char *)(obj) - (char *) (h)->chunk,			\
  (((h)->temp >= 0 && (h)->temp < (h)->chunk_limit - (char *) (h)->chunk)\
   ? (int) ((h)->next_free = (h)->object_base				\
	    = (h)->temp + (char *) (h)->chunk)				\
   : ((obstack_free) ((h), (h)->temp + (char *) (h)->chunk), 0)))

#endif /* not __OBSTACKS__ */
