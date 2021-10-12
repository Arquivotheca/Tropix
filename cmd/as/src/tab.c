/*
 ****************************************************************
 *								*
 *			tab.c					*
 *								*
 *	Gerência das tabelas de linhas e depuração		*
 *								*
 *	Versão	1.0.0, de 03.03.86				*
 *		4.4.0, de 17.03.03				*
 *								*
 *	Rotinas:						*
 *		putlno		writelnotb			*
 *		putdbtb		writedbtb			*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>
#include <wsdb.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/tabs.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis & Definições Locais				*
 ****************************************************************
 */
static	LNOTB	lnohead,
		*lnotail = &lnohead;

static	DBTB	dbhead,
		*dbtail	= &dbhead;

/*
 ****************************************************************
 *	Insere uma Entrada na Tabela de Linhas			*
 ****************************************************************
 */
LNOTB	*
putlno (void)
{
	LNOTB		*lp;

	if ((lp = malloc (sizeof (LNOTB))) == NOLNOTB)
		error (NOSTR);

	lnotail->l_next = lp;
	lp->l_next	= NOLNOTB;

	return (lnotail = lp);

}	/* end putlno */

#ifdef	DB
/*
 ****************************************************************
 *	Escreve a Tabela de Linhas				*
 ****************************************************************
 */
long
writelnotb (void)
{
	LNOTB		*lp;
	long		sz;
	int		faout;

	sz = 0;
	faout = fileno (aout);

	for (lp = lnohead.l_next; lp != NOLNOTB; lp = lp->l_next)
		sz += write (faout, (char *)&lp->l, sizeof (LINE) );

	return (sz);

}	/* end writelnotb */
#endif	DB

/*
 ****************************************************************
 *	Insere uma Entrada na Tabela de Depuração		*
 ****************************************************************
 */
DBTB	*
putdbtb (void)
{
	DBTB		*dp;

	if ((dp = malloc (sizeof (DBTB))) == NODBTB)
		error (NOSTR);

	dbtail->d_next  = dp;
	dp->d_next	= NODBTB;

	return (dbtail = dp);

}	/* end putdbtb */

#ifdef	DB
/*
 ****************************************************************
 *	Escreve a Tabela de Depuração				*
 ****************************************************************
 */
long
writedbtb (void)
{
	DBTB		*dp;
	long		sz;
	int		faout;

	sz = 0;
	faout = fileno (aout);

	for (dp = dbhead.d_next; dp != NODBTB; dp = dp->d_next)
	      sz += write (faout, (char *)&dp->db, sizeof (DBSYM) );

	return (sz);

}	/* end writedbtb */
#endif	DB
