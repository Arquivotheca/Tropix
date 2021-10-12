/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Funções auxiliares					*
 *								*
 *	Versão	1.0.0, de 23.10.86				*
 *		4.0.0, de 20.08.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
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
#include <a.out.h>

#include "../h/mklib.h"

/*
 ******	Variáveis globais ***************************************
 */
entry SYMTB	**hash_tb;	/* Endereço da tabela HASH */

/*
 ****************************************************************
 *	Procura um módulo na tabela				*
 ****************************************************************
 */
MOD *
mod_search (const char *mod_nm)
{
	MOD		*mp;

	for (mp = mod_first; /* abaixo */; mp = mp->m_next)
	{
		if (mp == NOMOD)
			return (NOMOD);
		
		if (streq (mp->m_mod_nm, mod_nm))
			return (mp);
	}

}	/* end mod_search */

/*
 ****************************************************************
 *	Procura um módulo na tabela mas pelo último nome	*
 ****************************************************************
 */
MOD *
last_nm_mod_search (const char *mod_nm)
{
	MOD		*mp;

	for (mp = mod_first; /* abaixo */; mp = mp->m_next)
	{
		if (mp == NOMOD)
			return (NOMOD);
		
		if (streq (last_nm (mp->m_mod_nm), mod_nm))
			return (mp);
	}

}	/* end last_nm_mod_search */

/*
 ****************************************************************
 *	Função de Hash						*
 ****************************************************************
 */
SYMTB *
hash (const char *nm, int len, SYMTB ***link_place)
{
	SYMTB		*zp, **hp;

	/*
	 *	Verifica se já há lista de colisão
	 */
	hp = &hash_tb[strhash (nm, len, HASHSZ)];

	if ((zp = *hp) == NOSYMTB)
		{ *link_place = hp; return (NOSYMTB); }

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (zp->z_sym_len == len && streq (zp->z_sym_nm, nm))
			return (zp);

		if (zp->z_link == NOSYMTB)
			{ *link_place = &zp->z_link; return (NOSYMTB); }

		zp = zp->z_link;
	}

}	/* end hash */

/*
 ****************************************************************
 *	Normaliza um caminho					*
 ****************************************************************
 */
const char *
path_can (const char *path)
{
	char		*cp;

	for (cp = strend (path) - 1; cp[0] == '/' && cp > path; cp--)
		*cp = '\0';

	while (path[0] == '.' && path[1] == '/')
		path += 2;

	return (path);

}	/* end path_can */

/*
 ****************************************************************
 *	Obtém o último nome de um caminho			*
 ****************************************************************
 */
const char *
last_nm (const char *path)
{
	const char	*cp;

	if (cp = strrchr (path, '/'))
		return (cp + 1);
	else
		return (path);

} 	/* end last_nm */
