/*
 ****************************************************************
 *								*
 *			timeout.c				*
 *								*
 *	Imprime informações de TIMEOUT				*
 *								*
 *	Versão	3.0.0, de x.98				*
 *		3.0.0, de x.98				*
 *								*
 *	Módulo: timeout						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/a.out.h>
#include <sys/kcntl.h>
#include <sys/timeout.h>
#include <sys/tty.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <id.h>
#include <errno.h>

/*
 ****************************************************************
 *	Imprime informações de TIMEOUT				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0, de x.98";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry const char *pgname;	/* Nome do programa */

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		map_and_print (const idp_t, int, const char *);
void		map_and_print_tty (const idp_t lid, int n, const char *nm);
void		help (void);

/*
 ****************************************************************
 *	Imprime informações de LOCK sobre os PTYs		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	int			opt, n;
	lid_t			lid;
	SYM			sym;
	const char		*ptr;
	const TIMEOUT_HASH	*tp;
	ulong			abs_ticks, timeout_abs_ticks;
	ulong			timeout_global_count, timeout_busy_count, timeout_overflow_count;
	ulong			map_busy_count;

	pgname = argv[0];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	x
	 */
	lidcan (lid, "_abs_ticks");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	abs_ticks = *(ulong *)ptr;

	phys (ptr, 0, 0);

	lidcan (lid, "_timeout_abs_ticks");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	timeout_abs_ticks = *(ulong *)ptr;

	phys (ptr, 0, 0);

	printf ("abs_ticks = %d, timeout_abs_ticks = %d\n", abs_ticks, timeout_abs_ticks);

	/*
	 *	x
	 */
	lidcan (lid, "_timeout_global_count");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	timeout_global_count = *(ulong *)ptr;

#if (0)	/*******************************************************/
	printf ("%s: addr = %P, value = %d\n", sym.s_name, sym.s_value, *(ulong *)ptr);
#endif	/*******************************************************/

	phys (ptr, 0, 0);

	/*
	 *	x
	 */
	lidcan (lid, "_timeout_busy_count");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	timeout_busy_count = *(ulong *)ptr;

	phys (ptr, 0, 0);

	/*
	 *	x
	 */
	lidcan (lid, "_timeout_overflow_count");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	timeout_overflow_count = *(ulong *)ptr;

#if (0)	/*******************************************************/
	printf ("%s: addr = %P, value = %d\n", sym.s_name, sym.s_value, *(ulong *)ptr);
#endif	/*******************************************************/

	phys (ptr, 0, 0);

	printf
	(	"timeout_global_count = %d, timeout_busy_count = %d, timeout_overflow_count = %d\n",
		timeout_global_count, timeout_busy_count, timeout_overflow_count
	);

	/*
	 *	x
	 */
	lidcan (lid, "_map_busy_count");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/

	if ((int)(ptr = phys ((void *)sym.s_value, sizeof (long), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	map_busy_count = *(ulong *)ptr;

	phys (ptr, 0, 0);

	/*
	 *	x
	 */
	lidcan (lid, "_timeout_hash");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: addr = %P\n\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/


	if
	(	(int)(tp = phys
			((void *)sym.s_value,
			TIMEOUT_HASH_SZ * sizeof (TIMEOUT_HASH),
			O_RDONLY|O_KERNEL)) == -1
	)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, lid, strerror (errno)
		);
		exit (1);
	}

	printf ("\n");

	for (n = 0; n < TIMEOUT_HASH_SZ; n++, tp++)
	{
		if (n == TIMEOUT_HASH_SZ / 2)
			printf ("\n");

		printf ("%d ", tp->o_count);
	}

	printf ("\n");

	phys (ptr, 0, 0);

	printf ("\nmap_busy_count = %d\n", map_busy_count);

	return (exit_code);

}	/* end timeout */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - programa\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-opções] arg\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-x: opção\n"
		"\t-y: opção\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
		"\nObs.:\tobservação\n"
	);

	exit (2);

}	/* end help */
