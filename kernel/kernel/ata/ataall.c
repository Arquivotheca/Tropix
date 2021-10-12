/*
 ****************************************************************
 *								*
 *			ataall.c				*
 *								*
 *	Miscelânea de Rotinas auxiliares			*
 *								*
 *	Versão	3.2.3, de 22.02.00				*
 *		4.9.0, de 16.04.07				*
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
#include "../h/scb.h"
#include "../h/devhead.h"
#include "../h/bhead.h"

#include "../h/frame.h"
#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

#endif

extern ATA_CHANNEL	*ata_channel[];
extern int		ata_next_channel;

/*
 ******	Protótipos de funções ***********************************
 */
#ifndef	BOOT
void		ataint (struct intr_frame);
#endif

int		ata_getparam (ATA_CHANNEL *, int, int);
void		ata_disk_attach (ATA_DEVICE *);
void		ata_atapi_attach (ATA_DEVICE *);

int		ata_command (ATA_DEVICE *, int, daddr_t, int, int, int);
int		ata_wait (ATA_DEVICE *, int);

void		ata_bswap (char *, int);
void		ata_btrim (char *, int);
void		ata_bpack (char *, char *, int);
int		ata_disk_version (int);

#ifndef	BOOT
/*
 ****************************************************************
 *	Imprime informações sobre o dispositivo			*
 ****************************************************************
 */
void
ata_print_dev_info (const ATA_DEVICE *adp)
{
	char	revision[12];

	memmove (revision, adp->ad_param.revision, 8); revision[8] = '\0';

	printf
	(	"%s[%d,%d]: <%s %s, %d MB, %s",
		adp->ad_dev_nm, ATA_DEVNO,
		adp->ad_param.model, revision,
		adp->ad_disksz >> 11,
		adp->ad_flags & HD_CHS_USED ? "GEO"
					   : adp->ad_param.support.address48 ? "LBA48" : "LBA32"
	);

	if (adp->ad_flags & ATAPI_DEV)
		printf (", ATAPI DRQ = %d", adp->ad_param.drq_type);

	printf (", %s", ata_dmamode_to_str (adp->ad_transfer_mode));

	if (adp->ad_transfer_mode > 0 && adp->ad_transfer_mode <= ATA_PIO_MAX)
		printf (", (%d bytes/intr)", adp->ad_max_pio_sz);

	printf (">\n");

}	/* end ata_print_dev_info */
#endif	BOOT

/*
 ****************************************************************
 *	Testa a existência de dispositivos (PATA)		*
 ****************************************************************
 */
int
ata_generic_reset (PCIDATA *pci, ATA_CHANNEL *chp)
{
#if (0)	/*******************************************************/
	return (ata_old_find_devices (chp));
#endif	/*******************************************************/

	return (ata_new_find_devices (chp));

}	/* end ata_generic_reset */

/*
 ****************************************************************
 *	Descobre e anexa os dispositivos em um canal		*
 ****************************************************************
 */
int
ata_new_find_devices (ATA_CHANNEL *chp)
{
	char		ostat0, ostat1 = 0;
	char		stat0 = 0, stat1 = 0;
	int		devices = 0, mask = 0, timeout;

	/*
	 *	Procura o alvo mestre
	 */
	write_port (ATA_D_IBM|ATA_D_LBA|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

	DELAY (10);

	ostat0 = read_port (chp->ch_ports[ATA_STATUS]);

	if ((ostat0 & 0xF8) != 0xF8 && ostat0 != 0xA5)
		{ stat0 = ATA_S_BUSY; mask |= 0x01; }

	/*
	 *	Procura o alvo escravo
	 */
	if ((chp->ch_flags & ATA_NO_SLAVE) == 0)
	{
		/* procura o alvo escravo */

		write_port (ATA_D_IBM|ATA_D_LBA|ATA_SLAVE, chp->ch_ports[ATA_DRIVE]);

		DELAY (10);

		ostat1 = read_port (chp->ch_ports[ATA_STATUS]);

		if ((ostat1 & 0xF8) != 0xF8 && ostat1 != 0xA5)
			{ stat1 = ATA_S_BUSY; mask |= 0x02; }
	}

	if (mask == 0)		/* Nada achou */
		return (0);

	/*
	 *	Dá o "reset" no canal
	 */
	write_port (ATA_D_IBM|ATA_D_LBA|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

	DELAY (10);

	write_port (ATA_A_IDS|ATA_A_RESET, chp->ch_ports[ATA_CONTROL]);

	DELAY (10000); 

	write_port (ATA_A_IDS, chp->ch_ports[ATA_CONTROL]);

	DELAY (100000);

	read_port (chp->ch_ports[ATA_ERROR]);

	/*
	 *	Espera ficar novamente desocupada
	 */
	for (timeout = 0; timeout < 310; timeout++)
	{
		char	lsb, msb, err;

		if ((mask & 0x01) && (stat0 & ATA_S_BUSY))
		{
			write_port (ATA_D_IBM|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

			DELAY (10);

			err	= read_port (chp->ch_ports[ATA_ERROR]);
			lsb	= read_port (chp->ch_ports[ATA_CYL_LSB]);
			msb	= read_port (chp->ch_ports[ATA_CYL_MSB]);
			stat0	= read_port (chp->ch_ports[ATA_STATUS]);

			if
			(	stat0 == err && lsb == err && msb == err &&
				timeout > (stat0 & ATA_S_BUSY ? 100 : 10)
			)
				mask &= ~0x01;

			if (!(stat0 & ATA_S_BUSY))
			{
				if ((err & 0x7F) == ATA_E_ILI)
				{
					if (lsb == ATAPI_MAGIC_LSB && msb == ATAPI_MAGIC_MSB)
						devices |= ATA_ATAPI_MASTER;
					elif (stat0 & ATA_S_READY)
						devices |= ATA_ATA_MASTER;
				}
				elif ((stat0 & 0x0F) && err == lsb && err == msb)
				{
					stat0 |= ATA_S_BUSY;
				}
			}
		}

		if ((mask & 0x02) && (stat1 & ATA_S_BUSY) && !((mask & 0x01) && (stat0 & ATA_S_BUSY)))
		{
			write_port (ATA_D_IBM|ATA_SLAVE, chp->ch_ports[ATA_DRIVE]);

			DELAY (10);

			err	= read_port (chp->ch_ports[ATA_ERROR]);
			lsb	= read_port (chp->ch_ports[ATA_CYL_LSB]);
			msb	= read_port (chp->ch_ports[ATA_CYL_MSB]);
			stat1	= read_port (chp->ch_ports[ATA_STATUS]);

			if
			(	stat1 == err && lsb == err && msb == err &&
				timeout > (stat1 & ATA_S_BUSY ? 100 : 10)
			)
				mask &= ~0x02;

			if (!(stat1 & ATA_S_BUSY))
			{
				if ((err & 0x7F) == ATA_E_ILI)
				{
					if (lsb == ATAPI_MAGIC_LSB && msb == ATAPI_MAGIC_MSB)
						devices |= ATA_ATAPI_SLAVE;
					elif (stat1 & ATA_S_READY)
						devices |= ATA_ATA_SLAVE;
				}
				elif ((stat1 & 0x0F) && err == lsb && err == msb)
				{
					stat1 |= ATA_S_BUSY;
				}
			}
		}

		if (mask == 0x00)       /* nada a esperar */
			break;

		if (mask == 0x01)       /* espera apenas o mestre */
		{
			if (!(stat0 & ATA_S_BUSY) || (stat0 == 0xFF && timeout > 10))
				break;
		}

		if (mask == 0x02)       /* espera apenas o escravo */
		{
			if (!(stat1 & ATA_S_BUSY) || (stat1 == 0xFF && timeout > 10))
				break;
		}

		if (mask == 0x03)	/* espera mestre e escravo */
		{
			if (!(stat0 & ATA_S_BUSY) && !(stat1 & ATA_S_BUSY))
				break;

			if ((stat0 == 0xFF) && (timeout > 20))
				mask &= ~0x01;

			if ((stat1 == 0xFF) && (timeout > 20))
				mask &= ~0x02;
		}

		DELAY (100000);
	}

	DELAY (10);

	write_port (ATA_A_4BIT, chp->ch_ports[ATA_CONTROL]);

	/*
	 *	Anexa o alvo mestre, se encontrado
	 */
	if (devices & ATA_ATA_MASTER)
	{
		if (ata_getparam (chp, 0, ATA_C_ATA_IDENTIFY) == 0)
			ata_disk_attach (chp->ch_dev[0]);
	}

	if (devices & ATA_ATAPI_MASTER)
	{
		if (ata_getparam (chp, 0, ATA_C_ATAPI_IDENTIFY) == 0)
			ata_atapi_attach (chp->ch_dev[0]);
	}

	/*
	 *	Anexa o alvo escravo, se encontrado
	 */
	if (devices & ATA_ATA_SLAVE)
	{
		if (ata_getparam (chp, 1, ATA_C_ATA_IDENTIFY) == 0)
			ata_disk_attach (chp->ch_dev[1]);
	}

	if (devices & ATA_ATAPI_SLAVE)
	{
		if (ata_getparam (chp, 1, ATA_C_ATAPI_IDENTIFY) == 0)
			ata_atapi_attach (chp->ch_dev[1]);
	}

	return (devices);

}	/* end ata_new_find_devices */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Descobre e anexa os dispositivos em um canal		*
 ****************************************************************
 */
int
ata_old_find_devices (ATA_CHANNEL *chp)
{
	char		lsb, msb, ostat0, ostat1 = 0;
	char		stat0 = 0, stat1 = 0;
	int		devices = 0, mask = 0, timeout;

	/*
	 *	Procura o alvo mestre
	 */
	write_port (ATA_D_IBM|ATA_D_LBA|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

	DELAY(10);

	ostat0 = read_port (chp->ch_ports[ATA_STATUS]);

	if ((ostat0 & 0xF8) != 0xF8 && ostat0 != 0xA5)
		{ stat0 = ATA_S_BUSY; mask |= 0x01; }

	if ((chp->ch_flags & ATA_NO_SLAVE) == 0)
	{
		/* procura o alvo escravo */

		write_port (ATA_D_IBM|ATA_D_LBA|ATA_SLAVE, chp->ch_ports[ATA_DRIVE]);

		DELAY(10);

		ostat1 = read_port (chp->ch_ports[ATA_STATUS]);

		if ((ostat1 & 0xF8) != 0xF8 && ostat1 != 0xA5)
			{ stat1 = ATA_S_BUSY; mask |= 0x02; }
	}

	if (mask == 0)		/* Nada achou */
		return (0);

	/*
	 *	Dá o "reset" no canal
	 */
	write_port (ATA_D_IBM|ATA_D_LBA|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

	DELAY (10);

	write_port (ATA_A_IDS|ATA_A_RESET, chp->ch_ports[ATA_CONTROL]);

	DELAY (10000); 

	write_port (ATA_A_IDS, chp->ch_ports[ATA_CONTROL]);

	DELAY (100000);

	read_port (chp->ch_ports[ATA_ERROR]);

	/*
	 *	Espera ficar novamente desocupada
	 */
	for (timeout = 0; timeout < 310000; timeout++)
	{
		if (stat0 & ATA_S_BUSY)
		{
			write_port (ATA_D_IBM|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

			DELAY (10);

			stat0 = read_port (chp->ch_ports[ATA_STATUS]);

			if (!(stat0 & ATA_S_BUSY))
			{
				/* Procura a assinatura ATAPI */

				lsb = read_port (chp->ch_ports[ATA_CYL_LSB]);
				msb = read_port (chp->ch_ports[ATA_CYL_MSB]);

				if (lsb == ATAPI_MAGIC_LSB && msb == ATAPI_MAGIC_MSB)
					devices |= ATA_ATAPI_MASTER;
			}
		}

		if (stat1 & ATA_S_BUSY)
		{
			write_port (ATA_D_IBM|ATA_SLAVE, chp->ch_ports[ATA_DRIVE]);

			DELAY (10);

			stat1 = read_port (chp->ch_ports[ATA_STATUS]);

			if (!(stat1 & ATA_S_BUSY))
			{
				/* Procura a assinatura ATAPI */

				lsb = read_port (chp->ch_ports[ATA_CYL_LSB]);
				msb = read_port (chp->ch_ports[ATA_CYL_MSB]);

				if (lsb == ATAPI_MAGIC_LSB && msb == ATAPI_MAGIC_MSB)
					devices |= ATA_ATAPI_SLAVE;
			}
		}

		if (mask == 1 && !(stat0 & ATA_S_BUSY))	/* Esperando apenas pelo mestre */
			break;

		if (mask == 2 && !(stat1 & ATA_S_BUSY))	/* Esperando apenas pelo escravo */
			break;

		if (mask == 3 && !(stat0 & ATA_S_BUSY) && !(stat1 & ATA_S_BUSY)) /* Ambos */
			break;

		DELAY (100);
	}

	if (stat0 & ATA_S_BUSY)
		mask &= ~0x01;

	if (stat1 & ATA_S_BUSY)
		mask &= ~0x02;

	if (!mask)
		return (0);

	/*
	 *	Encontra (ou não) os dispositivos ATA
	 */
	if (mask & 0x01 && ostat0 != 0x00 && !(devices & ATA_ATAPI_MASTER))
	{
		write_port (ATA_D_IBM|ATA_MASTER, chp->ch_ports[ATA_DRIVE]);

		DELAY (10);

		write_port (0x58, chp->ch_ports[ATA_ERROR]);
		write_port (0xA5, chp->ch_ports[ATA_CYL_LSB]);

		lsb = read_port (chp->ch_ports[ATA_ERROR]);
		msb = read_port (chp->ch_ports[ATA_CYL_LSB]);

		if (lsb != 0x58 && msb == 0xA5)
			devices |= ATA_ATA_MASTER;
	}

	if (mask & 0x02 && ostat1 != 0x00 && !(devices & ATA_ATAPI_SLAVE))
	{
		write_port (ATA_D_IBM|ATA_SLAVE, chp->ch_ports[ATA_DRIVE]);

		DELAY (10);

		write_port (0x58, chp->ch_ports[ATA_ERROR]);
		write_port (0xA5, chp->ch_ports[ATA_CYL_LSB]);

		lsb = read_port (chp->ch_ports[ATA_ERROR]);
		msb = read_port (chp->ch_ports[ATA_CYL_LSB]);

		if (lsb != 0x58 && msb == 0xA5)
			devices |= ATA_ATA_SLAVE;
	}

	/*
	 *	Anexa o alvo escravo, se encontrado
	 */
	if (devices & ATA_ATA_SLAVE)
	{
		if (ata_getparam (chp, 1, ATA_C_ATA_IDENTIFY) == 0)
			ata_disk_attach (chp->ch_dev[1]);
	}

	if (devices & ATA_ATAPI_SLAVE)
	{
		if (ata_getparam (chp, 1, ATA_C_ATAPI_IDENTIFY) == 0)
			ata_atapi_attach (chp->ch_dev[1]);
	}

	/*
	 *	Anexa o alvo mestre, se encontrado
	 */
	if (devices & ATA_ATA_MASTER)
	{
		if (ata_getparam (chp, 0, ATA_C_ATA_IDENTIFY) == 0)
			ata_disk_attach (chp->ch_dev[0]);
	}

	if (devices & ATA_ATAPI_MASTER)
	{
		if (ata_getparam (chp, 0, ATA_C_ATAPI_IDENTIFY) == 0)
			ata_atapi_attach (chp->ch_dev[0]);
	}

	return (devices);

}	/* end ata_old_find_devices */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Obtém os parâmetros do dispositivo			*
 ****************************************************************
 */
int
ata_getparam (ATA_CHANNEL *chp, int devno, int command)
{
	int		retry, wanted, bits;
	ATA_PARAM	tst_ataparam, *atp;
	ATA_DEVICE	*adp;

	if ((adp = malloc_byte (sizeof (ATA_DEVICE))) == NOATADEVICE)
		{ printf ("%g: memória esgotada\n"); return (-1); }

	memclr (adp, sizeof (ATA_DEVICE));

	adp->ad_channel = chp;
	adp->ad_devno   = devno;

	atp = &adp->ad_param;

	/*
	 *	Prepara os bits esperados
	 */
	if (command == ATA_C_ATAPI_IDENTIFY)
		wanted = ATA_S_DRQ;
	else
		wanted = ATA_S_READY|ATA_S_DSC|ATA_S_DRQ;

	/*
	 *	Determina se pode usar PIO de 32 bits
	 */
	for (bits = 16; bits <= 32; bits += 16)
	{
		/* Repete para algumas unidades */

		for (retry = 8; /* abaixo */; retry--)
		{
			if (retry <= 0)
			{
#undef	DEBUG
#ifdef	DEBUG
				printf
				(	"ata[%d,%d]: Excedido o número de tentativas do comando IDENTIFY\n",
					ATA_DEVNO
				);
#endif	DEBUG
#undef	DEBUG
				free_byte (adp); return (-1);
			}

			if (ata_command (adp, command, 0, 0, 0, ATA_IMMEDIATE) < 0)
				printf ("ata[%d,%d]: O comando IDENTIFY falhou\n", ATA_DEVNO);

			if (ata_wait (adp, wanted) == 0)
				break;
		}

		/* Lê a estrutura */

		if (bits == 16)
		{
			read_port_string_short
			(
				chp->ch_ports[ATA_DATA],
				atp,
				sizeof (ATA_PARAM) / sizeof (short)
			);
		}
		else
		{
			read_port_string_long
			(
				chp->ch_ports[ATA_DATA],
				&tst_ataparam,
				sizeof (ATA_PARAM) / sizeof (long)
			);
		}
	}

#ifndef BOOT
	if (memeq (&tst_ataparam, atp, sizeof (ATA_PARAM)))
	{
		adp->ad_read_port  = read_port_string_long;
		adp->ad_write_port = write_port_string_long;
		adp->ad_pio_shift  = 2;
	}
	else
#endif BOOT
	{
		adp->ad_read_port  = read_port_string_short;
		adp->ad_write_port = write_port_string_short;
		adp->ad_pio_shift  = 1;
	}

	/*
	 *	Ajeita alguns campos ...
	 */
	if
	(	command == ATA_C_ATA_IDENTIFY ||
		!((atp->model[0] == 'N' && atp->model[1] == 'E') ||
		(atp->model[0] == 'F' && atp->model[1] == 'X') ||
		(atp->model[0] == 'P' && atp->model[1] == 'i'))
	)
		ata_bswap (atp->model, sizeof (atp->model));

	ata_btrim (atp->model,    sizeof (atp->model));
	ata_bpack (atp->model,    atp->model, sizeof (atp->model));

	ata_bswap (atp->revision, sizeof (atp->revision));
	ata_btrim (atp->revision, sizeof (atp->revision));
	ata_bpack (atp->revision, atp->revision, sizeof (atp->revision));

	/*
	 *	Mais um dispositivo no canal
	 */
	chp->ch_dev[devno] = adp; chp->ch_ndevs++;

#ifndef	BOOT
	/*
	 *	Obtém os modos de transferência
	 */
	adp->ad_pio_mode  = adp->ad_wdma_mode = adp->ad_udma_mode = -1;

	if (atp->atavalid & ATA_FLAG_64_70)
	{
		if   (atp->apiomodes & 2)
			adp->ad_pio_mode = ATA_PIO4;
		elif (atp->apiomodes & 1)
			adp->ad_pio_mode = ATA_PIO3;
	}
	else
	{
		adp->ad_pio_mode = atp->retired_piomode;
	}

	if   (atp->mwdmamodes & 4)
		adp->ad_wdma_mode = ATA_WDMA2;
	elif (atp->mwdmamodes & 2)
		adp->ad_wdma_mode = ATA_WDMA1;
	elif (atp->mwdmamodes & 1)
		adp->ad_wdma_mode = ATA_WDMA0;

	if (atp->atavalid & ATA_FLAG_88)
	{
		if   (atp->udmamodes & 0x40)
			adp->ad_udma_mode = ATA_UDMA6;
		elif (atp->udmamodes & 0x20)
			adp->ad_udma_mode = ATA_UDMA5;
		elif (atp->udmamodes & 0x10)
			adp->ad_udma_mode = ATA_UDMA4;
		elif (atp->udmamodes & 8)
			adp->ad_udma_mode = ATA_UDMA3;
		elif (atp->udmamodes & 4)
			adp->ad_udma_mode = ATA_UDMA2;
		elif (atp->udmamodes & 2)
			adp->ad_udma_mode = ATA_UDMA1;
		elif (atp->udmamodes & 1)
			adp->ad_udma_mode = ATA_UDMA0;
	}

	adp->ad_transfer_mode = (adp->ad_pio_mode == ATA_PIO4) ? ATA_PIO4 : ATA_PIO2;
#endif	BOOT

	return (0);

}	/* end ata_getparam */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicia uma operação em um dispositivo			*
 ****************************************************************
 */
int
ata_begin_transaction (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			cmd, rw;

	/*
	 *	Verifica se será possível usar o DMA
	 */
	scbp->scb_bytes_requested = scbp->scb_count;

	if
	(	(scbp->scb_flags & B_STAT) == 0 &&
		adp->ad_transfer_mode >= ATA_DMA && ata_dma_load (adp, scbp) == 0
	)
	{
		scbp->scb_flags |= B_DMA;
	}
	else
	{
		scbp->scb_flags &= ~B_DMA;
		scbp->scb_bytes_requested = MIN (scbp->scb_count, scbp->scb_max_pio_sz);
	}

	/*
	 *	Trata separadamente os dispositivos ATAPI
	 */
	if (adp->ad_flags & ATAPI_DEV)
		{ atapi_start (adp); return (0); }

	rw = scbp->scb_flags & (B_READ|B_WRITE);

#ifdef	MSG
	if (CSWT (10))
	{
		printf
		(	"ata[%d,%d]: %s block %d (%s), area = %P, count = %d, %s\n",
			ATA_DEVNO,
			rw == B_WRITE ? "WRITE" : "READ",
			scbp->scb_blkno,
			adp->ad_flags & HD_CHS_USED ? "GEO" : "LBA",
			scbp->scb_area, scbp->scb_bytes_requested,
			scbp->scb_flags & B_DMA ? "DMA" : (adp->ad_pio_shift == 1) ? "PIO 16" : "PIO 32"
		);
	}
#endif	MSG

	/*
	 *	Escolhe o comando adequado
	 */
	if (scbp->scb_flags & B_DMA)
		cmd = (rw == B_WRITE) ? ATA_C_WRITE_DMA : ATA_C_READ_DMA;
	elif (scbp->scb_bytes_requested > BLSZ)
		cmd = (rw == B_WRITE) ? ATA_C_WRITE_MUL : ATA_C_READ_MUL;
	else
		cmd = (rw == B_WRITE) ? ATA_C_WRITE : ATA_C_READ;

	/*
	 *	Envia o comando
	 */
	ata_command (adp, cmd, scbp->scb_blkno, scbp->scb_bytes_requested >> adp->ad_blshift, 0, ATA_IMMEDIATE);

	/*
	 *	Se estiver usando DMA, inicia a operação e retorna
	 */
	if (scbp->scb_flags & B_DMA)
		{ ata_dma_start (chp, rw); return (0); }

	/*
	 *	Leitura por PIO: aguarda a interrupção
	 */
	if (rw == B_READ)
		return (0);

	/*
	 *	Escrita por PIO: fornece os dados
	 */
	ata_wait (adp, ATA_S_READY|ATA_S_DRQ);

	(*adp->ad_write_port) (chp->ch_ports[ATA_DATA], scbp->scb_area, scbp->scb_bytes_requested >> adp->ad_pio_shift);

	return (0);

}	/* end ata_begin_transaction */

/*
 ****************************************************************
 *	Analisa o resultado de uma operação			*
 ****************************************************************
 */
int
ata_end_transaction (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;

	/*
	 *	Lê o estado, o que acarreta limpar o pedido de interrupção
	 *	Se houve erro, lê a causa
	 */
	adp->ad_status = read_port (chp->ch_ports[ATA_STATUS]);

	if (adp->ad_status & ATA_S_ERROR)
	{
		adp->ad_error = read_port (chp->ch_ports[ATA_ERROR]);
		goto io_error;
	}

	if (adp->ad_flags & ATAPI_DEV)
	{
		/* Processa a próxima etapa do comando ATAPI */

		return (atapi_next_phase (adp, scbp));
	}

	if (scbp->scb_flags & B_DMA)
	{
		/* Utilizou DMA na transferência */

		if (ata_dma_stop (adp) & ATA_BMSTAT_ERROR)
		{
			adp->ad_status |= ATA_S_ERROR;

			printf ("ata[%d,%d]: erro na transferência com DMA\n", ATA_DEVNO);

			adp->ad_transfer_mode = adp->ad_pio_mode;		/* Reverte para PIO */

			goto io_error;
		}
	}
	else
	{
		/* PIO */

		if (ata_wait (adp, 0) != 0)
			goto io_error;

		/* Se foi leitura, obtém o conteúdo do bloco */

		if ((scbp->scb_flags & (B_READ|B_WRITE)) == B_READ)
		{
			if (ata_wait (adp, ATA_S_READY|ATA_S_DRQ) != 0)
			{
				printf ("ata[%d,%d]: TIMEOUT aguardando leitura com PIO\n", ATA_DEVNO);
				goto io_error;
			}

			(*adp->ad_read_port)
			(
				chp->ch_ports[ATA_DATA],
				scbp->scb_area,
				scbp->scb_bytes_requested >> adp->ad_pio_shift
			);
		}
	}

	/*
	 *	A operação terminou com sucesso
	 */
	scbp->scb_bytes_transferred = scbp->scb_bytes_requested;
	scbp->scb_result = 0;

	return (ATA_OP_FINISHED);

	/*
	 *	A operação terminou com erro recuperável
	 */
    io_error:
	scbp->scb_bytes_transferred = 0;
	scbp->scb_result = -1;

	return (ATA_OP_FINISHED);

}	/* end ata_end_transaction */
#endif

/*
 ****************************************************************
 *	Anexa um disco rígido ATA				*
 ****************************************************************
 */
void
ata_disk_attach (ATA_DEVICE *adp)
{
	ATA_PARAM	*atp = &adp->ad_param;
	ulong		lbasize;
	int		version;

	adp->ad_type	  = ATA_HD;

	adp->ad_cyl	  = atp->cylinders;
	adp->ad_head	  = atp->heads;
	adp->ad_sect	  = atp->sectors;
	adp->ad_disksz	  = adp->ad_cyl * adp->ad_head * adp->ad_sect;

	adp->ad_blsz	  = BLSZ;
	adp->ad_blshift	  = BLSHIFT;

	lbasize		  = atp->lba_size;

	version		  = ata_disk_version (atp->version_major);

	/*
	 *	Verifica se este disco deve usar o endereçamento geométrico
	 */
	if (lbasize == 0 || version == 0 || (atp->atavalid & ATA_FLAG_54_58) == 0)
		adp->ad_flags |= HD_CHS_USED;

	/*
	 *	Usa o LBA de 28 bits, se válido
	 */
	if (atp->cylinders == 16383 && adp->ad_disksz < lbasize)
		adp->ad_disksz = lbasize;

	/*
	 *	Usa o LBA de 48 bits, se válido
	 */
	if (adp->ad_param.support.address48)
	{
		if ((lbasize = *(ulong *)&atp->lba_size48_1) > adp->ad_disksz)
			adp->ad_disksz = lbasize;
	}

	adp->ad_cyl = adp->ad_disksz / (adp->ad_head * adp->ad_sect);

	/*
	 *	Liga o CACHE, se não for o "default"
	 */
	if (ata_command (adp, ATA_C_SETFEATURES, 0, 0, ATA_C_F_ENAB_RCACHE, 0))
		printf ("ata[%d,%d]: Não consegui ligar o CACHE\n", ATA_DEVNO);

	/*
	 *	Obtém o número de setores por interrupção
	 */
	adp->ad_max_pio_sz = BLSZ;

	if (version)
	{
		int	secs_per_int = adp->ad_param.sectors_intr;

		if   (secs_per_int > 8)		/* Limita em 4KB */
			secs_per_int = 8;
		elif (secs_per_int < 1)
			secs_per_int = 1;

		if (ata_command (adp, ATA_C_SET_MULTI, 0, secs_per_int, 0, 0) == 0)
			adp->ad_max_pio_sz = secs_per_int << BLSHIFT;
	}

#if (0)	/********************************************************/
    /* enable readahead caching */
    if (atadev->param.support.command1 & ATA_SUPPORT_LOOKAHEAD)
	ata_controlcmd(dev, ATA_SETFEATURES, ATA_SF_ENAB_RCACHE, 0, 0);

    /* enable write caching if supported and configured */
    if (atadev->param.support.command1 & ATA_SUPPORT_WRITECACHE) {
	if (ata_wc)
	    ata_controlcmd(dev, ATA_SETFEATURES, ATA_SF_ENAB_WCACHE, 0, 0);
	else
	    ata_controlcmd(dev, ATA_SETFEATURES, ATA_SF_DIS_WCACHE, 0, 0);
    }
#endif	/********************************************************/

}	/* end ata_disk_attach */

/*
 ****************************************************************
 *	Acha o equivalente de um comando em LBA48		*
 ****************************************************************
 */
int
ata_find_lba48_equivalent (int command)
{
	switch (command)
	{
	    case ATA_C_READ:
		command = ATA_C_READ48;
		break;

	    case ATA_C_READ_MUL:
		command = ATA_C_READ_MUL48;
		break;

	    case ATA_C_READ_DMA:
		command = ATA_C_READ_DMA48;
		break;

	    case ATA_C_READ_DMA_QUEUED:
		command = ATA_C_READ_DMA_QUEUED48;
		break;

	    case ATA_C_WRITE:
		command = ATA_C_WRITE48;
		break;

	    case ATA_C_WRITE_MUL:
		command = ATA_C_WRITE_MUL48;
		break;

	    case ATA_C_WRITE_DMA:
		command = ATA_C_WRITE_DMA48;
		break;

	    case ATA_C_WRITE_DMA_QUEUED:
		command = ATA_C_WRITE_DMA_QUEUED48;
		break;

	    case ATA_C_FLUSHCACHE:
		command = ATA_C_FLUSHCACHE48;
		break;

	    default:
		break;
	}

	return (command);

}	/* end ata_find_lba48_equivalent */

/*
 ****************************************************************
 *	Envia um comando qualquer para um dispositivo		*
 ****************************************************************
 */
int
ata_command (ATA_DEVICE *adp, int command, daddr_t lba_low, int count, int feature, int flags)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	enum			{ lba_high = 0 };

	/*
	 *	Seleciona o dispositivo
	 */
	write_port (ATA_D_IBM | ATA_D_LBA | (adp->ad_devno << 4), chp->ch_ports[ATA_DRIVE]);

	/*
	 *	Está pronta para receber um comando?
	 */
	if (ata_wait (adp, 0) < 0)
	{ 
		printf ("ata[%d,%d]: Tempo expirado tentando enviar comando %02X\n", ATA_DEVNO, command);
		return (-1);
	}

	write_port (ATA_A_4BIT, chp->ch_ports[ATA_CONTROL]);

	/*
	 *	Só utiliza o endereçamento estendido se necessário
	 *	(por enquanto, apenas 32 bits => 2 Tb)
	 */
#if (0)	/*******************************************************/
	if (adp->ad_param.support.address48 && (/* lba_high != 0 || */ lba_low > 0x0FFFFFFF || count > 256))
#endif	/*******************************************************/
	if (adp->ad_param.support.address48 && (/* lba_high != 0 || */ lba_low > 0))
	{
		command = ata_find_lba48_equivalent (command);

		write_port (feature >> 8,	chp->ch_ports[ATA_FEATURE]);
		write_port (feature,		chp->ch_ports[ATA_FEATURE]);
		write_port (count >> 8,		chp->ch_ports[ATA_COUNT]);
		write_port (count,		chp->ch_ports[ATA_COUNT]);
		write_port (lba_low >> 24,	chp->ch_ports[ATA_SECTOR]);
		write_port (lba_low,		chp->ch_ports[ATA_SECTOR]);
		write_port (lba_high,		chp->ch_ports[ATA_CYL_LSB]);
		write_port (lba_low >> 8,	chp->ch_ports[ATA_CYL_LSB]);
		write_port (lba_high >> 8,	chp->ch_ports[ATA_CYL_MSB]);
		write_port (lba_low >> 16,	chp->ch_ports[ATA_CYL_MSB]);

		write_port (ATA_D_LBA | (adp->ad_devno << 4), chp->ch_ports[ATA_DRIVE]);
	}
	else
	{
		ulong		cylin, head, sector, cmd;

		if (adp->ad_flags & HD_CHS_USED)
		{
			/* Endereçamento geométrico */

			sector	= (lba_low % adp->ad_sect) + 1;
			head	= (lba_low / adp->ad_sect) % adp->ad_head;
			cylin	= (lba_low / adp->ad_sect) / adp->ad_head;
			cmd	= ATA_D_IBM;
		}
		else
		{
			/* Endereçamento LBA */

			sector  = lba_low & 0xFF;
			head	= lba_low >> 24;
			cylin	= (lba_low >> 8) & 0xFFFF;
			cmd	= ATA_D_IBM | ATA_D_LBA;
		}

		cmd |= (adp->ad_devno << 4) | (head & 0x0F);

		write_port (feature,	chp->ch_ports[ATA_FEATURE]);
		write_port (count,	chp->ch_ports[ATA_COUNT]);
		write_port (sector,	chp->ch_ports[ATA_SECTOR]);
		write_port (cylin,	chp->ch_ports[ATA_CYL_LSB]);
		write_port (cylin >> 8, chp->ch_ports[ATA_CYL_MSB]);
		write_port (cmd,	chp->ch_ports[ATA_DRIVE]);
	}

	/*
	 *	Envia o comando
	 */
	write_port (command, chp->ch_ports[ATA_COMMAND]);

	if (flags == ATA_IMMEDIATE)		/* Retorna logo */
		return (0);

	/*
	 *	Aguarda ficar pronto
	 */
	if (ata_wait (adp, ATA_S_READY) < 0)
	{ 
		printf ("ata[%d,%d]: Espera esgotada para o comando = %02X\n", ATA_DEVNO, command);
		return (-1);
	}

	return (0);

}	/* end ata_command */

/*
 ****************************************************************
 *	Espera o canal desocupar				*
 ****************************************************************
 */
int
ata_wait (ATA_DEVICE *adp, int bits_wanted)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			timeout, status;

	/*
	 *	Espera desocupar
	 */
	DELAY (1);

	for (timeout = 0; timeout < 5000000; /* abaixo */)
	{
		if ((status = read_port (chp->ch_ports[ATA_ALTSTAT])) == 0xFF)
		{
			write_port (ATA_D_IBM | (adp->ad_devno << 4), chp->ch_ports[ATA_DRIVE]);

			timeout += 1000; DELAY (1000); continue;
		}

		if ((status & ATA_S_BUSY) == 0)
			break;

		if (timeout > 1000)
			{ timeout += 1000; DELAY (1000); }
		else
			{ timeout += 10;   DELAY (10); }
	}

	if (timeout >= 5000000)
		return (-1);

	if (bits_wanted == 0)
		return (status & ATA_S_ERROR);

	DELAY (1);

	/*
	 *	Espera o padrão de bits desejado
	 */
	for (timeout = 5000; timeout > 0; timeout--)
	{
		status = read_port (chp->ch_ports[ATA_ALTSTAT]);

		if ((status & bits_wanted) == bits_wanted)
			return (status & ATA_S_ERROR);

		DELAY (10);
	}

	return (-1);

}	/* end ata_wait */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Espera o canal desocupar				*
 ****************************************************************
 */
int
ata_wait (ATA_DEVICE *adp, int bits_wanted)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			timeout, incr, status;

	/*
	 *	Espera desocupar
	 */
	DELAY (1);

	for (timeout = 0; timeout < 5000000; timeout += incr)
	{
		if ((status = read_port (chp->ch_ports[ATA_ALTSTAT])) == 0xFF)
		{
			write_port (ATA_D_IBM | (adp->ad_devno << 4), chp->ch_ports[ATA_DRIVE]);

			DELAY (1000);
			incr = 1000;
			continue;
		}

		if ((status & ATA_S_BUSY) == 0)
			break;

		incr = (timeout > 1000) ? 1000 : 10;

		DELAY (incr);
	}

	if (status & ATA_S_ERROR)
		adp->ad_error = read_port (chp->ch_ports[ATA_ERROR]);

	adp->ad_status = status;

	if (timeout >= 5000000)
		return (-1);

	if (bits_wanted == 0)
		return (status & ATA_S_ERROR);

	/*
	 *	Espera o padrão de bits desejado
	 */
	for (timeout = 5000; timeout > 0; timeout--)
	{
		status = read_port (chp->ch_ports[ATA_ALTSTAT]);

		if ((status & bits_wanted) == bits_wanted)
		{
			if (status & ATA_S_ERROR)
				adp->ad_error = read_port (chp->ch_ports[ATA_ALTSTAT]);

			return ((adp->ad_status = status) & ATA_S_ERROR);
		}

		DELAY (10);
	}

	adp->ad_status = status;
	return (-1);

}	/* end ata_wait */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Troca a ordem dos bytes					*
 ****************************************************************
 */
void
ata_bswap (char *buf, int len)
{
	char	aux, *cp;

	for (cp = buf; cp < buf + len; cp += 2)
		{ aux = cp[0]; cp[0] = cp[1]; cp[1] = aux; }
	
}	/* end ata_bswap */

/*
 ****************************************************************
 *	Elimina brancos						*
 ****************************************************************
 */
void
ata_btrim (char *buf, int len)
{
	char	*p;

	for (p = buf; p < buf + len; ++p)
	{
		if (*p == '\0')
			*p = ' ';
	}

	for (p = buf + len - 1; p >= buf && *p == ' '; --p)
		*p = '\0';

}	/* end ata_btrim */

/*
 ****************************************************************
 *	Compacta						*
 ****************************************************************
 */
void
ata_bpack (char *src, char *dst, int len)
{
	int	i, j, blank;

	for (i = j = blank = 0 ; i < len; i++)
	{
		if (blank && src[i] == ' ')
			continue;

		if (blank && src[i] != ' ')
			{ dst[j++] = src[i]; blank = 0; continue; }

		if (src[i] == ' ')
		{
			blank = 1;

			if (i == 0)
				continue;
		}

		dst[j++] = src[i];
	}

	if (j < len) 
		dst[j] = 0;

}	/* end ata_bpack */

/*
 ****************************************************************
 *	Procura o primeiro bit mais significativo ligado	*
 ****************************************************************
 */
int
ata_disk_version (int version)
{
	int		b;

	if (version == 0xFFFF)
		return (0);

	for (b = 15; b >= 0; b--)
	{
		if (version & (1 << b))
			return (b);
	}

	return (0);

}	/* end ata_disk_version */

#ifndef	BOOT
/*
 ****************************************************************
 *	Verifica se o cabo tem 80 pinos				*
 ****************************************************************
 */
int
ata_check_80pin (ATA_DEVICE *adp, int mode)
{
	if (mode > ATA_UDMA2 && adp->ad_param.hwres_cblid == 0)
	{
		mode = ATA_UDMA2;
		printf ("ata[%d,%d]: DMA limitado a %s\n", ATA_DEVNO, ata_dmamode_to_str (mode));
	}

	return (mode);

}	/* end ata_check_80pin */
#endif	BOOT
