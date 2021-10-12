/*
 ****************************************************************
 *								*
 *			tst_op_table.c				*
 *								*
 *	Testa a tabela de operadores do AS			*
 *								*
 *	Versão	2.3.0, de 12.02.91				*
 *		3.0.0, de 26.02.94				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/op_code.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	2.3.0, de 12.02.91";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

entry int	col_vec[9];	/* Vetor para contar colisões */

/*
 ****** Endereços das tabelas ***********************************
 */
extern int	op_hash_sz;	/* Tamanho da tabela HASH */

extern OPCODE	*op_hash_tb[];	/* Endereço da Tabela HASH */

extern OPCODE	op_code_tb[];	/* Endereço da parte de "nome + grupo" */

extern OPADDR	op_addr_tb[];	/* Endereço da parte de "opcode + addr" */

/*
 ****** Protótipos de funções ***********************************
 */
void		print_op_entry (const char *);
void		print_op_colision (int);
void		help (void);
char		*editpercent (int);
int		getpercent (long, long);

/*
 ****************************************************************
 *	A função HASH de operadores				*
 ****************************************************************
 */
entry int	ncolision;

#define	MK
#include "../h/op_hash.h"
 
/*
 ****************************************************************
 *	Testa a tabela de operadores do AS			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opcode_cnt = 0, addr_cnt = 0, i, k;
	OPCODE		*op, **hp, **ehp;
	int		opt, total_colision = 0, max_colision = 0;

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

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se foram dados nomes de instruções
	 */
	if (argc > 0)
	{
		while (*argv)
			print_op_entry (*argv++);

		exit (exit_code);
	}

	/*
	 *	Percorre a tabela HASH
	 */
	for (hp = &op_hash_tb[0], ehp = &op_hash_tb[op_hash_sz]; hp < ehp; hp++)
	{
		for (op = *hp; op != NOOPCODE; op = op->o_link)
		{
			if (op_hash (op->o_name) != op)
				printf ("Não achei o mnemônico \"%s\"\n", op->o_name);

			if (ncolision < 8)
				col_vec[ncolision]++;
			else
				col_vec[8]++;

			if (ncolision > max_colision)
				max_colision = ncolision;

			total_colision += ncolision;

			opcode_cnt++;

			addr_cnt += op->o_end - op->o_begin;
		}
	}

	printf
	(	"A tabela objeto tem %d entradas e %d endereçamentos\n",
		opcode_cnt, addr_cnt
	);

	printf
	(	"Sz = %4d (%s %%), ",
		op_hash_sz,
		editpercent (getpercent (opcode_cnt, op_hash_sz))
	);

	i = opcode_cnt + total_colision;	i += opcode_cnt / 200;

	printf
	(	"Tentativas = %5d (%s)\n",
		opcode_cnt + total_colision,
		editpercent (getpercent (i, 100 * opcode_cnt))
	);


	printf ("\nMapa de colisões:\n\n");

	printf ("col\t0\t1\t2\t3\t4\t5\t6\t7\t> 7\n");

	printf ("op[col]");
	for (i = 0; i < 9; i++)
		printf ("\t%d", col_vec[i]);
	printf ("\n");

	printf ("%%");
	for (i = 0; i < 9; i++)
	{
		k = 100 * col_vec[i] + opcode_cnt / 200;
		printf ("\t%s", editpercent (getpercent (k, 100 * opcode_cnt)));
	}
	printf ("\n");

	if (vflag)
		print_op_colision (max_colision);

	return (exit_code);

}	/* end tst_op_table */

/*
 ****************************************************************
 *	Imprime uma entrada de operador				*
 ****************************************************************
 */
void
print_op_entry (const char *op_name)
{
	OPCODE		*op;
	OPADDR		*ap;

	if ((op = op_hash (op_name)) == NOOPCODE)
	{
		printf ("Não achei o mnemônico \"%s\"\n", op_name);
		return;
	}

	printf ("%s: %d endereço(s):", op->o_name, op->o_end - op->o_begin);

	for (ap = op->o_begin; ap < op->o_end; ap++)
		printf (" %X" , ap->o_opcode);

	printf ("\n");

}	/* end print_op_entry */

/*
 ****************************************************************
 *	Imprime os nomes dos operadores com colisão		*
 ****************************************************************
 */
void
print_op_colision (int max_colision)
{
	int		n_col, i = 0;
	OPCODE		*op, **hp, **ehp;

	/*
	 *	Percorre a tabela HASH
	 */
	for (n_col = 1; n_col <= max_colision; n_col++)
	{
		for (hp = &op_hash_tb[0], ehp = &op_hash_tb[op_hash_sz]; hp < ehp; hp++)
		{
			for (op = *hp; op != NOOPCODE; op = op->o_link)
			{
				if (op_hash (op->o_name) != op)
					printf ("Não achei o mnemônico \"%s\"\n", op->o_name);

				if (ncolision  != n_col)
					continue;

				if (i % 8 == 0)
				{
					if (i == 0)
						printf ("\nMnemônico(s) com %d colisão(ões):", n_col);
					printf ("\n");
				}
	
				printf (" %-8.8s", op->o_name);

				i++;
			}
		}

		if (i > 0)
			{ printf ("\n"); i = 0; }
	}

}	/* end print_op_colision */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);
	
	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */

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
		"%s - testa a tabela de operadores do AS\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [<opcode>] ...\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: verboso\n"
	);

	exit (2);

}	/* end help */
