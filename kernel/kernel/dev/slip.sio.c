/*
 ****************************************************************
 *								*
 *			slip.sio.c				*
 *								*
 *	Driver SLIP para linhas seriais com NS16450 ou NS16550	*
 *								*
 *	Versão	2.3.12, de 29.07.91				*
 *		4.5.00, de 17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/frame.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/sio.h"
#include "../h/tty.h"
#include "../h/ioctl.h"
#include "../h/intr.h"
#include "../h/devmajor.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
#define	t_fifosz t_canal	/* Funções alternativas */
#define	t_reserv t_vterm

entry TTY	slip[NSIO];	/* Um para cada linha serial */

extern VECDEF	*orig_vp[NSIO];

/*
 ****** Caracteres do protocolo SLIP ****************************
 */
#define	END		0300
#define	ESC		0333
#define	ESC_END		0334
#define	ESC_ESC		0335

/*
 ****** Estrutura das filas de ITBLOCKS *************************
 *
 *	Deve ser idêntica à estrutura CLIST (ver "tty.h")
 */
typedef struct
{
	LOCK	q_lock;		/* Semaforo da ITQ */
	char	q_reser0;
	short	q_count;

	ITBLOCK	*q_first;	/* Ponteiro para o primeiro ITBLOCK */
	ITBLOCK	*q_last;	/* Ponteiro para o último ITBLOCK */

}	ITQ;

/*
 ******	Protótipos **********************************************
 */
void		slipint (struct intr_frame);

extern void	sioint (struct intr_frame);
extern void	sioparam (const TTY *);

/*
 ****************************************************************
 *	 Função de Open						*
 ****************************************************************
 */
void
slipopen (dev_t dev, OFLAG oflag)
{
	TTY		*tp;
	int		unit, port, i, irq;
	VECDEF		*vp;

	/*
	 *	Prólogo
	 */
	if ((unit = MINOR (dev)) >= NSIO)
		{ u.u_error = ENXIO; return; }

	if (sio_lock[unit] || (oflag & O_LOCK) && sio_nopen[unit])
		{ u.u_error = EBUSY; return; }

   	if (sio_nopen[unit] > 0)
	   	{ sio_nopen[unit]++; return; }

	/*
	 *	Verifica se a unidade está ativa
	 */
	if (sio_config[unit].sio_unit_alive == 0)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Analisa a interrupção
	 */
	irq = scb.y_sio_irq[unit];

	vp = &vecdef[irq][0];

	for (i = 0; /* abaixo */; i++, vp++)
	{
		if (i >= VECLINESZ - 1)
		{
			printf
			(	"pppopen: Não encontrei a entrada de interrupção "
				"da unidade %d, irq = %d\n",
				unit, irq
			);
			u.u_error = EINVAL;
			return;
		}

		if ((vp->vec_func == sioint || vp->vec_func == slipint) && vp->vec_unit == unit)
			break;
	}

	orig_vp[unit] = vp;


	if   (vp->vec_func == sioint && sio_nopen[unit] == 0)
	{
	   /***	vp->vec_unit = unit;	***/
	   	vp->vec_func = slipint;
	}
#if (0)	/*******************************************************/
	elif (vp->vec_func != slipint)
	{
		u.u_error = EBUSY; return;
	}
#endif	/*******************************************************/

	/*
	 *	Inicializa os registros da unidade
	 */
	tp = &slip[unit];

#if (0)	/*******************************************************/
	splsio ();
#endif	/*******************************************************/
	splx (irq_pl_vec[irq]);

	if ((tp->t_state & ISOPEN) == 0)
	{
		/*
		 *	Prepara os Parametros
		 */
	   	port = scb.y_sio_port[unit];
	   	tp->t_addr  = (void *)port;
		tp->t_irq   = irq;

		tp->t_iflag = ICRNL|IXON;
		tp->t_oflag = OPOST|ONLCR;
		tp->t_cflag = B9600|CS8|CLOCAL;
		tp->t_lflag = ISO|ICOMM;

		if (sio_config[unit].sio_has_fifo)
			{ tp->t_state |=  HASFIFO; tp->t_fifosz = 16; }
		else
			{ tp->t_state &= ~HASFIFO; tp->t_fifosz = 1; }

		/* Inicializa a unidade */

		write_port (0, SIO_LINE_CTL_REG_PORT);
		write_port (0, SIO_INT_ID_PORT);

		DELAY (100);

		write_port
		(	FIFO_ENABLE|FIFO_RCV_RST|FIFO_XMT_RST|FIFO_TRIGGER_14,
			SIO_FIFO_CTL_PORT
		);

		DELAY (100);

		write_port (0, SIO_INT_ENABLE_REG_PORT);
		write_port (MCR_IENABLE, SIO_MODEM_CTL_PORT);

		sioparam (tp);

		EVENTCLEAR (&tp->t_inqnempty);
		EVENTSET   (&tp->t_outqnfull);
#if (0)	/*******************************************************/
		EVENTCLEAR (&tp->t_outqstart);
#endif	/*******************************************************/
		EVENTSET   (&tp->t_outqempty);
		CLEAR	   (&tp->t_obusy);

		tp->t_dev  = dev;

		strcpy (tp->t_termio.c_name, u.u_name);

		tp->t_state |= ISOPEN;
	}

	/*
	 *	Epílogo
	 */
	sio_nopen[unit]++;

	if (oflag & O_LOCK)
		sio_lock[unit] = 1;

	spl0 ();

}	/* end slipopen */

/*
 ****************************************************************
 *	Função de Close						*
 ****************************************************************
 */
void
slipclose (dev_t dev, int flag)
{
	TTY		*tp;
	VECDEF		*vp;
	int		unit, port;
	ITBLOCK		*bp, *nbp;
	int		i;

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev);

	if (--sio_nopen[unit] > 0)
		return;

	/*
	 *	Fecha a unidade
	 */
	tp = &slip[unit]; port = (int)tp->t_addr;

	EVENTWAIT (&tp->t_outqempty, PITNETOUT);

	i = read_port (SIO_LINE_CTL_REG_PORT) & ~CFCR_SBREAK;
	write_port (i, SIO_LINE_CTL_REG_PORT);
	write_port (0, SIO_INT_ENABLE_REG_PORT);
	write_port (0, SIO_MODEM_CTL_PORT);

	bp = ((ITQ *)&tp->t_inq)->q_first;

	while (bp != NOITBLOCK)
	{
		nbp = bp->it_forw;
		bp->it_in_driver_queue = 0; put_it_block (bp);
		bp = nbp;
	}

	/*
	 *	Restaura o vetor de interrupções original
	 */
	if ((vp = orig_vp[unit]) == NULL)
		printf ("slipclose: Não tinha a entrada original de interrupção\n");
	else
		vp->vec_func = sioint;

#if (0)	/*******************************************************/
	vp = &vecdef[scb.y_sio_irq[unit]];

   	vp->vec_func = sioint;
#endif	/*******************************************************/

	/*
	 *	Epílogo
	 */
	sio_lock[unit] = 0;

	tp->t_state = 0;

}	/* end slipclose */

/*
 ****************************************************************
 *	Função  de Write					*
 ****************************************************************
 */
void
slipwrite (IOREQ *iop)
{
	int		unit, port;
	TTY		*tp;
	ITQ		*qp;
	ITBLOCK		*bp;
	int		tas_ret;

	bp = (ITBLOCK *)iop->io_area;

   	bp->it_escape_char = 0;
	bp->it_area  = bp->it_u_area;
	bp->it_count = bp->it_u_count;

	/*
	 *	O "spl" é para não deixar uma interrupção
	 *	da entrada iniciar a saída
	 */
	unit = MINOR (iop->io_dev);
	tp = &slip[unit];
	qp = (ITQ *)&tp->t_outq;
	port = (int)tp->t_addr;

#if (0)	/*******************************************************/
	splsio ();
#endif	/*******************************************************/
	splx (irq_pl_vec[tp->t_irq]);

	/*
	 *	Examina o modo -CLOCAL (MODEM, 5 fios)
	 *
	 *	Se não tiver CTS, descarta o datagrama
	 */
	if ((tas_ret = TAS (&tp->t_obusy)) >= 0)
	{
		if ((tp->t_cflag & CLOCAL) == 0)
		{
			if ((read_port (SIO_MODEM_STATUS_PORT) & MSR_CTS) == 0)
			{
				CLEAR (&tp->t_obusy);

				bp->it_in_driver_queue = 0;

				if (bp->it_free_after_IO)
					put_it_block (bp);

				spl0 ();
				return;
			}

		}	/* if (! CLOCAL) */

	}	/* if (TAS (&tp->t_obusy) >= 0) */

	/*
	 *	Põe na fila do DRIVER
	 */
	SPINLOCK (&qp->q_lock);

	if (qp->q_first == NOITBLOCK)
		qp->q_first = bp;
	else
		qp->q_last->it_forw = bp;

	qp->q_last = bp;
	bp->it_forw = NOITBLOCK;

	EVENTCLEAR (&tp->t_outqempty);

	SPINFREE (&qp->q_lock);

	/*
	 *	Verifica se precisa iniciar a operação
	 */
	if (tas_ret < 0)
		{ spl0 (); return; }

	/*
	 *	Inicia a operação com o caracter END
	 */
	while ((read_port (SIO_LINE_STATUS_PORT) & LSR_TXRDY) == 0)
		/* vazio */;

	write_port (END, SIO_DATA_PORT);

	spl0 ();

}	/* end slipwrite */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
slipctl (dev_t dev, int cmd, void *arg, ...)
{
	TTY		*tp;
	int		unit, port;
	int		ret;

	tp = &slip[unit = MINOR (dev)]; port = (int)tp->t_addr;

	/*
	 *	Tratamento especial para certos comandos
	 */
	switch (cmd)
	{
		/*
		 *	Identifica este dispositivo próprio para datagramas IP
		 */
	    case TCINTERNET:
		return (0);

		/*
		 *	Atribui o tamanho a usar do FIFO
		 *
		 *	Repare que se valor novo == 0, NÃO modifica,
		 *	e (apenas) fornece o valor atual
		 */
	    case TCFIFOSZ:
		if ((int)arg == 0)
			return (tp->t_fifosz);

		if ((tp->t_state & HASFIFO) == 0 && (int)arg != 1)
			{ u.u_error = EINVAL; return (-1); }

		if ((unsigned)arg > 16)
			{ u.u_error = EINVAL; return (-1); }

		ret = tp->t_fifosz; tp->t_fifosz = (int)arg;

		return (ret);

		/*
		 *	get CLEAR TO SEND
		 */
	    case TCGETCTS:
		return (read_port (SIO_MODEM_STATUS_PORT) & MSR_CTS);

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
   /***	SLEEPLOCK (&tp->t_olock, PITNETOUT); ***/

	if ((ret = ttyctl (tp, cmd, arg)) >= 0)
	{
		if (cmd == TCSETS || cmd == TCSETAW || cmd == TCSETAF)
#if (0)	/*******************************************************/
			{ splsio (); sioparam (tp); spl0 (); }
#endif	/*******************************************************/
			{ splx (irq_pl_vec[tp->t_irq]); sioparam (tp); spl0 (); }
	}

   /***	SLEEPFREE (&tp->t_olock); ***/

	return (ret);

}	/* end slipctl */

/*
 ****************************************************************
 *	Função de Interrupção					*
 ****************************************************************
 */
void
slipint (struct intr_frame frame)
{
	int		unit, port, c, i;
	TTY		*tp;
	ITQ		*qp;
	ITBLOCK		*bp;
	static int	no_block_avail = 0;

	/*
	 *	Prólogo
	 */
	unit = frame.if_unit; tp = &slip[unit];

	if ((port = (int)tp->t_addr) == 0)
		return;

	/*
	 *	Seção de leitura
	 */
    again:
	qp = (ITQ *)&tp->t_inq;

	while (read_port (SIO_LINE_STATUS_PORT) & LSR_RCV_MASK)
	{
		c = read_port (SIO_DATA_PORT);

		if ((bp = qp->q_first) == NOITBLOCK)
		{
			if (no_block_avail)
			{
				if (c == END)
					no_block_avail = 0;
				else
					continue;
			}

			if ((bp = get_it_block (IT_IN)) == NOITBLOCK)
			{
				no_block_avail++;
#ifdef	MSG
				printf ("%g: Nao obtive bloco\n");
#endif	MSG
				continue;
			}

			qp->q_first = bp;

		   	bp->it_escape_char = 0;
			bp->it_area    =
			bp->it_u_area  = bp->it_frame + RESSZ;
			bp->it_u_count = DGSZ;
		}

		if   (c == END)
		{
			if (bp->it_u_count == DGSZ)
				continue;

			bp->it_u_count = DGSZ - bp->it_u_count;
			bp->it_ether_dev = 0;
			bp->it_ppp_dev   = 0;

			qp->q_first = NOITBLOCK;

			wake_up_in_daemon (IN_BP, bp);

			continue;
		}
		elif (c == ESC)
		{
			bp->it_escape_char++;
			continue;
		}

		if (bp->it_escape_char)
		{
			bp->it_escape_char = 0;

			if   (c == ESC_END)
				c = END;
			elif (c == ESC_ESC)
				c = ESC;
		}

		if (bp->it_u_count-- > 0)
			*bp->it_area++ = c;
		else
			bp->it_u_count = 0;

	}	/* end caracter disponível */

	/*
	 *	Verifica se temos CTS
	 */
	if ((read_port (SIO_MODEM_STATUS_PORT) & MSR_CTS) == 0)
	{
		if ((tp->t_cflag & CLOCAL) == 0)
			goto more_intr;
	}

	/*
	 *	Seção de Escrita
	 */
	if ((read_port (SIO_LINE_STATUS_PORT) & LSR_TXRDY) == 0)
		goto more_intr;

	qp = (ITQ *)&tp->t_outq;

	if ((bp = qp->q_first) == NOITBLOCK)
		{ CLEAR (&tp->t_obusy); goto more_intr; } /* "Não deve acontecer" */

	for (i = tp->t_fifosz; i > 0; i--)
	{
		/*
		 *	Obtém um caractere
		 */
		if   (bp->it_escape_char)	/* Tinha vindo um ESC */
		{
			c = bp->it_escape_char;
			bp->it_escape_char = 0;
			goto sendchar;
		}
		elif (bp->it_count-- > 0)	/* Tem mais bytes */
		{
			c = *bp->it_area++;
		}
		elif (bp->it_count-- == -1)	/* Acabou o datagrama */
		{
			c = END; goto sendchar;
		}
		else /* it_count == -4 */ 	/* Após o END final */
		{
			SPINLOCK (&qp->q_lock);

			if (bp->it_forw == NOITBLOCK)
			{
				qp->q_first = NOITBLOCK;
				qp->q_last  = NOITBLOCK;

				CLEAR (&tp->t_obusy);
				EVENTDONE (&tp->t_outqempty);
				SPINFREE (&qp->q_lock);

				bp->it_in_driver_queue = 0;

				if (bp->it_free_after_IO)
					put_it_block (bp);

				break;
			}
			else
			{
				qp->q_first = bp->it_forw;

				SPINFREE (&qp->q_lock);

				bp->it_in_driver_queue = 0;

				if (bp->it_free_after_IO)
					put_it_block (bp);

				bp = qp->q_first;

				bp->it_escape_char = 0;

				bp->it_count--;
				c = *bp->it_area++;
			}
		}

		/*
		 *	Processa os escapes do SLIP
		 */
		if   (c == END)
			{ c = ESC; bp->it_escape_char = ESC_END; }
		elif (c == ESC)
			{ c = ESC; bp->it_escape_char = ESC_ESC; }

		/*
		 *	Finalmente, escreve o caractere
		 */
	    sendchar:
		write_port (c, SIO_DATA_PORT);

	}	/* for (obtendo caracteres para escrever) */

	/*
	 *	Verifica se ainda tem motivo de interrupção
	 */
    more_intr:
	if ((read_port (SIO_INT_ID_PORT) & IIR_IMASK) != IIR_NOPEND)
		goto again;

}	/* end slipint */
