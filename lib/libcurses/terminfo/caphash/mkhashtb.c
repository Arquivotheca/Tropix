/*
 ****************************************************************
 *								*
 *			mkhashtb.c				*
 *								*
 *	Gera tabela de hash com as capacidades			*
 *								*
 *	Versão	2.0.0, de 27.04.87				*
 *		3.0.0, de 22.02.95				*
 *								*
 *	Modulo: caphash						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <terminfo.h>
#include <mkasmtb.h>

#include "hash.h"

/*
 ****************************************************************
 *	Variáveis Externas					*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL

extern	char	*capbnames[];
extern	char	*capnnames[];
extern	char	*capsnames[];

/*
 ****************************************************************
 *	Variáveis Globais:					*
 ****************************************************************
 */
#ifdef	DEBUG
extern	int	ncolision;	/* Numero de colisões na cap_hashtb */
entry	int	dflag = 0;	/* Switch de Depuração */
#endif	DEBUG

/*
 ******	Definições para "mkasmtb" *******************************
 */
const OBJENT	cap_hashtb_OBJENT[] =
{
	{ sizeof (CAPCLASS), offsetof (CAPHASH, h_class), 1, NOSTR, NOVOID },
	{ sizeof (int),      offsetof (CAPHASH, h_order), 1, NOSTR, NOVOID },
	{ 0 }
};

const OBJENT	int_OBJENT[] =
{
	{ sizeof (int), 0, 1, NOSTR, NOVOID },
	{ 0 }
};


/*
 ****************************************************************
 *	Tabela de Simbolos.					*
 ****************************************************************
 */
static int	BOOLNO;		/* No. de Capacidades Booleanas */
static int	NUMNO;		/* No. de Capacidades Numericas */
static int	STRNO;		/* No. de Capacidades String */

entry CAPHASH	cap_hashtb[CAPHASHSZ];	/* Tabela de hash */

/*
 ****************************************************************
 *	Mensagens de Erro					*
 ****************************************************************
 */
void
error (char *mens, char	*var)
{
	fprintf (stderr, mens, var);

}	/* end error */

/*
 ****************************************************************
 *	Gera as tabelas						*
 ****************************************************************
 */
int
main (void)
{
	CAPHASH		*hp;	/* Hash Pointer */
	char		**tp;	/* Cap-table Pointer */

	/*
	 *	Instala os Nomes das Capacidades Booleanas
	 */
	for (tp = capbnames ; *tp != NOSTR ; tp++)
	{
		hp = caphash (*tp);

		strcpy (hp->h_id, *tp);
		hp->h_class = C_BOOLEAN;
		hp->h_order = tp - capbnames;

	}

	BOOLNO = tp - capbnames;

#ifdef	DEBUG
	if (dflag)
		printf ("BOOLNO: %d\n", BOOLNO);
#endif	DEBUG

	/*
	 *	Instala os Nomes das Capacidades Numericas
	 */
	for (tp = capnnames ; *tp != NOSTR ; tp++)
	{
		hp = caphash (*tp);

		strcpy (hp->h_id, *tp);
		hp->h_class = C_NUMBER;
		hp->h_order = tp - capnnames;
	}

	NUMNO = tp - capnnames;

#ifdef	DEBUG
	if (dflag)
		printf ("NUMNO: %d\n", NUMNO);
#endif	DEBUG

	/*
	 *	Instala os Nomes das Capacidades Strings
	 */
	for (tp = capsnames ; *tp != NOSTR ; tp++)
	{
		hp = caphash (*tp);

		strcpy (hp->h_id, *tp);
		hp->h_class = C_STRING;
		hp->h_order = tp - capsnames;
	}

	/*
	 *	Instala os Nomes das Capacidades Strings com Parâmetros
	 */
	for (tp = tp + 1 ; *tp != NOSTR ; tp++)
	{
		hp = caphash (*tp);

		strcpy (hp->h_id, *tp);
		hp->h_class = C_PSTRING;
		hp->h_order = tp - capsnames;
	}

	STRNO = tp - capsnames;

#ifdef	DEBUG
	if (dflag)
		printf ("STRNO: %d\n", STRNO);
#endif	DEBUG

#ifdef	DEBUG
	if (ncolision > 0 && dflag)
		printf ("**** %d colisão(oes)\n", ncolision);
#endif	DEBUG

	/*
	 *	Constrói as Tabelas
	 */
	mkasmtb
	(	"caphashtb.s",
		"cap_hashtb",
		cap_hashtb,
		sizeof (cap_hashtb),
		RO,
		sizeof (CAPHASH),
		cap_hashtb_OBJENT
	);

	mkasmtb
	(	"boolno.s",
		"BOOLNO",
		&BOOLNO,
		sizeof (BOOLNO),
		RO,
		sizeof (BOOLNO),
		int_OBJENT
	);

	mkasmtb
	(	"numno.s",
		"NUMNO",
		&NUMNO,
		sizeof (NUMNO),
		RO,
		sizeof (NUMNO),
		int_OBJENT
	);

	mkasmtb
	(	"strno.s",
		"STRNO",
		&STRNO,
		sizeof (STRNO),
		RO,
		sizeof (STRNO),
		int_OBJENT
	);

	return (0);

}	/* end mkcap_hashtb */
