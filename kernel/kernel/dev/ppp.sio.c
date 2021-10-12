/*
 ****************************************************************
 *								*
 *			ppp.sio.c				*
 *								*
 *	Driver PPP para linhas seriais com NS16450 ou NS16550	*
 *								*
 *	Versão	3.0.0, de 28.06.96				*
 *		4.5.0, de 17.06.03				*
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
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/devmajor.h"
#include "../h/uproc.h"
#include "../h/itnet.h"
#include "../h/ppp.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
#define	t_fifosz t_canal		/* Funções alternativas */
#define	t_reser	t_vterm

entry TTY		ppp[NSIO];	/* Um para cada linha serial */

entry PPP_STATUS	ppp_status[NSIO];

#define NSL		3

entry SLCOMPRESS	slcompress[NSL]; /* Dados das compressões */

entry int		ppp_in_busy,	/* Para controlar o uso */
			ppp_out_busy;

entry VECDEF		*orig_vp[NSIO];

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
void		pppint (struct intr_frame);
void		lcp_proc (ITBLOCK *, const char *);
void		lcp_reply_echo_request (ITBLOCK *bp, const char *area);
void		ppp_lcp_packet_snd (ITBLOCK *, int, int, int);
int		pppfcs (int, const char *, int);

extern void	sioint (struct intr_frame);
extern void	sioparam (const TTY *);
extern int	sl_uncompress_tcp (ITBLOCK *, int, SLCOMPRESS *);
extern int	sl_compress_tcp (ITBLOCK *, const PPP_STATUS *);
extern void	sl_compress_init (SLCOMPRESS *);

/*
 ****************************************************************
 *	 Função de Open						*
 ****************************************************************
 */
void
pppopen (dev_t dev, OFLAG oflag)
{
	TTY		*tp;
	int		unit, port, irq, i;
	VECDEF		*vp;

	/*
	 *	Prólogo
	 */
	if ((unit = MINOR (dev)) >= NSIO)
		{ u.u_error = ENXIO; return; }

	if (sio_lock[unit] || (oflag & O_LOCK) && sio_nopen[unit])
		{ u.u_error = EBUSY; return; }

	tp = &ppp[unit];

	if ((tp->t_state & ISOPEN) && sio_nopen[unit] > 0)
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

		if ((vp->vec_func == sioint || vp->vec_func == pppint) && vp->vec_unit == unit)
			break;
	}

	orig_vp[unit] = vp;

   	if   (vp->vec_func == sioint && sio_nopen[unit] == 0)
	{
	   /***	vp->vec_unit = unit;	***/
	   	vp->vec_func = pppint;
	}
#if (0)	/*******************************************************/
	elif (vp->vec_func != pppint)
	{
		u.u_error = EBUSY; return;
	}
#endif	/*******************************************************/

	/*
	 *	Abre a unidade
	 */
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

		/*
		 *	Inicializa a unidade
		 */
	   	if (sio_nopen[unit] == 0)
		{
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
		}

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

	spl0 ();

	/*
	 *	Epílogo
	 */
	sio_nopen[unit]++;

	if (oflag & O_LOCK)
		sio_lock[unit] = 1;

}	/* end pppopen */

/*
 ****************************************************************
 *	Função de Close						*
 ****************************************************************
 */
void
pppclose (dev_t dev, int flag)
{
	TTY		*tp;
	VECDEF		*vp;
	int		unit, port;
	ITBLOCK		*bp, *nbp;
	PPP_STATUS	*sp;
	int		i;

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev); tp = &ppp[unit]; port = (int)tp->t_addr;

	if (flag == 0)
	{
		EVENTWAIT (&tp->t_outqempty, PITNETOUT);

#if (0)	/*******************************************************/
		splsio ();
#endif	/*******************************************************/
		splx (irq_pl_vec[tp->t_irq]);

		bp = ((ITQ *)&tp->t_inq)->q_first;

		while (bp != NOITBLOCK)
		{
			nbp = bp->it_forw;
			bp->it_in_driver_queue = 0; put_it_block (bp);
			bp = nbp;
		}

		/*
		 *	Desaloca a tabela de compressão, se houver
		 */
		sp = &ppp_status[unit];

		if (sp->ppp_slcompress_ptr != (SLCOMPRESS *)NULL)
			sp->ppp_slcompress_ptr->busy = 0;

		/*
		 *	Retira a entrada da tabela de rotas
		 */
		if (sp->ppp_my_addr_added)
		{
			if (del_route_entry (ppp_status[unit].snd.ppp_addr) < 0)
				printf ("pppclose: NÃO consegui remover a entrada ROUTE\n");
		}

		sp->ppp_my_addr_added = 0;

		/*
		 *	Restaura o vetor de interrupções original
		 */
		if ((vp = orig_vp[unit]) == NULL)
			printf ("pppclose: Não tinha a entrada original de interrupção\n");
		else
			vp->vec_func = sioint;

		tp->t_state = 0;

		spl0 ();
	}

	if (--sio_nopen[unit] <= 0)
	{
#if (0)	/*******************************************************/
		splsio ();
#endif	/*******************************************************/
		splx (irq_pl_vec[tp->t_irq]);

		i = read_port (SIO_LINE_CTL_REG_PORT) & ~CFCR_SBREAK;
		write_port (i, SIO_LINE_CTL_REG_PORT);
		write_port (0, SIO_INT_ENABLE_REG_PORT);
		write_port (0, SIO_MODEM_CTL_PORT);

		spl0 ();

		sio_lock[unit] = 0;
	}
		
}	/* end pppclose */

/*
 ****************************************************************
 *	Função  de Write					*
 ****************************************************************
 */
void
pppwrite (IOREQ *iop)
{
	int		unit, port;
	TTY		*tp;
	ITQ		*qp;
	ITBLOCK		*bp = (ITBLOCK *)iop->io_area;
	PPP_STATUS	*sp;
	char 		*cp;
	int		tas_ret, fcs, proto = PPP_IP;

	/*
	 *	Verifica se deve comprimir o datagrama
	 */
	unit = MINOR (iop->io_dev); sp = &ppp_status[unit]; tp = &ppp[unit];

   	bp->it_escape_char = 0;

	bp->it_area  = bp->it_u_area;		/* Área do Cabeçalho (+ dados) */
	bp->it_count = bp->it_u_count;

	bp->it_data_area  = NOSTR;		/* Área dos Dados */
	bp->it_data_count = 0;

	if (((IP_H *)bp->it_area)->ip_proto == TCP_PROTO && sp->snd.ppp_vj_compression)
		proto = sl_compress_tcp (bp, sp);

	/*
	 *	Coloca o cabeçalho PPP
	 */
	cp = bp->it_area;

	*--cp = proto;			/* IP, VJ_COM, VJ_UN Protocol */

	if (!sp->snd.ppp_proto_field_compression)
		*--cp = proto >> 8;

	if (!sp->snd.ppp_addr_field_compression)
	{
		*--cp = PPP_CTL;	/* Control */
		*--cp = PPP_ADDR;	/* Address */
	}

	if (cp < bp->it_frame)
		printf ("%g: UNDERFLOW da área do ITBLOCK!\n");

	bp->it_count += bp->it_area - cp;
	bp->it_area   = cp;

	/*
	 *	Calcula o FCS
	 */
	fcs = pppfcs (0xFFFF, cp, bp->it_count);

	if (bp->it_data_area != NOSTR)
		fcs = pppfcs (fcs, bp->it_data_area, bp->it_data_count);

	fcs = ~fcs;

	if (bp->it_data_area == NOSTR)
	{
		cp += bp->it_count;
		bp->it_count += 2;		/* Incorpora o CRC */
	}
	else	/* Tem Cabeçalho VJ */
	{
		cp = bp->it_data_area + bp->it_data_count;
		bp->it_data_count += 2;		/* Incorpora o CRC */
	}

	*cp++ = fcs; *cp++ = fcs >> 8;

#ifdef	MSG
	if (CSWT (11))
		printf ("pppwrite: count = %d, FCS = %04x\n", bp->it_count + bp->it_data_count, fcs);
#endif	MSG

	/*
	 *	O "spl" é para não deixar uma interrupção
	 *	da entrada iniciar a saída
	 */
	qp = (ITQ *)&tp->t_outq; port = (int)tp->t_addr;

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

	ppp_out_busy++;

	EVENTCLEAR (&tp->t_outqempty);

	SPINFREE (&qp->q_lock);

	/*
	 *	Verifica se precisa iniciar a operação
	 */
	if (tas_ret < 0)
		{ spl0 (); return; }

	/*
	 *	Inicia a operação com o caracter "FLAG"
	 */
	while ((read_port (SIO_LINE_STATUS_PORT) & LSR_TXRDY) == 0)
		/* vazio */;

	write_port (PPP_FLAG, SIO_DATA_PORT);

	spl0 ();

}	/* end pppwrite */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
pppctl (dev_t dev, int cmd, void *arg, ...)
{
	TTY		*tp;
	int		unit, port;
	VECDEF		*vp;
	PPP_STATUS	*sp;
	int		ret;
	SLCOMPRESS	*comp, *last_comp;
	static char	dev_nm[8] = "ppp00?";

	tp = &ppp[unit = MINOR (dev)]; port = (int )tp->t_addr;

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
	    {
#define	new_sz	(int)arg

		if (new_sz == 0)
			return (tp->t_fifosz);

		if ((tp->t_state & HASFIFO) == 0 && new_sz != 1)
			{ u.u_error = EINVAL; return (-1); }

		ret = tp->t_fifosz;

		if   ((unsigned)new_sz > 16)
			{ u.u_error = EINVAL; return (-1); }
		elif (new_sz < 4)
			tp->t_fifosz = 1;
		elif (new_sz < 8)
			tp->t_fifosz = 4;
		elif (new_sz < 14)
			tp->t_fifosz = 8;
		else
			tp->t_fifosz = 14;

#if (0)	/*******************************************************/
		{ splsio (); sioparam (tp); spl0 (); }
#endif	/*******************************************************/
		{ splx (irq_pl_vec[tp->t_irq]); sioparam (tp); spl0 (); }

		return (ret);

#undef	new_sz
	    }

#if (0)	/*******************************************************/
	    case TCFIFOSZ:
		if ((int)arg == 0)
			return (tp->t_fifosz);

		if ((tp->t_state & HASFIFO) == 0 && (int)arg != 1)
			{ u.u_error = EINVAL; return (-1); }

		if ((unsigned)arg > 16)
			{ u.u_error = EINVAL; return (-1); }

		ret = tp->t_fifosz; tp->t_fifosz = (int)arg;

		return (ret);
#endif	/*******************************************************/

		/*
		 *	get CLEAR TO SEND
		 */
	    case TCGETCTS:
		return (read_port (SIO_MODEM_STATUS_PORT) & MSR_CTS);

		/*
		 *	Prepara o uso do PPP
		 */
	    case TCPPPINT:
		if ((vp = orig_vp[unit]) == NULL)
		{
			printf ("pppctl: Não tinha a entrada original de interrupção\n");
			u.u_error = ENXIO;
			return (-1);
		}

		sp = &ppp_status[unit];

		if (unimove (sp, arg, sizeof (PPP_STATUS), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		/* Procura uma estrutura de compressão VJ */

		if (sp->snd.ppp_vj_compression || sp->rcv.ppp_vj_compression)
		{
			last_comp = &slcompress[NSL];

			for (comp = slcompress; /* abaixo */; comp++)
			{
				if (comp >= last_comp)
					{ u.u_error = EAGAIN; return (-1); }

				if (comp->busy == 0)
					break;
			}

			sp->ppp_slcompress_ptr = comp;
			sl_compress_init (comp);
			comp->busy++;
		}
		else
		{
			sp->ppp_slcompress_ptr = (SLCOMPRESS *)NULL;
		}

		/* Ajusta a interrupção */

#if (0)	/*******************************************************/
		vp = &vecdef[scb.y_sio_irq[unit]];
#endif	/*******************************************************/

	   /***	vp->vec_unit = unit;	***/
	   	vp->vec_func = pppint;

		dev_nm[5] = '0' + (unit << 1);

		if (sp->snd.ppp_addr != 0)
		{
			if (add_route_entry (sp->snd.ppp_addr, dev_nm, u.u_fileptr->f_inode) < 0)
				return (-1);

			sp->ppp_my_addr_added++;
		}

		return (0);

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

}	/* end pppctl */

/*
 ****************************************************************
 *	Função de Interrupção					*
 ****************************************************************
 */
void
pppint (struct intr_frame frame)
{
	int		unit, port, c, i;
	TTY		*tp;
	ITQ		*qp;
	ITBLOCK		*bp;
	static char	no_block_avail = 0;

	/*
	 *	Prólogo
	 */
	unit = frame.if_unit; tp = &ppp[unit];

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
				if (c == PPP_FLAG)
					no_block_avail = 0;
				else
					continue;
			}

			if ((bp = get_it_block (IT_IN)) == NOITBLOCK)
			{
				no_block_avail++;
#ifdef	MSG
				printf ("%g: Não obtive bloco\n");
#endif	MSG
				continue;
			}

			ppp_in_busy++;

			qp->q_first = bp;

		   	bp->it_escape_char = 0;
			bp->it_area    =
			bp->it_u_area  = bp->it_frame + RESSZ;
			bp->it_u_count = DGSZ;

			bp->it_dev = tp->t_dev;
		}

		if (c == PPP_FLAG)	/* Início ou final do pacote? */
		{
#ifdef	MSG
			if (CSWT (11))
				printf ("pppint: Recebi FLAG (%d)\n", DGSZ - bp->it_u_count);
#endif	MSG
			if (bp->it_u_count == DGSZ)
				continue;

			bp->it_u_count   = DGSZ - bp->it_u_count;
			bp->it_ether_dev = 0;
			bp->it_ppp_dev   = 1;

			qp->q_first = NOITBLOCK;

			wake_up_in_daemon (IN_BP, bp);

			ppp_in_busy--;

			continue;
		}

		if (c == PPP_ESC)
			{ bp->it_escape_char++; continue; }

		if (bp->it_escape_char)
			{ bp->it_escape_char = 0; c ^= PPP_EOR; }

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
		if   (bp->it_escape_char)	/* Tinha vindo um PPP_ESC */
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
			if (bp->it_data_area == NOSTR || bp->it_data_count == 0)
				{ c = PPP_FLAG; goto sendchar; }

			c = *bp->it_data_area;

			bp->it_area  = bp->it_data_area  + 1;
			bp->it_count = bp->it_data_count - 1;

			bp->it_data_area = NOSTR;
		}
		else /* it_count == -4 */ 	/* Após o PPP_FLAG final */
		{
			SPINLOCK (&qp->q_lock);

			ppp_out_busy--;

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
		 *	Repare que "ppp_char_map_vec[PPP_EOR]" é sempre 0
		 */
		if (ppp_status[unit].ppp_char_map_vec[c])
			{ bp->it_escape_char = c ^ PPP_EOR; c = PPP_ESC; }

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

}	/* end pppint */

/*
 ****************************************************************
 *	Processa a entrada de pacotes PPP			*
 ****************************************************************
 */
void
receive_ppp_packet (ITBLOCK *bp)
{
	char		*cp;
	int		proto;
	unsigned short	calculated_fcs, read_fcs;
	SLCOMPRESS	*comp;
	char		*str = NOSTR;

	/*
	 *	Confere o CRC
	 */
	cp = bp->it_u_area; bp->it_u_count -= 2;	/* Tira o FCS */

	calculated_fcs = ~(pppfcs (0xFFFF, cp, bp->it_u_count));

	read_fcs = *(ushort *)(cp + bp->it_u_count);

	if (calculated_fcs != read_fcs)
	{
#ifdef	MSG
		if (itscb.it_report_error)
			printf ("%g: Erro no FCS (0x%04x :: 0x%04x)\n", calculated_fcs, read_fcs);
#endif	MSG
		/* Abandona o próximo pacote com c_id implícito */

		if ((comp = ppp_status[MINOR (bp->it_dev)].ppp_slcompress_ptr) != (SLCOMPRESS *)NULL)
			comp->flags |= SLF_TOSS;

		put_it_block (bp); return;
	}

	/*
	 *	Retira o PROTOCOLO
	 */
	if (*cp == PPP_ADDR)
		cp++;

	if (*cp == PPP_CTL)
		cp++;

	if (*cp & 1)
		{ proto = *cp++; }
	else
		{ proto = *cp++ << 8; proto |= *cp++; }

	/*
	 *	Examina o protocolo
	 */
	switch (proto)
	{
	    case PPP_IP:	/* Internet Protocol */
		bp->it_u_count -= (cp - (char *)bp->it_u_area);
		bp->it_u_area   =  cp;
		receive_ip_datagram (bp);
		return;

	    case PPP_VJ_COM:	/* Van Jacobson Compressed TCP/IP */
	    case PPP_VJ_UN:	/* Van Jacobson Uncompressed TCP/IP */
		bp->it_u_count -= (cp - (char *)bp->it_u_area);
		bp->it_u_area   =  cp;
		comp = ppp_status[MINOR (bp->it_dev)].ppp_slcompress_ptr;

		if (comp == (SLCOMPRESS *)NULL)
		{
			printf ("%g: Recebi datagrama comprimido em linha sem compressão\n");
			put_it_block (bp); return;
		}

		if (sl_uncompress_tcp (bp, proto, comp) >= 0)
			receive_ip_datagram (bp);
		return;

	    case PPP_LCP:	/* Link Control Protocol */
		lcp_proc (bp, cp);
		return;

	    case PPP_IPCP:	/* Internet Protocol Control Protocol */
		str = " (IPCP)";
		break;

	    default:
		break;

	}	/* end switch (proto) */

	/*
	 *	Protocolo ainda não reconhecido
	 */
#ifdef	MSG
	if (itscb.it_report_error)
		printf ("%g: Protocolo ainda NÃO processado: 0x%04x%s\n", proto, str == NOSTR ? "" : str);
#endif	MSG

	put_it_block (bp); return;

}	/* end receive_ppp_packet */

/*
 ****************************************************************
 *	Processa o protocolo LCP				*
 ****************************************************************
 */
void
lcp_proc (ITBLOCK *bp, const char *area)
{
	int		code;

	switch (code = *area)
	{
	/** case LCP_CONF_REQ:		/* PPP LCP configure request */
	/** case LCP_CONF_ACK:		/* PPP LCP configure acknowledge */
	/** case LCP_CONF_NAK:		/* PPP LCP configure negative ack */
	/** case LCP_CONF_REJ:		/* PPP LCP configure reject */
	/** case LCP_TERM_REQ:		/* PPP LCP terminate request */
	/** case LCP_TERM_ACK:		/* PPP LCP terminate acknowledge */
	/** case LCP_CODE_REJ:		/* PPP LCP code reject */
	/** case LCP_PROTO_REJ:		/* PPP LCP protocol reject */

	    case LCP_ECHO_REQ:		/* PPP LCP echo request */
		lcp_reply_echo_request (bp, area);
		return;

	/** case LCP_ECHO_REPLY:	/* PPP LCP echo reply */
	/** case LCP_DISC_REQ:		/* PPP LCP discard request */

	    default:
		printf ("ppp: Código LCP ainda NÃO processado: %d\n", code);
		break;
	}

	put_it_block (bp);

}	/* end lcp_proc */

/*
 ****************************************************************
 *	LCP: Reply echo request					*
 ****************************************************************
 */
void
lcp_reply_echo_request (ITBLOCK *bp, const char *area)
{
	const LCP_H 	*lp;
	ITBLOCK		*ap;
	char		*cp;
	const char	*begin;
	int		unit, id, count;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = area + sizeof (LCP_H);

	/*
	 *	Envia a resposta (o eco)
	 */
	if ((ap = get_it_block (IT_IN)) == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: Não obtive bloco\n");
#endif	MSG
		put_it_block (bp); return;
	}

	cp = ap->it_frame + PPP_H_SZ + sizeof (LCP_H);
	unit = MINOR (bp->it_dev);

	memmove (cp, begin, count);
	*(long *)cp = ppp_status[unit].snd.ppp_magic_number;

	ap->it_u_area  = cp;
	ap->it_u_count = count;
	ap->it_dev     = bp->it_dev;

	put_it_block (bp);

	ap->it_in_driver_queue = 1;
	ap->it_free_after_IO = 1;

	ppp_lcp_packet_snd (ap, PPP_LCP, LCP_ECHO_REPLY, id);

#ifdef	MSG
	if (CSWT (11))
		printf ("%g: Enviei ECHO\n");
#endif	MSG

}	/* end lcp_reply_echo_request */

/*
 ****************************************************************
 *	Envia pacotes LCP					*
 ****************************************************************
 */
void
ppp_lcp_packet_snd (ITBLOCK *bp, int proto, int code, int id)
{
	int		unit, port;
	TTY		*tp;
	ITQ		*qp;
	char 		*cp;
	int		tas_ret, fcs, len;

   	bp->it_escape_char = 0;

	bp->it_area  = bp->it_u_area;		/* Área do Cabeçalho (+ dados) */
	bp->it_count = bp->it_u_count;

	bp->it_data_area  = NOSTR;		/* Área dos Dados */
	bp->it_data_count = 0;

	/*
	 *	O "spl" é para não deixar uma interrupção
	 *	da entrada iniciar a saída
	 */
	unit = MINOR (bp->it_dev);
	tp = &ppp[unit];
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
	 *	Prepara o cabeçalho
	 */
	cp = bp->it_area - PPP_H_SZ - sizeof (LCP_H);

	cp[0] = PPP_ADDR;		/* Address */
	cp[1] = PPP_CTL;		/* Control */
	cp[2] = proto >> 8;		/* Protocol */
	cp[3] = proto;

	len = bp->it_count + sizeof (LCP_H);

	cp[4] = code;			/* Code */
	cp[5] = id;			/* ID */
	cp[6] = len >> 8;		/* Len */
	cp[7] = len;

	bp->it_count += PPP_H_SZ + sizeof (LCP_H);	/* Incorpora o cabeçalho */
	bp->it_area   = cp;

	/*
	 *	Calcula o FCS
	 */
	fcs = ~(pppfcs (0xFFFF, cp, bp->it_count));

	cp += bp->it_count; *cp++ = fcs; *cp++ = fcs >> 8;

	bp->it_count += 2;			/* Incorpora o CRC */

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

	ppp_out_busy++;

	EVENTCLEAR (&tp->t_outqempty);

	SPINFREE (&qp->q_lock);

	/*
	 *	Verifica se precisa iniciar a operação
	 */
	if (tas_ret < 0)
		{ spl0 (); return; }

	/*
	 *	Inicia a operação com o caracter "FLAG"
	 */
	while ((read_port (SIO_LINE_STATUS_PORT) & LSR_TXRDY) == 0)
		/* vazio */;

	write_port (PPP_FLAG, SIO_DATA_PORT);

	spl0 ();

}	/* end ppp_lcp_packet_snd */

/*
 ****************************************************************
 *	Cálculo do CRC						*
 ****************************************************************
 */
int
pppfcs (int fcs, const char *cp, int count)
{
	extern const unsigned short	ppp_fcstb[];

	while (count-- > 0)
		fcs = (fcs >> 8) ^ ppp_fcstb[(fcs ^ *cp++) & 0xFF];

	return (fcs);

}	/* end pppfcs */

/*
 ****************************************************************
 *	Tabela de CRC						*
 ****************************************************************
 */
const unsigned short	ppp_fcstb[256] =
{
	0x0000,0x1189,0x2312,0x329B,0x4624,0x57AD,0x6536,0x74BF,
	0x8C48,0x9DC1,0xAF5A,0xBED3,0xCA6C,0xDBE5,0xE97E,0xF8F7,
	0x1081,0x0108,0x3393,0x221A,0x56A5,0x472C,0x75B7,0x643E,
	0x9CC9,0x8D40,0xBFDB,0xAE52,0xDAED,0xCB64,0xF9FF,0xE876,
	0x2102,0x308B,0x0210,0x1399,0x6726,0x76AF,0x4434,0x55BD,
	0xAD4A,0xBCC3,0x8E58,0x9FD1,0xEB6E,0xFAE7,0xC87C,0xD9F5,
	0x3183,0x200A,0x1291,0x0318,0x77A7,0x662E,0x54B5,0x453C,
	0xBDCB,0xAC42,0x9ED9,0x8F50,0xFBEF,0xEA66,0xD8FD,0xC974,
	0x4204,0x538D,0x6116,0x709F,0x0420,0x15A9,0x2732,0x36BB,
	0xCE4C,0xDFC5,0xED5E,0xFCD7,0x8868,0x99E1,0xAB7A,0xBAF3,
	0x5285,0x430C,0x7197,0x601E,0x14A1,0x0528,0x37B3,0x263A,
	0xDECD,0xCF44,0xFDDF,0xEC56,0x98E9,0x8960,0xBBFB,0xAA72,
	0x6306,0x728F,0x4014,0x519D,0x2522,0x34AB,0x0630,0x17B9,
	0xEF4E,0xFEC7,0xCC5C,0xDDD5,0xA96A,0xB8E3,0x8A78,0x9BF1,
	0x7387,0x620E,0x5095,0x411C,0x35A3,0x242A,0x16B1,0x0738,
	0xFFCF,0xEE46,0xDCDD,0xCD54,0xB9EB,0xA862,0x9AF9,0x8B70,
	0x8408,0x9581,0xA71A,0xB693,0xC22C,0xD3A5,0xE13E,0xF0B7,
	0x0840,0x19C9,0x2B52,0x3ADB,0x4E64,0x5FED,0x6D76,0x7CFF,
	0x9489,0x8500,0xB79B,0xA612,0xD2AD,0xC324,0xF1BF,0xE036,
	0x18C1,0x0948,0x3BD3,0x2A5A,0x5EE5,0x4F6C,0x7DF7,0x6C7E,
	0xA50A,0xB483,0x8618,0x9791,0xE32E,0xF2A7,0xC03C,0xD1B5,
	0x2942,0x38CB,0x0A50,0x1BD9,0x6F66,0x7EEF,0x4C74,0x5DFD,
	0xB58B,0xA402,0x9699,0x8710,0xF3AF,0xE226,0xD0BD,0xC134,
	0x39C3,0x284A,0x1AD1,0x0B58,0x7FE7,0x6E6E,0x5CF5,0x4D7C,
	0xC60C,0xD785,0xE51E,0xF497,0x8028,0x91A1,0xA33A,0xB2B3,
	0x4A44,0x5BCD,0x6956,0x78DF,0x0C60,0x1DE9,0x2F72,0x3EFB,
	0xD68D,0xC704,0xF59F,0xE416,0x90A9,0x8120,0xB3BB,0xA232,
	0x5AC5,0x4B4C,0x79D7,0x685E,0x1CE1,0x0D68,0x3FF3,0x2E7A,
	0xE70E,0xF687,0xC41C,0xD595,0xA12A,0xB0A3,0x8238,0x93B1,
	0x6B46,0x7ACF,0x4854,0x59DD,0x2D62,0x3CEB,0x0E70,0x1FF9,
	0xF78F,0xE606,0xD49D,0xC514,0xB1AB,0xA022,0x92B9,0x8330,
	0x7BC7,0x6A4E,0x58D5,0x495C,0x3DE3,0x2C6A,0x1EF1,0x0F78
};
