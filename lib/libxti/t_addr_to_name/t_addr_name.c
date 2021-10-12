/*
 ****************************************************************
 *								*
 *			t_addr_to_name.c			*
 *								*
 *	Obtém o nome de um NÓ a partir do seu endereço		*
 *								*
 *	Versão	2.3.0, de 10.10.91				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: t_addr_to_name					*
 *		libxti 						*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/itnet.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *  Obtém o nome de um NÓ (com busca) a partir do seu endereço	*
 ****************************************************************
 */
char *
t_addr_to_name (int fd, long addr)
{
	static char	*idp;

	if (idp == NOSTR && (idp = malloc (NETNM_SZ)) == NOSTR)
		return (t_addr_to_str (addr));

	if (itnet (fd, I_ADDR_TO_NAME, addr, idp) >= 0)
		return (idp);
	else
		return (t_addr_to_str (addr));

}	/* end  t_addr_to_name */
