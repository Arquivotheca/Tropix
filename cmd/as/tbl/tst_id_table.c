/*
 ****************************************************************
 *								*
 *			tst_id_table.c				*
 *								*
 *	Testa a HASH de indentificadores			*
 *								*
 *	Versão	3.0.0, de 24.02.94				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <a.out.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/id_sym_tb.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 24.02.94";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		proc_arg (const char *);
void		tst_all_syms (void);
void		error (const char *format, ...);
void		help (void);

/*
 ****************************************************************
 *	A função HASH de identificadores			*
 ****************************************************************
 */
extern SYMTB	*id_hash_tb[];		/* Endereço da Tabela HASH */

extern int 	reg_value_vec[];	/* Vetor de valores internos */

#include "../h/id_hash.h"
 
/*
 ****************************************************************
 *	Testa a HASH de indentificadores			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
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
	 *	Foram dados argumentos
	 */
	if (*argv) for (/* vazio */; *argv; argv++)
	{
		proc_arg (*argv);
	}
	else
	{
		tst_all_syms ();
	}

	return (exit_code);

}	/* end tst_id_table */

/*
 ****************************************************************
 *	Processa um identificador				*
 ****************************************************************
 */
void
proc_arg (const char *name)
{
	SYMTB		*sp, **link_place;

printf ("Testando \"%s\"\n", name);
	/*
	 *	Procura na tabela
	 */
	if ((sp = id_hash (name, strlen (name), &link_place)) == NOSYMTB)
		{ error ("Não encontrei \"%s\" na tabela"); return; }

	printf
	(	"id = \"%s\", type = %d, value = %d, code = %d\n",
		sp->s.s_name, sp->s.s_type, sp->s.s_value,
		reg_value_vec[sp->s.s_value]
	);

}	/* end proc_arg */

/*
 ****************************************************************
 *	Testa a tabela toda					*
 ****************************************************************
 */
void
tst_all_syms (void)
{
	int		i;
	SYMTB		*sp, *tst_sp, **link_place;

	for (i = 0; i < ID_HASH_SZ; i++)
	{
		sp = id_hash_tb[i];

		while (sp)
		{
			printf
			(
				"id = \"%s\", type = %d, value = %d, code = %d   ",
				sp->s.s_name, sp->s.s_type, sp->s.s_value,
				reg_value_vec[sp->s.s_value]
			);

			if   ((tst_sp = id_hash (sp->s.s_name, sp->s.s_nm_len, &link_place)) == NOSYMTB)
				printf ("\t********* Não encontrei \"%s\" na tabela\n");
			elif (tst_sp != sp)
				printf ("\t********* A hash achou outro endereço para \"%s\"\n");
			else
				printf ("\tOK\n");

			sp = sp->s_next;
		}
	}


}	/* end tst_all_syms */
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
