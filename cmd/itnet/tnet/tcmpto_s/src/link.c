/*
 ****************************************************************
 *								*
 *			link.c					*
 *								*
 *	Comparação remota de árvores (servidor)			*
 *								*
 *	Versão	2.3.0, de 29.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_S/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/tcmpto_s.h"
#include "../../tnet.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry LITEM	*remote_linklist_first,	/* Começo da Lista */
		*remote_linklist_last;	/* Final da Lista */

/*
 ****************************************************************
 *	Processa LINKs da estação remota			*
 ****************************************************************
 */
char *
remote_link_proc (const char *file_nm, STAT *sp)
{
	LITEM	*lp;

	/*
	 *	Se tem apenas 1 LINK, nada precisa ser feito
	 */
	if (sp->st_nlink == 1)
		return (NOSTR);

	/*
	 *	Insere na lista
	 */
	if ((lp = malloc (sizeof (LITEM))) == (LITEM *)NULL)
		error ("$Não obtive memória para item de elo físico");

	if ((lp->l_name = strdup (file_nm)) == NOSTR)
		error ("$Não obtive memória para item de elo físico");

	lp->l_nm_offset = file_nm_offset; 

	lp->l_dev = sp->st_dev;
	lp->l_ino = sp->st_ino;

	lp->l_next = (LITEM *)NULL;

	if (remote_linklist_first == (LITEM *)NULL)
		remote_linklist_first = lp;
	else
		remote_linklist_last->l_next = lp;

	remote_linklist_last = lp;

	/*
	 *	Verifica se um outro nome já foi visto
	 */
	for (lp = remote_linklist_first; lp != remote_linklist_last; lp = lp->l_next)
	{
		if (lp->l_dev == sp->st_dev && lp->l_ino == sp->st_ino)
			return (lp->l_name + lp->l_nm_offset);
	}

	return (NOSTR);

}	/* end remote_link_proc */

/*
 ****************************************************************
 *	Remove a lista de LINKs					*
 ****************************************************************
 */
void
delete_remote_link_list (void)
{
	LITEM	*lp, *next;

	for (lp = remote_linklist_first; lp != (LITEM *)NULL; lp = next)
	{
		free (lp->l_name);

		next = lp->l_next;

		free (lp);
	}

	remote_linklist_first = (LITEM *)NULL;
	remote_linklist_last  = (LITEM *)NULL;

}	/* end delete_remote_link_list */
