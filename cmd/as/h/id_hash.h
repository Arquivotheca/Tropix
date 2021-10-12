/*
 ****************************************************************
 *								*
 *			   id_hash.h				*
 *								*
 *	Função de HASH dos identificadores			*
 *								*
 *	Versão	1.0.0, de 13.02.86				*
 *		4.1.0, de 24.08.01				*
 *								*
 *	Rotinas:						*
 *		id_hash						*
 *								*
 *	Módulo: AS-68000/10/20/30				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Referencias/definições externas				*
 ****************************************************************
 */
#define	ID_HASH_SZ	997	/* O no. de entradas */

/*
 ****************************************************************
 *	Função de HASH para identificadores			*
 ****************************************************************
 */
SYMTB *
id_hash (const char *name, int len, SYMTB ***link_place)
{
	SYMTB		*sp, **hp;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	hp = &id_hash_tb[strhash (name, len, ID_HASH_SZ)];

#ifdef	MK
	ncolision = 0;
#endif	MK

	if ((sp = *hp) == NOSYMTB)
		{ *link_place = hp; return (NOSYMTB); }

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (sp->s.s_nm_len == len && streq (sp->s.s_name, name))
			return (sp);
#ifdef	MK
		ncolision++;
#endif	MK
		if (sp->s_link == NOSYMTB)
			{ *link_place = &sp->s_link; return (NOSYMTB); }
		else
			sp = sp->s_link;
	}

}	/* end id_hash */
