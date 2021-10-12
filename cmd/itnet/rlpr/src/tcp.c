/*
 ****************************************************************
 *								*
 *			tcp.c					*
 *								*
 *	Pequena Interface com o Transporte TCP			*
 *								*
 *	Versão	4.2.0, de 23.05.02				*
 *								*
 *	Módulo: rlpr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2002 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <xti.h>
#include <errno.h>

#include "../h/tcp.h"

/*
 ****************************************************************
 *	Macros							*
 ****************************************************************
 */
#define	LOCAL_IP_ADDR		((127 << 24) | 1)
#define MAX_LISTEN_QLEN		8

/*
 *	Macros para preencher as estruturas NETBUF e INADDR.
 */
#define	FILL_NETBUF(nb, _buf, _len)		\
		{				\
			nb.buf    = _buf;	\
			nb.len    = _len;	\
			nb.maxlen = _len;	\
		}

#define	FILL_INADDR(in, addr, port)		\
		{				\
			in.a_addr = addr;	\
			in.a_port = port;	\
		}

/*
 ****************************************************************
 *	Dispositivos associados aos protocolos TCP e UDP	*
 ****************************************************************
 */
static	const char	tcpdevname[]	=	"/dev/itntcp";
static	int		listen_port;
static	int		set_param (int);

#ifdef	SERVER
/*
 ****************************************************************
 *	Abre a conexão (parte do servidor)			*
 ****************************************************************
 */
int
tcp_open (int port)
{
	int		fd;
	T_BIND		req;
	INADDR		req_addr;

	if ((fd = t_open (tcpdevname, O_RDWR, (T_INFO *)NULL)) < 0)
		return (-1);

	/*
	 *	Preenche a estrutura T_BIND
	 */
	FILL_INADDR (req_addr, 0, port);
	FILL_NETBUF (req.addr, &req_addr, sizeof (req_addr));
	req.qlen = MAX_LISTEN_QLEN;

	if (t_bind (fd, &req, (T_BIND *)NULL) < 0)
		goto bad;

	if (set_param (fd) < 0)
		goto bad;

	listen_port = port;

	return (fd);

bad:	t_close (fd);
	return (-1);

}	/* end tcp_open */

/*
 ****************************************************************
 *	Aguarda pedidos de conexão (servidor)			*
 ****************************************************************
 */
int
tcp_listen (int fd, char *name)
{
	T_CALL		call;
	T_BIND		bind;
	INADDR		client_addr, addr[2];
	int		newfd;
	char		*cp;

	/*
	 *	Preenche a estrutura T_CALL: só o membro addr é relevante.
	 */
	FILL_NETBUF (call.addr,  &client_addr, sizeof (client_addr));
	FILL_NETBUF (call.opt,   NULL,         0);
	FILL_NETBUF (call.udata, NULL,         0);

	/*
	 *	Espera chegar um pedido de conexão.
	 */
	if (t_listen (fd, &call) < 0)
		return (-1);

	/*
	 *	Abre outro "endpoint".
	 */
	if ((newfd = t_open (tcpdevname, O_RDWR, (T_INFO *)NULL)) < 0)
		return (-1);

	/*
	 *	Preenche a estrutura T_BIND.
	 */
	if (client_addr.a_addr == LOCAL_IP_ADDR)
	{	/*
		 *	A conexão é local: deixa o kernel gerar outro port.
		 */
		bind.addr.len    = 0;
		bind.addr.buf    = addr;
		bind.addr.maxlen = sizeof (INADDR);
	}
	else
	{	/*
		 *	A conexão é remota: respeita o port do "t_listen".
		 */
		FILL_INADDR (addr[0], 0, listen_port);
		FILL_INADDR (addr[1], client_addr.a_addr, client_addr.a_port);
		FILL_NETBUF (bind.addr, addr, sizeof (addr));
	}

	bind.qlen = 0;		/* Não vamos dar "t_listen" nesta conexão */

	if (t_bind (newfd, &bind, (T_BIND *)NULL) < 0)
		goto bad;

	if (set_param (newfd) < 0)
		goto bad;

	/*
	 *	Aceita a conexão no novo "endpoint".
	 */
	if (t_accept (fd, newfd, &call) < 0)
		goto bad;

	if
	(	name != NOSTR &&
		(cp = t_addr_to_node (fd, client_addr.a_addr)) != NOSTR
	)
		strcpy (name, cp);

	return (newfd);

bad:	t_close (newfd);
	return (-1);

}	/* end tcp_listen */
#endif	SERVER

#ifdef	CLIENT
/*
 ****************************************************************
 *	Inicia um cliente					*
 ****************************************************************
 */
int
tcp_connect (int port, const char *name)
{
	int		fd;
	T_BIND		bind;
	T_CALL		call;
	INADDR		bind_addr, addr;
	IPADDR		remote_ip_addr;

	if ((fd = t_open (tcpdevname, O_RDWR, (T_INFO *)NULL)) < 0)
		return (-1);

	/*
	 *	Associa um endereço local.
	 */
	FILL_INADDR (bind_addr, 0, 0);
	FILL_NETBUF (bind.addr, &bind_addr, sizeof (bind_addr));
	bind.qlen = 0;		/* Não vamos dar "t_listen" nesta conexão */

	if (t_bind (fd, &bind, (T_BIND *)NULL) < 0)
		goto bad;

	if (set_param (fd) < 0)
		goto bad;

	/*
	 *	Converte o nome da estação remota em um endereço IP.
	 */
	if (name != NOSTR && name[0] != '\0')
	{
		remote_ip_addr = t_node_to_addr (fd, name, NULL);
		if (remote_ip_addr == -1)
			goto bad;
	}
	else
	{
		remote_ip_addr = LOCAL_IP_ADDR;
	}

	/*
	 *	Preenche a estrutura T_CALL: só o membro addr é relevante.
	 */
	FILL_INADDR (addr, remote_ip_addr, port);

	FILL_NETBUF (call.addr,  &addr, sizeof (addr));
	FILL_NETBUF (call.opt,   NULL,       0);
	FILL_NETBUF (call.udata, NULL,       0);

	/*
	 *	Tenta estabeler a conexão com a estação remota.
	 */
	if (t_connect (fd, &call, (T_CALL *)NULL) < 0)
		goto bad;

	return (fd);

bad:	t_close (fd);
	return (-1);

}	/* end tcp_connect */
#endif	CLIENT

/*
 ****************************************************************
 *	Obtém o número de bytes disponíveis para leitura	*
 ****************************************************************
 */
int
tcp_nread (int fd, int *pend)
{
	int	nbytes;

	nbytes = t_nread (fd);

	if (nbytes < 0)
	{	/*
		 *	Examina se foi desconexão.
		 */
		if (errno == TLOOK && t_look (fd) == T_ORDREL)
			errno = EPIPE, *pend = 0;
		else
			*pend = -1;

		return (-1);
	}

	*pend = nbytes;
	return (0);

}	/* end tcp_nread */

/*
 ****************************************************************
 *	Leitura da Conexão					*
 ****************************************************************
 */
int
tcp_read (int fd, void *buf, int size)
{
	return (t_rcv (fd, buf, size, NULL));

}	/* end tcp_read */

/*
 ****************************************************************
 *	Escrita na Conexão					*
 ****************************************************************
 */
int
tcp_write (int fd, void *buf, int size)
{
	if (size <= 0)
	{
		t_push (fd);
		return (0);
	}

	return (t_snd (fd, buf, size, T_PUSH));

}	/* end tcp_write */

/*
 ****************************************************************
 *	Encerra a conexão					*
 ****************************************************************
 */
int
tcp_close (int fd)
{
	if (t_getstate (fd) == T_DATAXFER)
	{
		if (t_sndrel (fd) < 0)
			return (-1);

		if (t_rcvrel (fd) < 0)
			return (-1);
	}

	return (t_close (fd));

}	/* end tcp_close */

/*
 ****************************************************************
 *	Aborta a conexão					*
 ****************************************************************
 */
int
tcp_abort (int fd)
{
	if (t_snddis (fd, (T_CALL *)NULL) < 0)
		return (-1);

	return (t_close (fd));

}	/* end tcp_abort */

/*
 ****************************************************************
 *	Modifica algumas características da conexão		*
 ****************************************************************
 */
static int
set_param (int fd)
{
	T_OPTMGMT	req_optmgmt, ret_optmgmt;
	TCP_OPTIONS	options;

	/*
	 *	Obtém os valores em USO
	 */
	memset (&options, 0, sizeof (TCP_OPTIONS));

	req_optmgmt.flags = T_CHECK;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
		return (-1);

	/*
	 *	Negocia o "max_wait" e "max_silence"
	 */
	req_optmgmt.flags = T_NEGOTIATE;

	options.max_wait     = 0;	/* Sem limite */
	options.max_silence  = 0;	/* Sem limite */

/***	req_optmgmt.opt.buf    = &options;		***/
/***	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);	***/

/***	ret_optmgmt.opt.buf    = &options;		***/
/***	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);	***/

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
		return (-1);

	return (0);

}	/* end set_param */
