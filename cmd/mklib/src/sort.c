/*
 ****************************************************************
 *								*
 *			sort.c					*
 *								*
 *	Funções de ordenação da sinopse				*
 *								*
 *	Versão	2.1.0, de 01.12.88				*
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
 ****************************************************************
 *	Ordena os símbolos de todos os módulos			*
 ****************************************************************
 */
void
sort_all_sym_lists (void)
{
	MOD		*mp;

	for (mp = mod_first; mp != NOMOD; mp = mp->m_next)
		sort_mod_sym (mp);

}	/* end sort_all_sym_lists */

#ifdef	SORT_MOD_LIST
/*
 ****************************************************************
 *	Ordena a lista de módulos				*
 ****************************************************************
 */
MOD *
sort_mod_list (MOD *first)
{
	MOD		*mp, **mod_vec, **mpp;
	int		n = 0;

	/*
	 *	Conta o número de módulos e prepara o vetor de ponteiros
	 */
	for (mp = first; mp != NOMOD; mp = mp->m_next)
		n++;

	if (n == 0)
		return (first);

	mod_vec = mpp = alloca (n * sizeof (MOD *));

	for (mp = first; mp != NOMOD; mp = mp->m_next)
		*mpp++ = mp;

	qtsort (mod_vec, n, compar_mod);

	for (mpp = mod_vec, n--; n > 0; n--, mpp++)
		mpp[0]->m_next = mpp[1];

	mpp[0]->m_next = NOMOD;

	return (*mod_vec);

} 	/* end sort_mod_sym */

/*
 ****************************************************************
 *	Compara entradas de módulos				*
 ****************************************************************
 */
int
compar_mod (const void *ve, const void *vd)
{
	const MOD	*me = ve, *md = vd;

	return (strcmp (me->m_mod_nm, md->m_mod_nm));

}	/* end compar_sym */
#endif	SORT_MOD_LIST

/*
 ****************************************************************
 *	Ordena os símbolos de um módulo				*
 ****************************************************************
 */
void
sort_mod_sym (MOD *mp)
{
	SYMTB		*zp, **sym_vec, **zpp;
	int		n;

	/*
	 *	Prepara o vetor de ponteiros
	 */
	if (mp->m_n_sym == 0)
		return;

	sym_vec = zpp = alloca (mp->m_n_sym * sizeof (SYMTB *));

	for (zp = mp->m_sym; zp != NOSYMTB; zp = zp->z_sym_next)
		*zpp++ = zp;

	qtsort (sym_vec, mp->m_n_sym, compar_sym);

	zpp = sym_vec; mp->m_sym = *zpp;

	for (n = mp->m_n_sym - 1; n > 0; n--, zpp++)
		zpp[0]->z_sym_next = zpp[1];

	zpp[0]->z_sym_next = NOSYMTB;

} 	/* end sort_mod_sym */

/*
 ****************************************************************
 *	Compara entradas de símbolos				*
 ****************************************************************
 */
int
compar_sym (const void *ve, const void *vd)
{
	const SYMTB	*ze = ve, *zd = vd;

	return (strcmp (ze->z_sym_nm, zd->z_sym_nm));

}	/* end compar_sym */
