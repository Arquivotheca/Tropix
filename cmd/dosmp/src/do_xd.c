/*
 ****************************************************************
 *								*
 *			do_xd.c					*
 *								*
 *	DUMP em hexadecimal e ISO				*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.0, de 10.06.95				*
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****** Definições globais **************************************
 */
#define	LINESZ	16

/*
 ****** Variáveis globais ***************************************
 */
entry int	xd_dump_addr;		/* End. do início da linha */

/*
 ******	Protótipos de funções ***********************************
 */
void	xd_print_line (const char *, int);
void	do_xd_help (void);

/*
 ****************************************************************
 *	DUMP em hexadecimal e ISO				*
 ****************************************************************
 */
void
do_xd (int argc, const char *argv[])
{
	int		opt, n;
	const char	*nm;
	char		area[LINESZ];
	DOSSTAT		z;
	DOSFILE		f;

	/*
	 *	Inicializações
	 */
	xd_dump_addr = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_xd_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_xd_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc != 1)
		{ do_xd_help (); return; }

	nm = argv[0];

	/*
	 *	Abre o arquivo
	 */
	if (dos_stat (nm, &z) < 0)
	{
		printf
		(	"%s: Não achei o arquivo \"%s\" (%s)\n",
			cmd_nm, nm, strerror (errno)
		);
		return;
	}

	if (!Z_ISREG (z.z_mode) && !Z_ISDIR (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é regular nem um diretório\n",
			cmd_nm, nm
		);
		return;
	}

	if (dos_open (&f, &z) < 0)
		return;

	/*
	 *	Percorre os blocos do arquivo
	 */
	while ((n = dos_read (&f, area, LINESZ)) > 0)
	{
		if (received_intr)
			break;

		xd_print_line (area, n);
	}

}	/* end do_xd */

/*
 ****************************************************************
 *	Imprime uma linha					*
 ****************************************************************
 */
void
xd_print_line (const char *area, int count)
{
	int		i;
	const char	*cp;

	printf ("%P:   ", xd_dump_addr);

	for (i = 0; i < LINESZ; i++)
	{
		if ((i & 1) == 0)
			printf (" ");

		if (i == LINESZ/2)
			printf (" ");

		if (i < count)
			printf ("%02X", area[i]);
		else
			printf ("  ");
	}

	printf ("     ");

	/*
	 *	Escreve caracteres em ISO
	 */
	for
	(	cp = area, i = 0;
		i < LINESZ && i < count;
		cp++, i++
	)
	{
		int	c;

		c = *cp;

		if (c < 0x20)
			c = '.';

		putchar (c);
	}

	printf ("\n");

	/*
	 *	Se o endereço é multiplo de 128, pula uma linha
	 */
	xd_dump_addr += count;

	if ((xd_dump_addr & 127) == 0)
		printf ("\n");

}	/* end xd_walk_function */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_xd_help (void)
{
	fprintf
	(	stderr,
		"%s - \"dump\" em hexadecimal e ISO de um arquivo DOS\n"
		"\nSintaxe:\n"
		"\t%s <arquivo>\n",
		cmd_nm, cmd_nm
	);

}	/* end do_xd_help */
