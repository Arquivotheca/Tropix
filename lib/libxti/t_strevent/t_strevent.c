/*
 ****************************************************************
 *								*
 *			t_strevent.c				*
 *								*
 *	Obtem o texto correspondente a um evento		*
 *								*
 *	Versão	3.0.0, de 04.08.92				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_strevent					*
 *		libxti 						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <stdlib.h>
#include <xti.h>

#define	NOSTR	(char *)NULL

/*
 ****************************************************************
 *	Obtem o texto correspondente a um evento		*
 ****************************************************************
 */
char *
t_strevent (int fd, int event)
{
	static struct { int event; char *area; }	e;

	if (e.area == NOSTR && (e.area = malloc (80)) == NOSTR)
		return ("Não consegui obter a cadeia do evento");

#ifdef	PC
	if (itnet (fd, I_GET_EVENT_STR, event, e.area) < 0)
		return ("Não consegui obter a cadeia do evento");
#else
	e.event = event;

	if (ioctl (fd, I_GET_EVENT_STR, &e) < 0)
		return ("Não consegui obter a cadeia do evento");
#endif	PC

	return (e.area);

}	/* end t_strevent */
