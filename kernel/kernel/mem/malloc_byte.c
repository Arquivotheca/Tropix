/*
 ****************************************************************
 *								*
 *			malloc_byte.c				*
 *								*
 *	Alocação dinâmica de memória				*
 *								*
 *	Versão	4.2.0, de 25.10.01				*
 *		4.2.0, de 03.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"

#include "../h/map.h"

#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	DEBUG
#define	SAVE
#undef	MSGs

/*
 ****** Definição da ARENA **************************************
 */
enum {	BUSY = 0x01 };		/* Região ocupada */
enum {	HOLE = 0x02 };		/* Pulo sobre regiões inexistentes */

#define	IS_BUSY(p)		((p) & BUSY)
#define	IS_HOLE(p)		((p) & HOLE)
#define	IS_BUSY_AND_HOLE(p)	((p) & (BUSY|HOLE) == (BUSY|HOLE))

/* Repare que sempre que atribuímos o HOLE, atribuímos também o BUSY */

#define	WITH_BUSY(p)		((STORE *)((int)(p) | BUSY))
#define	WITH_BUSY_AND_HOLE(p)	((STORE *)((int)(p) | BUSY|HOLE))
#define	ADDR(p)			((STORE *)((int)(p) & ~(BUSY|HOLE)))

#define	CLR_BUSY_AND_HOLE(p)	((p) &= ~(BUSY|HOLE))

typedef	union store	STORE;

union store
{
	int	m_status;		/* O bloco está ou não em uso */
	STORE	*m_ptr;			/* Aponta para o próximo bloco */
};

enum {	STORE_SHIFT = 2 };		/* LOG (2) (sizeof (STORE)) */

#define	NOSTORE		(STORE *)NULL

entry	STORE	*malloc_base;		/* Primeiro elemento da Arena */
entry	STORE	*malloc_top;		/* Último elemento da Arena */
entry	STORE	*malloc_search;		/* Ponteiro para busca */

entry	LOCK	malloc_lock;		/* O Semáforo da lista */

/*
 ****** Depuração ***********************************************
 */
#ifdef	DEBUG

#define ASSERT(expr)	if ((expr) == 0)			\
			{					\
				SPINFREE (&malloc_lock);	\
				malloc_assert (__LINE__);	\
				return (NOVOID);		\
			}

extern void	malloc_assert (int);

#else

#define ASSERT(expr)

#endif	DEBUG

void	check_arena (const char *, int);

/*
 ****************************************************************
 *	Alocação dinâmica de memória				*
 ****************************************************************
 */
void *
malloc_byte (int byte_count)
{
	STORE		*b, *t, *p, *q, *r, *s;
	int		store_count, cnt;
	pg_t		pgsz, pgno;

	/*
	 *	Calcula o número de STOREs necessários
	 *	(leva em conta o próprio ponteiro, e arredonda)
	 */
	store_count = (byte_count + 2 * sizeof (STORE) - 1) >> STORE_SHIFT;

	SPINLOCK (&malloc_lock);

	/*
	 *	Verifica se é a primeira vez
	 */
	if (malloc_base == NOSTORE)
	{
		pgsz = BYUPPG ((store_count + 1) << STORE_SHIFT);

		if ((pgno = malloce (M_CORE, pgsz)) == NOPG)
			{ SPINFREE (&malloc_lock); return (NOVOID); }

		p = (STORE *)PGTOBY (pgno);
		q = (STORE *)PGTOBY (pgno + pgsz) - 1;

		malloc_base = malloc_search = p; malloc_top = q;

		p->m_ptr = q; q->m_ptr = WITH_BUSY_AND_HOLE (p);
	}

	ASSERT (malloc_search >= malloc_base && malloc_search <= malloc_top);

	/*
	 *	Executa uma malha até encontrar um bloco de tamanho adequado
	 */
	for (p = malloc_search; /* vazio */; /* vazio */)
	{
		for (cnt = 0; /* abaixo */; /* abaixo */)
		{
			if (!IS_BUSY (p->m_status))
			{
				/*
				 *	Aglutina, se for o caso
				 */
				while (q = p->m_ptr, !IS_BUSY (q->m_status))
				{
					ASSERT (q > p && q < malloc_top);
					p->m_ptr = q->m_ptr;
				}

				/*
				 *	Verifica se o tamanho é suficiente
				 */
				if (q >= (r = p + store_count))
				{
					malloc_search = r;

					ASSERT (r <= malloc_top);

					if (q > r)
						r->m_ptr = q;

					p->m_ptr = WITH_BUSY (r);
#ifdef	SAVE
					check_arena ("malloc_byte (1)", 0);
#endif	SAVE
					SPINFREE (&malloc_lock);

					return (p + 1);
				}

			}	/* end if (aglutinando) */

			/*
			 *	Tenta o próximo bloco
			 */
			q = p;	p = ADDR (p->m_ptr);

			if   (p > q)
				{ ASSERT (p <= malloc_top); }
			elif (q != malloc_top || p != malloc_base)
				{ ASSERT (0); }
			elif (cnt++ > 0)
				break;

		}	/* end for (duas voltas) */

		/*
		 *	Não encontrou um bloco adequado
		 *	Pede mais memória à "malloc" de páginas
		 */
		SPINFREE (&malloc_lock);

		pgsz = BYUPPG ((store_count + 1) << STORE_SHIFT);

		if ((pgno = malloce (M_CORE, pgsz)) == NOPG)
			return (NOVOID);
#ifdef	MSGs
		printf ("%g: size = %d KB, addr = %d KB\n", PGTOKB (pgsz), PGTOKB (pgno - BYTOPG (SYS_ADDR)));
#endif	MSGs
		p = (STORE *)PGTOBY (pgno);
		q = (STORE *)PGTOBY (pgno + pgsz) - 1;

		SPINLOCK (&malloc_lock);

		b = malloc_base;
		t = malloc_top;

		/* Verifica se a área nova é grudada na anterior */

		if   (q < b)
		{
			t->m_ptr = WITH_BUSY_AND_HOLE (p);

			if   (q + 1 != b)
				{ p->m_ptr = q; q->m_ptr = WITH_BUSY_AND_HOLE (b); }
			elif (IS_BUSY (b->m_status))
				p->m_ptr = b;
			else
				p->m_ptr = b->m_ptr;

			malloc_base = b = p;
		}
		elif (p > t)
		{
			if (t + 1 != p)
				{ t->m_ptr = WITH_BUSY_AND_HOLE (p); p->m_ptr = q; }
			else
				t->m_ptr = q;

			malloc_top = t = q; t->m_ptr = WITH_BUSY_AND_HOLE (b);
		}
		else for (r = b; /* abaixo */; r = s)
		{
			if (r >= t)
			{
				printf ("%g: Não achei o local de inserção\n");
				SPINFREE (&malloc_lock); return (NOVOID);
			}

			s = ADDR (r->m_ptr);

			if (p <= r || q >= s)
				continue;

			ASSERT (IS_BUSY_AND_HOLE (r->m_status));

			if (r + 1 == p)
				r->m_ptr = p;
			else
				r->m_ptr = WITH_BUSY_AND_HOLE (p);

			if (q + 1 == s)
				p->m_ptr = s;
			else
				{ p->m_ptr = q; q->m_ptr = WITH_BUSY_AND_HOLE (s); }

			break;
		}

		ASSERT (t > b);

		malloc_search = p = b;
#ifdef	SAVE
		check_arena ("malloc_byte (2)", 0);
#endif	SAVE

	}	/* end for (EVER) */

}	/* end malloc_byte */

/*
 ****************************************************************
 *	Realocação de memória alocada dinâmicamente		*
 ****************************************************************
 */
void *
realloc_byte (void *area, int byte_count)
{
	STORE		*p, *q, *r, *s;
	int		old_store_count, new_store_count;

	/*
	 *	Verifica se foi dado o endereço
	 */
	if (area == NOVOID)
		return (malloc_byte (byte_count));

	p = area - sizeof (STORE);

	if (!IS_BUSY (p->m_status))
		{ printf ("%g: A área NÃO está alocada\n"); return (NOVOID); }

	SPINLOCK (&malloc_lock);

	old_store_count = ADDR (p->m_ptr) - p;

	new_store_count = (byte_count + 2 * sizeof (STORE) - 1) >> STORE_SHIFT;

	/*
	 *	Aglutina os blocos seguintes
	 */
	q = ADDR (p->m_ptr);

	while (!IS_BUSY (q->m_status))
		q = q->m_ptr;

	/*
	 *	Verifica se o espaço é suficiente
	 */
	if (q >= (r = p + new_store_count))
	{
		malloc_search = r;

		ASSERT (r <= malloc_top);

		if (q > r)
			r->m_ptr = q;

		p->m_ptr = WITH_BUSY (r);

		SPINFREE (&malloc_lock);
		return (p + 1);
	}

	SPINFREE (&malloc_lock);

#if (0)	/*******************************************************/
	STORE		*p, *q, *r, *s, *t;
	/*
	 *	Não foi suficiente a seguir; tenta antes
	 */
	for (t = malloc_base; (r = ADDR (t->m_ptr)) != q; /* abaixo */)
	{
		ASSERT (r > t && r < malloc_top);

		if (IS_BUSY (t->m_status))
			{ t = r; continue; }

		s = (r == p) ? q : r;

		if (s - t >= new_store_count)
		{
			CLR_BUSY_AND_HOLE (p->m_status);

			memmove (t + 1, p + 1, (old_store_count - 1) << STORE_SHIFT);

			malloc_search = r = t + new_store_count;

			if (s > r)
				r->m_ptr = s;

			t->m_ptr = WITH_BUSY (r);

			SPINFREE (&malloc_lock);
			return (t + 1);
		}

		if (IS_BUSY (r->m_status))
			t = r;
		else
			t->m_ptr = r->m_ptr;
	}
#endif	/*******************************************************/
		
	/*
	 *	Não há espaço
	 */
	if ((s = malloc_byte (byte_count)) == NOVOID)
		return (NOVOID);

	/*
	 *	Alocou em uma área nova
	 */
	memmove (s, p + 1, (old_store_count - 1) << STORE_SHIFT);

	free_byte (p + 1);
	
	return (s);

}	/* end realloc_byte */

/*
 ****************************************************************
 *	Liberação de memória alocada dinâmicamente		*
 ****************************************************************
 */
void
free_byte (void *area)
{
	STORE		*p, *q, *a, *n;
	pg_t		pgsz, pgno;
	int		sz;

	/*
	 *	Consistência inicial
	 */
	if (area == NOVOID)
		{ printf ("%g: Endereço NULO\n"); return; }

	p = (STORE *)area - 1;

	SPINLOCK (&malloc_lock);

	if (p < malloc_base || p >= malloc_top)
{
print_calls ();
		{ printf ("%g: Endereço inválido: %P\n", area); goto out; }
}

	if (!IS_BUSY (p->m_status))
		{ printf ("%g: Reliberação de um bloco\n"); goto out; }

	if (IS_HOLE (p->m_status))
		{ printf ("%g: Liberação de um buraco\n"); goto out; }

	CLR_BUSY_AND_HOLE (p->m_status);

	if (p->m_ptr <= p || p->m_ptr > malloc_top)
		{ printf ("%g: Arena inconsistente\n"); goto out; }

	malloc_search = p;

	/*
	 *	Aglutina e devolve, se for o caso
	 */
    again:
	for (a = NOSTORE, p = malloc_base; p != malloc_top; a = p, p = q)
	{
		if (IS_BUSY (p->m_status))
			{ q = ADDR (p->m_ptr); continue; }

		while (q = p->m_ptr, !IS_BUSY (q->m_status))
		{
			if (malloc_search == q)
				malloc_search = p;

			p->m_ptr = q->m_ptr;
		}

		if (((int)p & PGMASK) != 0 || !IS_HOLE (q->m_status))
			continue;

		if (a != NOSTORE && !IS_HOLE (a->m_status))
			continue;

		/* Achou uma região a devolver */

		n = ADDR (q->m_ptr);

		if   (p == malloc_base)
		{
			if (q == malloc_top)
				{ malloc_base = malloc_top = NOSTORE; }
			else
				{ malloc_base = n; malloc_top->m_ptr = WITH_BUSY_AND_HOLE (n); }
		}
		else	/* p != malloc_base */
		{
			if (q == malloc_top)
				{ malloc_top = a; a->m_ptr = WITH_BUSY_AND_HOLE (malloc_base); 	}
			else
				{ a->m_ptr = WITH_BUSY_AND_HOLE (n); }
		}

		malloc_search = malloc_base;
#ifdef	SAVE
		check_arena ("free_byte", 0);
#endif	SAVE
		SPINFREE (&malloc_lock);

		/*
		 *	Devolve a região
		 */
		if ((sz = (int)(q + 1) - (int )p) & PGMASK)
			{ printf ("%g: Tamanho %d NÃO múltiplo de página\n", sz); return; }

		pgno = BYTOPG (p);
		pgsz = BYTOPG (sz);

		mrelease (M_CORE, pgsz, pgno);
#ifdef	MSGs
		printf ("%g: size = %d KB, addr = %d KB\n", PGTOKB (pgsz), PGTOKB (pgno - BYTOPG (SYS_ADDR)));
#endif	MSGs
		SPINLOCK (&malloc_lock);

		goto again;

	}	/* end aglutinando e liberando */

	/*
	 *	Libera a lista e retorna
	 */
    out:
	SPINFREE (&malloc_lock);

}	/* end free_byte */


#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
malloc_assert (int line)
{
	printf ("%g: Arena inconsistente, linha %d\n", line);

}	/* end malloc_assert */

#endif	DEBUG

/*
 ****************************************************************
 *	Imprime e verifica a consistência da ARENA		*
 ****************************************************************
 */
void
check_arena (const char *nm, int print_flag)
{
	STORE		*p, *q;
	int		index = 0, search_found = 0;
	const char	*str;

	if (malloc_base == NOSTORE)
	{
		if (print_flag)
			printf ("%g: %s: ARENA Vazia\n", nm);

		return;
	}

	for (p = malloc_base; (q = ADDR (p->m_ptr)) > p; p = q)
	{
		if (p == malloc_search)
			str = "=>", search_found++;
		else
			str = "  ";

		if (print_flag)
		{
			printf ("%2d:  %s\t", index++, str);

			printf ("%P => %P\t", p, q);

			if   (IS_HOLE (p->m_status))
				printf ("\t----\t");
			elif (IS_BUSY (p->m_status))
				printf ("\tBUSY\t");
			else
				printf ("\tFREE\t");

			if (p != malloc_top)
				printf ("%6d", (q - p - 1) * sizeof (STORE));

			putchar ('\n');
		}
	}

	if (p != malloc_top)
		printf ("%g: %s: ARENA NÃO termina em malloc_top\n", nm);

	if (ADDR (malloc_top->m_ptr) != malloc_base)
		printf ("%g: %s: malloc_top NÃO aponta para malloc_base\n", nm);

	if (search_found != 1 && malloc_search != p)
		printf ("%g: %s: malloc_search NÃO encontrado\n", nm);

}	/* end check_arena */
