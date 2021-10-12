/*
 ****************************************************************
 *								*
 *			ata.c					*
 *								*
 *	Driver para os dispositivos ATA				*
 *								*
 *	Versão	3.0.0, de 20.07.94				*
 *		4.9.0, de 26.04.07				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include <disktb.h>
#include <devmajor.h>
#include <bhead.h>
#include <pci.h>
#include <scsi.h>
#include <ata.h>
#include <uerror.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#undef	DEBUG

const char	err_bits[] = 	"\x08" "BAD_BLOCK"
				"\x07" "ECC_ERROR"
				"\x06" "ID_CRC"
				"\x05" "NO_ID"
				/* Indicador desconhecido */
				"\x03" "ABORT"
				"\x02" "TRK_0"
				"\x01" "BAD_MARK";

/*
 ******	Informações acerca de um disco **************************
 */
entry ATA_CHANNEL	*ata_channel[NATAC];
entry int		ata_next_channel;

/*
 ******	Protótipos de funções ***********************************
 */
void			ata_new_device (ATA_DEVICE *);

/*
 ****************************************************************
 *	Anexação						*
 ****************************************************************
 */
void
ataattach (void)
{
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;
	int			channel, dev, unit;

	/*
	 *	Verifica se foi visto algum controlador IDE/ATA/SATA
	 */
	if (ata_next_channel == 0)
	{
		printf ("ata0: <ATA mystery controller, (legacy), 14/15, 2 canais>\n");

		/* Cria os canais */

		for (unit = 0, channel = 0; channel < 2; channel++, unit++)
		{
			if ((chp = malloc_byte (sizeof (ATA_CHANNEL))) == NOATACONTROLLER)
				{ printf ("%g: memória esgotada\n"); return; }

			memclr (chp, sizeof (ATA_CHANNEL));

			ata_channel[channel]	= chp;

			chp->ch_unit		= unit;
			chp->ch_index		= channel;

			chp->ch_flags		= ATA_LEGACY;

			/* Atribui os números às portas */

			ata_pci_allocate (NOPCIDATA, chp);

			ata_new_find_devices (chp);

#if (0)	/*******************************************************/
			ata_old_find_devices (chp);
#endif	/*******************************************************/
		}  

		ata_next_channel = 2;
	}
	else
	{
		pci_ata_attach_2 ();
	}

	/*
	 *	Preenche a tabela de partições (primeiro os discos fixos)
	 */
	for (channel = 0; channel < ata_next_channel; channel++)
	{
		if ((chp = ata_channel[channel]) == NOATACONTROLLER)
			continue;

		for (dev = 0; dev < NATAT; dev++)
		{
			if ((adp = chp->ch_dev[dev]) == NOATADEVICE)
				continue;

			if (adp->ad_flags & ATAPI_DEV)
				continue;

			ata_new_device (adp);
		}
	}

	/*
	 *	Agora, os discos removíveis
	 */
	for (channel = 0; channel < ata_next_channel; channel++)
	{
		if ((chp = ata_channel[channel]) == NOATACONTROLLER)
			continue;

		for (dev = 0; dev < NATAT; dev++)
		{
			if ((adp = chp->ch_dev[dev]) == NOATADEVICE)
				continue;

			if ((adp->ad_flags & ATAPI_DEV) == 0)
				continue;

			ata_new_device (adp);
		}
	}

}	/* end ataattach */

/*
 ****************************************************************
 *	Preenche a Tabela de Partições para um Dispositivo	*
 ****************************************************************
 */
void
ata_new_device (ATA_DEVICE *adp)
{
	ATA_CHANNEL		*chp = adp->ad_channel;
	DISKTB			*pp;
	char			revision[12];
	static char		drive_letter = 'a';

	/* Nomeia o dispositivo */

	adp->ad_dev_nm[0]  = 'h';
	adp->ad_dev_nm[1]  = 'd';
	adp->ad_dev_nm[2]  = drive_letter++;
	adp->ad_dev_nm[3]  = '\0';

	/* Aloca uma entrada na tabela de partições */

	if ((pp = malloc_byte (sizeof (DISKTB))) == NODISKTB)
		{ printf ("ata_new_device: Memória esgotada\n"); return; }

	memclr (pp, sizeof (DISKTB));

	strcpy (pp->p_name, adp->ad_dev_nm);

	pp->p_blshift = BLSHIFT;
   	pp->p_dev     = MAKEDEV (IDE_MAJOR, 0);

	pp->p_unit    = chp->ch_index;
	pp->p_target  = adp->ad_devno;

	adp->ad_disktb = pp;

	ata_ptr[ata_index++] = adp;

	if (adp->ad_flags & ATAPI_DEV)
	{
		SCSI		*sp = adp->ad_scsi;

		/* Se for ATAPI, processa a inicialização do SCSI */

		sp->scsi_target = pp->p_target;
		sp->scsi_disktb = pp;
		sp->scsi_dev_nm = pp->p_name;
		sp->scsi_cmd	= atastrategy;

		scsi_attach (sp);

		pp->p_blshift   = sp->scsi_blshift;
	}
	else
	{
		const DISK_INFO	*dp = disk_info_table_ptr;

		/* Se NÃO for ATAPI, coleta as informações da BIOS */

		if (dp->info_present)
		{
			daddr_t		CYLSZ;

			adp->ad_bios_head = dp->info_nhead;
			adp->ad_bios_sect = dp->info_nsect;

			CYLSZ		  = adp->ad_bios_head * adp->ad_bios_sect;
		   	adp->ad_bios_cyl  = (adp->ad_disksz + CYLSZ - 1) / CYLSZ;

			if (dp->info_int13ext)
				adp->ad_flags |= HD_INT13_EXT;

			disk_info_table_ptr++;
		}

		memmove (revision, adp->ad_param.revision, 8); revision[8] = '\0';

		printf
		(	"%s: <%s %s, %d MB, geo = (%d, %d, %d, %s)>\n",
			adp->ad_dev_nm, adp->ad_param.model, revision,
			BLtoMB (adp->ad_disksz),
			adp->ad_bios_cyl, adp->ad_bios_head, adp->ad_bios_sect,
			adp->ad_flags & HD_INT13_EXT ? "L" : "G"
		);
	}

}	/* end ata_new_device */

/*
 ****************************************************************
 *	Inicializa a operação					*
 ****************************************************************
 */
int
ataopen (const DISKTB *pp)
{
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		return (-1);

	if (adp->ad_flags & ATAPI_DEV)
	{
		/* Realiza o "open" do SCSI */

		if (scsi_open (adp->ad_scsi) < 0)
			return (-1);
	}

	return (0);

}	/* end ataopen */

/*
 ****************************************************************
 *	Realiza uma operação					*
 ****************************************************************
 */
int
atastrategy (BHEAD *bp)
{
	const DISKTB	*pp = bp->b_disktb;
	ATA_CHANNEL	*chp;
	ATA_DEVICE	*adp;
	int		cmd, rw, count, retry;
	void		*area;
	daddr_t		block;

	/*
	 *	Prólogo
	 */
	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ printf ("ata[%d,%d]: Unidade NÃO presente\n", pp->p_unit, pp->p_target); return (-1); }

	if (pp->p_size != 0 && (unsigned)bp->b_blkno >= pp->p_size)
		{ printf ("ata[%d,%d]: Bloco inválido: %d\n", ATA_DEVNO, bp->b_blkno); return (-1); }

	block = (bp->b_blkno + pp->p_offset) >> (adp->ad_blshift - BLSHIFT);

	/*
	 *	Trata os dispositivos ATAPI
	 */
	if (adp->ad_flags & ATAPI_DEV)
	{
		ATA_SCB		scb;
		int		ret;

		if ((bp->b_flags & B_STAT) == 0)
		{
			/* É um comando regular de leitura ou escrita */

			if ((bp->b_flags & (B_READ|B_WRITE)) == B_READ)
				bp->b_cmd_txt[0] = SCSI_CMD_READ_BIG;
			else
				bp->b_cmd_txt[0] = SCSI_CMD_WRITE_BIG;

			bp->b_cmd_txt[1] = 0;
			*(long *)&bp->b_cmd_txt[2] = long_endian_cv (block);
			bp->b_cmd_txt[6] = 0;
			*(short *)&bp->b_cmd_txt[7] = short_endian_cv (bp->b_base_count >> adp->ad_blshift);
			bp->b_cmd_txt[9] = 0;
		}

		/*
		 *	Tenta algumas vezes...
		 */
		for (retry = 8; retry > 0; retry--)
		{
			scb.scb_step	   = 0;
			scb.scb_max_pio_sz = BLSZ;
			scb.scb_flags	   = bp->b_flags;

			scb.scb_blkno	   = block;
			scb.scb_area	   = bp->b_addr;
			scb.scb_count	   = scb.scb_bytes_requested = bp->b_base_count;

			scb.scb_cmd	   = bp->b_cmd_txt;
			scb.scb_result	   = scb.scb_bytes_transferred = 0;

			if ((ret = atapi_start (adp, &scb)) == 0)
				return (0);

			if (ret > 0)
				return (-1);
		}

		return (-1);

	}	/* end (atapi_dev) */

	/*
	 *	Dispositivos não ATAPI: processa o comando por status
	 */
	area = bp->b_addr;
	rw   = bp->b_flags & (B_READ|B_WRITE);
	cmd  = (rw == B_WRITE) ? ATA_C_WRITE : ATA_C_READ;

#ifdef	DEBUG
	printf
	(	"ata[%d,%d]: %s blkno = %d, addr = %P, count = 1\n", 
		ATA_DEVNO, rw == B_WRITE ? "WRITE" : "READ", block, area
	);
#endif	DEBUG

	retry = 8; count = bp->b_base_count;

    again:
	while (count > 0)
	{
		int	t;

		ata_command (adp, cmd, block, 1 /* count */, 0 /* features */, ATA_IMMEDIATE);

		if (ata_wait (adp, ATA_S_READY|ATA_S_DSC|ATA_S_DRQ) != 0)
			goto error;

		/* Limpa o pedido pendente de interrupção */

		if (chp->ch_status)
			(*chp->ch_status) (chp);

		/* Lê/escreve a informação */

		if (rw == B_WRITE)
			(*adp->ad_write_port) (chp->ch_ports[ATA_DATA], area, BLSZ >> adp->ad_pio_shift);
		else
			(*adp->ad_read_port)  (chp->ch_ports[ATA_DATA], area, BLSZ >> adp->ad_pio_shift);

		/* Espera desocupar */

		for (t = 100000; /* abaixo */; t--)
		{
			if (t <= 0)
			{
				printf ("ata[%d,%d]: Tempo esgotado esperando desocupar\n", ATA_DEVNO);
				goto error;
			}

			if ((read_port (chp->ch_ports[ATA_STATUS]) & ATA_S_BUSY) == 0)
				break;

			DELAY (10);
		}

		block++; area += BLSZ; count -= BLSZ;
	}

	return (0);

	/*
	 *	Caso de erro
	 */
    error:
	printf
	(	"ata[%d,%d]: Erro de %s blkno = %d, addr = %P, count = 1, error = %b\n", 
		ATA_DEVNO, rw == B_WRITE ? "ESCRITA" : "LEITURA", block, area,
		read_port (chp->ch_ports[ATA_ERROR]), err_bits
	);

	if (--retry >= 0)
		goto again;

	return (-1);

}	/* end atastrategy */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
ata_ctl (const DISKTB *pp, int cmd, int arg)
{
	ATA_CHANNEL	*chp;
	ATA_DEVICE	*adp;

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ printf ("ata[%d,%d]: Unidade NÃO presente\n", pp->p_unit, pp->p_target); return (-1); }

	if ((adp->ad_flags & ATAPI_DEV) == 0)
		return (-1);

	return (scsi_ctl (adp->ad_scsi, pp, cmd, arg));

}	/* end ata_ctl */
