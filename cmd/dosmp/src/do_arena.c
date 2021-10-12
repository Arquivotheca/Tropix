/*
 ****************************************************************
 *								*
 *			arena.c					*
 *								*
 *    Imprime e verifica a consistência da arena de "malloc"	*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 09.06.95				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/proto.h"

/*
 ****** Definição da ARENA **************************************
 */
#define	NOSTORE	(STORE *)NULL

#define	BUSY		1	/* O endereço é sempre par */

#define	TST_BUSY(p)	((p) &  BUSY)

#define	WITH_BUSY(p)	((STORE *)((int)(p) |  BUSY))
#define	WITHOUT_BUSY(p)	((STORE *)((int)(p) & ~BUSY))

#define	SET_BUSY(p)	((p) |=  BUSY)
#define	CLR_BUSY(p)	((p) &= ~BUSY)

#define	NEXT(p)		(STORE *)((int)(p)->m_ptr & ~BUSY)

typedef	union	store	STORE;

union	store
{
	int	m_status;	/* O bloco está ou não em uso */
	STORE	*m_ptr;		/* Aponta para o próximo bloco */
};

extern	STORE	_malloc_base[2]; /* Arena inicial */

extern	STORE	*_malloc_search;	/* Ptr para busca */
extern	STORE	*_malloc_top;		/* Topo da Arena */

/*
 ******	Variáveis externas **************************************
 */
extern int	xd_dump_addr;		/* End. do início da linha */

/*
 ******	Protótipos de funções ***********************************
 */
void		do_arena_help (void);

extern void	xd_print_line (const char *, int);

/*
 ****************************************************************
 *	Programa de teste de malloc				*
 ****************************************************************
 */
void
do_arena (int argc, const char *argv[])
{
	const STORE	*p;
	int		n;
	int		total_free = 0, total_busy = 0;
	int		opt;
	KPSZ		kpsz;
	extern char	etext, edata, end;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "vH")) >= 0)
	{
		switch (opt)
		{
		    case 'v':			/* Listagem completa */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_arena_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_arena_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 0)
		{ do_arena_help (); return; }

	xd_dump_addr = 0;

	/*
	 *	Verfica "chkalloc"
	 */
	if (chkalloc () < 0)
		printf ("Erro em chkalloc\n\n"); 
	else
		printf ("chkalloc OK\n\n"); 

	/*
	 *	Imprime a arena
	 */
	if (cmd_vflag)
		printf ("Arena:\n\n");

	p = &_malloc_base[0];

	do
	{
		if (received_intr)
			break;

		if (cmd_vflag)
			printf ("%d => %d\t", p, NEXT (p));

		n = 0;

		if (p == &_malloc_base[0])
		{
			if (cmd_vflag)
				printf ("(Bloco inicial):\t");
		}
		elif (NEXT (p) == &_malloc_base[0] || p == &_malloc_base[1])
		{
			if (cmd_vflag)
				printf ("(Bloco fictício):\t");
		}
		else
		{
			n = (NEXT (p) - p - 1) * sizeof (STORE);

			if (cmd_vflag)
				printf ("(%d bytes):\t\t", n);
		}

		if (TST_BUSY (p->m_status))
		{
			if (cmd_vflag)
				printf ("BUSY ***\n");

			if (n > 0 && cmd_vflag > 1)
				xd_print_line ((char *)(p + 1), n > 16 ? 16 : n);

			total_busy += n;
		}
		else
		{
			if (cmd_vflag)
				printf ("FREE\n");

			total_free += n;
		}

		p = NEXT (p);
	}
	while (p != &_malloc_base[0]);

	if (cmd_vflag)
	{
		printf
		(	"\ntop: %d,  search: %d\n\n",
			_malloc_top, _malloc_search
		);
	}

	/*
	 *	Imprime os totais
	 */
	printf ("total_busy = %d, total_free = %d\n\n", total_busy, total_free);

	/*
	 *	Imprime a Memória ocupada
	 */
	kcntl (K_GETPSZ, &kpsz);

#ifdef	PC
	printf
	(
		"Texto = %d, Data = %d, Bss = (%d + %d), "
		"Stack = %d, Total = %d\n",
		(int)&etext - 0x400000,
		(int)&edata - 0x20000000,
		(int)&end - (int)&edata,
		(int)_cbreak - (int)&end,
		kpsz.k_ssize,
		(int)&etext - 0x400000 + (int)_cbreak - 0x20000000 + kpsz.k_ssize
	);
#else
	printf
	(
		"Texto = %d, Data = %d, Bss = (%d + %d), "
		"Stack = %d, Total = %d\n",
		(int)&etext,
		(int)&edata - 0x200000,
		(int)&end - (int)&edata,
		(int)_cbreak - (int)&end,
		kpsz.k_ssize,
		(int)&etext + (int)_cbreak - 0x200000 + kpsz.k_ssize
	);
#endif	PC

}	/* end do_arena */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_arena_help (void)
{
	fprintf
	(	stderr,
		"%s - imprime e verifica a consistência da arena de \"malloc\"\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Imprime as entradas da arena\n"
	);

}	/* end do_arena_help */
