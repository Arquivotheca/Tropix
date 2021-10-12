/*
 ****************************************************************
 *								*
 *			rtl.c					*
 *								*
 *	"Driver" para controlador RealTek Fast ETHERNET		*
 *								*
 *	Versão	3.2.3, de 09.05.00				*
 *		4.9.0, de 09.08.06				*
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
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"
#include "../h/map.h"

#include "../h/itnet.h"
#include "../h/ed.h"
#include "../h/tty.h"

#include "../h/pci.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/timeout.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

#define PL	2		/* Prioridade de interrupção */
#define spled	spl2		/* Função de prioridade de interrupção */

#define	NED	2		/* Número de unidades */

/*
 ****************************************************************
 *	Registradores						*
 ****************************************************************
 */
#define	RL_IDR0		0x0000		/* ID register 0 (station addr) */

#define RL_MAR0		0x0008		/* Multicast hash table */
#define RL_MAR4		0x000C

#define RL_TXSTAT0	0x0010		/* Estado do SLOT inicial */
#define RL_TXADDR0	0x0020		/* Endereço do SLOT inicial */

#define RL_RXADDR	0x0030		/* Endereço do buffer de recepção */
#define RL_COMMAND	0x0037		/* Registrador de comandos */
#define RL_CURRXADDR	0x0038		/* Endereço do pacote recebido */
#define RL_CURRXBUF	0x003A		/* current RX buffer address */
#define RL_IMR		0x003C		/* Máscara de interrupções */
#define RL_ISR		0x003E		/* Estado da interrupção */
#define RL_TXCFG	0x0040		/* Configuração da transmissão */
#define RL_RXCFG	0x0044		/* Configuração da recepção */
#define RL_TIMERCNT	0x0048		/* timer count register */
#define RL_MISSEDPKT	0x004C		/* Contador de pacotes perdidos */
#define RL_EECMD	0x0050		/* Comandos para a EEPROM */
#define RL_CFG0		0x0051		/* Registrador de configuração 0 */
#define RL_CFG1		0x0052		/* Registrador de configuração 1 */

#define RL_MEDIASTAT	0x0058		/* media status register (8139) */

#define RL_MII		0x005A		/* 8129 chip only */
#define RL_HALTCLK	0x005B
#define RL_MULTIINTR	0x005C		/* multiple interrupt */
#define RL_PCIREV	0x005E		/* PCI revision value */
					/* 005F reserved */
#define RL_TXSTAT_ALL	0x0060		/* TX status of all descriptors */

/* Direct PHY access registers only available on 8139 */
#define RL_BMCR		0x0062		/* PHY basic mode control */
#define RL_BMSR		0x0064		/* PHY basic mode status */
#define RL_ANAR		0x0066		/* PHY autoneg advert */
#define RL_LPAR		0x0068		/* PHY link partner ability */
#define RL_ANER		0x006A		/* PHY autoneg expansion */

#define RL_DISCCNT	0x006C		/* disconnect counter */
#define RL_FALSECAR	0x006E		/* false carrier counter */
#define RL_NWAYTST	0x0070		/* NWAY test register */
#define RL_RX_ER	0x0072		/* RX_ER counter */
#define RL_CSCFG	0x0074		/* CS configuration register */

/*
 ****************************************************************
 *	Configuração da Transmissão (RL_TXCFG)			*
 ****************************************************************
 */
#define RL_TXCFG_CLRABRT	0x00000001	/* retransmit aborted pkt */
#define RL_TXCFG_MAXDMA		0x00000700	/* max DMA burst size */
#define RL_TXCFG_CRCAPPEND	0x00010000	/* CRC append (0 = yes) */
#define RL_TXCFG_LOOPBKTST	0x00060000	/* loopback test */
#define RL_TXCFG_IFG		0x03000000	/* interframe gap */

#define RL_TXDMA_16BYTES	0x00000000
#define RL_TXDMA_32BYTES	0x00000100
#define RL_TXDMA_64BYTES	0x00000200
#define RL_TXDMA_128BYTES	0x00000300
#define RL_TXDMA_256BYTES	0x00000400
#define RL_TXDMA_512BYTES	0x00000500
#define RL_TXDMA_1024BYTES	0x00000600
#define RL_TXDMA_2048BYTES	0x00000700

#define RL_TX_NSLOTS		4		/* Nr de slots (pot. de 2) */
#define RL_TX_THRESH_INIT	96
#define RL_TXCFG_CONFIG		(RL_TXCFG_IFG|RL_TXDMA_256BYTES)

/*
 ****************************************************************
 *	Estado da transmissão					*
 ****************************************************************
 */
#define RL_TXSTAT_LENMASK	0x00001FFF
#define RL_TXSTAT_OWN		0x00002000
#define RL_TXSTAT_TX_UNDERRUN	0x00004000
#define RL_TXSTAT_TX_OK		0x00008000
#define RL_TXSTAT_EARLY_THRESH	0x003F0000
#define RL_TXSTAT_COLLCNT	0x0F000000
#define RL_TXSTAT_CARR_HBEAT	0x10000000
#define RL_TXSTAT_OUTOFWIN	0x20000000
#define RL_TXSTAT_TXABRT	0x40000000
#define RL_TXSTAT_CARRLOSS	0x80000000

#define	RL_TX_DONE		(RL_TXSTAT_TX_OK|RL_TXSTAT_TX_UNDERRUN|	\
				 RL_TXSTAT_TXABRT)

/*
 ****************************************************************
 *	Estado da interrupção					*
 ****************************************************************
 */
#define RL_ISR_RX_OK		0x0001
#define RL_ISR_RX_ERR		0x0002
#define RL_ISR_TX_OK		0x0004
#define RL_ISR_TX_ERR		0x0008
#define RL_ISR_RX_OVERRUN	0x0010
#define RL_ISR_PKT_UNDERRUN	0x0020
#define RL_ISR_FIFO_OFLOW	0x0040	/* 8139 only */
#define RL_ISR_PCS_TIMEOUT	0x4000	/* 8129 only */
#define RL_ISR_SYSTEM_ERR	0x8000

/*
 ******	Interrupções habilitadas ********************************
 */
#define RL_INTRS							\
	(RL_ISR_TX_OK|RL_ISR_RX_OK|RL_ISR_RX_ERR|RL_ISR_TX_ERR|		\
	RL_ISR_RX_OVERRUN|RL_ISR_PKT_UNDERRUN|RL_ISR_FIFO_OFLOW|	\
	RL_ISR_PCS_TIMEOUT|RL_ISR_SYSTEM_ERR)

/*
 ****************************************************************
 *	"Media status" (apenas para a 8139)			*
 ****************************************************************
 */
#define RL_MEDIASTAT_RXPAUSE	0x01
#define RL_MEDIASTAT_TXPAUSE	0x02
#define RL_MEDIASTAT_LINK	0x04
#define RL_MEDIASTAT_SPEED10	0x08
#define RL_MEDIASTAT_RXFLOWCTL	0x40	/* duplex mode */
#define RL_MEDIASTAT_TXFLOWCTL	0x80	/* duplex mode */

/*
 ****************************************************************
 *	Configuração da Recepção				*
 ****************************************************************
 */
#define RL_RXCFG_RX_ALLPHYS	0x00000001	/* accept all nodes */
#define RL_RXCFG_RX_INDIV	0x00000002	/* match filter */
#define RL_RXCFG_RX_MULTI	0x00000004	/* accept all multicast */
#define RL_RXCFG_RX_BROAD	0x00000008	/* accept all broadcast */
#define RL_RXCFG_RX_RUNT	0x00000010
#define RL_RXCFG_RX_ERRPKT	0x00000020
#define RL_RXCFG_WRAP		0x00000080
#define RL_RXCFG_MAXDMA		0x00000700
#define RL_RXCFG_BUFSZ		0x00001800
#define RL_RXCFG_FIFOTHRESH	0x0000E000
#define RL_RXCFG_EARLYTHRESH	0x07000000

#define RL_RXDMA_16BYTES	0x00000000
#define RL_RXDMA_32BYTES	0x00000100
#define RL_RXDMA_64BYTES	0x00000200
#define RL_RXDMA_128BYTES	0x00000300
#define RL_RXDMA_256BYTES	0x00000400
#define RL_RXDMA_512BYTES	0x00000500
#define RL_RXDMA_1024BYTES	0x00000600
#define RL_RXDMA_UNLIMITED	0x00000700

#define RL_RXBUF_8		0x00000000
#define RL_RXBUF_16		0x00000800
#define RL_RXBUF_32		0x00001000
#define RL_RXBUF_64		0x00001800

#define RL_RXFIFO_16BYTES	0x00000000
#define RL_RXFIFO_32BYTES	0x00002000
#define RL_RXFIFO_64BYTES	0x00004000
#define RL_RXFIFO_128BYTES	0x00006000
#define RL_RXFIFO_256BYTES	0x00008000
#define RL_RXFIFO_512BYTES	0x0000A000
#define RL_RXFIFO_1024BYTES	0x0000C000
#define RL_RXFIFO_NOTHRESH	0x0000E000

#define RL_RX_BUF_SZ		RL_RXBUF_64
#define RL_RXBUFLEN		(1 << ((RL_RX_BUF_SZ >> 11) + 13))
#define RL_RX_FIFOTHRESH	RL_RXFIFO_256BYTES

#define RL_RXCFG_CONFIG		(RL_RX_FIFOTHRESH|RL_RXDMA_256BYTES|RL_RX_BUF_SZ)

/*
 ******	Bits no cabeçalho do pacote recebido ********************
 */
#define RL_RXSTAT_RXOK		0x00000001
#define RL_RXSTAT_ALIGNERR	0x00000002
#define RL_RXSTAT_CRCERR	0x00000004
#define RL_RXSTAT_GIANT		0x00000008
#define RL_RXSTAT_RUNT		0x00000010
#define RL_RXSTAT_BADSYM	0x00000020
#define RL_RXSTAT_BROAD		0x00002000
#define RL_RXSTAT_INDIV		0x00004000
#define RL_RXSTAT_MULTI		0x00008000
#define RL_RXSTAT_LENMASK	0xFFFF0000

#define RL_RXSTAT_UNFINISHED	0xFFF0		/* DMA still in progress */

#define	RL_RXERROR		(RL_RXSTAT_BADSYM|RL_RXSTAT_RUNT|	\
				 RL_RXSTAT_GIANT|RL_RXSTAT_CRCERR|	\
				 RL_RXSTAT_ALIGNERR)

/*
 ****************************************************************
 *	Comandos (RL_COMMAND)					*
 ****************************************************************
 */
#define RL_CMD_EMPTY_RXBUF	0x0001
#define RL_CMD_TX_ENB		0x0004
#define RL_CMD_RX_ENB		0x0008
#define RL_CMD_RESET		0x0010

/*
 ****************************************************************
 *	Controle da EEPROM					*
 ****************************************************************
 */
#define RL_EE_DATAOUT		0x01	/* Data out */
#define RL_EE_DATAIN		0x02	/* Data in */
#define RL_EE_CLK		0x04	/* clock */
#define RL_EE_SEL		0x08	/* chip select */
#define RL_EE_MODE		(0x40|0x80)

#define RL_EEMODE_OFF		0x00
#define RL_EEMODE_AUTOLOAD	0x40
#define RL_EEMODE_PROGRAM	0x80
#define RL_EEMODE_WRITECFG	(0x80|0x40)

/* 9346 EEPROM commands */
#define RL_EECMD_WRITE		0x140
#define RL_EECMD_READ_6_BIT	0x180
#define RL_EECMD_READ_8_BIT	0x600
#define RL_EECMD_ERASE		0x1C0

#define RL_EE_ID		0x00
#define RL_EE_PCI_VID		0x01
#define RL_EE_PCI_DID		0x02
/* Location of station address inside EEPROM */
#define RL_EE_EADDR		0x07

/*
 ****************************************************************
 *	Bits do registrador CFG0				*
 ****************************************************************
 */
#define RL_CFG0_ROM0		0x01
#define RL_CFG0_ROM1		0x02
#define RL_CFG0_ROM2		0x04
#define RL_CFG0_PL0		0x08
#define RL_CFG0_PL1		0x10
#define RL_CFG0_10MBPS		0x20	/* 10 Mbps internal mode */
#define RL_CFG0_PCS		0x40
#define RL_CFG0_SCR		0x80

/*
 ****************************************************************
 *	Bits do registrador CFG1				*
 ****************************************************************
 */
#define RL_CFG1_PWRDWN		0x01
#define RL_CFG1_SLEEP		0x02
#define RL_CFG1_IOMAP		0x04
#define RL_CFG1_MEMMAP		0x08
#define RL_CFG1_RSVD		0x10
#define RL_CFG1_DRVLOAD		0x20
#define RL_CFG1_LED0		0x40
#define RL_CFG1_FULLDUPLEX	0x40	/* 8129 only */
#define RL_CFG1_LED1		0x80

/*
 ****************************************************************
 *	Controle da MII (apenas para 8129)			*
 ****************************************************************
 */
#define RL_MII_CLK		0x01
#define RL_MII_DATAIN		0x02
#define RL_MII_DATAOUT		0x04
#define RL_MII_DIR		0x80	/* 0 == input, 1 == output */

/*
 ******	Interface para programação da MII ***********************
 */
typedef struct
{
	uchar		mii_stdelim;
	uchar		mii_opcode;
	uchar		mii_phyaddr;
	uchar		mii_regaddr;
	uchar		mii_turnaround;
	uchar		mii_data;

}	MII_FRAME;

#define RL_MII_STARTDELIM	0x01
#define RL_MII_READOP		0x02
#define RL_MII_WRITEOP		0x01
#define RL_MII_TURNAROUND	0x02

#define RL_8129			1
#define RL_8139			2

#define RL_TIMEOUT		1000

#define PCIM_CMD_PORTEN		0x0001
#define PCIM_CMD_MEMEN		0x0002
#define PCIM_CMD_BUSMASTEREN	0x0004
#define PCIM_CMD_PERRESPEN	0x0040

/*
 *	Códigos dos diversos fabricantes
 */
#define	RT_VENDORID		0x10EC
#define	RT_DEVICEID_8129	0x8129
#define	RT_DEVICEID_8139	0x8139
#define ACCTON_VENDORID		0x1113
#define ACCTON_DEVICEID_5030	0x1211
#define DELTA_VENDORID		0x1500
#define DELTA_DEVICEID_8139	0x1360
#define ADDTRON_VENDORID	0x4033

/*
 ****************************************************************
 *	Registradores para acesso ao barramento PCI		*
 ****************************************************************
 */
#define RL_PCI_LOIO		0x10
#define RL_PCI_LOMEM		0x14
#define RL_PCI_INTLINE		0x3C
#define RL_PCI_CAPID		0x50 /* 8 bits */
#define RL_PCI_PWRMGMTCTRL	0x54 /* 16 bits */

#define RL_PSTATE_MASK		0x0003

/*
 ****************************************************************
 *	Controle dos PHY (8139 apenas)				*
 ****************************************************************
 */
#define PHY_UNKNOWN		6

#define RL_PHYADDR_MIN		0x00
#define RL_PHYADDR_MAX		0x1F

#define PHY_BMCR		0x00
#define PHY_BMSR		0x01
#define PHY_VENID		0x02
#define PHY_DEVID		0x03
#define PHY_ANAR		0x04
#define PHY_LPAR		0x05
#define PHY_ANEXP		0x06

#define PHY_ANAR_NEXTPAGE	0x8000
#define PHY_ANAR_RSVD0		0x4000
#define PHY_ANAR_TLRFLT		0x2000
#define PHY_ANAR_RSVD1		0x1000
#define PHY_ANAR_RSVD2		0x0800
#define PHY_ANAR_RSVD3		0x0400
#define PHY_ANAR_100BT4		0x0200
#define PHY_ANAR_100BTXFULL	0x0100
#define PHY_ANAR_100BTXHALF	0x0080
#define PHY_ANAR_10BTFULL	0x0040
#define PHY_ANAR_10BTHALF	0x0020
#define PHY_ANAR_PROTO4		0x0010
#define PHY_ANAR_PROTO3		0x0008
#define PHY_ANAR_PROTO2		0x0004
#define PHY_ANAR_PROTO1		0x0002
#define PHY_ANAR_PROTO0		0x0001

/*
 *	PHY BMCR Basic Mode Control Register
 */
#define PHY_BMCR_RESET			0x8000
#define PHY_BMCR_LOOPBK			0x4000
#define PHY_BMCR_SPEEDSEL		0x2000
#define PHY_BMCR_AUTONEGENBL		0x1000
#define PHY_BMCR_RSVD0			0x0800	/* write as zero */
#define PHY_BMCR_ISOLATE		0x0400
#define PHY_BMCR_AUTONEGRSTR		0x0200
#define PHY_BMCR_DUPLEX			0x0100
#define PHY_BMCR_COLLTEST		0x0080
#define PHY_BMCR_RSVD1			0x0040	/* write as zero, don't care */
#define PHY_BMCR_RSVD2			0x0020	/* write as zero, don't care */
#define PHY_BMCR_RSVD3			0x0010	/* write as zero, don't care */
#define PHY_BMCR_RSVD4			0x0008	/* write as zero, don't care */
#define PHY_BMCR_RSVD5			0x0004	/* write as zero, don't care */
#define PHY_BMCR_RSVD6			0x0002	/* write as zero, don't care */
#define PHY_BMCR_RSVD7			0x0001	/* write as zero, don't care */
/*
 * RESET: 1 == software reset, 0 == normal operation
 * Resets status and control registers to default values.
 * Relatches all hardware config values.
 *
 * LOOPBK: 1 == loopback operation enabled, 0 == normal operation
 *
 * SPEEDSEL: 1 == 100Mb/s, 0 == 10Mb/s
 * Link speed is selected by this bit or if auto-negotiation if bit
 * 12 (AUTONEGENBL) is set (in which case the value of this register
 * is ignored).
 *
 * AUTONEGENBL: 1 == Autonegotiation enabled, 0 == Autonegotiation disabled
 * Bits 8 and 13 are ignored when autoneg is set, otherwise bits 8 and 13
 * determine speed and mode. Should be cleared and then set if PHY configured
 * for no autoneg on startup.
 *
 * ISOLATE: 1 == isolate PHY from MII, 0 == normal operation
 *
 * AUTONEGRSTR: 1 == restart autonegotiation, 0 = normal operation
 *
 * DUPLEX: 1 == full duplex mode, 0 == half duplex mode
 *
 * COLLTEST: 1 == collision test enabled, 0 == normal operation
 */

/* 
 * PHY, BMSR Basic Mode Status Register 
 */   
#define PHY_BMSR_100BT4			0x8000
#define PHY_BMSR_100BTXFULL		0x4000
#define PHY_BMSR_100BTXHALF		0x2000
#define PHY_BMSR_10BTFULL		0x1000
#define PHY_BMSR_10BTHALF		0x0800
#define PHY_BMSR_RSVD1			0x0400	/* write as zero, don't care */
#define PHY_BMSR_RSVD2			0x0200	/* write as zero, don't care */
#define PHY_BMSR_RSVD3			0x0100	/* write as zero, don't care */
#define PHY_BMSR_RSVD4			0x0080	/* write as zero, don't care */
#define PHY_BMSR_MFPRESUP		0x0040
#define PHY_BMSR_AUTONEGCOMP		0x0020
#define PHY_BMSR_REMFAULT		0x0010
#define PHY_BMSR_CANAUTONEG		0x0008
#define PHY_BMSR_LINKSTAT		0x0004
#define PHY_BMSR_JABBER			0x0002
#define PHY_BMSR_EXTENDED		0x0001

/*
 ****************************************************************
 *	Informações sobre as unidades				*
 ****************************************************************
 */
#define	IFF_PROMISC	0x0001
#define	IFF_OACTIVE	0x0002
#define	IFF_BROADCAST	0x0004
#define	IFF_RUNNING	0x0008
#define	IFF_ALLMULTI	0x0010

#define	IFM_FAILED	0x0001
#define	IFM_100_T4	0x0002
#define	IFM_10_T	0x0004
#define	IFM_100_TX	0x0008
#define	IFM_AUTO	0x0010
#define	IFM_FDX		0x0100
#define	IFM_HDX		0x0200

typedef	struct
{
	char		unit_alive;	/* Unidade presente */

	char		unit;		/* Número da unidade */
	char		vendor;		/* interface vendor */
	char		type;		/* interface type code */

	dev_t		dev;		/* Dispositivo */

	char		nopen;		/* No. de Opens */
	char		lock;		/* Uso exclusivo */

	uchar		phyaddr;
	char		hasphy;

	int		flags;		/* Indicadores (ver acima) */
	int		port;		/* Port base */
	int		irq;		/* No. do IRQ */

	int		eecmd_read;

	ETHADDR		ethaddr;	/* Endereço ETHERNET */

	void		*rxbuf,		/* "Ring buffer" para recepção */
			*rxbuf_alloc;

	int		txthresh;

	char		autoneg_cnt;	/* Auto-negociação */
	EVENT		autoneg_done;
	ushort		media;		/* Modo negociado */

	LOCK		tx_lock;	/* Semáforo para acesso aos slots */
	char		tx_busy,	/* Nr de slots ocupados */
			last_tx;	/* Último transmitido */

	ITBLOCK		*itblock[RL_TX_NSLOTS];
	char		*txarea[RL_TX_NSLOTS];

	/*
	 *	Diversos contadores
	 */
	int		nerrors;
	int		ierrors;
	int		oerrors;
	int		ncollisions;

	/*
	 *	Fila de ITBLOCKs a serem transmitidos
	 */
	LOCK		it_lock;	/* Semáforo da fila */

	ITBLOCK		*it_first;	/* Primeiro ITBLOCK */
	ITBLOCK		*it_last;	/* Último   ITBLOCK */

}	RTL;

/*
 ******	Variáveis globais ***************************************
 */
entry RTL	rtl[NED];	/* Tabela de unidades */

extern int	ed_busy;	/* Para verificar o uso */

extern ulong	pci_rtl_count;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void		rtlint (struct intr_frame);
void		rtl_stop (RTL *);
void		rtl_init (RTL *);
void		rtl_tx_init (RTL *);
void		rtl_start (RTL *);
void		rtl_reset (int, int);
void		rtl_rxeof (RTL *);
void		rtl_get_packet (RTL *, void *, int);
void		rtl_ring_copy (RTL *, void *, void *, int);
void		rtl_txeof (RTL *);

void		rtl_eeprom_putbyte (const RTL *, int);
ushort		rtl_eeprom_getword (const RTL *, int);
void		rtl_read_eeprom (const RTL *, ushort *, int, int);

void		rtl_mii_sync (int);
void		rtl_mii_send (int, ulong, int);
int		rtl_mii_readreg (RTL *, MII_FRAME *);
int		rtl_mii_writereg (RTL *, MII_FRAME *);

ushort		rtl_phy_readreg (RTL *, int);
void		rtl_phy_writereg (RTL *, int, int);

void		rtl_autoneg (int);

/*
 ****************************************************************
 *	Identifica RTLs PCI					*
 ****************************************************************
 */
char *
pci_rtl_probe (PCIDATA * tag, ulong type)
{
	switch (type)
	{
	    case 0x812910EC:
		return ("RealTek RTL8129 Fast ETHERNET");

	    case 0x813910EC:
		return ("RealTek RTL8139 Fast ETHERNET");

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_rtl_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
void
pci_rtl_attach (PCIDATA * pci, int unit)
{
	RTL		*rp = &rtl[unit];
	ulong		io_port, irq;
	ushort		devtype;
	int		i;
	ulong		command, membase;

	/*
	 *	Descobre o "port" e o "irq"
	 */
	command = pci_read (pci, RL_PCI_CAPID, 4) & 0xFF;

	if (command == 0x01)
	{
		command = pci_read (pci, RL_PCI_PWRMGMTCTRL, 4);

		if (command & RL_PSTATE_MASK)
		{
			/*
			 *	Save important PCI config data.
			 */
			io_port = pci_read (pci, RL_PCI_LOIO, 4);
			membase = pci_read (pci, RL_PCI_LOMEM, 4);
			irq     = pci_read (pci, RL_PCI_INTLINE, 4);

			command &= ~3;

			pci_write (pci, RL_PCI_PWRMGMTCTRL, command, 4);

			/*
			 *	Restore PCI config data.
			 */
			pci_write (pci, RL_PCI_LOIO,  io_port, 4);
			pci_write (pci, RL_PCI_LOMEM,   membase, 4);
			pci_write (pci, RL_PCI_INTLINE, irq, 4);
		}
	}

	/*
	 *	Mapeia os registradores de controle/estado
	 */
	command  = pci_read (pci, PCI_COMMAND_STATUS_REG, 4);
	command |= (PCIM_CMD_PORTEN|PCIM_CMD_MEMEN|PCIM_CMD_BUSMASTEREN);

	pci_write (pci, PCI_COMMAND_STATUS_REG, command, 4);

	command = pci_read (pci, PCI_COMMAND_STATUS_REG, 4);

	/*
	 *	Obtém o "port"
	 */
	if ((io_port = pci_read (pci, PCI_MAP_REG_START, 4)) == 0)
		return;

	io_port &= ~1;

	/*
	 *	Obtém o "irq"
	 */
	irq = pci->pci_intline;

	if (irq <= 0)
	{
		printf ("rtl%d: IRQ não obtido\n", unit);
		return;
	}

	rp->unit = unit;
	rp->port = io_port;
	rp->irq  = irq;

	/*
	 *	Dá o "reset" na placa
	 */
	rtl_reset (unit, io_port);

	/*
	 *	Obtém o comando de leitura
	 */
	rp->eecmd_read = RL_EECMD_READ_6_BIT;

	rtl_read_eeprom (rp, &devtype, RL_EE_ID, 1);

	if (devtype != 0x8129)
		rp->eecmd_read = RL_EECMD_READ_8_BIT;

	/*
	 *	Obtém o endereço ETHERNET
	 */
	rtl_read_eeprom (rp, (ushort *)rp->ethaddr.addr, RL_EE_EADDR, 3);

	/*
	 *	Obtém o tipo
	 */
	rtl_read_eeprom (rp, &devtype, RL_EE_PCI_DID, 1);

	switch (devtype)
	{
	    case RT_DEVICEID_8139:
	    case ACCTON_DEVICEID_5030:
	    case DELTA_DEVICEID_8139:
		rp->type   = RL_8139;
		rp->hasphy = 1;
		break;

	    case RT_DEVICEID_8129:
		rp->type   = RL_8129;
		rp->hasphy = 0;

		for (i = RL_PHYADDR_MIN; i < RL_PHYADDR_MAX + 1; i++)
		{
			rp->phyaddr = i;

			rtl_phy_writereg (rp, PHY_BMCR, PHY_BMCR_RESET);

			DELAY (500);

			while (rtl_phy_readreg (rp, PHY_BMCR) & PHY_BMCR_RESET)
				/* espera */;

			if (rtl_phy_readreg (rp, PHY_BMSR))
			{
				rp->hasphy++;
				break;
			}
		}

		if (rp->hasphy)
			printf ("rtl%d: endereço do PHY = %02X\n", unit, i);

		break;

	    default:
		printf ("rtl%d: tipo desconhecido: %x\n", unit, devtype);
		return;
	}

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (irq, PL, unit, rtlint) < 0)
		return;

	/*
	 *	Inicia a autonegociação (mas não espera concluir)
	 */
	rtl_autoneg (rp->unit);

	/*
	 *	Imprime o que encontrou
	 */
	printf
	(	"rtl: [%d: 0x%X, %d, RTL%s, <",
		unit, io_port, irq, rp->type == RL_8139 ? "8139" : "8129"
	);

	ether_pr_ether_addr (&rp->ethaddr);

	printf (">]\n");

	rp->unit_alive = 1;	/* a unidade realmente está ativa */

}	/* end pci_rtl_attach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
rtlopen (dev_t dev, int oflag)
{
	int		unit, i;
	RTL		*rp;

	/*
	 *	Prólogo
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NED)
		{ u.u_error = ENXIO; return; }

	rp = &rtl[unit];

	if (rp->unit_alive == 0)
		{ u.u_error = ENXIO; return; }

	if (rp->lock || (oflag & O_LOCK) && rp->nopen)
		{ u.u_error = EBUSY; return; }

	if (rp->nopen)
		{ rp->nopen++; return; }

	rp->dev = dev;

	/*
	 *	Inicia a autonegociação apenas se falhou anteriormente
	 */
	if (rp->media & IFM_FAILED)
		rtl_autoneg (rp->unit);

	EVENTWAIT (&rp->autoneg_done, PTTYOUT);

	if (rp->media == 0)
	{
		printf ("rtl%d: autonegociação não completada\n", unit);

		rp->media = IFM_FAILED;
		u.u_error = ENXIO;
		return;
	}

	/*
	 *	Aloca o buffer de recepção e as áreas auxiliares de transmissão
	 */
	if ((rp->rxbuf_alloc = malloc_byte (RL_RXBUFLEN + 32)) == NOVOID)
		{ u.u_error = ENOMEM; return; }

	if (((rp->txarea[0] = malloc_byte (RL_TX_NSLOTS * (2 * KBSZ)))) == NOSTR)
		{ free_byte (rp->rxbuf_alloc); u.u_error = ENOMEM; return; }

	for (i = 1; i < RL_TX_NSLOTS; i++)
		rp->txarea[i] = rp->txarea[i - 1] + 2 * KBSZ;

	rp->rxbuf = rp->rxbuf_alloc + 16;		/* Deixa uns bytes no início (?) */

	rp->flags = IFF_ALLMULTI;
#if (0)	/*******************************************************/
	rp->flags = IFF_BROADCAST|IFF_ALLMULTI;
#endif	/*******************************************************/

	/*
	 *	Inicializa a placa
	 */
	rtl_init (rp);

	/*
	 *	Epílogo
	 */
	rp->nopen++;

	if (oflag & O_LOCK)
		rp->lock = 1;

	printf ("rtlopen: unidade = %d", unit);

	if (rp->media & IFM_100_T4)
		printf (", 100baseT4");

	if (rp->media & IFM_HDX)
		printf (", half-duplex");

	if (rp->media & IFM_FDX)
		printf (", full-duplex");

	if (rp->media & IFM_10_T)
		printf (", 10Mbps");

	if (rp->media & IFM_100_TX)
		printf (", 100Mbps");

	putchar ('\n');

}	/* end rtlopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
rtlclose (dev_t dev, int flag)
{
	RTL		*rp;

	/*
	 *	Prólogo
	 */
	rp = &rtl[MINOR (dev)];

	if (--rp->nopen > 0)
		return;

	rtl_stop (rp);

	/*
	 *	Libera os buffers
	 */
	free_byte (rp->rxbuf_alloc);
	free_byte (rp->txarea[0]);

	rp->rxbuf     = rp->rxbuf_alloc = NOVOID;
	rp->txarea[0] = NOSTR;
	rp->lock      = 0;

}	/* end rtlclose */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
rtlwrite (IOREQ *iop)
{
	RTL		*rp;
	ITBLOCK		*bp;

	/*
	 *	Pequena inicialização
	 */
	rp = &rtl[MINOR (iop->io_dev)];

	bp = (ITBLOCK *)iop->io_area;

	/*
	 *	Insere o ITBLOCK na fila do DRIVER
	 */
	SPINLOCK (&rp->it_lock);

	if (rp->it_first == NOITBLOCK)
		rp->it_first = bp;
	else
		rp->it_last->it_forw = bp;

	rp->it_last = bp;
	bp->it_forw = NOITBLOCK;

#if (0)	/*******************************************************/
	ed_busy++;
#endif	/*******************************************************/

	SPINFREE (&rp->it_lock);

	/*
	 *	Inicia a operação
	 */
#if (0)	/*******************************************************/
	spled ();
#endif	/*******************************************************/
	splx (irq_pl_vec[rp->irq]);

	rtl_start (rp);

	spl0 ();

}	/* end rtlwrite */

/*
 ****************************************************************
 *	Inicia a transmissão					*
 ****************************************************************
 */
void
rtl_start (RTL *rp)
{
	ITBLOCK		*bp;
	char		*buf;
	int		cur_tx, len;

	/*
	 *	Verifica se já está transmitindo
	 */
	if (TAS (&rp->tx_lock) < 0)
		return;

	ed_busy++;

	cur_tx = (rp->last_tx + rp->tx_busy) & (RL_TX_NSLOTS - 1);

	/*
	 *	Ocupa os "slots" de transmissão com os ITBLOCKS da fila
	 */
	while (rp->tx_busy < RL_TX_NSLOTS)
	{
		/*
		 *	Retira um ITBLOCK da fila, se houver
		 */
		SPINLOCK (&rp->it_lock);

		if ((bp = rp->it_first) == NOITBLOCK)
		{
			SPINFREE (&rp->it_lock);
			break;
		}

		if ((rp->it_first = bp->it_forw) == NOITBLOCK)
		   	rp->it_last = NOITBLOCK;

		SPINFREE (&rp->it_lock);

		/*
		 *	Verifica se o buffer está alinhado corretamente
		 */
		len = bp->it_u_count;

		if ((int)bp->it_u_area & (sizeof (long) - 1))
		{
			memmove
			(	buf = rp->txarea[cur_tx],
				bp->it_u_area,
				len
			);
#define	DEBUG
#ifdef	DEBUG
			printf ("rtl%d: copiando %d bytes de %P para %P\n", rp->unit, len, bp->it_u_area, buf);
#endif	DEBUG
		}
		else
		{
			buf = (char *)bp->it_u_area;
		}

		/*
		 *	Verifica se tem o tamanho mínimo
		 */
		if (len < ETHER_MIN_LEN)
			len = ETHER_MIN_LEN;

		rp->itblock[cur_tx] = bp;

		/*
		 *	Envia o pacote
	 	 */
		write_port_long
		(	VIRT_TO_PHYS_ADDR (buf),
			rp->port + RL_TXADDR0 + (cur_tx << 2)
		);

		write_port_long
		(	(rp->txthresh << 11) | len,
			rp->port + RL_TXSTAT0 + (cur_tx << 2)
		);

		if (++cur_tx >= RL_TX_NSLOTS)
			cur_tx = 0;

		rp->tx_busy++;
	}

	ed_busy--;

	CLEAR (&rp->tx_lock);

}	/* end rtl_start */

/*
 ****************************************************************
 *	Aborta o que estiver em andamento			*
 ****************************************************************
 */
void
rtl_stop (RTL *rp)
{
	int		i;
	ITBLOCK		*bp;

	write_port       (0, rp->port + RL_COMMAND);
	write_port_short (0, rp->port + RL_IMR);

	/*
	 *	Libera os descritores de transmissão
	 */
	for (i = 0; i < RL_TX_NSLOTS; i++)
	{
		if ((bp = rp->itblock[i]) != NOITBLOCK)
		{
			bp->it_in_driver_queue = 0;

			if (bp->it_free_after_IO)
				put_it_block (bp);

			rp->itblock[i] = NOITBLOCK;
		}

		write_port_long (0, rp->port + RL_TXADDR0 + (i << 2));
	}

}	/* end rtl_stop */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
rtlctl (dev_t dev, int cmd, void *arg, ...)
{
	RTL		*rp = &rtl[MINOR (dev)];

	/*
	 *	Executa os comandos pedidos
	 */
	switch (cmd)
	{
	    case TCINTERNET:		/* É um dispositivo INTERNET */
		return (0);

	    case TC_IS_ETHERNET:	/* É um dispositivo ETHERNET */
		return (0);

	    case TC_GET_ETHADDR:	/* Obtém o endereço ETHERNET */
		if (unimove (arg, &rp->ethaddr, sizeof (ETHADDR), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

	    default:			/* Nos outros casos, ... */
		{ u.u_error = EINVAL; return (-1); }

	}	/* end switch */

}	/* end rtlctl */

/*
 ****************************************************************
 *	Função de interrupção					*
 ****************************************************************
 */
void
rtlint (struct intr_frame frame)
{
	RTL		*rp;
	ushort		status;

	rp = &rtl[frame.if_unit];

	if (CSWT (29))
		printf ("rtl%d: Veio interrupção\n", rp->unit);

	/*
	 *	Desabilita interrupções
	 */
#if (0)	/*******************************************************/
	write_port_short (0, rp->port + RL_IMR);
#endif	/*******************************************************/

	for (EVER)
	{
		/*
		 *	Analisa a causa da interrupção
		 */
		status = read_port_short (rp->port + RL_ISR);

		if (status == 0xFFFF)
			break;

#if (0)	/*******************************************************/
#endif	/*******************************************************/
		if (status)
			write_port_short (status, rp->port + RL_ISR);

		if ((status & RL_INTRS) == 0)
			break;

		if (status & RL_ISR_RX_OK)
			rtl_rxeof (rp);

if (status & RL_ISR_RX_ERR)
printf ("%g: Erro de leitura\n");

if (status & RL_ISR_TX_ERR)
printf ("%g: Erro de escrita\n");

		if (status & RL_ISR_RX_ERR)
			rtl_rxeof (rp);

		if (status & (RL_ISR_TX_OK|RL_ISR_TX_ERR))
			rtl_txeof (rp);

		if (status & RL_ISR_SYSTEM_ERR)
		{
printf ("%g: ISR_SYSTEM_ERR\n");
			rtl_reset (rp->unit, rp->port);
			rtl_init  (rp);
		}
	}

	/*
	 *	Reabilita interrupções e inicia a transmissão
	 */
#if (0)	/*******************************************************/
	write_port_short (RL_INTRS, rp->port + RL_IMR);
#endif	/*******************************************************/

	rtl_start (rp);

}	/* end rtlint */

/*
 ****************************************************************
 *	Processa a interrupção de recepção			*
 ****************************************************************
 */
void
rtl_rxeof (RTL *rp)
{
	int		total_len = 0;
	ulong		rxstat;
	void		*rxbufpos;
	int		cur_rx, limit;
	int		rx_bytes = 0, max_bytes;

	/*
	 *	Obtém a posição no buffer onde o pacote foi depositado
	 */
	cur_rx = (read_port_short (rp->port + RL_CURRXADDR) + 16) % RL_RXBUFLEN;
	limit  = (read_port_short (rp->port + RL_CURRXBUF)) % RL_RXBUFLEN;

	if (limit < cur_rx)
		max_bytes = (RL_RXBUFLEN - cur_rx) + limit;
	else
		max_bytes = limit - cur_rx;

	while ((read_port (rp->port + RL_COMMAND) & RL_CMD_EMPTY_RXBUF) == 0)
	{
		/*
		 *	Os quatro bytes iniciais contêm um "cabeçalho"
		 */
		rxbufpos  = rp->rxbuf + cur_rx;
		rxstat    = *(ulong *)rxbufpos;
		total_len = rxstat >> 16;

		/*
		 *	Here's a totally undocumented fact for you. When the
		 *	RealTek chip is in the process of copying a packet into
		 *	RAM for you, the length will be 0xfff0. If you spot a
		 *	packet header with this value, you need to stop. The
		 *	datasheet makes absolutely no mention of this and
		 *	RealTek should be shot for this.
		 */
		if (total_len == RL_RXSTAT_UNFINISHED)
			break;
	
		if ((rxstat & RL_RXSTAT_RXOK) == 0)
		{
printf ("%g: Erro de RL_RXSTAT_RXOK\n");
			rp->nerrors++;

			if (rxstat & RL_RXERROR)
			{
				write_port_short (RL_CMD_TX_ENB, rp->port + RL_COMMAND);
				write_port_short (RL_CMD_TX_ENB|RL_CMD_RX_ENB, rp->port + RL_COMMAND);
				write_port_long (RL_RXCFG_CONFIG, rp->port + RL_RXCFG);
				write_port_long (VIRT_TO_PHYS_ADDR (rp->rxbuf), rp->port + RL_RXADDR);
				write_port_short (cur_rx - 16, rp->port + RL_CURRXADDR);

				cur_rx = 0;
			}

			break;
		}

		/*
		 *	Não houve erros: recebe o pacote
		 */
		rx_bytes += total_len + 4;

		if (rx_bytes > max_bytes)
			break;

		/*
		 *	Copia o pacote recebido para um ITBLOCK
		 */
		rxbufpos = rp->rxbuf + ((cur_rx + sizeof (long)) % RL_RXBUFLEN);

		if (rxbufpos == (rp->rxbuf + RL_RXBUFLEN))
			rxbufpos = rp->rxbuf;

		rtl_get_packet (rp, rxbufpos, total_len - ETHER_CRC_LEN);

		/*
		 *	Calcula a próxima posição
		 */
		cur_rx = (((cur_rx + total_len + 4) % RL_RXBUFLEN) + 3) & ~3;
		write_port_short (cur_rx - 16, rp->port + RL_CURRXADDR);
	}

}	/* end rtl_rxeof */

/*
 ****************************************************************
 *	Lê um pacote e entrega para o nível superior		*
 ****************************************************************
 */
void
rtl_get_packet (RTL *rp, void *area, int count)
{
	ITBLOCK		*bp;

	/*
	 *	Supõe "count" bem comportado ( <= 1500 + 14)
	 *
	 *	Em primeiro lugar, obtém um ITBLOCK
	 */
	if ((bp = get_it_block (IT_IN)) == NOITBLOCK)
	{
#undef	MSG
#ifdef	MSG
		printf ("%g: Não obtive ITBLOCK");
#endif	MSG
		return;
	}

	/*
	 *	Obtém o pacote (com o cabeçalho ETHERNET) do controlador
	 */
	ed_busy++;

	rtl_ring_copy (rp, area, bp->it_frame, count);

	bp->it_dev	 = rp->dev;
	bp->it_u_area	 = bp->it_frame;
	bp->it_u_count	 = count;
	bp->it_ether_dev = 1;
	bp->it_ppp_dev   = 0;

	wake_up_in_daemon (IN_BP, bp);

	ed_busy--;

}	/* rtl_get_packet */

/*
 ****************************************************************
 *	Copia do "ring buffer" para um buffer linear		*
 ****************************************************************
 */
void
rtl_ring_copy (RTL *rp, void *src, void *dst, int count)
{
	int		tmp_count;
	void		*bend = rp->rxbuf + RL_RXBUFLEN;

	if (CSWT (29))
		printf ("rtl_ring_copy: Count = %d\n", count);

	if (src + count > bend)
	{
		tmp_count = bend - src;

		memmove (dst, src, tmp_count);

		count -= tmp_count;
		src    = rp->rxbuf;
		dst   += tmp_count;
	}

	memmove (dst, src, count);

}	/* end rtl_ring_copy */

/*
 ****************************************************************
 *	Processa a interrupção de fim de transmissão		*
 ****************************************************************
 */
void
rtl_txeof (RTL *rp)
{
	ulong		txstat;
	ITBLOCK		*bp;

	/*
	 *	Verifica os ITBLOCKs já transmitidos
	 */
	for (EVER)
	{
		SPINLOCK (&rp->tx_lock);

		if (rp->tx_busy <= 0)
			break;

		txstat = read_port_long (rp->port + RL_TXSTAT0 + (rp->last_tx << 2));

		if ((txstat & RL_TX_DONE) == 0)
			break;

		/*
		 *	A transmissão foi completada
		 */
		bp = rp->itblock[rp->last_tx];
#if (0)	/*******************************************************/
printf ("%g: Enviei pacote com %d bytes\n", bp->it_u_count);
#endif	/*******************************************************/

		rp->itblock[rp->last_tx] = NOITBLOCK;

		if (++rp->last_tx >= RL_TX_NSLOTS)
			rp->last_tx = 0;

		rp->tx_busy--;

		SPINFREE (&rp->tx_lock);

		/*
		 *	Libera o ITBLOCK
		 */
		bp->it_in_driver_queue = 0;

		if (bp->it_free_after_IO)
			put_it_block (bp);

		rp->ncollisions += (txstat & RL_TXSTAT_COLLCNT) >> 24;

		if ((txstat & RL_TXSTAT_TX_OK) == 0)
		{
			int		oldthresh;

			rp->oerrors++;
printf ("%g: TX %d erros\n", rp->oerrors);

			if (txstat & (RL_TXSTAT_TXABRT|RL_TXSTAT_OUTOFWIN))
				write_port_long (RL_TXCFG_CONFIG, rp->port + RL_TXCFG);

			oldthresh = rp->txthresh;

			/* error recovery */
			rtl_reset (rp->unit, rp->port);
			rtl_init (rp);

			/*
			 *	If there was a transmit underrun,
			 *	bump the TX threshold.
			 */
			if (txstat & RL_TXSTAT_TX_UNDERRUN)
				rp->txthresh = oldthresh + 32;

			return;
		}
	}

	SPINFREE (&rp->tx_lock);

}	/* end rtl_txeof */

/*
 ****************************************************************
 *	Dá o "reset" na placa					*
 ****************************************************************
 */
void
rtl_reset (int unit, int port)
{
	int		i;

	write_port (RL_CMD_RESET, port + RL_COMMAND);

	for (i = RL_TIMEOUT; i > 0; i--)
	{
		DELAY (10);

		if ((read_port (port + RL_COMMAND) & RL_CMD_RESET) == 0)
			return;
	}

	printf ("rtl%d: \"reset\" não foi completado\n", unit);

}	/* end rtl_reset */

/*
 ****************************************************************
 *	Reinicializa a placa					*
 ****************************************************************
 */
void
rtl_init (RTL *rp)
{
	int		s, port = rp->port;
	ulong		rxcfg = 0;
	ushort		phy_bmcr = 0;

	s = splx (irq_pl_vec[rp->irq]);

	/*
	 * XXX Hack for the 8139: the built-in autoneg logic's state
	 * gets reset by rtl_init() when we don't want it to. Try
	 * to preserve it. (For 8129 cards with real external PHYs,
	 * the BMCR register doesn't change, but this doesn't hurt.)
	 */
	if (rp->hasphy)
		phy_bmcr = rtl_phy_readreg (rp, PHY_BMCR);

	/*
	 *	Cancela as pendências
	 */
	rtl_stop (rp);

#if (0)	/*******************************************************/
{
	int		i;
	char		*cp;

	/*
	 *	Envia o endereço ETHERNET (que foi lido da placa!!!!)
	 */
	write_port (RL_EEMODE_WRITECFG, port + RL_EECMD);

	for (cp = &rp->ethaddr.addr[0], i = 0; i < sizeof (ETHADDR); i++, cp++)
		write_port (*cp, port + RL_IDR0 + i);

	write_port (RL_EEMODE_OFF, port + RL_EECMD);
}
#endif	/*******************************************************/

	/*
	 *	Envia o endereço (físico) do "ring buffer" de recepção
	 */
	write_port_long (VIRT_TO_PHYS_ADDR (rp->rxbuf), port + RL_RXADDR);

	rtl_tx_init (rp);

	/*
	 *	Habilita transmissão e recepção
	 */
	write_port (RL_CMD_TX_ENB|RL_CMD_RX_ENB, port + RL_COMMAND);

	/*
	 *	Configura a transmissão
	 */
	write_port_long (RL_TXCFG_CONFIG, port + RL_TXCFG);

#undef	IGNORE_MULTICAST
#ifdef	IGNORE_MULTICAST
	/*
	 *	Configura a recepção
	 */
	rxcfg  = RL_RXCFG_CONFIG | RL_RXCFG_RX_ALLPHYS | RL_RXCFG_RX_BROAD;
   /***	rxcfg &= ~RL_RXCFG_RX_MULTI; ***/

	write_port_long (rxcfg, rp->port + RL_RXCFG);

	write_port_long (0,     rp->port + RL_MAR0);
	write_port_long (0,     rp->port + RL_MAR4);
#else
	/*
	 *	Configura o tipo de pacote a receber
	 */
	write_port_long (RL_RXCFG_CONFIG, port + RL_RXCFG);

	rxcfg = read_port_long (port + RL_RXCFG);
	rxcfg |= RL_RXCFG_RX_INDIV;

	if (rp->flags & IFF_PROMISC)
		rxcfg |= RL_RXCFG_RX_ALLPHYS;
	else
		rxcfg &= ~RL_RXCFG_RX_ALLPHYS;

	write_port_long (rxcfg, port + RL_RXCFG);

	if (rp->flags & IFF_BROADCAST)
		rxcfg |= RL_RXCFG_RX_BROAD;
	else
		rxcfg &= ~RL_RXCFG_RX_BROAD;

	write_port_long (rxcfg, port + RL_RXCFG);

	/*
	 *	Recebe os pacotes multicast
	 */
	rxcfg |= RL_RXCFG_RX_MULTI;

	write_port_long (rxcfg, rp->port + RL_RXCFG);
	write_port_long (0xFFFFFFFF,     rp->port + RL_MAR0);
	write_port_long (0xFFFFFFFF,     rp->port + RL_MAR4);

#endif	IGNORE_MULTICAST

	/*
	 *	Habilita as interrupções
	 */
	write_port_short (RL_INTRS, port + RL_IMR);

	/* Set initial TX threshold */

	rp->txthresh = RL_TX_THRESH_INIT;

	/* Start RX/TX process */

	write_port_long (0, port + RL_MISSEDPKT);

#if (0)	/*******************************************************/
	/*
	 *	Habilita transmissão e recepção
	 */
	write_port (RL_CMD_TX_ENB|RL_CMD_RX_ENB, port + RL_COMMAND);
#endif	/*******************************************************/

	/* Restore state of BMCR */

	if (rp->hasphy)
		rtl_phy_writereg (rp, PHY_BMCR, phy_bmcr);

	write_port (RL_CFG1_DRVLOAD|RL_CFG1_FULLDUPLEX, port + RL_CFG1);

	splx (s);

}	/* end rtl_init */

/*
 ****************************************************************
 *	Inicializa os descritores de transmissão		*
 ****************************************************************
 */
void
rtl_tx_init (RTL *rp)
{
	int		i;

	for (i = 0; i < RL_TX_NSLOTS; i++)
	{
		rp->itblock[i] = NOITBLOCK;

		write_port_long (0, rp->port + RL_TXADDR0 + (i << 2));
	}

	rp->tx_busy = rp->last_tx = 0;

}	/* end rtl_tx_init */

/*
 ****************************************************************
 *	Envia dados para a EEPROM				*
 ****************************************************************
 */
#define EE_SET(x)					\
	write_port (read_port (rp->port + RL_EECMD) | (x), rp->port + RL_EECMD)

#define EE_CLR(x)					\
	write_port (read_port (rp->port + RL_EECMD) & (~(x)), rp->port + RL_EECMD)

void
rtl_eeprom_putbyte (const RTL *rp, int addr)
{
	int		d, i;

	d = addr | rp->eecmd_read;

	/*
	 *	Envia "bit" a "bit".
	 */
	for (i = 0x400; i != 0; i >>= 1)
	{
		if (d & i)
			EE_SET (RL_EE_DATAIN);
		else
			EE_CLR (RL_EE_DATAIN);

		DELAY (100);
		EE_SET (RL_EE_CLK);
		DELAY (150);
		EE_CLR (RL_EE_CLK);
		DELAY (100);
	}

}	/* end rtl_eeprom_putbyte */

/*
 ****************************************************************
 *	Lê um "short" da EEPROM					*
 ****************************************************************
 */
ushort
rtl_eeprom_getword (const RTL *rp, int addr)
{
	int		i;
	ushort		word = 0;

	/*
	 *	Habilita o acesso à EEPROM.
	 */
	write_port (RL_EEMODE_PROGRAM|RL_EE_SEL, rp->port + RL_EECMD);

	/*
	 *	Envia o endereço a ser acessado.
	 */
	rtl_eeprom_putbyte (rp, addr);

	write_port (RL_EEMODE_PROGRAM|RL_EE_SEL, rp->port + RL_EECMD);

	/*
	 *	Inicia a leitura dos bits.
	 */
	for (i = 0x8000; i != 0; i >>= 1)
	{
		EE_SET (RL_EE_CLK);
		DELAY (100);

		if (read_port (rp->port + RL_EECMD) & RL_EE_DATAOUT)
			word |= i;

		EE_CLR (RL_EE_CLK);
		DELAY (100);
	}

	/*
	 *	Desabilita o acesso à EEPROM.
	 */
	write_port (RL_EEMODE_OFF, rp->port + RL_EECMD);

	return (word);

}	/* end rtl_eeprom_getword */

/*
 ****************************************************************
 *	Lê uma seqüência de "shorts" da EEPROM			*
 ****************************************************************
 */
void
rtl_read_eeprom (const RTL *rp, ushort *addr, int off, int count)
{
	ushort		*endaddr;

	for (endaddr = addr + count; addr < endaddr; addr++, off++)
		*addr = rtl_eeprom_getword (rp, off);

}	/* end rtl_read_eeprom */

/*
 ****************************************************************
 *	Rotinas de acesso a MII					*
 ****************************************************************
 */
#define MII_SET(x)					\
	write_port (read_port (port + RL_MII) | (x), port + RL_MII)

#define MII_CLR(x)					\
	write_port (read_port (port + RL_MII) & (~(x)), port + RL_MII)

/*
 ****************************************************************
 *	Gera 32 pulsos quadrados em MII_CLK			*
 ****************************************************************
 */
void
rtl_mii_sync (int port)
{
	int		i;

	MII_SET (RL_MII_DIR | RL_MII_DATAOUT);

	for (i = 0; i < 32; i++)
	{
		MII_SET (RL_MII_CLK);
		DELAY (1);
		MII_CLR (RL_MII_CLK);
		DELAY (1);
	}

}	/* end rtl_mii_sync */

/*
 ****************************************************************
 *	Envia os bits para o MII				*
 ****************************************************************
 */
void
rtl_mii_send (int port, ulong bits, int cnt)
{
	int		i;

	MII_CLR (RL_MII_CLK);

	for (i = (1 << (cnt - 1)); i != 0; i >>= 1)
	{
                if (bits & i)
			MII_SET (RL_MII_DATAOUT);
                else
			MII_CLR (RL_MII_DATAOUT);

		DELAY (1);
		MII_CLR (RL_MII_CLK);
		DELAY (1);
		MII_SET (RL_MII_CLK);
	}

}	/* end rtl_mii_send */

/*
 ****************************************************************
 *	Lê de um registrador PHY através do MII			*
 ****************************************************************
 */
int
rtl_mii_readreg (RTL *rp, MII_FRAME *frame)
{
	int		i, ack, s, port = rp->port;

	s = splx (irq_pl_vec[rp->irq]);

	/*
	 *	Set up frame for RX.
	 */
	frame->mii_stdelim    = RL_MII_STARTDELIM;
	frame->mii_opcode     = RL_MII_READOP;
	frame->mii_turnaround = 0;
	frame->mii_data       = 0;
	
	write_port_short (0, port + RL_MII);

	/*
 	 *	Turn on data xmit.
	 */
	MII_SET (RL_MII_DIR);

	rtl_mii_sync (port);

	/*
	 * Send command/address info.
	 */
	rtl_mii_send (port, frame->mii_stdelim, 2);
	rtl_mii_send (port, frame->mii_opcode,  2);
	rtl_mii_send (port, frame->mii_phyaddr, 5);
	rtl_mii_send (port, frame->mii_regaddr, 5);

	/* Idle bit */
	MII_CLR ((RL_MII_CLK|RL_MII_DATAOUT));
	DELAY (1);
	MII_SET (RL_MII_CLK);
	DELAY (1);

	/* Turn off xmit. */
	MII_CLR (RL_MII_DIR);

	/* Check for ack */
	MII_CLR (RL_MII_CLK);
	DELAY (1);
	MII_SET (RL_MII_CLK);
	DELAY (1);

	ack = read_port_short (port + RL_MII) & RL_MII_DATAIN;

	/*
	 * Now try reading data bits. If the ack failed, we still
	 * need to clock through 16 cycles to keep the PHY(s) in sync.
	 */
	if (ack)
	{
		for (i = 0; i < 16; i++)
		{
			MII_CLR (RL_MII_CLK);
			DELAY (1);
			MII_SET (RL_MII_CLK);
			DELAY (1);
		}
		goto fail;
	}

	for (i = 0x8000; i != 0; i >>= 1)
	{
		MII_CLR (RL_MII_CLK);
		DELAY (1);

		if (!ack)
		{
			if (read_port_short (port + RL_MII) & RL_MII_DATAIN)
				frame->mii_data |= i;

			DELAY (1);
		}

		MII_SET (RL_MII_CLK);
		DELAY (1);
	}

    fail:
	MII_CLR (RL_MII_CLK);
	DELAY (1);
	MII_SET (RL_MII_CLK);
	DELAY (1);

	splx (s);

	return (ack);

}	/* end rtl_mii_readreg */

/*
 ****************************************************************
 *	Escreve em um registrador PHY através do MII		*
 ****************************************************************
 */
int
rtl_mii_writereg (RTL *rp, MII_FRAME *frame)
{
	int		s, port = rp->port;

	s = splx (irq_pl_vec[rp->irq]);

	/*
	 *	Set up frame for TX.
	 */
	frame->mii_stdelim    = RL_MII_STARTDELIM;
	frame->mii_opcode     = RL_MII_WRITEOP;
	frame->mii_turnaround = RL_MII_TURNAROUND;
	
	/*
 	 *	Turn on data output.
	 */
	MII_SET (RL_MII_DIR);

	rtl_mii_sync (port);

	rtl_mii_send (port, frame->mii_stdelim, 2);
	rtl_mii_send (port, frame->mii_opcode, 2);
	rtl_mii_send (port, frame->mii_phyaddr, 5);
	rtl_mii_send (port, frame->mii_regaddr, 5);
	rtl_mii_send (port, frame->mii_turnaround, 2);
	rtl_mii_send (port, frame->mii_data, 16);

	/* Idle bit. */
	MII_SET (RL_MII_CLK);
	DELAY (1);
	MII_CLR (RL_MII_CLK);
	DELAY (1);

	/*
	 *	Turn off xmit.
	 */
	MII_CLR (RL_MII_DIR);

	splx (s);

	return (0);

}	/* end rtl_mii_writereg */

/*
 ****************************************************************
 *	Lê dos registradores PHY				*
 ****************************************************************
 */
const int	phy_reg[] =
{
	RL_BMCR, RL_BMSR, -1, -1, RL_ANAR, RL_LPAR, -1
};

ushort
rtl_phy_readreg (RTL *rp, int reg)
{
	MII_FRAME	frame;

	if (rp->type == RL_8139)
	{
		int	phy;

		if ((phy = phy_reg[reg]) < 0)
		{
			printf ("rtl%d: registrador PHY inválido (%d)\n", rp->unit, reg);
			return (0);
		}

		return (read_port_short (rp->port + phy));
	}

	memclr (&frame, sizeof (frame));

	frame.mii_phyaddr = rp->phyaddr;
	frame.mii_regaddr = reg;

	rtl_mii_readreg (rp, &frame);

	return (frame.mii_data);

}	/* end rtl_phy_readreg */

/*
 ****************************************************************
 *	Escreve nos registradores PHY				*
 ****************************************************************
 */
void
rtl_phy_writereg (RTL *rp, int reg, int data)
{
	MII_FRAME		frame;

	if (rp->type == RL_8139)
	{
		int	phy;

		if ((phy = phy_reg[reg]) < 0)
		{
			printf ("rtl%d: registrador PHY inválido (%d)\n", rp->unit, reg);
			return;
		}

		write_port_short (data, rp->port + phy);
	}
	else
	{
		memclr (&frame, sizeof (frame));

		frame.mii_phyaddr = rp->phyaddr;
		frame.mii_regaddr = reg;
		frame.mii_data    = data;

		rtl_mii_writereg (rp, &frame);
	}

}	/* end rtl_phy_writereg */

/*
 ****************************************************************
 *	Autonegociação em um PHY				*
 ****************************************************************
 */
void
rtl_autoneg (int unit)
{
	RTL		*rp = &rtl[unit];
	ushort		bmcr = 0, advert, ability, bmsr;

	/*
	 * The 100baseT4 PHY sometimes has the 'autoneg supported'
	 * bit cleared in the status register, but has the 'autoneg enabled'
	 * bit set in the control register. This is a contradiction, and
	 * I'm not sure how to handle it. If you want to force an attempt
	 * to autoneg for 100baseT4 PHYs, #define FORCE_AUTONEG_TFOUR
	 * and see what happens.
	 */
#ifndef FORCE_AUTONEG_TFOUR
	/*
	 *	Verifica se suporta a autonegociação
	 */
	bmsr = rtl_phy_readreg (rp, PHY_BMSR);

	if ((bmsr & PHY_BMSR_CANAUTONEG) == 0)
	{
		printf ("rtl%d: autonegociação não suportada\n", unit);
		goto done;
	}
#endif

	if (rp->autoneg_cnt == 0)
	{
		/* Primeira chamada: inicia e retorna */

		rtl_phy_writereg (rp, PHY_BMCR, PHY_BMCR_RESET);

		DELAY (500);

		while (rtl_phy_readreg (rp, PHY_BMCR) & PHY_BMCR_RESET)
			/* espera */;

		bmcr  = rtl_phy_readreg (rp, PHY_BMCR);
		bmcr |= PHY_BMCR_AUTONEGENBL|PHY_BMCR_AUTONEGRSTR;

		rtl_phy_writereg (rp, PHY_BMCR, bmcr);

		toutset ((1 * scb.y_hz) >> 1, rtl_autoneg, unit);

		EVENTCLEAR (&rp->autoneg_done);

		rp->media = 0;

		rp->autoneg_cnt = 1;
		return;
	}

	/* Demais chamadas: após 1/2 segundo, verifica se já completou a autonegociação */

	bmsr = rtl_phy_readreg (rp, PHY_BMSR) & rtl_phy_readreg (rp, PHY_BMSR);

	if ((bmsr & PHY_BMSR_AUTONEGCOMP) == 0)
	{
	   	if (rp->autoneg_cnt++ > 10)
			goto done;

		toutset ((1 * scb.y_hz) >> 1, rtl_autoneg, unit);
		return;
	}

	if ((bmsr & PHY_BMSR_LINKSTAT) == 0)
		goto done;

	bmcr    = rtl_phy_readreg (rp, PHY_BMCR);
	advert  = rtl_phy_readreg (rp, PHY_ANAR);
	ability = rtl_phy_readreg (rp, PHY_LPAR);

	if (advert & PHY_ANAR_100BT4 && ability & PHY_ANAR_100BT4)
	{
		rp->media = IFM_100_T4;
		bmcr    |= PHY_BMCR_SPEEDSEL;
		bmcr    &= ~PHY_BMCR_DUPLEX;
	}
	elif (advert & PHY_ANAR_100BTXFULL && ability & PHY_ANAR_100BTXFULL)
	{
		rp->media = IFM_100_TX|IFM_FDX;
		bmcr    |= PHY_BMCR_SPEEDSEL;
		bmcr    |= PHY_BMCR_DUPLEX;
	}
	elif (advert & PHY_ANAR_100BTXHALF && ability & PHY_ANAR_100BTXHALF)
	{
		rp->media = IFM_100_TX|IFM_HDX;
		bmcr    |= PHY_BMCR_SPEEDSEL;
		bmcr    &= ~PHY_BMCR_DUPLEX;
	}
	elif (advert & PHY_ANAR_10BTFULL && ability & PHY_ANAR_10BTFULL)
	{
		rp->media = IFM_10_T|IFM_FDX;
		bmcr    &= ~PHY_BMCR_SPEEDSEL;
		bmcr    |= PHY_BMCR_DUPLEX;
	}
	else
	{
		rp->media = IFM_10_T|IFM_HDX;
		bmcr    &= ~PHY_BMCR_SPEEDSEL;
		bmcr    &= ~PHY_BMCR_DUPLEX;
	}

	rtl_phy_writereg (rp, PHY_BMCR, bmcr);

    done:
	rp->autoneg_cnt = 0;

	EVENTDONE (&rp->autoneg_done);

}	/* end rtl_autoneg */
