/*
 ****************************************************************
 *								*
 *			scb.c					*
 *								*
 *	Prepara o SCB - bloco de controle do sistema		*
 *								*
 *	Versão	3.0.0, de 05.09.94				*
 *		4.9.0, de 28.07.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/bcb.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/default.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/kfile.h"
#include "../h/tty.h"
#include "../h/map.h"
#include "../h/mmu.h"
#include "../h/a.out.h"
#include "../h/timeout.h"
#include "../h/lockf.h"
#include "../h/ipc.h"
#include "../h/disktb.h"
#include "../h/devmajor.h"
#include "../h/kcntl.h"
#include "../h/itnet.h"
#include "../h/video.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define BOOT_WAIT (15 * 500)	/* Tempo de espera em alguma unidade (ms?) */

entry int	TTYMAXQ;		/* Limite para parar o processo */
entry int	TTYMINQ;		/* Limite para reiniciar o processo */
entry int	TTYPANIC;		/* Limite para abandonar toda a fila */
entry int	TTYCOMMPANIC;		/* Idem, para o modo de comunicações */

entry char	scbscreen = 'a';	/* Tela escolhida */

entry void	*tst_end_base_bss;	/* Final (teste) da área baixa alocada */
entry void	*tst_end_ext_bss;	/* Final (teste) da área alta alocada */

entry void	*given_endpmem;		/* Final da memória física (dado) */
entry void	*real_endpmem;		/* Final da memória física (real) */

entry int	alloc_status;		/* Retorno da última allocaçãoª*/

entry char	libt_nm[32] = "/lib/libt.o"; /* Nome da biblioteca "libt.o" */

entry char	CSW;			/* Código da depuração (para ASM) */

entry DISKTB	*ramd_pp;		/* As entradas dos RAMD */

/*
 ****** Algumas macros ******************************************
 */
#define	FATOR(i)  (i*15/10)	/* Fator de Carga do HASH */

#define	ROUND(i,mod) 	((void *)(((unsigned)i + mod - 1) & ~(mod - 1)))

#define	VIRTUALTOKB(i) 	(i == NOVOID ? 0 : (VIRT_TO_PHYS_ADDR (i) >> KBSHIFT))

#define	VIRTUALPGTOKB(i) ((i - (SYS_ADDR >> PGSHIFT)) << (PGSHIFT-KBSHIFT))

/*
 ****** Estrutura das opções do dialogo do SCB ******************
 */
typedef	struct
{
	char	s_name[11];	/* Nome da variável */
	char	s_sizeof;	/* Tamanho (em bytes) da variável */
	void	*s_ptr;		/* Ponteiro para a variável */

}	SCBOPT;

#define	SZ_PTR(member)	sizeof (member), (void *)(&member)

/*
 ******	Opções do dialogo do SCB, tela A ************************
 */
const SCBOPT	scbopta[] =
{
	"nproc",	SZ_PTR (scb.y_nproc),
	"ninode",	SZ_PTR (scb.y_ninode),
	"nbuf",		SZ_PTR (scb.y_nbuf),
	"nkfile",	SZ_PTR (scb.y_nkfile),
	"nclist",	SZ_PTR (scb.y_nclist),
	"nmap",		SZ_PTR (scb.y_nmap),
	"nhshlib",	SZ_PTR (scb.y_n_shlib_hash),
	"ntout",	SZ_PTR (scb.y_ntout),
	"npty",		SZ_PTR (scb.y_npty),
	"ncon",		SZ_PTR (scb.y_NCON),

	""
};

/*
 ******	Opções do dialogo do SCB, tela B ************************
 */
const SCBOPT	scboptb[] =
{
	"nlockf",	SZ_PTR (scb.y_nlockf),
	"neventl",	SZ_PTR (scb.y_nueventl),
	"neventg",	SZ_PTR (scb.y_nueventg),
	"nsemal",	SZ_PTR (scb.y_nusemal),
	"nsemag",	SZ_PTR (scb.y_nusemag),
	"nregionl",	SZ_PTR (scb.y_nregionl),
	"nregiong",	SZ_PTR (scb.y_nregiong),
	"ndns",		SZ_PTR (scb.y_n_dns),
	"nroute",	SZ_PTR (scb.y_n_route),
	"nether",	SZ_PTR (scb.y_n_ether),
	"nexport",	SZ_PTR (scb.y_n_export),
	"nitblock",	SZ_PTR (scb.y_n_itblock),
	"nrawep",	SZ_PTR (scb.y_n_raw_ep),
	"nudpep",	SZ_PTR (scb.y_n_udp_ep),
	"ntcpep",	SZ_PTR (scb.y_n_tcp_ep),

	""
};

/*
 ******	Opções do dialogo do SCB, tela C ************************
 */
const SCBOPT	scboptc [] =
{
	"rootdev",	SZ_PTR (scb.y_rootdev),
	"root",		SZ_PTR (scb.y_rootdev),
#if (0)	/*******************************************************/
	"pipedev",	SZ_PTR (scb.y_pipedev),
	"pipe",		SZ_PTR (scb.y_pipedev),
#endif	/*******************************************************/
	"swapdev",	SZ_PTR (scb.y_swapdev),
	"swap",		SZ_PTR (scb.y_swapdev),
	"swaplow",	SZ_PTR (scb.y_swaplow),
	"nswap",	SZ_PTR (scb.y_nswap),

	"endpmem",	SZ_PTR (given_endpmem),
	"ramdsz",	SZ_PTR (scb.y_ramd0sz),
	"ramd0sz",	SZ_PTR (scb.y_ramd0sz),
	"ramd1sz",	SZ_PTR (scb.y_ramd1sz),

	"preempt",	SZ_PTR (scb.y_preemption_mask[0]),
	"debug",	SZ_PTR (scb.y_CSW),
	"verbose",	SZ_PTR (scb.y_boot_verbose),

	"key",		SZ_PTR (scb.y_keyboard),

	""
};

/*
 ******	Opções do dialogo do SCB, tela D ************************
 */
const SCBOPT	scboptd [] =
{
	"tslice",	SZ_PTR (scb.y_tslice),
	"incpri",	SZ_PTR (scb.y_incpri),
	"decpri",	SZ_PTR (scb.y_decpri),
	"ttyinc",	SZ_PTR (scb.y_ttyinc),
	"nfactor",	SZ_PTR (scb.y_nfactor),

	"maxppu",	SZ_PTR (scb.y_maxppu),
	"maxpsz",	SZ_PTR (scb.y_maxpsz),
	"umask",	SZ_PTR (scb.y_umask),
	"ncexec",	SZ_PTR (scb.y_ncexec),
	"stksz",	SZ_PTR (scb.y_stksz),
	"stkincr",	SZ_PTR (scb.y_stkincr),

	"nmount",	SZ_PTR (scb.y_nmount),
	"hz",		SZ_PTR (scb.y_hz),
	"tag",		SZ_PTR (scb.y_tagenable),
	"tagenable",	SZ_PTR (scb.y_tagenable),
	"dma",		SZ_PTR (scb.y_dma_enable),
	"dmaenable",	SZ_PTR (scb.y_dma_enable),

	""
};

/*
 ******	Opções do dialogo do SCB, tela E ************************
 */
const SCBOPT	scbopte [] =
{
	"sioport0",	SZ_PTR (scb.y_sio_port[0]),
	"sioport1",	SZ_PTR (scb.y_sio_port[1]),
	"sioport2",	SZ_PTR (scb.y_sio_port[2]),
	"sioport3",	SZ_PTR (scb.y_sio_port[3]),
	"sioport4",	SZ_PTR (scb.y_sio_port[4]),
	"sioport5",	SZ_PTR (scb.y_sio_port[5]),
	"sioport6",	SZ_PTR (scb.y_sio_port[6]),
	"sioport7",	SZ_PTR (scb.y_sio_port[7]),

	"sioirq0",	SZ_PTR (scb.y_sio_irq[0]),
	"sioirq1",	SZ_PTR (scb.y_sio_irq[1]),
	"sioirq2",	SZ_PTR (scb.y_sio_irq[2]),
	"sioirq3",	SZ_PTR (scb.y_sio_irq[3]),
	"sioirq4",	SZ_PTR (scb.y_sio_irq[4]),
	"sioirq5",	SZ_PTR (scb.y_sio_irq[5]),
	"sioirq6",	SZ_PTR (scb.y_sio_irq[6]),
	"sioirq7",	SZ_PTR (scb.y_sio_irq[7]),

	"lpport0",	SZ_PTR (scb.y_lp_port[0]),
	"lpport1",	SZ_PTR (scb.y_lp_port[1]),
	"lpport2",	SZ_PTR (scb.y_lp_port[2]),
	"lpport3",	SZ_PTR (scb.y_lp_port[3]),

	"lpirq0",	SZ_PTR (scb.y_lp_irq[0]),
	"lpirq1",	SZ_PTR (scb.y_lp_irq[1]),
	"lpirq2",	SZ_PTR (scb.y_lp_irq[2]),
	"lpirq3",	SZ_PTR (scb.y_lp_irq[3]),

	"edport0",	SZ_PTR (scb.y_ed_port[0]),
	"edport1",	SZ_PTR (scb.y_ed_port[1]),
	"edport2",	SZ_PTR (scb.y_ed_port[2]),
	"edport3",	SZ_PTR (scb.y_ed_port[3]),

	"edirq0",	SZ_PTR (scb.y_ed_irq[0]),
	"edirq1",	SZ_PTR (scb.y_ed_irq[1]),
	"edirq2",	SZ_PTR (scb.y_ed_irq[2]),
	"edirq3",	SZ_PTR (scb.y_ed_irq[3]),

	"usb0",		SZ_PTR (scb.y_usb_enable[0]),
	"usb1",		SZ_PTR (scb.y_usb_enable[1]),
	"usb2",		SZ_PTR (scb.y_usb_enable[2]),
	"usb3",		SZ_PTR (scb.y_usb_enable[3]),
	"usb4",		SZ_PTR (scb.y_usb_enable[4]),
	"usb5",		SZ_PTR (scb.y_usb_enable[5]),
	"usb6",		SZ_PTR (scb.y_usb_enable[6]),
	"usb7",		SZ_PTR (scb.y_usb_enable[7]),

	"usb8",		SZ_PTR (scb.y_usb_enable[8]),
	"usb9",		SZ_PTR (scb.y_usb_enable[9]),
	"usb10",	SZ_PTR (scb.y_usb_enable[10]),
	"usb11",	SZ_PTR (scb.y_usb_enable[11]),
	"usb12",	SZ_PTR (scb.y_usb_enable[12]),
	"usb13",	SZ_PTR (scb.y_usb_enable[13]),
	"usb14",	SZ_PTR (scb.y_usb_enable[14]),
	"usb15",	SZ_PTR (scb.y_usb_enable[15]),

	""
};

/*
 ****** Tabela de tipos de sistemas de arquivos *****************
 */
const PARTNM	partnm[] =
{
	0x00,	"",		/* Vazio */
	0x01,	"DOS FAT12",
	0x02,	"XENIX root",
	0x03,	"XENIX usr",
	0x04,	"DOS FAT16 < 32 MB",
	0x05,	"DOS Extended",
	0x06,	"DOS FAT16 >= 32 MB",
	0x07,	"NTFS",
	0x08,	"AIX",
	0x09,	"AIX bootable",
	0x0A,	"OPUS",
	0x0B,	"DOS FAT32",
	0x0C,	"DOS FAT32 (L)",
	0x0E,	"DOS FAT16 (L)",
	0x0F,	"DOS Extended (L)",
	0x40,	"Venix 80286",
	0x4F,	"QNX",
	0x51,	"Novell (?)",
	0x52,	"Microport",
	0x63,	"GNU HURD",
	0x64,	"Novell",
	0x75,	"PC/IX",
	0x80,	"old MINIX",
	0x81,	"LINUX/MINIX",
	0x82,	"LINUX swap",
	0x83,	"LINUX ext",
	0x93,	"Amoeba",
	0x94,	"Amoeba BBT",
	0xA5,	"BSD 4.2",
	0xA8,	"TROPIX V7",
	0xA9,	"TROPIX T1",
	0xAA,	"TROPIX fs3",
	0xAE,	"TROPIX Extended",
	0xAF,	"TROPIX swap",
	0xB7,	"BSDI fs",
	0xB8,	"BSDI swap",
	0xC7,	"Syrinx",
	0xDB,	"CP/M",
	0xE1,	"DOS access",
	0xE3,	"DOS R/O",
	0xF2,	"DOS Secondary",
	0xFF,	"BBT",

	-1,	"???"		/* Final da tabela */
};

/*
 ****** Protótipos *********************************************
 */
void		dev_setup (void);
int		alloc_all_tb (void);
void		alloc_tb (long, int, void *, void *);
void		scb_hashset (void);
int		next_power (int);
void		scb_a_display (void);
void		scb_b_display (void);
void		scb_c_display (void);
void		scb_d_display (void);
void		scb_e_display (void);
void		scb_f_display (void);
void		scb_g_display (void);
void		scb_print_total (void);
void		scb_prdev (dev_t);
void		scb_dialog (void);

/*
 ****************************************************************
 *	Definição dos "defaults" do SCB				*
 ****************************************************************
 */
SCB	scb	=
{
	/*
	 *	Informações sobre as CPUs
	 */
	NULL,			/* Tipo da CPU (386, 486 ou 586) */
	NULL,			/* No. de CPUs ativas */
	NULL,			/* Pid do INIT */

	/*
	 *	Tabela de Processos
	 */
	NOUPROCV,		/* Início da tabela */
	NOUPROCV,		/* Ultima entrada usada */
	NOUPROCV,		/* Final da tabela */
	NPROC,			/* No. de entradas */

	/*
	 *	Tabela de HASH dos Processos
	 */
	(PHASHTB *)NULL,	/* Início da tabela */
	(PHASHTB *)NULL,	/* Final da tabela */
	NULL,			/* No. de entradas */
	NULL,			/* Mascara para o cálculo do Hash */

	/*
	 *	Tabela de INODEs
	 */
	NOINODE,		/* Começo da tabela */
	NOINODE,		/* Final da tabela */
	NINODE,			/* No. de entradas */

	/*
	 *	Tabela de HASH dos INODEs
	 */
	(IHASHTB *)NULL,	/* Início da tabela */
	(IHASHTB *)NULL,	/* Final da tabela */
	NULL,			/* No. de entradas */

	/*
	 *	Tabela de BUFFERs e BUFHEADs
	 */
	NOBHEAD,		/* Início dos BHEADs */
	NOBHEAD,		/* Final  dos BHEADs */
	NOVOID,			/* Início do Buffers 0 */
	NOVOID,			/* Final do Buffers 0 */
	NOVOID,			/* Início do Buffers 1 */
	NOVOID,			/* Final do Buffers 1 */
	NBUF,			/* No. de BUFFERs    */

	/*
	 *	Tabela de HASH dos BHEADs
	 */
	(BHASHTB *)NULL,	/* Início da tabela */
	(BHASHTB *)NULL,	/* Final da tabela */
	NULL,			/* No. de entradas */

	/*
	 *	Tabela de KFILEs
	 */
	NOKFILE,		/* Começo da tabela */
	NOKFILE,		/* Final da tabela */
	NKFILE,			/* No. de entradas */

	/*
	 *	Tabela de CLISTs
	 */
	NOCBLK,			/* Começo da tabela */
	NOCBLK,			/* Final da tabela */
	NCLIST,			/* No. de entradas */

	/*
	 *	Tabela de Alocação da Memoria Interna e do disco (swap)
	 */
	(MAP *)NULL,		/* Início da MAP */
	(MAP *)NULL,		/* Final  da MAP */
	NMAP,			/* No. de entradas */

	/*
	 *	Tabela de HASH dos símbolos das bibliotecas compartilhadas
	 */
	(void *)NULL, 		/* Início da SHLIB_HASH */
	(void *)NULL,		/* Final da SHLIB_HASH */
	NHSHLIB,		/* No. de entrada da SHLIB_HASH */

	/*
	 *	Tabela de TIMEOUT
	 */
	(TIMEOUT *)NULL,	/* Início da tabela */
	(TIMEOUT *)NULL,	/* Final da tabela */
	NTOUT,			/* No. de entradas por CPU */

	/*
	 *	Tabela de LOCKF
	 */
	(LOCKF *)NULL,		/* Início da tabela */
	(LOCKF *)NULL,		/* Final da tabela */
	NLOCKF,			/* No. de entradas */

	/*
	 *	Tabela de UEVENTL
	 */
	(UEVENTL *)NULL,	/* Início da tabela */
	(UEVENTL *)NULL,	/* Final da tabela */
	NUEVENTL,		/* No. de entradas */

	/*
	 *	Tabela de UEVENTG
	 */
	(UEVENTG *)NULL,	/* Início da tabela */
	(UEVENTG *)NULL,	/* Final da tabela */
	NUEVENTG,		/* No. de entradas */

	/*
	 *	Tabela de USEMAL
	 */
	(USEMAL *)NULL,		/* Início da tabela */
	(USEMAL *)NULL,		/* Final da tabela */
	NUSEMAL,		/* No. de entradas */

	/*
	 *	Tabela de USEMAG
	 */
	(USEMAG *)NULL,		/* Início da tabela */
	(USEMAG *)NULL,		/* Final da tabela */
	NUSEMAG,		/* No. de entradas */

	/*
	 *	Tabela de REGIONL
	 */
	(REGIONL *)NULL,	/* Início da tabela */
	(REGIONL *)NULL,	/* Final da tabela */
	NREGIONL,		/* No. de entradas */

	/*
	 *	Tabela de REGIONG
	 */
	(REGIONG *)NULL,	/* Início da tabela */
	(REGIONG *)NULL,	/* Final da tabela */
	NREGIONG,		/* No. de entradas */

	/*
	 *	Tabela do DOMAIN NAME SYSTEM
	 */
	(DNS *)NULL,		/* Começo da tabela */
	(DNS *)NULL,		/* Final da tabela */
	N_DNS,			/* No. de entradas */

	/*
	 *	Tabela de Roteamento da INTERNET
	 */
	(ROUTE *)NULL,		/* Começo da tabela */
	(ROUTE *)NULL,		/* Final da tabela */
	N_ROUTE,		/* No. de entradas */

	/*
	 *	Tabela de endereços ETHERNET
	 */
	(ETHER *)NULL,		/* Começo da tabela */
	(ETHER *)NULL,		/* Final da tabela */
	N_ETHER,		/* No. de entradas */
	(ETHER *)NULL,		/* Começo da lista */

	/*
	 *	Tabela de blocos para datagramas da INTERNET
	 */
	NOITBLOCK,		/* Começo da tabela */
	NOITBLOCK,		/* Final da tabela */
	N_ITBLOCK,		/* No. de entradas */

	/*
	 *	ENDPOINTs RAW
	 */
	NO_RAW_EP,		/* Começo da tabela */
	NO_RAW_EP,		/* Final da tabela */
	N_RAW_EP,		/* No. de entradas */

	/*
	 *	ENDPOINTs UDP
	 */
	NO_UDP_EP,		/* Começo da tabela */
	NO_UDP_EP,		/* Final da tabela */
	N_UDP_EP,		/* No. de entradas */

	/*
	 *	ENDPOINTs TCP
	 */
	NO_TCP_EP,		/* Começo da tabela */
	NO_TCP_EP,		/* Final da tabela */
	N_TCP_EP,		/* No. de entradas */

	/*
	 *	Tabela de Pseudo Terminais
	 */
	NOPTY,			/* Começo da tabela */
	NOPTY,			/* Final da tabela */
	NPTY,			/* No. de entradas */

	/*
	 *	Tabela de EXPORT
	 */
	(EXPORT *)NULL,		/* Começo da tabela */
	(EXPORT *)NULL,		/* Final da tabela */
	N_EXPORT,		/* No. de entradas */

	/*
	 *	Reserva para novas tabelas
	 */
	{ NULL },		/* Para cada TB 3 ints */

	/*
	 *	Começo e final das Tabelas
	 */
	NOVOID,			/* Início das tabelas 0 */
	NOVOID,			/* Final  das tabelas 0 */
	NOVOID,			/* Início das tabelas 1 */
	NOVOID,			/* Final  das tabelas 1 */

	/*
	 *	Definição dos Dispositivos
	 */
	ROOTDEV,		/* Dispositivo do ROOT */
	PIPEDEV,		/* Dispositivo do PIPE */
	SWAPDEV,		/* Dispositivo do SWAP */

	"/bin/sh",		/* Nome padrão do "sh" */

	/*
	 *	Variaveis Globais do SWAP
	 */
	SWAPLOW,		/* Bloco Inicial do SWAP */
	NSWAP,			/* Tamanho em Blocos do SWAP */

	/*
	 *	No. de Unidades Fisicas
	 */
	NULL,			/* No. de BLK */
	NULL,			/* No. de CHR */

	/*
	 *	Super Blocos
	 */
	NOV7SB,			/* Início da Lista de SBs */
	NMOUNT,			/* No. maximo de MOUNTs */

	/*
	 *	Memoria Fisica
	 */
	NOVOID,			/* Final da Memoria Fisica */

	/*
	 *	Começo e final da Memoria para os Processos
	 */
	NOPG,			/* Início da Memoria 0 */
	NOPG,			/* Final  da Memoria 0 */
	NOPG,			/* Início da Memoria 1 */
	NOPG,			/* Final  da Memoria 1 */

	/*
	 *	Começo e final dos Discos em memória
	 */
	NOVOID,			/* Início do Disco */
	NOVOID,			/* Final  do Disco */
	KBTOBY (RAMD0SZ),	/* No. de bytes de RAM disk */

	NOVOID,			/* Início do Disco */
	NOVOID,			/* Final  do Disco */
	KBTOBY (RAMD1SZ),	/* No. de bytes de RAM disk */

	/*
	 *	Parametros de Tempo
	 */
	HZ,			/* Frequencia da Rede */

	/*
	 *	Parametros do SCHEDULER
	 */
	TSLICE,			/* Time-slice em Ticks do Relogio */
	INCPRI,			/* Incremento da prioridade a cada tick */
	DECPRI,			/* Decremento da prioridade a cada tick */
	TTYINC,			/* Incremento para entrada de TTY */
	NFACTOR,		/* Influencia do NICE */
	{ NULL },

	/*
	 *	Parametros dos Processos do Usuario
	 */
	MAXPPU,			/* No. Maximo de Proc. por usuario */
	BYUPPG (MAXPSZ),	/* Tamanho Maximo de um Proc. de usuario */
	UMASK,			/* Mascara de criação de arquivos */
	STKSZ,			/* Tamanho inicial da stack (bytes) */
	STKINCR,		/* Incremento da stack (bytes) */
	NCEXEC,			/* No. max. de chars. do Exec */

	/*
	 *	Outros parâmetros
	 */
	PGSZ,			/* Tamanho (em bytes) da PG */
	NOVOID,			/* Tabela de padrão para IDLE */
	NOVOID,			/* Tabela de padrão para INTR */
	{ 0xFF },		/* Troca de proc. em modo SUP */

	/*
	 *	Parâmetros da atualização do "cache" dos dispositivos
	 */
	31,			/* Período de atualização de SB e INODEs */
	5,			/* Período de atualização de blocos */
	30,			/* Tempo limite para atualizar um bloco */
	40,			/* No. máximo de blocos atualizados por ciclo */

	/*
	 *	Tabela de Portas/interrupções das linhas seriais
	 */
	{ SIO_PORT_0,	SIO_PORT_1,	SIO_PORT_2,	SIO_PORT_3 },
	{ SIO_IRQ_0,	SIO_IRQ_1,	SIO_IRQ_2,	SIO_IRQ_3 },

	/*
	 *	Tabela de Portas/interrupções das saídas paralelas
	 */
	{ LP_PORT_0,	LP_PORT_1,	LP_PORT_2,	LP_PORT_3 },
	{ LP_IRQ_0,	LP_IRQ_1,	LP_IRQ_2,	LP_IRQ_3 },

	/*
	 *	Tabela de Portas/interrupções das unidades ethernet
	 */
	{ ED_PORT_0,	ED_PORT_1,	ED_PORT_2,	ED_PORT_3 },
	{ ED_IRQ_0,	ED_IRQ_1,	ED_IRQ_2,	ED_IRQ_3 },

	/*
	 *	Estrutura UTSNAME
	 */
	{ 0 },

	/*
	 *	Variáveis diversas
	 */
	0,			/* Usa o "boot" sem alterar nada */
	0,			/* Unidade de ponto flutuante ativada */
	0,			/* Para DEBUG */
	0,			/* Relógio CMOS é UT? */

	TZMIN_DEF,		/* Fuso horário, em minutos */
	SCREEN_SAVER_TIME,	/* Tempo do protetor de tela */
	0,			/* Para 1 micro-segundo */

	0,			/* Escreve informações sobre o BOOT */
	0,			/* Não atualiza os LEDs do teclado */
	0,			/* Tipo do teclado => 0: US, 1: ABNT2 */
	1,			/* Permite o uso de "tag queueing" */

	1,			/* Permite o uso de DMA para ATA */
	N_NFS_DAEMON,		/* No. de "daemon"s do servidor NFS */

	{ NULL },

	DMESGSZ,		/* Tamanho da área para "dmesg" (printf) */

	/*
	 *	Tabela de controladores USB
	 */
	{
		1, 0, 0, 0, 0, 0, 0, 0,	/* Habilita/desabilita cada controladora */
		0, 0, 0, 0, 0, 0, 0, 0
	},

	/*
	 *	Semi constantes do sistema
	 */
	SYS_ADDR,			/* Endereço virtual do início do núcleo */
	UPROC_ADDR,			/* Endereços virtual do início da UPROC */

	PGTOBY (USER_TEXT_PGA),		/* Endereço virtual recomendado para início do texto do usuário */
	PGTOBY (USER_DATA_PGA),		/* Endereço virtual recomendado para início do texto do usuário */

	USIZE,				/* Tamanho (em pgs) da UPROC */

	NUFILE,				/* No. max. de arq. abertos por processo */

	NCON,				/* No. de consoles virtuais */
	NSIO,				/* No. de linhas seriais */

	/*
	 *	Reserva para completar 1536 bytes
	 */
	{ NULL }
};

/*
 ****************************************************************
 *	Inicialização com os valores "default"			*
 ****************************************************************
 */
void
scbsetup (void)
{
	SCB		*sp = &scb;
	pg_t		n, total;
	int		i;

	/*
	 *	Inicializa o valor padrão do "init"
	 */
	strcpy (init_nm, "/etc/init");

	/*
	 *	Verifica o valor de NCON
	 */
	if (sp->y_NCON > NCON_MAX)
		sp->y_NCON = NCON_MAX;

	/*
	 *	Supõe que "end_base_bss" e "end_ext_bss" estão alinhados em PG
	 */
	real_endpmem = sp->y_endpmem = (void *)PHYS_TO_VIRT_ADDR (phys_mem << KBSHIFT);

	/*
	 *	Verifica se deve estimar um tamanho para "nbuf"
	 */
	if (sp->y_nbuf == 0)
	{
		if ((sp->y_nbuf = phys_mem / 64) > 2048)
			sp->y_nbuf = 2048;
	}

	/*
	 *	Aloca os DEVs
	 */
	dev_setup ();

	/*
	 *	Alocação preliminar das tabelas
	 */
	if ((alloc_status = alloc_all_tb ()) < 0)
	{
		printf
		(	"\nA memória disponível não é suficiente com os "
			"parâmetros \"default\"\n"
		);

		askyesno (0);
		scb_dialog ();
	}
	else
	{
		printf ("\nDeseja modificar parâmetros? (n): ");

		flushchar ();

		for (i = BOOT_WAIT; /* abaixo */; i--)
		{
			int		k;

			if (i <= 0)
				{ scb.y_std_boot++; break; }

			if (haschar ())
				break;

			/* Espera aproximadamente 1 ms */

			for (k = 800; k > 0; k--)
				read_port (0x84);
		}

		if   (scb.y_std_boot)
			printf ("n\n\n");
		elif (askyesno (0) != 0)
			scb_dialog ();
	}

	/*
	 *	Alocou; prepara os mapas de memória
	 */
	if (ramd_pp)
	{
		ramd_pp[0].p_size = BYTOBL (sp->y_ramd0sz);
		ramd_pp[1].p_size = BYTOBL (sp->y_ramd1sz);
	}

	memclr (end_base_bss, tst_end_base_bss - end_base_bss);
	memclr (end_ext_bss,  tst_end_ext_bss  - end_ext_bss);

	end_base_bss = tst_end_base_bss;
	end_ext_bss  = tst_end_ext_bss;

	total = 0;

	map_init_free_list ();

	if ((n = sp->y_enducore0 - sp->y_ucore0) > 0)
		{ mrelease (M_CORE, n, sp->y_ucore0); total += n; }

	if ((n = sp->y_enducore1 - sp->y_ucore1) > 0)
		{ mrelease (M_CORE, n, sp->y_ucore1); total += n; }

	printf ("Memória do usuário = %d KB\n\n", PGTOKB (total));

	if (sp->y_swapdev != NODEV && sp->y_nswap > 0)
		mrelease (M_SWAP, sp->y_nswap, SWAP_OFFSET);

	CSW = sp->y_CSW;	/* Funções em ASM */

	/*
	 *	Prepara as tabelas do teclado
	 */
	key_init_table ();

}	/* end scbsetup */

/*
 ****************************************************************
 *	Atribui os dispositivos					*
 ****************************************************************
 */
void
dev_setup (void)
{
	SCB		*sp = &scb;
	const DISKTB	*pp;

	/*
	 *	Verifica se deve usar o dispositivo herdado de "boot1"
	 */
	if (sp->y_rootdev == NODEV && bcb.y_rootdev != NODEV)
	{
		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '\0')
				break;

			if (pp->p_type != PAR_TROPIX_V7 && pp->p_type != PAR_TROPIX_T1)
				continue;

			if (pp->p_dev == bcb.y_rootdev)
				{ sp->y_rootdev = pp->p_dev; break; }
		}
	}

	/*
	 *	Procura um outro dispositivo
	 */
	if (sp->y_rootdev == NODEV)
	{
		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '\0')
				break;

			if (pp->p_type != PAR_TROPIX_V7 && pp->p_type != PAR_TROPIX_T1)
				continue;

			if (pp->p_flags & DISK_ACTIVE)	/* Ativo ? */
				{ sp->y_rootdev = pp->p_dev; break; }

			if (sp->y_rootdev == NODEV)
				sp->y_rootdev = pp->p_dev;
		}
	}

	if (sp->y_rootdev == NODEV)
		printf ("*** CUIDADO: Não tenho dispositivo de ROOT\n\n");

#if (0)	/*******************************************************/
	/*
	 *	Se não foi dado dispositivo "pipe" "default", ...
	 */
	if (sp->y_pipedev == NODEV)
		sp->y_pipedev = sp->y_rootdev;
#endif	/*******************************************************/

	/*
	 *	Procura o dispositivo de "swap", se for o caso
	 */
	if (sp->y_swapdev == NODEV)
	{
		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '\0')
				break;

			if (pp->p_type == PAR_TROPIX_SWAP)
				{ sp->y_swapdev = pp->p_dev; break; }
		}
	}

	/*
	 *	Procura o tamanho do "swap", se for o caso
	 */
	if (sp->y_nswap == 0)
	{
		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_dev == sp->y_swapdev)
				{ sp->y_nswap = pp->p_size - sp->y_swaplow; break; }
		}
	}

}	/* end dev_setup */

/*
 ****************************************************************
 *	Aloca todas as tabelas					*
 ****************************************************************
 */
int
alloc_all_tb (void)
{
	SCB		*sp = &scb;
	DISKTB		*pp;
	int		i;

	/*
	 *	Inicializa os valores provisórios (já alinhados em PGs)
	 */
	tst_end_base_bss = end_base_bss;
	tst_end_ext_bss  = end_ext_bss;

	/*
	 *	Ajusta TTYMAXQ, TTYMINQ, TTYPANIC e TTYCOMMPANIC
	 */
#define	CLIST_USERs	8		/* Questão de bom senso */

	TTYMAXQ      = (sp->y_nclist * CLBSZ) / CLIST_USERs;
	TTYMINQ      = TTYMAXQ / 4;
	TTYPANIC     = 2 * TTYMAXQ;
	TTYCOMMPANIC = 2 * TTYMAXQ;

	/*
	 *	Prepara os tamanhos do HASH
	 */
	scb_hashset ();

	/*
	 *	Aloca as tabelas
	 */
	alloc_tb
	(	sp->y_nproc * sizeof (UPROCV),		4,
		&sp->y_uproc,				&sp->y_enduproc
	);

	alloc_tb
	(	sp->y_nphash * sizeof (PHASHTB),	4,
		&sp->y_phash,				&sp->y_endphash
	);

	alloc_tb
	(	sp->y_ninode * sizeof (INODE),		4,
		&sp->y_inode,				&sp->y_endinode
	);

	alloc_tb
	(	sp->y_nihash * sizeof (IHASHTB),	4,
		&sp->y_ihash,				&sp->y_endihash
	);

	/**** BUF abaixo ****/

	alloc_tb
	(	sp->y_nbuf * sizeof (BHEAD),		4,
		&sp->y_bhead,				&sp->y_endbhead
	);

	alloc_tb
	(	sp->y_nbhash * sizeof (BHASHTB),	4,
		&sp->y_bhash,				&sp->y_endbhash
	);

	alloc_tb
	(	sp->y_nkfile * sizeof (KFILE), 		4,
		&sp->y_kfile,				&sp->y_endkfile
	);

	alloc_tb
	(	sp->y_nclist * sizeof (CBLK),		sizeof (CBLK),
		&sp->y_clist,				&sp->y_endclist
	);

	alloc_tb
	(	sp->y_nmap * sizeof (MAP),		4,
		&sp->y_map,				&sp->y_endmap
	);

	alloc_tb
	(	sp->y_n_shlib_hash * sizeof (SYM *),	4,
		&sp->y_shlib_hash,		 	&sp->y_end_shlib_hash
	);

	alloc_tb
	(	sp->y_ntout * sizeof (TIMEOUT), 4,
		&sp->y_timeout,				&sp->y_endtimeout
	);

	alloc_tb
	(	sp->y_nlockf * sizeof (LOCKF),		4,
		&sp->y_lockf,				&sp->y_endlockf
	);

	alloc_tb
	(	sp->y_nueventl * sizeof (UEVENTL),	4,
		&sp->y_ueventl,				&sp->y_endueventl
	);

	alloc_tb
	(	sp->y_nueventg * sizeof (UEVENTG),	4,
		&sp->y_ueventg,				&sp->y_endueventg
	);

	alloc_tb
	(	sp->y_nusemal * sizeof (USEMAL),	4,
		&sp->y_usemal,				&sp->y_endusemal
	);

	alloc_tb
	(	sp->y_nusemag * sizeof (USEMAG),	4,
		&sp->y_usemag,				&sp->y_endusemag
	);

	alloc_tb
	(	sp->y_nregionl * sizeof (REGIONL),	4,
		&sp->y_regionl,				&sp->y_endregionl
	);

	alloc_tb
	(	sp->y_nregiong * sizeof (REGIONG),	4,
		&sp->y_regiong,				&sp->y_endregiong
	);

	alloc_tb
	(	sp->y_n_dns * sizeof (DNS),		4,
		&sp->y_dns,				&sp->y_end_dns
	);

	alloc_tb
	(	sp->y_n_route * sizeof (ROUTE),		4,
		&sp->y_route,				&sp->y_end_route
	);

	alloc_tb
	(	sp->y_n_ether * sizeof (ETHER),		4,
		&sp->y_ether,				&sp->y_end_ether
	);

	alloc_tb
	(	sp->y_n_export * sizeof (EXPORT),	4,
		&sp->y_export,				&sp->y_end_export
	);

	alloc_tb
	(	sp->y_n_itblock * sizeof (ITBLOCK),	4,
		&sp->y_itblock,				&sp->y_end_itblock
	);

	alloc_tb
	(	sp->y_n_raw_ep * sizeof (RAW_EP),	4,
		&sp->y_raw_ep,				&sp->y_end_raw_ep
	);

	alloc_tb
	(	sp->y_n_udp_ep * sizeof (UDP_EP),	4,
		&sp->y_udp_ep,				&sp->y_end_udp_ep
	);

	alloc_tb
	(	sp->y_n_tcp_ep * sizeof (TCP_EP),	4,
		&sp->y_tcp_ep,				&sp->y_end_tcp_ep
	);

	alloc_tb
	(	sp->y_npty * sizeof (PTY),		4,
		&sp->y_pty,				&sp->y_end_pty
	);

	/*
	 *	Aloca os BUFs do cache em dois pedaços
	 */
	sp->y_buf0 = sp->y_endbuf0 = NOVOID;
	sp->y_buf1 = sp->y_endbuf1 = NOVOID;

	i = BASE_END - (unsigned)ROUND (tst_end_base_bss, BL4SZ);

	i = MIN (sp->y_nbuf * BL4SZ, i);

	if (i > 0)
	{
		alloc_tb
		(	i,				BL4SZ,
			&sp->y_buf0,			&sp->y_endbuf0
		);

		i = sp->y_nbuf * BL4SZ - i;
	}
	else
	{
		i = sp->y_nbuf * BL4SZ;
	}

	if (i > 0)
	{
		alloc_tb
		(	i,				BL4SZ,
			&sp->y_buf1,			&sp->y_endbuf1
		);
	}

	/*
	 *	Aloca os começos e finais das tabelas
	 */
	tst_end_base_bss = ROUND (tst_end_base_bss, PGSZ);
	tst_end_ext_bss  = ROUND (tst_end_ext_bss, PGSZ);

	sp->y_tb0    = end_base_bss;
	sp->y_endtb0 = tst_end_base_bss;

	sp->y_tb1    = end_ext_bss;
	sp->y_endtb1 = tst_end_ext_bss;

	/*
	 *	Aloca o final da memória
	 */
	if (given_endpmem != NOVOID)
	{
		if (given_endpmem > real_endpmem)
			return (-1);

		sp->y_endpmem = given_endpmem;
	}
	else
	{
		sp->y_endpmem = real_endpmem;
	}

	/*
	 *	Verifica se houve um tamanho de RAMD herdado de BOOT2
	 */
	for (pp = disktb; /* abaixo */; pp++)
	{
		if (pp->p_name[0] == '\0')
		{
			printf ("scb_setup: NÃO achei as entradas das RAMD\n");
			break;
		}

		if (MAJOR (pp->p_dev) == RAMD_MAJOR)
		{
			ramd_pp = pp;

			if (pp[0].p_size != 0)
				sp->y_ramd0sz = BLTOBY (pp[0].p_size);

			if (pp[1].p_size != 0)
				sp->y_ramd1sz = BLTOBY (pp[1].p_size);

			break;

		}
	}


#if (0)	/*******************************************************/
			if (sp->y_ramd0sz == 0 && pp[0].p_size != 0)
				sp->y_ramd0sz = BLTOBY (pp[0].p_size);

			if (sp->y_ramd1sz == 0 && pp[1].p_size != 0)
				sp->y_ramd1sz = BLTOBY (pp[1].p_size);


			if (pp[0].p_size != 0)
				sp->y_ramd0sz = BLTOBY (pp[0].p_size);
			else
				pp[0].p_size = BYTOBL (sp->y_ramd0sz);

			if (pp[1].p_size != 0)
				sp->y_ramd1sz = BLTOBY (pp[1].p_size);
			else
				pp[1].p_size = BYTOBL (sp->y_ramd1sz);
#endif	/*******************************************************/

	/*
	 *	Aloca os RAMDs tomando CUIDADO para NÃO superpor com área do SVGA !
	 */
	sp->y_ramd0sz  = PGROUND (sp->y_ramd0sz);
	sp->y_endramd0 = sp->y_endpmem;

	if (sp->y_endramd0 > (void *)(PHYS_TO_VIRT_ADDR (SVGA_ADDR)))
		sp->y_endramd0 = (void *)(PHYS_TO_VIRT_ADDR (SVGA_ADDR));

	sp->y_ramd0    = sp->y_endramd0 - sp->y_ramd0sz;

	sp->y_ramd1sz  = PGROUND (sp->y_ramd1sz);
	sp->y_endramd1 = sp->y_ramd0;
	sp->y_ramd1    = sp->y_endramd1 - sp->y_ramd1sz;

	if (sp->y_ramd1 < tst_end_ext_bss)
		return (-1);

	/*
	 *	Aloca as áreas do usuário (em PÁGINAS)
	 */
	sp->y_ucore0	= (unsigned)sp->y_endtb0 >> PGSHIFT;
	sp->y_enducore0	= (unsigned)BASE_END >> PGSHIFT;

	sp->y_ucore1	= (unsigned)sp->y_endtb1 >> PGSHIFT;
	sp->y_enducore1	= (unsigned)sp->y_ramd1 >> PGSHIFT;

	return (0);

}	/* end alloc_all_tb */

/*
 ****************************************************************
 *	Aloca uma tabela					*
 ****************************************************************
 */
void
alloc_tb (long sz, int align, void *tb_begin, void *tb_end)
{
	void		*tst_begin, *tst_end;

	/*
	 *	Supõe que "align" seja potência de 2
	 *	Verifica em qual região cabe a tabela
	 */
	tst_begin = ROUND (tst_end_base_bss, align);

	if ((tst_end = tst_begin + sz) <= (void *)BASE_END)
	{
	   /*** tst_begin = tst_end_base_bss ... ***/
	   /***	tst_end = tst_begin + sz; ***/
		tst_end_base_bss = tst_end;
	}
	else
	{
		tst_begin = ROUND (tst_end_ext_bss, align);
		tst_end = tst_begin + sz;
		tst_end_ext_bss = tst_end;
	}

	*(void **)tb_begin = tst_begin;
	*(void **)tb_end   = tst_end;

}	/* end alloc_tb */

/*
 ****************************************************************
 *	Calcula o tamanho das tabelas HASH			*
 ****************************************************************
 */
void
scb_hashset (void)
{
	SCB		*sp = &scb;

	/*
	 *	PROC Hash
	 */
	sp->y_nphash = next_power (FATOR (sp->y_nproc));
	sp->y_phmask = sp->y_nphash - 1;

	/*
	 *	INODE Hash
	 */
	sp->y_nihash = FATOR (sp->y_ninode);

	/*
	 *	BHEAD Hash
	 */
	sp->y_nbhash = FATOR (sp->y_nbuf);

}	/* end scb_hashset */

/*
 ****************************************************************
 *	Obtem a potência de 2 mais próxima do número		*
 ****************************************************************
 */
int
next_power (int n)
{
	int		i;

	for (i = 16; i < n; i <<= 1)
		/* vazio */;

	return (i);

}	/* end newtpower */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "A"		*
 ****************************************************************
 */
void
scb_a_display (void)
{
	SCB		*sp = &scb;

	printf ("\f        PALAVRA   ENTRADA NO. DE   NO. DE   TAMANHO\n");
	printf   ("TABELA   CHAVE    (Bytes) ENTRADAS ENTRADAS  TOTAL\n");
	printf   ("                          DEFAULT  EFETIVO  (Bytes)\n\n");

	/*
	 *	PROC
	 */
	printf
	(	"PROC\tnproc\t%7d%9d%9d%8d\n",
		sizeof (UPROCV), NPROC, sp->y_nproc,
		sp->y_nproc * sizeof (UPROCV)
	);

	printf
	(	"PHASH\t\t%7d%9d%9d%8d\n",
		sizeof (PHASHTB), next_power (FATOR (NPROC)), sp->y_nphash,
		sp->y_nphash * sizeof (PHASHTB)
	);

	/*
	 *	INODE
	 */
	printf
	(	"INODE\tninode\t%7d%9d%9d%8d\n",
		sizeof (INODE), NINODE, sp->y_ninode,
		sp->y_ninode * sizeof (INODE)
	);

	printf
	(	"IHASH\t\t%7d%9d%9d%8d\n",
		sizeof (IHASHTB), FATOR (NINODE), sp->y_nihash,
		sp->y_nihash * sizeof (IHASHTB)
	);

	/*
	 *	BUF
	 */
	printf
	(	"BUF\tnbuf\t%7d%9d%9d%8d\n",
		sizeof (BHEAD) + BL4SZ, NBUF, sp->y_nbuf,
		sp->y_nbuf * (sizeof (BHEAD) + BL4SZ)
	);

	printf
	(	"BHASH\t\t%7d%9d%9d%8d\n",
		sizeof (BHASHTB), FATOR (NBUF), sp->y_nbhash,
		sp->y_nbhash * sizeof (BHASHTB)
	);

	/*
	 *	KFILE
	 */
	printf
	(	"KFILE\tnkfile\t%7d%9d%9d%8d\n",
		sizeof (KFILE), NKFILE, sp->y_nkfile,
		sp->y_nkfile * sizeof (KFILE)
	);

	/*
	 *	CLIST
	 */
	printf
	(	"CLIST\tnclist\t%7d%9d%9d%8d\n",
		sizeof (CBLK), NCLIST, sp->y_nclist,
		sp->y_nclist * sizeof (CBLK)
	);

	/*
	 *	MAP
	 */
	printf
	(	"MAP    \tnmap\t%7d%9d%9d%8d\n",
		sizeof (MAP), NMAP, sp->y_nmap,
		sp->y_nmap * sizeof (MAP)
	);

	/*
	 *	SHLIB_HASH
	 */
	printf
	(	"SHLIB  \tnhshlib\t%7d%9d%9d%8d\n",
		sizeof (SYM *), NHSHLIB, sp->y_n_shlib_hash,
		sp->y_n_shlib_hash * sizeof (SYM *)
	);

	/*
	 *	TIMEOUT
	 */
	printf
	(	"TIMEOUT\tntout\t%7d%9d%9d%8d\n",
		sizeof (TIMEOUT), NTOUT, sp->y_ntout,
		sp->y_ntout * sizeof (TIMEOUT)
	);

	/*
	 *	PTYs
	 */
	printf
	(	"PTY\tnpty\t%7d%9d%9d%8d\n",
		sizeof (PTY), NPTY, sp->y_npty,
		sp->y_npty * sizeof (PTY)
	);

	/*
	 *	NCONs
	 */
	printf
	(	"CON\tncon\t%7d%9d%9d%8d\n",
		sizeof (VIDEODISPLAY), NCON, sp->y_NCON,
		sp->y_NCON * sizeof (VIDEODISPLAY)
	);

	/*
	 *	Imprime os totais
	 */
	scb_print_total ();

}	/* end scb_a_display */

/*
 ****************************************************************
 *	Mostra na tela o conjunto de parâmetros "B"		*
 ****************************************************************
 */
void
scb_b_display (void)
{
	SCB		*sp = &scb;

	printf ("\f        PALAVRA   ENTRADA NO. DE   NO. DE   TAMANHO\n");
	printf   ("TABELA   CHAVE    (Bytes) ENTRADAS ENTRADAS  TOTAL\n");
	printf   ("                          DEFAULT  EFETIVO  (Bytes)\n\n");

	/*
	 *	LOCKF
	 */
	printf
	(	"LOCKF\tnlockf\t%7d%9d%9d%8d\n",
		sizeof (LOCKF), NLOCKF, sp->y_nlockf,
		sp->y_nlockf * sizeof (LOCKF)
	);

	/*
	 *	UEVENTL
	 */
	printf
	(	"EVENTL\tneventl\t%7d%9d%9d%8d\n",
		sizeof (UEVENTL), NUEVENTL, sp->y_nueventl,
		sp->y_nueventl * sizeof (UEVENTL)
	);

	/*
	 *	UEVENTG
	 */
	printf
	(	"EVENTG\tneventg\t%7d%9d%9d%8d\n",
		sizeof (UEVENTG), NUEVENTG, sp->y_nueventg,
		sp->y_nueventg * sizeof (UEVENTG)
	);

	/*
	 *	USEMAL
	 */
	printf
	(	"SEMAL\tnsemal\t%7d%9d%9d%8d\n",
		sizeof (USEMAL), NUSEMAL, sp->y_nusemal,
		sp->y_nusemal * sizeof (USEMAL)
	);

	/*
	 *	USEMAG
	 */
	printf
	(	"SEMAG\tnsemag\t%7d%9d%9d%8d\n",
		sizeof (USEMAG), NUSEMAG, sp->y_nusemag,
		sp->y_nusemag * sizeof (USEMAG)
	);

	/*
	 *	REGIONL
	 */
	printf
	(	"REGIONL\tnregionl%7d%9d%9d%8d\n",
		sizeof (REGIONL), NREGIONL, sp->y_nregionl,
		sp->y_nregionl * sizeof (REGIONL)
	);

	/*
	 *	REGIONG
	 */
	printf
	(	"REGIONG\tnregiong%7d%9d%9d%8d\n",
		sizeof (REGIONG), NREGIONG, sp->y_nregiong,
		sp->y_nregiong * sizeof (REGIONG)
	);

	/*
	 *	DNS
	 */
	printf
	(	"DNS\tndns\t%7d%9d%9d%8d\n",
		sizeof (DNS), N_DNS, sp->y_n_dns,
		sp->y_n_dns * sizeof (DNS)
	);

	/*
	 *	ROUTE
	 */
	printf
	(	"ROUTE\tnroute\t%7d%9d%9d%8d\n",
		sizeof (ROUTE), N_ROUTE, sp->y_n_route,
		sp->y_n_route * sizeof (ROUTE)
	);

	/*
	 *	ETHER
	 */
	printf
	(	"ETHER\tnether\t%7d%9d%9d%8d\n",
		sizeof (ETHER), N_ETHER, sp->y_n_ether,
		sp->y_n_ether * sizeof (ETHER)
	);

	/*
	 *	EXPORT
	 */
	printf
	(	"EXPORT\tnexport\t%7d%9d%9d%8d\n",
		sizeof (EXPORT), N_EXPORT, sp->y_n_export,
		sp->y_n_export * sizeof (EXPORT)
	);

	/*
	 *	ITBLOCK
	 */
	printf
	(	"ITBLOCK\tnitblock%7d%9d%9d%8d\n",
		sizeof (ITBLOCK), N_ITBLOCK, sp->y_n_itblock,
		sp->y_n_itblock * sizeof (ITBLOCK)
	);

	/*
	 *	RAW_EP
	 */
	printf
	(	"RAWEP\tnrawep\t%7d%9d%9d%8d\n",
		sizeof (RAW_EP), N_RAW_EP, sp->y_n_raw_ep,
		sp->y_n_raw_ep * sizeof (RAW_EP)
	);

	/*
	 *	UDP_EP
	 */
	printf
	(	"UDPEP\tnudpep\t%7d%9d%9d%8d\n",
		sizeof (UDP_EP), N_UDP_EP, sp->y_n_udp_ep,
		sp->y_n_udp_ep * sizeof (UDP_EP)
	);

	/*
	 *	TCP_EP
	 */
	printf
	(	"TCPEP\tntcpep\t%7d%9d%9d%8d\n",
		sizeof (TCP_EP), N_TCP_EP, sp->y_n_tcp_ep,
		sp->y_n_tcp_ep * sizeof (TCP_EP)
	);

	/*
	 *	Imprime os totais
	 */
	scb_print_total ();

}	/* end scb_b_display */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "C"		*
 ****************************************************************
 */
void
scb_c_display (void)
{
	SCB		*sp = &scb;

	printf ("\f");

	/*
	 *	Nomes de arquivos
	 */
	printf ("initnm    = %s\t\t", init_nm);
	printf ("libtnm    = %s\n", libt_nm);
	printf ("shnm      = %s\n\n", sp->y_sh_nm);

	/*
	 *	Dispositivos
	 */
	printf ("rootdev   = %5v    \t\t", sp->y_rootdev);
#if (0)	/*******************************************************/
	printf ("rootdev   = ");
	scb_prdev (sp->y_rootdev);
	printf ("  \t");
#endif	/*******************************************************/
	printf ("swapdev   = %v\n", sp->y_swapdev);
#if (0)	/*******************************************************/
	printf ("swapdev   = ");
	scb_prdev (sp->y_swapdev);
	printf ("\n");
#endif	/*******************************************************/

	printf ("swaplow   = %d   \t\t", sp->y_swaplow);
	printf ("nswap     = %d\n\n", sp->y_nswap);

#if (0)	/*******************************************************/
	printf ("pipedev   = ");
	scb_prdev (sp->y_pipedev);
	printf ("  \t");
	printf ("nswap     = %d\n", sp->y_nswap);

	printf ("swapdev   = ");
	scb_prdev (sp->y_swapdev);
	printf ("\n\n");
#endif	/*******************************************************/

	/*
	 *	Memória
	 */
	printf ("ucore0    = %d KB\t\t", VIRTUALPGTOKB (sp->y_ucore0) );
	printf ("enducore0 = %d KB\n", VIRTUALPGTOKB (sp->y_enducore0) );

	printf ("ucore1    = %d KB\t\t", VIRTUALPGTOKB (sp->y_ucore1) );
	printf ("enducore1 = %d KB\n", VIRTUALPGTOKB (sp->y_enducore1) );

	printf ("ramd1sz   = %d KB\t\t", BYTOKB (sp->y_ramd1sz));
	printf ("ramd1     = %d KB\n", VIRTUALTOKB (sp->y_ramd1) );

	printf ("endramd1  = %d KB\t\t", VIRTUALTOKB (sp->y_endramd1) );
	printf ("ramd0sz   = %d KB\n", BYTOKB (sp->y_ramd0sz));

	printf ("ramd0     = %d KB\t\t", VIRTUALTOKB (sp->y_ramd0) );
	printf ("endramd0  = %d KB\n", VIRTUALTOKB (sp->y_endramd0) );

	printf ("endpmem   = %d KB\n\n", VIRTUALTOKB (sp->y_endpmem) );

	printf ("preempt   = 0x%02X\t\n", sp->y_preemption_mask[0]);

	printf ("debug     = %d \t\t\t", sp->y_CSW);
	printf ("verbose   = %d\n", sp->y_boot_verbose);

	printf ("key       = %d\n", sp->y_keyboard);

}	/* end scb_c_display */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "D"		*
 ****************************************************************
 */
void
scb_d_display (void)
{
	SCB		*sp = &scb;

	printf ("\f");

	/*
	 *	Scheduler
	 */
	printf ("tslice    = %d Ticks\n", sp->y_tslice);

	printf ("incpri    = %d\t\t\t", sp->y_incpri);
	printf ("decpri    = %d\n", sp->y_decpri);

	printf ("ttyinc    = %d\t\t\t", sp->y_ttyinc);
	printf ("nfactor   = %d\n", sp->y_nfactor);

	/*
	 *	Processo do usário
	 */
	printf ("maxppu    = %d\t\t\t", sp->y_maxppu);
	printf ("maxpsz    = %d KB\n", PGTOKB (sp->y_maxpsz));

	printf ("umask     = %3o\t\t\t", sp->y_umask);
	printf ("ncexec    = %d\n", sp->y_ncexec);

	printf ("stksz     = %d\t\t", sp->y_stksz);
	printf ("stkincr   = %d\n\n", sp->y_stkincr);

	/*
	 *	Diversos
	 */
	printf ("hz        = %d\t\t\t", sp->y_hz);
	printf ("nmount    = %d\n", sp->y_nmount);

	printf ("tagenable = %d\t\t\t", sp->y_tagenable);
	printf ("dmaenable = %d\n", sp->y_dma_enable);

}	/* end scb_d_display */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "E"		*
 ****************************************************************
 */
void
scb_e_display (void)
{
	SCB		*sp = &scb;

	printf ("\f");

	printf ("\nsioport0 =   0x%X\t",  sp->y_sio_port[0]);
	printf ("sioirq0  = %d\t", 	  sp->y_sio_irq[0]);
	printf ("sioport1 =   0x%X\t", 	  sp->y_sio_port[1]);
	printf ("sioirq1  = %d\n", 	  sp->y_sio_irq[1]);
	printf ("sioport2 =   0x%X\t", 	  sp->y_sio_port[2]);
	printf ("sioirq2  = %d\t", 	  sp->y_sio_irq[2]);
	printf ("sioport3 =   0x%X\t", 	  sp->y_sio_port[3]);
	printf ("sioirq3  = %d\n", 	  sp->y_sio_irq[3]);
	printf ("sioport4 =   0x%X\t", 	  sp->y_sio_port[4]);
	printf ("sioirq4  = %d\t", 	  sp->y_sio_irq[4]);
	printf ("sioport5 =   0x%X\t", 	  sp->y_sio_port[5]);
	printf ("sioirq5  = %d\n", 	  sp->y_sio_irq[5]);
	printf ("sioport6 =   0x%X\t", 	  sp->y_sio_port[6]);
	printf ("sioirq6  = %d\t", 	  sp->y_sio_irq[6]);
	printf ("sioport7 =   0x%X\t", 	  sp->y_sio_port[7]);
	printf ("sioirq7  = %d\n", 	  sp->y_sio_irq[7]);

	printf ("\nlpport0  =   0x%X\t",  sp->y_lp_port[0]);
	printf ("lpirq0   = %d\t", 	  sp->y_lp_irq[0]);
	printf ("lpport1  =   0x%X\t", 	  sp->y_lp_port[1]);
	printf ("lpirq1   = %d\n", 	  sp->y_lp_irq[1]);
	printf ("lpport2  =   0x%X\t", 	  sp->y_lp_port[2]);
	printf ("lpirq2   = %d\t", 	  sp->y_lp_irq[2]);
	printf ("lpport3  =   0x%X\t", 	  sp->y_lp_port[3]);
	printf ("lpirq3   = %d\n", 	  sp->y_lp_irq[3]);

	printf ("\nedport0  =   0x%X\t",  sp->y_ed_port[0]);
	printf ("edirq0   = %d\t", 	  sp->y_ed_irq[0]);
	printf ("edport1  =   0x%X\t", 	  sp->y_ed_port[1]);
	printf ("edirq1   = %d\n", 	  sp->y_ed_irq[1]);
	printf ("edport2  =   0x%X\t", 	  sp->y_ed_port[2]);
	printf ("edirq2   = %d\t", 	  sp->y_ed_irq[2]);
	printf ("edport3  =   0x%X\t", 	  sp->y_ed_port[3]);
	printf ("edirq3   = %d\n", 	  sp->y_ed_irq[3]);

	printf ("\nusb0     =   %d\t\t",  sp->y_usb_enable[0]);
	printf ("usb1     = %d\t",	  sp->y_usb_enable[1]);
	printf ("usb2     =   %d\t\t",	  sp->y_usb_enable[2]);
	printf ("usb3     = %d\n",	  sp->y_usb_enable[3]);
	printf ("usb4     =   %d\t\t",	  sp->y_usb_enable[4]);
	printf ("usb5     = %d\t",	  sp->y_usb_enable[5]);
	printf ("usb6     =   %d\t\t",	  sp->y_usb_enable[6]);
	printf ("usb7     = %d\n",	  sp->y_usb_enable[7]);

	printf ("\nusb8     =   %d\t\t",  sp->y_usb_enable[8]);
	printf ("usb9     = %d\t",	  sp->y_usb_enable[9]);
	printf ("usb10    =   %d\t\t",	  sp->y_usb_enable[10]);
	printf ("usb11    = %d\n",	  sp->y_usb_enable[11]);
	printf ("usb12    =   %d\t\t",	  sp->y_usb_enable[12]);
	printf ("usb13    = %d\t",	  sp->y_usb_enable[13]);
	printf ("usb14    =   %d\t\t",	  sp->y_usb_enable[14]);
	printf ("usb15    = %d\n",	  sp->y_usb_enable[15]);

}	/* end scb_e_display */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "F"		*
 ****************************************************************
 */
void
scb_f_display (void)
{
	const DISKTB	*pp;
	int		i;
	char		buf[16];

	/*
	 *	Imprime a tabela
	 */
	printf ("\nTABELA DE PARTIÇÕES:\n");

	printf ("\n-NAME- --OFFSET-- ---SIZE--- --MB- --DEV-- UNIT  TYPE\n\n");

	for (pp = disktb, i = 0; pp->p_name[0]; pp++, i++)
	{
		if (i && i % 5 == 0)
		{
			if (i % 15 == 0)
				gets (buf);
			else
				printf ("\n");
		}

		printf
		(	"%6s %10d %10d %5d (%d,%3d) %4d  ",
			pp->p_name,
			pp->p_offset,
			pp->p_size,
			pp->p_size >> (20-BLSHIFT),
			MAJOR (pp->p_dev),
			MINOR (pp->p_dev),
			pp->p_target
		);

		print_part_type_nm (pp->p_type);

		printf ("\n");

	}	/* end for (tabela de partições) */

}	/* scb_f_display */

/*
 ****************************************************************
 *	Mostra na Tela o Conjunto de Parametros "G"		*
 ****************************************************************
 */
void
scb_g_display (void)
{
	SCB		*sp = &scb;

	printf ("\f");

	/*
	 *	Endereços das tabelas
	 */
	printf ("UPROC:   %P, %P\t\t", sp->y_uproc, sp->y_enduproc);
	printf ("PHASH:   %P, %P\n",   sp->y_phash, sp->y_endphash);
	printf ("INODE:   %P, %P\t\t", sp->y_inode, sp->y_endinode);
	printf ("IHASH:   %P, %P\n",   sp->y_ihash, sp->y_endihash);
	printf ("BHEAD:   %P, %P\t\t", sp->y_bhead, sp->y_endbhead);
	printf ("BHASH:   %P, %P\n",   sp->y_bhash, sp->y_endbhash);
	printf ("BUF0:    %P, %P\t\t", sp->y_buf0, sp->y_endbuf0);
	printf ("BUF1:    %P, %P\n",   sp->y_buf1, sp->y_endbuf1);
	printf ("KFILE:   %P, %P\t\t", sp->y_kfile, sp->y_endkfile);
	printf ("CLIST:   %P, %P\n",   sp->y_clist, sp->y_endclist);
	printf ("MAP:     %P, %P\t\t", sp->y_map, sp->y_endmap);
	printf ("SHLIB:   %P, %P\t\t", sp->y_shlib_hash, sp->y_end_shlib_hash);
	printf ("TOUT :   %P, %P\t\t", sp->y_timeout, sp->y_endtimeout);
	printf ("LOCKF:   %P, %P\n",   sp->y_lockf, sp->y_endlockf);
	printf ("UEVENTL: %P, %P\t\t", sp->y_ueventl, sp->y_endueventl);
	printf ("UEVENTG: %P, %P\n",   sp->y_ueventg, sp->y_endueventg);
	printf ("USEMAL:  %P, %P\t\t", sp->y_usemal, sp->y_endusemal);
	printf ("USEMAG:  %P, %P\n",   sp->y_usemag, sp->y_endusemag);
	printf ("REGIONL: %P, %P\t\t", sp->y_regionl, sp->y_endregionl);
	printf ("REGIONG: %P, %P\n",   sp->y_regiong, sp->y_endregiong);
	printf ("DNS:     %P, %P\t\t", sp->y_dns, sp->y_end_dns);
	printf ("ROUTE:   %P, %P\n",   sp->y_route, sp->y_end_route);
	printf ("ETHER:	  %P, %P\t\t", sp->y_ether, sp->y_end_ether);
	printf ("EXPORT:  %P, %P\t\t", sp->y_export, sp->y_end_export);
	printf ("ITBLOCK: %P, %P\n",   sp->y_itblock, sp->y_end_itblock);
	printf ("RAW_EP : %P, %P\t\t", sp->y_raw_ep, sp->y_end_raw_ep);
	printf ("UDP_EP:  %P, %P\n",   sp->y_udp_ep, sp->y_end_udp_ep);
	printf ("TCP_EP : %P, %P\n",   sp->y_tcp_ep, sp->y_end_tcp_ep);

}	/* end scb_g_display */

/*
 ****************************************************************
 *	Escreve o tipo de uma partição				*
 ****************************************************************
 */
void
print_part_type_nm (int type)
{
	const PARTNM	*pnm;

	for (pnm = partnm; /* abaixo */; pnm++)
	{
		if (pnm->pt_type < 0)
			{ printf ("??? (0x%2X)", type); break; }

		if (pnm->pt_type == type)
			{ printf ("%s", pnm->pt_nm); break; }
	}

} 	/* end print_part_type_nm */

/*
 ****************************************************************
 *	Imprime os totais alocados				*
 ****************************************************************
 */
void
scb_print_total (void)
{
	SCB		*sp = &scb;
	long		base, ext, tb, total;

	/*
	 *	Kernel
	 */
	base	= sp->y_endtb0 - sp->y_tb0;
	ext	= sp->y_endtb1 - sp->y_tb1;
	tb	= base + ext;
	total	= sp->y_tb0 - (void *)SYS_ADDR + tb;

	printf
	(	"\nKERNEL: \t%d + %d + %d = %d (%d KB)\n",

		VIRT_TO_PHYS_ADDR (&etext),
		sp->y_tb0 - (void *)&etext,
		tb,
		total,
		BYTOKB (total)
	);

	/*
	 *	Usuário
	 */
	base	= sp->y_enducore0 - sp->y_ucore0;
	ext	= sp->y_enducore1 - sp->y_ucore1;
	total	= base + ext;

	printf
	(	"USUÁRIO:\t%d + %d = %d (%d KB)\n",
		PGTOBY (base),
		PGTOBY (ext),
		PGTOBY (total),
		PGTOKB (total)
	);

}	/* end scb_print_total */

/*
 ****************************************************************
 *	Diálogo para os parâmetros				*
 ****************************************************************
 */
void
scb_dialog (void)
{
	char		*cp;
	char		*np;
	int		n, old_n = 0;
	const SCBOPT	*op;
	SCB		*sp = &scb;
	char		area[80];

	/*
	 *	Escreve a tela corrente
	 */
    again:
	switch (scbscreen)
	{
	    case 'a':
		scb_a_display ();
		break;

	    case 'b':
		scb_b_display ();
		break;

	    case 'c':
		scb_c_display ();
		break;

	    case 'd':
		scb_d_display ();
		break;

	    case 'e':
		scb_e_display ();
		break;

	    case 'f':
		scb_f_display ();
		break;

	    case 'g':
		scb_g_display ();
		break;

	}	/* end switch */

	/*
	 *	Lê um comando
	 */
    prompt:
	printf ("\n> ");

	gets (area); putchar ('\n'); cp = &area[0];

	if (cp[0] == '\0')
		goto prompt;

	if (cp[0] == CEOT || (cp[0] == 'q' && cp[1] == '\0'))
	{
		if (alloc_status >= 0)
			return;
		printf
		(	"A memória disponível não é suficiente para os "
			"parâmetros atuais\n"
		);
		goto prompt;
	}

	/*
	 *	É um comando de uma única letra?
	 */
	if (cp[1] == '\0') switch (cp[0])
	{
	    case 'a':
	    case 'b':
	    case 'c':
	    case 'd':
	    case 'e':
	    case 'f':
	    case 'g':
		scbscreen = cp[0];
		goto again;

	    case 'r':
	    case '\0':
		goto again;

	    default:
		printf ("Ainda não temos a tela \"%c\"\n", cp[0]);
		goto prompt;

	}	/* end switch */

	/*
	 *	Procura o "="
	 */
	for (/* sem inicialização */; cp[0] != '='; cp++)
	{
		if (cp[0] == '\0')
		{
			printf ("Não achei o \"=\" da linha\n");
			goto prompt;
		}
	}

	/*
	 *	Isola o ID
	 */
	cp[0] = '\0'; 	np = cp + 1;

	/*
	 *	Verifica se é o nome de um arquivo
	 */
	if   (streq (area, "initnm"))
		{ strcpy (init_nm, np); goto again; }
	elif (streq (area, "libtnm"))
		{ strcpy (libt_nm, np); goto again; }
	elif (streq (area, "shnm"))
		{ strcpy (sp->y_sh_nm, np); goto again; }

	/*
	 *	Procura o ID na tabela A
	 */
	for (op = scbopta; op->s_name[0] != '\0'; op++)
	{
		if (streq (area, op->s_name))
			{ scbscreen = 'a'; goto found; }
	}

	/*
	 *	Procura o ID na tabela B
	 */
	for (op = scboptb; op->s_name[0] != '\0'; op++)
	{
		if (streq (area, op->s_name))
			{ scbscreen = 'b'; goto found; }
	}

	/*
	 *	Procura o ID na tabela C
	 */
	for (op = scboptc; op->s_name[0] != '\0'; op++)
	{
		if (streq (area, op->s_name))
			{ scbscreen = 'c'; goto found; }
	}

	/*
	 *	Procura o ID na tabela D
	 */
	for (op = scboptd; op->s_name[0] != '\0'; op++)
	{
		if (streq (area, op->s_name))
			{ scbscreen = 'd'; goto found; }
	}

	/*
	 *	Procura o ID na tabela E
	 */
	for (op = scbopte; op->s_name[0] != '\0'; op++)
	{
		if (streq (area, op->s_name))
			{ scbscreen = 'e'; goto found; }
	}

	/*
	 *	Não achou o ID em nenhuma tabela
	 */
	printf
	(	"Identificador (\"%s\") desconhecido ou não modificável\n",
		area
	);

	goto prompt;

	/*
	 *	Encontrou o parâmetro
	 */
#if (0)	/*******************************************************/
	(	streq (&area[4], "dev") || streq (area, "root")	||
		streq (area, "swap")	|| streq (area, "pipe")
#endif	/*******************************************************/
    found:
	if (streq (&area[4], "dev") || streq (area, "root") || streq (area, "swap"))
	{
		/* PROVISÓRIO */

		if   (streq (np, "-"))
			{ *(long *)op->s_ptr = NODEV; dev_setup (); goto again; }
		elif (streq (np, "nodev") || streq (np, "NODEV"))
			{ n = NODEV; goto good_value; }
		else
			n = strtodev (np);
	}
	else
	{
		n = strtol (np, (const char **)NULL, 0);
	}

	if (n < 0)
	{
		printf ("Valor (\"%s\") inválido/desconhecido\n", np);
		goto prompt;
	}

	/*
	 *	Analisa o argumento - existem algumas mudanças de unidades
	 */
    good_value:
	if   (streq (area, "maxpsz") || streq (area, "mmulimit"))
	{
		n = KBTOPG (n);
	}
#if (0)	/*******************************************************/
	elif (streq (area, "rootdev") || streq (area, "root"))
	{
		sp->y_pipedev = n;
	}
#endif	/*******************************************************/
	elif (streq (area, "endpmem"))
	{
		if (n != 0)
			n = (long)PHYS_TO_VIRT_ADDR (PGROUND (KBTOBY (n)));
	}
	elif (streq (area, "ramdsz") || streq (area, "ramd0sz") || streq (area, "ramd1sz"))
	{
		n = KBTOBY (n);
	}

	/*
	 *	Atribui o valor
	 */
	if   (op->s_sizeof == sizeof (long))
		{ old_n = *(long *)op->s_ptr;  *(long *)op->s_ptr = n; }
	elif (op->s_sizeof == sizeof (short))
		{ old_n = *(short *)op->s_ptr; *(short *)op->s_ptr = n; }
	elif (op->s_sizeof == sizeof (char))
		{ old_n = *(char *)op->s_ptr;  *(char *)op->s_ptr = n; }

	/*
	 *	Com os novos valores, aloca as tabelas
	 */
	if ((alloc_status = alloc_all_tb ()) < 0)
	{
		printf
		(	"A memória disponível não é suficiente para o "
			"valor dado\n"
		);

		if   (op->s_sizeof == sizeof (long))
			{ *(long *)op->s_ptr = old_n; }
		elif (op->s_sizeof == sizeof (short))
			{ *(short *)op->s_ptr = old_n; }
		elif (op->s_sizeof == sizeof (char))
			{ *(char *)op->s_ptr = old_n; }

		alloc_status = alloc_all_tb ();

		goto prompt;
	}

	goto again;

}	/* end scb_dialog */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Imprime o Nome do Dispositivo				*
 ****************************************************************
 */
void
scb_prdev (dev_t dev)
{
	printf ("%s ", edit_dev_nm (dev));

	if (dev != NODEV)
		printf ("(%u/%u)  ", MAJOR (dev), MINOR (dev) );

}	/* end scb_prdev */
#endif	/*******************************************************/
