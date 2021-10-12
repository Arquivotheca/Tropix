/*
 ****************************************************************
 *								*
 *			   hash.h				*
 *								*
 *	Função de HASH						*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 23.08.01				*
 *								*
 *	Módulo:	CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Função de Hash						*
 ****************************************************************
 */
SYMTB *
hash (const char *name, SYMTB ***link_place)
{
	SYMTB		*sp, **hp;
	int		len;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	if (name == token.l_name)
		len = token.l_nm_len;
	else
		len = strlen (name);

	hp = &hashtb[strhash (name, len, HASHSZ)];

	if ((sp = *hp) == NOSYMTB)
		{ *link_place = hp; return (NOSYMTB); }

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (streq (sp->s_name, name))
			return (sp);
#ifdef	MK
		ncolision++;
		printf
		(	"HASH: Colisão de \"%s\" com \"%s\"\n",
			name, sp->s_name
		);
#endif	MK
		if (sp->s_link == NOSYMTB)
			{ *link_place = &sp->s_link; return (NOSYMTB); }

		sp = sp->s_link;
	}

}	/* end hash */
