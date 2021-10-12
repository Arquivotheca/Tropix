/*
 ****************************************************************
 *								*
 *			mkgrp.c					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 27.07.97				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <grp.h>
#include <signal.h>

#include "../h/edusr.h"

/*
 ******	Protótipos de funções ***********************************
 */
int	do_mkgrp_write_entry (FILE *, const char *, int);
void	do_mkgrp_help (void);

/*
 ****************************************************************
 *	Imprime/modifica o grupo corrente			*
 ****************************************************************
 */
void
do_mkgrp (int argc, const char *argv[])
{
	register	FILE *tfp;
	int		opt;
	const GROUP	*gr;
	const char	*group_nm;
	int		group_gid = -1;
	const char	*str;
	int		colocou = 0, gid = 0;
	int		last_group_id = -1;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_mkgrp_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_mkgrp_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 1)
		{ do_mkgrp_help (); return; }

	/*
	 *	Imprime o "grp" corrente
	 */
	if (argc == 0)
		{ do_mkgrp_current (); return; }

	/*
	 *	Verifica se deu o nome ou o número do novo grupo
	 */
	group_nm = argv[0];

	if (isdigit (group_nm[0]))
	{
		if ((group_gid = strtol (group_nm, &str, 0)) < 0 || *str != '\0')
		{
			error ("Número de grupo \"%s\" inválido", group_nm);
			return;
		}
	}

	/*
	 *	Em primeiro lugar, procura o grupo
	 */
	if (group_gid >= 0)
		gr = getgrgid (group_gid);
	else
		gr = getgrnam (group_nm);

	if (gr != NOGROUP)
	{
		strcpy (current_group_nm, gr->gr_name);
		current_group_id = gr->gr_gid;

		do_mkgrp_current ();
		return;
	}
	elif (group_gid >= 0)
	{
		error ("Grupo %d desconhecido", group_gid);
		return;
	}

	/*
	 *	Cria o novo grupo
	 */
	signal (SIGINT, SIG_IGN);
	tfp = tmpfile ();
	setgrent ();

	while ((gr = getgrent ()) != NOGROUP)
	{
		if (!colocou && (gid = gr->gr_gid) > NIG && gid > last_group_id + 1)
		{
			gid = last_group_id < NIG ? NIG : last_group_id + 1;

			if (do_mkgrp_write_entry (tfp, group_nm, gid) < 0)
				goto abort;

			colocou++;
		}
	
		putgrent (gr, tfp); last_group_id = gr->gr_gid;
	
	}	/* malha copiando o arquivo de grupos */

	if (!colocou)
	{
		gid = last_group_id < NIG ? NIG : last_group_id + 1;

		if (do_mkgrp_write_entry (tfp, group_nm, gid) < 0)
			goto abort;
	}

	endgrent ();
	copyfile (tfp, group_file_nm);

	strcpy (current_group_nm, group_nm);
	current_group_id = gid;

	signal (SIGINT, on_intr);

	do_mkgrp_current ();
	return;

	/*
	 *	Em caso de não alteração do arquivo
	 */
    abort:
	endgrent ();
	fclose (tfp);
	signal (SIGINT, on_intr);

}	/* end do_mkgrp */

/*
 ****************************************************************
 *	Imprime o valor corrente				*
 ****************************************************************
 */
void
do_mkgrp_current (void)
{
	printf
	(	"O grupo corrente é \"%s\" (%d)\n",
		current_group_nm, current_group_id
	);

}	/* end do_mkgrp_current */

/*
 ****************************************************************
 *	Escreve a nova entrada GROUP				*
 ****************************************************************
 */
int
do_mkgrp_write_entry (FILE *tfp, const char *group_nm, int gid)
{
	fprintf (stderr, "\nGrupo\t\t  = %s (%d)\n", group_nm, gid);

	fprintf (stderr, "\nDeseja criar o grupo? (n): ");

	if (askyesno () <= 0)
		return (-1);

	fprintf (tfp, "%s:locked:%d\n", group_nm, gid);

	return (0);

}	/* end do_mkgrp_write_entry */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_mkgrp_help (void)
{
	fprintf
	(	stderr,
		"%s - altera o grupo corrente ou cria um novo grupo\n"
		"\nSintaxe:\n"
		"\t%s [<grupo>]\n"
		"\nObs.:\tSe o <grupo> não for dado, imprime o grupo corrente.\n"
		"\tSe o <grupo> dado ainda não existir, ele é criado\n",
		cmd_nm, cmd_nm
	);

}	/* end do_mkgrp_help */
