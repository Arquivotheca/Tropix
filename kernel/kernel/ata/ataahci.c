/*
 ****************************************************************
 *								*
 *			ataahci.c				*
 *								*
 *	Rotinas auxiliares para controladores AHCI		*
 *								*
 *	Versão	4.9.0, de 21.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */
#undef	DEBUG

#ifdef	BOOT

#include <common.h>
#include <bhead.h>

#include <pci.h>
#include <scsi.h>
#include <ata.h>

#include "../../boot/boot2/h/proto.h"

#else

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/map.h"

#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

#endif

/*
 ****************************************************************
 *	Definições de Registros e Bits				*
 ****************************************************************
 */
#define ATA_AHCI_CAP                    0x00
#define         ATA_AHCI_NPMASK         0x1f
#define		ATA_AHCI_CAP_CLO	0x01000000
#define		ATA_AHCI_CAP_64BIT	0x80000000

#define ATA_AHCI_GHC                    0x04
#define         ATA_AHCI_GHC_AE         0x80000000
#define         ATA_AHCI_GHC_IE         0x00000002
#define         ATA_AHCI_GHC_HR         0x80000001

#define ATA_AHCI_IS                     0x08
#define ATA_AHCI_PI                     0x0c
#define ATA_AHCI_VS                     0x10

#define ATA_AHCI_OFFSET                 0x80

#define ATA_AHCI_P_CLB                  0x100
#define ATA_AHCI_P_CLBU                 0x104
#define ATA_AHCI_P_FB                   0x108
#define ATA_AHCI_P_FBU                  0x10c
#define ATA_AHCI_P_IS                   0x110
#define ATA_AHCI_P_IE                   0x114
#define         ATA_AHCI_P_IX_DHR       0x00000001
#define         ATA_AHCI_P_IX_PS        0x00000002
#define         ATA_AHCI_P_IX_DS        0x00000004
#define         ATA_AHCI_P_IX_SDB       0x00000008
#define         ATA_AHCI_P_IX_UF        0x00000010
#define         ATA_AHCI_P_IX_DP        0x00000020
#define         ATA_AHCI_P_IX_PC        0x00000040
#define         ATA_AHCI_P_IX_DI        0x00000080

#define         ATA_AHCI_P_IX_PRC       0x00400000
#define         ATA_AHCI_P_IX_IPM       0x00800000
#define         ATA_AHCI_P_IX_OF        0x01000000
#define         ATA_AHCI_P_IX_INF       0x04000000
#define         ATA_AHCI_P_IX_IF        0x08000000
#define         ATA_AHCI_P_IX_HBD       0x10000000
#define         ATA_AHCI_P_IX_HBF       0x20000000
#define         ATA_AHCI_P_IX_TFE       0x40000000
#define         ATA_AHCI_P_IX_CPD       0x80000000

#define ATA_AHCI_P_CMD                  0x118
#define         ATA_AHCI_P_CMD_ST       0x00000001
#define         ATA_AHCI_P_CMD_SUD      0x00000002
#define         ATA_AHCI_P_CMD_POD      0x00000004
#define         ATA_AHCI_P_CMD_CLO      0x00000008
#define         ATA_AHCI_P_CMD_FRE      0x00000010
#define         ATA_AHCI_P_CMD_CCS_MASK 0x00001f00
#define         ATA_AHCI_P_CMD_ISS      0x00002000
#define         ATA_AHCI_P_CMD_FR       0x00004000
#define         ATA_AHCI_P_CMD_CR       0x00008000
#define         ATA_AHCI_P_CMD_CPS      0x00010000
#define         ATA_AHCI_P_CMD_PMA      0x00020000
#define         ATA_AHCI_P_CMD_HPCP     0x00040000
#define         ATA_AHCI_P_CMD_ISP      0x00080000
#define         ATA_AHCI_P_CMD_CPD      0x00100000
#define         ATA_AHCI_P_CMD_ATAPI    0x01000000
#define         ATA_AHCI_P_CMD_DLAE     0x02000000
#define         ATA_AHCI_P_CMD_ALPE     0x04000000
#define         ATA_AHCI_P_CMD_ASP      0x08000000
#define         ATA_AHCI_P_CMD_ICC_MASK 0xf0000000
#define         ATA_AHCI_P_CMD_NOOP     0x00000000
#define         ATA_AHCI_P_CMD_ACTIVE   0x10000000
#define         ATA_AHCI_P_CMD_PARTIAL  0x20000000
#define         ATA_AHCI_P_CMD_SLUMPER  0x60000000

#define ATA_AHCI_P_TFD                  0x120
#define ATA_AHCI_P_SIG                  0x124
#define ATA_AHCI_P_SSTS                 0x128
#define ATA_AHCI_P_SCTL                 0x12c
#define ATA_AHCI_P_SERR                 0x130
#define ATA_AHCI_P_SACT                 0x134
#define ATA_AHCI_P_CI                   0x138

#define ATA_AHCI_CL_SIZE                32
#define ATA_AHCI_CL_OFFSET              0
#define ATA_AHCI_FB_OFFSET              1024
#define ATA_AHCI_CT_OFFSET              1024+256
#define ATA_AHCI_CT_SG_OFFSET           128
#define ATA_AHCI_CT_SIZE                256

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
#define ATA_AHCI_PRD_MASK	0x003fffff	  /* max 4MB */
#define ATA_AHCI_PRD_IPC	(1 << 31)

struct ata_ahci_dma_prd
{
#if (0)	/*******************************************************/
	u_int64_t	dba;
#endif	/*******************************************************/
	ulong		dba_low;
	ulong		dba_high;
	ulong		reserved;
	ulong		dbc;			/* 0 based */
};

struct ata_ahci_cmd_tab
{
	uchar		cfis[64];
	uchar		acmd[32];
	uchar		reserved[32];
	struct ata_ahci_dma_prd	 prd_tab[16];
};

struct ata_ahci_cmd_list
{
	ushort		cmd_flags;
	ushort		prd_length;	 /* PRD entries */

	ulong		bytecount;

	ulong		cmd_table_phys_low; /* 128byte aligned */
	ulong		cmd_table_phys_high; /* 128byte aligned */
#if (0)	/*******************************************************/
	u_int64_t	cmd_table_phys; /* 128byte aligned */
#endif	/*******************************************************/
};


int		ata_ahci_allocate (PCIDATA *, ATA_CHANNEL *);
int		ata_ahci_reset (PCIDATA *, ATA_CHANNEL *);
void		ata_ahci_setmode (PCIDATA *, ATA_DEVICE *, int);
int		ata_ahci_status (ATA_CHANNEL *);
int		ata_ahci_begin_transaction (ATA_DEVICE *, ATA_SCB *);
int		ata_ahci_end_transaction (ATA_DEVICE *, ATA_SCB *);
int		ata_ahci_setup_fis (ATA_DEVICE *, uchar *, ATA_SCB *);

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 */
int
ata_ahci_chipinit (PCIDATA *pci)
{
	ATA_PCI_CTLR	*ctlrp = pci->pci_dev_info_ptr;
	ulong		version;
	int		base = pci->pci_addr[5];

	/* reset AHCI controller */
#if (0)	/*******************************************************/
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_GHC,
		 ATA_INL(ctlr->r_res2, ATA_AHCI_GHC) | ATA_AHCI_GHC_HR);
#endif	/*******************************************************/

	write_port_long (read_port_long (base + ATA_AHCI_GHC) | ATA_AHCI_GHC_HR, base + ATA_AHCI_GHC);

	DELAY (1000000);

#if (0)	/*******************************************************/
	if (ATA_INL(ctlr->r_res2, ATA_AHCI_GHC) & ATA_AHCI_GHC_HR) {
	bus_release_resource(dev, ctlr->r_type2, ctlr->r_rid2, ctlr->r_res2);
	device_printf(dev, "AHCI controller reset failure\n");
	return ENXIO;
	}
#endif	/*******************************************************/

	if (read_port_long (base + ATA_AHCI_GHC) & ATA_AHCI_GHC_HR)
	{
		printf ("O reset do controlador AHCI falhou\n");
		return (-1);
	}

#if (0)	/*******************************************************/
	/* enable AHCI mode */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_GHC,
		 ATA_INL(ctlr->r_res2, ATA_AHCI_GHC) | ATA_AHCI_GHC_AE);
#endif	/*******************************************************/

	write_port_long (read_port_long (base + ATA_AHCI_GHC) | ATA_AHCI_GHC_AE, base + ATA_AHCI_GHC);

#if (0)	/*******************************************************/
	/* get the number of HW channels */
	ctlr->channels = (ATA_INL(ctlr->r_res2, ATA_AHCI_CAP) & ATA_AHCI_NPMASK)+1;
#endif	/*******************************************************/

	ctlrp->ac_nchannels = (read_port_long (base + ATA_AHCI_CAP) & ATA_AHCI_NPMASK) + 1;

#if (0)	/*******************************************************/
	/* clear interrupts */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_IS, ATA_INL(ctlr->r_res2, ATA_AHCI_IS));
#endif	/*******************************************************/

	write_port_long (read_port_long (base + ATA_AHCI_IS), base + ATA_AHCI_IS);

#if (0)	/*******************************************************/
	/* enable AHCI interrupts */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_GHC,
		 ATA_INL(ctlr->r_res2, ATA_AHCI_GHC) | ATA_AHCI_GHC_IE);
#endif	/*******************************************************/

	write_port_long (read_port_long (base + ATA_AHCI_GHC) | ATA_AHCI_GHC_IE, base + ATA_AHCI_GHC);

#if (0)	/*******************************************************/
	ctlr->reset = ata_ahci_reset;
	ctlr->dmainit = ata_ahci_dmainit;
	ctlr->allocate = ata_ahci_allocate;
	ctlr->setmode = ata_sata_setmode;
#endif	/*******************************************************/

	ctlrp->ac_reset		= ata_ahci_reset;
	ctlrp->ac_allocate	= ata_ahci_allocate;

#ifndef	BOOT
	ctlrp->ac_setmode	= ata_sata_setmode;
#endif

	/* enable PCI interrupt */
	pci_write (pci, PCIR_COMMAND, pci_read (pci, PCIR_COMMAND, 2) & ~0x0400, 2);

	/* announce we support the HW */
	version = read_port_long (base + ATA_AHCI_VS);

	printf ("Controlador AHCI com %d canais\n", ctlrp->ac_nchannels);

#if (0)	/*******************************************************/
	device_printf(dev,
		  "AHCI Version %x%x.%x%x controller with %d ports detected\n",
		  (version >> 24) & 0xff, (version >> 16) & 0xff,
		  (version >> 8) & 0xff, version & 0xff, ctlr->channels);
#endif	/*******************************************************/

	return (0);

}	/* end ata_ahci_chipinit */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ata_ahci_allocate (PCIDATA *pci, ATA_CHANNEL *chp)
{
	int		offset = pci->pci_addr[5] + (chp->ch_unit << 7);
	ulong		dma_area_phys;

#ifndef	BOOT
	if ((chp->ch_dma_area = (void *)PGTOBY (malloce (M_CORE, 1 /* página */))) == NOVOID)
		return (-1);
#endif

	/* setup legacy cruft we need */

	chp->ch_ports[ATA_CYL_LSB]	= offset + ATA_AHCI_P_SIG + 1;
	chp->ch_ports[ATA_CYL_MSB]	= offset + ATA_AHCI_P_SIG + 3;
	chp->ch_ports[ATA_STATUS]	= offset + ATA_AHCI_P_TFD;
	chp->ch_ports[ATA_ALTSTAT]	= offset + ATA_AHCI_P_TFD;

	/* set the SATA resources */

	chp->ch_ports[ATA_SSTATUS]	= offset + ATA_AHCI_P_SSTS;
	chp->ch_ports[ATA_SERROR]	= offset + ATA_AHCI_P_SERR;
	chp->ch_ports[ATA_SCONTROL]	= offset + ATA_AHCI_P_SCTL;
	chp->ch_ports[ATA_SACTIVE]	= offset + ATA_AHCI_P_SACT;

	chp->ch_ports[ATA_AHCI]		= pci->pci_addr[5];

#ifndef	BOOT
	chp->ch_status			= ata_ahci_status;
	chp->ch_begin_transaction	= ata_ahci_begin_transaction;
	chp->ch_end_transaction		= ata_ahci_end_transaction;

	dma_area_phys = VIRT_TO_PHYS_ADDR (chp->ch_dma_area);

	write_port_long (dma_area_phys + ATA_AHCI_CL_OFFSET,  ATA_AHCI_P_CLB + offset);

	write_port_long (0, ATA_AHCI_P_CLBU + offset);

	write_port_long (dma_area_phys + ATA_AHCI_FB_OFFSET,  ATA_AHCI_P_FB + offset);

	write_port_long (0, ATA_AHCI_P_FBU + offset);
#endif

#if (0)	/*******************************************************/
	ch->hw.command = NULL;	  /* not used here */

	/* setup the work areas */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_CLB + offset,
		 ch->dma->work_bus + ATA_AHCI_CL_OFFSET);
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_CLBU + offset, 0x00000000);

	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_FB + offset,
		 ch->dma->work_bus + ATA_AHCI_FB_OFFSET);
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_FBU + offset, 0x00000000);
#endif	/*******************************************************/

	write_port_long
	(
		ATA_AHCI_P_IX_CPD | ATA_AHCI_P_IX_TFE | ATA_AHCI_P_IX_HBF |
		ATA_AHCI_P_IX_HBD | ATA_AHCI_P_IX_IF | ATA_AHCI_P_IX_OF |
		ATA_AHCI_P_IX_PRC | ATA_AHCI_P_IX_PC | ATA_AHCI_P_IX_DP |
		ATA_AHCI_P_IX_UF | ATA_AHCI_P_IX_SDB | ATA_AHCI_P_IX_DS |
		ATA_AHCI_P_IX_PS | ATA_AHCI_P_IX_DHR,
		offset + ATA_AHCI_P_IE
	);

	write_port_long
	(
		ATA_AHCI_P_CMD_ACTIVE | ATA_AHCI_P_CMD_FRE |
		ATA_AHCI_P_CMD_POD | ATA_AHCI_P_CMD_SUD | ATA_AHCI_P_CMD_ST,
		ATA_AHCI_P_CMD + offset
	);

#if (0)	/*******************************************************/
	/* enable wanted port interrupts */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_IE + offset,
		 (ATA_AHCI_P_IX_CPD | ATA_AHCI_P_IX_TFE | ATA_AHCI_P_IX_HBF |
		  ATA_AHCI_P_IX_HBD | ATA_AHCI_P_IX_IF | ATA_AHCI_P_IX_OF |
		  ATA_AHCI_P_IX_PRC | ATA_AHCI_P_IX_PC | ATA_AHCI_P_IX_DP |
		  ATA_AHCI_P_IX_UF | ATA_AHCI_P_IX_SDB | ATA_AHCI_P_IX_DS |
		  ATA_AHCI_P_IX_PS | ATA_AHCI_P_IX_DHR));

	/* start operations on this channel */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_CMD + offset,
		 (ATA_AHCI_P_CMD_ACTIVE | ATA_AHCI_P_CMD_FRE |
		  ATA_AHCI_P_CMD_POD | ATA_AHCI_P_CMD_SUD | ATA_AHCI_P_CMD_ST));
#endif	/*******************************************************/

	return 0;

}	/* end ata_ahci_allocate */

/*
 ****************************************************************
 *	Reset							*
 ****************************************************************
 */
int
ata_ahci_reset (PCIDATA *pci, ATA_CHANNEL *chp)
{
	ulong		cmd;
	int		timeout;
	int		base   = chp->ch_ports[ATA_AHCI];
	int		offset = base + (chp->ch_unit << 7);

	cmd = read_port_long (offset + ATA_AHCI_P_CMD);

	write_port_long (cmd & ~(ATA_AHCI_P_CMD_FRE | ATA_AHCI_P_CMD_ST), offset + ATA_AHCI_P_CMD);

#if (0)	/*******************************************************/
	/* kill off all activity on this channel */
	cmd = ATA_INL(ctlr->r_res2, ATA_AHCI_P_CMD + offset);
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_CMD + offset,
		 cmd & ~(ATA_AHCI_P_CMD_FRE | ATA_AHCI_P_CMD_ST));
#endif	/*******************************************************/

	/* XXX SOS this is not entirely wrong */
	timeout = 0;

	do
	{
		DELAY (1000);

		if (timeout++ > 500)
		{
			printf ("stopping AHCI engine failed\n");
			break;
		}

	} 	while (read_port_long (ATA_AHCI_P_CMD + offset) & ATA_AHCI_P_CMD_CR);

	/* issue Command List Override if supported */ 

	if (read_port_long (base + ATA_AHCI_CAP) & ATA_AHCI_CAP_CLO)
	{
		cmd = read_port_long (ATA_AHCI_P_CMD + offset);

		cmd |= ATA_AHCI_P_CMD_CLO;

		write_port_long (cmd, ATA_AHCI_P_CMD + offset);

		timeout = 0;

		do
		{
			DELAY (1000);

			if (timeout++ > 500)
			{
				printf ("executing CLO failed\n");
				break;
			}

		}	while (read_port_long (ATA_AHCI_P_CMD+offset) & ATA_AHCI_P_CMD_CLO);
	}

	/* spin up device */
	write_port_long (ATA_AHCI_P_CMD_SUD, ATA_AHCI_P_CMD + offset);

#if (0)	/*******************************************************/
	ata_sata_phy_enable (chp);
#endif	/*******************************************************/

	/* clear any interrupts pending on this channel */
	write_port_long (ATA_AHCI_P_IS + offset, read_port_long (ATA_AHCI_P_IS + offset));

	/* start operations on this channel */
	write_port_long
	(
		ATA_AHCI_P_CMD_ACTIVE | ATA_AHCI_P_CMD_FRE |
		ATA_AHCI_P_CMD_POD | ATA_AHCI_P_CMD_SUD | ATA_AHCI_P_CMD_ST,
		ATA_AHCI_P_CMD + offset
	);

	return (0);

}	/* end ata_ahci_reset */

/*
 ****************************************************************
 *	Status (chamada dentro da interrupção!)			*
 ****************************************************************
 */
int
ata_ahci_status (ATA_CHANNEL *chp)
{
	ulong		action, istatus, sstatus, error, issued;
	int		base   = chp->ch_ports[ATA_AHCI];
	int		offset = base + (chp->ch_unit << 7);
	int		tag = 0;

	action = read_port_long (base + ATA_AHCI_IS);

	if (action & (1 << chp->ch_unit))
	{
		istatus	= read_port_long (ATA_AHCI_P_IS + offset);
		issued	= read_port_long (ATA_AHCI_P_CI + offset);
		sstatus = read_port_long (ATA_AHCI_P_SSTS + offset);
		error	= read_port_long (ATA_AHCI_P_SERR + offset);

		/* clear interrupt(s) */

		write_port_long (action,  base + ATA_AHCI_IS);
		write_port_long (istatus, ATA_AHCI_P_IS + offset);
		write_port_long (error,   ATA_AHCI_P_SERR + offset);

		/* do we have cold connect surprise */

		if (istatus & ATA_AHCI_P_IX_CPD)
		{
			printf ("ata_ahci_status status=%08x sstatus=%08x error=%08x\n",
				istatus, sstatus, error);
		}

#if (0)	/********************************************************/
		/* check for and handle connect events */
		if ((istatus & ATA_AHCI_P_IX_PC) && (tp = (struct ata_connect_task *)
		  malloc(sizeof(struct ata_connect_task),
			 M_ATA, M_NOWAIT | M_ZERO)))
		{
			if (bootverbose)
				device_printf(ch->dev, "CONNECT requested\n");
			tp->action = ATA_C_ATTACH;
			tp->dev = ch->dev;
			TASK_INIT(&tp->task, 0, ata_sata_phy_event, tp);
			taskqueue_enqueue(taskqueue_thread, &tp->task);
		}

	/* check for and handle disconnect events */
		else if ((istatus & ATA_AHCI_P_IX_PRC) && 
			!((sstatus & ATA_SS_CONWELL_MASK) == ATA_SS_CONWELL_GEN1 ||
			  (sstatus & ATA_SS_CONWELL_MASK) == ATA_SS_CONWELL_GEN2) &&
			(tp = (struct ata_connect_task *)
			  malloc(sizeof(struct ata_connect_task),
				   M_ATA, M_NOWAIT | M_ZERO)))
		{
			if (bootverbose)
			device_printf(ch->dev, "DISCONNECT requested\n");
			tp->action = ATA_C_DETACH;
			tp->dev = ch->dev;
			TASK_INIT(&tp->task, 0, ata_sata_phy_event, tp);
			taskqueue_enqueue(taskqueue_thread, &tp->task);
		}
#endif	/********************************************************/

		/* do we have any device action ? */

		if (!(issued & (1 << tag)))
			return (1);
	}

	return (0);

}	/* end ata_ahci_status */

#ifndef	BOOT
/*
 ****************************************************************
 *	Status (chamada dentro da interrupção!)			*
 ****************************************************************
 */
int
ata_ahci_begin_transaction (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL			*chp = adp->ad_channel;
	struct ata_ahci_cmd_tab		*ctp;
	struct ata_ahci_cmd_list	*clp;
	int				rw, fis_size, tag = 0, entries = 0;

	/* get a piece of the workspace for this request */
	ctp = (struct ata_ahci_cmd_tab *)(chp->ch_dma_area + ATA_AHCI_CT_OFFSET + (ATA_AHCI_CT_SIZE * tag));

	/* setup the FIS for this request */ /* XXX SOS ATAPI missing still */

	if (!(fis_size = ata_ahci_setup_fis (adp, &ctp->cfis[0], scbp)))
	{
		printf ("ata[%d,%d]: setting up SATA FIS failed\n", ATA_DEVNO);
		return (-1);
	}

#if (0)	/********************************************************/
	/* if request moves data setup and load SG list */

	if (request->flags & (ATA_R_READ | ATA_R_WRITE))
	{
	if (ch->dma->load(ch->dev, request->data, request->bytecount,
			  request->flags & ATA_R_READ,
			  ctp->prd_tab, &entries)) {
		device_printf(request->dev, "setting up DMA failed\n");
		request->result = EIO;
		return ATA_OP_FINISHED;
	}
	}
#endif	/********************************************************/

	scbp->scb_bytes_requested = scbp->scb_count;

#if (0)	/********************************************************/
	if (ata_ahci_dma_load (adp, scbp, ctp->prd_tab, &entries) != 0)
	{
		printf ("ata[%d,%d]: erro ao programar o DMA\n", ATA_DEVNO);
		return (-1);
	}
#endif	/********************************************************/

	scbp->scb_flags |= B_DMA;

	rw = scbp->scb_flags & (B_READ|B_WRITE);

	/* setup the command list entry */
	clp = (struct ata_ahci_cmd_list *)(chp->ch_dma_area + ATA_AHCI_CL_OFFSET + (ATA_AHCI_CL_SIZE * tag));

	clp->prd_length = entries;

#if (0)	/********************************************************/
	clp->cmd_flags = (request->flags & ATA_R_WRITE ? (1<<6) : 0) |
			 (request->flags & ATA_R_ATAPI ? (1<<5) : 0) |
			 (fis_size / sizeof(ulong));
#endif	/********************************************************/

	clp->cmd_flags = ((rw == B_WRITE) ? (1 << 6) : 0) |
			 ((adp->ad_flags & ATAPI_DEV) ? (1 << 5) : 0) | (fis_size / sizeof(ulong));

	clp->bytecount = 0;
#if (0)	/********************************************************/
	clp->cmd_table_phys = htole64 (chp->ch_dma_area + ATA_AHCI_CT_OFFSET + (ATA_AHCI_CT_SIZE * tag));
#endif	/********************************************************/

	clp->cmd_table_phys_low  = VIRT_TO_PHYS_ADDR (chp->ch_dma_area + ATA_AHCI_CT_OFFSET + (ATA_AHCI_CT_SIZE * tag));
	clp->cmd_table_phys_high = 0;

#if (0)	/********************************************************/
	/* clear eventual ACTIVE bit */
	ATA_IDX_OUTL(ch, ATA_SACTIVE, ATA_IDX_INL(ch, ATA_SACTIVE) & (1 << tag));

	/* issue the command */
	ATA_OUTL(ctlr->r_res2, ATA_AHCI_P_CI + (ch->unit << 7), (1 << tag));
#endif	/********************************************************/

	write_port_long (read_port_long (chp->ch_ports[ATA_SACTIVE])  & (1 << tag), chp->ch_ports[ATA_SACTIVE]);

	write_port_long (1 << tag, chp->ch_ports[ATA_AHCI] + ATA_AHCI_P_CI);

	return (0);

}	/* end ata_ahci_begin_transaction */

/*
 ****************************************************************
 *	Status (chamada dentro da interrupção!)			*
 ****************************************************************
 */
int
ata_ahci_end_transaction (ATA_DEVICE *adp, ATA_SCB *scbp)
{
#if (0)	/********************************************************/
	struct ata_pci_controller *ctlr=device_get_softc(GRANDPARENT(request->dev));
	struct ata_channel *ch = device_get_softc(device_get_parent(request->dev));
#endif	/********************************************************/

	ATA_CHANNEL			*chp = adp->ad_channel;
	struct ata_ahci_cmd_list	*clp;
	int				tag = 0;
	int				base   = chp->ch_ports[ATA_AHCI];
	int				offset = base + (chp->ch_unit << 7);

	adp->ad_status = read_port_long (ATA_AHCI_P_TFD + offset);

	if (adp->ad_status & ATA_S_ERROR)
		adp->ad_error = adp->ad_status >> 8;

#if (0)	/********************************************************/
	request->status = tf_data;

	/* if error status get details */
	if (tf_data & ATA_S_ERROR)  
		request->error = tf_data >> 8;
#endif	/********************************************************/

	/* record how much data we actually moved */

	clp = (struct ata_ahci_cmd_list *)(chp->ch_dma_area + ATA_AHCI_CL_OFFSET + (ATA_AHCI_CL_SIZE * tag));

	scbp->scb_bytes_transferred = clp->bytecount;

	if (adp->ad_status & ATA_S_ERROR)  
		scbp->scb_result = 1;
	else
		scbp->scb_result = 0;

	return (ATA_OP_FINISHED);
#if (0)	/********************************************************/
	request->donecount = clp->bytecount;

	/* release SG list etc */
	ch->dma->unload(ch->dev);
#endif	/********************************************************/

}	/* end ata_ahci_end_transaction */
#endif	BOOT


#if (0)	/********************************************************/
static void
ata_ahci_dmasetprd(void *xsc, bus_dma_segment_t *segs, int nsegs, int error)
{	
	struct ata_dmasetprd_args *args = xsc;
	struct ata_ahci_dma_prd *prd = args->dmatab;
	int i;

	if (!(args->error = error)) {
	for (i = 0; i < nsegs; i++) {
		prd[i].dba = htole64(segs[i].ds_addr);
		prd[i].dbc = htole32((segs[i].ds_len - 1) & ATA_AHCI_PRD_MASK);
	}
	}
	args->nsegs = nsegs;
}

static void
ata_ahci_dmainit(device_t dev)
{
	struct ata_channel *ch = device_get_softc(dev);

	ata_dmainit(dev);
	if (ch->dma) {
	/* note start and stop are not used here */
	ch->dma->setprd = ata_ahci_dmasetprd;
	ch->dma->max_iosize = 8192 * DEV_BSIZE;
	}
}
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Preenche o descritor com o comando			*
 ****************************************************************
 */
int
ata_ahci_setup_fis (ATA_DEVICE *adp, uchar *fis, ATA_SCB *scbp)
{
	int		cmd, rw, lba48;
	uchar		*fp = fis;

	/* XXX SOS add ATAPI commands support later */

	/*
	 *	Escolhe o comando adequado
	 */
	rw    = scbp->scb_flags & (B_READ|B_WRITE);
	lba48 = adp->ad_param.support.address48 && (scbp->scb_flags & B_STAT) == 0;

	if (scbp->scb_flags & B_DMA)
		cmd = (rw == B_WRITE) ? ATA_C_WRITE_DMA : ATA_C_READ_DMA;
	elif (scbp->scb_bytes_requested > BLSZ)
		cmd = (rw == B_WRITE) ? ATA_C_WRITE_MUL : ATA_C_READ_MUL;
	else
		cmd = (rw == B_WRITE) ? ATA_C_WRITE : ATA_C_READ;

	if (lba48)
		cmd = ata_find_lba48_equivalent (cmd);

	*fp++ = 0x27;			/* host to device */
	*fp++ = 0x80;			/* command FIS (note PM goes here) */
	*fp++ = cmd;
	*fp++ = 0;			/* feature */

	*fp++ = scbp->scb_blkno;
	*fp++ = scbp->scb_blkno >> 8;
	*fp++ = scbp->scb_blkno >> 16;

	if (lba48)
		*fp++ = ATA_D_LBA | (adp->ad_devno << 4);
	else
		*fp++ = ATA_D_LBA | (adp->ad_devno << 4) | (scbp->scb_blkno >> 24 & 0x0F);

	*fp++ = scbp->scb_blkno >> 24;
	*fp++ = 0;			/* scbp->scb_blkno >> 32;  */
	*fp++ = 0;			/* scbp->scb_blkno >> 40;  */
	*fp++ = 0;			/* feature >> 8 */

	*fp++ = scbp->scb_bytes_requested;
	*fp++ = scbp->scb_bytes_requested >> 8;
	*fp++ = 0x00;
	*fp++ = ATA_A_4BIT;

	*fp++ = 0x00;
	*fp++ = 0x00;
	*fp++ = 0x00;
	*fp++ = 0x00;

	return (fp - fis);

}	/* end ata_ahci_setup_fis */
