/*
 ****************************************************************
 *								*
 *			sub.c					*
 *								*
 *	Subrotinas auxiliares					*
 *								*
 *	Versão	1.0.0, de 23.10.86				*
 *		4.3.0, de 11.06.02				*
 *								*
 *	Módulo: ld						*
 *		Utilitário básico				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <a.out.h>
#include <errno.h>
#include <wsdb.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#undef	COLISÃO
#ifdef	COLISÃO
entry int		ncolision;	/* Contador de colisões */
#endif	COLISÃO

/*
 ****** Estrutura da tabela de símbolos	*************************
 */
entry GSYM		**link_place;	/* Ponteiro para a entrada da HASHTB */

entry GSYM		gsym_list,	/* A lista de símbolos, por ordem de inserção */
			*gsym_last = &gsym_list;

entry int		gsym_index;	/* No. de entradas já em gsym_list */

/*
 ****** Estrutura da tabela de módulos **************************
 */
entry GSYM		*hashtb[HASHSZ]; /* A Tabela Hash */

entry MODULE		modtb_list,	/* A lista de módulos */
			*modtb_last = &modtb_list;

/*
 ****************************************************************
 *	Insere um Módulo na Tabela				*
 ****************************************************************
 */
MODULE *
put_module (const char *nm, int type, dev_t dev, ino_t ino, const HEADER *hp)
{
	MODULE		*mp;
	const char	*str;

	/*
	 *	O Nome deve ser copiado!
	 */
	if ((mp = malloc (sizeof (MODULE))) == NOMODULE)
		error (NOSTR);

	if ((str = strdup (nm)) == NOSTR)
		error (NOSTR);

	mp->m_nm     = str;
	mp->m_type   = type;
	mp->m_dev    = dev;
	mp->m_ino    = ino;
#if (0)	/*******************************************************/
	mp->m_size.t = hp->h_tsize;
	mp->m_size.d = hp->h_dsize;
	mp->m_size.b = hp->h_bsize;
   /***	mp->m_addr.t = ...; ***/
   /***	mp->m_addr.d = ...; ***/
   /***	mp->m_addr.b = ...; ***/
#endif	/*******************************************************/
	mp->m_next   = NOMODULE;

	modtb_last->m_next = mp; modtb_last = mp;

	return (mp);

}	/* end put_module */

/*
 ****************************************************************
 *	Função de Hash						*
 ****************************************************************
 */
GSYM *
hash (const char *name, int len)
{
	GSYM		*gp;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	link_place = &hashtb[strhash (name, len, HASHSZ)];

	if ((gp = *link_place) == NOGSYM)
		return (NOGSYM);

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (streq (gp->s.s_name, name))
			return (gp);
#ifdef	COLISÃO
		ncolision++;
#endif	COLISÃO
		if (gp->s_link == NOGSYM)
			{ link_place = &gp->s_link; return (NOGSYM); }
		else
			gp = gp->s_link;
	}

}	/* end hash */

/*
 ****************************************************************
 *	Insere um Símbolo na Tabela				*
 ****************************************************************
 */
GSYM *
put_global_symbol (const SYM *sp, const MODULE *mp)
{
	GSYM		*gp;
	int		sp_sz, gp_sz;

	/*
	 *	Coloca mais um elemento na lista
	 */
	sp_sz  = SYM_SIZEOF (sp->s_nm_len);
	gp_sz = sizeof (GSYM) - sizeof (SYM) + sp_sz;

	if ((gp = malloc (gp_sz)) == NOGSYM)
		error (NOSTR);

	gp->s_index   = gsym_index++;
	gp->s_next    = NOGSYM;
   /***	gp->s_link    = ...; ***/
	gp->s_mod     = mp;

	gp->s_ref_vec = NULL;
	gp->s_ref_len = 0;
	gp->s_ref_sz  = 0;

   /***	gp->s....     = ...; ***/

	gsym_last->s_next = gp; gsym_last = gp;

	SYMTB_SIZE += sp_sz; 	/* Atualiza o tamanho da tabela de símbolos */

	/*
	 *	Copia os campos
	 */
	memmove (&gp->s, sp, sp_sz);

	/*
	 *	Insere na tabela hash
	 */
	gp->s_link = NOGSYM; *link_place = gp;

	return (gp);

}	/* end put_global_symbol */

#ifdef	COLISÃO
/*
 ****************************************************************
 *	Teste de colisão					*
 ****************************************************************
 */
void
colision_tst (void)
{

	GSYM	*gp;
	int	n = 0;

	ncolision = 0;

	for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
	{
		if (hash (gp->s.s_name, gp->s.s_nm_len) != gp)
			error ("Busca de HASH falhou para \"%s\"", gp->s.s_name);

		n++;
	}

	printf
	(	"Total de símbolos = %d, gsym_index = %d, colisões = %d\n",
		n, gsym_index, ncolision
	);


}	/* end colision_tst */
#endif	COLISÃO

/*
 ****************************************************************
 *	Define os Símbolos Internos				*
 ****************************************************************
 */
void
defsym (const char *name, int type, long value)
{
	GSYM		*gp;
	SYM		*sp;
	int		sz, len;

	len = strlen (name); sz = SYM_SIZEOF (len);

	if ((gp = hash (name, len)) == NOGSYM)
	{
		sp = alloca (sz);

		sp->s_value  = value;
		sp->s_type   = type;
		sp->s_flags  = S_REF;
	   	sp->s_nm_len = len;
		strncpy (sp->s_name, name, SYM_NM_SZ (len));

		put_global_symbol (sp, NOMODULE);
	}
	else
	{
		if (!IS_UNDEF (&gp->s))
			error ("@Símbolo \"%s\" redefinido", name);

		gp->s.s_value = value;
		gp->s.s_type  = type;
		gp->s.s_flags = S_REF;
	}

}	/* end defsym */

/*
 ****************************************************************
 *	Edita um tipo de símbolo				*
 ****************************************************************
 */
const char *
edit_sym_type (const SYM *sp)
{
	switch (sp->s_type)
	{
	    case TEXT:
	    case EXTERN|TEXT:
		return ("TEXT");

	    case DATA:
	    case EXTERN|DATA:
		return ("DATA");

	    case BSS:
	    case EXTERN|BSS:
		return ("BSS");

	    case ABS:
	    case EXTERN|ABS:
		return ("ABS");

	    case UNDEF:
	    case EXTERN|UNDEF:
		if (sp->s_value == 0)
			return ("UNDEF");
		else
			return ("COMMON");

	    default:
		return ("???");

	}	/* end switch */

}	/* end edit_sym_type */

/*
 ****************************************************************
 *	Obtém o nome do módulo de definição de uma variável	*
 ****************************************************************
 */
const char *
get_module_name (const GSYM *gp)
{
	if (gp->s_mod != NOMODULE)
		return (gp->s_mod->m_nm);
	else
		return ("???");

}	/* end get_module_name */
