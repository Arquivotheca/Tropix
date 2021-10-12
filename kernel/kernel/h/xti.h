/*
 ****************************************************************
 *								*
 *			<sys/xti.h>				*
 *								*
 *	Definicões para a utilização da interface XTI		*
 *								*
 *	Versão	2.3.12, de 05.08.91				*
 *		4.9.00, de 22.08.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	XTI_H		/* Para incluir os protótipos */

/*
 ****************************************************************
 *	Algumas definições gerais				*
 ****************************************************************
 */
#define	NULL		0
#define	T_NULL		0
#define	T_YES		1
#define	T_NO		0
#define	T_UNUSED	-1
#define	T_ABSREQ	0x8000

/*
 ******	Macros de definições de algumas funções/variáveis *******
 */
#define	t_close		close
#define	t_error		perror
#define	t_errno		errno

#define	TSYSERR		(-3)	/* Nunca ocorre */

/*
 ******	Estado da conexão ***************************************
 */
typedef enum
{
	T_UNINIT,	/* Não inicializado */
	T_UNBND,	/* "unbound" */
	T_IDLE,		/* Inerte */
	T_OUTCON,	/* Enviando pedido de conexão */
	T_INCON,	/* Recebendo pedido de conexão */
	T_DATAXFER,	/* Transferindo dados */
	T_OUTREL,	/* Enviando pedido de desconexão */
	T_INREL		/* Recebendo pedido de desconexão */

}	T_STATE;

/*
 ******	Eventos *************************************************
 */
#define	T_LISTEN	0x0001	/* Recepção de um pedido de conexão */
#define T_CONNECT	0x0002	/* Recepção da confirmação de conexão */
#define	T_DATA		0x0004	/* Recepção de mensagem */
#define	T_EXDATA	0x0008	/* Recepção de mensagem expedita */
#define	T_DISCONNECT	0x0010	/* Recepção de um pedido de desconexão */
#define	T_UDERR		0x0040	/* Erro em datagrama */
#define	T_ORDREL	0x0080	/* Indicação de desconexão ordenada */
#define	T_GODATA	0x0100	/* É possível enviar mensagem */
#define	T_GOEXDATA	0x0200	/* É possível enviar mensagem expedita */
#define	T_TIMEOUT	0x0400	/* O tempo foi excedido */

/*
 ******	Definições de flags *************************************
 */
#define	T_MORE		0x0001	/* A mensagem continua */
#define	T_EXPEDITED	0x0002	/* Mensagem expedita */
#define	T_NEGOTIATE	0x0004	/* Especificar opções */
#define	T_CHECK		0x0008	/* Verificar opções */
#define	T_DEFAULT	0x0010	/* Utilizar os defaults nas opções */
#define	T_SUCCESS	0x0020	/* Sucesso */
#define	T_FAILURE	0x0040	/* Erro */
#define	T_PUSH		0x0100	/* PUSH do TCP */
#define	T_URGENT	0x0200	/* Segmento urgente do TCP */
#define	T_NO_CHECKSUM	0x0400	/* Datagrama UDP recebido sem "checksum" */

/*
 ******	Informação utilizada pelos protocolos *******************
 */
typedef struct	t_info
{
	long	addr;		/* Tamanho do endereço utilizado */
	long 	options;	/* Tamanho das opções utilizadas */
	long	tsdu;		/* Tamanho máximo da mensagem */
	long	etsdu;		/* Tamanho máximo da mensagem urgente */
	long	connect;	/* Max.de dados para primitivas de conexão */
	long	discon;		/* Max.de dados para primitivas de desconexão */
	long	servtype;	/* Modo de comunicação */

}	T_INFO;

/*
 ******	Modos de comunicação definidos **************************
 */
#define	T_COTS		1	/* connection mode */
#define	T_COTS_ORD	2	/* connection mode com desconexão ordenada */
#define	T_CLTS		3	/* connectionless mode */

/*
 ******	Estrutura do endereço ***********************************
 */
typedef struct	inaddr		/* Estrutura contendo um endereço */
{
    long	   a_addr;	/* Endereço na rede */
    unsigned short a_family;	/* Família de protocolos */
    unsigned short a_port;	/* "port" */

}	INADDR;

#define	a_proto	a_port		/* Para uso do protocolo RAW */

/*
 ******	Estrutura utilizada por diversas funções ****************
 */
typedef struct	netbuf
{
	int	maxlen;		/* Tamanho de buf */
	int	len;		/* Número de bytes em buf */
	void	*buf;		/* Dados */

}	NETBUF;

/*
 ******	Estrutura utilizada pela função "bind" ******************
 */
typedef struct	t_bind
{
	NETBUF	addr;		/* Endereço ("port") */
	int	qlen;		/* Tamanho da fila */

}	T_BIND;

/*
 ******	Estrutura para a gerência de opções *********************
 */
typedef struct	t_optmgmt
{
	NETBUF	opt;		/* Opções */
	long	flags;		/* Flags */

}	T_OPTMGMT;

typedef struct	secoptions
{
	short	security;	/* Security field */
	short	compartment;	/* Compartment */
	short	handling;	/* Handling retrictions */
	long	tcc;		/* Transmission control code */

}	SECOPTIONS;

typedef struct	tcp_options
{
	short	precedence;	/* Precedence */
	long	timeout;	/* Abort timeout for TCP in miliseconds */
	long	max_seg_size;	/* Maximum segment size */
	long	max_wait;	/* Tempo máximo de silêncio desconectado */
	long	max_silence;	/* Tempo máximo de silêncio conectado */
	long	window_size;	/* Tamanho da janela de entrada */
	long	max_client_conn;/* Número máximo de conexões por cliente */
	long	reser[1];	/* Reservado para uso futuro */
	SECOPTIONS secopt;	/* Security options for TCP */

}	TCP_OPTIONS;

/*
 ******	Estrutura de desconexão *********************************
 */
typedef struct	t_discon
{
	NETBUF	udata;		/* Mensagem */
	int	reason;		/* Motivo da desconexão */
	int	sequence;	/* Número da sequência */

}	T_DISCON;

/*
 ******	Estrutura utilizada pela função "connect" ***************
 */
typedef struct	t_call
{
	NETBUF	addr;		/* Endereço */
	NETBUF	opt;		/* Opções */
	NETBUF	udata;		/* Mensagem */
	int	sequence;	/* Número da seqüência */

}	T_CALL;

/*
 ******	Estrutura utilizada pela função "sndudata" **************
 */
typedef struct	t_unitdata
{
	NETBUF	addr;		/* Endereço */
	NETBUF	opt;		/* Opções */
	NETBUF	udata;		/* Mensagem */

}	T_UNITDATA;

/*
 ******	Estrutura de erro de UNITDATA ***************************
 */
typedef struct	t_uderr
{
	NETBUF	addr;		/* Endereço */
	NETBUF	opt;		/* Opções */
	long	error;		/* Código do erro */

}	T_UDERROR;

/*
 ******	Tipos de estruturas para "t_alloc" **********************
 */
typedef enum
{
	T_BIND_STR = 1,	/* Estrutura "t_bind" */
	T_OPTMGMT_STR,	/* Estrutura "t_optmgmt" */
	T_CALL_STR,	/* Estrutura "t_call" */
	T_DIS_STR,	/* Estrutura "t_discon" */
	T_UNITDATA_STR,	/* Estrutura "t_unitdata" */
	T_UDERROR_STR,	/* Estrutura "t_uderr" */
	T_INFO_STR	/* Estrutura "t_info" */

}	T_ALLOC_ENUM;

/*
 ******	Campos da estrutura para "t_alloc" **********************
 */
#define	T_ADDR		0x01	/* Endereço */
#define	T_OPT		0x02	/* Opções */
#define	T_UDATA		0x04	/* Dados */
#define	T_ALL		0x07	/* Todos acima */

/*
 ****** Protótipos **********************************************
 */
extern int	close (int);
extern void	perror (const char *);
extern int	errno;

extern int	t_accept (int, int, const T_CALL *);
extern void	*t_alloc (int, T_ALLOC_ENUM, int);
extern int	t_bind (int, const T_BIND *, T_BIND *);
extern int	t_connect (int, const T_CALL *, T_CALL *);
extern int	t_push (int);
extern int	t_free (const void *, T_ALLOC_ENUM);
extern int	t_getinfo (int, T_INFO *);
extern int	t_getstate (int);
extern int	t_listen (int, T_CALL *);
extern int	t_look (int);
extern int	t_nread (int);
extern int	t_open (const char *, int, T_INFO *);
extern int	t_optmgmt (int, const T_OPTMGMT *, T_OPTMGMT *);
extern int	t_rcv (int, void *, int, int *);
extern int	t_rcvconnect (int, T_CALL *);
extern int	t_rcvdis (int, T_DISCON *);
extern int	t_rcvrel (int);
extern int	t_rcvudata (int, T_UNITDATA *, int *);
extern int	t_rcvuderr (int, T_UDERROR *);
extern int	t_snd (int, const void *, int, int);
extern int	t_snddis (int, const T_CALL *);
extern int	t_sndrel (int);
extern int	t_sndudata (int, const T_UNITDATA *);
extern char	*t_strevent (int, int);
extern int	t_sync (int);
extern int	t_unbind (int);

extern char	*t_addr_to_name (int, long);
extern char	*t_addr_to_node (int, long);
extern char	*t_addr_to_str (long);
extern char	*t_mail_to_node (int, const char *, int, int *);
extern long	t_node_to_addr (int, const char *, long *);
extern long	t_str_to_addr (const char *);
extern int	t_getaddr (int, INADDR *);
extern int	t_rcvraw (int, T_UNITDATA *, int *);
extern int	t_sndraw (int, const T_UNITDATA *);
