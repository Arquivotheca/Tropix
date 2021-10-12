/*
 ****************************************************************
 *								*
 *			lp.c					*
 *								*
 *	"Driver" para saída paralela (impressora)		*
 *								*
 *	Versão	3.0.0, de 06.11.95				*
 *		3.2.3, de 22.10.99				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/frame.h"
#include "../h/cpu.h"
#include "../h/intr.h"
#include "../h/tty.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/timeout.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
#define NLP	4		/* Previsão para 4 portas paralelas */

#undef	DEBUG

#define PL	2		/* Prioridade de interrupção */
#define spllp	spl2		/* Função de prioridade de interrupção */

#define	t_unit	  t_canal	/* Nomes preferidos */
#define	t_control t_vterm

/*
 ******	Definições da saída paralela ****************************
 */
#define LP_DATA		0	/* Data to/from printer (R/W) */

#define LP_STATUS	1	/* Status of printer (R) */

#define	LPS_NERR		0x08	/* Printer no error */
#define	LPS_SEL			0x10	/* Printer selected */
#define	LPS_OUT			0x20	/* Printer out of paper */
#define	LPS_NACK		0x40	/* Printer no ack of data */
#define	LPS_NBSY		0x80	/* Printer not busy */

#define LP_CONTROL	2	/* Control printer (R/W) */

#define	LPC_STB			0x01	/* Strobe data to printer */
#define	LPC_AUTOL		0x02	/* Automatic linefeed */
#define	LPC_NINIT		0x04	/* Initialize printer */
#define	LPC_SEL			0x08	/* Printer selected */
#define	LPC_ENA			0x10	/* Enable interrupts */

/*
 *	Máscaras para testar se a unidade está pronta
 */
#define RDY_MASK	(LPS_SEL|LPS_OUT|LPS_NBSY|LPS_NERR|LPS_NACK)
#define RDY_VALUE	(LPS_SEL|        LPS_NBSY|LPS_NERR|LPS_NACK)

#define LP_READY(port)	((read_port (port + LP_STATUS) & RDY_MASK) == RDY_VALUE)

#if (0)	/*******************************************************/
#define RDY_MASK	(LPS_SEL|LPS_OUT|LPS_NBSY|LPS_NERR)	/* ready ? */
#define RDY_VALUE	(LPS_SEL|LPS_NBSY|LPS_NERR)
#endif	/*******************************************************/

/*
 ******	Definições para impressão por estado ********************
 */
#define	MAX_SLEEP	(scb.y_hz * 5)	/* Tempo máximo de espera */
#define	MAX_SPIN	200		/* Max delay for device ready in usecs */

#if (0)	/*******************************************************/
#define	LPS_INVERT	(LPS_NBSY | LPS_NACK |           LPS_SEL | LPS_NERR)
#define	LPS_MASK	(LPS_NBSY | LPS_NACK | LPS_OUT | LPS_SEL | LPS_NERR)
#define	NOT_READY(x)	((read_port (x) ^ LPS_INVERT) & LPS_MASK)
#endif	/*******************************************************/

/*
 ******	Definições da saída paralela ****************************
 */
typedef struct
{
	char	lp_unit_alive;	/* A unidade existe */
	LOCK	lp_is_open;	/* A unidade está aberta */
	char	lp_use_int;	/* A unidade usa interrupção */
	EVENT	lp_wait_ready;	/* Evento para esperar até ficar pronta */

}	LP_CONFIG;

entry LP_CONFIG	lp_config[NLP];

entry TTY	lpb[NLP];	/* Um para cada impressora */

/*
 ******	Protótipos de funções ***********************************
 */
int		lp_port_test (int port, char data);
void		lpint (struct intr_frame frame);
void		lp_int_start (TTY *tp, int flag);
void		lp_status_start (TTY *tp, int flag);

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
int
lpattach (int major)
{
	LP_CONFIG 	*lp;
	int		unit, port, irq, i;
	char		data;

	for (unit = 0, lp = lp_config; unit < NLP; unit++, lp++)
	{
		if ((port = scb.y_lp_port[unit]) == 0)
			continue;

		if ((unsigned)(irq = scb.y_lp_irq[unit]) >= 16)
		{
			printf
			(	"\nlp%d: IRQ inválido: %d\n",
				unit, irq
			);
			continue;
		}

		if (irq != 0)
			lp->lp_use_int++;

		/*
		 *	Realiza os teste com os diversos padrões
		 */
		if (lp_port_test (port, 0x55) < 0)
			continue;

		if (lp_port_test (port, 0xAA) < 0)
			continue;

		for (i = 0; i < 8; i++)		/* Um 0 de cada vez */
		{
			data = ~(1 << i);

			if (lp_port_test (port, data) < 0)
				continue;
		}

		for (i = 0; i < 8; i++)		/* Um 1 de cada vez */
		{
			data = (1 << i);

			if (lp_port_test (port, data) < 0)
				continue;
		}

		/*
		 *	Achou uma porta ativa
		 */
		printf ("lp:  [%d: 0x%X, %d] ", unit, port, irq);

		if (lp->lp_use_int)
			printf ("(por interrupção)\n");
		else
			{ printf ("(por estado)\n"); goto reset; }

		/*
		 *	Prepara a interrupção (se for o caso)
		 */
		if (set_dev_irq (irq, PL, unit, lpint) < 0)
			return (-1);

		lpb[unit].t_irq = irq;

		/*
		 *	Tudo certo; inicializa as portas de contrôle e dados
		 */
	    reset:
		lp->lp_unit_alive++;

		write_port (0, port + LP_DATA);
		write_port (0, port + LP_CONTROL);

		write_port (LPC_NINIT, port + LP_CONTROL);

	}	/* end for (pelas portas) */

	return (0);

}	/* lpattach */

/*
 ****************************************************************
 *	Testa o eco na porta de dados				*
 ****************************************************************
 */
int
lp_port_test (int port, char write_data)
{
	char	read_data;
	int	timeout = 10000;

	write_port (write_data, port + LP_DATA);

	do
	{
		DELAY (10);
		read_data = read_port (port + LP_DATA);
	}
	while (read_data != write_data && --timeout > 0);

#ifdef	DEBUG
	printf
	(	"lp_port_test (porta 0x%X): "
		"write_data = %02X, read_data = %02X, timeout = %d\n",
		port, write_data, read_data, timeout
	);
#endif	DEBUG

	if (read_data != write_data)
		return (-1);

	return (0);

}	/* end lp_port_test */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
lpopen (dev_t dev, int oflag)
{
	LP_CONFIG 	*lp;
	TTY		*tp;
	int		unit, port, n_try;

	/*
	 *	Impressoras não podem ser lidas
	 */
	if (oflag & O_READ)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Obtém a unidade
	 */
	if ((unsigned)(unit = MINOR (dev)) >= NLP)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Verifica se a unidade está ativa
	 */
	if ((lp = &lp_config[unit])->lp_unit_alive == 0)
		{ u.u_error = ENXIO; return; }

	/*
	 *	Verifica se já está aberta
	 */
	if (SLEEPTEST (&lp->lp_is_open) < 0)
		{ u.u_error = EBUSY; return; }

	port = scb.y_lp_port[unit];

	/*
	 *	Inicializa a impressora
	 */
	write_port (0, port + LP_CONTROL);
	DELAY (500);
	write_port (LPC_SEL|LPC_NINIT, port + LP_CONTROL);

	/*
	 *	Verifica se a impressora está pronta
	 */
	for (n_try = 4; /* abaixo */; n_try--)
	{
		if (n_try <= 0)
		{
			spl0 ();
			printf ("lp%d: Unidade OFFLINE\n", unit);
			SLEEPFREE (&lp->lp_is_open);
			u.u_error = EOLINE;
			return;
		}

		if (LP_READY (port))
			break;

		EVENTWAIT (&every_second_event, PTTYOUT);

	}	/* end for (tentativas) */

	/*
	 *	Prepara o comando em "LP_CONTROL"
	 */
	tp = &lpb[unit];

#if (0)	/*******************************************************/
	spllp ();
#endif	/*******************************************************/
	splx (irq_pl_vec[tp->t_irq]);

	tp->t_control = LPC_SEL|LPC_NINIT;

	if (lp->lp_use_int)
		tp->t_control |= LPC_ENA;

	write_port (tp->t_control, port + LP_CONTROL);

	/*
	 *	Prepara a estrutura TTY
	 */
	if ((tp->t_state & ISOPEN) == 0)
	{
	   	tp->t_unit  = unit;
	   	tp->t_addr  = (void *)port;
		tp->t_oproc = lp->lp_use_int ? lp_int_start : lp_status_start;

		if ((tp->t_state & ONEINIT) == 0)
		{
			tp->t_iflag  = ICRNL|IXON;
			tp->t_oflag  = OPOST|ONLCR|TAB3;
			tp->t_cflag  = B9600|CS8|CLOCAL;
			tp->t_lflag  = ISIG|ICANON|ECHO|ECHOE|ECHOK|CNTRLX|ISO|VIDEO;

			tp->t_nline  = 24;	/* Valores iniciais -std- */
			tp->t_ncol   = 80;
		}
	}

	/*
	 *	Epílogo
	 */
	ttyopen (dev, tp, oflag);

	tp->t_page = 0;

	tp->t_state |= ONEINIT;

	spl0 ();

}	/* end lpopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
lpclose (dev_t dev, int flag)
{
	TTY		*tp;
	LP_CONFIG 	*lp;
	int		unit, port;

	/*
	 *	Calcula a unidade e outros dados
	 */
	if (flag != 0)
		return;

	unit = MINOR (dev); tp = &lpb[unit]; port = (int)tp->t_addr;

	lp = &lp_config[unit];

	/*
	 *	Fecha o dispositivo
	 */
   /***	EVENTWAIT (&tp->t_outqempty, PTTYOUT);	***/

	ttyclose (tp);

	write_port (LPC_NINIT, port + LP_CONTROL);

	SLEEPFREE (&lp->lp_is_open);

}	/* end lpclose */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
lpwrite (IOREQ *iop)
{
	ttywrite (iop, &lpb[MINOR (iop->io_dev)]);

}	/* end lpwrite */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
lpctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY		*tp;
	int		ret;

	tp = &lpb[MINOR (dev)];

   /***	SLEEPLOCK (&tp->t_olock, PTTYOUT); ***/

	ret = ttyctl (tp, cmd, arg);

   /***	SLEEPFREE (&tp->t_olock); ***/

	return (ret);

}	/* end lpctl */

/*
 ****************************************************************
 *	Escreve caracteres por "status"				*
 ****************************************************************
 */
void
lp_status_start (TTY *tp, int flag)
{
	int		port = (int)tp->t_addr;
	int		spin, tic, c;
	EVENT		*wait_ready = &lp_config[tp->t_unit].lp_wait_ready;

	/*
	 *	Verifica se já está escrevendo
	 */
	if (TAS (&tp->t_obusy) < 0)
		return;

	/*
	 *	Malha até acabar os dados da CLIST
	 */
	for (EVER)
	{
		/*
		 *	Se não estiver pronta, espera
		 */
		for (spin = 0; !LP_READY (port) && spin < MAX_SPIN; spin++)
			DELAY (1);

		/*
		 *	Verifica se deu "timeout"
		 */
		if (spin >= MAX_SPIN)
		{
			tic = 0;

			while (!LP_READY (port))
			{
				/*
				 *    Espera um tempo exponencialmente crescente
				 */
				tic = tic + tic + 1;

				if (tic > MAX_SLEEP)
					tic = MAX_SLEEP;

				EVENTCLEAR (wait_ready);

				toutset (tic, eventdone, (int)wait_ready);

				EVENTWAIT (wait_ready, PTTYOUT);
			}
		}

		/*
		 *	Verifica se tem caracter para transmitir
		 */
		SPINLOCK (&tp->t_outq.c_lock);

		if ((c = cget (&tp->t_outq)) < 0)
		{
			CLEAR (&tp->t_obusy); EVENTDONE (&tp->t_outqempty);
			SPINFREE (&tp->t_outq.c_lock); return;
		}

		if (tp->t_outq.c_count <= TTYMINQ)
			EVENTDONE (&tp->t_outqnfull);

		SPINFREE (&tp->t_outq.c_lock);

		/*
		 *	Escreve o caractere
		 */
		write_port (tp->t_cvtb[c], port + LP_DATA);
		write_port (tp->t_control|LPC_STB, port + LP_CONTROL);
		write_port (tp->t_control, port + LP_CONTROL);

	}	/* end for (EVER) */

} 	/* lp_write_by_status */

/*
 ****************************************************************
 *	Função de interrupção					*
 ****************************************************************
 */
void
lpint (struct intr_frame frame)
{
	TTY		*tp;

	/*
	 *	Examina o estado da interrupção
	 */
	tp = &lpb[frame.if_unit];

	CLEAR (&tp->t_obusy);

	lp_int_start (tp, 1);

	if (tp->t_outq.c_count <= TTYMINQ)
		EVENTDONE (&tp->t_outqnfull);

}	/* end lpint */

/*
 ****************************************************************
 *	Função para iniciar a transmissão por interrupção	*
 ****************************************************************
 */
void
lp_int_start (TTY *tp, int flag)
{
	int		port, c, pl = 0;

	/*
	 *	O indicador "flag" indica a origem da chamada:
	 *	    0 => Chamada fora de lpint
	 *	    1 => Chamada de lpint
	 */

	/*
	 *	Verifica se tem operação em andamento
	 */
	if (TAS (&tp->t_obusy) < 0)
		return;

	port = (int)tp->t_addr;

#if (0)	/*******************************************************/
	if (flag == 0)
		pl = spllp ();
#endif	/*******************************************************/

	if (flag == 0)
		pl = splx (irq_pl_vec[tp->t_irq]);

	/*
	 *	Verifica se a impressora está pronta
	 */
	if ((read_port (port + LP_STATUS) & LPS_NERR) == 0)
		{ CLEAR (&tp->t_obusy); goto out; }

	/*
	 *	Verifica se tem caracter para transmitir
	 */
	SPINLOCK (&tp->t_outq.c_lock);

	if ((c = cget (&tp->t_outq)) < 0)
		{ CLEAR (&tp->t_obusy); EVENTDONE (&tp->t_outqempty); SPINFREE (&tp->t_outq.c_lock); goto out; }

	SPINFREE (&tp->t_outq.c_lock);

	/*
	 *	Escreve o caractere
	 */
	write_port (tp->t_cvtb[c], port + LP_DATA);
	write_port (tp->t_control|LPC_STB, port + LP_CONTROL);
	write_port (tp->t_control, port + LP_CONTROL);

	/*
	 *	Restaura a prioridade, se for o caso
	 */
    out:
	if (flag == 0)
		splx (pl);

}	/* end lp_int_start */
