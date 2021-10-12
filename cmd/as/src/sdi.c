/*
 ****************************************************************
 *								*
 *			sdi.c					*
 *								*
 *	Gerencia de "span-dependent instructions"		*
 *								*
 *	Versão	1.0.0, de 31.03.86				*
 *		4.5.0, de 04.12.03				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#undef	DEBUG

extern SYMTB	*defhead;	/* Lista de símbolos definidos */

/*
 ******	Estruturas da SDI ***************************************
 */
#define	NOSDI		(SDI *)NULL

typedef struct sdi	SDI;

typedef enum	{ JMP, ORG, ALIGN }	SDI_TYPE;

struct	sdi
{
	char		i_type;		/* Tipo: JMP, ORG ou ALIGN */
	schar		i_incr;		/* Aumento no tamanho do SDI */
	schar		i_med_inc;	/* Incremento médio  da SDI (médio) */
	schar		i_max_inc;	/* Incremento máximo da SDI (longo) */

	long		i_value;	/* Endereco mais baixo da SDI */
	SYMTB		*i_label;	/* Variavel do Endereco da SDI */
	SYMTB		*i_list;	/* Primeiro rótulo após o SDI */

	SDI		*i_next;	/* Ponteiro para a SDI seguinte */
};

#define	i_align_mask	i_incr		/* Para ALIGN */
#define	i_gap		i_med_inc

entry SDI	sdi_head;		/* Cabeca da lista de SDI's */

entry SDI	*sdi_begin = &sdi_head,	/* Inicio da Lista de SDI's */
		*sdi_end   = &sdi_head;	/* Fim da Lista de SDI's */

#define		sdi_first	sdi_begin->i_next

/*
 ****************************************************************
 *	Insere uma SDI de JMP na Lista				*
 ****************************************************************
 */
void
put_jmp_sdi (long value, SYMTB *label, int med_inc, int max_inc)
{
	SDI		*sd;

	if ((sd = malloc (sizeof (SDI))) == NOSDI)
		error (NOSTR);

	sdi_end->i_next = sd; sdi_end = sd;

	sd->i_type    = JMP;
	sd->i_incr    = 0;
	sd->i_med_inc = med_inc;
	sd->i_max_inc = max_inc;

	sd->i_value   = value;	/* Aponta para a próxima instrução */
	sd->i_label   = label;
   /***	sd->i_list    = ...;	***/

	sd->i_next    = NOSDI;

}	/* end put_jmp_sdi */

/*
 ****************************************************************
 *	Insere uma SDI de ORG na Lista				*
 ****************************************************************
 */
void
put_org_sdi (long value)
{
	SDI		*sd;

	if ((sd = malloc (sizeof (SDI))) == NOSDI)
		error (NOSTR);

	sdi_end->i_next = sd; sdi_end = sd;

	sd->i_type    = ORG;
   /***	sd->i_incr    = ...;	***/
   /***	sd->i_med_inc = ...;	***/
   /***	sd->i_max_inc = ...;	***/

	sd->i_value   = value;
   /***	sd->i_label   = ...;	***/
   /***	sd->i_list    = ...;	***/

	sd->i_next    = NOSDI;

}	/* end put_org_sdi */

/*
 ****************************************************************
 *	Insere uma SDI de ALIGN na Lista			*
 ****************************************************************
 */
void
put_align_sdi (long value, int align_mask, int gap)
{
	SDI		*sd;

	if ((sd = malloc (sizeof (SDI))) == NOSDI)
		error (NOSTR);

	sdi_end->i_next  = sd; sdi_end = sd;

	sd->i_type	 = ALIGN;
	sd->i_align_mask = align_mask;
	sd->i_gap	 = gap;
   /***	sd->i_max_inc	 = ...;	***/

	sd->i_value	 = value;
   /***	sd->i_label	 = ...;	***/
   /***	sd->i_list    = ...;	***/

	sd->i_next	 = NOSDI;

}	/* end put_align_sdi */

/*
 ****************************************************************
 *	Calcula o Incremento Ótimo das SDIs			*
 ****************************************************************
 */
void
SDIs_metabolism (void)
{
	SDI		*src_sd, *sd;
	SYMTB		*sp;
	long		span;
	int		incr, value_incr;
	long		old_value, new_value, base;
	char		found;

	/*
	 *	Prepara o ponteiro para o rótulo seguinte a cada SDI
	 */
	for (sp = defhead->s_def, sd = sdi_first; sd != NOSDI; sd = sd->i_next)
	{
		for (/* acima */; /* abaixo */; sp = sp->s_def)
		{
			if (sp == NOSYMTB)
				{ sd->i_list = NOSYMTB; break; } 

			if (sp->s.s_type != TEXT)
				continue;

			if (sp->s.s_value >= sd->i_value)
				{ sd->i_list = sp; break; } 
		}
	}

	/*
	 *	Percorre a lista de SDIs até que nada mais se altere
	 */
	do
	{
		found = 0;

		for (src_sd = sdi_first; src_sd != NOSDI; src_sd = src_sd->i_next)
		{
			if (src_sd->i_type != JMP)
				continue;

			/* Se o rótulo não pertencer à seção TEXT ou	*/
			/* estiver indefinido, força a opção longa	*/

			if ((sp = src_sd->i_label)->s.s_type != TEXT)
				span = 400000;
			else
				span = sp->s.s_value - src_sd->i_value;

			if (span >= 0)		/* Para frente */
			{
				if   (span <= 127)
					incr = 0;
				elif (span <= 32767)
					incr = src_sd->i_med_inc;
				else
					incr = src_sd->i_max_inc;
			}
			else			/* Para trás */
			{
				if   (span >= -128)
					incr = 0;
				elif (span - src_sd->i_med_inc >= -32768)
					incr = src_sd->i_med_inc;
				else
					incr = src_sd->i_max_inc;
			}

			if ((value_incr = incr - src_sd->i_incr) <= 0)
				continue;
#ifdef	DEBUG
			printf
			(	"Atualizando SDI JMP em %P, referenciando \"%s\"\n",
				src_sd->i_value, src_sd->i_label->s.s_name
			);
#endif	DEBUG
			src_sd->i_incr   = incr;
			src_sd->i_value += value_incr;

			found++;

			/*
			 *	Percorre a lista de SDIs em paralelo com a tabela de símbolos
			 */
			for (sp = src_sd->i_list, sd = src_sd->i_next; sd != NOSDI; sd = sd->i_next)
			{
				old_value = sd->i_value;

				/* Atualiza a continuação da tabela de símbolos até o SDI em análise */

				for (/* acima */; sp != NOSYMTB; sp = sp->s_def)
				{
					if (sp->s.s_type != TEXT)
						continue;

					if (sp->s.s_value >= old_value)
						break;

					sp->s.s_value += value_incr;

				}	/* end for (lista de SYMTB) */

				/* Analisa este SDI */

				switch (sd->i_type)
				{
				    case JMP:
					sd->i_value += value_incr;
					break;

				    case ORG:
					goto next_src_sd;
					break;

				    case ALIGN:
					if (value_incr <= sd->i_gap)
						{ sd->i_gap -= value_incr; goto next_src_sd; }

					base = old_value - sd->i_gap + value_incr;

					new_value = (base + sd->i_align_mask) & ~sd->i_align_mask;

					sd->i_value = new_value;
					sd->i_gap   = new_value - base;

					value_incr  = new_value - old_value;

					break;

				}	/* end switch */

			}	/* end for (lista de SDIs) */

			/*
			 *	Atualiza o final da tabela de símbolos
			 */
			lctext += value_incr;

			for (/* acima */; sp != NOSYMTB; sp = sp->s_def)
			{
				if (sp->s.s_type != TEXT)
					continue;

				sp->s.s_value += value_incr;

			}	/* end for (lista de SYMTB) */

			/*
			 *	Continua com o próximo SDI a analisar
			 */
		    next_src_sd:
			/* vazio */;

		}	/* for (lista de SDIs) */
	}
	while (found);

}	/* end SDIs_metabolism */

/*
 ****************************************************************
 *	Atualiza os Rótulos					*
 ****************************************************************
 */
long
labelupdate (char sect, long offset)
{
	SYMTB		*sp;

	/*
	 *	Se o deslocamento for NULO, ...
	 */
	if (offset == 0)
		return (0);

	/*
	 *	Atualiza
	 */
	for (sp = defhead->s_def; sp != NOSYMTB; sp = sp->s_def)
	{
		if (sp->s.s_type == sect)
			sp->s.s_value += offset;
	}

	return (0);

}	/* end labelupdate */

/*
 ****************************************************************
 *	Imprime Estatística das SDIs				*
 ****************************************************************
 */
void
sdistat (void)
{
	SDI		*sd;
	int		longos, medios, curtos;

	longos = medios = curtos = 0;

	for (sd = sdi_first; sd != NOSDI; sd = sd->i_next)
	{
		if   (sd->i_incr == 0)
			curtos++;
		elif (sd->i_incr < sd->i_max_inc)
			medios++;
		else
			longos++;
	}

	fprintf
	(	stderr,
		"SDI: Total = %d, %d Longos, %d Medios, %d Curtos\n",
		longos + medios + curtos, longos, medios, curtos
	);

}	/* end sdistat */
