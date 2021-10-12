/*
 ****************************************************************
 *								*
 *			hash.c					*
 *								*
 *	Calcula a posição de um nome na tabela hash		*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.2.3, de 14.12.99				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Função de Hash						*
 ****************************************************************
 */
entry HASH *
hash (char *name)
{
	HASH	*hp;
	SYM	*sp;
	char	*cp, c;
	int	eor, i, incr;

	/*
	 *	Calcula o ou-exclusivo dos caracteres do nome.
	 */
	for (eor = 0, cp = name, i = 0; (c = *cp) != '\0'; cp++, i++)
	{
		if (i & 1)
			eor ^= c;
		else
			eor ^= (c << 8);
	}

	incr = HASHSZ / 2;
	hp   = &hashtb[eor % HASHSZ];

	do
	{	/*
		 *	Verifica se a entrada está desocupada.
		 */
		if ((sp = hp->h_sym) == NOSYM)
			return (hp);

		/*
		 *	A entrada está ocupada: verifica
		 *	se é o nome procurado.
		 */
		if (streq (sp->s_name, name))
			return (hp);

		/*
		 *	Houve colisão: calcula a proxima tentativa.
		 */
		if ((hp += incr) >= &hashtb[HASHSZ])
			hp -= HASHSZ;

	} 	while (incr-- > 0);

	fprintf (stderr, "%s: Tabela hash cheia\n", pgname);
	exit (1);

	return (NOHASH);	/* Para fazer o "lint" feliz */

}	/* end hash */
