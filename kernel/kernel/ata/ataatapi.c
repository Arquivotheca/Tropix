/*
 ****************************************************************
 *								*
 *			ataatapi.c				*
 *								*
 *	Rotinas auxiliares para dispositivos ATAPI		*
 *								*
 *	Versão	4.9.0, de 19.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"

#include "../h/pci.h"
#include "../h/scsi.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/ata.h"

#include "../h/extern.h"
#include "../h/proto.h"

static char	sense_cmd[16] = { SCSI_CMD_REQUEST_SENSE, 0, 0, 0, sizeof (struct scsi_sense_data) };

/*
 ****** Protótipos de funções ***********************************
 */
int		atapi_read (ATA_DEVICE *, void *, int);
int		atapi_write (ATA_DEVICE *, void *, int);

/*
 ****************************************************************
 *	Anexa um dispositivo ATAPI				*
 ****************************************************************
 */
void
ata_atapi_attach (ATA_DEVICE *adp)
{
	SCSI		  *sp;
	ATA_PARAM	  *atp = &adp->ad_param;
	static const char zip_id[] = "IOMEGA ZIP 100";

	/*
	 *	Obtém o tamanho (global) do comando SCSI
	 */
	switch (atp->packet_size)
	{
	    case ATAPI_PSIZE_12:
		adp->ad_cmdsz = 12 / sizeof (short);
		break;

	    case ATAPI_PSIZE_16:
		adp->ad_cmdsz = 16 / sizeof (short);
		break;

	    default:
		printf ("ata[%d,%d]: Tamanho %d inválido do comando SCSI\n", ATA_DEVNO, atp->packet_size);
		return;

	}	/* end switch */

	/*
	 *	Examina o tipo
	 */
	switch (atp->type)
	{
	    case ATAPI_TYPE_CDROM:             /* CD-ROM */
		adp->ad_type	= ATAPI_CDROM;
	   	adp->ad_cyl	= adp->ad_head = adp->ad_sect = 1;

		adp->ad_blsz	= 4 * BLSZ;
		adp->ad_blshift	= 2 + BLSHIFT;
		break;

	    case ATAPI_TYPE_DIRECT:            /* ZIP ? */
		if (atp->removable && memeq (atp->model, zip_id, sizeof (zip_id) - 1))
		{
			adp->ad_type	= ATAPI_ZIP;
#ifdef	BOOT
		   	adp->ad_cyl	= adp->ad_bios_cyl  = 96;
			adp->ad_head	= adp->ad_bios_head = 64;
			adp->ad_sect	= adp->ad_bios_sect = 32;
#else
		   	adp->ad_cyl	= 96;
			adp->ad_head	= 64;
			adp->ad_sect	= 32;
#endif
			adp->ad_blsz	= BLSZ;
			adp->ad_blshift	= BLSHIFT;

			break;
		}

		/* continua no caso abaixo */

	    default:
		printf ("ata[%d,%d]: Dispositivo ATAPI não suportado: %02X\n", ATA_DEVNO, atp->type);
		return;

	}	/* end switch (atp->type) */

	/*
	 *	Aloca a estrutura SCSI
	 */
	if ((adp->ad_scsi = sp = malloc_byte (sizeof (SCSI))) == NOSCSI)
	{
		printf ("ata_atapi_attach: erro ao alocar a estrutura SCSI\n");
		return;
	}

	memclr (sp, sizeof (SCSI));

	/*
	 *	Preenche o restante das estruturas
	 */
	adp->ad_flags	 |= ATAPI_DEV;
	adp->ad_disksz	  = adp->ad_cyl * adp->ad_head * adp->ad_sect;
	sp->scsi_is_atapi = 1;
	adp->ad_max_pio_sz = 4 * KBSZ;

}	/* end ata_atapi_attach */

#ifndef	BOOT
/*
 ****************************************************************
 *	Inicia uma operação					*
 ****************************************************************
 */
void
atapi_start (ATA_DEVICE *adp)
{
	BHEAD		*bp	= adp->ad_bp;
	ATA_SCB		*scbp	= bp->b_ptr;

	if ((scbp->scb_flags & B_STAT) == 0)
	{
		/* É um comando regular de leitura ou escrita */

		if ((scbp->scb_flags & (B_READ|B_WRITE)) == B_READ)
			bp->b_cmd_txt[0] = SCSI_CMD_READ_BIG;
		else
			bp->b_cmd_txt[0] = SCSI_CMD_WRITE_BIG;

		bp->b_cmd_txt[1] = 0;
		*(long *)&bp->b_cmd_txt[2] = long_endian_cv (scbp->scb_blkno);
		bp->b_cmd_txt[6] = 0;
		*(short *)&bp->b_cmd_txt[7] = short_endian_cv (scbp->scb_bytes_requested >> adp->ad_blshift);
		bp->b_cmd_txt[9] = 0;
	}

	scbp->scb_cmd = bp->b_cmd_txt;

	/* Envia o comando */

	if (atapi_command (adp, scbp) >= 0)
		return;

	printf ("ata[%d,%d]: ERRO ao enviar o comando %s\n", ATA_DEVNO, scsi_cmd_name (scbp->scb_cmd[0]));

}	/* end atapi_start */

/*
 ****************************************************************
 *	Coloca na fila um comando ATAPI interno			*
 ****************************************************************
 */
int
atapi_internal_cmd (BHEAD *bp)
{
	/*
	 *	Completa o BHEAD
	 */
	bp->b_flags |= B_STAT;

	bp->b_phblkno = bp->b_blkno   = 0;

	/*
	 *	Coloca na fila do "driver" e espera concluir
	 */
	atastrategy (bp);

	EVENTWAIT (&bp->b_done, PBLKIO);	/* Espera concluir */

	return (geterror (bp));

}	/* end atapi_internal_cmd */
#endif

#ifdef	BOOT
/*
 ****************************************************************
 *	Envia o comando ATAPI, processando todas as suas fases	*
 ****************************************************************
 */
int 
atapi_start (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp;
	int			phase, count, op, total_bytes;

	/*
	 *	Envia o comando ATAPI
	 */
	if (atapi_command (adp, scbp) < 0)
		{ printf ("atapi_start: erro ao enviar o comando ATAPI\n"); return (-1); }

	chp = adp->ad_channel;

	/*
	 *	Processa as diversas fases até a conclusão comando
	 */
	for (total_bytes = 0, phase = 1; phase <= 100; phase++)
	{
		/* Espera o canal desocupar */

		for (count = 20000; /* abaixo */; count--)
		{
			if (count <= 0)
			{
				printf ("ata[%d,%d]: Tempo esgotado esperando desocupar\n", ATA_DEVNO);
				return (-1);
			}

			if (((adp->ad_status = read_port (chp->ch_ports[ATA_STATUS])) & ATA_S_BUSY) == 0)
				break;

			DELAY (100);
		}

		/* Processa a próxima fase */

		op = atapi_next_phase (adp, scbp);

		if (op == ATA_OP_CONTINUES_NO_DATA)
			continue;

		scbp->scb_area  += scbp->scb_bytes_transferred;
		scbp->scb_count -= scbp->scb_bytes_transferred;
		total_bytes	+= scbp->scb_bytes_transferred;

		if (op == ATA_OP_FINISHED)
		{
			scbp->scb_bytes_transferred = total_bytes;
			return (scbp->scb_result);
		}
	}

	printf
	(
		"ata[%d,%d]: cmd = %s, número excessivo de fases: %d\n",
		ATA_DEVNO, scsi_cmd_name (scbp->scb_cmd[0]), phase
	);

	return (-1);

}	/* end atapi_start */
#endif	BOOT

/*
 ****************************************************************
 *	Envia o comando ATAPI					*
 ****************************************************************
 */
int
atapi_command (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			count, flags, timeout, reason, status = 0;

	if (scbp->scb_cmd == sense_cmd)
	{
		count = sizeof (struct scsi_sense_data);
		flags = 0;
	}
	else
	{
		count = scbp->scb_bytes_requested;
		flags = (scbp->scb_flags & B_DMA) ? ATA_F_DMA : 0;
	}

	/*
	 *	Inicia a operação no dispositivo ATAPI.
	 */
	if (ata_command (adp, ATA_C_PACKET_CMD, count << 8, 0, flags, ATA_IMMEDIATE) < 0)
	{
		printf ("ata[%d,%d]: NÃO consegui iniciar o comando ATAPI\n", ATA_DEVNO);
		return (-1);
	}

#ifndef	BOOT
	if (flags == ATA_F_DMA)
		ata_dma_start (chp, scbp->scb_flags & (B_READ|B_WRITE));

	/* Retorna, caso o comando deva ser enviado apenas quando vier a interrupção */

	if (adp->ad_param.drq_type == ATAPI_DRQT_INTR)
		return (0);
#endif

	/*
	 *	Pronto para enviar o comando SCSI ?
	 */
	for (timeout = 5000; /* abaixo */; timeout--)
	{
		if (timeout <= 0)
		{
			printf ("ata[%d,%d]: NÃO consegui executar o comando ATAPI\n", ATA_DEVNO);

			adp->ad_status = status; return (-1);
		}

		reason = read_port (chp->ch_ports[ATA_IREASON]);
		status = read_port (chp->ch_ports[ATA_STATUS]);

		if (((reason & (ATA_I_CMD|ATA_I_IN)) | (status & (ATA_S_DRQ|ATA_S_BUSY))) == ATAPI_P_CMDOUT)
			break;

		DELAY (20);
	}

	adp->ad_status = status;

	DELAY (10);	/* Necessário para certos dispositivos lentos */

	/*
	 *	Envia o comando SCSI
	 */
	write_port_string_short (chp->ch_ports[ATA_DATA], scbp->scb_cmd, adp->ad_cmdsz);

	return (0);

}	/* end atapi_command */

/*
 ****************************************************************
 *	Processa a próxima fase de um comando ATAPI		*
 ****************************************************************
 */
int
atapi_next_phase (ATA_DEVICE *adp, ATA_SCB *scbp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	int			is_sense, reason, dma_status = 0;

	/*
	 *	O processamento do comando REQUEST_SENSE NÃO deve afetar os
	 *	campos do SCB, pois o erro pode ter sido recuperado !
	 */
	is_sense = scbp->scb_cmd == sense_cmd;

	if (!is_sense)
		scbp->scb_bytes_transferred = 0;

	/*
	 *	Determina e analisa a causa da interrupção
	 */
	reason = (read_port (chp->ch_ports[ATA_IREASON]) & (ATA_I_CMD|ATA_I_IN)) | (adp->ad_status & ATA_S_DRQ);

	switch (reason)
	{
	    case ATAPI_P_CMDOUT:
		if ((adp->ad_status & ATA_S_DRQ) == 0)
		{
			printf ("ata[%d,%d]: Interrupção espúria\n", ATA_DEVNO);
			scbp->scb_result = -1;
			return (ATA_OP_FINISHED);
		}

		write_port_string_short (chp->ch_ports[ATA_DATA], scbp->scb_cmd, adp->ad_cmdsz);
		return (is_sense ? ATA_OP_CONTINUES_NO_DATA : ATA_OP_CONTINUES);

	    case ATAPI_P_WRITE:
		scbp->scb_bytes_transferred = atapi_write (adp, scbp->scb_area, scbp->scb_bytes_requested);
		return (ATA_OP_CONTINUES);

	    case ATAPI_P_READ:
		if (is_sense)
		{
			atapi_read (adp, &scbp->scb_sense, sizeof (scbp->scb_sense));
			return (ATA_OP_CONTINUES_NO_DATA);
		}
		else
		{
			scbp->scb_bytes_transferred = atapi_read (adp, scbp->scb_area, scbp->scb_bytes_requested);
			return (ATA_OP_CONTINUES);
		}

	    case ATAPI_P_DONEDRQ:
		if (scbp->scb_flags & B_READ)
		{
			if (is_sense)
				atapi_read (adp, &scbp->scb_sense, sizeof (scbp->scb_sense));
			else
				scbp->scb_bytes_transferred = atapi_read (adp, scbp->scb_area, scbp->scb_bytes_requested);
		}
		else
		{
			scbp->scb_bytes_transferred = atapi_write (adp, scbp->scb_area, scbp->scb_bytes_requested);
		}
		break;

	    case ATAPI_P_DONE:
#ifndef	BOOT
		if (scbp->scb_flags & B_DMA)
		{
			/* Usou DMA na transferência */

			dma_status	= ata_dma_stop (adp);
			adp->ad_status	= read_port (chp->ch_ports[ATA_STATUS]);

			scbp->scb_bytes_transferred = scbp->scb_bytes_requested;
		}
#endif	BOOT
		break;

	    default:
		printf
		(
			"ata[%d,%d]: cmd = %s, reason = %P\n",
			ATA_DEVNO, scsi_cmd_name (scbp->scb_cmd[0]), reason
		);
		break;

	}	/* end switch (reason) */

	/*
	 *	Verifica se o comando terminou com sucesso
	 */
	if ((dma_status & ATA_BMSTAT_ERROR) == 0 && (adp->ad_status & (ATA_S_ERROR | ATA_S_DWF)) == 0)
	{
		int		sense;

		if (!is_sense)
			{ scbp->scb_result = 0; return (ATA_OP_FINISHED); }

		/* O comando era um REQUEST SENSE: analisa o resultado */

		sense = scbp->scb_sense.flags & 0x0F;

		if (sense == 2 /* Unit not ready */)
			{ scbp->scb_result = 1; return (ATA_OP_FINISHED); }

		/* Prepara o retorno */

		if ((sense = scsi_sense (adp->ad_scsi, sense)) > 0)
			scbp->scb_result = 0;	/* recuperou */
		elif (sense == 0)
			scbp->scb_result = -1;	/* pode repetir */
		else
			scbp->scb_result = 1;	/* irrecuperável */

		return (ATA_OP_FINISHED);
	}

	/*
	 *	Houve erro na operação 
	 */
	adp->ad_error = read_port (chp->ch_ports[ATA_ERROR]);

#ifdef	DEBUG
	printf
	(
		"ata[%d,%d]: cmd = %s, erro =  %02X\n",
		ATA_DEVNO, scsi_cmd_name (scbp->scb_cmd[0]), adp->ad_error
	);
#endif	DEBUG

	if ((adp->ad_error & ATAPI_SK_MASK) && !is_sense)
	{
		/* Envia o comando REQUEST SENSE */

#ifdef	DEBUG
		printf ("ata[%d,%d]: comando %02X (%s) falhou. Enviando o REQUEST_SENSE\n",
					ATA_DEVNO, scbp->scb_cmd[0], scsi_cmd_name (scbp->scb_cmd[0]));
#endif	DEBUG

		scbp->scb_cmd = sense_cmd;

		if (atapi_command (adp, scbp) >= 0)
			return (ATA_OP_CONTINUES_NO_DATA);
	}

	scbp->scb_result = -1;

	return (ATA_OP_FINISHED);

}	/* end atapi_next_phase */

/*
 ****************************************************************
 *	Leitura ATAPI						*
 ****************************************************************
 */
int
atapi_read (ATA_DEVICE *adp, void *area, int max_sz)
{
	ATA_CHANNEL	*chp = adp->ad_channel;
	int		count;

	count = read_port (chp->ch_ports[ATA_CYL_LSB]) | (read_port (chp->ch_ports[ATA_CYL_MSB]) << 8);

	if (count > max_sz)
		count = max_sz;

	if ((count & 3) == 0)
		(*adp->ad_read_port)    (chp->ch_ports[ATA_DATA], area, count >> adp->ad_pio_shift);
	else
		read_port_string_short (chp->ch_ports[ATA_DATA], area, count / sizeof (short));

	return (count);

}	/* end atapi_read */

/*
 ****************************************************************
 *	Escrita ATAPI						*
 ****************************************************************
 */
int
atapi_write (ATA_DEVICE *adp, void *area, int max_sz)
{
	ATA_CHANNEL	*chp = adp->ad_channel;
	int		count;

	count = read_port (chp->ch_ports[ATA_CYL_LSB]) | (read_port (chp->ch_ports[ATA_CYL_MSB]) << 8);

	if (count > max_sz)
		count = max_sz;

	if ((count & 3) == 0)
		(*adp->ad_write_port)    (chp->ch_ports[ATA_DATA], area, count >> adp->ad_pio_shift);
	else
		write_port_string_short (chp->ch_ports[ATA_DATA], area, count / sizeof (short));

	return (count);

}	/* end atapi_write */
