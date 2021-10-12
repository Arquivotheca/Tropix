/*
 ****************************************************************
 *								*
 *			t_sndudata.c				*
 *								*
 *	Envia um datagrama do usuário (UDP)			*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_sndudata					*
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
 *	Envia um datagrama do usuário (UDP)			*
 ****************************************************************
 */
int
t_sndudata (int fd, const T_UNITDATA *unitdata)
{
#ifdef	PC
	return (itnet (fd, I_SNDUDATA, unitdata));
#else
	return (ioctl (fd, I_SNDUDATA, unitdata));
#endif	PC

}	/* end t_sndudata */
