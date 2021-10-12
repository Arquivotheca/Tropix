/*
 ****************************************************************
 *								*
 *			mk_id_table.c				*
 *								*
 *	Constrói a tabela de identificadores do AS		*
 *								*
 *	Versão	1.0.0, de 24.09.92				*
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

#include <a.out.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <mkasmtb.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/id_sym_tb.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 21.02.94";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Tabela dos registros ************************************
 */
typedef struct
{
	short	r_regno;		/* índice do registrador */
	short	r_value;		/* Código interno do registrador */
	char	r_name[LIDSZ+1];	/* Nome */

}	REGLIST;

#define	DEF(i, v, n)		i, v, n,

entry REGLIST	src_reg_table[]	=
{
#include "../h/id_insert.h"

	0,	0,	""
};

#undef	DEF

/*
 ****** Definição da relocação **********************************
 */
entry OBJENT	id_hash_rel[] =
{
	{ sizeof (SYMTB *), 0 , 1, "id_sym_tb",	NOVOID },
	{ 0 }
};

entry OBJENT	id_sym_rel[] =
{
	{ sizeof (SYMTB *), offsetof (SYMTB, s_next),  1, "id_sym_tb",	NOVOID },
	{ sizeof (SYMTB *), offsetof (SYMTB, s_def),   1, "id_sym_tb",	NOVOID },
	{ sizeof (short),   offsetof (SYMTB, s_line),  1, "",		NOVOID },
	{ sizeof (short),   offsetof (SYMTB, s_index), 1, "",		NOVOID },
	{ sizeof (SYMTB *), offsetof (SYMTB, s_link),  1, "id_sym_tb",	NOVOID },
	{ sizeof (long),    offsetof (SYMTB, s.s_value), 1, "",		NOVOID },
	{ 0 }
};

#define	LINK_INDEX 4	/* Não esquecer de atualizar */

entry OBJENT	reg_value_vec_sz_rel[] =
{
	{ sizeof (int), 0 , 1, "",	NOVOID },
	{ 0 }
};

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
void		error (const char *, ...);

/*
 ****************************************************************
 *	A função HASH de identificadores			*
 ****************************************************************
 */
entry int	ncolision;

entry SYMTB	**id_hash_tb;	/* Endereço da Tabela HASH */

#define	MK
#include "../h/id_hash.h"
 
/*
 ****************************************************************
 *	Constrói a tabela de identificadores do AS		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	REGLIST		*rl;
	int		opt, reg_cnt = 0;
	SYMTB		*id_sym_tb;
	SYMTB		*sp, **link_place;
	int		*reg_value_vec;
	int		reg_value_vec_sz = 0;
	int		len, max_len = 0, code;
	int		symtb_sizeof;

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
	 *	Determina o número de registros a serem instalados
	 *	Acha o tamanho do "reg_value_vec"
	 */
	for (rl = src_reg_table; rl->r_name[0] != '\0'; rl++)
	{
		reg_cnt++;

		if (rl->r_regno > reg_value_vec_sz)
			reg_value_vec_sz = rl->r_regno;

		if (max_len < (len = strlen (rl->r_name)))
			max_len = len;
	}

	reg_value_vec_sz++;	/* Começa de zero */

	/*
	 *	Escreve o no. de registros
	 */
	if (vflag)
		printf ("%d registros\n", reg_cnt);

	/*
	 *	Aloca memória para o vetor de valores
	 */
	if ((reg_value_vec = malloc (reg_value_vec_sz * sizeof (int))) == NOVOID)
		error (NOSTR);

	/*
	 *	Aloca memória para a tabela de simbolos
	 */
	symtb_sizeof = sizeof (SYMTB) - sizeof (SYM) + SYM_SIZEOF (max_len);

	if ((id_sym_tb = sp = malloc (reg_cnt * symtb_sizeof)) == NOSYMTB)
		error (NOSTR);

	memsetl
	(	(long *)id_sym_tb,
		0,
		reg_cnt * symtb_sizeof / sizeof (long)
	);

	if ((id_hash_tb = malloc (ID_HASH_SZ * sizeof (SYMTB *))) == NOVOID)
		error (NOSTR);

	memsetl
	(	(long *)id_hash_tb,
		0,
		ID_HASH_SZ * sizeof (SYMTB *) / sizeof (long)
	);

	/*
	 *	Preenche a tabela de simbolos e o vetor de valores
	 */
	for (rl = src_reg_table; rl->r_name[0] != '\0'; rl++)
	{
		if (id_hash (rl->r_name, strlen (rl->r_name), &link_place) != NOSYMTB)
			printf ("Registro repetido: \"%s\"\n", rl->r_name);

		if (ncolision)
			printf ("Colisão em: \"%s\"\n", rl->r_name);

		sp->s_link = NOSYMTB; *link_place = sp;

		len = strlen (rl->r_name);

		sp->s.s_value = rl->r_regno;
		sp->s.s_type = REG;
	   /***	sp->s.s_flags &= ~(S_LBL|S_EXTERN|S_LOCAL|S_REF); ***/
		sp->s.s_nm_len = len;
		strncpy (sp->s.s_name, rl->r_name, SYM_NM_SZ (len));

	   /***	sp->s_next = NOSYMTB; ***/
	   /***	sp->s_def = NOSYMTB; ***/
	   /***	sp->s_line = 0; ***/
	   /***	sp->s_index = 0; ***/

		reg_value_vec[rl->r_regno] = rl->r_value;

		sp = (SYMTB *)((int)sp + symtb_sizeof);

	}	/* end percorrendo a tabela de registros */

	/*
	 *	Termina de inicializar os "OBJENT"s
	 */
	id_hash_rel[0].o_rel_addr = id_sym_tb;
	id_sym_rel[LINK_INDEX].o_rel_addr  = id_sym_tb;

	/*
	 *	Gera os módulos em "assembly"
	 */
	code = mkasmtb
	(	".id_hash_tb.s",
		"id_hash_tb",
		id_hash_tb,
		ID_HASH_SZ * sizeof (SYMTB *),
		RW,
		sizeof (SYMTB *),
		id_hash_rel
	);

	if (code < 0)
		error ("\"mkasmtb\" retornou erro para \".id_hash_tb.s\"");

	code = mkasmtb
	(	".id_sym_tb.s",
		"id_sym_tb",
		id_sym_tb,
	 	reg_cnt * symtb_sizeof,
		RW,
		symtb_sizeof,
		id_sym_rel
	);

	if (code < 0)
		error ("\"mkasmtb\" retornou erro para \".id_sym_tb.s\"");

	code = mkasmtb
	(	".reg_val_vec.s",
		"reg_value_vec",
		reg_value_vec,
		reg_value_vec_sz * sizeof (int),
		RO,
		sizeof (int),
		reg_value_vec_sz_rel
	);

	if (code < 0)
		error ("\"mkasmtb\" retornou erro para \".reg_val_vec.s\"");

	return (exit_code);

}	/* end mk_id_table */

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
		"%s - constrói a tabela de identificadores do AS\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: verboso\n"
	);

	exit (2);

}	/* end help */
