/*
 ****************************************************************
 *								*
 *			<sys/ppp.h>				*
 *								*
 *	Definições do protocolo PPP				*
 *								*
 *	Versão	3.0.0, de 28.06.96				*
 *		3.2.3, de 30.03.00				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	PPP_H			/* Para definir os protótipos */

/*
 ******	Definições do PPP ***************************************
 */
#define	PPP_FLAG		0x7E	/* Separador dos pacotes */
#define	PPP_ESC			0x7D	/* Caractere identificador */
#define	PPP_EOR			0x20	/* Valor para alterer o caractere */
#define	PPP_ADDR		0xFF	/* Todas as estações */
#define	PPP_CTL			0x03	/* Informação sem número */

#define PPP_IP			0x0021	/* Internet Protocol */
#define PPP_ISO			0x0023	/* ISO OSI Protocol */
#define PPP_XNS			0x0025	/* Xerox NS Protocol */
#define PPP_VJ_COM		0x002D	/* Van Jacobson Compressed TCP/IP */
#define PPP_VJ_UN		0x002F	/* Van Jacobson Uncompressed TCP/IP */
#define PPP_IPCP		0x8021	/* Internet Protocol Control Protocol */
#define PPP_LCP			0xC021	/* Link Control Protocol */
#define PPP_AUTH_PAP		0xC023	/* Password Authentication Protocol */
#define PPP_AUTH_CHAP		0xC223	/* Challenge handshake Authentication Protocol */

#define LCP_CONF_REQ		 1	/* PPP LCP configure request */
#define LCP_CONF_ACK		 2	/* PPP LCP configure acknowledge */
#define LCP_CONF_NAK		 3	/* PPP LCP configure negative ack */
#define LCP_CONF_REJ		 4	/* PPP LCP configure reject */
#define LCP_TERM_REQ		 5	/* PPP LCP terminate request */
#define LCP_TERM_ACK		 6	/* PPP LCP terminate acknowledge */
#define LCP_CODE_REJ		 7	/* PPP LCP code reject */
#define LCP_PROTO_REJ		 8	/* PPP LCP protocol reject */
#define LCP_ECHO_REQ		 9	/* PPP LCP echo request */
#define LCP_ECHO_REPLY		10	/* PPP LCP echo reply */
#define LCP_DISC_REQ		11	/* PPP LCP discard request */

#define LCP_OPT_MRU		1	/* maximum receive unit */
#define LCP_OPT_ASYNC_MAP	2	/* async control character map */
#define LCP_OPT_AUTH_PROTO	3	/* authentication protocol */
#define LCP_OPT_QUAL_PROTO	4	/* quality protocol */
#define LCP_OPT_MAGIC		5	/* magic number */
#define LCP_OPT_RESERVED	6	/* reserved */
#define LCP_OPT_PROTO_COMP	7	/* protocol field compression */
#define LCP_OPT_ADDR_COMP	8	/* address/control field compression */

#define IPCP_CONF_REQ	LCP_CONF_REQ	/* PPP IPCP configure request */
#define IPCP_CONF_ACK	LCP_CONF_ACK	/* PPP IPCP configure acknowledge */
#define IPCP_CONF_NAK	LCP_CONF_NAK	/* PPP IPCP configure negative ack */
#define IPCP_CONF_REJ	LCP_CONF_REJ	/* PPP IPCP configure reject */
#define IPCP_TERM_REQ	LCP_TERM_REQ	/* PPP IPCP terminate request */
#define IPCP_TERM_ACK	LCP_TERM_ACK	/* PPP IPCP terminate acknowledge */
#define IPCP_CODE_REJ	LCP_CODE_REJ	/* PPP IPCP code reject */

#define IPCP_OPT_VJ		2	/* Compressão TCP/IP */
#define IPCP_OPT_IPADDR		3	/* Endereço IP */

/* Nas estruturas seguintes, não podemos usar "long" em virtude do alinhamento */

#define	PPP_H_SZ	4
typedef struct { char code, id; short len; }	LCP_H;
typedef struct { char code, id; short len; }	IPCP_H;

typedef struct { char code, len, map[4]; }	ASYNC_MAP_OPT;
typedef struct { char code, len, magic[4]; }	MAGIC_OPT;
typedef struct { char code, len; }		PROTO_COMP_OPT;
typedef struct { char code, len; }		ADDR_COMP_OPT;

typedef struct { char code, len; short p; char max_slot, comp_slot; } VJ_OPT;
typedef struct { char code, len, ipaddr[4]; }	IPADDR_OPT;

/*
 ******	Estrutura de controle do protocolo PPP ******************
 */
typedef struct slcompress	SLCOMPRESS;

typedef	struct
{
	unsigned long	ppp_magic_number;
	unsigned long	ppp_async_char_map;
	IPADDR		ppp_addr;

	char		ppp_proto_field_compression;
	char		ppp_addr_field_compression;
	ushort		ppp_vj_compression;

	char		ppp_vj_slot_compression;
	char		ppp_vj_max_slot;

}	PPP_COMP;

typedef struct
{
	char			ppp_lcp_state;
	char			ppp_ipcp_state;
	char			ppp_my_addr_added;

	PPP_COMP		snd;
	PPP_COMP		rcv;

	char			ppp_char_map_vec[256];
	SLCOMPRESS		*ppp_slcompress_ptr;

}	PPP_STATUS;

/*
 ******	Estado do AUTOMATA **************************************
 */
enum
{
	LCP_STATE_CLOSED,		/* LCP state: closed */
	LCP_STATE_ACK_SENT,		/* LCP state: conf-ack sent */
	LCP_STATE_ACK_RCVD,		/* LCP state: conf-ack received */
	LCP_STATE_OPENED		/* LCP state: opened */
};

enum
{
	IPCP_STATE_CLOSED,		/* IPCP state: closed */
	IPCP_STATE_ACK_SENT,		/* IPCP state: conf-ack sent */
	IPCP_STATE_ACK_RCVD,		/* IPCP state: conf-ack received */
	IPCP_STATE_OPENED		/* IPCP state: opened */
};

/*
 ******	Definições da compressão segundo Van Jacobson ***********
 */
#define N_SLOT		16	/* Must be > 2 and < 255 */
#define MAX_TCP_IP_HDR	128	/* Max TCP+IP hdr length (by protocol def) */

/*
 *	"State" data for each active tcp conversation on the wire.
 *	This is basically a copy of the entire IP/TCP header from
 *	the last packet together with a small identifier the transmit
 *	& receive ends of the line use to locate saved header
 */
typedef struct cstate	CSTATE;

struct cstate
{
	CSTATE	*cs_next;	/* Next most recently used cstate (xmit only) */
	ushort	cs_hlen;	/* Size of hdr (receive only) */
	char	cs_slot;	/* Connection # associated with this state */
	char	cs_filler;

    union
    {
	char	csu_hdr[MAX_TCP_IP_HDR];
	IP_H	csu_ip;		/* IP/TCP hdr from most recent packet */

    }	slcs_u;
};

#define cs_ip	slcs_u.csu_ip
#define cs_hdr	slcs_u.csu_hdr

/*
 *	All the state data for one serial line (we need one of these per line)
 */
struct slcompress
{
	CSTATE	*last_cs;		/* Most recently used tstate */
	char	last_rcv_slot;		/* Last rcvd conn. id */
	char	last_snd_slot;		/* Last sent conn. id */
	char	flags;
	char	busy;
	CSTATE	tstate[N_SLOT];		/* Xmit connection states */
	CSTATE	rstate[N_SLOT];		/* Receive connection states */

};

/* Flag values */

#define SLF_TOSS 1	/* Tossing rcvd frames because of input err */
