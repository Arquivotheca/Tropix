/*
 ****************************************************************
 *								*
 *			xtrans.c				*
 *								*
 *	Interface com a XTI					*
 *								*
 *	Versão	1.0.0, de 31.07.97				*
 *	Versão	1.0.2, de 26.07.98				*
 *								*
 *	Módulo: xc/lib/xtrans					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1995 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <id.h>
#include <xti.h>
#include <errno.h>

#include "Xtrans.h"

/*
 ****************************************************************
 *	Macros							*
 ****************************************************************
 */
#define	NETNM_SZ		80
#define X_TCP_PORT		6000
#define	LOCAL_IP_ADDR		((127 << 24) | 1)
#define MAX_LISTEN_QLEN		8

#ifdef X11_t

/*
 *	These values come from X.h and Xauth.h, and MUST match them. Some
 *	of these values are also defined by the ChangeHost protocol message.
 */
#define FamilyInternet		0
#define FamilyDECnet		1
#define FamilyChaos		2
#define FamilyAmoeba		33
#define FamilyLocalHost		252
#define FamilyKrb5Principal	253
#define FamilyNetname		254
#define FamilyLocal		256
#define FamilyWild		65535

#endif

#ifdef XSERV_t
#define xalloc(_size)		Xalloc(_size)
#define xcalloc(_num,_size)	Xcalloc((_num) * (_size))
#define xrealloc(_ptr,_size)	Xrealloc(_ptr,_size)
#define xfree(_ptr)		Xfree(_ptr)
#define	xtrans_error		Log
#else
#define xalloc(_size)		malloc(_size)
#define xcalloc(_num,_size)	calloc(_num,_size)
#define xrealloc(_ptr,_size)	realloc(_ptr,_size)
#define xfree(_ptr)		free(_ptr)
#endif

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
 *	Estrutura de uma conexão				*
 ****************************************************************
 */
#define	LOCAL_ADDR	0	/* cip->addr[LOCAL_ADDR] */
#define	REMOTE_ADDR	1	/* cip->addr[REMOTE_ADDR] */

#define	NAMESZ		32

struct _XtransConnInfo
{
#if defined (XSERV_t) || defined (TRANS_SERVER)
	char		client_id[NAMESZ];	/* Identificação do Cliente */
#endif
	int		fd,			/* "fd" associado */
			listen_port;		/* "port" associado */

	INADDR		addr[2];		/* Endereços local e remoto */
};

/*
 ****************************************************************
 *	Dispositivos associados aos protocolos TCP e UDP	*
 ****************************************************************
 */
static const char	tcpdevname[]	=	"/dev/itntcp";
static const char	udpdevname[]	=	"/dev/itnudp";

/*
 ****************************************************************
 *	Nome do programa (externo)				*
 ****************************************************************
 */
extern const char	*pgname;

#ifndef	XSERV_t
/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
static void
xtrans_error (char *msg, ...)
{
	va_list		arg;
	static int	pid = 0;

	if (pid == 0)
		pid = getpid ();

	va_start (arg, msg);
	fprintf (stderr, "[%s] %s(%d): ", fulltime (), pgname, pid);
	vfprintf (stderr, msg, arg);
	va_end (arg);

	fflush (stderr);

}	/* end xtrans_error */
#endif

/*
 ****************************************************************
 *	Obtém um "endpoint"					*
 ****************************************************************
 */
static XtransConnInfo
do_open (const char *device, int mode)
{
	XtransConnInfo	cip;

	/*
	 *	Aloca a estrutura _XtransConnInfo.
	 */
	cip = (XtransConnInfo)xcalloc (1, sizeof (struct _XtransConnInfo));
	if (cip == NULL)
	{
		xtrans_error ("do_open: erro de alocação\n");
		return (NULL);
	}

	/*
	 *	Dá o "t_open" no dispositivo.
	 */
	if ((cip->fd = t_open (device, mode, (T_INFO *)NULL)) < 0)
	{
		xtrans_error
		(	"do_open: erro no \"t_open\" (%s)\n",
			strerror (errno)
		);

		xfree (cip);
		return (NULL);
	}

	return (cip);

}	/* end do_open */

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
	{
		xtrans_error
		(	"set_param: não consegui obter parâmetros (%s)\n",
			strerror (errno)
		);

		return (-1);
	}

#if (0)	/****************************************************/
	xtrans_error
	(	"fd = %d: max_wait = %d, max_silence = %d, max_seg_size = %d\n",
		fd, ret_options.max_wait, ret_options.max_silence,
		ret_options.max_seg_size
	);
#endif	/****************************************************/

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
	{
		xtrans_error
		(	"set_param: não consegui negociar parâmetros (%s)\n",
			strerror (errno)
		);

		return (-1);
	}

#if (0)	/****************************************************/
	xtrans_error
	(	"fd = %d: max_wait = %d, max_silence = %d, max_seg_size = %d\n",
		fd, ret_options.max_wait, ret_options.max_silence,
		ret_options.max_seg_size
	);
#endif	/****************************************************/

	return (0);

}	/* end set_param */

#ifdef TRANS_CLIENT
/*
 ****************************************************************
 *	Inicia um cliente					*
 ****************************************************************
 */
static XtransConnInfo
init_client (const char *device)
{
	XtransConnInfo	cip;
	T_BIND		bind;
	INADDR		bind_addr;

	if ((cip = do_open (device, O_RDWR)) == NULL)
		return (NULL);

	/*
	 *	Associa um endereço local.
	 */
	FILL_INADDR (bind_addr, 0, 0);
	FILL_NETBUF (bind.addr, &bind_addr, sizeof (bind_addr));
	bind.qlen = 0;		/* Não vamos dar "t_listen" nesta conexão */

	if (t_bind (cip->fd, &bind, (T_BIND *)NULL) < 0)
	{
		xtrans_error ("init_client: erro no \"t_bind\" (%s)\n", strerror (errno));
		goto bad;
	}

	if (set_param (cip->fd) < 0)
	{
		xtrans_error ("init_client: erro ao negociar parâmetros\n");
		goto bad;
	}

	return (cip);

    bad:
	t_close (cip->fd);
	xfree (cip);
	return (NULL);

}	/* end init_client */
#endif /* TRANS_CLIENT */

#ifdef TRANS_REOPEN
/*
 ****************************************************************
 *	Reabre uma conexão (código comum)			*
 ****************************************************************
 */
static XtransConnInfo
do_reopen_server (int fd, int port)
{
	XtransConnInfo	cip;

	if (t_sync (fd) < 0)
	{
		xtrans_error
		(	"do_reopen_server: erro no \"t_sync\" (%s)\n",
			strerror (errno)
		);

		return (NULL);
	}

	cip = (XtransConnInfo)xcalloc (1, sizeof (struct _XtransConnInfo));
	if (cip == NULL)
	{
		xtrans_error ("do_reopen: erro de alocação\n");
		return (NULL);
	}
    
	cip->fd = fd;

	return (cip);

}	/* end do_reopen_server */
#endif /* TRANS_REOPEN */

/*
 ****************************************************************
 *	Obtém o endereço local ou remoto			*
 ****************************************************************
 */
static int
get_addr (XtransConnInfo cip, int *familyp, int *addrlenp,
			Xtransaddr **addrp, int which)
{
	struct sockaddr_in	iaddr;
	int			family;

	/*
	 *	Aloca e preenche uma estrutura sockaddr_in.
	 */
	*addrp = (Xtransaddr *)xalloc (sizeof (iaddr));
	if (*addrp == NULL)
		goto bad;

	family = AF_INET;

	/*
	 *	Aparentemente, as rotinas que chamam esperam o port
	 *	e o endereço IP no formato BIG ENDIAN.
	 */
	iaddr.sin_family	= family;
	iaddr.sin_port		= short_endian_cv (cip->addr[which].a_port);
	iaddr.sin_addr.s_addr	= long_endian_cv (cip->addr[which].a_addr);

	*familyp = family;
	*addrlenp = sizeof (iaddr);
	memmove (*addrp, &iaddr, sizeof (iaddr));

	return (0);

bad: 	*addrp    = NULL;
	*addrlenp = 0;
	*familyp  = AF_UNSPEC;

	return (-1);

}	/* end get_addr */

/*
 ****************************************************************
 *	Sabe-se lá o que faz					*
 ****************************************************************
 */
int
TRANS(NoListen) (char *str)
{
}	/* end TRANS(NoListen) */

/*
 ****************************************************************
 *	Libera a estrutura _XtransConnInfo			*
 ****************************************************************
 */
void
TRANS(FreeConnInfo) (XtransConnInfo cip)
{
	xfree ((char *)cip);

}	/* end TRANS(FreeConnInfo) */

/*
 ****************************************************************
 *	Disseca um endereço					*
 ****************************************************************
 *
 *	Formato geral do endereço:
 *		[protocolo][/máquina]:display[/catálogo]
 */
int
TRANS(ParseAddress) (char *address, char **protocol, char **host, int *port)
{
	char	*mybuf, *tmpptr;
	char	*_protocol, *_host, *_port, *_catalogue;
	char	hostnamebuf[NETNM_SZ];
	int	len;

	/*
	 *	Mantém a salvo a cadeia dada.
	 */
	tmpptr = mybuf = alloca (len = strlen (address) + 1);
	memmove (mybuf, address, len);

	/*
	 *	Obtém o protocolo.
	 */
	_protocol = mybuf;

	if ((mybuf = strpbrk (mybuf,"/:")) == NOSTR)
		goto bad;

	if (*mybuf == ':')
	{	/*
		 *	O protocolo foi omitido: assume "inet".
		 */
		_protocol = "inet";
		mybuf = tmpptr;
	}
	else
	{	/*
		 *	Achou '/'. Se estiver no início da cadeia,
		 *	o protocolo foi omitido e "inet" é assumido.
		 */
		if (mybuf == tmpptr)
			_protocol = "inet";

		*mybuf++ = '\0';
	}

	/*
	 *	Obtém o nome da máquina.
	 */
	_host = mybuf;

	if ((mybuf = strchr (mybuf, ':')) == NOSTR)
		goto bad;

	if (mybuf == _host)
	{
		/*
		 *	O nome da máquina foi omitido. Obtém o nome local.
		 */
		gethostname (hostnamebuf, sizeof (hostnamebuf) - 1);
		_host = hostnamebuf;
	}

	*mybuf++ = '\0';

	/*
	 *	Guarda o início do port.
	 */
	_port = mybuf;

#if defined(FONT_t) || defined(FS_t)
	/*
	 *	Verifica se o catálogo foi especificado.
	 */
	if ((mybuf = strchr (mybuf,'/')) != NOSTR)
		*mybuf ++= '\0';

	_catalogue = mybuf;
#endif

	/*
	 *	Todos os componentes foram obtidos.
	 *	Prepara os retornos.
	 */
	if ((*port = atoi (_port)) < 0)
		goto bad;

	len = strlen (_protocol) + 1;
	if ((*protocol = (char *)xalloc (len)) == NOSTR)
		goto bad;

	memmove (*protocol, _protocol, len);

	len = strlen (_host) + 1;
	if ((*host = (char *)xalloc (len)) == NOSTR)
	{
		xfree (*protocol);
		goto bad;
	}

	memmove (*host, _host, len);

	return (1);

bad: 	*port     = -1;
	*host     = NOSTR;
	*protocol = NOSTR;

	return (0);

}	/* end TRANS(ParseAddress) */

#ifdef TRANS_CLIENT
/*
 ****************************************************************
 *	Inicia uma conexão no modo-com-conexão (cliente)	*
 ****************************************************************
 */
XtransConnInfo
TRANS(OpenCOTSClient) (char *address)
{
	XtransConnInfo	cip;

	if ((cip = init_client (tcpdevname)) == NULL)
		xtrans_error ("TRANS(OpenCOTSClient): erro na inicialização\n");

	return (cip);

}	/* end TRANS(OpenCOTSClient) */

/*
 ****************************************************************
 *	Rotina de "connect"					*
 ****************************************************************
 */
int
TRANS(Connect) (XtransConnInfo cip, char *address)
{
	T_CALL	call;
	INADDR	addr;
	IPADDR	remote_ip_addr;
	int	fd, port;
	char	*protocol, *host;

	/*
	 *	Analisa o endereço dado.
	 */
	if (!TRANS(ParseAddress) (address, &protocol, &host, &port))
	{
		xtrans_error ("TRANS(Connect): endereço inválido: %s\n", address);
		return (-1);
	}

	fd = cip->fd;

	/*
	 *	Converte o nome da estação remota em um endereço IP.
	 */
	remote_ip_addr = t_node_to_addr (fd, host, NULL);
	if (remote_ip_addr < 0)
	{
		xtrans_error
		(	"TRANS(Connect): erro no \"t_node_to_addr\" (%s)\n",
			strerror (errno)
		);
		goto bad;
	}


	port += X_TCP_PORT;

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
	{
		if (errno == TLOOK && t_look (fd) == T_DISCONNECT)
			xtrans_error ("TRANS(Connect): a conexão foi recusada\n");
		else
			xtrans_error ("TRANS(Connect): não consegui estabelecer conexão\n");
		goto bad;
	}

	/*
	 *	Guarda os endereços efetivamente associados à conexão.
	 */
	if (t_getaddr (fd, cip->addr) < 0)
	{
		xtrans_error ("TRANS(Connect): erro no \"t_getaddr\" (%s)\n", strerror (errno));
		goto bad;
	}

#if (0)	/****************************************************/
	xtrans_error
	(	"TRANS(Connect): endereço local:  (%s, %d)\n",
		t_addr_to_str (cip->addr[LOCAL_ADDR].a_addr),
		cip->addr[LOCAL_ADDR].a_port
	);

	xtrans_error
	(	"TRANS(Connect): endereço remoto: (%s, %d)\n",
		t_addr_to_str (cip->addr[REMOTE_ADDR].a_addr),
		cip->addr[REMOTE_ADDR].a_port
	);
#endif	/****************************************************/

	if (remote_ip_addr == LOCAL_IP_ADDR)
	{
		char	client_id[NAMESZ];

	 	/*
		 *	A conexão é local: envia o nome(pid).
		 */
		sprintf (client_id, "%s(%d)", pgname, getpid ());
		t_snd (cip->fd, client_id, sizeof (client_id), T_PUSH);
	}

	xfree (protocol);
	xfree (host);
	return (0);

    bad:
 	xfree (protocol);
	xfree (host);
	return (-1);

}	/* end TRANS(Connect) */
#endif /* TRANS_CLIENT */

#ifdef TRANS_SERVER
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
XtransConnInfo
TRANS(OpenCOTSServer) (char *address)
{
	XtransConnInfo	cip;

	if ((cip = do_open (tcpdevname, O_RDWR)) == NULL)
		xtrans_error ("TRANS(OpenCOTSServer): erro na inicialização\n");

	return (cip);

}	/* end TRANS(OpenCOTSServer) */

/*
 ****************************************************************
 *	Completa a instalação do servidor			*
 ****************************************************************
 */
int
TRANS(CreateListener) (XtransConnInfo cip, int port)
{
	int		fd;
	T_CALL		call;
	T_BIND		req, ret;
	INADDR		req_addr, ret_addr, nodata;

	/*
	 *	Preenche as duas estruturas T_BIND.
	 */
	FILL_INADDR (req_addr, 0, port);
	FILL_NETBUF (req.addr, &req_addr, sizeof (req_addr));
	req.qlen = MAX_LISTEN_QLEN;

	FILL_INADDR (ret_addr, 0, 0);
	FILL_NETBUF (ret.addr, &ret_addr, sizeof (ret_addr));

	fd = cip->fd;

	if (t_bind (fd, &req, &ret) < 0)
	{
		xtrans_error
		(	"TRANS(CreateListener): erro no \"t_bind\" (%s)\n",
			strerror (errno)
		);

		return (TRANS_CREATE_LISTENER_FAILED);
	}

	/*
	 *	Vê se o port alocado foi o pedido.
	 */
	if (req_addr.a_port != ret_addr.a_port)
	{
		xtrans_error
		(	"TRANS(CreateListener): port solicitado = %d, "
			"port retornado = %d\n",
			req_addr.a_port,
			ret_addr.a_port
		);

		return (TRANS_CREATE_LISTENER_FAILED);
	}

	if (set_param (fd) < 0)
		return (TRANS_CREATE_LISTENER_FAILED);

	cip->listen_port = port;

	return (0);

}	/* end TRANS(CreateListener) */

/*
 ****************************************************************
 *	Reinicializa o servidor					*
 ****************************************************************
 */
int
TRANS(ResetListener) (XtransConnInfo cip)
{
	return (TRANS_RESET_NOOP);

}	/* end TRANS(ResetListener) */

/*
 ****************************************************************
 *	Espera um pedido e estabelece a conexão			*
 ****************************************************************
 */
XtransConnInfo
TRANS(Accept) (XtransConnInfo cip, int *status)
{
	XtransConnInfo	newcip;
	T_CALL		call;
	T_BIND		bind;
	INADDR		client_addr, addr[2];
	int		local_conn, fd;

	/*
	 *	Preenche a estrutura T_CALL: só o membro addr é relevante.
	 */
	FILL_NETBUF (call.addr,  &client_addr, sizeof (client_addr));
	FILL_NETBUF (call.opt,   NULL,         0);
	FILL_NETBUF (call.udata, NULL,         0);

	/*
	 *	Espera chegar um pedido de conexão.
	 */
	if (t_listen (cip->fd, &call) < 0)
	{
		error
		(	"TRANS(Accept): erro no \"t_listen\" (%s)\n",
			strerror (errno)
		);

		*status = TRANS_ACCEPT_MISC_ERROR;
		return (NULL);
	}

	fprintf (stderr, "\n=============================================================\n");

	xtrans_error
	(	"Pedido de conexão: (%s [%s], %d)\n",
		t_addr_to_str (client_addr.a_addr),
		t_addr_to_node (cip->fd, client_addr.a_addr),
		client_addr.a_port
	);

	/*
	 *	Abre outro "endpoint".
	 */
	if ((newcip = do_open (tcpdevname, O_RDWR)) == NULL)
	{
		xtrans_error ("TRANS(Accept): erro ao abrir outra conexão\n");
		*status = TRANS_ACCEPT_MISC_ERROR;
		return (NULL);
	}

	fd = newcip->fd;

	local_conn = client_addr.a_addr == LOCAL_IP_ADDR;

	/*
	 *	Preenche a estrutura T_BIND.
	 */
	if (local_conn)
	{	/*
		 *	A conexão é local: deixa o kernel gerar outro port.
		 */
		bind.addr.len    = 0;
		bind.addr.buf    = addr;
		bind.addr.maxlen = sizeof (INADDR);
	}
	else
	{	/*
		 *	A conexão é remota: mantém o mesmo port do listen.
		 */
		FILL_INADDR (addr[0], 0, cip->listen_port);
		FILL_INADDR (addr[1], client_addr.a_addr, client_addr.a_port);
		FILL_NETBUF (bind.addr, addr, sizeof (addr));
	}

	bind.qlen = 0;		/* Não vamos dar "t_listen" nesta conexão */

	if (t_bind (fd, &bind, (T_BIND *)NULL) < 0)
	{
		xtrans_error
		(	"TRANS(Accept): erro no \"t_bind\" (%s)\n",
			strerror (errno)
		);

		*status = TRANS_ACCEPT_MISC_ERROR;
		goto bad;
	}

	if (set_param (fd) < 0)
	{
		xtrans_error ("TRANS(Accept): erro ao negociar parâmetros\n");
		goto bad;
	}

	/*
	 *	Aceita a conexão no novo "endpoint".
	 */
	if (t_accept (cip->fd, fd, &call) < 0)
	{
		xtrans_error
		(	"TRANS(Accept): erro no \"t_accept\" (%s)\n",
			strerror (errno)
		);

		if (errno == TLOOK)
		{
			xtrans_error
			(	"TRANS(Accept): EVENTO = %s\n",
				t_strevent (fd, t_look (fd))
			);
		}

		*status = TRANS_ACCEPT_FAILED;
		goto bad;
	}

	if (t_getstate (fd) != T_DATAXFER)
	{
		xtrans_error
		(	"TRANS(Accept): o estado da conexão do fd = %d é %s\n",
			fd, statetostr (t_getstate (fd))
		);
	}

	/*
	 *	Guarda os endereços efetivamente associados à conexão.
	 */
	if (t_getaddr (fd, newcip->addr) < 0)
	{
		xtrans_error ("TRANS(Accept): erro no \"t_getaddr\" (%s)", strerror (errno));
		*status = TRANS_ACCEPT_FAILED;
		goto bad;
	}

	xtrans_error
	(	"Local = (%s, %d), "
		"Remoto = (%s, %d)\n",
		t_addr_to_str (newcip->addr[LOCAL_ADDR].a_addr),
		newcip->addr[LOCAL_ADDR].a_port,
		t_addr_to_str (newcip->addr[REMOTE_ADDR].a_addr),
		newcip->addr[REMOTE_ADDR].a_port
	);

	*status = 0;

	if (local_conn)
	{
		if (t_rcv (fd, newcip->client_id, NAMESZ, NULL) != NAMESZ)
		{
			xtrans_error ("TRANS(Accept): erro ao ler o nome do cliente\n");
			*status = TRANS_ACCEPT_FAILED;
			goto bad;
		}
	}
	else
	{
		strcpy (newcip->client_id, "cliente remoto");
	}

	return (newcip);

    bad:
	t_close (fd);
	xfree (newcip);
	return (NULL);

}	/* end TRANS(Accept) */
#endif /* TRANS_SERVER */

#ifdef TRANS_REOPEN
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
XtransConnInfo
TRANS(ReopenCOTSServer) (int trans_id, int fd, int port)
{
	XtransConnInfo	cip = NULL;
	int		i, save_port;

	save_port = port;

	if ((cip = do_reopen_server (fd, port)) == NULL)
	{
		xtrans_error ("TRANS(ReopenCOTSServer): erro ao reabrir (%d,%d)\n", fd, port);
		return (NULL);
	}

	cip->listen_port = save_port;

	return (cip);

}	/* end TRANS(ReopenCOTSServer) */

/*
 ****************************************************************
 *	Recupera informações sobre a conexão			*
 ****************************************************************
 */
int
TRANS(GetReopenInfo) (XtransConnInfo cip, int *trans_id, int *fd, int *port)
{
	*trans_id = 0;	/* TRANS_XTI_ID; */
	*fd       = cip->fd;
	*port     = cip->listen_port;

	return (0);
}
#endif /* TRANS_REOPEN */

/*
 ****************************************************************
 *	Modifica opções da conexão				*
 ****************************************************************
 */
int
TRANS(SetOption) (XtransConnInfo cip, int option, int arg)
{
	int	ret = 0;

	switch (option)
	{
	    case TRANS_NONBLOCKING:
		ret = fcntl (cip->fd, F_SETFL, arg ? O_NDELAY : 0);
		break;

	    case TRANS_CLOSEONEXEC:
		ret = fcntl (cip->fd, F_SETFD, 1);
		break;
	}

	return (ret);

}	/* end TRANS(SetOption) */

/*
 ****************************************************************
 *	Obtém o número de bytes disponíveis para leitura	*
 ****************************************************************
 */
int
TRANS(BytesReadable) (XtransConnInfo cip, BytesReadable_t *pend)
{
	int	nbytes;

	nbytes = t_nread (cip->fd);

	if (nbytes < 0)
	{	/*
		 *	Examina se foi desconexão.
		 */
		if (errno == TLOOK && t_look (cip->fd) == T_ORDREL)
			errno = EPIPE, *pend = 0;
		else
			*pend = -1;

		return (-1);
	}

	*pend = nbytes;
	return (0);

}	/* end TRANS(BytesReadable) */

/*
 ****************************************************************
 *	Leitura da Conexão					*
 ****************************************************************
 */
#if (0) /* def	XSERV_t */
static int	timeout;

void
handle_timeout (int sig, ...)
{
	xtrans_error ("Recebi o sinal %s\n", sigtostr (sig));
	timeout++;
}
#endif

int
TRANS(Read) (XtransConnInfo cip, char *buf, int size)
{
	int		fd, ret, avail;

	fd = cip->fd;

	if (size <= 0)
	{
		xtrans_error
		(	"TRANS(Read): leitura de %d bytes do fd = %d !!!\n",
			size, fd
		);
	}

#ifdef	XSERV_t
	if ((avail = t_nread (fd)) < 0)
	{
		if (errno == TLOOK)
		{	/*
			 *	Desconexão percebida.
			 */
			if ((ret = t_look (fd)) == T_ORDREL)
				return (0);

			xtrans_error
			(	"TRANS(Read): t_nread < 0, erro = %s\n",
				t_strevent (fd, ret)
			);
		}
		else
		{
			xtrans_error
			(	"TRANS(Read): t_nread < 0, erro = %s\n",
				strerror (errno)
			);
		}

		return (-1);
	}

	if (avail == 0)
	{
		xtrans_error
		(	"TRANS(Read): leitura de %d bytes do fd = %d iria prender\n",
			size, fd
		);

		return (-1);
	}

#if (0)	/****************************************************/
	signal (SIGALRM, handle_timeout);
	timeout = 0;
	alarm (10);
#endif	/****************************************************/

#endif	XSERV_t

	/*
	 *	Lê os bytes.
	 */
	ret = t_rcv (fd, buf, size, NULL);

#ifdef	XSERV_t
#if (0)	/*******************************************************/
	alarm (0);
	if (ret < 0 && errno == EINTR && timeout)
#endif	/*******************************************************/

	if (ret < 0)
	{
		if (errno == EINTR)
		{
			xtrans_error
			(	"TRANS(Read): t_rcv (%d, %P, %d) foi "
				"interrompida, avail = %d\n",
				fd, buf, size, avail
			);
		}
		else
		{
			xtrans_error
			(	"TRANS(Read): erro \"%s\" em t_rcv (%d, %P, %d), "
				"avail = %d\n",
				strerror (errno),
				fd, buf, size, avail
			);
		}

#if (0)	/****************************************************/
		timeout = 0;
#endif	/****************************************************/
	}
#endif	XSERV_t

	return (ret);

}	/* end TRANS(Read) */

/*
 ****************************************************************
 *	Escrita na Conexão					*
 ****************************************************************
 */
int
TRANS(Write) (XtransConnInfo cip, char *buf, int size)
{
	if (size <= 0)
	{
		xtrans_error ("TRANS(Write): size = %d\n", size);
		t_push (cip->fd);
		return (0);
	}

	return (t_snd (cip->fd, buf, size, T_PUSH));

}	/* end TRANS(Write) */

#ifndef	XSERV_t
/*
 ****************************************************************
 *	Recebe dados com "readv"				*
 ****************************************************************
 */
int
TRANS(Readv) (XtransConnInfo cip, IOVEC *iov, int iovcnt)
{
	int	fd, i, len, total, nbytes;
	void	*base;

	fd = cip->fd;
	for (i = 0, total = 0;  i < iovcnt;  i++, iov++)
	{
		len = iov->iov_len;
		base = iov->iov_base;

		while (len > 0)
		{
			nbytes = t_rcv (fd, base, len, NULL);

			if (nbytes < 0 && total == 0)
				return (-1);

			if (nbytes <= 0)
				return (total);

			len   -= nbytes;
			total += nbytes;
			base  += nbytes;
		}
	}

	return (total);

}	/* end TRANS(Readv) */
#endif

/*
 ****************************************************************
 *	Envia dados com "writev"				*
 ****************************************************************
 */
int
TRANS(Writev) (XtransConnInfo cip, IOVEC *iov, int iovcnt)
{
	int	fd, i, len, total, nbytes;
	void	*base;

	fd = cip->fd;

	for (i = 0, total = 0;  i < iovcnt;  i++, iov++)
	{
		len = iov->iov_len;
		base = iov->iov_base;

		while (len > 0)
		{
			nbytes = t_snd (fd, base, len, 0);

			if (nbytes < 0 && total == 0)
				return (-1);

			if (nbytes <= 0)
				return (total);

			len   -= nbytes;
			total += nbytes;
			base  += nbytes;
		}
	}

	t_push (fd);

	return (total);

}	/* end TRANS(Writev) */

#if (0)	/****************************************************/
/*
 ****************************************************************
 *	Descarta todos os dados pendentes na conexão		*
 ****************************************************************
 */
void
discard_all_data (int fd)
{
	int	n, total;
	void	*buf;

	total = 0;
	while ((n = t_nread (fd)) > 0)
	{
		if ((buf = malloc (n)) != NOVOID)
		{
			if ((n = t_rcv (fd, buf, n, NULL)) <= 0)
			{
				xtrans_error
				(	"erro no \"t_rcv\" descartando dados: %s\n",
					strerror (errno)
				);
			}
			else
			{
				total += n;
			}

			free (buf);
		}
		else
		{
			xtrans_error ("erro no \"malloc\" para descartar dados\n");
		}
	}

	if (total > 0)
		xtrans_error ("%d bytes descartados\n", total);

}	/* end discard_all_data */
#endif	/****************************************************/

/*
 ****************************************************************
 *	Encerra a conexão					*
 ****************************************************************
 */
int
TRANS(Disconnect) (XtransConnInfo cip)
{
	int	fd, state;

	fd = cip->fd;
/***	discard_all_data (fd);	***/

#ifdef	XSERV_t
	xtrans_error ("vou chamar \"t_sndrel\" para o fd = %d\n", fd);
#endif

	if ((state = t_getstate (fd)) != T_DATAXFER)
	{
		if (state < 0)
			return (-1);

		xtrans_error ("ATENÇÃO: estado = %s (%d)!\n", statetostr (state), state);
	}

	if (t_sndrel (fd) < 0)
	{
		xtrans_error
		(	"erro no \"t_sndrel\": %s, "
			"estado = %s\n",
			strerror (errno),
			statetostr (t_getstate (fd))
		);

		if (errno == TLOOK)
		{
			xtrans_error
			(	"t_look = %s\n",
				t_strevent (fd, t_look (fd))
			);
		}

		return (-1);
	}

#ifdef	XSERV_t
	xtrans_error ("vou chamar \"t_rcvrel\" para o fd = %d\n", fd);
#endif

	if (t_rcvrel (fd) < 0)
	{
		xtrans_error
		(	"erro no \"t_rcvrel\": %s, "
			"estado = %s\n",
			strerror (errno),
			statetostr (t_getstate (fd))
		);

		if (errno == TLOOK)
		{
			xtrans_error
			(	"t_look = %s\n",
				t_strevent (fd, t_look (fd))
			);
		}

		return (-1);
	}

#ifdef	XSERV_t
	xtrans_error
	(	"desconexão OK para o fd = %d, estado = %s\n",
		fd, statetostr (t_getstate (fd))
	);
#endif

	return (0);

}	/* end TRANS(Disconnect) */

/*
 ****************************************************************
 *	Fecha o dispositivo					*
 ****************************************************************
 */
int
TRANS(Close) (XtransConnInfo cip)
{
	int	ret;

	if (t_getstate (cip->fd) == T_DATAXFER)
	{
		if (TRANS(Disconnect) (cip) < 0)
			return (-1);
	}

/***	t_unbind (cip->fd);	***/
	ret = t_close (cip->fd);
	TRANS(FreeConnInfo) (cip);

	return (ret);

}	/* end TRANS(Close) */

#if (0)	/****************************************************/
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
int
TRANS(CloseForCloning) (XtransConnInfo cip)
{
	int	ret;

	/*
	 *	Não chama "t_unbind".
	 */
	ret = t_close (cip->fd);
	TRANS(FreeConnInfo) (cip);
	return (ret);

}	/* end TRANS(CloseForCloning) */
#endif	/****************************************************/

/*
 ****************************************************************
 *	Verifica se a conexão é local				*
 ****************************************************************
 */
int
TRANS(IsLocal) (XtransConnInfo cip)
{
	return (0);

}	/* end TRANS(IsLocal) */

/*
 ****************************************************************
 *	Obtém o endereço local					*
 ****************************************************************
 */
int
TRANS(GetMyAddr) (XtransConnInfo cip, int *familyp, int *addrlenp,
			Xtransaddr **addrp)
{
	return (get_addr (cip, familyp, addrlenp, addrp, LOCAL_ADDR));

}	/* end TRANS(GetMyAddr) */

/*
 ****************************************************************
 *	Obtém o endereço remoto					*
 ****************************************************************
 */
int
TRANS(GetPeerAddr) (XtransConnInfo cip, int *familyp, int *addrlenp,
			Xtransaddr **addrp)
{
	return (get_addr (cip, familyp, addrlenp, addrp, REMOTE_ADDR));

}	/* end TRANS(GetPeerAddr) */

/*
 ****************************************************************
 *	Obtém o "fd" associado à conexão			*
 ****************************************************************
 */
int
TRANS(GetConnectionNumber) (XtransConnInfo cip)
{
	return (cip->fd);

}	/* end TRANS(GetConnectionNumber) */

#ifdef TRANS_SERVER
/*
 ****************************************************************
 *	x
 ****************************************************************
 */
int
TRANS(MakeAllCOTSServerListeners) (char *port, int *partial, int *count_ret,
					XtransConnInfo **cips_ret)
{
	char		address[256];
	XtransConnInfo	cip;
	int		port_number;

	/*
	 *	Verifica se o port dado é numérico.
	 */
	port_number = 0;
	if (port != NOSTR && (port_number = atoi (port)) < 0)
	{
		xtrans_error ("TRANS(MakeAllCOTSServerListeners): port = %s\n", port);
		goto bad_return;
	}

	port_number += X_TCP_PORT;

	sprintf	(address, "inet/:%d", port_number);

	if ((cip = TRANS(OpenCOTSServer) (address)) == NULL)
	{
		xtrans_error
		(	"TRANS(MakeAllCOTSServerListeners): "
			"falha ao alocar um servidor\n"
		);

		goto bad_return;
	}

	if (TRANS(CreateListener) (cip, port_number) < 0)
	{
		xtrans_error
		(	"TRANS(MakeAllCOTSServerListeners): "
			"erro ao alocar o servidor ouvindo a porta %d\n",
			port_number
		);

		goto bad_return;
	}

	/*
	 *	Prepara os retornos.
	 */
	*partial   = 0;
	*count_ret = 1;

	*cips_ret = (XtransConnInfo *)xalloc (sizeof (XtransConnInfo));
	if (*cips_ret == NULL)
		goto bad_return;

	**cips_ret = cip;
	return (0);

bad_return:
	*count_ret = 0;
	*cips_ret = NULL;
	*partial = 0;
	return (-1);

}	/* end TRANS(MakeAllCOTSServerListeners) */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
XtransConnInfo
TRANS(MakeListener) (int port)
{
	char		address[20];
	XtransConnInfo	cip;

	port += X_TCP_PORT;

	sprintf	(address, "inet/:%d", port);

	if ((cip = TRANS(OpenCOTSServer) (address)) == NULL)
	{
		xtrans_error
		(	"TRANS(MakeListener): "
			"falha ao alocar um servidor\n"
		);

		return (NULL);
	}

	if (TRANS(CreateListener) (cip, port) < 0)
	{
		xtrans_error
		(	"TRANS(MakeListener): "
			"erro ao alocar o servidor ouvindo a porta %d\n",
			port
		);

		t_close (cip->fd);
		xfree (cip);

		return (NULL);
	}

	return (cip);

}	/* end TRANS(MakeListener) */

/*
 ****************************************************************
 *	Obtém o nome do cliente					*
 ****************************************************************
 */
char *
TRANS(GetClientName) (XtransConnInfo cip)
{
	return ((char *)cip->client_id);

}	/* end TRANS(GetClientName) */
#endif /* TRANS_SERVER */

/*
 ****************************************************************
 *	Obtém o nome da máquina					*
 ****************************************************************
 */
int
TRANS(GetHostname) (char *buf, int maxlen)
{
	return (gethostname (buf, maxlen));

}	/* end TRANS(GetHostName) */

#ifdef X11_t
/*
 ****************************************************************
 *	Conversão de endereços					*
 ****************************************************************
 */
int
TRANS(ConvertAddress) (int *familyp, int *addrlenp, Xtransaddr **addrp)
{
	struct sockaddr_in	addr;
	char			hostnamebuf[NETNM_SZ];
	int			len;

	switch (*familyp)
	{
	    case AF_INET:
		memmove (&addr, *addrp, sizeof (struct sockaddr_in));

		if (addr.sin_addr.s_addr == LOCAL_IP_ADDR)
		{	
			*familyp = FamilyLocal;
		}
		else
		{
			*familyp = FamilyInternet;
			*addrlenp = sizeof (addr.sin_addr.s_addr);
			memmove (*addrp, &addr.sin_addr.s_addr, sizeof (addr.sin_addr.s_addr));
		}
		break;

	    case AF_UNIX:
		*familyp = FamilyLocal;
		break;

	    default:
		xtrans_error ("TRANS(ConvertAddr): família inválida %d\n", *familyp);
		return (-1);
	}

	if (*familyp == FamilyLocal)
	{
		/*
		 * In the case of a local connection, we need to get the
		 * host name for authentication.
		 */
		len = gethostname (hostnamebuf, sizeof (hostnamebuf));
	
		if (len > 0)
		{
			if (*addrp && *addrlenp < (len + 1))
			{
				xfree ((char *)*addrp);
				*addrp = NULL;
			}

			if (!*addrp)
				*addrp = (Xtransaddr *)xalloc (len + 1);

			if (*addrp)
			{
				strcpy ((char *)*addrp, hostnamebuf);
				*addrlenp = len;
			}
			else
			{
				*addrlenp = 0;
			}
		}
		else
		{
			if (*addrp)
				xfree ((char *)*addrp);
			*addrp = NULL;
			*addrlenp = 0;
		}
	}

	return (0);

}	/* end TRANS(ConvertAddress) */
#endif /* X11_t */

#ifdef ICE_t
/*
 ****************************************************************
 *	Código comum para GetMyNetworkID e GetPeerNetworkId	*
 ****************************************************************
 */
static char *
get_network_id (int fd, INADDR *ip)
{
	char		*cp;
	static char	netid[256];

	if ((cp = t_addr_to_node (fd, ip->a_addr)) == NOSTR)
		return (NOSTR);

	sprintf (netid, "inet/%s:%d", cp, ip->a_port);

	return (netid);

}	/* end get_network_id */

/*
 ****************************************************************
 *	Obtém o nome local					*
 ****************************************************************
 */
char *
TRANS(GetMyNetworkId) (XtransConnInfo cip)
{
	return (get_network_id (cip->fd, &cip->addr[LOCAL_ADDR]));

}	/* end GetMyNetworkId */

/*
 ****************************************************************
 *	Obtém o nome remoto					*
 ****************************************************************
 */
char *
TRANS(GetPeerNetworkId) (XtransConnInfo cip)
{
	return (get_network_id (cip->fd, &cip->addr[REMOTE_ADDR]));

}	/* end GetPeerNetworkId */
#endif /* ICE_t */
