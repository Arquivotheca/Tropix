/*
 ****************************************************************
 *								*
 *			ata.c					*
 *								*
 *	Driver para dispositivos ATA				*
 *								*
 *	Versão	4.0.0, de 17.10.00				*
 *		4.9.0, de 07.05.07				*
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
#include "../h/scb.h"
#include "../h/region.h"
#include "../h/map.h"

#include "../h/pci.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/disktb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/ioctl.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/scsi.h"
#include "../h/ata.h"
#include "../h/cdio.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	MAX_RETRY	4	/* Número de máximo tentativas */

/*
 ****** Variáveis globais ***************************************
 */
entry ATA_CHANNEL	*ata_channel[NATAC];	/* Os canais dos controladores ATA */
entry int		ata_next_channel;	/* Próxima posição livre em "ata_channel" */

entry DEVHEAD		atatab;			/* Cabeca da lista de dp's e do major */
entry BHEAD		ratabuf;		/* Para as operações "raw" */

entry int		ata_busy;		/* Para verificar o uso */

entry const char	err_bits[] = 		"\x08" "BAD_BLOCK"
						"\x07" "ECC_ERROR"
						"\x06" "ID_CRC"
						"\x05" "NO_ID"
						/* Indicador desconhecido */
						"\x03" "ABORT"
						"\x02" "TRK_0"
						"\x01" "BAD_MARK";

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void		atastart (ATA_CHANNEL *, ATA_SCB *);

#ifdef	TRUE_SCBS
void		ata_init_scb_list (void);
ATA_SCB		*ata_get_scb (ATA_CHANNEL *);
void		ata_put_scb (ATA_SCB *, ATA_CHANNEL *);
#else
#define		ata_get_scb(chp)		(chp)->ch_scb
#define		ata_put_scb(scbp,chp)
#endif	TRUE_SCBS

/*
 ****************************************************************
 *	Anexa os dispositivos ATA/ATAPI				*
 ****************************************************************
 */
int
ataattach (int major)
{
	PCIDATA			*pci;
	ATA_PCI_CTLR		*ctlrp;
	ATA_CHANNEL		*chp;
	int			channel, unit;
	extern void		ataint (struct intr_frame frame);

	/*
	 *	Verifica se foi visto um controlador IDE/ATA/SATA
	 */
	if (ata_next_channel == 0)
	{
		/* Cria o "pci" */

		if ((pci = malloc_byte (sizeof (PCIDATA))) == NULL)
			{ printf ("%g: memória esgotada\n"); return (-1); }

		memclr (pci, sizeof (PCIDATA));

	   /***	pci->pci_addr[0] = 0;		/* Usa as portas padrão ***/

		/* Cria o "controlador" */

		if ((ctlrp = malloc_byte (sizeof (ATA_PCI_CTLR))) == NULL)
			{ printf ("%g: memória esgotada\n"); return (-1); }

		memclr (ctlrp, sizeof (ATA_PCI_CTLR));

		ctlrp->ac_pci_name	= "ATA mystery controller";
	   /***	ctlrp->ac_index		= 0;	***/
		ctlrp->ac_legacy	= 1;
		ctlrp->ac_nchannels	= 2;

		ctlrp->ac_reset		= ata_generic_reset;
		ctlrp->ac_setmode	= NULL;

		/* Cria os canais */

		for (unit = 0, channel = 0; channel < 2; channel++, unit++)
		{
			if ((chp = malloc_byte (sizeof (ATA_CHANNEL))) == NOATACONTROLLER)
				{ printf ("%g: memória esgotada\n"); return (-1); }

			memclr (chp, sizeof (ATA_CHANNEL));

			chp->ch_pci	= pci;
			chp->ch_ctlrp	= ctlrp;

			/* Por enquanto, um SCB por canal */

			if ((chp->ch_scb = malloc_byte (sizeof (ATA_SCB))) == NOATASCB)
				{ printf ("%g: memória esgotada\n"); return (-1); }

			memclr (chp->ch_scb, sizeof (ATA_SCB));

			ata_channel[channel]	= chp;

			chp->ch_unit		= unit;
			chp->ch_index		= channel;

			chp->ch_flags		= ATA_LEGACY;

			/* Atribui os números às portas */

			ata_pci_allocate (NOPCIDATA, chp);

			chp->ch_irq		= PATA_IRQ_BASE + unit;

			set_dev_irq (chp->ch_irq, PL, channel, ataint);
		}

		ata_next_channel = 2;
	}

	/*
	 *	Agora, os dois casos convergem
	 */
	pci_ata_attach_2 ();

	return (0);

}	/* end ataattach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
ataopen (dev_t dev, int oflag)
{
	DISKTB			*pp;
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;

	/*
	 *	Prólogo
	 */
	if ((pp = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica o "O_LOCK"
	 */
	if (pp->p_lock || (oflag & O_LOCK) && pp->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	if (adp->ad_flags & ATAPI_DEV)
	{
		SCSI		*sp = adp->ad_scsi;

		/* Prepara a função interna */

		sp->scsi_cmd = atapi_internal_cmd;

		/* Realiza o "open" do SCSI */

		if (scsi_open (sp, dev, oflag) < 0)
			return (-1);

		/* Trava o meio no primeiro "open", se possível */

		if (sp->scsi_nopen == 0 && sp->scsi_removable)
			scsi_ctl (sp, dev, ZIP_LOCK_UNLOCK, 1);

		sp->scsi_nopen++;

		/* Usa o tamanho de bloco devolvido pelo SCSI */

		if (adp->ad_scsi->scsi_blsz == CD_RAW_BLSZ)
		{
			adp->ad_scsi->scsi_blsz    = 4 * BLSZ;
			adp->ad_scsi->scsi_blshift = 2 + BLSHIFT;
		}

		adp->ad_blsz	= adp->ad_scsi->scsi_blsz;
		adp->ad_blshift	= adp->ad_scsi->scsi_blshift;

	} 	/* end if (adp->ad_flags & ATAPI_DEV) */

	/*
	 *	Sucesso
	 */
	pp->p_nopen++;

	if (oflag & O_LOCK)
		pp->p_lock = 1;

	return (0);

}	/* end ataopen */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
ataclose (dev_t dev, int flag)
{
	DISKTB			*pp;
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;

	/*
	 *	Prólogo
	 */
	pp = &disktb[MINOR (dev)];

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ u.u_error = ENXIO; return (-1); }

	if (--pp->p_nopen <= 0)
		pp->p_lock = 0;

	if (adp->ad_flags & ATAPI_DEV)
	{
		/* Se for ATAPI, realiza o "close" do scsi */

		SCSI		*sp = adp->ad_scsi;

		if (--sp->scsi_nopen <= 0)
		{
			if (sp->scsi_removable)
				scsi_ctl (sp, dev, ZIP_LOCK_UNLOCK, 0 /* unlock */);

			scsi_close (sp, dev);
		}
	}

	return (0);

}	/* end ataclose */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
atastrategy (BHEAD *bp)
{
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;
	const DISKTB		*pp;
	daddr_t			bn;

	pp = &disktb[MINOR (bp->b_phdev)];

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ printf ("atastrategy: dispositivo inválido\n"); goto bad; }

	/*
	 *	Verifica a validade do pedido
	 */
	if ((bn = bp->b_phblkno) < 0 || bn + BYTOBL (bp->b_base_count) > pp->p_size)
	{
		if ((bp->b_flags & B_STAT) == 0)
			goto bad;
	}

	/*
	 *	Coloca o pedido na fila e inicia a operação
	 */
	bp->b_cylin	= bn + pp->p_offset;
	bp->b_dev_nm	= pp->p_name;
	bp->b_retry_cnt = MAX_RETRY;
	bp->b_ptr	= NOATASCB;

	splx (irq_pl_vec[chp->ch_irq]);

	insdsort (&adp->ad_queue, bp); ata_busy++;

	/*
	 *	Se o canal está livre, inicia logo a operação
	 */
	if (TAS (&chp->ch_busy) >= 0)
		atastart (chp, ata_get_scb (chp));

	spl0 ();

	return (0);

    bad:
	bp->b_error  = ENXIO;
	bp->b_flags |= B_ERROR;

	EVENTDONE (&bp->b_done);

	return (-1);

}	/* end atastrategy */

/*
 ****************************************************************
 *	Inicia uma operação em uma unidade			*
 ****************************************************************
 */
void
atastart (ATA_CHANNEL *chp, ATA_SCB *scbp)
{
	ATA_DEVICE		*adp;
	DEVHEAD			*dp;
	BHEAD			*bp;
	const DISKTB		*pp;

	if ((adp = chp->ch_active_dev) == NOATADEVICE)
	{
		int		i;

		/* Obtém um pedido, examinando as filas dos dispositivos */

		for (i = 0; /* abaixo */; i++)
		{
			if (i >= NATAT)
			{
			   	ata_put_scb (scbp, chp);		/* Nada a fazer */
				CLEAR (&chp->ch_busy);
				return;
			}

			if (++chp->ch_target >= NATAT)
				chp->ch_target = 0;

			if ((adp = chp->ch_dev[chp->ch_target]) == NOATADEVICE)
				continue;

			dp = &adp->ad_queue; 

			SPINLOCK (&dp->v_lock);

			if ((bp = dp->v_first) != NOBHEAD)
				break;

			SPINFREE (&dp->v_lock);
		}

		/* Achou um pedido */

		SPINFREE (&dp->v_lock);

		chp->ch_active_dev 	= adp;		/* dispositivo ativo corrente */
		adp->ad_bp		= bp;
		bp->b_ptr		= scbp;

		scbp->scb_step		= 0;
		scbp->scb_max_pio_sz	= adp->ad_max_pio_sz;
	}
	else
	{
		/* Continuação do pedido anterior */

		bp = adp->ad_bp;

		scbp->scb_step++;

	} 	/* end if (há operação pendente) */

	/*
	 *	Inicializa o SCB
	 */
	if (scbp->scb_step == 0)
	{
		scbp->scb_flags = bp->b_flags;

		if ((scbp->scb_flags & B_STAT) == 0)
		{
			/* É um comando regular de leitura ou escrita */

			pp = &disktb[MINOR (bp->b_phdev)];

			scbp->scb_blkno	= (ulong)(bp->b_phblkno + pp->p_offset) >> (adp->ad_blshift - BLSHIFT);
		}
		else
		{
			scbp->scb_blkno	= 0;
		}

		scbp->scb_area	 = bp->b_base_addr;
		scbp->scb_count  = bp->b_base_count;
		scbp->scb_result = 0;
	}

	/*
	 *	Inicia a operação
	 *	"scbp->scb_bytes_requested" será inicializado em "chp->ch_begin_transaction"
	 */
	scbp->scb_bytes_transferred = 0;

	(*chp->ch_begin_transaction) (adp, scbp);

}	/* end atastart */

/*
 ****************************************************************
 *	Interrupção						*
 ****************************************************************
 */
void
ataint (struct intr_frame frame)
{
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;
	BHEAD			*bp;
	DEVHEAD			*dp;
	ATA_SCB			*scbp;
	int			op;

	/*
	 *	Verifica se a interrupção é legítima
	 */
	chp = ata_channel[frame.if_unit];

#if (0)	/*******************************************************/
printf ("%g: canal = %d\n", frame.if_unit); getchar ();
#endif	/*******************************************************/

if (chp->ch_status == NULL)
{
	printf ("%g: Canal %d, função STATUS NULA\n", frame.if_unit);
	getchar ();
	return;
}

	if (chp->ch_status && !(*chp->ch_status) (chp))
		return;

	if ((adp = chp->ch_active_dev) == NOATADEVICE)
	{
#ifdef	DEBUG
		int altstat, status, ireason, error;

		ireason = read_port (chp->ch_ports[ATA_IREASON]);
		altstat = read_port (chp->ch_ports[ATA_ALTSTAT]);
		status  = read_port (chp->ch_ports[ATA_STATUS]);
		error   = 0;

		if (status & ATA_S_ERROR)
			error = read_port (chp->ch_ports[ATA_ERROR]);

		printf ("ataint: channel = %d, ireason = %02X\n", chp->ch_index, ireason);
		printf ("        altstat = %02X, status = %02X, error = %02X\n", altstat, status, error);
#endif	DEBUG
		read_port (chp->ch_ports[ATA_STATUS]);
		return;
	}

	bp = adp->ad_bp; scbp = bp->b_ptr;

	/*
	 *	Processa o final de mais uma fase e analisa o resultado
	 */
	op = (*chp->ch_end_transaction) (adp, scbp);

	if (op == ATA_OP_CONTINUES_NO_DATA)		/* A operação continua, mas NÃO há dados transferidos */
		return;

	scbp->scb_area  += scbp->scb_bytes_transferred;
	scbp->scb_count -= scbp->scb_bytes_transferred;

	if (op == ATA_OP_CONTINUES)			/* Aguarda mais interrupções para concluir */
		return;

	switch (scbp->scb_result)			/* A operação terminou ... */
	{
	    case 0:					/* ... com sucesso */
		if (scbp->scb_flags & B_STAT)
			goto done;
		break;

	    case 1:					/* ... com erro irrecuperável */
		bp->b_retry_cnt = 0;
		/* continua abaixo */

	    default:					/* ... com erro recuperável */
		goto io_error;
	}

	if (scbp->scb_count <= 0)
		goto done;

	if (scbp->scb_bytes_transferred != scbp->scb_bytes_requested)
	{
		printf
		(	"ata[%d,%d]: bytes requisitados = %d, bytes transferidos = %d\n",
			ATA_DEVNO, scbp->scb_bytes_transferred, scbp->scb_bytes_requested
		);
	}

	/* Prepara a continuação da operação */

	scbp->scb_blkno += scbp->scb_bytes_requested >> adp->ad_blshift;

	atastart (chp, scbp); 	return;

	/*
	 *	Houve erro na operação
	 */
    io_error:
	if ((scbp->scb_flags & B_STAT) == 0)
	{
		printf
		(	"ata[%d,%d]: ERRO de %s: bloco = %d, erro = %b\n",
			ATA_DEVNO,
			(scbp->scb_flags & (B_READ|B_WRITE)) == B_READ ? "leitura" : "escrita",
			scbp->scb_blkno, adp->ad_error, err_bits
		);
	}

	if (--bp->b_retry_cnt > 0)
	{
		if (bp->b_retry_cnt == (MAX_RETRY >> 1))
		{
			printf ("ata[%d,%d]: Passando para o modo bloco a bloco\n", ATA_DEVNO);

			scbp->scb_max_pio_sz = adp->ad_blsz;
		}

		scbp->scb_step = -1;		/* Repete a operação desde o início */

		atastart (chp, scbp); return;
	}

	bp->b_error	 = EIO;			/* Erro irrecuperável */
	scbp->scb_flags |= B_ERROR;

	/*
	 *	A operação chegou ao fim
	 */
    done:
	dp = &adp->ad_queue; 

	SPINLOCK (&dp->v_lock);

	bp = remdsort (dp); ata_busy--;

	SPINFREE (&dp->v_lock);

	bp->b_flags    = scbp->scb_flags;
	bp->b_residual = scbp->scb_count;

	EVENTDONE (&bp->b_done);

	chp->ch_active_dev = NOATADEVICE; 	/* Operação finalizada */

	atastart (chp, scbp);			/* Próxima operação */

}	/* end ataint */

/*
 ****************************************************************
 *	Leitura em modo "raw"					*
 ****************************************************************
 */
int
ataread (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, atastrategy, &ratabuf, B_READ, 0 /* dma */);

	return (UNDEF);

}	/* end ataread */

/*
 ****************************************************************
 *	Escrita em modo "raw"					*
 ****************************************************************
 */
int
atawrite (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, atastrategy, &ratabuf, B_WRITE, 0 /* dma */);

	return (UNDEF);

}	/* end atawrite */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
atactl (dev_t dev, int cmd, int arg, int flag)
{
	DISKTB			*pp;
	ATA_CHANNEL		*chp;
	ATA_DEVICE		*adp;

	pp = &disktb[MINOR (dev)];

	if
	(	(unsigned)pp->p_unit >= ata_next_channel || (unsigned)pp->p_target >= NATAT ||
		(chp = ata_channel[pp->p_unit]) == NOATACONTROLLER ||
		(adp = chp->ch_dev[pp->p_target]) == NOATADEVICE
	)
		{ u.u_error = ENXIO; return (-1); }

	if (adp->ad_flags & ATAPI_DEV)
		return (scsi_ctl (adp->ad_scsi, dev, cmd, arg));

	if (cmd == DKISADISK)
		return (0);

	u.u_error = EINVAL; return (-1);

}	/* end atactl */

#ifdef	TRUE_SCBS
/*
 ****************************************************************
 *	Inicializa a lista de SCBs				*
 ****************************************************************
 */
void
ata_init_scb_list (void)
{
	int			channel;
	ATA_CHANNEL		*chp;
	ATA_SCB			*scbp;

	/*
	 *	Por enquanto, apenas um SCB por canal
	 */
	for (channel = 0; channel < NATAC; channel++)
	{
		if ((chp = ata_channel[channel]) == NOATACONTROLLER)
			continue;

		if ((scbp = malloc_byte (sizeof (ATA_SCB))) == NOATASCB)
			panic ("ata_init_scb_list: Não consegui alocar os SCBs");

		chp->ch_scb = scbp;

		memclr (scbp, sizeof (ATA_SCB));
	}

}	/* end ata_init_scb_list */

/*
 ****************************************************************
 *	Obtém um SCB						*
 ****************************************************************
 */
ATA_SCB *
ata_get_scb (ATA_CHANNEL *chp)
{
	ATA_SCB		*scbp;

	/*
	 *	Recebe e devolve "chp->ch_busy" travado
	 */
	if ((scbp = chp->ch_scb) != NOATASCB)
		chp->ch_scb = scbp->scb_next;
	else
		panic ("ata_get_scb: lista de SCBs vazia");

	return (scbp);

}	/* end ata_get_scb */

/*
 ****************************************************************
 *	Devolve um SCB						*
 ****************************************************************
 */
void
ata_put_scb (ATA_SCB *scbp, ATA_CHANNEL *chp)
{
	/*
	 *	Recebe e devolve "chp->ch_busy" travado
	 */
	scbp->scb_next  = chp->ch_scb;
	chp->ch_scb = scbp;

}	/* end ata_put_scb */
#endif	TRUE_SCBS

#if (0)	/********************************************************/
/*
 ****************************************************************
 *	Atualiza a Tabela de Partições para um Dispositivo	*
 ****************************************************************
 */
void
ata_update_partition_table (ATA_CHANNEL *chp, int dev)
{
	ATA_DEVICE	*adp = chp->ch_dev[dev];
	DISKTB		*pp, *base_pp;
	char		dev_nm[16];

	/*
	 *	Prepara o essencial da primeira entrada (para todo o dispositivo)
	 */
	if ((pp = next_disktb) + 2 >= end_disktb)
		{ printf ("%g: DISKTB cheia\n"); return; }

	strcpy (pp->p_name, adp->ad_dev_nm);

	pp->p_offset	= 0;
   	pp->p_size	= MAX (adp->ad_disksz, BL4SZ / BLSZ);

   /***	pp->p_type	= 0;	***/
   /***	pp->p_flags	= 0;	***/
   /***	pp->p_lock	= 0;	***/
   /***	pp->p_blshift	= ...;	***/	/* Completado por "scsi_open" */

	pp->p_dev	= MAKEDEV (IDE_MAJOR, pp - disktb);
   	pp->p_unit	= 2;
	pp->p_target	= chp->ch_index * NATAT + dev;

   /***	pp->p_head	= ....;	***/
   /***	pp->p_sect	= ....;	***/
   /***	pp->p_cyl	= ....;	***/

   /***	pp->p_nopen	= 0;	   ***/
   /***	pp->p_sb	= NOSB;	   ***/
   /***	pp->p_inode	= NOINODE; ***/

	/*
	 *	Lê a tabela de partições, completando as entradas na DISKTB
	 */
	if ((base_pp = disktb_create_partitions (pp)) == NODISKTB)
		{ printf ("%s: erro na leitura da tabela de partições\n", adp->ad_dev_nm); return; }

	/*
	 *	Atualiza o "/dev"
	 */
	if (rootdir == NOINODE)
		return;		/* Ainda não pode ler INODEs */

	strcpy (dev_nm, "/dev/");

	for (pp = base_pp; memeq (pp->p_name, adp->ad_dev_nm, 3); pp++)
	{	
		strcpy (dev_nm + 5, pp->p_name);
		kmkdev (dev_nm, IFBLK|0740, pp->p_dev);

		dev_nm[5] = 'r'; strcpy (dev_nm + 6, pp->p_name);
		kmkdev (dev_nm, IFCHR|0640, pp->p_dev);
	}

}	/* end ata_update_partition_table */
#endif	/********************************************************/
