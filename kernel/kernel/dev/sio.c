/*
 ****************************************************************
 *								*
 *			sio.c					*
 *								*
 *	"Driver" para linhas seriais com NS16450 ou NS16550	*
 *								*
 *	Versão	3.0.0, de 12.02.95				*
 *		4.5.0, de 17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/pci.h"
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

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
#undef	MSG			/* Liga/desliga as mensagens de depuração */

#define	t_fifosz t_canal	/* Usa para o tamanho do FIFO */

/*
 ******	Tabela de conversão das velocidades *********************
 */
const int sio_speed_cvtb[] =
{
	0,			/* Desconecta */
	COMBRD (110),
	COMBRD (134),
	COMBRD (150),
	COMBRD (200),
	COMBRD (300),
	COMBRD (600),
	COMBRD (1200),
	COMBRD (1800),
	COMBRD (2400),
	COMBRD (4800),
	COMBRD (9600),
	COMBRD (19200),
	COMBRD (38400),
	COMBRD (57600),
	COMBRD (115200)
};

/*
 ******	Variáveis globais ***************************************
 */
entry TTY	*sio_tp[NSIO];	/* Tela virtual corrente de cada unidade */

entry TTY	sio[NSIO];		/* Um para cada tela */

entry char	sio_nopen[NSIO];	/* Para evitar conflito com SLIP */

entry char	sio_lock[NSIO];		/* Para o "O_LOCK" */

entry char	sio_modem[NSIO];	/* Para o MODEM */

entry SIO_CONFIG sio_config[NSIO + 1];

/*
 ******	Protótipos de funções ***********************************
 */
int		sio_generic_attach (int, int, int);
void		sioint (struct intr_frame);
void		siomultiint (struct intr_frame);
void		sioparam (const TTY *);
void		siostart (TTY *, int);

/*
 ****************************************************************
 *	Função de "attach" para unidades ISA			*
 ****************************************************************
 */
int
sioattach (int major)
{
	int		unit, port;

	/*
	 *	Examina cada unidade
	 */
	for (unit = 0; unit < NSIO; unit++)
	{
		if ((port = scb.y_sio_port[unit]) != 0)
			sio_generic_attach (unit, port, scb.y_sio_irq[unit]);

	}

	return (0);

}	/* end sioattach */

/*
 ****************************************************************
 *	Identifica SIOs PCI					*
 ****************************************************************
 */
char *
pci_sio_probe (PCIDATA *tag, ulong type)
{
	switch (type)
	{
	    case 0x100812B9:
		return ("U.S. Robotics 56K PCI Fax Modem");

	    default:
		break;

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_sio_probe */

/*
 ****************************************************************
 *	Função de "attach" para unidades PCI			*
 ****************************************************************
 */
void
pci_sio_attach (PCIDATA * pci, int unit)
{
	ulong		port;
	int		irq;

	/*
	 *	Procura uma entrada vaga no SCB
	 *
	 *	Repare que o "unit" dado do PCI não é usado
	 */
	for (unit = 0; /* abaixo */; unit++)
	{
		if (unit >= NSIO)
		{
			printf ("pci_sio_attach: Tabela de SIOs esgotada\n");
			return;
		}

		if (scb.y_sio_port[unit] == 0)
			break;
	}

	/*
	 *	Obtém a porta
	 */
	if ((port = pci_read (pci, PCI_MAP_REG_START, 4)) == 0)
		return;

	port &= ~1; 	/* The first bit of PCI_BASEADR0 is always set hence we mask it off */

	/*
	 *	Prepara a interrupção
	 */
	irq = pci->pci_intline;
#if (0)	/*******************************************************/
	irq = PCI_INTERRUPT_LINE_EXTRACT (pci_read (pci, PCI_INTERRUPT_REG, 4));
#endif	/*******************************************************/

	/*
	 *	Deteta a unidade
	 */
	if (sio_generic_attach (unit, port, irq) >= 0)
	{
		scb.y_sio_port[unit] = port;
		scb.y_sio_irq[unit] = irq;
	}

}	/* end pci_sio_attach */

/*
 ****************************************************************
 *	Deteta uma porta serial					*
 ****************************************************************
 */
int
sio_generic_attach (int unit, int port, int irq)
{
	SIO_CONFIG 	*sp;

	/*
	 *	Verifica se o IRQ é válido
	 */
	if ((unsigned)irq >= 16)
	{
		printf
		(	"\nsio%d: IRQ inválido: %d\n",
			unit, irq
		);
		return (-1);
	}

	/*
	 *	Inicialmente, verifica se a unidade existe
	 */
	write_port (0, SIO_LINE_CTL_REG_PORT);
	write_port (0, SIO_INT_ID_PORT);

	DELAY (100);

	if (read_port (SIO_INT_ID_PORT) & 0x38)
		return (-1);

	/*
	 *	Procura o FIFO e inicializa para interromper após 14 caracteres
	 */
	write_port
	(	FIFO_ENABLE|FIFO_RCV_RST|FIFO_XMT_RST|FIFO_TRIGGER_14,
		SIO_FIFO_CTL_PORT
	);

	DELAY (100);

	/*
	 *	Imprime os dados da unidade
	 */
	printf ("tty: [%d: 0x%X, %d", unit, port, irq);

	sp = &sio_config[unit]; sp->sio_unit_alive++;

	if ((read_port (SIO_INT_ID_PORT) & IIR_FIFO_MASK) == IIR_FIFO_MASK)
		{ sp->sio_has_fifo++; printf (", FIFO"); }

	printf ("]\n");

	/*
	 *	Inicializa a unidade
	 */
	write_port (0, SIO_INT_ENABLE_REG_PORT);
	write_port (MCR_IENABLE, SIO_MODEM_CTL_PORT);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (irq, PL, unit, sioint) < 0)
		return (-1);

	sio[unit].t_irq = irq;

	return (0);

}	/* end sio_generic_attach */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
sioopen (dev_t dev, int oflag)
{
	TTY		*tp;
	int		unit, port;

	/*
	 *	Prólogo
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NSIO)
		{ u.u_error = ENXIO; return; }

	if (sio_lock[unit] || (oflag & O_LOCK) && sio_nopen[unit])
		{ u.u_error = EBUSY; return; }

	tp = &sio[unit];

   	if (sio_nopen[unit] > 0)
	   	{ ttyopen (dev, tp, oflag); sio_nopen[unit]++; return; }

	/*
	 *	Verifica se a unidade está ativa
	 */
	if (sio_config[unit].sio_unit_alive == 0)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Abre esta console virtual
	 */
#if (0)	/*******************************************************/
	splsio ();
#endif	/*******************************************************/
	splx (irq_pl_vec[tp->t_irq]);

	if ((tp->t_state & ISOPEN) == 0)
	{
		port = scb.y_sio_port[unit];

		sio_modem[unit] = 0;

	   	tp->t_addr   = (void *)port;
		tp->t_oproc  = siostart;

		tp->t_iflag  = ICRNL|IXON;
		tp->t_oflag  = OPOST|ONLCR;
		tp->t_cflag  = B9600|CS8|CLOCAL;
		tp->t_lflag  = ISIG|ICANON|ECHO|ECHOE|ECHOK|CNTRLX|ISO|VIDEO;

		tp->t_nline  = 24;	/* Valores iniciais -std- */
		tp->t_ncol   = 80;

		if (sio_config[unit].sio_has_fifo)
			tp->t_state |= HASFIFO;
		else
			tp->t_state &= ~HASFIFO;

		if (sio_config[unit].sio_has_fifo)
			{ tp->t_state |= HASFIFO; tp->t_fifosz = 14; }
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

		sio_tp[unit] = tp;
		sioparam (tp);
	}

	ttyopen (dev, tp, oflag);

	spl0 ();

	/*
	 *	Epílogo
	 */
	sio_nopen[unit]++;

	if (oflag & O_LOCK)
		sio_lock[unit] = 1;

}	/* end sioopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
sioclose (dev_t dev, int flag)
{
	TTY		*tp;
	int		unit, i, port;

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev);

	tp = &sio[unit]; port = (int)tp->t_addr;

	if (flag == 0)
	{
		ttyclose (tp);
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

}	/* end sioclose */

/*
 ****************************************************************
 *	Função de "read"					*
 ****************************************************************
 */
void
sioread (IOREQ *iop)
{
	ttyread (iop, &sio[MINOR (iop->io_dev)]);

}	/* end sioread */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
siowrite (IOREQ *iop)
{
	ttywrite (iop, &sio[MINOR (iop->io_dev)]);

}	/* end siowrite */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
sioctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY		*tp;
	int		unit, port;
	int		ret;

	/*
	 *	Prólogo
	 */
	unit = MINOR (dev); tp = &sio[unit]; port = (int)tp->t_addr;

	/*
	 *	Tratamento especial para certos comandos
	 */
	switch (cmd)
	{
		/*
		 *	Atribui o tamanho a usar do FIFO na leitura
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

		/*
		 *	get CLEAR TO SEND
		 */
	    case TCGETCTS:
		return (read_port (SIO_MODEM_STATUS_PORT) & MSR_CTS);

		/*
		 *	Processa/prepara o ATTENTION
		 */
	    case U_ATTENTION:
#if (0)	/*******************************************************/
		splsio ();
#endif	/*******************************************************/
		splx (irq_pl_vec[tp->t_irq]);

		if (EVENTTEST (&tp->t_inqnempty) == 0)
			{ spl0 (); return (1); }

		tp->t_uproc  = u.u_myself;
		tp->t_index = (int)arg;

		tp->t_attention_set = 1;
		*(char **)flag	= &tp->t_attention_set;

		spl0 ();

		return (0);

#ifdef	UNATTENTION
		/*
		 *	Desarma o ATTENTION
		 */
	    case U_UNATTENTION:
		tp->t_state &= ~ATTENTION;

		return (0);
#endif	UNATTENTION

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

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

}	/* end sioctl */

/*
 ****************************************************************
 *	Modificar os parametros de "hardware" 			*
 ****************************************************************
 */
void
sioparam (const TTY *tp)
{
	int		port, speed, char_fmt, cflag;

	/*
	 *	Inicia o processamento de configuração
	 */
	port = (int)tp->t_addr; cflag = tp->t_cflag;

	/*
	 *	Liga as interrupções
	 */
	write_port (IER_ERXRDY|IER_ETXRDY|IER_ERLS|IER_EMSC, SIO_INT_ENABLE_REG_PORT);

	/*
	 *	Prepara o formato do caracter
	 */
	char_fmt = (cflag & CSIZE) >> 4; /* Tamanho: coincide com ...BITS */

	if (cflag & PARENB)	/* Paridade */
	{
		char_fmt |= CFCR_PENAB;

		if ((cflag & PARODD) == 0)
			char_fmt |= CFCR_PEVEN;
	}

	if (cflag & CSTOPB)
		char_fmt |= CFCR_STOPB;

   /***	if ((cflag & CLOCAL) == 0) ***/
		/* ... */

	/*
	 *	Codifica a velocidade
	 */
	speed = sio_speed_cvtb[cflag & CBAUD];

	write_port (char_fmt|CFCR_DLAB, SIO_LINE_CTL_REG_PORT);

	write_port (speed,      SIO_DIV_LATCH_LOW_PORT);
	write_port (speed >> 8, SIO_DIV_LATCH_HIGH_PORT);

	write_port (char_fmt, SIO_LINE_CTL_REG_PORT);

	/*
	 *	Prepara o FIFO, se for o caso
	 */
	if (tp->t_state & HASFIFO)
	{
		int		fifoctl;

		switch (tp->t_fifosz)
		{
		    case 1:
		    default:
			fifoctl = FIFO_ENABLE|FIFO_TRIGGER_1; break;

		    case 4:
			fifoctl = FIFO_ENABLE|FIFO_TRIGGER_4; break;

		    case 8:
			fifoctl = FIFO_ENABLE|FIFO_TRIGGER_8; break;

		    case 14:
			fifoctl = FIFO_ENABLE|FIFO_TRIGGER_14; break;
		}

		write_port (fifoctl, SIO_FIFO_CTL_PORT);
	}

	/*
	 *	Prepara o registro de MODEM
	 */
	write_port (MCR_DTR|MCR_RTS|MCR_IENABLE, SIO_MODEM_CTL_PORT);

	DELAY (500);

}	/* end sioparam */

/*
 ****************************************************************
 *	Função de interrupção da linha serial (linha normal)	*
 ****************************************************************
 */
void
sioint (struct intr_frame frame)
{
	int		unit, port, c;
	int		intr_status, line_status, modem_status;
	TTY		*tp;

	/*
	 *	Examina o estado da interrupção
	 */
	unit = frame.if_unit;

#ifdef	MSG
	if (CSWT (11))
		printf ("sioint: unidade %d\n", unit);
#endif	MSG

	if ((tp = sio_tp[unit]) == NOTTY)
		return;

	port = (int)tp->t_addr;

	for (EVER) switch (intr_status = (read_port (SIO_INT_ID_PORT) & IIR_IMASK))
	{
		/*
		 *	Nenhuma interrupção mais pendente
		 */
	    case IIR_NOPEND:
		return;

		/*
		 *	Interrupção de entrada
		 */
	    case IIR_RXTOUT:
	    case IIR_RXRDY:
		do
		{
			c = read_port (SIO_DATA_PORT);
#ifdef	MSG
			if (CSWT (11))
				printf ("sioint: li o caractere %d (%c)\n", c, c);
#endif	MSG
			ttyin (c, tp);
		}
		while (read_port (SIO_LINE_STATUS_PORT) & LSR_RCV_MASK);

		break;

		/*
		 *	Interrupção de saída
		 */
	    case IIR_TXRDY:
		CLEAR (&tp->t_obusy);
	    start_output:
		siostart (tp, 1);

		if (tp->t_outq.c_count <= TTYMINQ)
			EVENTDONE (&tp->t_outqnfull);

		break;

		/*
		 *	Interrupção de mudança do estado da linha
		 */
	    case IIR_RLS:
		line_status = read_port (SIO_LINE_STATUS_PORT);

		c = read_port (SIO_DATA_PORT);

#ifdef	MSG
		if (CSWT (11))
			printf ("sioint: caractere malformado %d (%c)\n", c, c);
#endif	MSG
		break;

		/*
		 *	Interrupção de mudança do estado do modem
		 */
	    case IIR_MLSC:
		modem_status = read_port (SIO_MODEM_STATUS_PORT);

		if   (modem_status & MSR_DCD)	/* Data Carrier detect */
		{
			sio_modem[unit] = 1;
		}
		elif (sio_modem[unit]) 		/* Verifica se caiu a conexão */
		{
			sio_modem[unit] = 0;
			sigtgrp (tp->t_tgrp, SIGHUP);
		}

		if (tp->t_cflag & CLOCAL)
			break;

		if (modem_status & MSR_CTS)
			{ tp->t_state &= ~TTYSTOP; goto start_output; }
		else
			tp->t_state |= TTYSTOP;
		break;

		/*
		 *	Interrupção de causas desconhecidas
		 */
	    default:
#ifdef	MSG
		printf ("sioint: Interrupção estranha: %X\n", intr_status);
#endif	MSG
		break;

	}	/* end for (EVER) switch */

}	/* end sioint */

/*
 ****************************************************************
 *	Função para iniciar a transmissão			*
 ****************************************************************
 */
void
siostart (TTY *tp, int flag)
{
	int		port, pl = 0, c, i;

	/*
	 *	O "flag" indica a origem da chamada:
	 *
	 *	    0 => Chamada fora de "sioint"
	 *	    1 => Chamada de "sioint"
	 */

	/*
	 *	Verifica se tem operação em andamento
	 */
	if (TAS (&tp->t_obusy) < 0)
		return;

	port = (int)tp->t_addr;

#if (0)	/*******************************************************/
	if (flag == 0)
		pl = splsio ();
#endif	/*******************************************************/

	if (flag == 0)
		pl = splx (irq_pl_vec[tp->t_irq]);

	/*
	 *	Verifica se pode transmitir e não está no estado STOP
	 */
	if (tp->t_state & TTYSTOP || (read_port (SIO_LINE_STATUS_PORT) & LSR_TXRDY) == 0)
		{ CLEAR (&tp->t_obusy); goto out; }

	/*
	 *	Verifica se tem caracter para transmitir
	 */
	SPINLOCK (&tp->t_outq.c_lock);

	if ((c = cget (&tp->t_outq)) < 0)
		{ CLEAR (&tp->t_obusy); EVENTDONE (&tp->t_outqempty); SPINFREE (&tp->t_outq.c_lock); goto out; }

	SPINFREE (&tp->t_outq.c_lock);

	/*
	 *	Escreve um caracter
	 */
	write_port (tp->t_cvtb[c], SIO_DATA_PORT);

	if (c == '\n' && (tp->t_oflag & OPOST))
	{
		tp->t_lno++;

		if (tp->t_page != 0 && tp->t_lno >= tp->t_page)
			{ tp->t_state |= TTYSTOP; goto out; }
	}

	/*
	 *	Se tiver FIFO, pode escrever mais caracteres
	 */
	if (tp->t_state & HASFIFO)
	{
		for (i = 16 - 1; i > 0; i--)
		{
			SPINLOCK (&tp->t_outq.c_lock);

			if ((c = cget (&tp->t_outq)) < 0)
				{ EVENTDONE (&tp->t_outqempty); SPINFREE (&tp->t_outq.c_lock); goto out; }

			SPINFREE (&tp->t_outq.c_lock);

			write_port (tp->t_cvtb[c], SIO_DATA_PORT);

			if (c == '\n')
			{
				if ((tp->t_oflag & OPOST) == 0)
					continue;

				tp->t_lno++;

				if (tp->t_page != 0 && tp->t_lno >= tp->t_page)
					{ tp->t_state |= TTYSTOP; break; }
			}
		}
	}

	/*
	 *	Restaura a prioridade, se for o caso
	 */
    out:
	if (flag == 0)
		splx (pl);

}	/* end siostart */
