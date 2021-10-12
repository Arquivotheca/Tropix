/*
 ****************************************************************
 *								*
 *			prlno.c					*
 *								*
 *	Imprime a Tabela de Número de Linhas			*
 *								*
 *	Versão	1.0.0, de 19.05.88				*
 *		2.3.1, de 19.12.90				*
 *								*
 *	Modulo: PRLNO						*
 *		Utilitários					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <a.out.h>
#include <lib.h>

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
HEADER	header;

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
main (argc, argv)
int	argc;
char	**argv;
{
	int	n;

	if (argc < 2)
	{
		fprintf (stderr, "Use: prlno arq...\n");
		exit (1);
	}

	n = 0;
	while (*++argv != (char *)NULL)
		n += prlno (*argv);

	exit (n != 0);

}	/* end main */

/*
 ****************************************************************
 *	Processa um Arquivo					*
 ****************************************************************
 */
prlno (name)
char	*name;
{
	int	fd;
	SYM	*sp;
	LINE	*lp, *lnoend;
	SYM		*symbuf;
	LINE		*lnobuf;

	if ((fd = open (name, O_RDONLY)) < 0)
	{
		fprintf (stderr, "prlno: Não pude abrir %s.\n", name);
		return (1);
	}

	if (read (fd, (char *)&header, sizeof (HEADER)) != sizeof (HEADER))
	{
		fprintf (stderr, "prlno: Erro na leitura do Cabeçalho.\n");
		close (fd);
		return (1);
	}

	if (header.h_magic != FMAGIC && header.h_magic != NMAGIC)
	{
		fprintf (stderr, "prlno: %s não é arquivo objeto.\n", name);
		close (fd);
		return (1);
	}

	printf ("%s (%d):\n", name, header.h_lnosize);
	if (header.h_lnosize == 0)
	{
		printf ("    Tabela ausente.\n");
		close (fd);
		return (0);
	}

	if ((symbuf = (SYM *)malloc (header.h_ssize)) == (SYM *)NULL)
	{
		fprintf (stderr, "prlno: Não há memória para a SYMTB.\n");
		close (fd);
		return (1);
	}

	lseek (fd, sizeof (HEADER) + header.h_tsize + header.h_dsize, 0);

	if (read (fd, (char *)symbuf, header.h_ssize) != header.h_ssize)
	{
		fprintf (stderr, "prlno: Erro na leitura da SYMTB.\n");
		close (fd);
		free ((char *)symbuf);
		return (1);
	}

	if ((lnobuf = (LINE *)malloc (header.h_lnosize)) == (LINE *)NULL)
	{
		fprintf (stderr, "prlno: Memória excedida.\n");
		close (fd);
		free ((char *)symbuf);
		return (1);
	}

	lseek (fd, header.h_rtsize + header.h_rdsize, 1);

	if (read (fd, (char *)lnobuf, header.h_lnosize) != header.h_lnosize)
	{
		fprintf (stderr, "prlno: Erro na leitura da LNO.\n");
		close (fd);
		free ((char *)symbuf);
		free ((char *)lnobuf);
		return (1);
	}

	lnoend = (LINE *)((char *)lnobuf + header.h_lnosize);

	for (lp = lnobuf; lp < lnoend; lp++)
	{
		if (lp->ll_line != 0)
		{
			printf
			(
				"    Addr = %P    Linha = %d\n",
				lp->ll_addr, lp->ll_line
			);
		}
		else
		{
			sp = &symbuf [lp->ll_addr];

			printf
			(
				"    Addr = %P    Sfile = %s\n",
				sp->s_value, sp->s_name
			);
		}
	}

	free ((char *)symbuf);
	free ((char *)lnobuf);
	close (fd);

	return (0);

}	/* end prlno */
