/*
 ****************************************************************
 *								*
 *			changes.c				*
 *								*
 *	Alterações em um texto					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Tipos Locais						*
 ****************************************************************
 */
typedef	struct	change		CHANGE;

struct	change
{
	BASE	*c_ant;
	BASE	*c_pos;
	BASE	*c_list;
	CHANGE	*c_next;
};

#define	NOCHANGE (CHANGE *)0

/*
 ****************************************************************
 *	Variáveis Locais					*
 ****************************************************************
 */
static CHANGE	*Changes = NOCHANGE;
static CHANGE	*Tail = NOCHANGE;	/* Ainda não implementado */
static bool	init = true;

/*
 ******	Protótipos de funções ***********************************
 */
static void	insertchange (CHANGE *);

/*
 ****************************************************************
 *	Iniciando salvamento					*
 ****************************************************************
 */
void
txt_usave (void)
{
	init = true;

}	/* end txt_usave */

/*
 ****************************************************************
 *	Acumula alterações					*
 ****************************************************************
 */
void
txt_lsave (BASE *anterior, BASE *posterior)
{
	CHANGE		*oc;

	/*
	 *	Cria um novo bloco reproduzindo a lista
	 */
	oc = malloc (sizeof (CHANGE));

	if (anterior->pos == posterior)
	{
		/*
		 *	Lista vazia
		 */
		if (Debug)
			fprintf (stderr, "Lista vazia\n");

		oc->c_list = NOBASE;
	}
	else
	{
		/*
		 *	Copia a lista
		 */
		BASE	*b;
		BASE		*lb,
				*ob;

		b = (BASE *)malloc (sizeof (BASE));
		b->ch = (char *)malloc (strlen (anterior->pos->ch) +1);
		strcpy (b->ch, anterior->pos->ch);

		if (Debug)
			fprintf (stderr, "Copiando: \"%s\"\n", b->ch);

		oc->c_list = b;

		for (ob = anterior->pos->pos; ob != posterior; ob = ob->pos)
		{
			lb = b;
			b = malloc (sizeof (BASE));
			lb->pos = b;
			b->ant = lb;
			b->ch = (char *)malloc (strlen (ob->ch) +1);
			strcpy (b->ch, ob->ch);

			if (Debug)
				fprintf (stderr, "Copiando: \"%s\"\n", b->ch);
		}

		oc->c_list->ant = b;
	}

	/*
	 *	Guarda localização do bloco no texto
	 */
	oc->c_ant = anterior;
	oc->c_pos = posterior;

	insertchange (oc);

}	/* end txt_lsave */

/*
 ****************************************************************
 *	Salvamento para frente					*
 ****************************************************************
 */
void
txt_fsave (BASE *base)
{
	CHANGE		*oc;

	if (Debug)
		fprintf (stderr, "fsave: %s\n", base->ch);

	/*
	 *	Cria um novo bloco sem lista
	 */
	oc = malloc (sizeof (CHANGE));
	oc->c_list = NOBASE;

	/*
	 *	Guarda a localização do bloco no texto
	 */
	oc->c_ant = base;
	oc->c_pos = base->pos;

	/*
	 *	Insere a alteração
	 */
	insertchange (oc);

}	/* end txt_fsave */

/*
 ****************************************************************
 *	Salvamento para trás					*
 ****************************************************************
 */
void
txt_bsave (BASE *base)
{
	CHANGE		*oc;

	if (Debug)
		fprintf (stderr, "bsave: %s\n", base->ch);

	/*
	 *	Cria um novo bloco sem lista
	 */
	oc = malloc (sizeof (CHANGE));
	oc->c_list = NOBASE;

	/*
	 *	Guarda a localização do bloco no texto
	 */
	oc->c_ant = base->ant;
	oc->c_pos = base;

	/*
	 *	Insere a alteração
	 */
	insertchange (oc);

}	/* end txt_bsave */

/*
 ****************************************************************
 *	Informa cada alteração					*
 ****************************************************************
 */
bool
txt_change (BASE **anteriorp, BASE **posteriorp, int *intcountp, int *extcountp)
{
	static CHANGE	*c = NOCHANGE;
	static bool	new = true;
	int		i;
	BASE		*b;

	if (Debug)
		fprintf (stderr, "\ntxt_changes:\n");

	/*
	 *	Verifica se há alterações
	 */
	if (Changes == NOCHANGE)
		return (false);

	/*
	 *	Primeira vez
	 */
	if (new)
		{ c = Changes; new = false; }

	/*
	 *	Última vez
	 */
	if (c == NOCHANGE)
		{ new = true; return (false); }

	/*
	 *	Localização
	 */
	*anteriorp = c->c_ant;
	*posteriorp = c->c_pos;

	/*
	 *	Conta o tamanho interno
	 */
	if (c->c_list)
	{
		i = 1;

		for (b = c->c_list; b != c->c_list->ant; b = b->pos)
		{
			i++;

			if (Debug)
				fprintf (stderr, "interno: %s\n", b->ch);
		}

		if (Debug)
			fprintf (stderr, "interno: %s\n", b->ch);

		*intcountp = i;
	}
	else
	{
		*intcountp = 0;
	}

	/*
	 *	Conta o tamanho externo
	 */
	i = 0;

	for (b = (*anteriorp)->pos; b != *posteriorp; b = b->pos)
	{
		i++;

		if (Debug)
			fprintf (stderr, "externo: %s\n", b->ch);
	}

	*extcountp = i;

	/*
	 *	Avança para a próxima alteração
	 */
	c = c->c_next;

	return (true);

}	/* end txt_change */

/*
 ****************************************************************
 *	Restaura o texto ao seu estado anterior			*
 ****************************************************************
 */
BASE *
txt_restore (void)
{
	CHANGE		*c;
	BASE		*first,
			*last;
	BASE		*base;

	base = NOBASE;

	/*
	 *	Restaura o texto ao seu estado anterior
	 */
	for (c = Changes; c != NOCHANGE; c = c->c_next)
	{
		/*
		 *	Listas interna e externa vazias
		 */
		if (c->c_list == NOBASE && c->c_ant->pos == c->c_pos)
		{
			if (Debug)
				fprintf (stderr, "Ambas listas vazias\n");

			continue;
		}

		/*
		 *	Verifica as listas
		 */
		if (c->c_ant->pos == c->c_pos)
		{
			/*
			 *	Lista externa vazia
			 */
			c->c_ant->pos = c->c_list;
			c->c_pos->ant = c->c_list->ant;

			c->c_list->ant->pos = c->c_pos;
			c->c_list->ant = c->c_ant;

			c->c_list = NOBASE;
		}
		elif (c->c_list == NOBASE)
		{
			/*
			 *	Lista interna vazia
			 */
			c->c_list = c->c_ant->pos;
			c->c_list->ant = c->c_pos->ant;

			c->c_ant->pos = c->c_pos;
			c->c_pos->ant = c->c_ant;
		}
		else
		{
			/*
			 *	Listas interna e externa não vazias
			 */
			first = c->c_ant->pos;
			last = c->c_pos->ant;

			first->ant->pos = c->c_list;
			last->pos->ant = c->c_list->ant;

			c->c_list->ant->pos = last->pos;
			c->c_list->ant = first->ant;

			c->c_list = first;
			c->c_list->ant = last;
		}

		/*
		 *	Computa a nova referência
		 */
		if (ISLAST (c->c_ant))
			base = c->c_ant;
		else
			base = c->c_ant->pos;
	}

	if (base != NOBASE)
		set (Text->t_flags, T_CHANGED|T_EDITION);

	return (base);

}	/* end txt_restore */

/*
 ****************************************************************
 *	Inserção de uma Alteração				*
 ****************************************************************
 */
/*
 *	Hipótese: as linhas entre anterior e posterior
 *		  não foram "txt_save'd" antes
 */
static void
insertchange (CHANGE *oc)
{
	CHANGE		*c, *lc;

	/*
	 *	Sinaliza alteração no Texto
	 */
	set (Text->t_flags, T_CHANGED|T_EDITION);

	/*
	 *	Inicialização: libera a memória
	 */
	if (init)
	{
		BASE	*b;

		for (c = Changes; c != NOCHANGE; c = c->c_next)
		{
			if (c->c_list != NOBASE)
			{
				for (b = c->c_list; b != c->c_list->ant; b = b->pos)
				{
					free (b->ch);
					free (b);
				}
			}

			free (c);
		}

		Changes = NOCHANGE;
		Tail = NOCHANGE;

		init = false;
	}

	/*
	 *	Insere o bloco na lista de blocos
	 */
	if (Changes == NOCHANGE)
	{
		Changes = oc;
		oc->c_next = NOCHANGE;
	}
	else
	{
		oc->c_next = Changes;
		Changes = oc;
	}

	/*
	 *	Tenta agrupar o novo bloco com os outros na lista
	 */
	lc = Changes;

	for (c = Changes->c_next; c != NOCHANGE; c = c->c_next)
	{
		if (Debug)
			fprintf (stderr, "Procurando agrupar\n");

		/*
		 *	Procura o bloco
		 */
		if (oc->c_pos->ant == c->c_ant)
		{
			/*
			 *	Encontrado o bloco posterior
			 */
			BASE	*last;

			if (oc->c_list != NOBASE && c->c_list != NOBASE)
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco posterior\n");

				last = c->c_list->ant;

				oc->c_list->ant->pos = c->c_list;
				c->c_list->ant = oc->c_list->ant;

				oc->c_list->ant = last;

#if (0)	/*************************************/
				oc->c_ant = oc->c_ant;
#endif	/*************************************/
				oc->c_pos = c->c_pos;
			}
			elif (c->c_list != NOBASE)
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco posterior: estou vazio\n");

				oc->c_list = c->c_list;
#if (0)	/*************************************/
				oc->c_ant = c->c_ant;
#endif	/*************************************/
				oc->c_pos = c->c_pos;
			}
			else
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco posterior vazio\n");
			}

			lc->c_next = c->c_next;
			free (c);
		}
		elif (oc->c_ant->pos == c->c_pos)
		{
			/*
			 *	Encontrado o bloco anterior
			 */
			BASE	*last;

			if (oc->c_list != NOBASE && c->c_list != NOBASE)
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco anterior\n");

				last = oc->c_list->ant;

				c->c_list->ant->pos = oc->c_list;
				oc->c_list->ant = c->c_list->ant;

				oc->c_list = c->c_list;
				c->c_list->ant = last;

				oc->c_ant = c->c_ant;
#if (0)	/*************************************/
				oc->c_pos = oc->c_pos;
#endif	/*************************************/
			}
			elif (c->c_list != NOBASE)
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco anterior: estou vazio\n");

				oc->c_list = c->c_list;
				oc->c_ant = c->c_ant;
				oc->c_pos = c->c_pos;
			}
			else
			{
				if (Debug)
					fprintf (stderr, "Encontrei o bloco anterior vazio\n");
			}

			lc->c_next = c->c_next;
			free (c);
		}

		lc = c;
	}

}	/* end insertchange */
