/*
 ****************************************************************
 *								*
 *			t_push.c				*
 *								*
 *   Envia fisicamente todos os segmentos, mesmo incompletos	*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_push						*
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

#include <xti.h>

/*
 ****************************************************************
 *   Envia fisicamente todos os segmentos, mesmo incompletos	*
 ****************************************************************
 */
int
t_push (int fd)
{
#ifdef	PC
	return (itnet (fd, I_PUSH));
#else
	return (ioctl (fd, I_PUSH));
#endif	PC

}	/* end t_push */
