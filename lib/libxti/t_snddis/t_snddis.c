/*
 ****************************************************************
 *								*
 *			t_snddis.c				*
 *								*
 *	Inicia a desconexão abortiva TCP			*
 *								*
 *	Versão	2.3.0, de 30.09.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_snddis					*
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
 *	Inicia a desconexão abortiva TCP			*
 ****************************************************************
 */
int
t_snddis (int fd, const T_CALL *call)
{
#ifdef	PC
	return (itnet (fd, I_SNDDIS, call));
#else
	return (ioctl (fd, I_SNDDIS, call));
#endif	PC

}	/* end t_snddis */
