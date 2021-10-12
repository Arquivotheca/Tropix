/*
 ****************************************************************
 *								*
 *			mkasmtb.c				*
 *								*
 *	Geração direta de módulos em linguagem simbólica	*
 *								*
 *	Versão	1.0.0, de 10.11.86				*
 *		3.0.0, de 23.02.95				*
 *								*
 *	Módulo: mkasmtb						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <errno.h>

#include "mkasmtb.h"

/*
 ******	Definições globais **************************************
 */
#define	elif	else if
#define	NOSTR	(char *)NULL

/*
 ******	Protótipos de funções ***********************************
 */
static int	emit_member (const char *, FILE *, const OBJENT *, const void *);

/*
 ****************************************************************
 *	Geração direta de módulos em linguagem simbólica	*
 ****************************************************************
 */
int
mkasmtb
(	const char *as_nm,	/* Nome do módulo a gerar (com ".s") */
	const char *tb_nm,	/* Nome da tabela dada (sem o '_') */
	const void *tb_addr,	/* Endereco da tabela dada */
	long tb_sz,		/* Tamanho da tabela (em bytes) */
	ROW tb_access,		/* Tipo da proteção da tabela */
	int tb_ent_sz,		/* Tamanho de cada entrada da tabela */
	const OBJENT *tb_ent_desc /* Vetor de descrição das entradas */
)
{
	register const void	*vp;
	register const OBJENT	*op;
	register FILE		*fp;
	register int		i, offset, ent_count, count;
	register int		size, error_count = 0;

	/*
	 *	Examina a descrição da entrada (se houver)
	 */
	if ((op = tb_ent_desc) != NOOBJENT)
	{
		offset = 0;

		for (i = 0; op->o_size != 0; op++, i++)
		{
			size = op->o_count * op->o_size;

			if   ((unsigned)op->o_offset >= tb_ent_sz)
			{
				fprintf
				(	stderr,
					"mkasmtb (módulo \"%s\"): Entrada %d com deslocamente inválido: %d\n",
					as_nm, i, op->o_offset
				);
				error_count++;
			}
			elif ((unsigned)op->o_offset + size > tb_ent_sz)
			{
				fprintf
				(	stderr,
					"mkasmtb (módulo \"%s\"): Entrada %d com tamanho inválido: %d\n",
					as_nm, i, op->o_size
				);
				error_count++;
			}
			elif (op->o_offset < offset)
			{
				fprintf
				(	stderr,
					"mkasmtb (módulo \"%s\"): Entrada %d fora de ordem\n",
					as_nm, i
				);
				error_count++;
			}
			else
			{
				offset = op->o_offset + size;
			}
		}

	}	/* end for (perrcorrendo a descrição das entradas) */

	if (error_count > 0)
		return (-1);

	/*
	 *	Cria o arquivo a gerar
	 */
	if ((fp = fopen (as_nm, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"mkasmtb: Não consegui criar \"%s\" (%s)\n",
			as_nm, strerror (errno)
		);
		return (-1);
	}

	/*
	 *	Coloca o prólogo
	 */
	if (tb_access == RO || tb_access == RW)
		fprintf (fp, "	.global	_%s\n", tb_nm);

	if ((op = tb_ent_desc) != NOOBJENT)
	{
		for (/* acima */; op->o_size != 0; op++)
		{
			if (op->o_rel_nm != NOSTR && op->o_rel_nm[0] != '\0')
				fprintf (fp, "	.global	_%s\n", op->o_rel_nm);
		}
	}

	fprintf
	(	fp,
		(tb_access == RW || tb_access == RWL ? "\t.data\n\n" : "\t.const\n\n")
	);

	fprintf (fp, "_%s:\n", tb_nm);

	/*
	 *	Calcula o número de entradas
	 */
	if (tb_sz % tb_ent_sz != 0)
	{
		fprintf
		(	stderr,
			"mkasmtb (módulo \"%s\"): O tamanho da tabela não é múltiplo da entrada\n",
			as_nm
		);
		return (-1);
	}

	ent_count = tb_sz / tb_ent_sz;

	/*
	 *	Coloca a informação
	 */
	for (i = 0, vp = tb_addr; i < ent_count; i++, vp += tb_ent_sz)
	{
		for (offset = 0; offset < tb_ent_sz; /* abaixo */)
		{
			/*
			 *	Se não houver descrição, emite um "char"
			 */
			if ((op = tb_ent_desc) == NOOBJENT)
			{
			    emit_one_byte:
				fprintf
				(	fp,
					"\t.byte\t0x%02X\n",
					*(char *)(vp + offset)
				);
				offset += 1;
				continue;
			}

			/*
			 *	Procura uma descrição
			 */
			for (/* acima */; /* abaixo */; op++)
			{
				if (op->o_size == 0)
					goto emit_one_byte;

				if (op->o_offset == offset)
					break;

			}	/* end for (itens da descrição) */

			/*
			 *	Encontrou uma descrição
			 */
			for (count = 0; count < op->o_count; count++)
			{
				if (emit_member (as_nm, fp, op, vp + offset) < 0)
					return (-1);

				if ((offset += op->o_size) > tb_ent_sz)
				{
					fprintf
					(	stderr,
						"mkasmtb (módulo \"%s\"): Deslocamento %d fora dos limites\n",
						as_nm, offset
					);
					return (-1);
				}
			}

		}	/* end for (membros da entrada) */

		fprintf (fp, "\n");

	}	/* end for (entradas) */

	fclose (fp);

	return (0);

}	/* end mkasmtb */

/*
 ****************************************************************
 *	Gera um membro de tamanho maior do que um byte		*
 ****************************************************************
 */
static int
emit_member
(	const char *as_nm,
	register FILE *fp,
	register const OBJENT *op,
	register const void *vp
)
{
	/*
	 *	Sem relocação
	 */
	if (op->o_rel_nm == NOSTR || op->o_rel_nm[0] == '\0')
	{
		switch (op->o_size)
		{
		    case sizeof (char):
			fprintf (fp, "\t.byte\t0x%02X\n", *(char *)vp & 0xFF);
			return (0);

		    case sizeof (short):
			fprintf (fp, "\t.word\t0x%04X\n", *(short *)vp & 0xFFFF);
			return (0);

		    case sizeof (long):
			fprintf (fp, "\t.long\t0x%08X\n", *(long *)vp);
			return (0);

		    default:
			fprintf
			(	stderr,
				"mkasmtb (módulo \"%s\"): Tamanho %d inválido de membro\n",
				as_nm, op->o_size
			);
			return (-1);

		}	/* end switch */

	}	/* end sem relocação */

	/*
	 *	Com relocação
	 */
	if (op->o_size != sizeof (long))
	{
		fprintf
		(	stderr,
			"mkasmtb (módulo \"%s\", rel_nm = \"%s\"): Tamanho de ponteiro (%d) inválido\n",
			as_nm, op->o_rel_nm, op->o_size
		);
		return (-1);
	}

	fprintf (fp, "\t.long\t");

	if   (*(long *)vp == 0)
	{
		fprintf (fp, "0");
	}
	else
	{
		fprintf (fp, "_%s", op->o_rel_nm);

		if   (*(long *)vp > (long)op->o_rel_addr)
			fprintf (fp, "+%d", *(long *)vp - (long)op->o_rel_addr);
		elif (*(long *)vp < (long)op->o_rel_addr)
			fprintf (fp, "-%d", (long)op->o_rel_addr - *(long *)vp);
	}

	fprintf (fp, "\n");

	return (0);

}	/* end emit_member */
