/*
 ****************************************************************
 *								*
 *			t_optmgmt.c				*
 *								*
 *	Gerencia opções de transporte 				*
 *								*
 *	Versão	2.3.0, de 30.09.91				*
 *		3.0.0, de 10.05.95				*
 *								*
 *	Módulo: t_optmgmt					*
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
 *	Gerencia opções de transporte 				*
 ****************************************************************
 */
int
t_optmgmt (int fd, const T_OPTMGMT *req, T_OPTMGMT *ret)
{
#ifdef	PC
	return (itnet (fd, I_OPTMGMT, req, ret));
#else
	return (ioctl (fd, I_OPTMGMT, &req));
#endif	PC

}	/* end t_optmgmt */
