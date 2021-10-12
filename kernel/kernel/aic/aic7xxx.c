/*
 ****************************************************************
 *								*
 *			aic7xxx.c				*
 *								*
 *	Função de acesso ao controlador				*
 *								*
 *	Versão	4.0.0, de 04.04.01				*
 *		4.0.0, de 28.12.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

#include <stddef.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/pci.h"
#include "../h/frame.h"
#include "../h/disktb.h"
#include "../h/scsi.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/devhead.h"
#include "../h/bhead.h"
#include "../h/cdio.h"

#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

#include "aic7xxx_seq.h"
#include "aic7xxx.h"

/*
 ******	Mensagens de erro de paridade para casa fase ************
 */
struct phase_table_entry
{
	uchar		phase;
	uchar		mesg_out;	/* Message response to parity errors */
	const char	*phasemsg;
};

struct phase_table_entry ahc_phase_table[] =
{
	P_DATAOUT,	MSG_NOOP,		"in Data-out phase",
	P_DATAIN,	MSG_INITIATOR_DET_ERR,	"in Data-in phase",
	P_DATAOUT_DT,	MSG_NOOP,		"in DT Data-out phase",
	P_DATAIN_DT,	MSG_INITIATOR_DET_ERR,	"in DT Data-in phase",
	P_COMMAND,	MSG_NOOP,		"in Command phase",
	P_MESGOUT,	MSG_NOOP,		"in Message-out phase",
	P_STATUS,	MSG_INITIATOR_DET_ERR,	"in Status phase",
	P_MESGIN,	MSG_PARITY_ERROR,	"in Message-in phase",
	P_BUSFREE,	MSG_NOOP,		"while idle",
	0,		MSG_NOOP,		"in unknown phase"
};

const uint     ahc_num_phases = NUM_ELEMENTS (ahc_phase_table) - 1;	/* Não inclui a última entrada */

/*
 *******Tabela de velocidades de transferências síncronas *******
 */

/* Mapeia o período de transferência em ns para o valor do registro "scsixfer" */

const struct ahc_syncrate ahc_syncrates[] =
{
    /*	     Ultra2	     fast/ultra		period   rate	double_rate */

	{ 0x02|DT_SXFR,	    0x00,		     9,	"80.0", "160.0"	},	/*  0	DT	*/
	{ 0x03,		    0x00,		    10,	"40.0",  "80.0"	},	/*  1	ULTRA2	*/
	{ 0x04,	   	    0x00,		    11,	"33.0",  "66.0"	},	/*  2		*/
	{ 0x05,		    0x00|ULTRA_SXFR,	    12,	"20.0",  "40.0"	},	/*  3	ULTRA	*/
	{ 0x06,		    0x10|ULTRA_SXFR,        15,	"16.0",  "32.0"	},	/*  4		*/
	{ 0x07,		    0x20|ULTRA_SXFR,        18,	"13.3",  "26.6"	},	/*  5		*/
	{ 0x08,		    0x00,		    25,	"10.0",  "20.0"	},	/*  6	FAST	*/
	{ 0x09|ST_SXFR,	    0x10,		    31,	 "8.0",  "16.0"	},	/*  7		*/
	{ 0x0A|ST_SXFR,	    0x20,		    37,	 "6.67", "13.3"	},	/*  8		*/
	{ 0x0B|ST_SXFR,	    0x30,		    43,	 "5.7",  "11.4"	},	/*  9		*/
	{ 0x0C|ST_SXFR,	    0x40,		    50,	 "5.0",  "10.0"	},	/* 10		*/
	{ 0x00,		    0x50,		    56,	 "4.4",   "8.8"	},	/* 11		*/
	{ 0x00,		    0x60,		    62,	 "4.0",   "8.0"	},	/* 12		*/
	{ 0x00,		    0x70,		    68,	 "3.6",   "7.2"	},	/* 13		*/
	{ 0x00,		    0x00,		     0,	NOSTR,    NOSTR	}

}	/* end ahc_syncrates */;

entry int	speed_verbose = 1;

/*
 ****** Variáveis globais ***************************************
 */
#ifndef	BOOT
entry DEVHEAD	ahc_tab;	/* Cabeca da lista de dps e do major */
#endif	BOOT

extern int	ahc_busy;		/* Para verificar o uso */

entry struct ahc_softc	*ahc_softc_vec[NAHC];	/* Acesso às estruturas pelo # da unidade */

#ifndef	BOOT
/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
ahc_open (dev_t dev, int oflag)
{
	struct ahc_softc	*ahc;
	int			unit, target;
	DISKTB			*up;
	AHCINFO			*sp;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if ((unsigned)(unit = up->p_unit) >= NAHC)
		{ u.u_error = ENXIO; return (-1); }

	if ((unsigned)(target = up->p_target) >= NTARGET)
		{ u.u_error = ENXIO; return (-1); }

	if ((ahc = ahc_softc_vec[unit]) == NULL)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica o "O_LOCK"
	 */
	if (up->p_lock || (oflag & O_LOCK) && up->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	sp = &ahc->ahc_info[target];

	sp->info_scsi.scsi_cmd = ahc_cmd;

	if (scsi_open (&sp->info_scsi, dev, oflag) < 0)
		return (-1);

	/*
	 *	Se for o primeiro "open" em um dispositivo de meio
	 *	removível, trava o meio.
	 */
	if (sp->info_scsi.scsi_nopen == 0 && sp->info_scsi.scsi_removable)
		scsi_ctl (&sp->info_scsi, dev, CD_LOCK_UNLOCK, 1);

	/*
	 *	Sucesso
	 */
	sp->info_scsi.scsi_nopen++;
	up->p_nopen++;

	if (oflag & O_LOCK)
		up->p_lock = 1;

	return (0);

}	/* end ahc_open */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
ahc_close (dev_t dev, int flag)
{
	struct ahc_softc	*ahc;
	DISKTB			*up;
	int			unit, target;
	AHCINFO			*sp;

	/*
	 *	Prólogo
	 */
	up = &disktb[MINOR (dev)];

	unit = up->p_unit; target = up->p_target;

	ahc = ahc_softc_vec[unit];
	sp = &ahc->ahc_info[target];

	/*
	 *	Atualiza os contadores
	 */
	if (--up->p_nopen <= 0)
		up->p_lock = 0;

	if (--sp->info_scsi.scsi_nopen <= 0)
	{
		if (sp->info_scsi.scsi_removable)
			scsi_ctl (&sp->info_scsi, dev, CD_LOCK_UNLOCK, 0);

		scsi_close (&sp->info_scsi, dev);
	}

	return (0);

}	/* end ahc_close */

/*
 ****************************************************************
 *	Executa uma operação interna				*
 ****************************************************************
 */
int
ahc_cmd (BHEAD *bp)
{
	/*
	 *	recebe e devolve o bhead "locked"
	 */
	if ((unsigned)bp->b_cmd_len > 12)			/* PROVISÓRIO */
		{ u.u_error = EINVAL; return (-1); }

	bp->b_flags |= B_STAT;

	ahc_strategy (bp);

	EVENTWAIT (&bp->b_done, PBLKIO);

	if (geterror (bp) < 0)
		return (-1);

	return (0);

}	/* end ahc_cmd */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
ahc_strategy (BHEAD *bp)
{
	struct ahc_softc	*ahc;
	struct ahc_scb		*scbp;
	AHCINFO			*sp;
	DEVHEAD			*dp;
	const DISKTB		*up;
	int			unit, target;
	daddr_t			bn;
	char			*cp;

	/*
	 *	verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_phdev)];

	target = up->p_target; unit = up->p_unit;

	if ((unsigned)target >= NTARGET)
		{ bp->b_error = ENXIO; goto bad; }

	ahc = ahc_softc_vec[unit];
	sp = &ahc->ahc_info[target];

	if ((bn = bp->b_phblkno) < 0 || bn + BYTOBL (bp->b_base_count) > up->p_size)
	{
		if ((bp->b_flags & B_STAT) == 0)
			{ bp->b_error = ENXIO; goto bad; }
	}

	bp->b_dev_nm = up->p_name;

	/*
 	 *	Prepara o comando SCSI de leitura/escrita
	 */
#if (0)	/*******************************************************/
	splahc (); 
#endif	/*******************************************************/
#ifndef	BOOT
	splx (irq_pl_vec[ahc->irq]);
#endif	BOOT

	if ((bp->b_flags & B_STAT) == 0)
	{
		bp->b_cmd_len = 10;

		cp = bp->b_cmd_txt;

		if ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)	/* Write */
		{
			/* A cada AHC_INC_OP escritas, experimenta incrementar MAX_TAG */

			if (sp->info_scsi.scsi_tagenable && --sp->info_inc_op < 0)
			{
				sp->info_max_tag++;
				sp->info_inc_op = AHC_INC_OP;
#undef	TAG_DEBUG
#ifdef	TAG_DEBUG
				if (CSWT (37)) printf
				(	"%s: Incrementei \"max_tag\" para %d\n",
					sp->info_scsi.scsi_dev_nm, sp->info_max_tag
				);
#endif	TAG_DEBUG
			}

			cp[0] = SCSI_CMD_WRITE_BIG;
		}
		else							/* Read */
		{
			cp[0] = SCSI_CMD_READ_BIG;
		}

		cp[1] = 0;
#if (0)	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv (bp->b_phblkno + up->p_offset);
		*(long *)&cp[2] = long_endian_cv ((bp->b_phblkno + up->p_offset) / (sp->info_scsi.scsi_blsz / BLSZ));
#endif	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv
				((bp->b_phblkno + up->p_offset) >> (sp->info_scsi.scsi_blshift - BLSHIFT));
		cp[6] = 0;
#if (0)	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count / sp->info_scsi.scsi_blsz);
#endif	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> sp->info_scsi.scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	coloca o pedido na fila e inicia a operação
	 */
	bp->b_cylin = (unsigned)(bn + up->p_offset) >> 10;
	bp->b_retry_cnt = NUM_TRY;

	dp = &sp->info_utab;

	ahc_busy++;

	/*
	 *	Põe o pedido na fila
	 */
	if (sp->info_scsi.scsi_tagenable)
	{
		SPINLOCK (&dp->v_lock);

		if (dp->v_first == NOBHEAD)
			dp->v_first = bp;
		else
			dp->v_last->b_forw = bp;

		dp->v_last = bp;
	
		bp->b_forw = NOBHEAD;
	   /***	bp->b_back = NOBHEAD; ***/

		SPINFREE (&dp->v_lock);
	}
	else
	{
		insdsort (dp, bp);

		if (TAS (&sp->info_busy) < 0)
			{ spl0 (); return (0); }
	}

	/*
	 *	Inicia a operação
	 */
	if   (sp->info_scsi.scsi_tagenable)
	{
		if (sp->info_cur_op >= sp->info_max_tag)
		{
#ifdef	TAG_DEBUG
			if (CSWT (37)) printf
			(	"%s: Evitei o entupimento da fila em \"strategy\" (%d, %d)\n",
				ahc->name, sp->info_cur_op, sp->info_max_tag
			);
#endif	TAG_DEBUG
			spl0 (); return (0);
		}

		if (ahc->ahc_busy_scb >= AHC_SCB_MAX - NTARGET)
		{
#ifdef	TAG_DEBUG
			if (CSWT (37))
				printf ("%s: Poucos SCBs em \"strategy\"\n", ahc->name);
#endif	TAG_DEBUG
			spl0 (); return (0);
		}
	}

	if ((scbp = ahc_get_scb (ahc)) != NULL)
	{
		ahc_start (ahc, target, scbp);
	}
	else
	{
		printf ("%s: Não consegui um SCB em \"strategy\"\n", ahc->name);
	}

	spl0 (); return (0);

	/*
	 *	Houve algum erro
	 */
    bad:
	bp->b_flags |= B_ERROR;
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end ahc_strategy */

/*
 ****************************************************************
 *	Inicia uma operação em um controlador			*
 ****************************************************************
 */
void
ahc_start (struct ahc_softc *ahc, int target, struct ahc_scb *scbp)
{
	AHCINFO			*sp;
	BHEAD			*bp;
	DEVHEAD			*dp;
	struct hardware_scb	*hscb;	
	struct ahc_dma_seg	*seg;
	int			i;
	ushort			mask;

	/*
	 *	Procura o novo pedido
	 */
	sp = &ahc->ahc_info[target];

	if   ((bp = scbp->bp) != NOBHEAD)	/* Caso de erro */
	{
#ifdef	TAG_DEBUG
		if (CSWT (37))
			printf ("%s: \"start\" chamada sem \"bp\"\n", ahc->name);
#endif	TAG_DEBUG
	}
	elif (sp->info_scsi.scsi_tagenable)
	{
		if (ahc->ahc_busy_scb < AHC_SCB_MAX - NTARGET)
		{
			dp = &sp->info_utab;

			SPINLOCK (&dp->v_lock);

			if ((bp = dp->v_first) == NOBHEAD)
			{
				SPINFREE (&dp->v_lock);
				ahc_put_scb (ahc, scbp);
				return;
			}
		}
		else
		{
#ifdef	TAG_DEBUG
			if (CSWT (37))
				printf ("%s: Poucos SCBs em \"start\"\n", ahc->name);
#endif	TAG_DEBUG
			for (sp++, i = NTARGET; /* abaixo */; sp++, target++, i--)
			{
				if (i <= 0)
				{
					ahc_put_scb (ahc, scbp);
#ifdef	TAG_DEBUG
					if (CSWT (37)) printf
					(	"%s: Não há operação \"tag\" para iniciar em \"start\"\n",
						ahc->name
					);
#endif	TAG_DEBUG
					return;
				}

				if (target >= NTARGET)
				{
					target = 0; sp -= NTARGET;
				}

				dp = &sp->info_utab;

				SPINLOCK (&dp->v_lock);

				if ((bp = dp->v_first) == NOBHEAD)
				{
					SPINFREE (&dp->v_lock);
					continue;
				}

				break;
			}
		}

	   /***	dp = &sp->info_utab; ***/

		if ((dp->v_first = bp->b_forw) == NOBHEAD)
			dp->v_last = NOBHEAD;

		SPINFREE (&dp->v_lock);
	}
	else	/* Sem TAGs */
	{
		dp = &sp->info_utab;

		SPINLOCK (&dp->v_lock);

		if ((bp = dp->v_first) == NOBHEAD)
		{
			SPINFREE (&dp->v_lock);
			CLEAR (&sp->info_busy);
			ahc_put_scb (ahc, scbp);
			return;
		}

		bp = remdsort (&sp->info_utab);

		SPINFREE (&dp->v_lock);
	}

	/*
	 *	Põe o comando no SCB
	 */
	hscb = ahc->next_queued_scb->hscb;	/* Usa o HSCB que o sequenciador está esperando */

	scbp->bp = bp;

	memmove (hscb->shared_data.cdb, bp->b_cmd_txt, bp->b_cmd_len);
	hscb->cdb_len = bp->b_cmd_len;

	/*
	 *	Prepara a tabela de DMA
	 */
	if (bp->b_base_count)
	{
		seg = &scbp->sg_list[0];

		hscb->dataptr	= seg->addr = VIRT_TO_PHYS_ADDR (bp->b_base_addr);
#if (0)	/*******************************************************/
		hscb->dataptr	= seg->addr = (ulong)bp->b_phaddr;
#endif	/*******************************************************/
		hscb->datacnt	= seg->len  = bp->b_base_count | AHC_DMA_LAST_SEG;

		hscb->sgptr	= VIRT_TO_PHYS_ADDR (seg + 1) | SG_FULL_RESID;

		scbp->sg_count  = 1;
	}
	else
	{
		hscb->dataptr	= 0;
		hscb->datacnt	= 0;

		hscb->sgptr	= SG_LIST_NULL;

		scbp->sg_count  = 0;
	}

	/*
	 *	Prepara os campos do HSCB
	 */
	if (sp->info_scsi.scsi_tagenable)
		hscb->control	= TAG_ENB;
	else
		hscb->control	= 0;

	hscb->scsiid	= (target << 4) | ahc->our_id;
	hscb->lun	= 0;

	mask = 1 << target;

	hscb->scsirate	 = sp->transinfo.scsirate;
	hscb->scsioffset = sp->transinfo.current.offset;

	if ((ahc->ultraenb & mask) != 0)
		hscb->control |= ULTRAENB;

	if ((ahc->discenable & mask) != 0)
		hscb->control |= DISCENB;

	/*
	 *	Põe o SCB na fila do controlador
	 */
	sp->info_cur_op++;

	scbp->flags |= SCB_ACTIVE;

   /***	hscb->tag  = ...; ***/
	hscb->next = scbp->hscb->tag;

	/* Now swap HSCB pointers. */

	ahc->next_queued_scb->hscb = scbp->hscb;
	scbp->hscb = hscb;

	/* Now define the mapping from tag to SCB in the scbindex */

	ahc->scbindex[hscb->tag] = scbp;

	if (hscb->tag == SCB_LIST_NULL || hscb->next == SCB_LIST_NULL)
		printf ("%s: ERRO FATAL: Tentando enviar SCB inválido: %d, %d\n", ahc->name, hscb->tag, hscb->next);

	/*
	 *	Keep a history of SCBs we've downloaded in the qinfifo
	 */
	ahc->qinfifo[ahc->qinfifonext++] = hscb->tag;

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"%s: control = %02X, scsiid = %02X, tag = %d, rate = %02X, offset = %02X, next = %d\n",
		ahc->name, q_hscb->control, q_hscb->scsiid, q_hscb->tag,
		q_hscb->scsirate, q_hscb->scsioffset, q_hscb->next
	); 
#endif	DEBUG

	/*
	 *	Informa ao controlador que a fila de entrada avançou
	 */
	if ((ahc->features & AHC_QUEUE_REGS) != 0)
	{
		ahc_outb (ahc, HNSCB_QOFF, ahc->qinfifonext);
	}
	else
	{
		if ((ahc->features & AHC_AUTOPAUSE) == 0)
			pause_sequencer (ahc);

		ahc_outb (ahc, KERNEL_QINPOS, ahc->qinfifonext);

		if ((ahc->features & AHC_AUTOPAUSE) == 0)
			unpause_sequencer (ahc);
	}

}	/* end ahc_start */

/*
 ****************************************************************
 *	Leitura de modo "raw"					*
 ****************************************************************
 */
int
ahc_read (IOREQ *iop)
{
	struct ahc_softc	*ahc;

	ahc = ahc_softc_vec[disktb[MINOR (iop->io_dev)].p_unit];

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, ahc_strategy, &ahc->ahc_raw_buf, B_READ, 0 /* dma 32 bits */);

	return (UNDEF);

}	/* end ahc_read */

/*
 ****************************************************************
 *	Escrita de modo "raw"					*
 ****************************************************************
 */
int
ahc_write (IOREQ *iop)
{
	struct ahc_softc	*ahc;

	ahc = ahc_softc_vec[disktb[MINOR (iop->io_dev)].p_unit];

	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, ahc_strategy, &ahc->ahc_raw_buf, B_WRITE, 0 /* dma 32 bits */);

	return (UNDEF);

}	/* end ahc_write */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
ahc_ctl (dev_t dev, int cmd, int arg, int flag)
{
	struct ahc_softc	*ahc;
	AHCINFO			*sp;
	const DISKTB		*up;
	
	up = &disktb[MINOR (dev)];

	ahc = ahc_softc_vec[up->p_unit];
	sp = &ahc->ahc_info[up->p_target];

	return (scsi_ctl (&sp->info_scsi, dev, cmd, arg));

}	/* end ahc_ctl */
#endif	BOOT

/*
 ****************************************************************
 *	Restart the sequencer program from address zero		*
 ****************************************************************
 */
void
restart_sequencer (struct ahc_softc *ahc)
{
	ushort        stack[4];

	pause_sequencer (ahc);

	ahc_outb (ahc, SCSISIGO, 0);	/* De-assert BSY */
	ahc_outb (ahc, MSG_OUT, MSG_NOOP);	/* No message to send */
	ahc_outb (ahc, SXFRCTL1, ahc_inb (ahc, SXFRCTL1) & ~BITBUCKET);

	/*
	 * Ensure that the sequencer's idea of TQINPOS matches our own.  The
	 * sequencer increments TQINPOS only after it sees a DMA complete and
	 * a reset could occur before the increment leaving the kernel to
	 * believe the command arrived but the sequencer to not.
	 */
	ahc_outb (ahc, TQINPOS, ahc->tqinfifonext);

	/* Always allow reselection */

	ahc_outb (ahc, SCSISEQ, ahc_inb (ahc, SCSISEQ_TEMPLATE) & (ENSELI | ENRSELI | ENAUTOATNP));

	if ((ahc->features & AHC_CMD_CHAN) != 0) /* Ensure that no DMA operations are in progress */
	{
		ahc_outb (ahc, CCSGCTL, 0);
		ahc_outb (ahc, CCSCBCTL, 0);
	}

	ahc_outb (ahc, MWI_RESIDUAL, 0);

	/*
	 * Avoid stack pointer lockup on aic7895 chips where SEQADDR0 cannot
	 * be changed without first writing to SEQADDR1.  This seems to only
	 * happen if an interrupt or pause occurs mid update of the stack
	 * pointer (i.e. during a ret).
	 */
	stack[0] = ahc_inb (ahc, STACK) | (ahc_inb (ahc, STACK) << 8);
	stack[1] = ahc_inb (ahc, STACK) | (ahc_inb (ahc, STACK) << 8);
	stack[2] = ahc_inb (ahc, STACK) | (ahc_inb (ahc, STACK) << 8);
	stack[3] = ahc_inb (ahc, STACK) | (ahc_inb (ahc, STACK) << 8);

	if (stack[0] == stack[1] && stack[1] == stack[2] && stack[2] == stack[3] && stack[0] != 0)
		ahc_outb (ahc, SEQADDR1, 0);

	ahc_outb (ahc, SEQCTL, FASTMODE);
	ahc_outb (ahc, SEQADDR0, 0);
	ahc_outb (ahc, SEQADDR1, 0);

	unpause_sequencer (ahc);

}	/* end restart_sequencer */

/*
 ****************************************************************
 *	Examina a fila de saída					*
 ****************************************************************
 */
void
ahc_run_qoutfifo (struct ahc_softc *ahc)
{
	struct ahc_scb	*scbp;
	uint		scb_tag;

	while (ahc->qoutfifo[ahc->qoutfifonext] != SCB_LIST_NULL)
	{
		scb_tag = ahc->qoutfifo[ahc->qoutfifonext];

		if ((ahc->qoutfifonext & 0x03) == 0x03)
		{
			uint           modnext;

			/*
			 *	Clear 32bits of QOUTFIFO at a time so that we
			 *	don't clobber an incomming byte DMA to the array
			 *	on architectures that only support 32bit load and
			 *	store operations.
			 */
			modnext = ahc->qoutfifonext & ~0x3;
			*((ulong *) (&ahc->qoutfifo[modnext])) = 0xFFFFFFFF;
		}

		ahc->qoutfifonext++;

		if ((scbp = ahc->scbindex[scb_tag]) == NULL)
		{
			printf
			(	"%s: ADVERTÊNCIA: comando completado sem o SCB %d\n",
				ahc->name, scb_tag
			);

			continue;
		}

		ahc_done (ahc, scbp);
	}

}	/* end ahc_run_qoutfifo */

#ifndef	BOOT
/*
 ****************************************************************
 *	Tabela de mensagens de erros do sequenciador		*
 ****************************************************************
 */
struct hard_error_entry
{
	uchar		errno;
	const char	*errmesg;
};

struct hard_error_entry	ahc_hard_error[] =
{
	ILLHADDR,	"Illegal Host Access",				/* 0x01 */
	ILLSADDR,	"Illegal Sequencer Address referrenced",	/* 0x02 */
	ILLOPCODE,	"Illegal Opcode in sequencer program",		/* 0x04 */
	SQPARERR,	"Sequencer Parity Error",			/* 0x08 */
	DPARERR,	"Data-path Parity Error",			/* 0x10 */
	MPARERR,	"Scratch or SCB Memory Parity Error",		/* 0x20 */
	PCIERRSTAT,	"PCI Error detected",				/* 0x40 */
	CIOPARERR,	"CIOBUS Parity Error",				/* 0x80 */
};

const uint     ahc_num_hard_errors = NUM_ELEMENTS (ahc_hard_error);

/*
 ****************************************************************
 *	Erro no programa do sequenciador			*
 ****************************************************************
 */
void
ahc_handle_brkadrint (struct ahc_softc *ahc)
{
	int             i, error;

	/*
	 *	Imprime a mensagem de erro
	 */
	error = ahc_inb (ahc, ERROR);

	for (i = 0; error != 1 && i < ahc_num_hard_errors; i++)
		error >>= 1;

	printf
	(	"%s: Erro \"%s\" no programa do sequenciador no endereço 0x%04X",
		ahc->name, ahc_hard_error[i].errmesg,
		ahc_inb (ahc, SEQADDR0) | (ahc_inb (ahc, SEQADDR1) << 8)
	);

}	/* end ahc_handle_brkadrint */
#endif	BOOT

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_handle_seqint (struct ahc_softc *ahc, uint intstat)
{
	struct ahc_scb		*scbp;
	int			target;

	target = ahc_inb (ahc, SAVED_SCSIID) >> 4;

	/*
	 *	Clear the upper byte that holds SEQINT status codes and clear the
	 *	SEQINT bit. We will unpause the sequencer, if appropriate, after
	 *	servicing the request.
	 */
	ahc_outb (ahc, CLRINT, CLRSEQINT);

	switch (intstat & SEQINT_MASK)
	{
	    case BAD_STATUS:
	    {
		uint			scb_tag;
		struct hardware_scb	*hscb;

		/*
		 *	Set the default return value to 0 (don't send sense).
		 *	The sense code will change this if needed.
		 */
		ahc_outb (ahc, RETURN_1, 0);

		/*
		 *	The sequencer will notify us when a command has an error that would be of interest
		 *	to the kernel. This allows us to leave the sequencer running in the common case of
		 *	command completes without error.  The sequencer will already have dma'd the SCB
		 *	back up to us, so we can reference the in kernel copy directly.
		 */
		scb_tag = ahc_inb (ahc, SCB_TAG);

		if ((scbp = ahc->scbindex[scb_tag]) == NULL)
		{
			printf
			(	"%s (%d): ERRO FATAL: SCB %d referenciado não válido na interrupção (0x%02X)\n",
				ahc->name, target, scb_tag, intstat
			);

			break;
		}

		hscb = scbp->hscb;

		/* Don't want to clobber the original sense code */

		if ((scbp->flags & SCB_SENSE) != 0)
		{
			/*
			 *	Clear the SCB_SENSE Flag and have the
			 *	sequencer do a normal command complete.
			 */
			scbp->flags &= ~SCB_SENSE;
#if (0)	/*******************************************************/
			ahc_set_transaction_status (scbp, CAM_AUTOSENSE_FAIL);
#endif	/*******************************************************/
			break;
		}

#if (0)	/*******************************************************/
		ahc_set_transaction_status (scbp, CAM_SCSI_STATUS_ERROR);
#endif	/*******************************************************/

		/* Freeze the queue until the client sees the error. */

		switch (hscb->shared_data.status.scsi_status)
		{
		    case SCSI_STATUS_OK:
		    {
			printf ("%s: Interrupção com estado OK (?)\n", ahc->name);
			break;
		    }

		    case SCSI_STATUS_CMD_TERMINATED:
		    case SCSI_STATUS_CHECK_COND:
		    {
			struct ahc_dma_seg	*seg;
			uchar			*cp;

			/*
			 *	Salva o residual
			 */
			scbp->bp->b_residual = ahc_calc_residual (ahc, scbp);

			/*
			 *	Prepara o comando de SENSE
			 */
		   	cp = &hscb->shared_data.cdb[0];

			cp[0] = SCSI_CMD_REQUEST_SENSE;
			cp[1] = 0;	/* No futuro (lun << 5) */
			cp[2] = 0;
			cp[3] = 0;
			cp[4] = sizeof (struct scsi_sense_data);
			cp[5] = 0;	/* Control */

			hscb->cdb_len = 6;

			/*
			 *	Prepara a tabela de DMA
			 */
			seg = &scbp->sg_list[0];

#ifndef	BOOT
			hscb->dataptr	= seg->addr = VIRT_TO_PHYS_ADDR (&scbp->sense_area);
			hscb->datacnt	= seg->len  = sizeof (struct scsi_sense_data) | AHC_DMA_LAST_SEG;

			hscb->sgptr	= VIRT_TO_PHYS_ADDR (seg + 1) | SG_FULL_RESID;
#else
			hscb->dataptr	= seg->addr = (unsigned)&scbp->sense_area;
			hscb->datacnt	= seg->len  = sizeof (struct scsi_sense_data) | AHC_DMA_LAST_SEG;

			hscb->sgptr	= (unsigned)(seg + 1) | SG_FULL_RESID;
#endif	BOOT

			scbp->sg_count	= 1;

			/*
			 * Would be nice to preserve DISCENB
			 * here, but due to the way we manage
			 * busy targets, we can't.
			 */
			hscb->control = 0;

			/*
			 * This request sense could be
			 * because the the device lost power
			 * or in some other way has lost our
			 * transfer negotiations. Renegotiate
			 * if appropriate.  Unit attention
			 * errors will be reported before any
			 * data phases occur.
			 */
#if (0)	/*******************************************************/
			if (ahc_get_residual (scbp) == ahc_get_transfer_length (scbp))
			if (scbp->io_ctx->csio.resid == scbp->io_ctx->csio.dxfer_len)
			{
			    ahc_update_target_msg_request
				(ahc, &devinfo, targ_info, /* force */ TRUE, /* paused */ TRUE);
			}
#endif	/*******************************************************/

			scbp->flags |= SCB_SENSE;
			ahc_qinfifo_requeue_tail (ahc, scbp);
			ahc_outb (ahc, RETURN_1, SEND_SENSE);
#ifdef __FreeBSD__
			/*
			 * Ensure we have enough time to
			 * actually retrieve the sense.
			 */
			untimeout (ahc_timeout, (caddr_t) scbp, scbp->io_ctx->ccb_h.timeout_ch);
			scbp->io_ctx->ccb_h.timeout_ch =
				timeout (ahc_timeout, (caddr_t) scbp, 5 * hz);
#endif

			break;
		    }

		    case SCSI_STATUS_QUEUE_FULL:
		    {
			scbp->flags |= SCB_QUEUE_FULL;
			break;
		    }

		    default:
		    {
			printf
			(	"%s: Interrupção com código 0x%02X não previsto\n",
				ahc->name, hscb->shared_data.status.scsi_status
			);
			break;
		    }

		}	/* end switch (scsi_status) */

		break;
	    }

	    case NO_MATCH:
	    {
		/* Ensure we don't leave the selection hardware on */

		ahc_outb (ahc, SCSISEQ, ahc_inb (ahc, SCSISEQ) & (ENSELI | ENRSELI | ENAUTOATNP));

		printf
		(	"%s (%d): Não há um SCB ativo para um alvo reconectando\n",
			ahc->name, target
		);

		ahc->msgout_buf[0] = MSG_BUS_DEV_RESET;
		ahc->msgout_len = 1;
		ahc->msgout_index = 0;
		ahc->msg_type = MSG_TYPE_INITIATOR_MSGOUT;

		ahc_outb (ahc, MSG_OUT, HOST_MSG);
		ahc_outb (ahc, SCSISIGO, ahc_inb (ahc, LASTPHASE) | ATNO);

		break;
	    }

	    case SEND_REJECT:
	    {
		printf
		(	"%s (%d): Advertência: recebida mensagem 0x%02X desconhecida - rejeitando\n",
			ahc->name, target, ahc_inb (ahc, ACCUM)
		);

		break;
	    }

	    case NO_IDENT:
	    {
		/*
		 *	The reconnecting target either did not send an identify message, or did,
		 *	but we didn't find an SCB to match and before it could respond to our
		 *	ATN/abort, it hit a dataphase. The only safe thing to do is to blow it
		 *	away with a bus reset.
		 */
		printf
		(	"%s (%d): ERRO FATAL: O alvo não enviou um mensagem de IDENTIFICAÇÃO\n",
			ahc->name, target
		);

		/* Iria reiniciar o canal */

		break;
	    }

	    case IGN_WIDE_RES:
		printf
		(	"%s: Recebi uma mensagem para ignorar o resíduo largo letal\n",
			ahc_get_dev_nm (ahc, target)
		);
		break;

	    case BAD_PHASE:
	    {
		if (ahc_inb (ahc, LASTPHASE) == P_BUSFREE)
		{
			printf
			(	"%s (%d): Liberação do barramento perdida. Fase corrente = 0x%02X\n",
				ahc->name, target, ahc_inb (ahc, SCSISIGI)
			);

			restart_sequencer (ahc);
			return;
		}
		else
		{
			printf
			(	"%s (%d): Fase SCSI 0x%02X desconhecida. Tentando continuar\n",
				ahc->name, target, ahc_inb (ahc, SCSISIGI)
			);
		}

		break;
	    }

	    case HOST_MSG_LOOP:
	    {
		/*
		 *	The sequencer has encountered a message phase that requires host assistance
		 *	for completion. While handling the message phase(s), we will be notified
		 *	by the sequencer after each byte is transfered so we can track bus phase changes.
		 * 
		 *	If this is the first time we've seen a HOST_MSG_LOOP interrupt, initialize
		 *	the state of the host message loop.
		 */
		if (ahc->msg_type == MSG_TYPE_NONE)
		{
			uint           bus_phase, scb_tag;

			bus_phase = ahc_inb (ahc, SCSISIGI) & PHASE_MASK;

			if (bus_phase != P_MESGIN && bus_phase != P_MESGOUT)
			{
				printf
				(	"%s: Fase 0x%02X inválida do HOST_MSG_LOOP\n",
					ahc_get_dev_nm (ahc, target), bus_phase
				);

				/*
				 *	Probably transitioned to bus free before we got here.
				 *	Just punt the message.
				 *	Clear any interrupt conditions this may have caused
				 */
				ahc_outb (ahc, CLRSINT0, CLRSELDO | CLRSELDI | CLRSELINGO);

				ahc_outb (ahc, CLRSINT1, CLRSELTIMEO | CLRATNO | CLRSCSIRSTI
				  | CLRBUSFREE | CLRSCSIPERR | CLRPHASECHG | CLRREQINIT);

				ahc_outb (ahc, CLRINT, CLRSCSIINT);

				restart_sequencer (ahc);
				return;
			}

			/* Fase válida */

			scb_tag = ahc_inb (ahc, SCB_TAG);

			if ((scbp = ahc->scbindex[scb_tag]) == NULL)
				printf ("%s: ERRO FATAL: Leitura de mensagem com SCB %d inválido\n", ahc->name, scb_tag);

			if (bus_phase == P_MESGOUT)
			{
				ahc_setup_initiator_msgout (ahc, target, scbp);
			}
			else
			{
				ahc->msg_type = MSG_TYPE_INITIATOR_MSGIN;
				ahc->msgin_index = 0;
			}
		}

		ahc_handle_message_phase (ahc);
		break;
	    }

	    case PERR_DETECTED:
	    {
		/*
		 * If we've cleared the parity error interrupt but
		 * the sequencer still believes that SCSIPERR is
		 * true, it must be that the parity error is for the
		 * currently presented byte on the bus, and we are
		 * not in a phase (data-in) where we will eventually
		 * ack this byte.  Ack the byte and throw it away in
		 * the hope that the target will take us to message
		 * out to deliver the appropriate error message.
		 */
		if ((intstat & SCSIINT) == 0 && (ahc_inb (ahc, SSTAT1) & SCSIPERR) != 0)
		{
			uint           curphase;

			/*
			 * The hardware will only let you ack bytes
			 * if the expected phase in SCSISIGO matches
			 * the current phase.  Make sure this is
			 * currently the case.
			 */
			curphase = ahc_inb (ahc, SCSISIGI) & PHASE_MASK;
			ahc_outb (ahc, LASTPHASE, curphase);
			ahc_outb (ahc, SCSISIGO, curphase);
			ahc_inb (ahc, SCSIDATL);
		}
		break;
	    }

	    case DATA_OVERRUN:
	    {
		/*
		 * When the sequencer detects an overrun, it places
		 * the controller in "BITBUCKET" mode and allows the
		 * target to complete its transfer. Unfortunately,
		 * none of the counters get updated when the
		 * controller is in this mode, so we have no way of
		 * knowing how large the overrun was.
		 */
		uint           scbindex = ahc_inb (ahc, SCB_TAG);
		uint           lastphase = ahc_inb (ahc, LASTPHASE);
		uint           i;

		scbp = ahc->scbindex[scbindex];

		for (i = 0; i < ahc_num_phases; i++)
		{
			if (lastphase == ahc_phase_table[i].phase)
				break;
		}

		printf
		(	"%s: Detetado perda de dados (%s), índice = %d\n",
			ahc_get_dev_nm_by_scb (ahc, scbp), 
			ahc_phase_table[i].phasemsg,
			scbp->hscb->tag
		);

		printf
		(	"%s: %s vista a fase de dados, comprimento = %d, %d segmentos\n",
			ahc_get_dev_nm_by_scb (ahc, scbp), 
			ahc_inb (ahc, SEQ_FLAGS) & DPHASE ? "Foi" : "Não foi",
			scbp->bp->b_base_count, scbp->sg_count
		);

		if (scbp->sg_count > 0)
		{
			for (i = 0; i < scbp->sg_count; i++)
			{
				printf
				(	"sg[%d] - Addr 0x%x : Length %d\n",
					i,
					scbp->sg_list[i].addr,
					scbp->sg_list[i].len & AHC_SG_LEN_MASK
				);
			}
		}

		/*
		 * Set this and it will take effect when the target
		 * does a command complete.
		 */
		scbp->flags |= SCB_DATA_OVERRUN;
#if (0)	/*******************************************************/
		ahc_set_transaction_status (scbp, CAM_DATA_RUN_ERR);
#endif	/*******************************************************/
		break;
	    }

	    case BOGUS_TAG:
	    {
		printf ("%s: ERRO FATAL: De algum modo foi posto na fila o índice 0xFF\n", ahc->name);
		break;
	    }

	    case ABORT_QINSCB:
	    {
		printf ("%s: Abort QINSCB\n", ahc->name);
		DELAY (10000000);
		break;
	    }

	    case NO_FREE_SCB:
	    {
		printf ("%s: ERRO FATAL: Não há SCBs livres nem desconectados\n", ahc->name);
		break;
	    }

	    case SCB_MISMATCH:
	    {
		int           index;

		index = ahc_inb (ahc, SCBPTR);

		printf
		(	"%s: ERRO FATAL: Índice inválido após SCBPTR %d, índice %d, nosso índice %d\n",
			ahc->name, index, ahc_inb (ahc, ARG_1), ahc->hscbs[index].tag
		);

		break;
	    }

	    case SCBPTR_MISMATCH:
	    {
		int           index;

		index = ahc_inb (ahc, SCBPTR);

		printf
		(	"%s: ERRO FATAL: SCBPTR errado. SCBPTR %d, índice %d, nosso índice %d\n",
			ahc->name, index, ahc_inb (ahc, ARG_1), ahc->hscbs[index].tag
		);

		break;
	    }

	    case OUT_OF_RANGE:
	    {
		printf ("%s: ERRO FATAL: Cálculo do BTT fora dos limites\n", ahc->name);

		break;
	    }

	    default:
	    {
		printf
		(	"%s: SEQINT desconhecido, intstat == 0x%02X, scsisigi = 0x%02X\n",
			ahc->name, intstat, ahc_inb (ahc, SCSISIGI)
		);

		break;
	    }

	}	/* end (switch (intstat & SEQINT_MASK)) */

	/*
	 *	The sequencer is paused immediately on a SEQINT, so we should restart it when we're done.
	 */
	unpause_sequencer (ahc);

}	/* end ahc_handle_seqint */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_handle_scsiint (struct ahc_softc *ahc, uint intstat)
{
	uint		scb_tag, status;
	struct ahc_scb	*scbp;

	/* Make sure the sequencer is in a safe location */

	ahc_clear_critical_section (ahc);

	if ((status = ahc_inb (ahc, SSTAT1)) == 0)
	{
		printf ("%s: Interrupção SCSI espúria\n", ahc->name);
		ahc_outb (ahc, CLRINT, CLRSCSIINT);
		unpause_sequencer (ahc);
		return;
	}

	scb_tag = ahc_inb (ahc, SCB_TAG);
	scbp = ahc->scbindex[scb_tag];

	if (scbp != NULL && (ahc_inb (ahc, SEQ_FLAGS) & IDENTIFY_SEEN) == 0)
		scbp = NULL;

	if   ((status & SCSIRSTI) != 0)
	{
		printf ("%s: ERRO FATAL: o barramento foi reiniciado (?)\n", ahc->name);
	}
	elif ((status & SCSIPERR) != 0)
	{
		/*
		 *	Determine the bus phase and queue an appropriate message.
		 *	SCSIPERR is latched true as soon as a parity error occurs.
		 *	If the sequencer acked the transfer that caused the parity
		 *	error and the currently presented transfer on the bus has
		 *	correct parity, SCSIPERR will be cleared by CLRSCSIPERR.
		 *	Use this to determine if we should look at the last phase
		 *	the sequencer recorded, or the current phase presented on
		 *	the bus.
		 */
		uint		mesg_out, curphase, errorphase, lastphase, scsirate, i, sstat2;

		lastphase = ahc_inb (ahc, LASTPHASE);
		curphase = ahc_inb (ahc, SCSISIGI) & PHASE_MASK;
		sstat2 = ahc_inb (ahc, SSTAT2);
		ahc_outb (ahc, CLRSINT1, CLRSCSIPERR);

		/*
		 *	For all phases save DATA, the sequencer won't
		 *	automatically ack a byte that has a parity error in it.
		 *	So the only way that the current phase could be 'data-in'
		 *	is if the parity error is for an already acked byte in the
		 *	data phase.  During synchronous data-in transfers, we may
		 *	actually ack bytes before latching the current phase in
		 *	LASTPHASE, leading to the discrepancy between curphase and
		 *	lastphase.
		 */
		if ((ahc_inb (ahc, SSTAT1) & SCSIPERR) != 0 || curphase == P_DATAIN || curphase == P_DATAIN_DT)
			errorphase = curphase;
		else
			errorphase = lastphase;

		for (i = 0; i < ahc_num_phases; i++)
		{
			if (errorphase == ahc_phase_table[i].phase)
				break;
		}

		mesg_out = ahc_phase_table[i].mesg_out;

		if (scbp != NULL)
			printf ("%s: ", ahc_get_dev_nm_by_scb (ahc, scbp));
		else
			printf ("%s: ", ahc_get_dev_nm (ahc, ahc_inb (ahc, SAVED_SCSIID) >> 4));

		scsirate = ahc_inb (ahc, SCSIRATE);

		printf
		(	"Detetado erro de paridade. SEQADDR = 0x%02X, SCSIRATE = 0x%02X\n",
			ahc_phase_table[i].phasemsg,
			ahc_inb (ahc, SEQADDR0) | (ahc_inb (ahc, SEQADDR1) << 8),
			scsirate
		);

		if ((ahc->features & AHC_DT) != 0)
		{
			if ((sstat2 & CRCVALERR) != 0)
				printf ("\tValor do CRC não confere\n");

			if ((sstat2 & CRCENDERR) != 0)
				printf ("\tNão recebeu pacote de CRC final\n");

			if ((sstat2 & CRCREQERR) != 0)
				printf ("\tPedido de pacote de CRC ilegal\n");

			if ((sstat2 & DUAL_EDGE_ERR) != 0)
			{
				printf
				(	"\tFase de dados %sDT não esperada\n",
					(scsirate & SINGLE_EDGE) ? "" : "não-"
				);
			}
		}

		/*
		 *	We've set the hardware to assert ATN if we   get a parity
		 *	error on "in" phases, so all we  need to do is stuff the
		 *	message buffer with the appropriate message.  "In" phases
		 *	have set mesg_out to something other than MSG_NOP.
		 */
		if (mesg_out != MSG_NOOP)
		{
			if (ahc->msg_type != MSG_TYPE_NONE)
				ahc->send_msg_perror = TRUE;
			else
				ahc_outb (ahc, MSG_OUT, mesg_out);
		}

		ahc_outb (ahc, CLRINT, CLRSCSIINT);
		unpause_sequencer (ahc);
	}
	elif ((status & BUSFREE) != 0 && (ahc_inb (ahc, SIMODE1) & ENBUSFREE) != 0)
	{
		/*
		 *	First look at what phase we were last in. If its message
		 *	out, chances are pretty good that the busfree was in
		 *	response to one of our abort requests.
		 */
		int		lastphase = ahc_inb (ahc, LASTPHASE);
		int		printerror = 1;

		ahc_outb (ahc, SCSISEQ, ahc_inb (ahc, SCSISEQ) & (ENSELI | ENRSELI | ENAUTOATNP));

		if (lastphase == P_MESGOUT)
		{
			uint		message, tag;

			message = ahc->msgout_buf[ahc->msgout_index - 1];
			tag = SCB_LIST_NULL;

			switch (message)
			{
			    case MSG_ABORT_TAG:
				tag = scbp->hscb->tag;
				/* FALLTRHOUGH */

			    case MSG_ABORT:
				printf
				(	"%s: ERRO FATAL: Aborto completado do SCB %d\n",
					ahc_get_dev_nm_by_scb (ahc, scbp), scbp->hscb->tag
				);

				/* Aborta o SCB ... */

				break;

			    case MSG_BUS_DEV_RESET:
			    {
				printf
				(	"%s: ERRO FATAL: recebi uma mensagem para reiniciar dispositivo",
					ahc->name
				);
			    }

			    default:
				break;

			}	/* end switch (message) */
		}

		if (printerror != 0)
		{
			uint		i;

			if (scbp != NULL)
			{
				uint		tag;

				if ((scbp->hscb->control & TAG_ENB) != 0)
					tag = scbp->hscb->tag;
				else
					tag = SCB_LIST_NULL;

				printf ("%s: ERRO FATAL: iria abortar SCBs ", ahc_get_dev_nm_by_scb (ahc, scbp));
			}
			else
			{
				/*
				 *	We had not fully identified this
				 *	connection, so we cannot abort anything.
				 */
				printf ("%s: ", ahc->name);
			}

			for (i = 0; i < ahc_num_phases; i++)
			{
				if (lastphase == ahc_phase_table[i].phase)
					break;
			}

			printf
			(	"Liberação do barramento inesperada %s SEQADDR = 0x%04X\n",
				ahc_phase_table[i].phasemsg,
				ahc_inb (ahc, SEQADDR0) | (ahc_inb (ahc, SEQADDR1) << 8)
			);
		}

		ahc_clear_msg_state (ahc);

		ahc_outb (ahc, SIMODE1, ahc_inb (ahc, SIMODE1) & ~ENBUSFREE);
		ahc_outb (ahc, CLRSINT1, CLRBUSFREE | CLRSCSIPERR);
		ahc_outb (ahc, CLRINT, CLRSCSIINT);

		restart_sequencer (ahc);
	}
	elif ((status & SELTO) != 0)
	{
		uint		scbptr;

		scbptr = ahc_inb (ahc, WAITING_SCBH);
		ahc_outb (ahc, SCBPTR, scbptr);
		scb_tag = ahc_inb (ahc, SCB_TAG);

		if ((scbp = ahc->scbindex[scb_tag]) == NULL)
		{
			printf
			(	"%s: SCB referenciado inválido durante SELTO SCB (%d, %d)\n",
				ahc->name, scbptr, scb_tag
			);
		}

		ahc_search_wainting_selection_list (ahc);

		ahc_outb (ahc, SCSISEQ, 0); 	/* Stop the selection */

		ahc_clear_msg_state (ahc); 	/* No more pending messages */

		/*
		 *	Although the driver does not care about the 'Selection in
		 *	Progress' status bit, the busy LED does.  SELINGO is only
		 *	cleared by a sucessful selection, so we must manually
		 *	clear it to insure the LED turns off just incase no future
		 *	successful selections occur (e.g. no devices on the bus).
		 */
		ahc_outb (ahc, CLRSINT0, CLRSELINGO);

		/* Clear interrupt state */

		ahc_outb (ahc, CLRSINT1, CLRSELTIMEO | CLRBUSFREE | CLRSCSIPERR);
		ahc_outb (ahc, CLRINT, CLRSCSIINT);
		restart_sequencer (ahc);
	}
	else	/* Desconhecido */
	{
		printf ("%s: Interrupção SCSIINT desconhecida = 0x%02X\n", ahc_get_dev_nm_by_scb (ahc, scbp), status);

		ahc_outb (ahc, CLRSINT1, status);
		ahc_outb (ahc, CLRINT, CLRSCSIINT);
		unpause_sequencer (ahc);
	}

}	/* end ahc_handle_scsiint */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Called when we have an active connection to a target on the bus,
 * this function finds the nearest syncrate to the input period limited
 * by the capabilities of the bus connectivity of and sync settings for
 * the target.
 */
const struct ahc_syncrate *
ahc_devlimited_syncrate (struct ahc_softc *ahc, struct ahc_tinfo_trio *tinfo,
		uint *period, uint *ppr_options)
{
	struct ahc_transinfo	*transinfo;
	uint		maxsync;

	if   ((ahc->features & AHC_ULTRA2) != 0)
	{
		if ((ahc_inb (ahc, SBLKCTL) & ENAB40) != 0 && (ahc_inb (ahc, SSTAT2) & EXP_ACTIVE) == 0)
		{
			maxsync = AHC_SYNCRATE_DT;
		}
		else
		{
			maxsync = AHC_SYNCRATE_ULTRA;

			/* Can't do DT on an SE bus */

			*ppr_options &= ~MSG_EXT_PPR_DT_REQ;
		}
	}
	elif ((ahc->features & AHC_ULTRA) != 0)
	{
		maxsync = AHC_SYNCRATE_ULTRA;
	}
	else
	{
		maxsync = AHC_SYNCRATE_FAST;
	}

	/*
	 *	Never allow a value higher than our current goal period otherwise
	 *	we may allow a target initiated negotiation to go above the limit
	 *	as set by the user.  In the case of an initiator initiated sync
	 *	negotiation, we limit based on the user setting.  This allows the
	 *	system to still accept incoming negotiations even if target
	 *	initiated negotiation is not performed.
	 */
	transinfo = &tinfo->goal;

	*ppr_options &= transinfo->ppr_options;

	if (transinfo->period == 0)
	{
		*period = 0;
		*ppr_options = 0;
		return (NULL);
	}

	*period = MAX (*period, transinfo->period);

	return (ahc_find_syncrate (ahc, period, ppr_options, maxsync));

}	/* end ahc_devlimited_syncrate */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Look up the valid period to SCSIRATE conversion in our table.
 * Return the period and offset that should be sent to the target
 * if this was the beginning of an SDTR.
 */
const struct ahc_syncrate *
ahc_find_syncrate (struct ahc_softc *ahc, uint *period, uint *ppr_options, uint maxsync)
{
	const struct ahc_syncrate	*syncrate;

	if ((ahc->features & AHC_DT) == 0)
		*ppr_options &= ~MSG_EXT_PPR_DT_REQ;

	for (syncrate = &ahc_syncrates[maxsync]; syncrate->rate != NULL; syncrate++)
	{
		/*
		 *	The Ultra2 table doesn't go as low as for the Fast/Ultra cards.
		 */
		if ((ahc->features & AHC_ULTRA2) != 0 && (syncrate->sxfr_u2 == 0))
			break;

		/* Skip any DT entries if DT is not available */

		if ((*ppr_options & MSG_EXT_PPR_DT_REQ) == 0 && (syncrate->sxfr_u2 & DT_SXFR) != 0)
			continue;

		if (*period <= syncrate->period)
		{
			/*
			 *	When responding to a target that requests sync,
			 *	the requested rate may fall between two rates that
			 *	we can output, but still be a rate that we can
			 *	receive.  Because of this, we want to respond to
			 *	the target with the same rate that it sent to us
			 *	even if the period we use to send data to it is
			 *	lower.  Only lower the response period if we must.
			 */
			if (syncrate == &ahc_syncrates[maxsync])
				*period = syncrate->period;

			/*
			 *	At some speeds, we only support ST transfers.
			 */
			if ((syncrate->sxfr_u2 & ST_SXFR) != 0)
				*ppr_options &= ~MSG_EXT_PPR_DT_REQ;

			break;
		}

	}	/* end for (syncrate) */

	/*
	 *	Se não achou nenhuma entrada apropriada, usa asíncrono
	 */
	if
	(	*period == 0 || syncrate->rate == NULL ||
		(ahc->features & AHC_ULTRA2) != 0 && syncrate->sxfr_u2 == 0
	)
	{
		*period = 0;
		syncrate = NULL;
		*ppr_options &= ~MSG_EXT_PPR_DT_REQ;
	}

	return (syncrate);

}	/* end ahc_find_syncrate */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Convert from an entry in our syncrate table to the SCSI equivalent
 * sync "period" factor.
 */
uint
ahc_find_period (struct ahc_softc *ahc, uint scsirate, uint maxsync)
{
	const struct ahc_syncrate	*syncrate;

	if ((ahc->features & AHC_ULTRA2) != 0)
		scsirate &= SXFR_ULTRA2;
	else
		scsirate &= SXFR;

	syncrate = &ahc_syncrates[maxsync];

	while (syncrate->rate != NULL)
	{
		if   ((ahc->features & AHC_ULTRA2) != 0)
		{
			if   (syncrate->sxfr_u2 == 0)
				break;
			elif (scsirate == (syncrate->sxfr_u2 & SXFR_ULTRA2))
				return (syncrate->period);
		}
		elif (scsirate == (syncrate->sxfr & SXFR))
		{
			return (syncrate->period);
		}

		syncrate++;
	}

	return (0);		/* async */

}	/* end ahc_find_period */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Truncate the given synchronous offset to a value the
 * current adapter type and syncrate are capable of.
 */
void
ahc_validate_offset (struct ahc_softc *ahc, struct ahc_tinfo_trio *tinfo,
		     const struct ahc_syncrate * syncrate, uint *offset, int wide)
{
	uint           maxoffset;

	/* Limit offset to what we can do */

	if   (syncrate == NULL)
	{
		maxoffset = 0;
	}
	elif ((ahc->features & AHC_ULTRA2) != 0)
	{
		maxoffset = MAX_OFFSET_ULTRA2;
	}
	else
	{
		if (wide)
			maxoffset = MAX_OFFSET_16BIT;
		else
			maxoffset = MAX_OFFSET_8BIT;
	}

	*offset = MIN (*offset, maxoffset);

	if (tinfo != NULL)
		*offset = MIN (*offset, tinfo->goal.offset);

}	/* end ahc_validate_offset */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Truncate the given transfer width parameter to a value the
 * current adapter type is capable of.
 */
void
ahc_validate_width (struct ahc_softc *ahc, struct ahc_tinfo_trio * tinfo, uint * bus_width)
{
	switch (*bus_width)
	{
	    default:
		if (ahc->features & AHC_WIDE)
		{
			/* Respond Wide */

			*bus_width = MSG_EXT_WDTR_BUS_16_BIT;
			break;
		}
		/* FALLTHROUGH */

	    case MSG_EXT_WDTR_BUS_8_BIT:
		*bus_width = MSG_EXT_WDTR_BUS_8_BIT;
		break;

	}	/* end switch */

	if (tinfo != NULL)
		*bus_width = MIN (tinfo->goal.width, *bus_width);

}	/* end ahc_validate_width */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Update the bitmask of targets for which the controller should
 * negotiate with at the next convenient oportunity.  This currently
 * means the next time we send the initial identify messages for
 * a new transaction.
 */
void
ahc_update_target_msg_request (struct ahc_softc *ahc, int target, int force, int paused)
{
	struct ahc_tinfo_trio	*tinfo;
	ushort			targ_msg_req_orig, mask;

	tinfo = &ahc->ahc_info[target].transinfo;
	mask = 1 << target;

	targ_msg_req_orig = ahc->targ_msg_req;

	if
	(	tinfo->current.period != tinfo->goal.period || tinfo->current.width != tinfo->goal.width ||
		tinfo->current.offset != tinfo->goal.offset ||
		tinfo->current.ppr_options != tinfo->goal.ppr_options ||
			(	force && (tinfo->goal.period != 0 || tinfo->goal.width != MSG_EXT_WDTR_BUS_8_BIT
			    || tinfo->goal.ppr_options != 0)
			)
	)
		ahc->targ_msg_req |= mask;
	else
		ahc->targ_msg_req &= ~mask;

	/*
	 *	Verifica se mudou
	 */
	if (ahc->targ_msg_req != targ_msg_req_orig)
	{
		/* Update the message request bit for this target */

		if (!paused)
			pause_sequencer (ahc);

		ahc_outb (ahc, TARGET_MSG_REQUEST,     ahc->targ_msg_req);
		ahc_outb (ahc, TARGET_MSG_REQUEST + 1, ahc->targ_msg_req >> 8);

		if (!paused)
			unpause_sequencer (ahc);
	}

}	/* end ahc_update_target_msg_request */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Update the user/goal/current tables of synchronous negotiation
 * parameters as well as, in the case of a current or active update,
 * any data structures on the host controller.  In the case of an
 * active update, the specified target is currently talking to us on
 * the bus, so the transfer parameter update must take effect
 * immediately.
 */
void
ahc_set_syncrate (struct ahc_softc *ahc, int target,
		  const struct ahc_syncrate *syncrate, uint period,
		  uint offset, uint ppr_options, uint type, int paused)
{
	AHCINFO		*sp;
	uint		control, old_period, old_offset, old_ppr;
	int		active = (type & AHC_TRANS_ACTIVE) == AHC_TRANS_ACTIVE;
	ushort		mask;

	/*
	 *	x
	 */
	if (syncrate == NULL)
		{ period = 0; offset = 0; }

	sp = &ahc->ahc_info[target];
	mask = 1 << target;

	old_period = sp->transinfo.current.period;
	old_offset = sp->transinfo.current.offset;
	old_ppr    = sp->transinfo.current.ppr_options;

	/*
	 *	x
	 */
	if
	(	(type & AHC_TRANS_CUR) != 0 &&
		(old_period != period || old_offset != offset || old_ppr != ppr_options)
	)
	{
		uint           scsirate;

		scsirate = sp->transinfo.scsirate;

		if ((ahc->features & AHC_ULTRA2) != 0)
		{
			scsirate &= ~(SXFR_ULTRA2 | SINGLE_EDGE | ENABLE_CRC);

			if (syncrate != NULL)
			{
				scsirate |= syncrate->sxfr_u2;

				if ((ppr_options & MSG_EXT_PPR_DT_REQ) != 0)
					scsirate |= ENABLE_CRC;
				else
					scsirate |= SINGLE_EDGE;
			}
		}
		else
		{
			scsirate &= ~(SXFR | SOFS);

			/*
			 *	Ensure Ultra mode is set properly for this target.
			 */
			ahc->ultraenb &= ~mask;

			if (syncrate != NULL)
			{
				if (syncrate->sxfr & ULTRA_SXFR)
					ahc->ultraenb |= mask;

				scsirate |= syncrate->sxfr & SXFR;
				scsirate |= offset & SOFS;
			}

			if (active)
			{
				uint           sxfrctl0;

				sxfrctl0 = ahc_inb (ahc, SXFRCTL0);
				sxfrctl0 &= ~FAST20;

				if (ahc->ultraenb & mask)
					sxfrctl0 |= FAST20;

				ahc_outb (ahc, SXFRCTL0, sxfrctl0);
			}
		}

		if (active)
		{
			ahc_outb (ahc, SCSIRATE, scsirate);

			if ((ahc->features & AHC_ULTRA2) != 0)
				ahc_outb (ahc, SCSIOFFSET, offset);
		}

		sp->transinfo.scsirate			= scsirate;
		sp->transinfo.current.period		= period;
		sp->transinfo.current.offset		= offset;
		sp->transinfo.current.ppr_options	= ppr_options;

		/*
		 *	Atualiza os dados na controladora
		 */
		control = ahc_inb (ahc, SCB_CONTROL);
		control &= ~ULTRAENB;

		if ((ahc->ultraenb & mask) != 0)
			control |= ULTRAENB;

		ahc_outb (ahc, SCB_CONTROL, control);
		ahc_outb (ahc, SCB_SCSIRATE, sp->transinfo.scsirate);
		ahc_outb (ahc, SCB_SCSIOFFSET, sp->transinfo.current.offset);

		/*
		 *	Se for o caso, imprime a mensagem
		 */
		if (offset != 0)
		{
			sp->info_rate = sp->transinfo.current.width ? syncrate->double_rate : syncrate->rate;

			if (speed_verbose) printf
			(
				"%s: Síncrono a %s MB/s (%d bits)\n",
				sp->info_scsi.scsi_dev_nm, sp->info_rate,
				8 * (1 << sp->transinfo.current.width)
			);

#if (0)	/*******************************************************/
			if (speed_verbose) printf
			(
				"%s: Síncrono a %s MB/s (freq. = %s MHz%s, offset = %d)\n",
				sp->info_scsi.scsi_dev_nm, sp->info_rate,
				syncrate->rate, (ppr_options & MSG_EXT_PPR_DT_REQ) ? " DT" : "", offset
			);
#endif	/*******************************************************/
		}
		else
		{
			sp->info_rate = NOSTR;

			if (speed_verbose) printf
			(
				"%s: Assíncrono\n",
				sp->info_scsi.scsi_dev_nm
			);
		}
	}

	if ((type & AHC_TRANS_GOAL) != 0)
	{
		sp->transinfo.goal.period = period;
		sp->transinfo.goal.offset = offset;
		sp->transinfo.goal.ppr_options = ppr_options;
	}

	if ((type & AHC_TRANS_USER) != 0)
	{
		sp->transinfo.user.period = period;
		sp->transinfo.user.offset = offset;
		sp->transinfo.user.ppr_options = ppr_options;
	}

	ahc_update_target_msg_request (ahc, target, /* force */ FALSE, paused);

}	/* end ahc_set_syncrate */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Update the user/goal/current tables of wide negotiation
 * parameters as well as, in the case of a current or active update,
 * any data structures on the host controller.  In the case of an
 * active update, the specified target is currently talking to us on
 * the bus, so the transfer parameter update must take effect
 * immediately.
 */
void
ahc_set_width (struct ahc_softc *ahc, int target, uint width, uint type, int paused)
{
	AHCINFO			*sp;
	uint				oldwidth;
	int				active = (type & AHC_TRANS_ACTIVE) == AHC_TRANS_ACTIVE;

	sp = &ahc->ahc_info[target];

	oldwidth = sp->transinfo.current.width;

	if ((type & AHC_TRANS_CUR) != 0 && oldwidth != width)
	{
		uint           scsirate;

		scsirate = sp->transinfo.scsirate;
		scsirate &= ~WIDEXFER;

		if (width == MSG_EXT_WDTR_BUS_16_BIT)
			scsirate |= WIDEXFER;

		sp->transinfo.scsirate = scsirate;

		if (active)
			ahc_outb (ahc, SCSIRATE, scsirate);

		sp->transinfo.current.width = width;

#if (0)	/*******************************************************/
		if (speed_verbose) printf
		(
			"%s: Usando barramento de %d bits\n",
			sp->info_scsi.scsi_dev_nm, 8 * (1 << width)
		);
#endif	/*******************************************************/
	}

	if ((type & AHC_TRANS_GOAL) != 0)
		sp->transinfo.goal.width = width;

	if ((type & AHC_TRANS_USER) != 0)
		sp->transinfo.user.width = width;

	ahc_update_target_msg_request (ahc, target, /* force */ FALSE, paused);

}	/* end ahc_set_width */

/*
 ****************************************************************
 * Update the current state of tagged queuing for given target	*
 ****************************************************************
 */
void
ahc_set_tags (struct ahc_softc *ahc, int target, int enable)
{
	struct ahc_tinfo_trio	*tinfo;
	ushort			orig_tagenable, mask;

	mask = 1 << target;
	tinfo = &ahc->ahc_info[target].transinfo;

	orig_tagenable = ahc->tagenable;

	if (enable)
		ahc->tagenable |= mask;
	else
		ahc->tagenable &= ~mask;

}	/* end ahc_set_tags */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/************************ Message Phase Processing ****************************/
/*
 * When an initiator transaction with the MK_MESSAGE flag either reconnects
 * or enters the initial message out phase, we are interrupted.  Fill our
 * outgoing message buffer with the appropriate message and beging handing
 * the message phase(s) manually.
 */
void
ahc_setup_initiator_msgout (struct ahc_softc *ahc, int target, struct ahc_scb *scbp)
{
	ushort		mask;

	mask = 1 << target;

	/*
	 *	To facilitate adding multiple messages together, each routine should
	 *	increment the index and len variables instead of setting them explicitly.
	 */
	ahc->msgout_index = 0;
	ahc->msgout_len = 0;

#if (0)	/*******************************************************/
	if ((scbp->flags & SCB_DEVICE_RESET) == 0 && ahc_inb (ahc, MSG_OUT) == MSG_IDENTIFYFLAG)
#endif	/*******************************************************/

	if (ahc_inb (ahc, MSG_OUT) == MSG_IDENTIFYFLAG)
	{
		uchar		identify_msg;

		identify_msg = MSG_IDENTIFYFLAG | scbp->hscb->lun;

		if ((scbp->hscb->control & DISCENB) != 0)
			identify_msg |= MSG_IDENTIFY_DISCFLAG;

		ahc->msgout_buf[ahc->msgout_index++] = identify_msg;
		ahc->msgout_len++;

		if ((scbp->hscb->control & TAG_ENB) != 0)
		{
			ahc->msgout_buf[ahc->msgout_index++] = scbp->hscb->control & (TAG_ENB | SCB_TAG_TYPE);
			ahc->msgout_buf[ahc->msgout_index++] = scbp->hscb->tag;
			ahc->msgout_len += 2;
		}
	}

#if (0)	/*******************************************************/
	if   (scbp->flags & SCB_DEVICE_RESET)
	{
		ahc->msgout_buf[ahc->msgout_index++] = MSG_BUS_DEV_RESET;
		ahc->msgout_len++;

		printf
		(	"%s: Foi enviada a mensagem de inicialização de barramento\n",
			ahc_get_dev_nm_by_scb (ahc, scbp)
		);
	}
	elif ((scbp->flags & SCB_ABORT) != 0)
	{
		if ((scbp->hscb->control & TAG_ENB) != 0)
			ahc->msgout_buf[ahc->msgout_index++] = MSG_ABORT_TAG;
		else
			ahc->msgout_buf[ahc->msgout_index++] = MSG_ABORT;

		ahc->msgout_len++;

		printf
		(	"%s: Foi enviada a mensagem de aborto\n",
			ahc_get_dev_nm_by_scb (ahc, scbp)
		);
	}
	elif ((ahc->targ_msg_req & mask) != 0 || (scbp->flags & SCB_NEGOTIATE) != 0)
#endif	/*******************************************************/

	if ((ahc->targ_msg_req & mask) != 0)
	{
		ahc_build_transfer_msg (ahc, target);
	}
	else
	{
		printf ("ahc_intr: AWAITING_MSG for an SCB that does not have a waiting message\n");

		printf
		(	"%s: Esperando mensagem para um SCB que não a pediu\n",
			ahc_get_dev_nm (ahc, target)
		);
	}

	/*
	 * Clear the MK_MESSAGE flag from the SCB so we aren't asked to send this message again.
	 */
	ahc_outb (ahc, SCB_CONTROL, ahc_inb (ahc, SCB_CONTROL) & ~MK_MESSAGE);

	ahc->msgout_index = 0;
	ahc->msg_type = MSG_TYPE_INITIATOR_MSGOUT;

}	/* end ahc_setup_initiator_msgout */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Build an appropriate transfer negotiation message for the
 * currently active target.
 */
void
ahc_build_transfer_msg (struct ahc_softc *ahc, int target)
{
	/*
	 *	We need to initiate transfer negotiations. If our current and goal
	 *	settings are identical, we want to renegotiate due to a check condition.
	 */
	AHCINFO			*sp;
	struct ahc_tinfo_trio	*tinfo;
	const struct ahc_syncrate	*rate;
	int			dowide, dosync, doppr, use_ppr;
	uint			period, ppr_options, offset;

	sp = &ahc->ahc_info[target];
	tinfo = &sp->transinfo;

	dowide = tinfo->current.width != tinfo->goal.width;
	dosync = tinfo->current.period != tinfo->goal.period;
	doppr = tinfo->current.ppr_options != tinfo->goal.ppr_options;

	if (!dowide && !dosync && !doppr)
	{
		dowide = tinfo->goal.width != MSG_EXT_WDTR_BUS_8_BIT;
		dosync = tinfo->goal.period != 0;
		doppr = tinfo->goal.ppr_options != 0;
	}

	if (!dowide && !dosync && !doppr)
		printf ("%s: ERRO FATAL: AWAITING_MSG para negociação, mas ela não é necessária\n", ahc->name);

	use_ppr = (tinfo->current.transport_version >= 3) || doppr;

	/*
	 * Both the PPR message and SDTR message require the goal syncrate to
	 * be limited to what the target device is capable of handling (based
	 * on whether an LVD->SE expander is on the bus), so combine these
	 * two cases. Regardless, guarantee that if we are using WDTR and
	 * SDTR messages that WDTR comes first.
	 */
	if (use_ppr || (dosync && !dowide))
	{

		period = tinfo->goal.period;
		ppr_options = tinfo->goal.ppr_options;

		if (use_ppr == 0)
			ppr_options = 0;

		rate = ahc_devlimited_syncrate (ahc, tinfo, &period, &ppr_options);
		offset = tinfo->goal.offset;

		ahc_validate_offset
		(	ahc, tinfo, rate, &offset,
			use_ppr ? tinfo->goal.width : tinfo->current.width
		);

		if (use_ppr)
			ahc_construct_ppr (ahc, sp, period, offset, tinfo->goal.width, ppr_options);
		else
			ahc_construct_sdtr (ahc, sp, period, offset);
	}
	else
	{
		ahc_construct_wdtr (ahc, sp, tinfo->goal.width);
	}

}	/* end ahc_build_transfer_msg */

/*
 ****************************************************************
 *	Build a synchronous negotiation message in our message	*
 ****************************************************************
 */
void
ahc_construct_sdtr (struct ahc_softc *ahc, const AHCINFO *sp, uint period, uint offset)
{
	uchar		*cp = &ahc->msgout_buf[ahc->msgout_index];

	*cp++ = MSG_EXTENDED;
	*cp++ = MSG_EXT_SDTR_LEN;
	*cp++ = MSG_EXT_SDTR;
	*cp++ = period;
	*cp   = offset;

	ahc->msgout_index += 5;
	ahc->msgout_len += 5;

#undef	MSG_VERBOSE
#ifdef	MSG_VERBOSE
	printf
	(	"%s: Sending SDTR period %d, offset %d\n",
		sp->info_scsi.scsi_dev_nm, period, offset
	);
#endif	MSG_VERBOSE

}	/* end ahc_construct_sdtr */

/*
 ****************************************************************
 *	Build a wide negotiateion message in our message	*
 ****************************************************************
 */
void
ahc_construct_wdtr (struct ahc_softc *ahc, const AHCINFO *sp, uint bus_width)
{
	uchar		*cp = &ahc->msgout_buf[ahc->msgout_index];

	*cp++ = MSG_EXTENDED;
	*cp++ = MSG_EXT_WDTR_LEN;
	*cp++ = MSG_EXT_WDTR;
	*cp   = bus_width;

	ahc->msgout_index += 4;
	ahc->msgout_len += 4;

#ifdef	MSG_VERBOSE
	printf
	(	"%s: Sending WDTR %x\n",
		sp->info_scsi.scsi_dev_nm, bus_width
	);
#endif	MSG_VERBOSE

}	/* end ahc_construct_wdtr */

/*
 ****************************************************************
 *   Build a parallel protocol request message in our message	*
 ****************************************************************
 */
void
ahc_construct_ppr (struct ahc_softc *ahc, const AHCINFO *sp, uint period, uint offset,
		uint bus_width, uint ppr_options)
{
	uchar		*cp = &ahc->msgout_buf[ahc->msgout_index];

	*cp++ = MSG_EXTENDED;
	*cp++ = MSG_EXT_PPR_LEN;
	*cp++ = MSG_EXT_PPR;
	*cp++ = period;
	*cp++ = 0;
	*cp++ = offset;
	*cp++ = bus_width;
	*cp   = ppr_options;

	ahc->msgout_index += 8;
	ahc->msgout_len += 8;

#ifdef	MSG_VERBOSE
	printf
	(	"%s: Sending PPR bus_width %x, period %d, offset %d, ppr_options %x\n",
		sp->info_scsi.scsi_dev_nm, bus_width, period, offset, ppr_options
	);
#endif	MSG_VERBOSE

}	/* end ahc_construct_ppr */

/*
 ****************************************************************
 *	Clear any active message state				*
 ****************************************************************
 */
void
ahc_clear_msg_state (struct ahc_softc *ahc)
{
	ahc->msgout_len = 0;
	ahc->msgin_index = 0;
	ahc->msg_type = MSG_TYPE_NONE;
	ahc_outb (ahc, MSG_OUT, MSG_NOOP);

}	/* end ahc_clear_msg_state */

/*
 ****************************************************************
 *	Manual message loop handler				*
 ****************************************************************
 */
void
ahc_handle_message_phase (struct ahc_softc *ahc)
{
	uint			bus_phase;
	int			target, end_session;

	target = ahc_inb (ahc, SAVED_SCSIID) >> 4;

	end_session = FALSE;
	bus_phase = ahc_inb (ahc, SCSISIGI) & PHASE_MASK;

    reswitch:
	switch (ahc->msg_type)
	{
	    case MSG_TYPE_INITIATOR_MSGOUT:
	    {
		int		lastbyte, phasemis, msgdone;

		if (ahc->msgout_len == 0)
			printf ("%s: ERRO FATAL: Interrupção HOST_MSG_LOOP sem mensagem ativa", ahc->name);

		if (phasemis = (bus_phase != P_MESGOUT))
		{
			if (bus_phase == P_MESGIN)
			{
				/*
				 * Change gears and see if this messages is of interest to us or
				 * should be passed back to the sequencer.
				 */
				ahc_outb (ahc, CLRSINT1, CLRATNO);
				ahc->send_msg_perror = FALSE;
				ahc->msg_type = MSG_TYPE_INITIATOR_MSGIN;
				ahc->msgin_index = 0;
				goto reswitch;
			}

			end_session = TRUE;
			break;
		}

		if (ahc->send_msg_perror)
		{
			ahc_outb (ahc, CLRSINT1, CLRATNO);
			ahc_outb (ahc, CLRSINT1, CLRREQINIT);
			ahc_outb (ahc, SCSIDATL, MSG_PARITY_ERROR);

			break;
		}

		if (msgdone = ahc->msgout_index == ahc->msgout_len)
		{
			/*
			 * The target has requested a retry.
			 * Re-assert ATN, reset our message index to
			 * 0, and try again.
			 */
			ahc->msgout_index = 0;
			ahc_outb (ahc, SCSISIGO, ahc_inb (ahc, SCSISIGO) | ATNO);
		}

		if (lastbyte = ahc->msgout_index == (ahc->msgout_len - 1))
		{
			/* Last byte is signified by dropping ATN */

			ahc_outb (ahc, CLRSINT1, CLRATNO);
		}

		/*
		 *	Clear our interrupt status and present the next byte on the bus.
		 */
		ahc_outb (ahc, CLRSINT1, CLRREQINIT);
		ahc_outb (ahc, SCSIDATL, ahc->msgout_buf[ahc->msgout_index++]);
		break;
	    }

	    case MSG_TYPE_INITIATOR_MSGIN:
	    {
		int		phasemis, message_done;

		if (phasemis = (bus_phase != P_MESGIN))
		{
			ahc->msgin_index = 0;

			if
			(	bus_phase == P_MESGOUT && (ahc->send_msg_perror == TRUE ||
				(ahc->msgout_len != 0 && ahc->msgout_index == 0))
			)
			{
				ahc->msg_type = MSG_TYPE_INITIATOR_MSGOUT;
				goto reswitch;
			}

			end_session = TRUE;
			break;
		}

		/* Pull the byte in without acking it */

		ahc->msgin_buf[ahc->msgin_index] = ahc_inb (ahc, SCSIBUSL);

		if (message_done = ahc_parse_msg (ahc, target))
		{
			/*
			 * Clear our incoming message buffer in case
			 * there is another message following this
			 * one.
			 */
			ahc->msgin_index = 0;

			/*
			 * If this message illicited a response,
			 * assert ATN so the target takes us to the
			 * message out phase.
			 */
			if (ahc->msgout_len != 0)
				ahc_outb (ahc, SCSISIGO, ahc_inb (ahc, SCSISIGO) | ATNO);
		}
		else
		{
			ahc->msgin_index++;
		}

		/* Ack the byte */

		ahc_outb (ahc, CLRSINT1, CLRREQINIT);
		ahc_inb (ahc, SCSIDATL);
		break;
	    }

	    case MSG_TYPE_TARGET_MSGIN:
	    {
		int		msgdone, msgout_request;

		if (ahc->msgout_len == 0)
			printf ("%s: ERRO FATAL: MSGIN de alvo sem mensagem ativa\n", ahc->name);

		/*
		 * If we interrupted a mesgout session, the initiator
		 * will not know this until our first REQ.  So, we
		 * only honor mesgout requests after we've sent our
		 * first byte.
		 */
		if ((ahc_inb (ahc, SCSISIGI) & ATNI) != 0 && ahc->msgout_index > 0)
			msgout_request = TRUE;
		else
			msgout_request = FALSE;

		if (msgout_request)
		{
			/*
			 *	Change gears and see if this messages is of interest to
			 *	us or should be passed back to the sequencer.
			 */
			ahc->msg_type = MSG_TYPE_TARGET_MSGOUT;
			ahc_outb (ahc, SCSISIGO, P_MESGOUT | BSYO);
			ahc->msgin_index = 0;

			/* Dummy read to REQ for first byte */

			ahc_inb (ahc, SCSIDATL);
			ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) | SPIOEN);
			break;
		}

		if (msgdone = ahc->msgout_index == ahc->msgout_len)
		{
			ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) & ~SPIOEN);
			end_session = TRUE;
			break;
		}

		/*
		 * Present the next byte on the bus.
		 */
		ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) | SPIOEN);
		ahc_outb (ahc, SCSIDATL, ahc->msgout_buf[ahc->msgout_index++]);
		break;
	    }

	    case MSG_TYPE_TARGET_MSGOUT:
	    {
		int		lastbyte, msgdone;

		/*
		 *	The initiator signals that this is the last byte by dropping ATN
		 */
		lastbyte = (ahc_inb (ahc, SCSISIGI) & ATNI) == 0;

		/*
		 *	Read the latched byte, but turn off SPIOEN first so that we don't
		 *	inadvertantly cause a REQ for the next byte
		 */
		ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) & ~SPIOEN);
		ahc->msgin_buf[ahc->msgin_index] = ahc_inb (ahc, SCSIDATL);

		if ((msgdone = ahc_parse_msg (ahc, target)) == MSGLOOP_TERMINATED)
		{
			/*
			 * The message is *really* done in that it
			 * caused us to go to bus free.  The
			 * sequencer has already been reset at this
			 * point, so pull the ejection handle.
			 */
			return;
		}

		ahc->msgin_index++;

		/*
		 * XXX Read spec about initiator dropping ATN too
		 * soon and use msgdone to detect it.
		 */
		if (msgdone == MSGLOOP_MSGCOMPLETE)
		{
			ahc->msgin_index = 0;

			/*
			 *	If this message illicited a response, transition to
			 *	the Message in phase and send it.
			 */
			if (ahc->msgout_len != 0)
			{
				ahc_outb (ahc, SCSISIGO, P_MESGIN | BSYO);
				ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) | SPIOEN);
				ahc->msg_type = MSG_TYPE_TARGET_MSGIN;
				ahc->msgin_index = 0;

				break;
			}
		}

		if (lastbyte)
			end_session = TRUE;
		else
		{
			/* Ask for the next byte. */

			ahc_outb (ahc, SXFRCTL0, ahc_inb (ahc, SXFRCTL0) | SPIOEN);
		}

		break;
	    }

	    default:
		printf ("%s: ERRO FATAL: Tipo desconhecido de mensagem REQINIT\n", ahc->name);

	}	/* end switch (ahc->msg_type) */

	if (end_session)
	{
		ahc_clear_msg_state (ahc);
		ahc_outb (ahc, RETURN_1, EXIT_MSG_LOOP);
	}
	else
		ahc_outb (ahc, RETURN_1, CONT_MSG_LOOP);

}	/* end ahc_handle_message_phase */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * See if we sent a particular extended message to the target.
 * If "full" is true, return true only if the target saw the full
 * message.  If "full" is false, return true if the target saw at
 * least the first byte of the message.
 */
int
ahc_sent_msg (struct ahc_softc *ahc, uint msgtype, int full)
{
	int		found;
	uint		index;

	found = FALSE;
	index = 0;

	while (index < ahc->msgout_len)
	{
		if   (ahc->msgout_buf[index] == MSG_EXTENDED)
		{
			/* Found a candidate */

			if (ahc->msgout_buf[index + 2] == msgtype)
			{
				uint           end_index;

				end_index = index + 1 + ahc->msgout_buf[index + 1];

				if   (full)
				{
					if (ahc->msgout_index > end_index)
						found = TRUE;
				}
				elif (ahc->msgout_index > index)
				{
					found = TRUE;
				}
			}

			break;
		}
		elif
		(	ahc->msgout_buf[index] >= MSG_SIMPLE_Q_TAG &&
			ahc->msgout_buf[index] <= MSG_IGN_WIDE_RESIDUE
		)
		{
			/* Skip tag type and tag id or residue param */

			index += 2;
		}
		else
		{
			/* Single byte message */

			index++;
		}
	}

	return (found);

}	/* end ahc_sent_msg */

/*
 ****************************************************************
 * Wait for a complete incomming message, parse it, and respond *
 ****************************************************************
 */
int
ahc_parse_msg (struct ahc_softc *ahc, int target)
{
	AHCINFO			*sp;
	struct ahc_tinfo_trio	*tinfo;
	int			reject, done, response;
	uint			targ_scsirate;

	done = MSGLOOP_IN_PROG;
	response = FALSE;
	reject = FALSE;

	sp = &ahc->ahc_info[target];
	tinfo = &sp->transinfo;

	targ_scsirate = tinfo->scsirate;

	/*
	 * Parse as much of the message as is availible, rejecting it if we
	 * don't support it.  When the entire message is availible and has
	 * been handled, return MSGLOOP_MSGCOMPLETE, indicating that we have
	 * parsed an entire message.
	 * 
	 * In the case of extended messages, we accept the length byte outright
	 * and perform more checking once we know the extended message type.
	 */
	switch (ahc->msgin_buf[0])
	{
	    case MSG_MESSAGE_REJECT:
		response = ahc_handle_msg_reject (ahc, target);
		/* FALLTHROUGH */

	    case MSG_NOOP:
		done = MSGLOOP_MSGCOMPLETE;
		break;

	    case MSG_EXTENDED:
	    {
		/* Wait for enough of the message to begin validation */

		if (ahc->msgin_index < 2)
			break;

		switch (ahc->msgin_buf[2])
		{
		    case MSG_EXT_SDTR:
		    {
			const struct ahc_syncrate	*syncrate;
			uint				period, ppr_options, offset, saved_offset;

			if (ahc->msgin_buf[1] != MSG_EXT_SDTR_LEN)
			{
				reject = TRUE;
				break;
			}

			/*
			 *	Wait until we have both args before validating and acting on this message.
			 *	Add one to MSG_EXT_SDTR_LEN to account for the extended message preamble
			 */
			if (ahc->msgin_index < (MSG_EXT_SDTR_LEN + 1))
				break;

			period = ahc->msgin_buf[3];
			ppr_options = 0;
			saved_offset = offset = ahc->msgin_buf[4];

			syncrate = ahc_devlimited_syncrate (ahc, tinfo, &period, &ppr_options);

			ahc_validate_offset
				(ahc, tinfo, syncrate, &offset, targ_scsirate & WIDEXFER);
#ifdef	MSG_VERBOSE
			printf
			(	"(%s:%d): Received SDTR period %d, offset %d\n"
				"\tFiltered to period %d, offset %d\n",
				ahc->name, target,
				ahc->msgin_buf[3], saved_offset, period, offset
			);
#endif	MSG_VERBOSE
			ahc_set_syncrate
			(	ahc, target, syncrate, period, offset, ppr_options,
				AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE
			);

			/*
			 *	See if we initiated Sync Negotiation and didn't have to
			 *	fall down to async transfers.
			 */
			if (ahc_sent_msg (ahc, MSG_EXT_SDTR, /* full */ TRUE))
			{
				/* We started it */

				if (saved_offset != offset)
				{
					/*
					 *	Went too low - force async
					 */
					reject = TRUE;
				}
			}
			else
			{
				/*
				 *	Send our own SDTR in reply
				 */
#ifdef	MSG_VERBOSE
				printf
				(	"(%s:%d): Target Initiated SDTR\n",
					ahc->name, target
				);
#endif	MSG_VERBOSE
				ahc->msgout_index = 0;
				ahc->msgout_len = 0;
				ahc_construct_sdtr (ahc, sp, period, offset);
				ahc->msgout_index = 0;
				response = TRUE;
			}

			done = MSGLOOP_MSGCOMPLETE;
			break;

		    }	/* end case MSG_EXT_SDTR */

		    case MSG_EXT_WDTR:
		    {
			uint		bus_width, saved_width, sending_reply;

			sending_reply = FALSE;

			if (ahc->msgin_buf[1] != MSG_EXT_WDTR_LEN)
			{
				reject = TRUE;
				break;
			}

			/*
			 *	Wait until we have our arg before validating and acting on this message.
			 * 
			 *	Add one to MSG_EXT_WDTR_LEN to account for the extended message preamble.
			 */
			if (ahc->msgin_index < (MSG_EXT_WDTR_LEN + 1))
				break;

			bus_width = ahc->msgin_buf[3];
			saved_width = bus_width;
			ahc_validate_width (ahc, tinfo, &bus_width);
#ifdef	MSG_VERBOSE
			printf
			(	"(%s:%d): Received WDTR %x filtered to %x\n",
				ahc->name, target, saved_width, bus_width
			);
#endif	MSG_VERBOSE
			if (ahc_sent_msg (ahc, MSG_EXT_WDTR, /* full */ TRUE))
			{
				/*
				 *	Don't send a WDTR back to the target, since we asked
				 *	first. If the width went higher than our request, reject it.
				 */
				if (saved_width > bus_width)
				{
					reject = TRUE;

					printf
					(	"(%s:%d): requested %dBit transfers.  Rejecting...\n",
						ahc->name, target, 8 * (0x01 << bus_width)
					);
					bus_width = 0;
				}
			}
			else
			{
				/*
				 *	Send our own WDTR in reply
				 */
#ifdef	MSG_VERBOSE
				printf
				(	"(%s:%d): Target Initiated WDTR\n",
					ahc->name, target
				);
#endif	MSG_VERBOSE
				ahc->msgout_index = 0;
				ahc->msgout_len = 0;
				ahc_construct_wdtr (ahc, sp, bus_width);
				ahc->msgout_index = 0;
				response = TRUE;
				sending_reply = TRUE;
			}

			ahc_set_width
				(ahc, target, bus_width, AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE);

			/* After a wide message, we are async */

			ahc_set_syncrate
			(	ahc, target, /* syncrate */ NULL, /* period */ 0,
				/* offset */ 0, /* ppr_options */ 0, AHC_TRANS_ACTIVE, /* paused */ TRUE
			);

			if (sending_reply == FALSE && reject == FALSE)
			{
				if (tinfo->goal.period)
				{
					ahc->msgout_index = 0;
					ahc->msgout_len = 0;
					ahc_build_transfer_msg (ahc, target);
					ahc->msgout_index = 0;
					response = TRUE;
				}
			}

			done = MSGLOOP_MSGCOMPLETE;
			break;

		    }	/* end case MSG_EXT_WDTR */

		    case MSG_EXT_PPR:
		    {
			const struct ahc_syncrate	*syncrate;
			uint			period, offset, bus_width, ppr_options, saved_width;
			uint			saved_offset, saved_ppr_options;

			if (ahc->msgin_buf[1] != MSG_EXT_PPR_LEN)
			{
				reject = TRUE;
				break;
			}

			/*
			 *	Wait until we have all args before validating and acting on this message.
			 *	Add one to MSG_EXT_PPR_LEN to account for the extended message preamble.
			 */
			if (ahc->msgin_index < (MSG_EXT_PPR_LEN + 1))
				break;

			period = ahc->msgin_buf[3];
			offset = ahc->msgin_buf[5];
			bus_width = ahc->msgin_buf[6];
			saved_width = bus_width;
			ppr_options = ahc->msgin_buf[7];

			/*
			 *  According to the spec, a DT only period factor with no DT option set implies async.
			 */
			if ((ppr_options & MSG_EXT_PPR_DT_REQ) == 0 && period == 9)
				offset = 0;

			saved_ppr_options = ppr_options;
			saved_offset = offset;

			/*
			 *	Mask out any options we don't support on any controller.
			 *	Transfer options are only available if we are negotiating wide.
			 */
			ppr_options &= MSG_EXT_PPR_DT_REQ;

			if (bus_width == 0)
				ppr_options = 0;

			ahc_validate_width (ahc, tinfo, &bus_width);
			syncrate = ahc_devlimited_syncrate (ahc, tinfo, &period, &ppr_options);
			ahc_validate_offset (ahc, tinfo, syncrate, &offset, bus_width);

			if (ahc_sent_msg (ahc, MSG_EXT_PPR, /* full */ TRUE))
			{
				/*
				 *	If we are unable to do any of the requested options
				 *	(we went too low), then we'll have to reject the message.
				 */
				if
				(	saved_width > bus_width || saved_offset != offset ||
					saved_ppr_options != ppr_options
				)
				{
					reject = TRUE;
					period = 0;
					offset = 0;
					bus_width = 0;
					ppr_options = 0;
					syncrate = NULL;
				}
			}
			else
			{
				printf
				(	"(%s:%d): Target Initiated PPR\n",
					ahc->name, target
				);

				ahc->msgout_index = 0;
				ahc->msgout_len = 0;
				ahc_construct_ppr (ahc, sp, period, offset, bus_width, ppr_options);
				ahc->msgout_index = 0;
				response = TRUE;
			}
#ifdef	MSG_VERBOSE
			printf
			(	"(%s:%d): Received PPR width %x, period %d, offset %d, "
				"options %x\n"
				"\tFiltered to width %x, period %d, offset %d, options %x\n",
				ahc->name, target,
				ahc->msgin_buf[3], saved_width,
				saved_offset, saved_ppr_options,
				bus_width, period, offset, ppr_options
			);
#endif	MSG_VERBOSE
			ahc_set_width
				(ahc, target, bus_width, AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE);

			ahc_set_syncrate
			(	ahc, target, syncrate, period, offset, ppr_options,
				AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE
			);
			done = MSGLOOP_MSGCOMPLETE;
			break;

		    }	/* end case MSG_EXT_PPR */

		    default:
			/* Unknown extended message.  Reject it. */
			reject = TRUE;
			break;

		}	/* end switch (ahc->msgin_buf[2]) */

		break;

	    }	/* end case MSG_EXTENDED */

	    case MSG_BUS_DEV_RESET:
		printf ("%s: ERRO FATAL: recebi uma mensagem para reiniciar dispositivo", ahc->name);

	    case MSG_ABORT_TAG:
	    case MSG_ABORT:
	    case MSG_CLEAR_QUEUE:
	    case MSG_TERM_IO_PROC:
	    default:
		reject = TRUE;
		break;
	}

	if (reject)
	{
		/*
		 *	Setup to reject the message
		 */
		ahc->msgout_index = 0;
		ahc->msgout_len = 1;
		ahc->msgout_buf[0] = MSG_MESSAGE_REJECT;
		done = MSGLOOP_MSGCOMPLETE;
		response = TRUE;
	}

	if (done != MSGLOOP_IN_PROG && !response) /* Clear the outgoing message buffer */
		ahc->msgout_len = 0;

	return (done);

}	/* end ahc_parse_msg */

/*
 ****************************************************************
 *	Process a message reject message			*
 ****************************************************************
 */
int
ahc_handle_msg_reject (struct ahc_softc *ahc, int target)
{
	/*
	 *	What we care about here is if we had an outstanding SDTR or WDTR
	 *	message for this target.  If we did, this is a signal that the
	 *	target is refusing negotiation.
	 */
	AHCINFO			*sp;
	struct ahc_scb		*scbp;
	uint			scb_tag, last_msg;
	int			response = 0;

	scb_tag = ahc_inb (ahc, SCB_TAG);
	scbp = ahc->scbindex[scb_tag];

	sp = &ahc->ahc_info[target];

	/* Might be necessary */

	last_msg = ahc_inb (ahc, LAST_MSG);

	if   (ahc_sent_msg (ahc, MSG_EXT_PPR, /* full */ FALSE))
	{
		/*
		 *	Target does not support the PPR message. Attempt to negotiate SPI-2 style.
		 */
#ifdef	MSG_VERBOSE
		printf
		(	"(%s:%d): PPR Rejected. Trying WDTR/SDTR\n",
			ahc->name, target
		);
#endif	MSG_VERBOSE

		sp->transinfo.goal.ppr_options = 0;
		sp->transinfo.current.transport_version = 2;
		sp->transinfo.goal.transport_version = 2;
		ahc->msgout_index = 0;
		ahc->msgout_len = 0;
		ahc_build_transfer_msg (ahc, target);
		ahc->msgout_index = 0;
		response = 1;
	}
	elif (ahc_sent_msg (ahc, MSG_EXT_WDTR, /* full */ FALSE))
	{
		/* note 8bit xfers */

		printf
		(	"%s: Recusou a negociação de barramento de 16 bits."
			" Usando transferências de 8 bits\n",
			sp->info_scsi.scsi_dev_nm
		);

		ahc_set_width
		(	ahc, target, MSG_EXT_WDTR_BUS_8_BIT,
			AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE
		);

		/*
		 * No need to clear the sync rate.  If the target did not
		 * accept the command, our syncrate is unaffected.  If the
		 * target started the negotiation, but rejected our response,
		 * we already cleared the sync rate before sending our WDTR.
		 */
		if (sp->transinfo.goal.period)
		{
			/* Start the sync negotiation */

			ahc->msgout_index = 0;
			ahc->msgout_len = 0;
			ahc_build_transfer_msg (ahc, target);
			ahc->msgout_index = 0;
			response = 1;
		}
	}
	elif (ahc_sent_msg (ahc, MSG_EXT_SDTR, /* full */ FALSE))
	{
		/* note asynch xfers and clear flag */

		ahc_set_syncrate
		(	ahc, target, /* syncrate */ NULL, /* period */ 0,
			/* offset */ 0, /* ppr_options */ 0,
			AHC_TRANS_ACTIVE | AHC_TRANS_GOAL, /* paused */ TRUE
		);

		printf
		(	"%s: Recusou a negociação síncrona. Usando transferências assíncronas\n",
			sp->info_scsi.scsi_dev_nm
		);
	}
	elif ((scbp->hscb->control & TAG_ENB) != 0)
	{
		printf
		(	"%s: Recusa comandos indexados. O processamento talvez seja interrompido\n",
			sp->info_scsi.scsi_dev_nm
		);

		sp->info_scsi.scsi_tagenable = 0;

		ahc_set_tags (ahc, target, FALSE);

		/*
		 *	Resend the identify for this CCB as the target may believe
		 *	that the selection is invalid otherwise.
		 */
		ahc_outb (ahc, SCB_CONTROL, ahc_inb (ahc, SCB_CONTROL) & ~MSG_SIMPLE_Q_TAG);
		scbp->hscb->control &= ~MSG_SIMPLE_Q_TAG;
		ahc_outb (ahc, MSG_OUT, MSG_IDENTIFYFLAG);
		ahc_outb (ahc, SCSISIGO, ahc_inb (ahc, SCSISIGO) | ATNO);

#if (0)	/*******************************************************/
		ahc_busy_tcl (ahc, BUILD_TCL (scbp->hscb->scsiid, 0), scbp->hscb->tag);
#endif	/*******************************************************/

		/* Neste ponto, várias coisas foram suprimidas ... */
	}
	else	/* Ignora */
	{
		printf
		(	"%s: Foi recusada a mensagem 0x%02X - ignorando\n",
			sp->info_scsi.scsi_dev_nm, last_msg
		);
	}

	return (response);

}	/* end ahc_handle_msg_reject */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_qinfifo_requeue_tail (struct ahc_softc *ahc, struct ahc_scb *scbp)
{
	struct ahc_scb     *prev_scb;

	prev_scb = NULL;

	if (ahc_qinfifo_count (ahc) != 0)
	{
		uint           prev_tag;
		uchar         prev_pos;

		prev_pos = ahc->qinfifonext - 1;
		prev_tag = ahc->qinfifo[prev_pos];
		prev_scb = ahc->scbindex[prev_tag];
	}

	ahc_qinfifo_requeue (ahc, prev_scb, scbp);

	if ((ahc->features & AHC_QUEUE_REGS) != 0)
		ahc_outb (ahc, HNSCB_QOFF, ahc->qinfifonext);
	else
		ahc_outb (ahc, KERNEL_QINPOS, ahc->qinfifonext);

}	/* end ahc_qinfifo_requeue_tail */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_qinfifo_requeue (struct ahc_softc *ahc, struct ahc_scb *prev_scb, struct ahc_scb *scbp)
{
	if (prev_scb == NULL)
		ahc_outb (ahc, NEXT_QUEUED_SCB, scbp->hscb->tag);
	else
		prev_scb->hscb->next = scbp->hscb->tag;

	ahc->qinfifo[ahc->qinfifonext++] = scbp->hscb->tag;
	scbp->hscb->next = ahc->next_queued_scb->hscb->tag;

}	/* end ahc_qinfifo_requeue */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ahc_qinfifo_count (struct ahc_softc *ahc)
{
	uchar        qinpos;
	uchar        diff;

	if ((ahc->features & AHC_QUEUE_REGS) != 0)
	{
		qinpos = ahc_inb (ahc, SNSCB_QOFF);
		ahc_outb (ahc, SNSCB_QOFF, qinpos);
	}
	else
	{
		qinpos = ahc_inb (ahc, QINPOS);
	}

	diff = ahc->qinfifonext - qinpos;

	return (diff);

}	/* end ahc_qinfifo_count */

/*
 ****************************************************************
 *	Calculate the residual for a just completed SCB		*
 ****************************************************************
 */
int
ahc_calc_residual (struct ahc_softc *ahc, struct ahc_scb *scbp)
{
	struct hardware_scb	*hscb;
	const struct status_pkt	*spkt;

	/*
	 *   5 cases:
	 *
	 *	1) No residual. SG_RESID_VALID clear in sgptr.
	 *	2) Transferless command
	 *	3) Never performed any transfers. sgptr has SG_FULL_RESID set.
	 *	4) No residual but target did not save data pointers after the
	 *	   last transfer, so sgptr was never updated.
	 *	5) We have a partial residual. Use residual_sgptr to determine
	 *	   where we are.
	 */
	hscb = scbp->hscb;

	if ((hscb->sgptr & SG_RESID_VALID) == 0)	 	/* Case 1 */
		return (0);

	hscb->sgptr &= ~SG_RESID_VALID;

	if ((hscb->sgptr & SG_LIST_NULL) != 0) 			/* Case 2 */
		return (0);

	spkt = &hscb->shared_data.status;

	if   ((hscb->sgptr & SG_FULL_RESID) != 0) 		/* Case 3 */
	{
		return (scbp->bp->b_base_count);
	}
	elif ((spkt->residual_sg_ptr & SG_LIST_NULL) != 0) 	/* Case 4 */
	{
		return (0);
	}
	elif ((spkt->residual_sg_ptr & ~SG_PTR_MASK) != 0)
	{
		printf ("%s: ERRO FATAL: Valor residual %d inválido\n", ahc->name, spkt->residual_sg_ptr);
		return (0);
	}
	else
	{
		return (spkt->residual_datacnt & AHC_SG_LEN_MASK);
	}

}	/* end ahc_calc_residual */

#ifndef	BOOT
/*
 ****************************************************************
 *	Terminou a execução de um SCB				*
 ****************************************************************
 */
void
ahc_done (struct ahc_softc *ahc, struct ahc_scb *scbp)
{
	struct hardware_scb	*hscb;
	uint			target;
	BHEAD			*bp;
	AHCINFO			*sp;
	int			sense;

	hscb = scbp->hscb;

	target = hscb->scsiid >> 4;

	sp = &ahc->ahc_info[target];

	bp = scbp->bp;

	/*
	 *	Remove da fila de pedidos pendentes
	 */
	sp->info_cur_op--;

	/*
	 *	Verifica se foi um comando que o disco não aceito
	 *	em virtude de fila cheia
	 */
	if (scbp->flags & SCB_QUEUE_FULL)
	{
		int		old_max_tag = sp->info_max_tag;
		int		new_max_tag = sp->info_cur_op;
		DEVHEAD		*dp;

		if (new_max_tag < old_max_tag)
		{
			sp->info_max_tag = new_max_tag;
#ifdef	TAG_DEBUG
			if (CSWT (37))
			{
				printf
				(	"%s: max_tag passou de %d para %d\n",
					sp->info_scsi.scsi_dev_nm,
					old_max_tag, new_max_tag
				);
			}
#endif	TAG_DEBUG
		}

		/* recoloca na fila */

		dp = &sp->info_utab;

		SPINLOCK (&dp->v_lock);

		bp->b_forw = dp->v_first;
	   /***	bp->b_back = NOBHEAD; ***/

		if (dp->v_first == NOBHEAD)
			dp->v_last = bp;

		dp->v_first = bp;

		SPINFREE (&dp->v_lock);

		ahc_put_scb (ahc, scbp);

		return;
	}

	/*
	 *	Verifica se houve "OVERRUN"
	 */
	if (scbp->flags & SCB_DATA_OVERRUN)
		goto bad;

	/*
	 *	Verifica se houve erro
	 */
	if ((scbp->flags & SCB_SENSE) == 0)
	{
		bp->b_residual = ahc_calc_residual (ahc, scbp);
		goto done;
	}

	scbp->flags &= ~SCB_SENSE;

	sense = (scbp->sense_area.flags & 0x0F);

	if   ((sense = scsi_sense (&sp->info_scsi, sense)) > 0)
		goto done;
	elif (sense < 0)
		goto very_bad;

	/*
	 *	Teve erro: verifica se tenta novamente
	 */
       bad:
	if (--bp->b_retry_cnt > 0)
	{
		ahc->scbindex[hscb->tag] = NULL;
		scbp->flags = SCB_FREE;

		ahc_start (ahc, target, scbp);
		return;
	}

    very_bad:
	bp->b_error = EIO;
	bp->b_flags |= B_ERROR;

	/*
	 *	Terminou a operação
	 */
    done:
	scbp->bp = NOBHEAD;

	ahc_busy--;

	EVENTDONE (&bp->b_done);

	/*
	 *	Chama "scstart" para começar outra operação na mesma unidade
	 */
	ahc->scbindex[hscb->tag] = NULL;
	scbp->flags = SCB_FREE;

	ahc_start (ahc, target, scbp);

}	/* end ahc_done */
#endif	BOOT

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
XPT_SET_TRAN_SETTINGS (struct ahc_softc *ahc, int target)
{
	struct ahc_tinfo_trio		*tinfo;
	ushort				*discenable, *tagenable;
	uint				update_type, mask;
	uint				bus_width, sync_period, sync_offset;
	int				pl;
	const struct ahc_syncrate	*syncrate;
	uint				ppr_options;
	uint				maxsync;

	tinfo = &ahc->ahc_info[target].transinfo;
	mask = 1 << target;

	update_type = AHC_TRANS_GOAL;

	discenable = &ahc->discenable;
	tagenable  = &ahc->tagenable;

	bus_width   = tinfo->user.width;
	sync_period = tinfo->user.period; 
	sync_offset = tinfo->user.offset; 
	
#if (0)	/*******************************************************/
	pl = splahc ();
#endif	/*******************************************************/
#ifndef	BOOT
	pl = splx (irq_pl_vec[ahc->irq]);
#endif	BOOT

	if (ahc->user_discenable & mask)
		*discenable |= mask;
	else
		*discenable &= ~mask;

	*tagenable &= ~mask;

	ahc_validate_width (ahc, /*tinfo limit*/NULL, &bus_width);
	ahc_set_width(ahc, target, bus_width, update_type, /*paused*/FALSE);

	/*
	 *	x
	 */
	if ((ahc->features & AHC_ULTRA2) != 0)
		maxsync = AHC_SYNCRATE_DT;
	else if ((ahc->features & AHC_ULTRA) != 0)
		maxsync = AHC_SYNCRATE_ULTRA;
	else
		maxsync = AHC_SYNCRATE_FAST;

	ppr_options = 0;

	if (sync_period <= 9)
		ppr_options = MSG_EXT_PPR_DT_REQ;

	syncrate = ahc_find_syncrate (ahc, &sync_period, &ppr_options, maxsync);

	ahc_validate_offset
		(ahc, NULL, syncrate, &sync_offset, MSG_EXT_WDTR_BUS_8_BIT);

	if (sync_offset == 0) 	/* We use a period of 0 to represent async */
	{
		sync_period = 0;
		ppr_options = 0;
	}

	if (ppr_options == MSG_EXT_PPR_DT_REQ && tinfo->user.transport_version >= 3)
	{
		tinfo->goal.transport_version	 = tinfo->user.transport_version;
		tinfo->current.transport_version = tinfo->user.transport_version;
	}
	
	ahc_set_syncrate
	(	ahc, target, syncrate, sync_period, sync_offset,
		ppr_options, update_type, /*paused*/FALSE
	);

	splx (pl);

}	/* end XPT_SET_TRAN_SETTINGS */





#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_get_tran_settings (struct ahc_softc *ahc, int our_id, char channel, struct ccb_trans_settings *cts)
{
	struct ahc_devinfo		devinfo;
	struct ahc_tinfo_trio	*targ_info;
	struct tmode_tstate		*tstate;
	struct ahc_transinfo		*tinfo;
	int				pl;

	ahc_compile_devinfo
	(	&devinfo, our_id, cts->ccb_h.target_id, cts->ccb_h.target_lun, channel, ROLE_UNKNOWN);

	targ_info = ahc_fetch_transinfo(ahc, devinfo.channel, devinfo.our_scsiid, devinfo.target, &tstate);
	
	if ((cts->flags & CCB_TRANS_CURRENT_SETTINGS) != 0)
		tinfo = &targ_info->current;
	else
		tinfo = &targ_info->user;
	
#if (0)	/*******************************************************/
	pl = splahc ();
#endif	/*******************************************************/
#ifndef	BOOT
	pl = splx (irq_pl_vec[ahc->irq]);
#endif	BOOT

	cts->flags &= ~(CCB_TRANS_DISC_ENB|CCB_TRANS_TAG_ENB);

	if ((cts->flags & CCB_TRANS_CURRENT_SETTINGS) == 0)
	{
		if ((ahc->user_discenable & devinfo.target_mask) != 0)
			cts->flags |= CCB_TRANS_DISC_ENB;

		if ((ahc->user_tagenable & devinfo.target_mask) != 0)
			cts->flags |= CCB_TRANS_TAG_ENB;
	}
	else
	{
		if ((tstate->discenable & devinfo.target_mask) != 0)
			cts->flags |= CCB_TRANS_DISC_ENB;

		if ((tstate->tagenable & devinfo.target_mask) != 0)
			cts->flags |= CCB_TRANS_TAG_ENB;
	}

	cts->sync_period = tinfo->period;
	cts->sync_offset = tinfo->offset;
	cts->bus_width = tinfo->width;
	
	splx (pl);

	cts->valid = CCB_TRANS_SYNC_RATE_VALID
		   | CCB_TRANS_SYNC_OFFSET_VALID
		   | CCB_TRANS_BUS_WIDTH_VALID;

	if (cts->ccb_h.target_lun != CAM_LUN_WILDCARD)
		cts->valid |= CCB_TRANS_DISC_VALID|CCB_TRANS_TQ_VALID;

	cts->ccb_h.status = CAM_REQ_CMP;

}	/* end ahc_get_tran_settings */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Request that the sequencer stop and wait, indefinitely, for it
 * to stop.  The sequencer will only acknowledge that it is paused
 * once it has reached an instruction boundary and PAUSEDIS is
 * cleared in the SEQCTL register.  The sequencer may use PAUSEDIS
 * for critical sections.
 */
void
pause_sequencer (struct ahc_softc *ahc)
{
	ahc_outb (ahc, HCNTRL, ahc->pause);

	/*
	 * Since the sequencer can disable pausing in a critical section, we
	 * must loop until it actually stops.
	 */
	while ((ahc_inb (ahc, HCNTRL) & PAUSE) == 0)
		/* vazio */;

	if ((ahc->features & AHC_ULTRA2) != 0) 	/* pause_bug_fix */
		ahc_inb (ahc, CCSCBCTL);

}	/* end pause_sequencer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Allow the sequencer to continue program execution.
 * We check here to ensure that no additional interrupt
 * sources that would cause the sequencer to halt have been
 * asserted.  If, for example, a SCSI bus reset is detected
 * while we are fielding a different, pausing, interrupt type,
 * we don't want to release the sequencer before going back
 * into our interrupt handler and dealing with this new
 * condition.
 */
void
unpause_sequencer (struct ahc_softc *ahc)
{
	if ((ahc_inb(ahc, INTSTAT) & (SCSIINT | SEQINT | BRKADRINT)) == 0)
		ahc_outb(ahc, HCNTRL, ahc->unpause);

}	/* end unpause_sequencer */

/*
 ****************************************************************
 * Get a free scb. If there are none, see if we can allocate a new SCB.
 ****************************************************************
 */
struct ahc_scb *
ahc_get_scb (struct ahc_softc *ahc)
{
	struct ahc_scb	*scbp;

	if ((scbp = ahc->free_scbs) == NULL)
		return (NULL);

	ahc->free_scbs = ahc->free_scbs->free_next;

	ahc->ahc_busy_scb++;

	scbp->bp = NOBHEAD;

#if (0)	/*******************************************************/
	if (CSWT (37))
		printf ("\r%s: SCBs alocados = %3d   ", ahc->name, ahc->ahc_busy_scb);
#endif	/*******************************************************/

	return (scbp);

}	/* end ahc_get_scb */

/*
 ****************************************************************
 *	Return an SCB resource to the free list			*
 ****************************************************************
 */
void
ahc_put_scb (struct ahc_softc *ahc, struct ahc_scb *scbp)
{       
	struct hardware_scb	*hscb;

	ahc->ahc_busy_scb--;

	hscb = scbp->hscb;

	/* Clean up for the next user */

	ahc->scbindex[hscb->tag] = NULL;
	scbp->flags = SCB_FREE;
	hscb->control = 0;

        scbp->free_next = ahc->free_scbs; ahc->free_scbs = scbp;

}	/* end ahc_put_scb */

#ifndef	BOOT
/*
 ****************************************************************
 *	Interrupção						*
 ****************************************************************
 */
void
ahc_intr (struct intr_frame frame)
{
	struct ahc_softc	*ahc = ahc_softc_vec[frame.if_unit];
	uint			intstat;

	intstat = ahc_inb (ahc, INTSTAT);

#ifdef	DEBUG
	printf ("%s: Interrupção: 0x%02X\n", ahc->name, intstat);
#endif	DEBUG

	if ((intstat & INT_PEND) == 0)
		return;

	if (intstat & CMDCMPLT)
	{
		ahc_outb (ahc, CLRINT, CLRCMDINT);

		/*
		 * Ensure that the chip sees that we've cleared
		 * this interrupt before we walk the output fifo.
		 * Otherwise, we may, due to posted bus writes,
		 * clear the interrupt after we finish the scan,
		 * and after the sequencer has added new entries
		 * and asserted the interrupt again.
		 */
		ahc_inb (ahc, INTSTAT);

		ahc_run_qoutfifo (ahc);
	}

	if (intstat & BRKADRINT)
	{
		ahc_handle_brkadrint (ahc);
	}

	if ((intstat & (SEQINT|SCSIINT)) != 0)
	{
		if ((ahc->features & AHC_ULTRA2) != 0) 	/* pause_bug_fix */
			ahc_inb (ahc, CCSCBCTL);
	}

	if ((intstat & SEQINT) != 0)
	{
		ahc_handle_seqint (ahc, intstat);
	}

	if ((intstat & SCSIINT) != 0)
	{
		ahc_handle_scsiint (ahc, intstat);
	}

}	/* end ahc_intr */
#endif	BOOT

/*
 ****************************************************************
 *	Obtém o nome do dispositivo a partir do alvo		*
 ****************************************************************
 */
const char *
ahc_get_dev_nm (struct ahc_softc *ahc, int target)
{
	return (ahc->ahc_info[target].info_scsi.scsi_dev_nm);

}	/* end ahc_get_dev_nm */

/*
 ****************************************************************
 *	Obtém o nome do dispositivo a partir do SCB		*
 ****************************************************************
 */
const char *
ahc_get_dev_nm_by_scb (struct ahc_softc *ahc, struct ahc_scb *scbp)
{
	return (ahc->ahc_info[scbp->hscb->scsiid >> 4].info_scsi.scsi_dev_nm);

}	/* end ahc_get_dev_nm_by_scb */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_search_wainting_selection_list (struct ahc_softc *ahc)
{
	char		curscbptr, next, prev;
	struct ahc_scb	*scbp;

#if (0)	/*******************************************************/
	struct	scb *scb;
	struct	scb *prev_scb;
	uint8_t qinstart;
	uint8_t qinpos;
	uint8_t qintail;
	uint8_t next, prev;
	uint8_t curscbptr;
	int	found;
	int	maxtarget;
	int	i;
	int	have_qregs;

#endif	/*******************************************************/

	/*
	 * Search waiting for selection list.
	 */
	curscbptr = ahc_inb (ahc, SCBPTR);
	next = ahc_inb (ahc, WAITING_SCBH);  /* Start at head of list. */
	prev = SCB_LIST_NULL;

#if (0)	/*******************************************************/
	while (next != SCB_LIST_NULL)
#endif	/*******************************************************/
	{
		char		scb_index;

		ahc_outb (ahc, SCBPTR, next);
		scb_index = ahc_inb (ahc, SCB_TAG);

		if (scb_index >= ahc->numscbs)
		{
			printf
			(	"Waiting List inconsistency. "
			       "SCB index == %d, yet numscbs == %d.",
			       scb_index, ahc->numscbs
			);
			panic ("for safety");
		}

		scbp = ahc->scbindex[scb_index];

		/*
		 *	We found an scb that needs to be acted on.
		 */
		if ((scbp->flags & SCB_ACTIVE) == 0)
			printf ("Inactive SCB in Waiting List\n");

		ahc_done (ahc, scbp);

		next = ahc_rem_wscb (ahc, next, prev);
	}

	ahc_outb (ahc, SCBPTR, curscbptr);

}	/* end ahc_search_wainting_selection_list */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Manipulate the waiting for selection list and return the
 * scb that follows the one that we remove.
 */
int
ahc_rem_wscb (struct ahc_softc *ahc, int scbpos, int prev)
{       
	int		curscb, next;

	/*
	 *	Select the SCB we want to abort and
	 *	pull the next pointer out of it.
	 */
	curscb = ahc_inb (ahc, SCBPTR);
	ahc_outb (ahc, SCBPTR, scbpos);
	next = ahc_inb (ahc, SCB_NEXT);

	/* Clear the necessary fields */

	ahc_outb (ahc, SCB_CONTROL, 0);

	ahc_add_curscb_to_free_list (ahc);

	/* update the waiting list */

	if (prev == SCB_LIST_NULL)
	{
		/* First in the list */

		ahc_outb (ahc, WAITING_SCBH, next); 

		/*
		 *	Ensure we aren't attempting to perform selection for this entry.
		 */
		ahc_outb (ahc, SCSISEQ, (ahc_inb (ahc, SCSISEQ) & ~ENSELO));
	}
	else
	{
		/*
		 *	Select the scb that pointed to us and update its next pointer.
		 */
		ahc_outb (ahc, SCBPTR, prev);
		ahc_outb (ahc, SCB_NEXT, next);
	}

	/*
	 *	Point us back at the original scb position.
	 */
	ahc_outb (ahc, SCBPTR, curscb);

	return (next);

}	/* end ahc_rem_wscb */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Add the SCB as selected by SCBPTR onto the on chip list of
 * free hardware SCBs.  This list is empty/unused if we are not
 * performing SCB paging.
 */
void
ahc_add_curscb_to_free_list (struct ahc_softc *ahc)
{
	/*
	 *	Invalidate the tag so that our abort routines don't think it's active.
	 */
	ahc_outb (ahc, SCB_TAG, SCB_LIST_NULL);

	if ((ahc->flags & AHC_PAGESCBS) != 0)
	{
		ahc_outb (ahc, SCB_NEXT, ahc_inb (ahc, FREE_SCBH));
		ahc_outb (ahc, FREE_SCBH, ahc_inb (ahc, SCBPTR));
	}

}	/* end ahc_add_curscb_to_free_list */
