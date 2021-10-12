/*
 ****************************************************************
 *								*
 *			t_node_to_addr.c			*
 *								*
 *	Obtém o endereço de um NÓ a partir do seu nome		*
 *								*
 *	Versão	2.3.0, de 10.10.91				*
 *		3.0.0, de 03.12.95				*
 *								*
 *	Módulo: t_node_to_addr					*
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
#include <sys/syscall.h>
#include <sys/itnet.h>

#include <string.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Obtém o endereço de um NÓ a partir do seu nome		*
 ****************************************************************
 */
IPADDR
t_node_to_addr (int fd, register const char *id, IPADDR *my_addr)
{
	struct 	{ char id[NETNM_SZ]; IPADDR node_addr; IPADDR my_addr; } s;

	/*
	 *	Procura o "nome" ou endereço dado
	 */
	s.id[0]	    = 0;
	s.node_addr = 0;
	s.my_addr   = 0;

	if   (id == NOSTR)			/* Não foi dado => Nó local */
	{
		/* vazio */
	}
	elif (id[0] >= '0' && id[0] <= '9')	/* Foi dado dd.dd.dd.dd */
	{
		if ((s.node_addr = t_str_to_addr (id)) == -1)
			{ errno = TBADADDR; return (-1); }
	}
	else					/* Foi dado "alpha" */
	{
		strncpy (s.id, id, sizeof (s.id));
	}

#ifdef	PC
	if (itnet (fd, I_NODE_TO_ADDR, &s) < 0)
		return (-1);
#else
	if (ioctl (fd, I_NODE_TO_ADDR, &s) < 0)
		return (-1);
#endif	PC

	/*
	 *	Processa "my_addr"
	 */
	if (my_addr != (IPADDR *)NULL)
		*my_addr = s.my_addr;

	return (s.node_addr);

}	/* end t_node_to_addr */
