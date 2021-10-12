/*
 ****************************************************************
 *								*
 *			t_rcvrel.c				*
 *								*
 *	Recebe o começo da desconexão TCP			*
 *								*
 *	Versão	2.3.0, de 11.09.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcvrel					*
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
 *	Inicia a desconexão TCP					*
 ****************************************************************
 */
int
t_rcvrel (int fd)
{
#ifdef	PC
	return (itnet (fd, I_RCVREL));
#else
	return (ioctl (fd, I_RCVREL));
#endif	PC

}	/* end t_rcvrel */
