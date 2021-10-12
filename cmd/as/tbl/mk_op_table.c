/*
 ****************************************************************
 *								*
 *			mk_op_table.c				*
 *								*
 *	Constrói a tabela de operadores do AS			*
 *								*
 *	Versão	1.0.0, de 17.09.92				*
 *		3.0.0, de 11.03.94				*
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
#include <stddef.h>
#include <stdarg.h>
#include <mkasmtb.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/op_code.h"
#include "../h/src_op_code.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 25.02.94";

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Endereço das tabelas ************************************
 */
entry OPCODE	*op_code_tb;	/* Endereço da parte de "nome + grupo" */

entry OPADDR	*op_addr_tb;	/* Endereço da parte de "opcode + addr" */

extern SRC_OPCODE src_op_table[]; /* A tabela fonte de opcodes */

/*
 ****** Definição da relocação **********************************
 */
entry OBJENT	op_code_rel[] =
{
	{ sizeof (short),    offsetof (OPCODE, o_flags), 1, "",		  NOVOID },
	{ sizeof (OPADDR *), offsetof (OPCODE, o_begin), 1, "op_addr_tb", NOVOID },
	{ sizeof (OPADDR *), offsetof (OPCODE, o_end),   1, "op_addr_tb", NOVOID },
	{ sizeof (OPCODE *), offsetof (OPCODE, o_link),  1, "op_code_tb", NOVOID },
	{ 0 }
};

entry OBJENT	op_addr_rel[] =
{
	{ sizeof (long),     offsetof (OPADDR, o_opnd1),  1, "",	  NOVOID },
	{ sizeof (long),     offsetof (OPADDR, o_opnd2),  1, "",	  NOVOID },
	{ 0 }
};

entry OBJENT	op_hash_rel[] =
{
	{ sizeof (OPADDR *), 0, 1, "op_code_tb", NOVOID },
	{ 0 }
};

entry OBJENT	op_hash_sz_rel[] =
{
	{ sizeof (int),      0, 1, "",		 NOVOID },
	{ 0 }
};

/*
 ****** Protótipos de funções ***********************************
 */
int		find_best_op_hash_size (int);
char		*editpercent (int);
int		getpercent (long, long);
void		error (const char *format, ...);
void		help (void);

/*
 ****************************************************************
 *	A função HASH de operadores				*
 ****************************************************************
 */
entry int	ncolision;

entry int	op_hash_sz;

entry OPCODE	**op_hash_tb;	/* Endereço da Tabela HASH */

#define	MK
#include "../h/op_hash.h"
 
/*
 ****************************************************************
 *	Constrói a tabela de operadores do AS			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		i;
	SRC_OPCODE	*src_op;
	OPCODE		*op;
	OPCODE		*ep;
	OPADDR		*src_ap, *ap;
	int		opcode_cnt = 0, addr_cnt = 0;
	int		opt, new_opcode_cnt = 0;
	int		given_sz = 0;
	int		pri, min_pri = +1000000, max_pri = -1000000;

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
	 *	Conta o no. de entradas e endereçamentos da "optab" fonte
	 */
	for (src_op = src_op_table; src_op->o_name[0][0]; src_op++)
	{
		int		len;

		if (src_op->o_pri < min_pri)
			min_pri = src_op->o_pri;

		if (src_op->o_pri > max_pri)
			max_pri = src_op->o_pri;

		for (i = 0; i < MAX_NAME; i++)
		{
			if (src_op->o_name[i][0] == '\0')
				break;

			opcode_cnt++;

			if ((len = strlen (src_op->o_name[i])) > SIDSZ)
			{
				error
				(	"CUIDADO: O mnemônico \"%s\" tem %d caracteres",
					src_op->o_name[i], len
				); 
			}
		}

		for (i = 0, src_ap = &src_op->o_a[0]; i < MAX_ADDR; i++, src_ap++)
		{
			if
			(	src_ap->o_opcode || src_ap->o_opexten ||
				src_ap->o_flags  ||
				src_ap->o_opnd1  || src_ap->o_opnd2
			)
				{ addr_cnt += 1; }
			else
				{ break; }
		}
	}

	if (vflag) printf
	(	"Prioridade mínima = %d, prioridade máxima = %d\n",
		min_pri, max_pri
	);

	if (vflag) printf
	(	"A tabela fonte tem %d mnemômicos e %d endereçamentos\n",
		opcode_cnt, addr_cnt
	);

	if (vflag) printf
	(	"Memória estimada: code = %d, addr = %d, total = %d\n",
		opcode_cnt * sizeof (OPCODE), addr_cnt * sizeof (OPADDR),
		opcode_cnt * sizeof (OPCODE) + addr_cnt * sizeof (OPADDR)
	);

	/*
	 *	Aloca memória para a tabela nova
	 */
	if ((op_code_tb = op = malloc (opcode_cnt * sizeof (OPCODE))) == NOVOID)
		error (NOSTR);

	if ((op_addr_tb = ap = malloc (addr_cnt * sizeof (OPADDR))) == NOVOID)
		error (NOSTR);

	/*
	 *	Prepara a parte de "nome + grupo" e "opcode + opaddr"
	 */
	for (pri = max_pri; pri >= min_pri; pri--)
	{
		for (src_op = src_op_table; src_op->o_name[0][0]; src_op++)
		{
			OPADDR		*begin_ap;

			if (src_op->o_pri != pri)
				continue;

			begin_ap = ap;

			for (i = 0, src_ap = &src_op->o_a[0]; i < MAX_ADDR; i++, src_ap++)
			{
				if
				(	src_ap->o_opcode || src_ap->o_opexten ||
					src_ap->o_flags  ||
					src_ap->o_opnd1  || src_ap->o_opnd2
				)
	                        	{ memmove (ap++, src_ap, sizeof (OPADDR)); }
				else
					{ break; }
			}

			for (i = 0; i < MAX_NAME; i++)
			{
				if (src_op->o_name[i][0] == '\0')
					break;

				strcpy (op->o_name, src_op->o_name[i]);
				op->o_type  = src_op->o_type;
				op->o_group = src_op->o_group;
				op->o_flags = src_op->o_flags;
				op->o_begin = begin_ap;
				op->o_end   = ap;
				op->o_link  = NOOPCODE;

				op++;
			}

		}	/* end for (tabela) */

	}	/* end for (prioridades) */

	if (op_code_tb + opcode_cnt != op)
		error ("CUIDADO: Ponteiro OP final incorreto");

	if (op_addr_tb + addr_cnt != ap)
		error ("CUIDADO: Ponteiro AP final incorreto");

	/*
	 *	Acha um bom tamanho de HASH
	 */
	if (argv[0] != NOSTR && !streq (argv[0], "-") && !streq (argv[0], "0"))
	{
		if ((op_hash_sz = strtol (argv[0], (const char **)NULL, 0)) <= 0)
			error ("$Tamanho inválido: \"%s\"", argv[0]);

		given_sz++;
	}
	else
	{
		op_hash_sz = find_best_op_hash_size (opcode_cnt);
	}

	/*
	 *	Agora, de posse do bom tamanho, cria a tabela HASH de verdade
	 */
	if ((op_hash_tb = malloc (op_hash_sz * sizeof (OPCODE *))) == NOVOID)
		error (NOSTR);

	memsetl
	(	(long *)op_hash_tb,
		0,
		op_hash_sz * sizeof (OPCODE *) / sizeof (long)
	);

	ep = &op_code_tb[opcode_cnt];

	/*
	 *	Em primeiro lugar, o nomes principais
	 */
	for (op = &op_code_tb[0]; op < ep; op++)
	{
		if (op > &op_code_tb[0] && op[0].o_begin == op[-1].o_begin)
			continue;

		if (op_hash (op->o_name) !=  NOOPCODE)
		{
			error
			(
				"Opcode repetido: \"%s\"",
				op->o_name
			);
		}

		op->o_link = NOOPCODE;
		*op_link_place = op;

		new_opcode_cnt++;

	}	/* end for (percorrendo a "op_tb" */

	/*
	 *	Em segundo lugar, os sinônimos
	 */
	for (op = &op_code_tb[1]; op < ep; op++)
	{
		if (op[0].o_begin != op[-1].o_begin)
			continue;

		if (op_hash (op->o_name) !=  NOOPCODE)
		{
			error
			(
				"Opcode repetido: \"%s\"",
				op->o_name
			);
		}

		op->o_link = NOOPCODE;
		*op_link_place = op;

		new_opcode_cnt++;

	}	/* end for (percorrendo a "op_tb" */

	if (new_opcode_cnt != opcode_cnt)
	{
		error
		(	"Inconsistência no número de "
			"operadores: %d :: %d",
			new_opcode_cnt, opcode_cnt
		);
	}

	/*
	 *	Escreve o melhor resultado
	 */
	if (given_sz) printf
	(
		"OPs = %d, Tamanho = %d (%s %%)\n",
		new_opcode_cnt, op_hash_sz,
		editpercent (getpercent (new_opcode_cnt, op_hash_sz))
	);

	/*
	 *	Termina de inicializar os "OBJENT"s
	 */
	op_code_rel[1].o_rel_addr = op_addr_tb;		/* o_begin */
	op_code_rel[2].o_rel_addr = op_addr_tb;		/* o_end */
	op_code_rel[3].o_rel_addr = op_code_tb;		/* o_link */

	op_hash_rel[0].o_rel_addr = op_code_tb;

	/*
	 *	Gera os módulos em "assembly"
	 */
	mkasmtb
	(	".op_code_tb.s",
		"op_code_tb",
		op_code_tb,
		opcode_cnt * sizeof (OPCODE),
		RO,
		sizeof (OPCODE),
		op_code_rel
	);

	mkasmtb
	(	".op_addr_tb.s",
		"op_addr_tb",
		op_addr_tb,
		addr_cnt * sizeof (OPADDR),
		RO,
		sizeof (OPADDR),
		op_addr_rel
	);

	mkasmtb
	(	".op_hash_tb.s",
		"op_hash_tb",
		op_hash_tb,
		op_hash_sz * sizeof (OPCODE *),
		RO,
		sizeof (OPCODE *),
		op_hash_rel
	);

	mkasmtb
	(	".op_hash_sz.s",
		"op_hash_sz",
		&op_hash_sz,
		sizeof (op_hash_sz),
		RO,
		sizeof (op_hash_sz),
		op_hash_sz_rel
	);

	return (exit_code);

}	/* end mk_op_table */

/*
 ****************************************************************
 *	Gera o próximo no. primo a partir de "base"		*
 ****************************************************************
 */
int
next_prime (int base)
{
	int		candidato, divisor, meio_candidato;

	candidato = base | 01;

	for (EVER)
	{
		candidato += 2;	meio_candidato = candidato >> 1;

		for (divisor = 3; /* abaixo */; divisor += 2)
		{
			if (divisor > meio_candidato)
				return (candidato);

			if (candidato % divisor == 0)
				break;
		}

	}	/* end for (EVER) */

}	/* end next_prime */

/*
 ****************************************************************
 *	Estima o melhor tamanho para a tabela hash		*
 ****************************************************************
 */
int
find_best_op_hash_size (int opcode_cnt)
{
	OPCODE		*op, *ep;
	int		i;
	int		good_size = 0;
	int		new_opcode_cnt = 0;
	int		min_size, max_size;
	int		min_col = 1000000, max_col;
	int		min_try = 1000000, try_cnt;
	char		*cp = NOSTR;

	/*
	 *	Delimita o intervalo de variação do tamanho razoável
	 */
	min_size = next_prime (opcode_cnt + (opcode_cnt >> 1));	/* 1.5 */
	max_size = (min_size << 1);				/* 3.0 */
#if (0)	/*************************************/
	max_size = (opcode_cnt << 1);				/* 2.0 */
	max_size = min_size + opcode_cnt;			/* 2.5 */
#endif	/*************************************/

	/*
	 *	Tenta os vários tamanhos
	 */
	for
	(	op_hash_sz = min_size;
		op_hash_sz <= max_size;
		op_hash_sz = next_prime (op_hash_sz)
	)
	{
		if ((op_hash_tb = malloc (op_hash_sz * sizeof (OPCODE *))) == NOVOID)
			error (NOSTR);

		/*
		 *	Inicializa a tabela HASH
		 */
		memsetl
		(	(long *)op_hash_tb,
			0,
			op_hash_sz * sizeof (OPCODE *) / sizeof (long)
		);

		new_opcode_cnt = 0;
		try_cnt = 0;
		max_col = -1;
		ep = &op_code_tb[opcode_cnt];

		/*
		 *	Preenche a tabela HASH, instalando os opcodes e
		 *	computando-se o maior número de colisões obtidas
		 *
		 *	Em primeiro lugar, o nomes principais
		 */
		for (op = &op_code_tb[0]; op < ep; op++)
		{
			if (op > &op_code_tb[0] && op[0].o_begin == op[-1].o_begin)
				continue;

			if (op_hash (op->o_name) !=  NOOPCODE)
			{
				error
				(
					"Opcode repetido: \"%s\"",
					op->o_name
				);
			}

			op->o_link = NOOPCODE;
			*op_link_place = op;

			new_opcode_cnt++;

			/*
			 *	Guarda o maior no. de colisões
			 */
			if (ncolision > max_col)
			{
				max_col = ncolision;
				cp = (char *)op->o_name;
			}

			try_cnt += ncolision + 1;

		}	/* end for (percorrendo a "op_tb" */

		/*
		 *	Em segundo lugar, os sinônimos
		 */
		for (op = &op_code_tb[1]; op < ep; op++)
		{
			if (op[0].o_begin != op[-1].o_begin)
				continue;

			if (op_hash (op->o_name) !=  NOOPCODE)
			{
				error
				(
					"Opcode repetido: \"%s\"",
					op->o_name
				);
			}

			op->o_link = NOOPCODE;
			*op_link_place = op;

			new_opcode_cnt++;

			/*
			 *	Guarda o maior no. de colisões
			 */
			if (ncolision > max_col)
			{
				max_col = ncolision;
				cp = (char *)op->o_name;
			}

			try_cnt += ncolision + 1;

		}	/* end for (percorrendo a "op_tb" */

		if (new_opcode_cnt != opcode_cnt)
		{
			error
			(	"Inconsistência no número de "
				"operadores: %d :: %d",
				new_opcode_cnt, opcode_cnt
			);
		}

		if (vflag)
		{
			printf
			(	"SZ = %4d (%s %%), ",
				op_hash_sz,
				editpercent (getpercent (new_opcode_cnt, op_hash_sz))
			);

			i = try_cnt;	i += new_opcode_cnt / 200;

			printf
			(	"Tentativas = %5d (%s), Maxcol = %2d (\"%s\")\n",
				try_cnt,
				editpercent (getpercent (i, 100 * new_opcode_cnt)),
				max_col,
				cp
			);
		}

		/*
		 *	Computa o minimo da média de tentativas
		 */
		if (try_cnt < min_try)
		{
			min_col = max_col;
			min_try = try_cnt;
			good_size = op_hash_sz;
		}

		/*
		 *	Libera a area anteriormente alocada
		 */
		free (op_hash_tb);

	}	/* end for (tentando vários tamanhos de HASH) */

	/*
	 *	Escreve o melhor resultado
	 */
	printf
	(	"OPs = %d, Tamanho = %d (%s %%), ",
		new_opcode_cnt,
		good_size,
		editpercent (getpercent (new_opcode_cnt, good_size))
	);

	i = min_try;	i += new_opcode_cnt / 200;

	printf
	(	"Tentativas = %d (%s), Maxcolisões = %d\n",
		min_try,
		editpercent (getpercent (i, 100 * new_opcode_cnt)),
		min_col
	);

	return (good_size);

}	/* end find_best_ophash_size */

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
		"%s - constrói a tabela de operadores do AS\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [<good_hash_size>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: verboso\n"
		"\nObs.:\tSe <good_hash_size> for dado, o valor será usado "
			"ao invés de recalculado\n"
	);

	exit (2);

}	/* end help */
