/*
 ****************************************************************
 *								*
 *			t_mail_to_node.c			*
 *								*
 *	Obtém um servidor de correio				*
 *								*
 *	Versão	3.0.0, de 29.11.97				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: t_mail_to_node					*
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
#include <sys/itnet.h>
#include <sys/syscall.h>

#include <stdlib.h>
#include <string.h>
#include <xti.h>

/*
 ****************************************************************
 *	Obtém o endereço de um NÓ a partir do seu nome		*
 ****************************************************************
 */
char *
t_mail_to_node (int fd, const char *domain, int preference, int *new_preference)
{
	static DNS	*dp;

	if (dp == NODNS && (dp = malloc (sizeof (DNS))) == NODNS)
		return (NOSTR);

	strncpy (dp->d_host_nm, domain, NETNM_SZ);
	dp->d_preference = preference;

	if (itnet (fd, I_MAIL_TO_NODE, dp) < 0)
		return (NOSTR);

	if (new_preference != (int *)NULL)
		*new_preference = dp->d_preference;

	return (dp->d_host_nm);

}	/* end t_mail_to_node */
