/*
 ****************************************************************
 *								*
 *			t_sndraw.c				*
 *								*
 *	Envia um datagrama RAW 					*
 *								*
 *	Versão	3.0.0, de 28.12.92				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_sndraw					*
 *		libxti 						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <xti.h>

/*
 ****************************************************************
 *	Envia um datagrama RAW 					*
 ****************************************************************
 */
int
t_sndraw (int fd, const T_UNITDATA *unitdata)
{
#ifdef	PC
	return (itnet (fd, I_SNDRAW, unitdata));
#else
	return (ioctl (fd, I_SNDRAW, unitdata));
#endif	PC

}	/* end t_sndraw */
