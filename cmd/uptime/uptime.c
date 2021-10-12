/*
 ****************************************************************
 *								*
 *			uptime.c				*
 *								*
 *	Fornece o tempo decorrido desde o último "boot"		*
 *								*
 *	Versao	1.0.0, de 09.02.87				*
 *		3.0.1, de 14.07.97				*
 *								*
 *	Módulo: uptime						*
 *		Utilitários administrativos			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.1, de 14.07.97";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Fornece o tempo decorrido desde o último "boot"		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	int			dias, hor, min, seg; 
	time_t			boot_time;
	TMS			tms;

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

	/*
	 *	Pequena consistência
	 */
   /***	argv += optind; ***/
	argc -= optind;

	if (argc > 0)
		help ();

	/*
	 *	Obtém o tempo decorrido
	 */
	if ((seg = times (&tms)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro na chamada \"times\" (%s)\n",
			strerror (errno)
		);
		exit (1);
	} 

	/*
	 *	Obtém a hora do "boot"
	 */
	boot_time = time ((time_t *)NULL) - seg;

	/*
	 *	Calcula o tempo decorrido em dias, horas, ...
	 */
	min  = seg / 60;
	seg  = seg % 60;

	hor  = min / 60;
	min  = min % 60;

	dias = hor / 24;
	hor  = hor % 24;

	printf
	(	"%3d dias, %02d:%02d:%02d",
		dias, hor, min, seg
	);
	
	/*
	 *	Imprime a hora do "boot"
	 */
	printf ("\t(boot = %24.24s)\n", btime (&boot_time));

	return (0);

}	/* end uptime */

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
		"%s - fornece o tempo decorrido desde o último \"boot\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s\n",
		pgname, pgversion, pgname
	);

	exit (2);

}	/* end help */
