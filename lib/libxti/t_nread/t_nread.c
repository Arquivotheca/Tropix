/*
 ****************************************************************
 *								*
 *			t_nread.c				*
 *								*
 *	Obtém o número de caracteres da fila de entrada		*
 *								*
 *	Versão	2.3.0, de 17.11.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_nread						*
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
 *	Obtém o número de caracteres da fila de entrada		*
 ****************************************************************
 */
int
t_nread (int fd)
{
#ifdef	PC
	return (itnet (fd, I_NREAD));
#else
	return (ioctl (fd, I_NREAD));
#endif	PC

}	/* end t_nread */
