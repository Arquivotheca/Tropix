/*
 ****************************************************************
 *								*
 *			atapci.c				*
 *								*
 *	Funções relativas ao Barramento PCI			*
 *								*
 *	Versão	3.2.3, de 22.02.00				*
 *		4.9.0, de 07.05.08				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
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
#include "../h/scb.h"
#include "../h/devhead.h"

#include "../h/disktb.h"
#include "../h/devmajor.h"

#include "../h/frame.h"
#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

#endif

/*
 ******	Variáveis Globais ***************************************
 */
extern ATA_CHANNEL	*ata_channel[];
extern int		ata_next_channel;

#ifndef	BOOT
extern void		ataint (struct intr_frame frame);
#endif	BOOT

/*
 ****************************************************************
 *	Identifica controladores ATA PCI			*
 ****************************************************************
 */
char *
pci_ata_probe (PCIDATA *pci, ulong type)
{
	const ATA_CHIP_ID	*idp;
	ATA_PCI_CTLR		*ctlrp;
	static char		area[64];

	/*
	 *	Prólogo
	 */
	if ((pci_read (pci, PCI_CLASS_REG, 4) & PCI_CLASS_MASK) != PCI_CLASS_MASS_STORAGE)
		return (NOSTR);

	if ((pci->pci_dev_info_ptr = ctlrp = malloc_byte (sizeof (ATA_PCI_CTLR))) == NULL)
		return (NOSTR);

	memclr (ctlrp, sizeof (ATA_PCI_CTLR));

	/*
	 *	Identifica o controlador
	 */
	switch (pci->pci_vendor)
	{
	    case ATA_INTEL_ID:
		idp = ata_intel_ident (pci, type);
		break;

	    case ATA_JMICRON_ID:
		idp = ata_jmicron_ident (pci, type);
		break;

	    case ATA_VIA_ID:
		idp = ata_via_ident (pci, type);
		break;

	    case ATA_SIS_ID:
		idp = ata_sis_ident (pci, type);
		break;

	    default:
		idp = NULL;
		break;
	}

	if (idp == NULL)
		{ free_byte (ctlrp); pci->pci_dev_info_ptr = NOVOID; return (NOSTR); }

	/*
	 *	Imprime o controlador encontrado
	 */
	snprintf
	(
		area, sizeof (area), "%s %s %s controller",
		ctlrp->ac_chip_nm, idp->ch_nm,
		idp->ch_max_dma >= ATA_SA150 ? (idp->ch_cfg1 == AHCI ? "AHCI" : "SATA") : "ATA"
	);

	if ((ctlrp->ac_pci_name = malloc_byte (strlen (area) + 1)) == NOSTR)
		return (NOSTR);

	strcpy (ctlrp->ac_pci_name, area);

	return (area);

}	/* end pci_ata_probe */

/*
 ****************************************************************
 *	Anexa controladores ATA PCI				*
 ****************************************************************
 */
void
pci_ata_attach (PCIDATA *pci, int pci_unit)
{
	int			ret, irq, cmd, legacy;
	int			start_channel, end_channel;
	ATA_PCI_CTLR		*ctlrp;
	PCIDATA			*pci_copy;
	ATA_CHANNEL		*chp;
	int			channel, unit;
	static int		ata_ctrl_index_source;

	/*
	 *	x
	 */
	if ((ctlrp = pci->pci_dev_info_ptr) == NULL || ctlrp->ac_chipinit == NULL) /* NÃO deve acontecer... */
		return;

	ctlrp->ac_index = ata_ctrl_index_source++;

	/*
	 *	Se necessário, habilita o "busmastering"
	 */
	cmd = pci_read (pci, PCIR_COMMAND, 2);

	if ((cmd & PCIM_CMD_BUSMASTEREN) == 0)
		pci_write (pci, PCIR_COMMAND, cmd | PCIM_CMD_BUSMASTEREN, 2);

	/*
	 *	Obtém o número de canais (normalmente 1 ou 2)
	 */
	ctlrp->ac_legacy 	= legacy = ata_legacy (pci);
	ctlrp->ac_nchannels	= (legacy || pci_read (pci, PCIR_BAR (2), 4) & IOMASK) ? 2 : 1;

	/*
	 *	Inicializa de acordo com o fabricante
	 */
	ctlrp->ac_allocate	= NULL;
	ctlrp->ac_reset		= ata_generic_reset;
	ctlrp->ac_setmode	= NULL;

	if ((ret = (*ctlrp->ac_chipinit) (pci)) < 0)
		{ printf ("%g: NÃO foi possível inicializar o controlador ATA\n"); return; }

	start_channel		= ata_next_channel;
	end_channel		= ata_next_channel + ctlrp->ac_nchannels;

	if (end_channel >= NATAC)
		{ printf ("%g: número máximo de canais ATA excedido\n"); return; }

	if (ctlrp->ac_idp->ch_max_dma < ATA_SA150)
	{
		/* Controladores PATA */

		pci->pci_addr[0] = 0;		/* Usa as portas padrão */

		if (ctlrp->ac_allocate == NOFUNC)
			ctlrp->ac_allocate = ata_pci_allocate;

		irq = -1;
	}
	else
	{
		/* Controladores SATA */

		if (ctlrp->ac_allocate == NOFUNC)
			{ printf ("%g: Função de alocação de portas não especificada\n"); return; }

		irq = pci->pci_intline;
	}

	/*
	 *	Tira uma cópia do "pci"
	 */
	if ((pci_copy = malloc_byte (sizeof (PCIDATA))) == NULL)
		{ printf ("%g: memória esgotada\n"); return; }

	memmove (pci_copy, pci, sizeof (PCIDATA));

	pci = pci_copy;

	/*
	 *	Inicializa as estruturas de dados dos canais
	 */
	for (unit = 0, channel = start_channel; channel < end_channel; channel++, unit++)
	{
		if ((chp = malloc_byte (sizeof (ATA_CHANNEL))) == NOATACONTROLLER)
			{ printf ("%g: memória esgotada\n"); return; }

		memclr (chp, sizeof (ATA_CHANNEL));

		chp->ch_pci	= pci;
		chp->ch_ctlrp	= ctlrp;

		/* Por enquanto, um SCB por canal */

#ifndef	BOOT
		if ((chp->ch_scb = malloc_byte (sizeof (ATA_SCB))) == NOATASCB)
			{ printf ("%g: memória esgotada\n"); return; }

		memclr (chp->ch_scb, sizeof (ATA_SCB));
#endif

		ata_channel[channel]	= chp;

		chp->ch_unit		= unit;
		chp->ch_index		= channel;

		if (legacy)
			chp->ch_flags	= ATA_LEGACY;

		/* Atribui os números às portas */

		(*ctlrp->ac_allocate) (pci, chp);
#ifndef	BOOT
		chp->ch_irq		= (irq >= 0) ? irq : PATA_IRQ_BASE + unit;
#endif
	}

	ata_next_channel = end_channel;

}	/* end pci_ata_attach */

/*
 ****************************************************************
 *	Anexa controladores ATA PCI				*
 ****************************************************************
 */
void
pci_ata_attach_2 (void)
{
	PCIDATA			*pci;
	ATA_CHANNEL		**ata_channel_ptr;
	ATA_CHANNEL		*chp;
	int			channel = 0;
	ATA_PCI_CTLR		*ctlrp;

#ifndef	BOOT
	const DISKTB		*base_pp, *pp;
	ATA_DEVICE		*adp;
	int			dev;
#endif

#ifndef	BOOT
	/*
	 *	Localiza, na Tabela de Partições, o primeiro dispositivo ATA
	 */
	for (base_pp = disktb; /* abaixo */; base_pp++)
	{
		if (base_pp->p_name[0] == '\0')
			return;

		if (MAJOR (base_pp->p_dev) == IDE_MAJOR)
			break;
	}

	/*
	 *	Inicializa as interrupções
	 */
	disable_int ();

	for (ata_channel_ptr = ata_channel; chp = *ata_channel_ptr++; /* vazio */)
	{
		set_dev_irq (chp->ch_irq, PL, chp->ch_index, ataint);
	}

	enable_int ();
#endif	BOOT

	/*
	 *	Inicializa os canais
	 */
	for (ata_channel_ptr = ata_channel; chp = *ata_channel_ptr++; channel++)
	{
		pci	= chp->ch_pci;
		ctlrp	= chp->ch_ctlrp;

		if (ctlrp->ac_pci_name_written++ == 0)
		{
			printf ("ata%d: <%s, ", ctlrp->ac_index, ctlrp->ac_pci_name);

			if (ctlrp->ac_legacy)
			{
				printf ("(legacy), 14/15, 2 canais>\n");
			}
			else
			{
				printf
				(	"0x%04X, %d, %d canais>\n",
					pci->pci_addr[0], pci->pci_intline, ctlrp->ac_nchannels
				);
			}
		}
#ifndef	BOOT
		if (chp->ch_begin_transaction == NULL)
		{
			chp->ch_begin_transaction = ata_begin_transaction;
			chp->ch_end_transaction   = ata_end_transaction;
		}

		ata_dma_reset (chp);
#endif

		/* Dá o "reset" e procura os dispositivos */

		if (ctlrp->ac_reset)
			(*ctlrp->ac_reset) (pci, chp);

		if (chp->ch_ndevs == 0)
			continue;

#ifndef	BOOT
		/* Inicializa os dispositivos */

		for (dev = 0; dev < NATAT; dev++)
		{
			if ((adp = chp->ch_dev[dev]) == NOATADEVICE)
				continue;

			/* Estabelece a freqüência do DMA */

			if (scb.y_dma_enable && adp->ad_param.support_dma && ctlrp->ac_setmode)
			{
				int	best_mode;

				if ((best_mode = adp->ad_udma_mode) < 0)
				{
					if ((best_mode = adp->ad_wdma_mode) < 0)
						best_mode = adp->ad_pio_mode;
				}

				(*ctlrp->ac_setmode) (pci, adp, adp->ad_transfer_mode);

				if (adp->ad_transfer_mode >= ATA_SA150)
					(*ctlrp->ac_setmode) (pci, adp, adp->ad_transfer_mode);
				elif (best_mode > 0)
					(*ctlrp->ac_setmode) (pci, adp, best_mode);
			}

			/* Procura o nome do dispositivo na Tabela de Partições */

			for (pp = base_pp; /* abaixo */; pp++)
			{
				if (pp->p_name[0] == '\0' || MAJOR (pp->p_dev) != IDE_MAJOR)
				{
#ifdef	DEBUG
					printf ("Procurando %d, %d\n\n", channel, dev);

					for (pp = base_pp; /* abaixo */; pp++)
					{
						if (pp->p_name[0] == '\0')
							break;

						printf ("%-6s, %d, %d\n", pp->p_name, pp->p_unit, pp->p_target);
					}
#endif	DEBUG
					printf
					(	"ata[%d,%d]: dispositivo NÃO encontrado na Tabela de Partições\n",
						ATA_DEVNO
					);
					goto out;
				}

				if (pp->p_unit == channel && pp->p_target == dev)
					break;
			}

			strcpy (adp->ad_dev_nm, pp->p_name);

			/* Imprime as informações sobre o dispositivo */

out:
			ata_print_dev_info (adp);
		}
#endif	BOOT
	}

}	/* end pci_ata_attach_2 */

/*
 ****************************************************************
 *	Descobre se o controlador é antigo			*
 ****************************************************************
 */
int
ata_legacy (PCIDATA *pci)
{
	return
	(
		(pci_read (pci, PCIR_PROGIF, 1) & PCIP_STORAGE_IDE_MASTERDEV) &&
		((pci_read (pci, PCIR_PROGIF, 1) &
		(PCIP_STORAGE_IDE_MODEPRIM | PCIP_STORAGE_IDE_MODESEC)) !=
		(PCIP_STORAGE_IDE_MODEPRIM | PCIP_STORAGE_IDE_MODESEC))
	);

}	/* end ata_legacy */

#ifndef	BOOT
#if (0)
/*
 ****************************************************************
 *	Obtém o estado (para interrupção)			*
 ****************************************************************
 */
int
ata_generic_status (ATA_CHANNEL *chp)
{
	if (read_port (chp->ch_ports[ATA_ALTSTAT]) & ATA_S_BUSY)
	{
		DELAY (100);

		if (read_port (chp->ch_ports[ATA_ALTSTAT]) & ATA_S_BUSY)
			return (0);
	}

	return (1);

}	/* end ata_generic_status */
#endif
#endif

/*
 ****************************************************************
 *	Obtém o estado (para interrupção)			*
 ****************************************************************
 */
int
ata_pci_status (ATA_CHANNEL *chp)
{
	if
	(	chp->ch_ports[ATA_BMCMD_PORT] != 0 &&
		(chp->ch_flags & (ATA_LEGACY|ATA_ALWAYS_DMASTAT)) == ATA_ALWAYS_DMASTAT
	)
	{
		int	bmstat = read_port (chp->ch_ports[ATA_BMSTAT_PORT]) & ATA_BMSTAT_MASK;

		if ((bmstat & (ATA_BMSTAT_ACTIVE | ATA_BMSTAT_INTERRUPT)) != ATA_BMSTAT_INTERRUPT)
			return (0);

		write_port (bmstat & ~ATA_BMSTAT_ERROR, chp->ch_ports[ATA_BMSTAT_PORT]);

		DELAY (1);
	}

	if (read_port (chp->ch_ports[ATA_ALTSTAT]) & ATA_S_BUSY)
	{
		DELAY (100);

		if (read_port (chp->ch_ports[ATA_ALTSTAT]) & ATA_S_BUSY)
			return (0);
	}

	return (1);

}	/* end ata_pci_status */

/*
 ****************************************************************
 *	Atribui os números de ports "default"			*
 ****************************************************************
 */
int
ata_pci_allocate (PCIDATA *pci, ATA_CHANNEL *chp)
{
	int		i, port, altport, dmaport;

	if (pci == NOPCIDATA || pci->pci_addr[0] == 0)
	{
		if (chp->ch_unit == 0)
		{
			port	= ATA_PRIMARY;
			altport	= ATA_PRIMARY + ATA_ALTOFFSET;
			dmaport	= pci == NOPCIDATA ? 0 : pci->pci_addr[4];
		}
		else
		{
			port	= ATA_SECONDARY;
			altport	= ATA_SECONDARY + ATA_ALTOFFSET;
			dmaport	= pci == NOPCIDATA ? 0 : pci->pci_addr[4] + ATA_BMIOSIZE;
		}
	}
	else
	{
		if (chp->ch_unit == 0)
		{
			port	= pci->pci_addr[0];
			altport	= pci->pci_addr[1];
			dmaport	= pci->pci_addr[4];
		}
		else
		{
			port	= pci->pci_addr[2];
			altport	= pci->pci_addr[3];
			dmaport	= pci->pci_addr[4] + ATA_BMIOSIZE;
		}
	}

	for (i = ATA_DATA; i <= ATA_COMMAND; i++)
		chp->ch_ports[i] = port + i;

	chp->ch_ports[ATA_CONTROL] = altport + ((chp->ch_flags & ATA_LEGACY) ? 0 : 2);

	chp->ch_ports[ATA_IDX_ADDR] = 0;

	ata_default_registers (chp);

	for (i = ATA_BMCMD_PORT; i <= ATA_BMDTP_PORT; i++)
		chp->ch_ports[i] = dmaport + (i - ATA_BMCMD_PORT);

	chp->ch_status		= ata_pci_status;

#ifndef	BOOT
	chp->ch_begin_transaction = ata_begin_transaction;
	chp->ch_end_transaction   = ata_end_transaction;
#endif

	return (0);

}	/* end ata_pci_allocate */

/*
 ****************************************************************
 *	Trata os ports com mesmo deslocamento físico		*
 ****************************************************************
 */
void
ata_default_registers (ATA_CHANNEL *chp)
{
	chp->ch_ports[ATA_ERROR]	  = chp->ch_ports[ATA_FEATURE];
	chp->ch_ports[ATA_IREASON] = chp->ch_ports[ATA_COUNT];
	chp->ch_ports[ATA_STATUS]  = chp->ch_ports[ATA_COMMAND];
	chp->ch_ports[ATA_ALTSTAT] = chp->ch_ports[ATA_CONTROL];

}	/* end ata_default_registers */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime os ports informados pelo PCI			*
 ****************************************************************
 */
void
ata_pci_print_ports (PCIDATA *pci)
{
	int	i;

	for (i = 0; i < MAX_BASE_ADDR; i++)
		printf (" port[%d] = %P", i, pci->pci_addr[i]);

	printf (" dmaport = %P\n", pci_read (pci, 0x20, 4));

}	/* end ata_pci_print_ports */
#endif	DEBUG
