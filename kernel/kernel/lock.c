/*
 ****************************************************************
 *								*
 *			lock.c					*
 *								*
 *	Imprime informações de LOCK sobre os PTYs		*
 *								*
 *	Versão	3.0.0, de x.98				*
 *		3.0.0, de x.98				*
 *								*
 *	Módulo: lock						*
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
#include <sys/tty.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <id.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
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
	register int	opt;
	lid_t		lid;

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
	lidcan (lid, "_con_nopen"); map_and_print  (lid,  8, " con_nopen");
	lidcan (lid, "_con_lock");  map_and_print  (lid,  8, "  con_lock");
	printf ("\n");

	lidcan (lid, "_sio_nopen"); map_and_print  (lid,  8, " sio_nopen");
	lidcan (lid, "_sio_lock");  map_and_print  (lid,  8, "  sio_lock");
	printf ("\n");

	lidcan (lid, "_sio"); map_and_print_tty  (lid,  2,  " sio_state");
	lidcan (lid, "_slip"); map_and_print_tty  (lid,  1, "slip_state");
	lidcan (lid, "_ppp"); map_and_print_tty  (lid,  1,  " ppp_state");
	printf ("\n");

	lidcan (lid, "_ptyc_nopen"); map_and_print (lid, 16, "ptyc_nopen");
	lidcan (lid, "_ptyc_lock");  map_and_print (lid, 16, " ptyc_lock");
	printf ("\n");

	lidcan (lid, "_ptys_nopen"); map_and_print (lid, 16, "ptys_nopen");
	lidcan (lid, "_ptys_lock");  map_and_print (lid, 16, " ptys_lock");

	exit (exit_code);

}	/* end lock */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
map_and_print (const idp_t lid, int n, const char *nm)
{
	register int	i;
	const char	*ptr;
	SYM		sym;

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P\n", sym.s_name, sym.s_value);
#endif	/*******************************************************/

	/*
	 *	Mapeia o vetor "nm"
	 */
	if ((int)(ptr = phys ((void *)sym.s_value, n, O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Imprime o vetor "nm"
	 */
	printf ("%s: ", nm);

	for (i = 0; i < n; i++)
		printf (" %2d", ptr[i]);

	printf ("\n");

	phys (ptr, 0, 0);

}	/* end map_and_print */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
map_and_print_tty (const idp_t lid, int n, const char *nm)
{
	register int		i;
	const TTY		*ptr;
	register const TTY	*tp;
	SYM			sym;

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o endereço de \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Mapeia o vetor "nm"
	 */
	if ((int)(ptr = tp = phys ((void *)sym.s_value, 8 * sizeof (TTY), O_RDONLY|O_KERNEL)) == -1)
	{
		fprintf
		(	stderr,
			"%s: Não consegui mapear \"%s\" (%s)\n",
			pgname, nm, strerror (errno)
		);
		exit (1);
	}

#if (0)	/*******************************************************/
	printf ("%s: %P -> %P\n", sym.s_name, sym.s_value, tp);
#endif	/*******************************************************/

	/*
	 *	Imprime o vetor "nm"
	 */
	printf ("%s: ", nm);

	for (i = 0; i < 8; i++, tp += n)
	{
		printf (" %04X", tp->t_state);
	}

	printf ("\n");

	phys (ptr, 0, 0);

}	/* end map_and_print */

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
