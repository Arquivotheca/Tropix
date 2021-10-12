/*
 ****************************************************************
 *								*
 *			aic7_boot.c				*
 *								*
 *	Função de acesso ao controlador				*
 *								*
 *	Versão	4.0.0, de 04.04.01				*
 *		4.6.0, de 14.04.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

#include <stddef.h>

#include <common.h>
#include <sync.h>
#include <scb.h>
#include <region.h>

#include <pci.h>
#include <frame.h>
#include <disktb.h>
#include <devmajor.h>
#include <scsi.h>
#include <kfile.h>
#include <devhead.h>
#include <bhead.h>
#include <cdio.h>

#include <signal.h>
#include <uproc.h>
#include <uerror.h>

#include <../aic/aic7xxx_seq.h>
#include <../aic/aic7xxx.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
extern void		ahc_handle_scsiint (struct ahc_softc *ahc, uint intstat);

/*
 ****************************************************************
 *	Procura os alvos desta controladora			*
 ****************************************************************
 */
int
ahc_get_targets (struct ahc_softc *ahc)
{
	AHCINFO		*sp;
	int		target, max_target;
	static DISKTB	d = { "sd?", 0, 0, 0, 0, 0, BLSHIFT, MAKEDEV (SC_MAJOR, 0), 0, 0 };

	/*
	 *	Inicialização
	 */
	d.p_unit   = ahc->unit;

	DELAY (500 * 1000);	/* Para as unidades ficarem prontas */

	/*
	 *	Obtém o número de alvos
	 */
	if (ahc->features & AHC_WIDE)
		max_target = 16;
	else
		max_target = 8;

	/*
	 *	Verifica qual dos alvos está presente
	 */
	for (target = 0, sp = ahc->ahc_info; target < max_target; target++, sp++)
	{
		if (target == ahc->our_id)
			continue;

		sp->info_scsi.scsi_target = target;

		d.p_name[2]  = 'a' + scsi_index;
	   /***	d.p_dev	     = MAKEDEV (SC_MAJOR, 0); ***/
		d.p_target   = target;

		sp->info_scsi.scsi_disktb = &d;

		sp->info_scsi.scsi_dev_nm = d.p_name;
		sp->info_scsi.scsi_cmd	  = ahc_cmd;

		scsi_attach (&sp->info_scsi);

		d.p_blshift = sp->info_scsi.scsi_blshift;

		if (sp->info_scsi.scsi_is_present)
		{
			if (scsi_index >= MAX_TARGETs)
				{ printf ("Dispositivos SCSI excessivos\n"); break; }

			if ((sp->info_scsi.scsi_disktb = malloc_byte (sizeof (DISKTB))) == NODISKTB)
				{ printf ("Memória esgotada\n"); break; }

			memmove (sp->info_scsi.scsi_disktb, &d, sizeof (DISKTB));

			sp->info_scsi.scsi_dev_nm = ((DISKTB *)sp->info_scsi.scsi_disktb)->p_name;

			scsi_ptr[scsi_index++] = &sp->info_scsi;
		}
	}

	return (0);

}	/* end ahc_get_targets */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
ahc_open (const DISKTB *up)
{
	struct ahc_softc	*ahc;
	SCSI			*sp;

	/*
	 *	Verifica a validade dos argumentos
	 */
	if ((ahc = ahc_softc_vec[up->p_unit]) == NULL)
	{
		printf ("%s: Unidade inválida\n", up->p_name);
		return (-1);
	}

	sp = &ahc->ahc_info[up->p_target].info_scsi;

	if (!sp->scsi_is_present)
	{
		printf ("%s: Alvo inválido\n", up->p_name);
		return (-1);
	}

	return (scsi_open (sp));

}	/* end ahc_open */

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
	{
		printf ("ahc: Comando SCSI com mais de 12 bytes\n");
		return (-1);
	}

	bp->b_flags |= B_STAT;

	return (ahc_strategy (bp));

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
	int			unit, target, i;
	char			*cp;
	uint			intstat;
	const DISKTB		*up;
	int			status = 0;

	/*
	 *	Pequeno prólogo
	 */
	up = bp->b_disktb;

	target = up->p_target; unit = up->p_unit;

	ahc = ahc_softc_vec[unit];
	sp = &ahc->ahc_info[target];

	bp->b_flags &= ~B_ERROR;
	bp->b_done = 0;

	/*
	 *	Verifica a validade dos argumentos
	 */
#if (0)	/*******************************************************/
	if ((unsigned)target >= NTARGET)
	{
		printf ("ahc: Alvo %d inválido\n", target);
		return (-1);
	}
#endif	/*******************************************************/

	if (up->p_size != 0 && (unsigned)bp->b_blkno >= up->p_size)
	{
		printf ("%s: Bloco inválido: %d\n", up->p_name, bp->b_blkno);
		return (-1);
	}

	bp->b_ptr	= sp;
	bp->b_retry_cnt = NUM_TRY;

	/*
 	 *	Prepara o comando SCSI de leitura/escrita
	 */
	if ((bp->b_flags & B_STAT) == 0)
	{
		bp->b_cmd_len = 10;

		cp = bp->b_cmd_txt;

		if ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)	/* Write */
			cp[0] = SCSI_CMD_WRITE_BIG;
		else							/* Read */
			cp[0] = SCSI_CMD_READ_BIG;

		cp[1] = 0;
#if (0)	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv (bp->b_blkno + up->p_offset);
#endif	/*******************************************************/
		*(long *)&cp[2] = long_endian_cv
				((bp->b_blkno + up->p_offset) >> (sp->info_scsi.scsi_blshift - BLSHIFT));
		cp[6] = 0;
#if (0)	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count / sp->info_scsi.scsi_blsz);
#endif	/*******************************************************/
		*(short *)&cp[7] = short_endian_cv (bp->b_base_count >> sp->info_scsi.scsi_blshift);
		cp[9] = 0;
	}

	/*
	 *	Inicia a operação
	 */
	if ((scbp = ahc_get_scb (ahc)) != NULL)
	{
		scbp->bp = bp;	/* Fornece o BHEAD no próprio SCB */

		ahc_start (ahc, target, scbp);
	}
	else
	{
		printf ("%s: Não consegui um SCB em \"strategy\"\n", ahc->name);
		return (-1);
	}

	/*
	 *	Espera o final da operação
	 */
    wait_again:
	for (i = 10000000; /* abaixo */; i--)
	{
		if (i <= 0)
		{
			printf ("%s: Não veio o final da operação\n", up->p_name);
			goto wait_again;
		}

		intstat = ahc_inb (ahc, INTSTAT);

		if (intstat & INT_PEND)
			break;

		DELAY (10);
	}

#undef	DEBUG
#ifdef	DEBUG
	printf ("%s: Veio interrupção: 0x%02X\n", up->p_name, intstat);
#endif	DEBUG

	/*
	 *	Analisa a interrupção
	 */
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

		if (bp->b_flags & B_ERROR)
			status = -1;
	}

	if (intstat & BRKADRINT)
	{
		printf ("%s: Recebi uma interrupção BRKADRINT\n", up->p_name);
	}

	if ((intstat & (SEQINT|SCSIINT)) != 0)
	{
		if ((ahc->features & AHC_ULTRA2) != 0) 	/* pause_bug_fix */
			ahc_inb (ahc, CCSCBCTL);
	}

	if ((intstat & SEQINT) != 0)		/* Obtém o SENSE */
	{
		ahc_handle_seqint (ahc, intstat);
	}

	if ((intstat & SCSIINT) != 0)		/* Unidade inexistentes */
	{
		ahc_handle_scsiint (ahc, intstat);
		status = -1;
		bp->b_done++;
	}

	if (!bp->b_done)
		goto wait_again;

	ahc_put_scb (ahc, scbp);

	return (status);

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
	struct hardware_scb	*hscb;	
	struct ahc_dma_seg	*seg;
	ushort			mask;

	/*
	 *	Procura o novo pedido
	 */
	sp = &ahc->ahc_info[target];

	bp = scbp->bp;

	/*
	 *	Põe o comando no SCB
	 */
	hscb = ahc->next_queued_scb->hscb;	/* Usa o HSCB que o sequenciador está esperando */

	memmove (hscb->shared_data.cdb, bp->b_cmd_txt, bp->b_cmd_len);
	hscb->cdb_len = bp->b_cmd_len;

	/*
	 *	Prepara a tabela de DMA
	 */
	if (bp->b_base_count)
	{
		seg = &scbp->sg_list[0];

		hscb->dataptr	= seg->addr = (unsigned)bp->b_addr;
		hscb->datacnt	= seg->len  = bp->b_base_count | AHC_DMA_LAST_SEG;

		hscb->sgptr	= (unsigned)(seg + 1) | SG_FULL_RESID;

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
	 *	Teve erro
	 */
   /*** bad: **/
	if (--bp->b_retry_cnt > 0)
	{
		ahc->scbindex[hscb->tag] = NULL;
		scbp->flags = SCB_FREE;

		ahc_start (ahc, target, scbp);
		return;
	}

    very_bad:
	bp->b_flags |= B_ERROR;

	/*
	 *	Terminou a operação
	 */
    done:
	ahc->scbindex[hscb->tag] = NULL;
	scbp->flags = SCB_FREE;

	bp->b_done++;
	return;

}	/* end ahc_done */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
ahc_ctl (const DISKTB *up, int cmd, int arg)
{
	struct ahc_softc	*ahc;
	AHCINFO			*sp;
	
	ahc = ahc_softc_vec[up->p_unit];
	sp = &ahc->ahc_info[up->p_target];

	return (scsi_ctl (&sp->info_scsi, up, cmd, arg));

}	/* end ahc_ctl */

/*
 ****************************************************************
 *	Para compatibilizar com o núcleo			*
 ****************************************************************
 */
void splx () { }

void
panic (const char *fmt, int var)
{
	printf (fmt, var);
}
