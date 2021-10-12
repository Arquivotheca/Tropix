/*
 ****************************************************************
 *								*
 *			t_rcvraw.c				*
 *								*
 *	Recebe um datagrama RAW 				*
 *								*
 *	Versão	3.0.0, de 28.12.92				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_rcvraw					*
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
 *	Recebe um datagrama RAW 				*
 ****************************************************************
 */
int
t_rcvraw (int fd, T_UNITDATA *unitdata, int *flags)
{
#ifdef	PC
	return (itnet (fd, I_RCVRAW, unitdata, flags));
#else
	return (ioctl (fd, I_RCVRAW, &unitdata));
#endif	PC

}	/* end t_rcvraw */
