/*
 ****************************************************************
 *								*
 *			lst.c					*
 *								*
 *	Geração da listagem					*
 *								*
 *	Versão	1.0.0, de 16.04.86				*
 *		3.0.0, de 28.02.94				*
 *								*
 *	Rotinas:						*
 *		initlist	prntloc		prntvalue	*
 *		prntcode	prntln				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
static short	valcol;		/* Número de Colunas já Impressas */
static short	locflg;		/* LC já foi impresso */

/*
 ****************************************************************
 *	Inicializa a Listagem					*
 ****************************************************************
 */
void
initlist (void)
{
	locflg = 0;

	if ((list = fopen (listname, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar \"%s\" (%s)\n",
			pgname, listname, strerror (errno)
		);
		lflag = 0;
	}

}	/* end initlist */

/*
 ****************************************************************
 *	Imprime o Location Counter				*
 ****************************************************************
 */
void
prntloc (long loc)
{
	if (locflg++)
		return;

	fprintf (list, "%06X", loc);

}	/* end prntloc */

/*
 ****************************************************************
 *	Escreve um Número em Hexa				*
 ****************************************************************
 */
void
prntvalue (unsigned value)
{
	if (!locflg++)
		fputs ("      ", list);

	fprintf	(list, " %P", value);

	valcol = 10;

}	/* end prntvalue */

/*
 ****************************************************************
 *   Escreve o Codigo da Instrução (na ordem "little endian")	*
 ****************************************************************
 */
void
prntcode (int value, int sz)
{
	if (valcol >= 15)
	{
		prntln ();
		fputs ("      ", list);
		valcol = 0;
		locflg++;
	}
	else
	{
		prntloc (*lc);
	}

	switch (sz)
	{
	    case 2:
		fprintf (list, " %02X", value);
		break;

	    case 4:
		fprintf (list, " %02X%02X", value & 0xFF, value >> 8);
		break;

	    default:
		msg
		(	ERRO, NOMARK,
			"Tipo imprevisto de tamanho de prntcode: %d",
			sz
		);
		break;
	}

	valcol += 1 + sz;

}	/* end prntcode */

/*
 ****************************************************************
 *    Escreve o Codigo da Instrução (na ordem "big endian")	*
 ****************************************************************
 */
void
prntbigcode (int value, int sz)
{
	if (valcol >= 15)
	{
		prntln ();
		fputs ("      ", list);
		valcol = 0;
		locflg++;
	}
	else
	{
		prntloc (*lc);
	}

	switch (sz)
	{
	    case 2:
		fprintf (list, " %02X", value);
		break;

	    case 4:
		fprintf (list, " %02X%02X", value >> 8, value &0xFF);
		break;

	    default:
		msg
		(	ERRO, NOMARK,
			"Tipo imprevisto de tamanho de prntcode: %d",
			sz
		);
		break;
	}

	valcol += 1 + sz;

}	/* end prntbigcode */

/*
 ****************************************************************
 *	Imprime a Linha do Comando				*
 ****************************************************************
 */
void
prntln (void)
{
	static short	lastline = -1;
	short		i;

	if (lastline != token->l_line)
	{
		if (!locflg)
			fputs ("      ", list);
		for (i = 18 - valcol; i > 0; i--)
			fputc (' ', list);
		fnputs (line, list);
		lastline = token->l_line;
	}
	else
	{
		fputc ('\n', list);
	}

	locflg = valcol = 0;

}	/* end prntln */
