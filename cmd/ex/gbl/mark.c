/*
 ****************************************************************
 *								*
 *			mark.c					*
 *								*
 *	Marcação de posições					*
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

#include "../h/global.h"
#include "../h/text.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Armazenamento das marcações				*
 ****************************************************************
 */
typedef	struct
{
	BASE	*m_base;
	int	m_order;

}	MARK;

static MARK	mark['z' - 'a' + 2];

#define	ID(c)	((c == ' ') ? 'z' - 'a' + 1 : c - 'a')

/*
 ****************************************************************
 *	Associa uma marca a uma base				*
 ****************************************************************
 */
void	
mrk_define (int id, BASE *base, int order)
{
	MARK		*mp;

	mp = &mark[ID (id)];

	mp->m_base = base;
	mp->m_order = order;

}	/* end mrk_define */

/*
 ****************************************************************
 *	Informa a base associada a uma marca			*
 ****************************************************************
 */
BASE *
mrk_base (int id)
{
	MARK		*mp;
	BASE		*base, *b;
	
	/*
	 *	Verifica a existência da marcação
	 */
	mp = &mark[ID (id)];

	if ((base = mp->m_base) == NOBASE)
		return (NOBASE);

	/*
	 *	Procura a base no texto
	 */
	for (b = FIRST (); /* teste abaixo */; b = b->pos)
	{
		if (b == base)
			return (base);

		/*
		 *	Teste de término
		 */
		if (ISLAST (b))
			break;
	}

	/*
	 *	Se base esta ausente do Texto,
	 *	desmarca base e retorna NOBASE
	 */
	mp->m_base = NOBASE;

	return (NOBASE);

}	/* end mrk_base */

/*
 ****************************************************************
 *	Informa a base e a ordem associada a uma marca		*
 ****************************************************************
 */
bool
mrk_pos (int id, BASE **basep, int *orderp)
{
	MARK		*mp;
	BASE		*base, *b;

	/*
	 *	Verifica a existência da marcação
	 */
	mp = &mark[ID (id)];

	if ((base = mp->m_base) == NOBASE)
		return (false);

	/*
	 *	Procura a base no texto
	 */
	for (b = FIRST (); /* teste abaixo */; b = b->pos)
	{
		/*
		 *	Se a base ainda está no Texto, retorna a base
		 */
		if (b == base)
		{
			*basep = base;
			*orderp = mp->m_order;
			return (true);
		}

		/*
		 *	Teste de término da malha
		 */
		if (ISLAST (b)) 
			break;
	}

	/*
	 *	Se a base está ausente do texto,
	 *	desmarca a base e retorna "false"
	 */
	mp->m_base = NOBASE;

	return (false);

}	/* end mrk_pos */
