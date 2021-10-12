/*
 ****************************************************************
 *								*
 *			Xtrans.h				*
 *								*
 *	Definições visíveis externamente			*
 *								*
 *	Versão	1.0.0, de 30.10.96				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#ifndef _XTRANS_H_
#define _XTRANS_H_

#include "Xos.h"

/*
 ****************************************************************
 *	Cada macaco no seu galho				*
 ****************************************************************
 */
#ifdef X11_t
#define TRANS(func)	_X11Trans##func
#endif /* X11_t */

#ifdef XSERV_t
#define TRANS(func)	_XSERVTrans##func
#define X11_t
#endif /* X11_t */

#ifdef XIM_t
#define TRANS(func)	_XimXTrans##func
#endif /* XIM_t */

#ifdef FS_t
#define TRANS(func)	_FSTrans##func
#endif /* FS_t */

#ifdef FONT_t
#define TRANS(func)	_FontTrans##func
#endif /* FONT_t */

#ifdef ICE_t
#define TRANS(func)	_IceTrans##func
#endif /* ICE_t */

#ifdef TEST_t
#define TRANS(func)	_TESTTrans##func
#endif /* TEST_t */

#if !defined(TRANS)
#define TRANS(func)	_XTrans##func
#endif /* !TRANS */

/*
 ****************************************************************
 *	Valores retornados/esperados por algumas funções	*
 ****************************************************************
 */
/*
 *	Opções para TRANS(SetOption)
 */
#define TRANS_NONBLOCKING		1
#define	TRANS_CLOSEONEXEC		2

/*
 *	Valores de retorno para TRANS(Connect)
 */
#define TRANS_CONNECT_FAILED		-1
#define TRANS_TRY_CONNECT_AGAIN		-2
#define TRANS_IN_PROGRESS		-3

/*
 *	Valores de retorno para TRANS(CreateListener)
 */
#define TRANS_CREATE_LISTENER_FAILED 	-1
#define TRANS_ADDR_IN_USE		-2

/*
 *	Valores de retorno para TRANS(Accept)
 */
#define TRANS_ACCEPT_BAD_MALLOC		-1
#define TRANS_ACCEPT_FAILED 		-2
#define TRANS_ACCEPT_MISC_ERROR		-3

/*
 *	Valores de retorno para TRANS(ResetListener)
 */
#define TRANS_RESET_NOOP		1
#define TRANS_RESET_NEW_FD		2
#define TRANS_RESET_FAILURE		3

/*
 ****************************************************************
 *	Definições de tipos					*
 ****************************************************************
 */
/*
 *	Estrutura universal para endereços (?)
 */
#define XTRANS_MAX_ADDR_LEN	128	/* large enough to hold sun_path */

typedef	struct
{
	uchar	addr[XTRANS_MAX_ADDR_LEN];

}	Xtransaddr;

/*
 *	Tipo "número de bytes", compatível com read, write, ...
 */
typedef int	BytesReadable_t;

/*
 *	Apenas o ponteiro para a estrutura _XtransConnInfo é
 *	utilizado externamente, funcionando como descritor.
 */
typedef struct _XtransConnInfo	*XtransConnInfo;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
#ifdef TRANS_CLIENT
extern XtransConnInfo	TRANS(OpenCOTSClient) (char *);
extern XtransConnInfo	TRANS(OpenCLTSClient) (char *);
extern int		TRANS(Connect) (XtransConnInfo, char *);
#endif /* TRANS_CLIENT */

#ifdef TRANS_SERVER
extern XtransConnInfo	TRANS(OpenCOTSServer) (char *);
extern XtransConnInfo	TRANS(OpenCLTSServer) (char *);
extern XtransConnInfo	TRANS(Accept) (XtransConnInfo, int *);
extern int		TRANS(CreateListener) (XtransConnInfo, int);
extern int		TRANS(ResetListener) (XtransConnInfo);
extern int		TRANS(MakeAllCOTSServerListeners) (char *, int *, int *, XtransConnInfo **);
extern int		TRANS(MakeAllCLTSServerListeners) (char *, int *, int *, XtransConnInfo **);
extern XtransConnInfo	TRANS(MakeListener) (int);
extern char		*TRANS(GetClientName) (XtransConnInfo);
#endif /* TRANS_SERVER */

#ifdef TRANS_REOPEN
extern XtransConnInfo	TRANS(ReopenCOTSServer) (int, int, int);
extern XtransConnInfo	TRANS(ReopenCLTSServer) (int, int, int);
extern int		TRANS(GetReopenInfo) (XtransConnInfo, int *, int *, int *);
#endif /* TRANS_REOPEN */

extern int		TRANS(SetOption) (XtransConnInfo, int, int);
extern int		TRANS(BytesReadable) (XtransConnInfo, BytesReadable_t *);
extern int		TRANS(Read) (XtransConnInfo, char *, int);
extern int		TRANS(Write) (XtransConnInfo, char *, int);
extern int		TRANS(Readv) (XtransConnInfo, struct iovec *, int);
extern int		TRANS(Writev) (XtransConnInfo, struct iovec *, int);
extern int		TRANS(Disconnect) (XtransConnInfo);
extern int		TRANS(Close) (XtransConnInfo);
extern int		TRANS(CloseForCloning) (XtransConnInfo);
extern int		TRANS(IsLocal) (XtransConnInfo);
extern int		TRANS(GetMyAddr) (XtransConnInfo, int *, int *, Xtransaddr **);
extern int		TRANS(GetPeerAddr) (XtransConnInfo, int *, int *, Xtransaddr **);
extern int		TRANS(GetConnectionNumber) (XtransConnInfo);

#ifdef X11_t
extern int		TRANS(ConvertAddress) (int *, int *, Xtransaddr **);
#endif /* X11_t */

#ifdef ICE_t
extern char		*TRANS(GetMyNetworkId) (XtransConnInfo);
extern char		*TRANS(GetPeerNetworkId) (XtransConnInfo);
#endif /* ICE_t */

#endif /* _XTRANS_H_ */
