/*
 ****************************************************************
 *								*
 *			do_chmod.c				*
 *								*
 *	Altera atributos do modo de um arquivo DOS		*
 *								*
 *	Versão	3.0.0, de 19.11.93				*
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
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****** Para processa os atributos ******************************
 */
typedef struct	{ short op; short mode; }	ACTION;

#define	ASZ	10

entry ACTION	action[ASZ],	/* Ações a tomar */
		*actionp;

/*
 ******	Protótipos de funções ***********************************
 */
int		mode_op_cv (const char *);
int		file_mode_change (const char *);
void		do_chmod_help (void);

/*
 ****************************************************************
 *	Altera atributos do modo de um arquivo DOS		*
 ****************************************************************
 */
void
do_chmod (int argc, const char *argv[])
{
	const char	*bp;
	const char	*ptr;
	char		*ep;
	int		opt;

	/*
	 *	Inicializações
	 */
	actionp = action;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "vdH")) >= 0)
	{
		switch (opt)
		{
		    case 'd':			/* Debug */
			cmd_dflag++;
			break;

		    case 'v':			/* Verboso */
			cmd_vflag++;
			break;

		    case 'H':			/* Help */
			do_chmod_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_chmod_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	/*
	 *	Processa os vários modos dados
	 */
	bp = *argv++;

	if (*bp >= '0' && *bp <= '9')
	{
		/*
		 *	Modo absoluto
		 */
		action->op	= '=';
		action->mode	= strtol (bp, &ptr, 0);

		if (*ptr != '\0')
		{
			printf
			(	"%s: Modo inválido: \"%s\"\n\n",
				cmd_nm, bp
			);
			do_chmod_help ();
			return;
		}

		actionp++;
	}
	else
	{
		/*
		 *	Modo simbólico
		 */
		while ((ep = strchr (bp, ',')) != NOSTR)
		{
			*ep = '\0';

			if (mode_op_cv (bp) < 0)
				return;

			bp = ep + 1;
		}

		if (mode_op_cv (bp) < 0)
			return;
	}

	/*
	 *	Deve sobrar algum arquivo
	 */
	if (*argv == NOSTR)
		{ do_chmod_help (); return; }

	/*
	 *	Processa os arquivos
	 */
	for (/* vazio */; *argv; argv++)
	{
		if (received_intr)
			break;

		file_mode_change (*argv);
	}

}	/* end do_chmod */

/*
 ****************************************************************
 *	Converte uma codificação simbólica na forma interna	*
 ****************************************************************
 */
int
mode_op_cv (const char *modenm)
{
	char const	*cp = modenm;
	int		op;
	long		mode = 0;

	/*
	 *	Primeira parte: Quem (opcional)
	 */
	if (*cp == 'u')
		cp++;

	/*
	 *	Segunda parte: "Operator"
	 */
	switch (op = *cp++)
	{
	    case '+':
	    case '-':
	    case '=':
		break;

	    default:
	    mode_err:
		printf
		(	"%s: Modo inválido: \"%s\"\n\n",
			cmd_nm, modenm
		);
		do_chmod_help ();
		return (-1);
	}

	/*
	 *	Terceira parte: "Permission"
	 */
	for (EVER) switch (*cp++)
	{
	    case 'r':
		mode |= Z_RO;
		break;

	    case 'h':
		mode |= Z_HIDDEN;
		break;

	    case 's':
		mode |= Z_SYS;
		break;

	    case 'm':
		mode |= Z_MODIF;
		break;

	    case '1':
		mode |= Z_1;
		break;

	    case '2':
		mode |= Z_2;
		break;

	    case '\0':
		goto out;

	    default:
		goto mode_err;
	}

	/*
	 *	Armazena o modo
	 */
    out:
	if (cmd_dflag)
	{
		printf
		(	"op = '%c', mode = \"%s\" %8o \"%s\"\n",
			op, file_mode_edit (mode), mode, modenm
		);
	}

	if (actionp >= &action[ASZ])
	{
		printf
		(	"%s: Foi dado um número excessivo de modos\n",
			cmd_nm
		);
		return (-1);
	}

	actionp->op	= op;
	actionp->mode	= mode;

	actionp++;

	return (0);

}	/* end mode_op_cv */

/*
 ****************************************************************
 *	Troca o modo de um arquivo 				*
 ****************************************************************
 */
int
file_mode_change (const char *filenm)
{
	const ACTION	*ap;
	int		mode;
	int		old_mode;
	DOSSTAT		z;

	/*
	 *	Prepara a mudança do modo
	 */
	if (dos_stat (filenm, &z) < 0)
	{
		printf
		(	"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			cmd_nm, filenm, strerror (errno)
		);
		return (-1);
	}

	/*
	 *	Executa as várias ações
	 */
	old_mode = mode = z.z_mode;

	for (ap = action; ap < actionp; ap++)
	{
		switch (ap->op)
		{
		    case '=':
			mode = ap->mode;
			break;

		    case '+':
			mode |= ap->mode;
			break;

		    case '-':
			mode &= ~ap->mode;
			break;

		    default:
			error
			(	"chmod: Operação inválida: '%c'\n",
				ap->op
			);
			return (-1);

		}	/* end switch */

	}	/* end for */

	/*
	 *	Atribui o modo novo
	 */
	z.z_mode = z.z_mode & (Z_VOL|Z_DIR) | mode & ~(Z_VOL|Z_DIR);

	dos_dir_write (&z);

	if (cmd_vflag)
	{
		printf ("\"%s\"  =>  ", file_mode_edit (old_mode));
		printf ("\"%s\"\t%s\n", file_mode_edit (z.z_mode), filenm);
	}

	return (0);

}	/* end file_mode_change () */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_chmod_help (void)
{
	fprintf
	(	stderr,
		"%s - altera atributos de arquivos DOS\n"
		"\nSintaxe:\n"
		"\t%s [-v] <modo> <arquivo> ...\n",
		cmd_nm, cmd_nm
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Lista os modos antigo e novo de cada <arquivo>\n"
		"\n<modo>:"
		"\tAbsoluto:  um inteiro			(ex.: 0201)\n"
		"\tSimbólico: [u] <op> <atributo>		(ex.: +r)\n\n"
		"\tModos simbólicos múltiplos podem ser dados\n"
		"\tseparados por vírgulas			(ex.: u-h,+r)\n" 
	);

}	/* end do_chmod_help */
