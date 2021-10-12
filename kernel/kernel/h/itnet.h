/*
 ****************************************************************
 *								*
 *			<sys/itnet.h>				*
 *								*
 *	Definicões do INTERNET					*
 *								*
 *	Versão	3.0.0, de 06.09.94				*
 *		4.9.0, de 30.08.06				*
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

#define	ITNET_H		/* Para incluir os protótipos */

/*
 ******	Definições gerais ***************************************
 */
#define	PITNETIN	720	/* Entrada do ITNET */
#define	PITNETOUT	710	/* Saida   do ITNET */

/*
 ******	Definição de alguns tipos *******************************
 */
typedef ulong			seq_t;	/* Tipo de sequenciamento */

typedef enum { DAEMON = 0, RAW, UDP, TCP }	IT_MINOR;

typedef long			IPADDR;	/* Endereço IP */

typedef struct { char addr[6]; } ETHADDR; /* Endereço ETHERNET */

/*** typedef struct itblock	ITBLOCK; (em "common.h ") ***/

/*** typedef	struct raw_ep 	RAW_EP; (em "common.h ") ***/

/*** typedef	struct udp_ep 	UDP_EP; (em "common.h ") ***/

/*** typedef	struct tcp_ep 	TCP_EP; (em "common.h ") ***/

/*
 ****************************************************************
 *	Estruturas gerais da INTERNET				*
 ****************************************************************
 */

/*
 ******	Estrutura da tabela DNS *********************************
 */
enum {	NETNM_SZ = 80 };		/* Tamanho do domain-name */

struct dns
{
	char	d_host_nm[NETNM_SZ];	/* Nome simbólico do nó destino */
	IPADDR	d_host_addr;		/* Endereço do nó destino */

	char	d_entry_code;		/* Código da entrada: veja abaixo */
	EVENT	d_resolver_done;	/* O "name-resolver" deu um resultado */
	char	d_error_code;		/* Se diferente de 0, é erro (abaixo) */
	char	d_server_index;		/* No. do servidor que conseguiu o resultado */

	time_t	d_expir_time;		/* Data de expiração do endereço */
	time_t	d_last_use_time;	/* Data da última consulta ao endereço */

	ushort	d_preference;		/* Preferência no caso de servidor de mail */
	char	d_resolver_got_req;	/* O "name-resolver" pegou o pedido */
	char	d_mail_list_ok;		/* A lista de servidores de mail foi examinada */

	DNS	*d_can_ptr;		/* Ponteiro para a entrada canônica */
	DNS	*d_mail_ptr;		/* Lista de servidores de mail */
};

#define	NODNS	(DNS *)NULL		/* Ponteiro NULO */

/*
 *	Códigos da entrada:
 *
 *		"C" para nome canônico
 *		"A" para alias
 *		"M" domínio de correio
 */

enum DNS_ERROR_CODE
{
	/*  0 */   DNS_NO_ERROR = 0,	/* Nenhum erro */
	/*  1 */   DNS_NAME_ERROR,	/* Nome não existe */
	/*  2 */   DNS_SERVER_ERROR	/* Problemas com o servidor */
};

/*
 ******	Estrutura da tabela ROUTE *******************************
 */
struct route
{
	IPADDR		r_mask;			/* Máscara para roteamento */
	IPADDR		r_net_addr;		/* O endereço IP desta rede destino */

	IPADDR		r_my_addr;		/* O meu endereço IP */
	IPADDR		r_gateway_addr;		/* O endereço IP do "gateway" */

	INODE		*r_inode;		/* Define o dispositivo para enviar */
	char		r_dev_nm[16];		/* Nome do dispositivo para enviar */

	ETHADDR		r_my_ether_addr;	/* O meu endereço ETHERNET */

	bit		r_ether_dev	: 1;	/* Indica que é ETHERNET */
	bit		r_dhcp		: 1;	/* Indica que esta entrada é DHCP */
	bit		r_default	: 1;	/* Indica que esta entrada é "default" */

	EVENT		r_dhcp_done;		/* Para acordar o "daemon" DHCP */
	LOCK		r_dhcp_lock;		/* Para evitar corridas */

	ITBLOCK		*r_dhcp_bp;		/* Bloco com a informação DHCP */

	int		r_reser[3];		/* Reserva */
};

#define	NOROUTE	(ROUTE *)NULL		/* Ponteiro NULO */

#define	MY_IP_ADDR (127 << 24 | 1)	/* Endereço IP do próprio nó */

/*
 ******	Estrutura da tabela ETHER *******************************
 */
typedef struct ether	ETHER;

struct ether
{
	IPADDR	e_ip_addr;		/* O endereço IP */
	ETHADDR	e_ether_addr;		/* O endereço ETHERNET */
	time_t	e_ether_time;		/* Última referencia a este endereço */
	ETHER	*e_next;		/* Ponteiro para próxima */
};

#define	NOETHER	(ETHER *)NULL		/* Ponteiro NULO */

/*
 ******	Estrutura da tabela EXPORT ******************************
 */
struct export
{
	char	e_dir_path[NETNM_SZ];	/* Nome do diretório a ser exportado */

	/* Os 3 membros seguintes devem ser compatíveis com a chamada a "k_node_to_addr" */

	char	e_client_nm[NETNM_SZ];	/* Nome simbólico do nó destino */
	IPADDR	e_client_addr;		/* Endereço do nó destino */
	IPADDR	e_my_addr;		/* Endereço do próprio nó */

	char	e_active;		/* Esta entrada de "exports" está ativa */
	char	e_mounted;		/* O diretório está montado */
	char	e_secure;		/* Porta deve ser abaixo de 1024 */
	char	e_ronly;		/* Somente para leitura */

	char	e_sync;			/* As escritas devem ser síncronas */
	char	e_hide;			/* Esconde árvores montadas */
	char	e_no_root_squash;	/* NÃO mapeia ROOT em -2 */
	char	e_all_squash;		/* Mapeia todos os usuários em -2 */

	char	e_reser0[3];
	char	e_use_anon_ids;		/* Usa os IDs abaixo */

	int	e_anon_uid;		/* UID de conta anônima */
	int	e_anon_gid;		/* GID de conta anônima */

	int	e_reser1[17];		/* Reserva para 256 bytes */
};

#define	NOEXPORT (EXPORT *)NULL		/* Ponteiro NULO */

/*
 ******	Bloco de controle da ITNET ******************************
 */
typedef enum { IN_EOF, IN_BP, IN_EP } IN_ENUM;	/* Para a fila de entrada TCP */

typedef struct
{
	IN_ENUM	inq_enum;
	void	*inq_ptr;

}	INQ_PTR;

typedef struct
{
	LOCK	it_init_lock;	/* Não zero se inicializada (TAS) */

	LOCK	it_lock_free;	/* SPINLOCK da lista livre */
	SEMA	it_block_sema;	/* Semáforo de blocos livres */
	int	it_bl_count;	/* No. total de blocos obtidos */
	int	it_bl_other;	/* No. total de blocos obtidos */
	int	it_bl_max_other; /* No. total de blocos obtidos */
	ITBLOCK	*it_block_free;	/* Início da lista livre de blocos */

	LOCK	it_rep_lock;	/* SPINLOCK da lista livre de RAW EPs */
	int	it_rep_total;	/* No. total de RAW EPs */
	int	it_rep_count;	/* No. total de RAW EPs alocados */
	RAW_EP	*it_rep_free;	/* Início da lista livre de RAW EPs */
	RAW_EP	*it_rep_busy;	/* Lista ativa de RAW EPs */

	LOCK	it_uep_lock;	/* SPINLOCK da lista livre de UDP EPs */
	int	it_uep_total;	/* No. total de UDP EPs */
	int	it_uep_count;	/* No. total de UDP EPs alocados */
	UDP_EP	*it_uep_free;	/* Início da lista livre de UDP EPs */
	UDP_EP	*it_uep_busy;	/* Lista ativa de UDP EPs */

	LOCK	it_tep_lock;	/* SPINLOCK da lista livre de TCP EPs */
	int	it_tep_total;	/* No. total de TCP EPs */
	int	it_tep_count;	/* No. total de TCP EPs alocados */
	TCP_EP	*it_tep_free;	/* Início da lista livre de TCP EPs */
	TCP_EP	*it_tep_busy;	/* Lista ativa de TCP EPs */

	LOCK	it_inq_lock;	/* SPINLOCK da fila de entrada TCP */
	EVENT	it_inq_nempty;	/* Fila vazia? */
	INQ_PTR	it_inq_first;	/* Começo da fila */
	INQ_PTR	it_inq_last;	/* Último da fila */

	/* Vários indicadores de contrôle e depuração */

	char	it_write_input;	/* Grava em disco a entrada */
	char	it_write_output;/* Grava em disco a saída */
	char	it_list_input;	/* Relata a entrada */
	char	it_list_output;	/* Relata  a saída */
	char	it_retransmission; /* Relata retransmissões */
	char	it_outq_deletions; /* Relata remoções da lista de saída */
	char	it_gen_chk_err;	/* Gera alguns erro de checksum */
	char	it_report_error; /* Relata erros */
	char	it_list_info;	/* Relata algumas informações */
	char	it_list_bad_seg; /* Relata segmentos recusados */
	char	it_list_window;	/* Relata valores das janelas */
	char	it_list_SRTT;	/* Relata cálculo do SRTT */
	char	it_gateway;	/* Envia datagramas para o nó seguinte */
	char	it_pipe_mode;	/* Usa a otimização interna */
	char	it_reserv0[18];	/* Para completar 32 */

#define ITSCB_FLAG_SZ	32

	int	it_N_BLOCK;	/* Semiconstante: No. de blocos do "pool" */
	int	it_WND_SZ;	/* Semiconstante: Tamanho padrão da janela */
	int	it_GOOD_WND;	/* Semiconstante: Tamanho da janela "cheia" */
	int	it_ALPHA;	/* Semiconstante: Envelhecimento de SRTT */
	int	it_BETA;	/* Semiconstante: Fator de SRTT */
	int	it_SRTT;	/* Semiconstante: Valor inicial de SRTT */
	int	it_N_TRANS;	/* Semiconstante: No. de transmissões de TCP */
	int	it_WAIT;	/* Semiconstante: Máximo de silencio sem conexão */
	int	it_SILENCE;	/* Semiconstante: Máximo de silencio conectado */
	int	it_MAX_SGSZ;	/* Semiconstante: Tamanho máximo do segmento */

	int	it_reser1[20];	/* Para completar 256 bytes */

}	ITSCB;

/*
 ****************************************************************
 *	Definições relativas a ETHERNET				*
 ****************************************************************
 */
#define	ETHER_MIN_LEN	60		/* Tamanho mínimo de um pacote */
#define ETHER_MAX_LEN	1514		/* Tamanho máximo de um pacote */
#define	ETHER_CRC_LEN	4		/* Tamanho do CRC */

typedef struct
{
	ETHADDR	it_ether_dst;		/* Endereço ethernet destino */
	ETHADDR	it_ether_src;		/* Endereço ethernet fonte */
	ushort	it_type;		/* Tipo do pacote ethernet */

}	ETH_H;

#define ETHERTYPE_ARP	0x0806		/* Protocolo ARP */
#define ETHERTYPE_IP	0x0800		/* Protocolo IP */

/*
 ****************************************************************
 *	Definições relativas aos bloco de entrada/saída		*
 ****************************************************************
 */
#define	DGSZ	1500			/* Tamanho máximo dos datagramas */
#define	RESSZ	130			/* Tamanho de reserva (2 bytes a mais) */
#define	FRAMESZ (RESSZ + DGSZ)		/* Tamanho máximo do pacote */

#define NOITBLOCK	(ITBLOCK *)NULL

struct itblock
{
	/* Fila de ITBLOCKs */

	dev_t	it_dev;			/* Dispositivo de origem do pacote */
	char	it_get_enum;		/* Fila: IT_IN, IT_OUT_... */

	/* Variáveis diversas */

	EVENT	it_arp_event;		/* Para esperar resposta ARP */
	char	it_ppp_is_ppp;		/* O pacote de entrada é PPP */
	char	it_escape_char;		/* Usado pelo SLIP e PPP */

	void	*it_ip_header;		/* Ponteiro para o cabeçalho IP */
	int	it_proto;		/* Para o modo RAW */
	ITBLOCK	*it_busy_next;		/* Lista de depuração */

	int	it_reser0;

	/* Indicadores */

  union
  {
	int	it_flags1;		/* Os 4 indicadores abaixo */

     struct
     {
	char	it_u_in_free_list;	/* Ligado se na lista livre */
	char	it_u_in_driver_queue;	/* Ligado se na fila do driver */
	char	it_u_free_after_IO;	/* Devolva para a lista livre após IO */
	char	it_u_ether_dev;		/* Vai/veio de dispositivo ETHERNET */

     }	s1;

  }	u1;

  union
  {
	int	it_flags2;		/* Os 4 os indicadores abaixo */

     struct
     {
	char	it_u_ether_header_ready; /* Cabeçalho ETHERNET já semipronto */
	char	it_u_wait_for_arp;	/* Se não tem endereço ETHERNET, espera */
	char	it_u_ppp_dev;		/* Veio de disposito PPP */
	char	it_u_wnd_tst_seg;	/* Este é um segmento de teste de janela */

     }	s2;

  }	u2;

  union
  {
	int	it_flags3;		/* Os 4 indicadores abaixo */

	/* Reservado para mais 4 indicadores */

  }	u3;

	/* Ponteiros das filas */

	INQ_PTR	it_inq_forw;		/* Ponteiro da fila de entrada */

	ITBLOCK	*it_outq_forw;		/* Ponteiro da fila de saída */
	ITBLOCK	*it_forw;		/* Ponteiro da fila livre/driver */

	/* Endereços IP */

	IPADDR	it_src_addr;		/* Endereço da fonte */
	IPADDR	it_dst_addr;		/* Endereço do destino */
	ROUTE	*it_route;		/* Entrada de roteamento associada */

	/* Ponteiros para o conteúdo do bloco */

	void	*it_u_area;		/* Usado durante a E/S */
	int	it_u_count;

	char	*it_area;
	int	it_count;

	char	*it_data_area;		/* No momento só usado pelo PPP */
	int	it_data_count;

	/* Informações usadas pelo TCP */

	ulong	it_ctl;			/* Indicadores do segmento */
	ulong	it_seg_seq;		/* No. de seqüência do início */
	ulong	it_seg_len;		/* Tamanho do segmento */
	ulong	it_seg_nxt;		/* No. de seqüência do bloco seguinte */
	ulong	it_seg_ack;		/* No. de ACK */
	ulong	it_seg_wnd;		/* Janela do segmento */
	ulong	it_seg_up;		/* Ponteiro de urgência */
	ulong	it_seg_prc;		/* Valor de precedência */

	int	it_listen_seq;		/* No. de seq. do SYN */
	int	it_n_transmitted;	/* No. de vezes já transmitido */
	time_t	it_time;		/* Tempo da última transmissão */

	/* Fila de pedidos para NFS */

	ITBLOCK	*it_nfs_forw;

	/* Reserva */

	int	it_reser1[7];

	/* Conteúdo do bloco */

	char	it_frame[FRAMESZ];	/* Pacote (cabeç. ETHERNET + Datagrama) */

}	/* end ITBLOCK */;

	/* Equivalências */

#define	it_in_free_list		u1.s1.it_u_in_free_list
#define	it_in_driver_queue	u1.s1.it_u_in_driver_queue
#define	it_free_after_IO	u1.s1.it_u_free_after_IO
#define	it_ether_dev		u1.s1.it_u_ether_dev
#define	it_ether_header_ready	u2.s2.it_u_ether_header_ready
#define	it_wait_for_arp		u2.s2.it_u_wait_for_arp
#define	it_ppp_dev		u2.s2.it_u_ppp_dev
#define	it_wnd_tst_seg		u2.s2.it_u_wnd_tst_seg

#define	it_urg_count 		it_seg_up

/*
 ****** Modo de pedir um bloco **********************************
 */
typedef enum
{
	IT_IN,		/* Entrada: NÃO espera se nenhum disponível */
	IT_OUT_DATA,	/* Espera até ficar disponível */
	IT_OUT_CTL	/* Espera até ficar disponível */

}	ITBLENUM;

/*
 ****************************************************************
 *	Definições relativas ao protocolo IP			*
 ****************************************************************
 */
#define	IP_VERSION	4		/* Versão atual */

#define	IP_BROADCAST	0xFFFFFFFF	/* Endereço para difusão */

/*
 ******	Números associados aos protocolos ***********************
 */
typedef	enum
{
	ICMP_PROTO = 1,
	TCP_PROTO  = 6,
	UDP_PROTO  = 17

}	PROTO_ENUM;

/*
 ******	Cabeçalho do datagrama IP *******************************
 */
typedef	struct
{
#ifdef	LITTLE_ENDIAN
	bit	ip_h_size : 4;		/* Tamanho do cabeçalho em longs */
	bit	ip_version : 4;		/* Versão do protocolo IP */

	bit	ip_reser0 : 2;
	bit	ip_high_rely : 1;	/* Grande confiabilidade */
	bit	ip_high_th_put : 1;	/* Grande capacidade */
	bit	ip_low_delay : 1;	/* Resposta rápida */
	bit	ip_precedence : 3;	/* Precedência (0 a 7) */

	short	ip_size;		/* Tamanho total do datagrama em bytes */
	ushort	ip_id;			/* Identificação do pacote */

	bit	ip_fragment_off : 13;	/* Deslocamento do fragmento */
	bit	ip_last_fragment : 1;	/* Último fragmento */
	bit	ip_do_not_fragment : 1;	/* Não fragmente este datagrama */
	bit	ip_reser1 : 1;
#else
	bit	ip_version : 4;		/* Versão do protocolo IP */
	bit	ip_h_size : 4;		/* Tamanho do cabeçalho em longs */

	bit	ip_precedence : 3;	/* Precedência (0 a 7) */
	bit	ip_low_delay : 1;	/* Resposta rápida */
	bit	ip_high_th_put : 1;	/* Grande capacidade */
	bit	ip_high_rely : 1;	/* Grande confiabilidade */
	bit	ip_reser0 : 2;

	short	ip_size;		/* Tamanho total do datagrama em bytes */
	ushort	ip_id;			/* Identificação do pacote */

	bit	ip_reser1 : 1;
	bit	ip_do_not_fragment : 1;	/* Não fragmente este datagrama */
	bit	ip_last_fragment : 1;	/* Último fragmento */
	bit	ip_fragment_off : 13;	/* Deslocamento do fragmento */
#endif	LITTLE_ENDIAN

	char	ip_time_to_live;	/* Tempo de vida do datagrama */
	char	ip_proto;		/* Protocolo utilizado nos dados */
	ushort	ip_h_checksum;		/* Verificação do cabeçalho */

	IPADDR	ip_src_addr;		/* Endereço do remetente */
	IPADDR	ip_dst_addr;		/* Endereço do destino */

	char	ip_extension[20];	/* Reservado para o cabeçalho completo */

}	IP_H;

#define MIN_IP_H_SZ	(sizeof (IP_H) - sizeof (((IP_H *)0)->ip_extension))

/*
 ****************************************************************
 *	Definições relativas ao protocolo ICMP			*
 ****************************************************************
 */

/*
 ******	Cabeçalho do datagrama ICMP *****************************
 */
typedef	struct
{
	uchar	ih_type;	/* Tipo do datagrama */
	uchar	ih_code;	/* Código do datagrama */
	ushort	ih_checksum;	/* Verificação */

  union
  {
	IPADDR	ih_addr;	/* Endereço */

     struct
     {
	ushort	ih_id;		/* Identificador */
	ushort	ih_seq;		/* Nr. de seqüência */

     }	s;

  }	u;

}	ICMP_H;

/*
 ******	Números dos tipos das mensagens ICMP ********************
 */
typedef	enum
{
	ICMP_ECHO_REPLY	= 0,
	ICMP_DST_UNREACH = 3,
	ICMP_SRC_QUENCH,
	ICMP_REDIRECT,
	ICMP_ECHO_REQ = 8,
	ICMP_TIME_EXCEEDED = 11,
	ICMP_PARAM_PROBLEM,
	ICMP_TIME_STAMP_REQ,
	ICMP_TIME_STAMP_REPLY,
	ICMP_INFO_REQ,
	ICMP_INFO_REPLY

}	ICMP_MSG_TYPE_ENUM;

typedef	enum
{
	ICMP_NET_UNREACH = 0,
	ICMP_HOST_UNREACH,
	ICMP_PROTOCOL_UNREACH,
	ICMP_PORT_UNREACH,
	ICMP_FRAG_NEEDED,
	ICMP_SOURCE_ROUTE_FAILED

}	ICMP_UNREACH_CODE_ENUM;

/*
 ****************************************************************
 *	Definições relativas ao protocolo RAW			*
 ****************************************************************
 */

/*
 ******	Endpoint do datagrama RAW *******************************
 */
#define	NO_RAW_EP	(RAW_EP *)NULL

struct	raw_ep
{
	LOCK	rp_lock;	/* LOCK do endpoint */
	EVENT	rp_inq_nempty;	/* Fila de entrada não vazia */
	LOCK	rp_inq_lock;	/* SPINLOCK da fila de entrada */

	int	rp_bind_proto;	/* Protocolo dado no "t_bind" */
	int	rp_snd_proto;	/* Protocolo do datagrama a enviar */
	int	rp_rcv_proto;	/* Protocolo do datagrama recebido */

	IPADDR	rp_my_addr;	/* Endereço do endpoint (do último snd) */

	IPADDR	rp_bind_addr;	/* Endereço remoto dado no "t_bind" */
	IPADDR	rp_snd_addr;	/* Endereço remoto do datagrama a enviar */
	IPADDR	rp_rcv_addr;	/* Endereço do último datagrama recebido */

	int	rp_state;	/* Estado do endpoint */

	ITBLOCK	*rp_inq_first;	/* Fila de entrada */
	ITBLOCK	*rp_inq_last;

	int	rp_flag;	/* Reservado para futuros indicadores */

	char	rp_attention_set; /* Execute o procedimento de ATTENTION */

	UPROC	*rp_uproc;	/* Processo para o ATTENTION */
	int	rp_fd_index;	/* Índice/descritor (ATTENTION) */

	long	rp_reser1[2];

	RAW_EP	*rp_next;	/* Lista de RAW EPs */
};

/*
 ****************************************************************
 *	Definições relativas ao protocolo UDP			*
 ****************************************************************
 */

/*
 ******	Endpoint do datagrama UDP *******************************
 */
#define	NO_UDP_EP	(UDP_EP *)NULL

struct	udp_ep
{
	LOCK	up_lock;	/* LOCK do endpoint */
	EVENT	up_inq_nempty;	/* Fila de entrada não vazia */
	LOCK	up_inq_lock;	/* SPINLOCK da fila de entrada */

	int	up_bind_port;	/* Port remoto dado no "t_bind" */
	int	up_snd_port;	/* Port remoto do datagrama a enviar */
	int	up_rcv_port;	/* Port remoto do datagrama recebido */

	int	up_my_port;	/* Port local do endpoint */
	IPADDR	up_my_addr;	/* Endereço local do endpoint (do último snd) */

	IPADDR	up_bind_addr;	/* Endereço remoto dado no "t_bind" */
	IPADDR	up_snd_addr;	/* Endereço remoto do datagrama a enviar */
	IPADDR	up_rcv_addr;	/* Endereço do último datagrama recebido */

	int	up_state;	/* Estado do endpoint */

	ITBLOCK	*up_inq_first;	/* Fila de entrada */
	ITBLOCK	*up_inq_last;

	int	up_flag;	/* Reservado para futuros indicadores */

	char	up_attention_set; /* Execute o procedimento de ATTENTION */

	UPROC	*up_uproc;	/* Processo para o ATTENTION */
	int	up_fd_index;	/* Índice/descritor (ATTENTION) */

	long	up_reser1[2];

	UDP_EP	*up_next;	/* Lista de UDP EPs */
};

/*
 ******	Cabeçalho do datagrama UDP ******************************
 */
typedef	struct
{
	ushort	uh_src_port;	/* "Port" do remetente */
	ushort	uh_dst_port;	/* "Port" do destinatário */
	ushort	uh_size;	/* Tamanho da mensagem */
	ushort	uh_checksum;	/* Verificação */

}	UDP_H;

/*
 ******	Pseudo-cabeçalho UDP para cálculo de checksum ***********
 */
typedef	struct
{
	IPADDR	ph_src_addr;	/* Endereço do destinatário */
	IPADDR	ph_dst_addr;	/* Endereço do destinatário */
	ushort	ph_proto;	/* Protocolo de transporte utilizado */
	ushort	ph_size;	/* Tamanho da mensagem de transporte */

}	PSD_H;

/*
 ****** O Pseudo-cabeçalho e cabeçalho efetivo reunidos *********
 */
typedef	struct
{
	PSD_H	ph;		/* O pseudo-cabeçalho */ 
	UDP_H	uh;		/* O cabeçalho "de verdade" */ 

}	UT_H;

/*
 ****************************************************************
 *	Definições relativas ao protocolo TCP			*
 ****************************************************************
 */
#define	MAX_SGSZ (DGSZ - sizeof (IP_H) - sizeof (TCP_H))

/*
 ******	Comparação circular *************************************
 */
#define	TCP_LT(a, b)	((int)(n = (a) - (b)) < 0)
#define	TCP_LE(a, b)	((int)(n = (a) - (b)) <= 0)
#define	TCP_GT(a, b)	((int)(n = (a) - (b)) > 0)
#define	TCP_GE(a, b)	((int)(n = (a) - (b)) >= 0)

#define	TCP_GTR(a, b, r) ((int)((r) = (a) - (b)) > 0)

/*
 ******	Estados da conexão **************************************
 */
typedef enum
{
	S_CLOSED = 1,	/* Não há EP */
	S_UNBOUND,	/* Esperando para atribuir um "port" */
	S_BOUND,	/* Já foi atribuído um "port" */
	S_LISTEN,	/* Esperando por pedido de conexão remota */
	S_SYN_SENT,	/* Esperando pedido de conexão após ter emitido um */
	S_SYN_RECEIVED,	/* Esperando ACK após ter emitido e recebido SYN */
	S_ESTABLISHED,	/* Fase de transferência de dados */
	S_FIN_WAIT_1,	/* Esperando pedido de término ou ACK */
	S_FIN_WAIT_2,	/* Esperando pedido de término */
	S_CLOSE_WAIT,	/* Esperando pedido de término do usuário local */
	S_CLOSING,	/* Esperando ACK de término */
	S_LAST_ACK,	/* Esperando ACK do pedido de término */
	S_TIME_WAIT	/* Esperando o tempo da estação remota receber o ACK */

}	S_STATE;

/*
 ******	Endpoint do datagrama TCP *******************************
 */
#define	NO_TCP_EP	(TCP_EP *)NULL

typedef struct			/* Estrutura do LISTEN */
{
	int	tp_listen_seq;	/* No. de seqüência */
	IPADDR	tp_listen_addr;	/* Endereço do remetente */
	int	tp_listen_port;	/* Porta do remetente */
	IPADDR	tp_listen_my_addr; /* o meu Endereço */
	seq_t	tp_listen_irs;	/* No. de sequência do SYN */
	int	tp_max_seg_sz;	/* Tamanho máximo do segmento */
	int	tp_window_size;	/* Tamanho da janela */
	time_t	tp_listen_time;	/* Chegada do SYN */

}	LISTENQ;

#define	LISTENQ_MAXSZ	8

typedef struct			/* Estrutura da área circular */
{
	int	tp_rnd_sz;	/* Tamanho de cada área circular de E/S (em bytes) */
	int	tp_rnd_count;   /* Tamanho em uso da área circular de E/S */
	char	*tp_rnd_begin;  /* Endereço da área circular de E/S */
	char	*tp_rnd_end;	/* Final da área circular de E/S */
	char	*tp_rnd_head;   /* Endereço da área circular de E/S */
	char	*tp_rnd_tail;   /* Final da área circular de E/S */

	LOCK	tp_rnd_lock;	/* Semáforo da área */
	EVENT	tp_rnd_nfull;	/* Semáforo de área não cheia */
	EVENT	tp_rnd_nempty;	/* Semáforo de área não cheia */

	char	tp_rnd_syn;	/* Há um SYN na área */
	char	tp_rnd_fin;	/* Há um FIN na área */

}	RND;

struct	tcp_ep
{
	LOCK	tp_lock;		/* LOCK do endpoint */
	LOCK	tp_inq_lock;		/* SPINLOCK da fila de entrada */
	EVENT	tp_inq_nempty;		/* Fila de entrada não vazia */
	EVENT	tp_good_snd_wnd;	/* Janela de transmissão suficiente */

	LOCK	tp_rnd_lock;		/* SPINLOCK de alocação das áreas circulares */
	char	tp_pipe_mode;		/* Usa a otimização interna */
	EVENT	tp_pipe_event;		/* Para esperar o estado ESTABLISHED */

	int	tp_my_port;		/* Port do endpoint */
	IPADDR	tp_my_addr;		/* Endereço do remetente */

	IPADDR	tp_dst_addr;		/* Endereço do destinatário */
	int	tp_dst_port;		/* Port do destinatário */

	ROUTE	*tp_route;		/* Entrada de roteamento */
	S_STATE	tp_state;		/* Estado do endpoint */

	int	tp_SRTT;		/* Smoothed round trip time */
	int	tp_max_seg_sz;		/* Tamanho máximo do segmento */
	int	tp_good_wnd;		/* Janela boa para transmitir */
	TCP_EP	*tp_pipe_tcp_ep;	/* Endpoint destino para modo "pipe" */

	/* Transmissão */

	seq_t	tp_snd_iss;		/* Endereço inicial */
	seq_t	tp_snd_una;		/* Começo da parte enviada ainda sem ACK */
	seq_t	tp_snd_nxt;		/* Começo da parte a enviar */
	seq_t	tp_snd_wnd;		/* Final da parte permitida para o envio */
	seq_t	tp_snd_up;		/* Ponteiro de urgência */
	seq_t	tp_snd_psh;		/* Ponteiro de "push" */
	seq_t	tp_snd_fin;		/* No. de seq do FIN */

	/* Recepção */

	seq_t	tp_rcv_irs;		/* Endereço inicial */
	seq_t	tp_rcv_usr;		/* Começo da parte do usuário */
	seq_t	tp_rcv_nxt;		/* Começo da parte a receber */
	seq_t	tp_rcv_wnd;		/* Final da parte permitida na recepção */
	seq_t	tp_old_wnd;		/* Última janela enviada */
	seq_t	tp_rcv_up;		/* Ponteiro de urgência */
	seq_t	tp_rcv_psh;		/* Ponteiro de "push" */

	/* Fila de entrada */

	ITBLOCK	*tp_inq_first;		/* Primeiro elemento da fila */
	ITBLOCK	*tp_inq_last;		/* Último elemento da fila */

	/* Vários indicadores de EVENTOS ocorridos */
 
	char	tp_listen;		/* Chegou um pedido de conexão (SYN) */
	char	tp_connect;		/* Chegou uma confirmação de conexão */
	char	tp_data;		/* Chegaram dados normais */
	char	tp_urgdata;		/* Chegaram dados urgentes */
	char	tp_rst;			/* Chegou um segmento com RST (disconnect) */
	char	tp_uderr;		/* Erro de transmissão UDP */
	char	tp_fin;			/* Chegou um segmento com FIN (ordrel)  */
	char	tp_godata;		/* Possível enviar dados novamente */
	char	tp_gourgdata;		/* Possível enviar dados urgentes novamente */
	char	tp_timeout;		/* O limite de tempo se esgotou */
	char	tp_reser0[2];

	/* Variáveis do attention */

	UPROC	*tp_uproc;		/* Processo para o ATTENTION */
	int	tp_fd_index;		/* Índice/descritor (ATTENTION) */
	char	tp_attention_set;	/* Execute o procedimento de ATTENTION */

	/* Ponteiro da fila de entrada */

	char	tp_inq_present;		/* Já na fila de ACKs */
	INQ_PTR	tp_inq_forw;		/* Elemento seguinte da fila */

	/* Tempos */

	int	tp_max_wait;		/* Tempo máximo permitido sem conexão */
	int	tp_max_silence;		/* Tempo máximo permitido sem entrada/saída */

	time_t	tp_last_rcv_time;	/* Tempo da última entrada */
	time_t	tp_last_snd_time;	/* Tempo da última transmissão */
	time_t	tp_last_ack_time;	/* Tempo do último ACK efetivo */
	int	tp_retransmitted;	/* No. de retransmissões */
	time_t	tp_closed_wnd_time;	/* Tempo em que a janela fechou */
	time_t	tp_outq_time;		/* Para o "daemon" */

	/* Fila de LISTEN */

	int	tp_listen_maxqlen;	/* Tamanho máximo da fila */
	int	tp_listen_qlen;		/* Tamanho atual da fila */
	int	tp_listen_source;	/* Fonte dos Nos. de seqüência */

	LISTENQ	tp_listen_q[LISTENQ_MAXSZ]; /* Fila de LISTEN */

	/* Áreas circulares de E/S */

	pg_t	tp_rnd_addr;		/* Endereço da área circular de E/S */
	pg_t	tp_rnd_size;		/* Tamanho da área circular de E/S */

	RND	tp_rnd_in;		/* Área circular de entrada */
	RND	tp_rnd_out;		/* Área circular de saída */

	int	tp_max_client_conn;	/* Número máximo de conexões por cliente */

	int	tp_reser2[14];

	TCP_EP	*tp_next;	/* Lista de TCP EPs */
};

/*
 ******	Cabeçalho do datagrama TCP ******************************
 */
typedef	struct
{
	ushort	th_src_port;	/* "Port" do remetente */
	ushort	th_dst_port;	/* "Port" do destinatário */
	ulong	th_seq_no;	/* Posição dos dados de saída */
	ulong	th_ack_no;	/* Posição dos dados recebidos */

#ifdef	LITTLE_ENDIAN
	bit	th_reser0 : 4;
	bit	th_h_size : 4;	/* Tamanho do cabeçalho em longs */
#else
	bit	th_h_size : 4;	/* Tamanho do cabeçalho em longs */
	bit	th_reser0 : 4;
#endif	LITTLE_ENDIAN

	char	th_ctl;		/* Indicadores de controle */
	ushort	th_window;	/* Quantidade de dados que aceita */

	ushort	th_checksum;	/* Verificação */
	ushort	th_urg_ptr;	/* Ponteiro para dados urgentes */

    struct
    {
	char	th_opt_kind;	/* Tipo da opção */
	char	th_opt_size;	/* Tamanho da opção */
	ushort	th_max_seg_sz;	/* Tamanho máximo do segmento */

    }	opt;

}	TCP_H;

#define MIN_TCP_H_SZ	(sizeof (TCP_H) - sizeof (((TCP_H *)0)->opt))

/*
 ****** Indicadores de controle *********************************
 */
#define	C_TST_BYTE	0x1000	/* Enviando byte de teste de janela */
#define	C_RETRANS	0x0800	/* Retransmissão (não usado no cabeçalho TCP) */
#define	C_ACK_of_FIN 	0x0400	/* ACK do FIN (não usado no cabeçalho TCP) */
#define	C_ACK_of_SYN 	0x0200	/* ACK do SYN (não usado no cabeçalho TCP) */
#define	C_DATA		0x0100	/* Dados (não usado no cabeçalho TCP) */

#define	C_URG		0x0020	/* Ponteiro urgente é válido */
#define	C_ACK		0x0010	/* Campo de "ack" é válido */
#define	C_PSH		0x0008	/* É solicitado um "push" */
#define	C_RST		0x0004	/* Reset the connection */
#define	C_SYN		0x0002	/* Início de protocolo */
#define	C_FIN		0x0001	/* Final de protocolo */

/*
 ****** O Pseudo-cabeçalho e cabeçalho efetivo reunidos *********
 */
typedef	struct
{
	PSD_H	ph;		/* O pseudo-cabeçalho (igual ao do UDP) */ 
	TCP_H	th;		/* O cabeçalho "de verdade" */ 

}	TT_H;

/*
 ****************************************************************
 *	Definições relativas ao protocolo DHCP			*
 ****************************************************************
 */
#define	DHCP_SERVER_PORT	67	/* Port para enviar  pacotes ao servidor */
#define	DHCP_CLIENT_PORT	68	/* Port para receber pacotes do servidor */

/*
 ****************************************************************
 *	Definição dos IOCTLs das funções do XTI			*
 ****************************************************************
 */
#define	I_ACCEPT	(('I' << 8) |  1)
#define	I_ALLOC		(('I' << 8) |  2)
#define	I_BIND		(('I' << 8) |  3)
#define	I_CLOSE		(('I' << 8) |  4)
#define	I_CONNECT	(('I' << 8) |  5)
#define	I_ERROR		(('I' << 8) |  6)
#define	I_FREE		(('I' << 8) |  7)
#define	I_GETINFO	(('I' << 8) |  8)
#define	I_GETSTATE	(('I' << 8) |  9)
#define	I_LISTEN	(('I' << 8) | 10)
#define	I_LOOK		(('I' << 8) | 11)
#define	I_OPEN		(('I' << 8) | 12)
#define	I_OPTMGMT	(('I' << 8) | 13)
#define	I_RCV		(('I' << 8) | 14)
#define	I_RCVCONNECT	(('I' << 8) | 15)
#define	I_RCVDIS	(('I' << 8) | 16)
#define	I_RCVREL	(('I' << 8) | 17)
#define	I_RCVUDATA	(('I' << 8) | 18)
#define	I_RCVUDERR	(('I' << 8) | 19)
#define	I_SND		(('I' << 8) | 20)
#define	I_SNDDIS	(('I' << 8) | 21)
#define	I_SNDREL	(('I' << 8) | 22)
#define	I_SNDUDATA	(('I' << 8) | 23)
#define	I_SYNC		(('I' << 8) | 24)
#define	I_UNBIND	(('I' << 8) | 25)

#define	I_PUSH		(('I' << 8) | 30)
#define	I_NREAD		(('I' << 8) | 31)
#define	I_GETADDR	(('I' << 8) | 32)
#define	I_RCVRAW	(('I' << 8) | 33)
#define	I_SNDRAW	(('I' << 8) | 34)

#define	I_GET_EVENT_STR	(('I' << 8) | 40)
#define	I_ADDR_TO_NODE	(('I' << 8) | 41)
#define	I_NODE_TO_ADDR	(('I' << 8) | 42)
#define	I_MAIL_TO_NODE	(('I' << 8) | 43)
#define	I_ADDR_TO_NAME	(('I' << 8) | 44)

/*
 ****************************************************************
 *	Outras definição de IOCTLs				*
 ****************************************************************
 */
enum {	DNS_SERVER_CNT		= 8 };

enum
{
	I_IN_DAEMON		= (('I' << 8) | 49),
	I_OUT_DAEMON		= (('I' << 8) | 50),
	I_NFS_DAEMON		= (('I' << 8) | 51),
	I_GET_ITSCB		= (('I' << 8) | 52),
	I_PUT_ITSCB		= (('I' << 8) | 53),
	I_GET_DNS		= (('I' << 8) | 54),
	I_GET_ROUTE		= (('I' << 8) | 55),
	I_GET_ETHER		= (('I' << 8) | 56),
	I_GET_EXPORT		= (('I' << 8) | 57),
	I_PUT_EXPORT		= (('I' << 8) | 58),
	I_GET_NFS_MOUNT		= (('I' << 8) | 59),
	I_GET_TCP_EP		= (('I' << 8) | 60),
	I_GET_TCP_SORTQ		= (('I' << 8) | 61),
	I_GET_TCP_INQ		= (('I' << 8) | 62),
	I_GET_TCP_OUTQ		= (('I' << 8) | 63),
	I_GET_TCP_INCOM		= (('I' << 8) | 64),
	I_GET_TCP_LISQ		= (('I' << 8) | 65),
	I_GET_RAW_INQ		= (('I' << 8) | 66),
	I_GET_UDP_INQ		= (('I' << 8) | 67),

	I_DNS_WAIT_REQ		= (('I' << 8) | 71),
	I_DNS_PUT_INFO		= (('I' << 8) | 72),

	I_GET_DNS_SERVER	= (('I' << 8) | 80),
	I_PUT_DNS_SERVER	= (('I' << 8) | 81)
};
