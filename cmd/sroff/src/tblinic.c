/*
 ****************************************************************
 *								*
 *			tblinic.c				*
 *								*
 *	Inicializa as variáveis relativas às tabelas.		*
 *								*
 *	Versão	2.0.0, de 08.07.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		inictbl						*
 *								*
 *	Modulo: SROFF						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/sroff.h"
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Inicializa variaveis relativas as tabelas.		*
 ****************************************************************
 */
void
inictbl (void)
{
	center = expand = box = doublebox = allbox = FALSE;
	tab = '\t';
	lintit = -1;
	temtit = titul = titneg = FALSE;
	maxdata = contdata = maxfmt = contfmt = inicfmt = 0;
	numcol = 0;
	dtpont = (DADOS **)NULL;
	fmtpont = (FORMATO **)NULL;
	crtcol = (COLUNA *)NULL;

}	/* end tblinic */
