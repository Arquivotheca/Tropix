/*
 ****************************************************************
 *								*
 *			ed.c					*
 *								*
 *	"Driver" para controlador ETHERNET estilo 3Com 3c503	*
 *								*
 *	Versão	3.0.0, de 09.11.95				*
 *		4.5.0, de 17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/itnet.h"
#include "../h/ed.h"
#include "../h/tty.h"

#include "../h/pci.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 * Device driver for National Semiconductor DS8390/WD83C690 based ethernet
 *   adapters. By David Greenman, 29-April-1993
 *
 * Copyright (C) 1993, David Greenman. This software may be used, modified,
 *   copied, distributed, and sold, in both source and binary form provided
 *   that the above copyright and these terms are retained. Under no
 *   circumstances is the author responsible for the proper functioning
 *   of this software, nor does the author assume any responsibility
 *   for damages incurred with its use.
 *
 * Currently supports the Western Digital/SMC 8003 and 8013 series,
 *   the SMC Elite Ultra (8216), the 3Com 3c503, the NE1000 and NE2000,
 *   and a variety of similar clones.
 */

/*
 ******	Definições globais **************************************
 */
#define NED	4		/* Previsão para unidades ethernet */

#undef	DEBUG

#define PL	2		/* Prioridade de interrupção */
#define spled	spl2		/* Função de prioridade de interrupção */

/*
 ******	Estruturas de controle das unidades *********************
 */
typedef struct
{
	short	if_unit;	/* sub-unit for lower level driver */
	short	if_flags;	/* up/down, broadcast, etc. */

	dev_t	dev;		/* Dispositivo */

	int	if_irq;		/* No. do IRQ */

	char	*type_str;	/* pointer to type string */

	ETHADDR	ac_enaddr;	/* ethernet hardware address */

	char	nopen;		/* No. de Opens */
	char	lock;		/* Uso exclusivo */

	char	vendor;		/* interface vendor */
	char	type;		/* interface type code */
	char	unit_alive;	/* Unidade presente */

	ushort	asic_addr;	/* ASIC I/O bus address */
	ushort	nic_addr;	/* NIC (DS8390) I/O bus address */

	/*
	 *	The following 'proto' variable is part of a work-around for
	 *	8013EBT asics being write-only.
	 *	It's sort of a prototype/shadow of the real thing.
	 */
	char	wd_laar_proto;
	char	cr_proto;
	char	isa16bit;	/* width of access to card 0 = 8 or 1 = 16 */
	char	is790;		/* set by the probe code if the card is 790
				 * based */

	void	*mem_start;	/* NIC memory start address */
	void	*mem_end;	/* NIC memory end address */
	int	mem_size;	/* total NIC memory size */
	void	*mem_ring;	/* start of RX ring-buffer (in NIC mem) */

	char	mem_shared;	/* NIC memory is shared with host */
	char	xmit_busy;	/* transmitter is busy */
	char	txb_cnt;	/* number of transmit buffers */
	char	txb_inuse;	/* number of TX buffers currently in-use */

	char	txb_new;	/* pointer to where new buffer will be added */
	char	txb_next_tx;	/* pointer to next buffer ready to xmit */
	ushort	txb_len[8];	/* buffered xmit buffer lengths */
	char	tx_page_start;	/* first page of TX buffer area */
	char	rec_page_start;	/* first page of RX ring-buffer */
	char	rec_page_stop;	/* last page of RX ring-buffer */
	char	next_packet;	/* pointer to next unread RX packet */

	/*
	 *	Fila de ITBLOCKs
	 */
	LOCK	q_lock;		/* Semaforo da ITQ */
	EVENT	q_outqempty;	/* Fila de saída vazia */

	ITBLOCK	*q_first;	/* Ponteiro para o primeiro ITBLOCK */
	ITBLOCK	*q_last;	/* Ponteiro para o último ITBLOCK */

}	ED_STATUS;

entry ED_STATUS	ed_status[NED];

/*
 ******	Indicadores *********************************************
 */
#define	IFF_OACTIVE	0x400	/* transmission in progress */

/*
 ******	Variáveis globais ***************************************
 */
entry int	ed_busy;	/* Para verificar o uso */

extern ulong	pci_ed_count;

/*
 ******	Protótipos de funções ***********************************
 */
int		ed_port_test (int port, char data);
int		ed_attach_3Com (int, int, int);
int		ed_attach_novell (int, int, int);
void		ed_reset (int unit);
void		ed_stop (int unit);
void		ed_init (int unit);
void		ed_setrcr (ED_STATUS *);
void		ds_getmcaf (ED_STATUS *sc, ulong *mcaf);
ulong		ds_crc (char *ep, int n);
void		ed_start (int unit);
void		ed_xmit (int);
void		edint (struct intr_frame frame);
void		ed_rint (int unit);
void		edstart (TTY *tp, int flag);
void		ed_get_packet (ED_STATUS *, char *, int, int);
void		ed_pio_readmem (ED_STATUS *, char *, char *, int);
void		ed_pio_writemem (ED_STATUS *, char *, char *, int);
char		*ed_ring_copy (ED_STATUS *sc, void *src, void *dst, int amount);

/*
 ****************************************************************
 *	Identifica EDs ISA PnP					*
 ****************************************************************
 */
int
pnp_ed_probe (ulong id)
{
	switch (id)
	{
	    case 0x0090252A:
		return (1);
	}

	return (0);

}	/* end pnp_ed_probe */

/*
 ****************************************************************
 *	Anexa SBs ISA PnP					*
 ****************************************************************
 */
void
pnp_ed_attach (ushort *portp, uchar *irqp, uchar *dmap)
{
	SCB		*sp = &scb;
	int		unit, port, irq;

	port = portp[0];  irq = irqp[0];

	/*
	 *	Verifica se, no SCB, já existe uma entrada com o "port"
	 *	obtido no PnP
	 */
	for (unit = 0; unit < NED; unit++)
	{
		if (sp->y_ed_port[unit] != port)
			continue;

		/*
		 *	Encontrou: confere os IRQs
		 */
		if (sp->y_ed_irq[unit] != irq)
		{
			printf
			(	"pnp_ed_attach (unidade = %d): "
				"IRQs diferem (SCB = %d, PnP = %d)\n",
				unit, sp->y_ed_irq[unit], irq
			);

			sp->y_ed_irq[unit] = irq;
		}

		return;
	}

	/*
	 *	Não achou. Procura uma entrada vaga no SCB
	 */
	for (unit = 0; unit < NED; unit++)
	{
		if (sp->y_ed_port[unit] == 0)
		{
			sp->y_ed_port[unit] = port;
			sp->y_ed_irq[unit]  = irq;

			return;
		}
	}

	printf ("pnp_ed_attach: não há espaço no SCB\n");

}	/* end pnp_ed_attach */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
edattach (int major)
{
	SCB		*sp = &scb;
	int		unit, port, irq;

	for (unit = 0; unit < NED; unit++)
	{
		if ((port = sp->y_ed_port[unit]) == 0)
			continue;

		irq = sp->y_ed_irq[unit];

		ed_status[unit].unit_alive = 0;

		if (ed_attach_3Com (unit, port, irq) >= 0)
			{ pci_ed_count++; continue; }

		if (ed_attach_novell (unit, port, irq) >= 0)
			{ pci_ed_count++; continue; }

		printf ("edattach: Unidade %d não identificada\n", unit);
	}

	return (0);

}	/* edattach */

/*
 ****************************************************************
 *	Identifica EDs PCI					*
 ****************************************************************
 */
char *
pci_ed_probe (PCIDATA * tag, ulong type)
{
	switch (type)
	{
	    case 0x802910EC:
		return ("NE2000 PCI Ethernet (RealTek 8029)");

	    case 0x50004A14:
		return ("NE2000 PCI Ethernet (NetVin 5000)");

	    case 0x09401050:
		return ("NE2000 PCI Ethernet (ProLAN)");

	    case 0x140111F6:
		return ("NE2000 PCI Ethernet (Compex)");

	    case 0x30008E2E:
		return ("NE2000 PCI Ethernet (KTI)");

	    default:
		break;

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_ed_probe */

/*
 ****************************************************************
 *	Anexa EDs PCI						*
 ****************************************************************
 */
void
pci_ed_attach (PCIDATA * pci, int unit)
{
	ulong		io_port;
	int		irq;

	if ((io_port = pci_read (pci, PCI_MAP_REG_START, 4)) == 0)
		return;

	/*
	 *	The first bit of PCI_BASEADR0 is always
	 *	set hence we mask it off.
	 */
	io_port &= ~1;

#ifdef	DEBUG
	printf ("pci_ed_attach: ed%d: io_port = %X\n", unit, io_port);
#endif	DEBUG

	/*
	 *	Verifica se o número do controlador é válido
	 */
#ifdef	DEBUG
	printf ("pci_ed_attach: Número de controlador (unit): %d\n", unit);
#endif	DEBUG

	/*
	 *	Prepara a interrupção
	 */
	irq = pci->pci_intline;
#if (0)	/*******************************************************/
	irq = PCI_INTERRUPT_LINE_EXTRACT (pcibus_read (pci, PCI_INTERRUPT_REG));
#endif	/*******************************************************/

#ifdef	DEBUG
	printf ("pci_ed%d: IRQ %d\n", unit, irq);
#endif	DEBUG

	if (irq <= 0)
	{
		printf ("ed%d: int line register not set by bios\n", unit);
		return;
	}

#if (0)	/*******************************************************/
	if (irq >= PCI_MAX_IRQ)
	{
		printf ("ed%d: irq %d invalid\n", unit, irq);
		return;
	}
#endif	/*******************************************************/

	if (ed_attach_novell (unit, io_port, irq) < 0)
		printf ("ed%d: Unidade NÃO reconhecida\n", unit);

}	/* end pci_ed_attach */

/*
 ****************************************************************
 *	Procura os controladores 3Com 3c503			*
 ****************************************************************
 */
int
ed_attach_3Com (int unit, int iobase, int irq)
{
	ED_STATUS	*sc = &ed_status[unit];
	int		i;
	int		memsize;
	char		isa16bit;
	void		*maddr;

	sc->if_unit = unit;

	/*
	 *	Prepara os endereços das portas
	 */
	sc->asic_addr = iobase + ED_3COM_ASIC_OFFSET;
	sc->nic_addr =  iobase + ED_3COM_NIC_OFFSET;

	/*
	 *	Verifica que o controlador existe no endereço dado
	 */
	switch (read_port (sc->asic_addr + ED_3COM_BCFR))
	{
	    case ED_3COM_BCFR_300:
		if (iobase != 0x300)
			goto not_found;
		break;

	    case ED_3COM_BCFR_310:
		if (iobase != 0x310)
			goto not_found;
		break;

	    case ED_3COM_BCFR_330:
		if (iobase != 0x330)
			goto not_found;
		break;

	    case ED_3COM_BCFR_350:
		if (iobase != 0x350)
			goto not_found;
		break;

	    case ED_3COM_BCFR_250:
		if (iobase != 0x250)
			goto not_found;
		break;

	    case ED_3COM_BCFR_280:
		if (iobase != 0x280)
			goto not_found;
		break;

	    case ED_3COM_BCFR_2A0:
		if (iobase != 0x2a0)
			goto not_found;
		break;

	    case ED_3COM_BCFR_2E0:
		if (iobase != 0x2e0)
			goto not_found;
		break;

	    default:
	    not_found:
#ifdef	DEBUG
		printf ("ed%d: Não encontrado na porta 0x%X\n", unit, iobase);
#endif	DEBUG
		return (-1);

	}	/* end switch (port) */

#ifdef	DEBUG
	printf ("ed%d: Encontrado na porta 0x%X\n", unit, iobase);
#endif	DEBUG

	/*
	 *	Read the board configured address
	 */
	switch (read_port (sc->asic_addr + ED_3COM_PCFR))
	{
	    case ED_3COM_PCFR_DC000:
		maddr = (void *)0xDC000;
		break;

	    case ED_3COM_PCFR_D8000:
		maddr = (void *)0xD8000;
		break;

	    case ED_3COM_PCFR_CC000:
		maddr = (void *)0xCC000;
		break;

	    case ED_3COM_PCFR_C8000:
		maddr = (void *)0xC8000;
		break;

	    default:
		return (-1);

	}	/* end switch (memory) */

#ifdef	DEBUG
	printf ("ed%d: Endereço da memória compartilhada = 0x%X\n", unit, maddr);
#endif	DEBUG

	/*
	 *	Reset NIC and ASIC. Enable on-board transceiver
	 *	throughout reset sequence because it'll lock up
	 *	if the cable isn't connected if we don't.
	 */
	write_port (ED_3COM_CR_RST|ED_3COM_CR_XSEL, sc->asic_addr + ED_3COM_CR);

	/*
	 *	Wait for a while, then un-reset it
	 */
	DELAY (50);

	/*
	 *	The 3Com ASIC defaults to rather strange settings
	 *	for the CR after a reset - it's important to set
	 *	it again after the following write_port
	 *	(this is done when we map the PROM below).
	 */
	write_port (ED_3COM_CR_XSEL, sc->asic_addr + ED_3COM_CR);

	/*
	 *	Wait a bit for the NIC to recover from the reset
	 */
	DELAY (5000);

	sc->vendor	= ED_VENDOR_3COM;
	sc->type_str	= "3Com 3c503";
	sc->mem_shared	= 1;
	sc->cr_proto	= ED_CR_RD2;

	/*
	 *	Hmmm...a 16bit 3Com board has 16k of memory,
	 *	but only an 8k window to it.
	 */
	memsize = 8192;

	/*
	 *	Get station address from on-board ROM
	 */

	/*
	 *	First, map ethernet address PROM over the top
	 *	of where the NIC registers normally appear.
	 */
	write_port (ED_3COM_CR_EALO|ED_3COM_CR_XSEL, sc->asic_addr + ED_3COM_CR);

	for (i = 0; i < sizeof (ETHADDR); ++i)
		sc->ac_enaddr.addr[i] = read_port (sc->nic_addr + i);

#ifdef	DEBUG
	printf ("ed%d: Endereço ethernet = ", unit);
	ether_pr_ether_addr (&sc->ac_enaddr);
	printf ("\n");
#endif	DEBUG

	/*
	 *	Unmap PROM - select NIC registers. The proper setting of the
	 *	tranceiver is set in ed_init so that the attach code is given a
	 *	chance to set the default based on a compile-time config option
	 */
	write_port (ED_3COM_CR_XSEL, sc->asic_addr + ED_3COM_CR);

	/*
	 *	Determine if this is an 8bit or 16bit board
	 */

	/*
	 *	select page 0 registers
	 */
	write_port (ED_CR_RD2|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	/*
	 *	Attempt to clear WTS bit. If it doesn't clear,
	 *	then this is a 16bit board.
	 */
	write_port (0, sc->nic_addr + ED_P0_DCR);

	/*
	 *	select page 2 registers
	 */
	write_port (ED_CR_PAGE_2|ED_CR_RD2|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	/*
	 *	The 3c503 forces the WTS bit to a one if this is a 16bit board
	 */
	if (read_port (sc->nic_addr + ED_P2_DCR) & ED_DCR_WTS)
		isa16bit = 1;
	else
		isa16bit = 0;

	/*
	 *	Select page 0 registers
	 */
	write_port (ED_CR_RD2|ED_CR_STP, sc->nic_addr + ED_P2_CR);

	sc->mem_start = (void *)PHYS_TO_VIRT_ADDR (maddr);
	sc->mem_size = memsize;
	sc->mem_end = sc->mem_start + memsize;

	/*
	 *	We have an entire 8k window to put the transmit buffers
	 *	on the 16bit boards. But since the 16bit 3c503's shared
	 *	memory is only fast enough to overlap the loading of one
	 *	full-size packet, trying to load more than 2 buffers can
	 *	actually leave the transmitter idle during the load. So
	 *	2 seems the best value. (Although a mix of variable-sized
	 *	packets might change this assumption. Nonetheless, we
	 *	optimize for linear transfers of same-size packets.)
	 */
	if (isa16bit)
	{
#if (0)	/*******************************************************/
		if (isa_dev->id_flags & ED_FLAGS_NO_MULTI_BUFFERING)
			sc->txb_cnt = 1;
		else
#endif	/*******************************************************/
			sc->txb_cnt = 2;

		sc->tx_page_start  = ED_3COM_TX_PAGE_OFFSET_16BIT;
		sc->rec_page_start = ED_3COM_RX_PAGE_OFFSET_16BIT;
		sc->rec_page_stop  = memsize / ED_PAGE_SIZE +
			ED_3COM_RX_PAGE_OFFSET_16BIT;
		sc->mem_ring = sc->mem_start;
	}
	else
	{
		sc->txb_cnt = 1;
		sc->tx_page_start  = ED_3COM_TX_PAGE_OFFSET_8BIT;
		sc->rec_page_start = ED_TXBUF_SIZE + ED_3COM_TX_PAGE_OFFSET_8BIT;
		sc->rec_page_stop  = memsize / ED_PAGE_SIZE +
			ED_3COM_TX_PAGE_OFFSET_8BIT;
		sc->mem_ring = sc->mem_start + (ED_PAGE_SIZE * ED_TXBUF_SIZE);
	}

	sc->isa16bit = isa16bit;

#ifdef	DEBUG
	printf ("ed%d: Placa de %d bits\n", unit, isa16bit ? 16 : 8);
#endif	DEBUG

	/*
	 *	Initialize GA page start/stop registers.
	 *	Probably only needed if doing DMA, but what the hell.
	 */
	write_port (sc->rec_page_start, sc->asic_addr + ED_3COM_PSPR);
	write_port (sc->rec_page_stop,  sc->asic_addr + ED_3COM_PSPR);

	/*
	 *	Set IRQ. 3c503 only allows a choice of irq 2-5.
	 */
	switch (irq)
	{
	    case 2:
		write_port (ED_3COM_IDCFR_IRQ2, sc->asic_addr + ED_3COM_IDCFR);
		break;

	    case 3:
		write_port (ED_3COM_IDCFR_IRQ3, sc->asic_addr + ED_3COM_IDCFR);
		break;

	    case 4:
		write_port (ED_3COM_IDCFR_IRQ4, sc->asic_addr + ED_3COM_IDCFR);
		break;

	    case 5:
		write_port (ED_3COM_IDCFR_IRQ5, sc->asic_addr + ED_3COM_IDCFR);
		break;

	    default:
		printf
		(	"ed%d: Invalid irq configuration (%d) must be 2-5 for 3c503\n",
			unit, irq
		);
		return (-1);

	}	/* end switch (irq) */

#ifdef	DEBUG
	printf ("ed%d: Atribuído o IRQ = %d\n", unit, irq);
#endif	DEBUG

	/*
	 *	Initialize GA configuration register.
	 *	Set bank and enable shared mem.
	 */
	write_port (ED_3COM_GACFR_RSEL|ED_3COM_GACFR_MBS0, sc->asic_addr + ED_3COM_GACFR);

	/*
	 *	Initialize "Vector Pointer" registers. These gawd-awful
	 *	things are compared to 20 bits of the address on ISA,
	 *	and if they match, the shared memory is disabled.
	 *	We set them to 0xFFFF0...allegedly the reset vector.
	 */
	write_port (0xFF, sc->asic_addr + ED_3COM_VPTR2);
	write_port (0xFF, sc->asic_addr + ED_3COM_VPTR1);
	write_port (0x00, sc->asic_addr + ED_3COM_VPTR0);

	/*
	 *	Zero memory and verify that it is clear
	 */
	memset (sc->mem_start, 0, memsize);

	for (i = 0; i < memsize; ++i)
	{
		if (((char *)sc->mem_start)[i])
		{
			printf
			(	"ed%d: failed to clear shared memory at 0x%X - check configuration\n",
				unit, sc->mem_start + i
			);
			return (-1);
		}
	}

#ifdef	DEBUG
	printf ("ed%d: Teste de memória correto\n", unit);
#endif	DEBUG

	/*
	 *	Set interface to stopped condition (reset)
	 */
	ed_stop (unit);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (irq, PL, unit, edint) < 0)
		return (-1);

	printf
	(	"ed:  [%d: 0x%X, %d, %s, %s bits, <",
		unit, iobase, irq, sc->type_str, sc->isa16bit ? "16" : "8"
	);
	ether_pr_ether_addr (&sc->ac_enaddr);
	printf (">]\n");

	sc->if_irq	= irq;
	sc->unit_alive	= 1;

	return (0);

}	/* end ed_attach_3Com */

/*
 ****************************************************************
 *	Procura os controladores NOVELL (NE1000/NE2000)		*
 ****************************************************************
 */
int
ed_attach_novell (int unit, int iobase, int irq)
{
	ED_STATUS	*sc = &ed_status[unit];
	int		i, memsize;
	char		romdata[16], tmp;
	char		test_buffer[32];
	static char	test_pattern[32] = "THIS is A memory TEST pattern";

	sc->if_unit = unit;

	/*
	 *	Prepara os endereços das portas
	 */
	sc->asic_addr = iobase + ED_NOVELL_ASIC_OFFSET;
	sc->nic_addr =  iobase + ED_NOVELL_NIC_OFFSET;

	/* Reset the board */

	tmp = read_port (sc->asic_addr + ED_NOVELL_RESET);

	/*
	 *	I don't know if this is necessary; probably cruft leftover
	 *	from Clarkson packet driver code. Doesn't do a thing on the
	 *	boards I've tested. -DG [note that a write_port (0, 0x84)
	 * 	seems to work here, and is non-invasive...but some boards
	 *	don't seem to reset and I don't have complete documentation
	 *	on what the 'right' thing to do is...so we do the invasive
	 *	thing for now. Yuck.]
	 */
	write_port (tmp, sc->asic_addr + ED_NOVELL_RESET);
	DELAY (5000);

	/*
	 *	This is needed because some NE clones apparently don't reset
	 *	the NIC properly (or the NIC chip doesn't reset fully on
	 *	power-up) - this makes the probe invasive! ...Done against
	 *	my better judgement. -DLG
	 */
	write_port (ED_CR_RD2|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	DELAY (5000);

	/*
	 *	Make sure that we really have an 8390 based board
	 */
	if ((read_port (sc->nic_addr + ED_P0_CR) &
	   (ED_CR_RD2|ED_CR_TXP|ED_CR_STA|ED_CR_STP)) != (ED_CR_RD2|ED_CR_STP))
		return (-1);

#ifdef	DEBUG
	printf ("ed%d: 8390 fase 1 OK\n", unit);
#endif	DEBUG

	if ((read_port (sc->nic_addr + ED_P0_ISR) & ED_ISR_RST) != ED_ISR_RST)
		return (-1);

#ifdef	DEBUG
	printf ("ed%d: 8390 fase 2 OK\n", unit);
#endif	DEBUG

	sc->vendor	= ED_VENDOR_NOVELL;
   /**	sc->type_str	= "Novell NE1000/2000"; ***/
	sc->mem_shared	= 0;
	sc->cr_proto	= ED_CR_RD2;

	/*
	 *	Test the ability to read and write to the NIC memory.
	 *	This has the side affect of determining if this is an
	 *	NE1000 or an NE2000.
	 *
	 *	This prevents packets from being stored in the NIC memory
	 *	when the readmem routine turns on the start bit in the CR.
	 */
	write_port (ED_RCR_MON, sc->nic_addr + ED_P0_RCR);

	/* Temporarily initialize DCR for byte operations */

	write_port (ED_DCR_FT1|ED_DCR_LS, sc->nic_addr + ED_P0_DCR);

	write_port (8192  / ED_PAGE_SIZE, sc->nic_addr + ED_P0_PSTART);
	write_port (16384 / ED_PAGE_SIZE, sc->nic_addr + ED_P0_PSTOP);

	sc->isa16bit = 0;

	/*
	 *	Write a test pattern in byte mode. If this fails, then there
	 *	probably isn't any memory at 8k - which likely means that the
	 *	board is an NE2000.
	 */
	ed_pio_writemem (sc, test_pattern, (char *)8192, sizeof (test_pattern));
	ed_pio_readmem  (sc, (char *)8192, test_buffer,  sizeof (test_pattern));

#ifdef	DEBUG
	printf ("ed%d: test_pattern = \"%s\"\n", unit, test_buffer);
#endif	DEBUG

	if (memeq (test_pattern, test_buffer, sizeof (test_pattern)) == 0)
	{
#ifdef	DEBUG
		printf ("ed%d: NE1000 recusado\n", unit);
#endif	DEBUG

		/* Not an NE1000 - try NE2000 */

		write_port (ED_DCR_WTS|ED_DCR_FT1|ED_DCR_LS, sc->nic_addr + ED_P0_DCR);
		write_port (16384 / ED_PAGE_SIZE, sc->nic_addr + ED_P0_PSTART);
		write_port (32768 / ED_PAGE_SIZE, sc->nic_addr + ED_P0_PSTOP);

		sc->isa16bit = 1;

		/*
		 *	Write a test pattern in word mode. If this also fails,
		 *	then we don't know what this board is.
		 */
		ed_pio_writemem (sc, test_pattern, (char *)16384, sizeof (test_pattern));
		ed_pio_readmem  (sc, (char *)16384, test_buffer,  sizeof (test_pattern));

#ifdef	DEBUG
		printf ("ed%d: test_pattern = \"%s\"\n", unit, test_buffer);
#endif	DEBUG

		if (memeq (test_pattern, test_buffer, sizeof(test_pattern)) == 0)
		{
#ifdef	DEBUG
			printf ("ed%d: NE2000 recusado\n", unit);
#endif	DEBUG

			return (-1);	/* not an NE2000 either */
		}

		sc->type	= ED_TYPE_NE2000;
		sc->type_str	= "NE2000";
	}
	else
	{
		sc->type	= ED_TYPE_NE1000;
		sc->type_str	= "NE1000";
	}

#ifdef	DEBUG
	printf
	(	"ed%d: Unidade %s, %s bits\n",
		unit, sc->type_str, sc->isa16bit ? "16" : "8"
	);
#endif	DEBUG

	/* 8k of memory plus an additional 8k if 16bit */

	sc->mem_size = memsize = 8192 + sc->isa16bit * 8192;

	/* NIC memory doesn't start at zero on an NE board */
	/* The start address is tied to the bus width */

	sc->mem_start = (char *)8192 + sc->isa16bit * 8192;
	sc->mem_end = sc->mem_start + memsize;
	sc->tx_page_start = memsize / ED_PAGE_SIZE;

	/*
	 *	Use one xmit buffer if < 16k, two buffers otherwise
	 */
	if (memsize < 16384)
		sc->txb_cnt = 1;
	else
		sc->txb_cnt = 2;

	sc->rec_page_start = sc->tx_page_start + sc->txb_cnt * ED_TXBUF_SIZE;
	sc->rec_page_stop  = sc->tx_page_start + memsize / ED_PAGE_SIZE;

	sc->mem_ring = sc->mem_start + sc->txb_cnt * ED_PAGE_SIZE * ED_TXBUF_SIZE;

	ed_pio_readmem (sc, 0, romdata, 16);

	for (i = 0; i < sizeof (ETHADDR); i++)
		sc->ac_enaddr.addr[i] = romdata[i * (sc->isa16bit + 1)];

#ifdef	DEBUG
	printf ("ed%d: Endereço ethernet = ", unit);
	ether_pr_ether_addr (&sc->ac_enaddr);
	printf ("\n");
#endif	DEBUG

	/* clear any pending interrupts that might have occurred above */

	write_port (0xFF, sc->nic_addr + ED_P0_ISR);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (irq, PL, unit, edint) < 0)
		return (-1);

	printf
	(	"ed:  [%d: 0x%X, %d, %s, %s bits, <",
		unit, iobase, irq, sc->type_str, sc->isa16bit ? "16" : "8"
	);
	ether_pr_ether_addr (&sc->ac_enaddr);
	printf (">]\n");

	sc->if_irq	= irq;
	sc->unit_alive	= 1;

	return (0);

}	/* end ed_attach_novell */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
edopen (dev_t dev, int oflag)
{
	int		unit;
	ED_STATUS	*sc;

	/*
	 *	Prólogo
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NED)
		{ u.u_error = ENXIO; return; }

	sc = &ed_status[unit];

	if (sc->lock || (oflag & O_LOCK) && sc->nopen)
		{ u.u_error = EBUSY; return; }

	if (sc->nopen)
		{ sc->nopen++; return; }

	sc->dev = dev;

	/*
	 *	Verifica se a unidade está ativa
	 */
	if (sc->unit_alive == 0)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Prepara o controlador para transmitir
	 */
	ed_reset (unit);

	/*
	 *	Epílogo
	 */
	sc->nopen++;

	if (oflag & O_LOCK)
		sc->lock = 1;

}	/* end edopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
edclose (dev_t dev, int flag)
{
	ED_STATUS	*sc;
	int		unit;

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev); sc = &ed_status[unit];

	if (--sc->nopen > 0)
		return;

	ed_stop (unit);

	/*
	 *	Epílogo
	 */
	sc->lock = 0;

}	/* end edclose */

/*
 ****************************************************************
 *	Prepara a unidade					*
 ****************************************************************
 */
void
ed_reset (int unit)
{
	int		pl;

	pl = spled ();

	/*
	 *	Pára a unidade e reinicializa
	 */
	ed_stop (unit);
	ed_init (unit);

	splx (pl);

}	/* ed_reset */

/*
 ****************************************************************
 *	Pára a unidade						*
 ****************************************************************
 */
void
ed_stop (int unit)
{
	ED_STATUS	*sc = &ed_status[unit];
	int		n = 5000;

	/*
	 *	Stop everything on the interface, and select page 0 registers.
	 */
	write_port (sc->cr_proto|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	/*
	 *	Wait for interface to enter stopped state, but limit # of
	 *	checks to 'n' (about 5ms). It shouldn't even take 5us on
	 *	modern DS8390's, but just in case it's an old one.
	 */
	while (((read_port (sc->nic_addr + ED_P0_ISR) & ED_ISR_RST) == 0) && --n)
		/* vazio */;

}	/* ed_stop */

/*
 ****************************************************************
 *	Inicializa a unidade					*
 ****************************************************************
 */
void
ed_init (int unit)
{
	ED_STATUS	*sc = &ed_status[unit];
	int 		i, pl;

	/*
	 *	Initialize the NIC in the exact order outlined in the NS manual.
	 *	This init procedure is "mandatory"...don't change what or when
	 *	things happen.
	 */
	pl = spled ();

	/* Reset transmitter flags */

	sc->xmit_busy = 0;

	sc->txb_inuse = 0;
	sc->txb_new = 0;
	sc->txb_next_tx = 0;

	/* This variable is used below - don't move this assignment */

	sc->next_packet = sc->rec_page_start + 1;

	/*
	 *	Set interface for page 0, Remote DMA complete, Stopped
	 */
	write_port (sc->cr_proto|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	if (sc->isa16bit)
	{
		/*
		 *	Set FIFO threshold to 8, No auto-init Remote DMA, byte
		 *	order=80x86, word-wide DMA xfers,
		 */
		write_port (ED_DCR_FT1|ED_DCR_WTS|ED_DCR_LS, sc->nic_addr + ED_P0_DCR);
	}
	else
	{
		/*
		 *	Same as above, but byte-wide DMA xfers
		 */
		write_port (ED_DCR_FT1|ED_DCR_LS, sc->nic_addr + ED_P0_DCR);
	}

	/*
	 *	Clear Remote Byte Count Registers
	 */
	write_port (0, sc->nic_addr + ED_P0_RBCR0);
	write_port (0, sc->nic_addr + ED_P0_RBCR1);

	/*
	 *	For the moment, don't store incoming packets in memory.
	 */
	write_port (ED_RCR_MON, sc->nic_addr + ED_P0_RCR);

	/*
	 *	Place NIC in internal loopback mode
	 */
	write_port (ED_TCR_LB0, sc->nic_addr + ED_P0_TCR);

	/*
	 *	Initialize transmit/receive (ring-buffer) Page Start
	 */
	write_port (sc->tx_page_start, sc->nic_addr + ED_P0_TPSR);
	write_port (sc->rec_page_start, sc->nic_addr + ED_P0_PSTART);

	/*	Set lower bits of byte addressable framing to 0 */

	if (sc->is790)
		write_port (0, sc->nic_addr + 0x09);

	/*
	 *	Initialize Receiver (ring-buffer) Page Stop and Boundry
	 */
	write_port (sc->rec_page_stop, sc->nic_addr + ED_P0_PSTOP);
	write_port (sc->rec_page_start, sc->nic_addr + ED_P0_BNRY);

	/*
	 *	Clear all interrupts. A '1' in each bit position clears the
	 *	corresponding flag.
	 */
	write_port (0xFF, sc->nic_addr + ED_P0_ISR);

	/*
	 *	Enable the following interrupts: receive/transmit complete,
	 *	receive/transmit error, and Receiver OverWrite.
	 *
	 *	Counter overflow and Remote DMA complete are *not* enabled.
	 */
	write_port
	(	ED_IMR_PRXE|ED_IMR_PTXE|ED_IMR_RXEE|ED_IMR_TXEE|ED_IMR_OVWE,
		sc->nic_addr + ED_P0_IMR
	);

	/*
	 *	Program Command Register for page 1
	 */
	write_port (sc->cr_proto|ED_CR_PAGE_1|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	/*
	 *	Copy out our station address
	 */
	for (i = 0; i < sizeof (ETHADDR); ++i)
		write_port (sc->ac_enaddr.addr[i], sc->nic_addr + ED_P1_PAR0 + i);

	/*
	 *	Set Current Page pointer to next_packet (initialized above)
	 */
	write_port (sc->next_packet, sc->nic_addr + ED_P1_CURR);

	/*
	 *	Program Receiver Configuration Register and multicast filter.
	 *	CR is set to page 0 on return.
	 */
	ed_setrcr (sc);

	/*
	 *	Take interface out of loopback
	 */
	write_port (0, sc->nic_addr + ED_P0_TCR);

	/*
	 *	If this is a 3Com board, the tranceiver must be software enabled
	 *	(there is no settable hardware default).
	 */
	if (sc->vendor == ED_VENDOR_3COM)
	{
#if (0) /*******************************************************/
		if (sc->if_flags & IFF_ALTPHYS)
			write_port (0, sc->asic_addr + ED_3COM_CR);
		else
#endif  /*******************************************************/
			write_port (ED_3COM_CR_XSEL, sc->asic_addr + ED_3COM_CR);
	}

	/*
	 *	Set 'running' flag, and clear output active flag.
	 */
	sc->if_flags &= ~IFF_OACTIVE;

	/*
	 *	...and attempt to start output
	 */
	ed_start (unit);

	splx (pl);

	if (CSWT (29))
		printf ("ed%d: Final de ed_init\n", unit);

}	/* ed_init */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
edwrite (IOREQ *iop)
{
	ED_STATUS	*sc;
	int		unit;
	ITBLOCK		*bp;

	/*
	 *	Pequena inicialização
	 */
	unit = MINOR (iop->io_dev); sc = &ed_status[unit];

	bp = (ITBLOCK *)iop->io_area;

	/*
	 *	Põe na fila do DRIVER
	 */
	SPINLOCK (&sc->q_lock);

	if (sc->q_first == NOITBLOCK)
		sc->q_first = bp;
	else
		sc->q_last->it_forw = bp;

	sc->q_last = bp;
	bp->it_forw = NOITBLOCK;

	EVENTCLEAR (&sc->q_outqempty);

	ed_busy++;

	SPINFREE (&sc->q_lock);

	/*
	 *	Verifica se precisa iniciar a operação
	 */
	spled ();

	if ((sc->if_flags & IFF_OACTIVE) == 0)
		ed_start (unit);

	spl0 ();

}	/* end edwrite */

/*
 ****************************************************************
 *	Inicia saída na unidade					*
 ****************************************************************
 */
/*
 * We make two assumptions here:
 *  1) that the current priority is set to splimp _before_ this code
 *     is called *and* is returned to the appropriate priority after
 *     return
 *  2) that the IFF_OACTIVE flag is checked before this code is called
 *     (i.e. that the output part of the interface is idle)
 */
void
ed_start (int unit)
{
	ED_STATUS	*sc = &ed_status[unit];
	ITBLOCK		*bp;
	void		*buffer;
	int		len;

	/*
	 *	First, see if there are buffered packets and an
	 *	idle transmitter - should never happen at this point.
	 */
	if (sc->txb_inuse && (sc->xmit_busy == 0))
	{
		printf ("ed: packets buffered, but transmitter idle\n");
		ed_xmit (unit);
	}

	/*
	 *	See if there is room to put another packet in the buffer.
	 */
    outloop:
	if (sc->txb_inuse == sc->txb_cnt)
	{
		sc->if_flags |= IFF_OACTIVE;
#ifdef	DEBUG
		printf ("ed%d: No room\n", unit);
#endif	DEBUG
		return;
	}

	/*
	 *	Verifica se a fila tem algum ITBLOCK
	 */
	SPINLOCK (&sc->q_lock);

	if ((bp = sc->q_first) == NOITBLOCK)
	{
		EVENTDONE (&sc->q_outqempty);
		SPINFREE (&sc->q_lock);
		return;
	}

	/*
	 *	Tira o bloco da fila
	 */
	if ((sc->q_first = bp->it_forw) == NOITBLOCK)
	{
	   /***	sc->q_last  = NOITBLOCK; ***/
		EVENTDONE (&sc->q_outqempty);
	}

	SPINFREE (&sc->q_lock);

	/* txb_new points to next open buffer slot */

	buffer = sc->mem_start + (sc->txb_new * ED_TXBUF_SIZE * ED_PAGE_SIZE);

	if (sc->mem_shared)
	{
		/*
		 *	Special case setup for 16 bit boards ...
		 */
		if (sc->isa16bit)
		{
			switch (sc->vendor)
			{
				/*
				 *	For 16bit 3Com boards (which have 16k
				 *	of memory), we have the xmit buffers
				 *	in a different page of memory
				 *	('page 0') - so change pages.
				 */
			    case ED_VENDOR_3COM:
				write_port (ED_3COM_GACFR_RSEL, sc->asic_addr + ED_3COM_GACFR);
				break;

				/*
				 *	Enable 16bit access to shared memory on
				 *	WD/SMC boards.
				 */
			    case ED_VENDOR_WD_SMC:
				write_port (sc->wd_laar_proto|ED_WD_LAAR_M16EN, sc->asic_addr + ED_WD_LAAR);

				if (sc->is790)
					write_port (ED_WD_MSR_MENB, sc->asic_addr + ED_WD_MSR);

				break;

			}	/* end switch */
		}

		len = bp->it_u_count;

		memmove (buffer, bp->it_u_area, len);

		/*
		 *	Restore previous shared memory access
		 */
		if (sc->isa16bit)
		{
			switch (sc->vendor)
			{
			    case ED_VENDOR_3COM:
				write_port (ED_3COM_GACFR_RSEL|ED_3COM_GACFR_MBS0, sc->asic_addr + ED_3COM_GACFR);
				break;

			    case ED_VENDOR_WD_SMC:
				if (sc->is790)
					write_port (0x00, sc->asic_addr + ED_WD_MSR);

				write_port (sc->wd_laar_proto, sc->asic_addr + ED_WD_LAAR);
				break;

			}	/* end switch */

		}	/* if (16 bits) */

	}
	else	/* Memória não compartilhada */
	{
		len = bp->it_u_count;

		if (sc->isa16bit && (len & 1))
			len++;

		ed_pio_writemem (sc, bp->it_u_area, buffer, len);
	}

	sc->txb_len[sc->txb_new] = MAX (len, ETHER_MIN_LEN);

	sc->txb_inuse++;

	/*
	 *	Libera o ITBLOCK
	 */
	bp->it_in_driver_queue = 0;

	if (bp->it_free_after_IO)
		put_it_block (bp);

	ed_busy--;

	/*
	 *	Point to next buffer slot and wrap if necessary.
	 */
	sc->txb_new++;

	if (sc->txb_new == sc->txb_cnt)
		sc->txb_new = 0;

	if (sc->xmit_busy == 0)
		ed_xmit (unit);

	goto outloop;

}	/* end ed_start */

/*
 ****************************************************************
 *	This routine actually starts the transmission		*
 ****************************************************************
 */
void
ed_xmit (int unit)
{
	ED_STATUS	*sc = &ed_status[unit];
	int		len;

	len = sc->txb_len[sc->txb_next_tx];

	/*
	 *	Set NIC for page 0 register access
	 */
	write_port (sc->cr_proto|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	/*
	 *	Set TX buffer start page
	 */
	write_port (sc->tx_page_start + sc->txb_next_tx * ED_TXBUF_SIZE, sc->nic_addr + ED_P0_TPSR);

	/*
	 *	Set TX length
	 */
	write_port (len, sc->nic_addr + ED_P0_TBCR0);
	write_port (len >> 8, sc->nic_addr + ED_P0_TBCR1);

	/*
	 *	Set page 0, Remote DMA complete, Transmit Packet, and *Start*
	 */
	write_port (sc->cr_proto|ED_CR_TXP|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	sc->xmit_busy = 1;

	/*
	 *	Point to next transmit buffer slot and wrap if necessary.
	 */
	sc->txb_next_tx++;

	if (sc->txb_next_tx == sc->txb_cnt)
		sc->txb_next_tx = 0;

	if (CSWT (29))
		printf ("ed%d: Final de ed_xmit\n", unit);

}	/* end ed_xmit */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
edctl (dev_t dev, int cmd, void *arg, ...)
{
	int		unit;
	ED_STATUS	*sc;

	/*
	 *	Obtém o número da unidade
	 */
	unit = MINOR (dev); sc = &ed_status[unit];

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
		if (unimove (arg, &sc->ac_enaddr, sizeof (ETHADDR), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

	    default:			/* Nos outros casos, ... */
		{ u.u_error = EINVAL; return (-1); }

	}	/* end switch */

}	/* end edctl */

/*
 ****************************************************************
 *	Função de interrupção					*
 ****************************************************************
 */
void
edint (struct intr_frame frame)
{
	int		unit = frame.if_unit;
	ED_STATUS	*sc = &ed_status[unit];
	char		isr;
	static int	collision_count = 0;

	if (CSWT (29))
		printf ("ed%d: Veio interrupção\n", unit);

	/*
	 *	Set NIC to page 0 registers
	 */
	write_port (sc->cr_proto|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	/*
	 *	loop until there are no more new interrupts
	 */
	while ((isr = read_port (sc->nic_addr + ED_P0_ISR)) != 0)
	{
		/*
		 *	Reset all the bits that we are 'acknowledging'
		 *	by writing a '1' to each bit position that was
		 *	set (writing a '1' *clears* the bit)
		 */
		write_port (isr, sc->nic_addr + ED_P0_ISR);

		/*
		 *	Handle transmitter interrupts. Handle these
		 *	first because the receiver will reset the
		 *	board under some conditions.
		 */
		if (isr & (ED_ISR_PTX|ED_ISR_TXE))
		{
			char  collisions = read_port (sc->nic_addr + ED_P0_NCR) & 0x0F;

			/*
			 *	Check for transmit error. If a TX completed
			 *	with an error, we end up throwing the packet
			 *	away. Really the only error that is possible
			 *	is excessive collisions, and in this case it
			 *	is best to allow the automatic mechanisms of
			 * 	TCP to backoff the flow. Of course, with UDP
			 *	we're screwed, but this is expected when a
			 *	network is heavily loaded.
			 */
			read_port (sc->nic_addr + ED_P0_TSR);

			if (isr & ED_ISR_TXE)
			{
				/*
				 *	Excessive collisions (16)
				 */
				if ((read_port (sc->nic_addr + ED_P0_TSR) & ED_TSR_ABT) && (collisions == 0))
				{
					/*
					 *	When collisions total 16, the
					 *	P0_NCR will indicate 0, and the
					 *	TSR_ABT is set.
					 */
					collisions = 16;
				}

				/*
				 *	update output errors counter
				 */
				if ((++collision_count % 16) == 0)
				{
					printf
					(	"ed%d: Erro de transmissão (%d colisões)\n",
						unit, collision_count
					);
				}
			}
			else
			{

				/*
				 *	Update total number of successfully
				 *	transmitted packets.
				 */
			}

			/*
			 *	Reset tx busy and output active flags
			 */
			sc->xmit_busy = 0;
			sc->if_flags &= ~IFF_OACTIVE;

			/*
			 *	Add in total number of collisions on last
			 *	transmission.
			 */

			/*
			 *	Decrement buffer in-use count if not zero
			 *	(can only be zero if a transmitter interrupt
			 *	occured while not actually transmitting).
			 *	If data is ready to transmit, start it
			 *	transmitting, otherwise defer until after
			 *	handling receiver
			 */
			if (sc->txb_inuse && --sc->txb_inuse)
				ed_xmit (unit);
		}

		/*
		 *	Handle receiver interrupts
		 */
		if (isr & (ED_ISR_PRX|ED_ISR_RXE|ED_ISR_OVW))
		{
			/*
			 *	Overwrite warning. In order to make sure
			 *	that a lockup of the local DMA hasn't
			 *	occurred, we reset and re-init the NIC.
			 *	The NSC manual suggests only a partial
			 *	reset/re-init is necessary - but some chips
			 *	seem to want more. The DMA lockup has been
			 *	seen only with early rev chips - Methinks
			 *	this bug was fixed in later revs. -DG
			 */
			if (isr & ED_ISR_OVW)
			{
				printf ("ed%d: warning - receiver ring buffer overrun\n", unit);

				/*
				 *	Stop/reset/re-init NIC
				 */
				ed_reset (unit);
			}
			else
			{
				/*
				 *	Receiver Error. One or more of:
				 *	CRC error, frame alignment error FIFO
				 *	overrun, or missed packet.
				 */
				if (isr & ED_ISR_RXE)
				{
#ifdef	RECEIVE_ERROR
					printf
					(	"ed%d: receive error 0x%02X\n", unit,
						read_port (sc->nic_addr + ED_P0_RSR)
					);
#endif	RECEIVE_ERROR
				}

				/*
				 *	Go get the packet(s) XXX - Doing this
				 *	on an error is dubious because there
				 *	shouldn't be any data to get (we've
				 *	configured the interface to not
				 *	accept packets with errors).
				 */

				/*
				 *	Enable 16bit access to shared memory
				 *	first on WD/SMC boards.
				 */
				if (sc->isa16bit && (sc->vendor == ED_VENDOR_WD_SMC))
				{
					sc->wd_laar_proto |= ED_WD_LAAR_M16EN;
					write_port (sc->wd_laar_proto, sc->asic_addr + ED_WD_LAAR);

					if (sc->is790)
						write_port (ED_WD_MSR_MENB, sc->asic_addr + ED_WD_MSR);
				}

				ed_rint (unit);

				/* disable 16bit access */

				if (sc->isa16bit && (sc->vendor == ED_VENDOR_WD_SMC))
				{
					if (sc->is790)
						write_port (0, sc->asic_addr + ED_WD_MSR);

					sc->wd_laar_proto &= ~ED_WD_LAAR_M16EN;
					write_port (sc->wd_laar_proto, sc->asic_addr + ED_WD_LAAR);
				}
			}
		}

		/*
		 *	If it looks like the transmitter can take more data,
		 *	attempt to start output on the interface. This is done
		 *	after handling the receiver to give the receiver
		 *	priority.
		 */
		if ((sc->if_flags & IFF_OACTIVE) == 0)
			ed_start (unit);

		/*
		 *	Return NIC CR to standard state: page 0, remote DMA
		 *	complete, start (toggling the TXP bit off, even if
		 *	was just set in the transmit routine, is *okay* -
		 *	it is 'edge' triggered from low to high)
		 */
		write_port (sc->cr_proto|ED_CR_STA, sc->nic_addr + ED_P0_CR);

		/*
		 *	If the Network Talley Counters overflow, read them
		 *	to reset them. It appears that old 8390's won't
		 *	clear the ISR flag otherwise - resulting in an
		 *	infinite loop.
		 */
		if (isr & ED_ISR_CNT)
		{
			read_port (sc->nic_addr + ED_P0_CNTR0);
			read_port (sc->nic_addr + ED_P0_CNTR1);
			read_port (sc->nic_addr + ED_P0_CNTR2);
		}

	}	/* end while (interrupts) */

}	/* end edint */

/*
 ****************************************************************
 *	Ethernet interface receiver interrupt			*
 ****************************************************************
 */
void
ed_rint (int unit)
{
	ED_STATUS	*sc = &ed_status[unit];
	char		boundry;
	int		len;
	struct ed_ring	packet_hdr;
	char		*packet_ptr;

	if (CSWT (29))
		printf ("ed%d: Interrupção de recepção\n", unit);

	/*
	 *	Set NIC to page 1 registers to get 'current' pointer
	 */
	write_port (sc->cr_proto|ED_CR_PAGE_1|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	/*
	 *	'sc->next_packet' is the logical beginning of the ring-buffer -
	 *	i.e. it points to where new data has been buffered. The 'CURR'
	 *	(current) register points to the logical end of the ring-buffer
	 *	- i.e. it points to where additional new data will be added.
	 *	We loop here until the logical beginning equals the logical end
	 *	(or in other words, until the ring-buffer is empty).
	 */
	while (sc->next_packet != read_port (sc->nic_addr + ED_P1_CURR))
	{
		/* get pointer to this buffer's header structure */

		packet_ptr = sc->mem_ring +
		    (sc->next_packet - sc->rec_page_start) * ED_PAGE_SIZE;

		/*
		 *	The byte count includes a 4 byte header that was
		 *	added by the NIC.
		 */
		if (sc->mem_shared)
			packet_hdr = *(struct ed_ring *) packet_ptr;
		else
			ed_pio_readmem (sc, packet_ptr, (char *)&packet_hdr,
				       sizeof (packet_hdr));

		len = packet_hdr.count;

		if (len > ETHER_MAX_LEN + 4) /* len includes 4 byte header */
		{
			/*
			 *	Length is a wild value. There's a good chance
			 *	that this was caused by the NIC being old and
			 *	buggy. The bug is that the length low byte is
			 *	duplicated in the high byte. Try to recalculate
			 *	the length based on the pointer to the next
			 *	packet.
			 *
			 *	NOTE: sc->next_packet is pointing at the
			 *	current packet.
			 */
			len &= ED_PAGE_SIZE - 1; /* preserve offset into page */

			if (packet_hdr.next_packet >= sc->next_packet)
			{
				len += (packet_hdr.next_packet - sc->next_packet) * ED_PAGE_SIZE;
			}
			else
			{
				len += ((packet_hdr.next_packet - sc->rec_page_start) +
					(sc->rec_page_stop - sc->next_packet)) * ED_PAGE_SIZE;
			}
		}

		/*
		 *	Be fairly liberal about what we allow as a "reasonable"
		 *	length so that a [crufty] packet will make it to BPF
		 *	(and can thus be analyzed). Note that all that is really
		 *	important is that we have a length that will fit into
		 *	one mbuf cluster or less; the upper layer protocols can
		 *	then figure out the length from their own length
		 *	field(s).
		 */
		if
		(	len <= sizeof (struct ed_ring) + sizeof (ETH_H) + DGSZ &&
			packet_hdr.next_packet >= sc->rec_page_start &&
			packet_hdr.next_packet < sc->rec_page_stop
		)
		{
			/*
			 *	Go get packet.
			 */
			ed_get_packet
			(	sc, packet_ptr + sizeof(struct ed_ring),
				len - sizeof (struct ed_ring),
				packet_hdr.rsr & ED_RSR_PHY
			);
		}
		else
		{
			/*
			 *	Really BAD. The ring pointers are corrupted.
			 */
			if (CSWT (29))
			{
				printf
				(	"ed%d: NIC memory corrupt - invalid packet length %d\n",
					unit, len
				);
			}

			ed_reset (unit);
			return;
		}

		/*
		 *	Update next packet pointer
		 */
		sc->next_packet = packet_hdr.next_packet;

		/*
		 *	Update NIC boundry pointer - being careful to keep
		 *	it one buffer behind. (as recommended by NS databook)
		 */
		boundry = sc->next_packet - 1;

		if (boundry < sc->rec_page_start)
			boundry = sc->rec_page_stop - 1;

		/*
		 *	Set NIC to page 0 registers to update boundry register
		 */
		write_port (sc->cr_proto|ED_CR_STA, sc->nic_addr + ED_P0_CR);
		write_port (boundry, sc->nic_addr + ED_P0_BNRY);

		/*
		 *	Set NIC to page 1 registers before looping to top
		 *	(prepare to get 'CURR' current pointer)
		 */
		write_port (sc->cr_proto|ED_CR_PAGE_1|ED_CR_STA, sc->nic_addr + ED_P0_CR);
	}

}	/* end ed_rint */

/*
 ****************************************************************
 *	Lê um pacote e entrega para o nível superior		*
 ****************************************************************
 */
void
ed_get_packet (ED_STATUS *sc, char *area, int count, int multicast)
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

	ed_ring_copy (sc, area, bp->it_frame, count);

	bp->it_dev	 = sc->dev;
	bp->it_u_area	 = bp->it_frame;
	bp->it_u_count	 = count;
	bp->it_ether_dev = 1;
	bp->it_ppp_dev   = 0;

	wake_up_in_daemon (IN_BP, bp);

	ed_busy--;

}	/* ed_get_packet */

/*
 ****************************************************************
 *	Copy 'amount' of a packet from the ring buffer		*
 ****************************************************************
 */
/*
 * Given a source and destination address, copy 'amount' of a packet from
 *	the ring buffer into a linear destination buffer. Takes into account
 *	ring-wrap.
 */
char *
ed_ring_copy (ED_STATUS *sc, void *src, void *dst, int amount)
{
	int	tmp_amount;

	if (CSWT (29))
		printf ("ed_ring_copy: Count = %d\n", amount);

	/* does copy wrap to lower addr in ring buffer? */

	if (src + amount > sc->mem_end)
	{
		tmp_amount = sc->mem_end - src;

		/* copy amount up to end of NIC memory */

		if (sc->mem_shared)
			memmove (dst, src, tmp_amount);
		else
			ed_pio_readmem (sc, src, dst, tmp_amount);

		amount -= tmp_amount;
		src = sc->mem_ring;
		dst += tmp_amount;
	}

	if (sc->mem_shared)
		memmove (dst, src, amount);
	else
		ed_pio_readmem (sc, src, dst, amount);

	return (src + amount);

}	/* end ed_ring_copy */

/*
 ****************************************************************
 * Program Receiver Configuration Register and multicast filter	*
 ****************************************************************
 */
void
ed_setrcr (ED_STATUS *sc)
{
	int		i;

	/* set page 1 registers */

	write_port (sc->cr_proto|ED_CR_PAGE_1|ED_CR_STP, sc->nic_addr + ED_P0_CR);

	/*
	 *	Initialize multicast address hashing registers
	 *	to not accept multicasts.
	 */
	for (i = 0; i < 8; ++i)
		write_port (0, sc->nic_addr + ED_P1_MAR0 + i);

	/* Set page 0 registers */

	write_port (sc->cr_proto|ED_CR_STP, sc->nic_addr + ED_P0_CR);
	write_port (ED_RCR_AB, sc->nic_addr + ED_P0_RCR);

	/*
	 *	Start interface
	 */
	write_port (sc->cr_proto|ED_CR_STA, sc->nic_addr + ED_P0_CR);

}	/* ed_setrcr */

/*
 ****************************************************************
 *	Lê a memória do dispositivo				*
 ****************************************************************
 */
/*
 * Given a NIC memory source address and a host memory destination
 *	address, copy 'amount' from NIC to host using Programmed I/O.
 *	The 'amount' is rounded up to a word - okay as long as mbufs
 *		are word sized.
 *	This routine is currently Novell-specific.
 */
void
ed_pio_readmem (ED_STATUS *sc, char *src, char *dst, int amount)
{
	/* select page 0 registers */

	write_port (ED_CR_RD2|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	/* round up to a word */

	if (amount & 1)
		++amount;

	/* set up DMA byte count */

	write_port (amount, sc->nic_addr + ED_P0_RBCR0);
	write_port (amount >> 8, sc->nic_addr + ED_P0_RBCR1);

	/* set up source address in NIC mem */

	write_port ((int)src, sc->nic_addr + ED_P0_RSAR0);
	write_port ((int)src >> 8, sc->nic_addr + ED_P0_RSAR1);

	write_port (ED_CR_RD0|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	if (sc->isa16bit)
		read_port_string_short (sc->asic_addr + ED_NOVELL_DATA, dst, amount / 2);
	else
		read_port_string_byte (sc->asic_addr + ED_NOVELL_DATA, dst, amount);

}	/* end ed_pio_readmem */

/*
 ****************************************************************
 *	Escreve na memória do dispositivo			*
 ****************************************************************
 */
/*
 * Stripped down routine for writing a linear buffer to NIC memory.
 *	Only used in the probe routine to test the memory. 'len' must
 *	be even.
 */
void
ed_pio_writemem (ED_STATUS *sc, char *src, char *dst, int len)
{
	int     maxwait = 200;	/* about 240us */

	/* Select page 0 registers */

	write_port (ED_CR_RD2|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	/* Reset remote DMA complete flag */

	write_port (ED_ISR_RDC, sc->nic_addr + ED_P0_ISR);

	/* Set up DMA byte count */

	write_port (len, sc->nic_addr + ED_P0_RBCR0);
	write_port (len >> 8, sc->nic_addr + ED_P0_RBCR1);

	/* Set up destination address in NIC mem */

	write_port ((int)dst, sc->nic_addr + ED_P0_RSAR0);
	write_port ((int)dst >> 8, sc->nic_addr + ED_P0_RSAR1);

	/* Set remote DMA write */

	write_port (ED_CR_RD1|ED_CR_STA, sc->nic_addr + ED_P0_CR);

	if (sc->isa16bit)
		write_port_string_short (sc->asic_addr + ED_NOVELL_DATA, src, len >> 1);
	else
		write_port_string_byte (sc->asic_addr + ED_NOVELL_DATA, src, len);

	/*
	 * Wait for remote DMA complete. This is necessary because on the
	 * transmit side, data is handled internally by the NIC in bursts and
	 * we can't start another remote DMA until this one completes. Not
	 * waiting causes really bad things to happen - like the NIC
	 * irrecoverably jamming the ISA bus.
	 */
	while (((read_port (sc->nic_addr + ED_P0_ISR) & ED_ISR_RDC) != ED_ISR_RDC) && --maxwait)
		/* vazio */;

}	/* end ed_pio_writemem */

#if (0) /*******************************************************/
/*
 ****************************************************************
 *	Compute the multicast address filter from the		*
 *	list of multicast addresses we need to listen to	*
 ****************************************************************
 */
void
ds_getmcaf (ED_STATUS *sc, ulong *mcaf)
{
	unsigned int		index;
	char			*af = (char *) mcaf;
	struct ether_multi	*enm;
	struct ether_multistep	step;
	int			i;

	mcaf[0] = 0;
	mcaf[1] = 0;

	ETHER_FIRST_MULTI (step, &sc->arpcom, enm);

	while (enm != NULL)
	{
		for (i = 0; i < 6; i++)
		{
			if (enm->enm_addrlo[i] != enm->enm_addrhi[i])
			{
				mcaf[0] = 0xFFFFFFFF;
				mcaf[1] = 0xFFFFFFFF;
				return;
			}
		}

		index = ds_crc (enm->enm_addrlo, 6) >> 26;
		af[index >> 3] |= 1 << (index & 7);

		ETHER_NEXT_MULTI (step, enm);
	}

}	/* ds_getmcaf */

/*
 ****************************************************************
 *	Compute crc for ethernet address			*
 ****************************************************************
 */
ulong
ds_crc (char *ep, int n)
{
#define POLYNOMIAL 0x04C11DB6

	ulong		crc = 0xFFFFFFFF;
	int		carry, i, j;
	char		b;

	for (i = n; --i >= 0;)
	{
		b = *ep++;

		for (j = 8; --j >= 0; /* vazio */)
		{
			carry = ((crc & 0x80000000) ? 1 : 0) ^ (b & 0x01);
			crc <<= 1;
			b >>= 1;

			if (carry)
				crc = ((crc ^ POLYNOMIAL)|carry);
		}
	}

	return (crc);

}	/* end ds_crc */
#endif	/*******************************************************/
