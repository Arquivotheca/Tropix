/*
 ****************************************************************
 *								*
 *			do_cmp.c				*
 *								*
 *	Compara arquivos entre o DOS e o sistema TROPIX		*
 *								*
 *	Versão	3.0.0, de 18.11.93				*
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
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		cmp_file (const char *, int);
int		cmp_one_file (const char *, int, STAT *, DOSSTAT *);
void		print_interval (time_t);
void		time_cmp (time_t, time_t);
void		do_cmp_help (void);

/*
 ****************************************************************
 *	Compara arquivos entre o DOS e o sistema TROPIX		*
 ****************************************************************
 */
void
do_cmp (int argc, const char *argv[])
{
	int		opt;
	int		cmpput = streq (cmd_nm, "cmpput");

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_cmp_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_cmp_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc == 0)
		{ do_cmp_help (); return; }

	/*
	 *	Processa os diversos arquivos
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			break;

		printf ("%s:\n", *argv);

		cmp_file (*argv, cmpput);
	}

}	/* end do_cmp */

/*
 ****************************************************************
 *	Obtém um arquivo					*
 ****************************************************************
 */
void
cmp_file (const char *path, int cmpput)
{
	int		fd;
	char		nm[LFN_SZ];
	STAT		s;
	DOSSTAT		z;

	/*
	 *	Abre o arquivo no DOS
	 */
	strcpy (nm, last_nm (path));

	if (dos_stat (cmpput ? nm : path, &z) < 0)
	{
		printf
		(	"\t*** Não achei o arquivo no sistema %s (%s)\n",
			"DOS", strerror (errno)
		);
		return;
	}

	if (!Z_ISREG (z.z_mode))
	{
		printf
		(	"\t*** O arquivo do sistema %s não é regular\n",
			"DOS"
		);
		return;
	}

	/*
	 *	Abre o arquivo no TROPIX
	 */
	if ((fd = open (cmpput ? path : nm, O_RDONLY)) < 0  || fstat (fd, &s) < 0)
	{
		printf
		(	"\t*** Não achei o arquivo no sistema %s (%s)\n",
			sys_nm, strerror (errno)
		);
		return;
	}

	if (!S_ISREG (s.st_mode))
	{
		printf
		(	"\t*** O arquivo do sistema %s não é regular\n",
			sys_nm
		);
		close (fd); 	return;
	}

	/*
	 *	Executa a comparação
	 */
	cmp_one_file (path, fd, &s, &z);

	close (fd);

}	/* end cmp_file */

/*
 ****************************************************************
 *	Compara um arquivo DOS com um do TROPIX			*
 ****************************************************************
 */
int
cmp_one_file (const char *path, int fd, STAT *sp, DOSSTAT *zp)
{
	const char	*cp, *bp, *end_bp;
	int		dos_big_size, n, count;
	int		cmp_offset = 0;
	char		sofix_area[BLSZ], dos_area[BLSZ];
	DOSFILE		f;

	/*
	 *	Compara os tamanhos
	 */
	if (bin_mode && sp->st_size != (dos_big_size = GET_LONG (zp->z_size)))
	{
		printf
		(	"\t*** Os tamanhos dos arquivos são diferentes: "
			"%d (%s) :: %d (DOS)\n",
			sp->st_size, sys_nm, dos_big_size
		);
		goto print_time;
	}

	/*
	 *	Percorre os blocos dos arquivos e compara-os
	 */
	if (dos_open (&f, zp) < 0)
		return (0);

	while ((count = dos_read (&f, dos_area, BLSZ)) > 0)
	{
		if ((n = read (fd, sofix_area, count)) < 0)
		{
			printf
			(	"\t*** Erro na leitura de \"%s\" (%s)\n",
				path, strerror (errno)
			);
			return (-1);
		}

		if (n < count)
		{
			printf
			(	"\t*** O arquivo do sistema %s é um prefixo do DOS\n",
				sys_nm
			);
			goto print_time;
		}

		/*
		 *	Compara o bloco
		 */
		for
		(	bp = sofix_area, end_bp = bp + count, cp = dos_area;
			bp < end_bp;
			/* abaixo */
		)
		{
			if (*bp++ != *cp++)
			{
				printf
				(	"\t*** Os arquivos diferem no caracter %d\n",
					cmp_offset + (bp - sofix_area) + 1
				);
				goto print_time;
			}
		}

		cmp_offset += count;
	}

	if (read (fd, sofix_area, 1) > 0)
	{
		printf
		(	"\t*** O arquivo do sistema DOS é um prefixo do %s\n",
			sys_nm
		);
		goto print_time;
	}

	return (0);

	/*
	 *	Diz qual o mais recente
	 */
    print_time:
	time_cmp (sp->st_mtime, get_dos_file_time (&zp->z_d));

	return (-1);

}	/* end cmp_one_file */

/*
 ****************************************************************
 *	Imprime a Mensagem Comparando os Tempos			*
 ****************************************************************
 */
void
time_cmp (time_t sofix_time, time_t dos_time)
{
	if (sofix_time == dos_time)
		return;
	else
		printf ("\t*** A Versão mais recente é do ");

	if (sofix_time > dos_time)
		printf ("\"%s\" ", sys_nm);
	else
		printf ("\"%s\" ", "DOS");

	print_interval (sofix_time - dos_time);

	printf ("\n");

}	/* end time_cmp */

/*
 ****************************************************************
 *	Emite uma noção de intervalo de tempo			*
 ****************************************************************
 */
void
print_interval (time_t inter)
{
	if   (inter == 0)
		return;
	elif (inter < 0)
		inter = -inter;

	printf ("(");

	if   (inter < 60)
		printf ("%d segundo%s", inter, plural (inter));
	elif ((inter /= 60) < 60)
		printf ("%d minuto%s", inter, plural (inter));
	elif ((inter /= 60) < 24)
		printf ("%d hora%s", inter, plural (inter));
	elif ((inter /= 24) < 30)
		printf ("%d dia%s", inter, plural (inter));
	elif (inter < 365)
		printf ("%d mes(es)", inter / 30);
	else
		printf ("%d ano%s", inter / 365, plural (inter / 365));

	printf (")");

}	/* end print_interval */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_cmp_help (void)
{
	int	cmpput = streq (cmd_nm, "cmpput");

	fprintf
	(	stderr,
		"%s - compara arquivos entre o sistema %s e o "
			"diretório corrente %s\n"
		"\nSintaxe:\n"
		"\t%s <arquivo> ...\n",
		cmd_nm, 	cmpput ? sys_nm : "DOS",
		cmpput ? "DOS"  : sys_nm, 	cmd_nm
	);

	fprintf (stderr, somewhere_expand, cmpput ? sys_nm : "DOS");

}	/* end do_cmp_help */
