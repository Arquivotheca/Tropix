/*
 ****************************************************************
 *								*
 *			t_look.c				*
 *								*
 *	Obtém os eventos ocorridos				*
 *								*
 *	Versão	2.3.0, de 30.09.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_look						*
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
 *	Obtém os eventos ocorridos				*
 ****************************************************************
 */
int
t_look (int fd)
{
#ifdef	PC
	return (itnet (fd, I_LOOK));
#else
	return (ioctl (fd, I_LOOK));
#endif	PC

}	/* end t_look */
