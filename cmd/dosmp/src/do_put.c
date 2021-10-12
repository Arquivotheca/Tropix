/*
 ****************************************************************
 *								*
 *			do_put.c				*
 *								*
 *	Copia arquivos para o sistema de arquivos DOS		*
 *								*
 *	Versão	3.0.0, de 14.11.93				*
 *		3.1.0, de 15.01.99				*
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
#include <time.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
int		iso_to_dos (const void *, int, char *);
void		do_put_help (void);

/*
 ****************************************************************
 *	Copia arquivos para o sistema de arquivos DOS		*
 ****************************************************************
 */
void
do_put (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "ifvH")) >= 0)
	{
		switch (opt)
		{
		    case 'i':			/* Interativo */
			cmd_iflag++;
			break;

		    case 'f':			/* Força */
			cmd_fflag++;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_put_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_put_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc == 0)
		{ do_put_help (); return; }

	/*
	 *	Processa os diversos arquivos
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			return;

		if   (cmd_iflag)
		{
			fprintf (stderr, "%s? (n): ", *argv);

			if (askyesno () <= 0)
				continue;
		}
		elif (cmd_vflag)
		{
			printf ("%s\n", *argv);
		}

		put_one_file (*argv, 0 /* apenas o último nome */);
	}

}	/* end do_put */

/*
 ****************************************************************
 *	Copia um arquivo para o sistema de arquivos DOS		*
 ****************************************************************
 */
int
put_one_file (const char *path, int complete_path)
{
	const UNI	*up = &uni;
	const char	*cp;
	const char	*dos_path, *son_nm;
	int		count, residual;
	int		clusno;
	int		d_cluster = 0, d_size = 0;
	int		fd, n, r = 0;
	int		par_clusno;
	const char	*area;
	char		*in_area;
	char		*out_area = NOSTR;
	DOSSTAT		z;
	STAT		s;
	char		par_nm[BLSZ];

	/*
	 *	Abre o arquivo
	 */
	if ((fd = open (path, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
	{
		printf
		(	"%s: Não consegui abrir \"%s\" (%s)\n",
			cmd_nm, path, strerror (errno)
		);
		return (-1);
	}

	if (!S_ISREG (s.st_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é regular\n",
			cmd_nm, path
		);
		close (fd);
		return (-1);
	}

	/*
	 *	Obtém o nome do diretório pai
	 */
	dos_path = complete_path ? path : last_nm (path);

	if   ((cp = strrchr (dos_path, '/')) == NOSTR)
		{ strcpy (par_nm, "."); son_nm = dos_path; }
	elif (cp == dos_path)
		{ strcpy (par_nm, "/"); son_nm = cp + 1; }
	else
		{ strcpy (par_nm, dos_path); par_nm[cp - dos_path] = '\0'; son_nm = cp + 1; }

	/*
	 *	Analisa o diretório pai
	 */
	if (dos_stat (par_nm, &z) < 0)
	{
		printf
		(	"%s: O diretório pai \"%s\" já deve existir (%s)\n",
			cmd_nm, par_nm, strerror (errno)
		);
		close (fd); return (-1);
	}

	if (!Z_ISDIR (z.z_mode))
	{
		printf
		(	"%s: O arquivo \"%s\" não é um diretório\n",
			cmd_nm, par_nm
		);
		close (fd); return (-1);
	}

	par_clusno = z.z_cluster;

	/*
	 *	Verifica se por acaso o arquivo filho já existe
	 */
	if (dos_fstat (son_nm, par_clusno, &z) >= 0)
	{
		if (!Z_ISREG (z.z_mode))
		{
			printf
			(	"%s: O arquivo DOS \"%s\" já existe e "
				"NÃO é regular\n", cmd_nm, dos_path
			);
			{ close (fd); return (-1); }
		}

		fprintf
		(	stderr,
			"%s: O arquivo DOS \"%s\" já existe%s. "
			"Reescreve? (n): ",
			cmd_nm, dos_path,
			(z.z_mode & Z_RO) ? " e só permite leituras" : ""
		);

		if   (cmd_fflag)
			fprintf (stderr, "SIM\n");
		elif (askyesno () <= 0)
			{ close (fd); return (0); }

		/*
		 *	Remove os blocos antigos (trunca)
		 */
		put_cluster_list (z.z_cluster);
	   /***	PUT_LONG (0, z.z_size); ***/
	}

	/*
	 *	Aloca áreas compatíveis com o tamanho do CLUSTER
	 */
	in_area = alloca (CLUSZ);

	if (iso_mode)
		out_area = alloca (3 * CLUSZ);	/* Muitos '\n's */

	/*
	 *	Copia os blocos
	 */
	for (clusno = 0, residual = 0; /* abaixo */; /* abaixo */)
	{
		if ((n = read (fd, in_area, CLUSZ)) < 0)
		{
			printf
			(	"%s: Erro de leitura do arquivo \"%s\" (%s)\n",
				cmd_nm, path, strerror (errno)
			);
			break;
		}

		if (n == 0)
			break;

		if (iso_mode)
		{
			area = out_area;
			count = iso_to_dos (in_area, n, out_area + residual) +
								residual;
		}
		else
		{
			area = in_area;
			count = n;
		}

		while (count >= CLUSZ)
		{
			if ((clusno = get_new_cluster (clusno)) < 0)
			{
				printf
				(	"%s: Não consegui obter um cluster "
					"para o arquivo \"%s\" (%s)\n",
					cmd_nm, dos_path, strerror (errno)
				);
				r = -1;	 goto bad;
			}

			if (d_cluster == 0)
				d_cluster = clusno;

			clus_write (clusno, area, CLUSZ);

			area += CLUSZ;	count -= CLUSZ;	d_size += CLUSZ;
		}

		if ((residual = count) > 0 && iso_mode && area != out_area)
			memmove (out_area, area, residual);
	}

	/*
	 *	Escreve o último CLUSTER
	 */
	if (residual > 0)
	{
		if ((clusno = get_new_cluster (clusno)) < 0)
		{
			printf
			(	"%s: Não consegui obter um cluster "
				"para o arquivo \"%s\" (%s)\n",
				cmd_nm, dos_path, strerror (errno)
			);
			r = -1;	 goto bad;
		}

		if (d_cluster == 0)
			d_cluster = clusno;

		clus_write (clusno, iso_mode ? out_area : in_area, residual);

		d_size += residual;
	}

	/*
	 *	Prepara a entrada do pai para o filho
	 */
    bad:
   /***	z.z_name = ...; ***/
   /***	z.z_ext = ...; ***/
	z.z_mode = Z_REG|Z_MODIF;
	put_dos_file_time (time ((time_t *)NULL), &z.z_d);
	PUT_CLUSTER (d_cluster, &z);
	PUT_LONG (d_size, z.z_size);

	z.z_lfn_clusno = par_clusno;
	z.z_lfn_nm = (char *)son_nm;

	dos_lfn_dir_put (&z);

	close (fd); return (r);

}	/* end put_one_file */

/*
 ****************************************************************
 *	Converte a representação de ISO para DOS		*
 ****************************************************************
 */
int
iso_to_dos (const void *in_area, int count, char *out_area)
{
	const char	*ip, *ep;
	char		*op;
	char		c;

	for (ip = in_area, ep = ip + count, op = out_area; ip < ep; /* abaixo */)
	{
		if ((c = *ip++) == '\n')
			*op++ = '\r';

		*op++ = c;
	}

	return (op - out_area);

}	/* end iso_to_dos */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_put_help (void)
{
	fprintf
	(	stderr,
		"%s - copia arquivos %s para o diretório corrente "
			"do sistema DOS\n"
		"\nSintaxe:\n"
		"\t%s [-ifv] <arquivo> ...\n",
		cmd_nm, sys_nm, cmd_nm
	);

	fprintf (stderr, "\nOpções:%s", options_ifv);

	fprintf (stderr, somewhere_expand, sys_nm);

}	/* end do_put_help */
