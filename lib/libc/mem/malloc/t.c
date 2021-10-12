/*
 ****************************************************************
 *	Teste de "malloc", ...					*
 ****************************************************************
 */
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>

#define		EVER	;;
#define		elif	else if

/*
 ****** Definição da ARENA **************************************
 */
#define	PGSZ		4096		/* Incremento para "sbrk" */
#define	PGMASK		(PGSZ-1)	/* Máscara para "sbrk" */

#define	BUSY		1		/* O endereço é sempre par */

#define	IS_BUSY(p)	((p) &  BUSY)

#define	WITH_BUSY(p)	((STORE *)((int)(p) |  BUSY))
#define	WITHOUT_BUSY(p)	((STORE *)((int)(p) & ~BUSY))

#define	SET_BUSY(p)	((p) |=  BUSY)
#define	CLR_BUSY(p)	((p) &= ~BUSY)

#define NEXT(p)		(STORE *)((int)(p)->m_ptr & ~BUSY)

typedef	union store	STORE;

union store
{
	int	m_status;		/* O bloco está ou não em uso */
	STORE	*m_ptr;			/* Aponta para o próximo bloco */
};

#define	STORE_SHIFT	2		/* LOG (2) (sizeof (STORE)) */

#define	NOSTORE		(STORE *)NULL

extern	STORE	*_malloc_base;		/* Primeiro elemento da Arena */
extern	STORE	*_malloc_top;		/* Último elemento da Arena */

extern	STORE	*_malloc_search;	/* Ptr para busca */

/*
 ****************************************************************
 *	Programa de teste de malloc				*
 ****************************************************************
 */
main ()
{
	int		nbytes, n;
	char		*addr;
	register STORE	*p;
	char		buferr[512];
	void		*old_break;
	int		index;

	setbuf (stderr, buferr);

	for (EVER)
	{
		printf ("malloc (1, m), free (2, f), realloc (3, r), chkalloc (4, c), brk (5, b) ? ");

		switch (getchar ())
		{
		    case '1':
		    case 'm':
			printf ("\nEntre no de bytes: ");
			scanf ("%d", &nbytes);

			old_break = _cbreak;
			addr = malloc (nbytes);
			printf ("\nold_break = %P, new_break = %P, ", old_break, _cbreak); 
			printf ("Área alocada: %P\n", addr);
			break;

		    case '2':
		    case 'f':
			printf ("\nEntre o número da área: ");
			scanf ("%d", &n);

			for (p = _malloc_base, index = 0; /* abaixo */; /* abaixo */)
			{
				if (index++ == n)
					{ free (p + 1); break; }

				if ((p = NEXT (p)) == _malloc_base)
					{ printf ("Índice inválido\n"); break; }
			}

#if (0)	/*******************************************************/
			printf ("\nEntre endereço: ");
			scanf ("%X", &addr);
			free (addr);
#endif	/*******************************************************/
			break;

		    case '3':
		    case 'r':
			printf ("\nEntre o número da área: ");
			scanf ("%d", &n);

			for (p = _malloc_base, index = 0; /* abaixo */; /* abaixo */)
			{
				if (index++ == n)
				{
					printf ("Entre no de bytes: ");
					scanf ("%d", &nbytes);
					addr = realloc (p + 1, nbytes);
					printf ("Área realocada: %P\n", addr);
					break;
				}

				if ((p = NEXT (p)) == _malloc_base)
					{ printf ("Índice inválido\n"); break; }
			}

#if (0)	/*******************************************************/
			printf ("\nEntre endereço: ");
			scanf ("%X", &addr);
#endif	/*******************************************************/
			break;

		    case '4':
		    case 'c':
			break;

		    case '5':
		    case 'b':
			sbrk (1024);
			break;

		    case 'q':
		    case EOF:
			printf ("\nAdeus\n");
			exit (0);

		    default:
			while (getchar () != '\n')
				/* vazio */;

			putchar ('\n');
			continue;
		}

		while (getchar () != '\n')
			/* vazio */;

		if (chkalloc () < 0)
			printf ("Erro em chkalloc\n"); 

		printf ("\nArena:\n\n");

		p = _malloc_base; index = 0;

		do
		{
			printf ("%2d:  %s\t", index++, p == _malloc_search ? "=>" : "  ");

			printf ("%P => %P\t", p, NEXT (p));

			if (IS_BUSY (p->m_status))
				printf ("\tBUSY\t");
			else
				printf ("\tFREE\t");

			if (p != _malloc_top)
				printf ("%6d", (NEXT (p) - p - 1) * sizeof (STORE));

			putchar ('\n');

			p = NEXT (p);
		}
		while (p != _malloc_base);

		printf ("\nbreak = %P\n\n", _cbreak);

#if (0)	/*******************************************************/
		printf
		(	"\nbase = %P, search = %P, top = %P, cbreak = %P\n\n",
			_malloc_base, _malloc_search, _malloc_top, _cbreak
		);
#endif	/*******************************************************/
	}

}	/* end main */
