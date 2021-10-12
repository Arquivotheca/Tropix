/*
 ****************************************************************
 *								*
 *			t_rcv.c					*
 *								*
 *	Recebe uma mensagem TCP					*
 *								*
 *	Versão	2.3.0, de 16.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcv						*
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
 *	Recebe uma mensagem TCP					*
 ****************************************************************
 */
int
t_rcv (int fd, void *buf, int nbytes, int *flags)
{
#ifdef	PC
	return (itnetrcv (fd, buf, nbytes, flags));
#else
	return (ioctl (fd, I_RCV, &buf));
#endif	PC

}	/* end t_rcv */
