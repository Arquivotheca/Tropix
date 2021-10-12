/*
 ****************************************************************
 *								*
 *			prdbtb.c				*
 *								*
 *	Imprime a Tabela de Depuração				*
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
#include <wsdb.h>

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
HEADER	header;

/*
 ****************************************************************
 *	Tabela de Classes					*
 ****************************************************************
 */
uchar	*classtb [] =
{
	"C_UNDEF",
	"C_PARAM",
	"C_AUTO",
	"C_EXTERN",
	"C_REGISTER",
	"C_STATIC",
	"C_USTATIC",
	"C_TYPEDEF",
	"C_READONLY",
	"C_ENTRY",
	"C_COMMON",
	"C_LABEL",
	"C_ULABEL",
	"C_STAG",
	"C_UTAG",
	"C_ETAG",
	"C_MOS",
	"C_MOU",
	"C_MOE",
	"C_FILEBEGIN",
	"C_FILEEND",
	"C_FUNCTIONBEGIN",
	"C_FUNCTIONEND",
	"C_BLOCKBEGIN",
	"C_BLOCKEND",
	"",
	"C_EXTENSION",
	"C_DIMENSIONS",
	"C_EOS"
};

/*
 ****************************************************************
 *	Tabela de Tipos						*
 ****************************************************************
 */
uchar	*typetb []	=
{
	"T_UNDEF",
	"T_ARG",
	"T_VOID",
	"T_CHAR",
	"T_SHORT",
	"T_INT",
	"T_LONG",
	"T_QUAD",
	"T_UCHAR",
	"T_USHORT ",
	"T_UINT",
	"T_ULONG",
	"T_UQUAD",
	"T_FLOAT ",
	"T_DOUBLE ",
	"T_EXTD",
	"T_SIGNED",
	"T_UNSIGNED",
	"T_CONST",
	"T_VOLATILE",
	"T_STRUCT",
	"T_UNION",
	"T_ENUM",
	"T_MOE"
};

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
		fprintf (stderr, "Use: prdbtb arq...\n");
		exit (1);
	}

	n = 0;
	while (*++argv != (char *)NULL)
		n += prdbtb (*argv);

	exit (n != 0);

}	/* end main */

/*
 ****************************************************************
 *	Processa um Arquivo					*
 ****************************************************************
 */
prdbtb (name)
char	*name;
{
	int	fd;
	DBSYM	*dp, *dbend;
	DBSYM		*dbbuf;

	if ((fd = open (name, O_RDONLY)) < 0)
	{
		fprintf (stderr, "prdbtb: Não pude abrir %s.\n", name);
		return (1);
	}

	if (read (fd, (char *)&header, sizeof (HEADER)) != sizeof (HEADER))
	{
		fprintf (stderr, "prdbtb: Erro na leitura do Cabeçalho.\n");
		close (fd);
		return (1);
	}

	if (header.h_magic != FMAGIC && header.h_magic != NMAGIC)
	{
		fprintf (stderr, "prdbtb: %s não é arquivo objeto.\n", name);
		close (fd);
		return (1);
	}

	printf ("%s (%d):\n", name, header.h_dbsize);
	if (header.h_dbsize == 0)
	{
		printf ("    Tabela ausente.\n");
		close (fd);
		return (0);
	}

/**************************
	if ((symbuf = (SYM *)malloc (header.h_ssize)) == (SYM *)NULL)
	{
		fprintf (stderr, "prdbtb: Não há memória para a SYMTB.\n");
		close (fd);
		return (1);
	}

	lseek (fd, sizeof (HEADER) + header.h_tsize + header.h_dsize, 0);

	if (read (fd, (char *)symbuf, header.h_ssize) != header.h_ssize)
	{
		fprintf (stderr, "prdbtb: Erro na leitura da SYMTB.\n");
		close (fd);
		free ((char *)symbuf);
		return (1);
	}
**************************/

	if ((dbbuf = (DBSYM *)malloc (header.h_dbsize)) == (DBSYM *)NULL)
	{
		fprintf (stderr, "prdbtb: Memória excedida.\n");
		close (fd);
		return (1);
	}

	lseek (fd, -header.h_dbsize, 2);

	if (read (fd, (char *)dbbuf, header.h_dbsize) != header.h_dbsize)
	{
		fprintf (stderr, "prdbtb: Erro na leitura da DBSYM.\n");
		close (fd);
		free ((char *)dbbuf);
		return (1);
	}

	dbend = (DBSYM *)((char *)dbbuf + header.h_dbsize);

	for (dp = dbbuf; dp < dbend; dp++)
	{
		switch (dp->d_class)
		{
		    case C_DIMENSIONS:
			printf
			(
				"C_DIMENSIONS\t\t\t%-31s\t%d",
				dp->d_name,
				dp->d.d_dim[0]
			);

			if (dp->d.d_dim[1] != 0)
			{
				printf (" %d", dp->d.d_dim[1]);
				if (dp->d.d_dim[2] != 0)
					printf (" %d", dp->d.d_dim[2]);
			}
			break;

		    case C_EXTENSION:
			printf
			(
				"C_EXTENSION\t\t\t%-31s\t%d",
				dp->d_name,
				dp->d.dbent.d_value
			);
			break;

		    case C_FILEEND:
			printf
			(
				"C_FILEEND\t************************"
				"***********************************"
			);
			break;

		    default:
			printf
			(
				"%-15s\t%-10s\t%-31s\t%d",
				classtb [dp->d_class],
				typetb [BTYPE (dp->d.dbent.d_type)],
				dp->d_name,
				dp->d.dbent.d_value
			);
			break;
		}

		putchar ('\n');
	}

	free ((char *)dbbuf);
	close (fd);

	return (0);

}	/* end prdbtb */
