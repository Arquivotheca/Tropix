/*
 ****************************************************************
 *								*
 *			ps2m.c					*
 *								*
 *	"Driver" para o "camundongo" PS/2			*
 *								*
 *	Versão	4.4.0, de 18.11.02				*
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
#include "../h/region.h"

#include "../h/kfile.h"
#include "../h/tty.h"
#include "../h/kbd.h"
#include "../h/frame.h"
#include "../h/intr.h"
#include "../h/ioctl.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Definições globais **************************************
 */
#define	PS2M_IRQ	12	/* No. do IRQ do "mouse" */
#define PS2M_PL		1	/* Prioridade de interrupção */
#define splps2m		spl1	/* Função de prioridade de interrupção */

/*
 ******	Fila de caracteres **************************************
 */
entry KBDQ	kbd_kbd_q =   { kbd_kbd_q.q,   kbd_kbd_q.q,   0 };

entry KBDQ	kbd_mouse_q = { kbd_mouse_q.q, kbd_mouse_q.q, 0 };

/*
 ******	Variáveis Globais ***************************************
 */
entry int	ps2m_present;

entry int	kbd_command_byte = -1;

/*
 ******	Vários modelos de camundongo ****************************
 */
int		kbd_enable_groller ();
int		kbd_enable_gmouse ();
int		kbd_enable_aglide ();
int		kbd_enable_kmouse ();
int		kbd_enable_msexplorer ();
int		kbd_enable_msintelli ();
int		kbd_enable_4dmouse ();
int		kbd_enable_4dplus ();
int		kbd_enable_mmanplus ();
int		kbd_enable_versapad ();

typedef struct
{
	int		model;
	char		syncmask;
	int 		packetsize;
	int 		(*probefunc) (void);

}	VENDORTYPE;

const VENDORTYPE	vendortype[] =
{
#if (0)	/*******************************************************/
	/* Genius NetMouse */

	{ MOUSE_MODEL_NET,		0x08,	MOUSE_PS2INTELLI_PACKETSIZE,	kbd_enable_gmouse, },

	 /* Genius NetScroll */

	{ MOUSE_MODEL_NETSCROLL,	0xc8,	6,				kbd_enable_groller, },
#endif	/*******************************************************/

	/* Logitech MouseMan+ */

	{ MOUSE_MODEL_MOUSEMANPLUS,	0x08,	MOUSE_PS2_PACKETSIZE,		kbd_enable_mmanplus, },

#if (0)	/*******************************************************/
	/* Microsoft IntelliMouse Explorer */

	{ MOUSE_MODEL_EXPLORER,	0x08,	MOUSE_PS2INTELLI_PACKETSIZE,		kbd_enable_msexplorer, },

	/* A4 Tech 4D Mouse */

	{ MOUSE_MODEL_4D,		0x08,	MOUSE_4D_PACKETSIZE,		kbd_enable_4dmouse, },

	/* A4 Tech 4D+ Mouse */

	{ MOUSE_MODEL_4DPLUS,		0xc8,	MOUSE_4DPLUS_PACKETSIZE,	kbd_enable_4dplus, },

	/* Microsoft IntelliMouse */

	{ MOUSE_MODEL_INTELLI,		0x08,	MOUSE_PS2INTELLI_PACKETSIZE,	kbd_enable_msintelli, },

	/* ALPS GlidePoint */

	{ MOUSE_MODEL_GLIDEPOINT,	0xc0,	MOUSE_PS2_PACKETSIZE,		kbd_enable_aglide, },

	/* Kensignton ThinkingMouse */

	{ MOUSE_MODEL_THINK, 		0x80,	MOUSE_PS2_PACKETSIZE,		kbd_enable_kmouse, },

	/* Interlink electronics VersaPad */

	{ MOUSE_MODEL_VERSAPAD,		0xe8,	MOUSE_PS2VERSA_PACKETSIZE,	kbd_enable_versapad, },
#endif	/*******************************************************/

	/* Genérica */

	{ MOUSE_MODEL_GENERIC,		0xc0,	MOUSE_PS2_PACKETSIZE,		NULL, },
};

/*
 ******	Variáveis globais ***************************************
 */
entry TTY	ps2m_tty;		/* Para o "mouse" PS/2 */

entry char	ps2m_nopen;		/* No. de Opens */

entry char	ps2m_lock;		/* Para o "O_LOCK" */

/*
 ******	Protótipos de funções ***********************************
 */
void		ps2mstart (TTY *tp, int flag);

extern void	conint (struct intr_frame);

/*
 ****************************************************************
 *	Anexa o camundongo PS/2					*
 ****************************************************************
 */
int
ps2mattach (int major)
{
	int			i, command_byte;
	int			status[3];
	int			rate, resolution, type, buttons;
	const VENDORTYPE	*vendorp;

	splps2m ();

	kbd_empty_both_buffers (10);

	if ((command_byte = kbd_get_controller_command_byte ()) < 0)
		return (-1);

	/*
	 *	Desabilita o teclado, para testar o camundongo
	 */
	if
	(	kbd_set_controller_command_byte
		(
			KBD_KBD_CONTROL_BITS|KBD_AUX_CONTROL_BITS,
	  		KBD_DISABLE_KBD_PORT|KBD_DISABLE_KBD_INT|KBD_ENABLE_AUX_PORT|KBD_DISABLE_AUX_INT
		) < 0
	)
		goto bad;

	/*
	 *	Testa a porta do camundongo
	 */
	kbd_write_controller_command (KBDC_ENABLE_AUX_PORT);

	switch (i = kbd_test_aux_port ())
	{
	    case 1:		/* Erro a ignorar */
	    case PSM_ACK:
	    case 0:		/* Valor desejado */
		break;

       /*** case -1:		/* Tempo esgotado ***/
	    default:
		goto bad;
	}

	if (kbd_reset_aux_dev () < 0)
		goto bad;

	/*
	 *	Liga e desliga rapidamente o camundongo
	 */
	if (kbd_enable_aux_dev () < 0 || kbd_disable_aux_dev () < 0)
		goto bad;

	/*
	 *	Obtém os valores "default"
	 */
	if (kbd_get_mouse_status (status, 0, 3) >= 3)
	{
		rate		= status[2];
		resolution	= status[1];
	}
	else
	{
		rate		= -1;
		resolution	= -1;
	}

	/*
	 *	Verifica se o dispositivo auxiliar é de fato um camundongo
	 */
	switch (i = kbd_get_aux_id ())
	{
	    case PSM_MOUSE_ID:
	    case PSM_INTELLI_ID:
	    case PSM_EXPLORER_ID:
	    case PSM_4DMOUSE_ID:
	    case PSM_4DPLUS_ID:
		type = MOUSE_MOUSE;
		break;

	    case PSM_BALLPOINT_ID:
		type = MOUSE_TRACKBALL;
		break;

	    default:
		type = MOUSE_UNKNOWN;
		break;
	}

	/*
	 *	Obtém o número de botões
	 */
	buttons = kbd_get_mouse_buttons ();

	/*
	 *	Inicialização de cada modelo
	 */
	for (vendorp = vendortype; vendorp->probefunc != NULL; vendorp++)
	{
		if ((*vendorp->probefunc) () >= 0)
			break;
	}

	kbd_restore_controller (command_byte);

	kbd_empty_both_buffers (10);

	spl0 ();

	/*
	 *	Atribui o IRQ
	 */
	if (set_dev_irq (PS2M_IRQ, PS2M_PL, 0, conint) < 0)
		return (-1);

	printf ("\e[32m");
	printf ("PS2: [0: 0x60, %d]\n", PS2M_IRQ);
	printf  ("\e[0m");

	ps2m_present++;

	return (0);


	/*
	 *	Liga o camundongo
	 */
	if (kbd_enable_aux_dev () < 0)
		printf ("%g: Não consegui ligar o camundongo\n");

	if (kbd_get_mouse_status (status, 0, 3) < 3)
		printf ("%g: Não consegui obter o estado do camundongo\n");

	if
	(	kbd_set_controller_command_byte
		(
			KBD_KBD_CONTROL_BITS|KBD_AUX_CONTROL_BITS,
	  		KBD_ENABLE_KBD_PORT|KBD_ENABLE_KBD_INT|KBD_ENABLE_AUX_PORT|KBD_ENABLE_AUX_INT
		) < 0
	)
	{
		printf ("%g: Não consegui atribuir o comando\n");
		kbd_restore_controller (command_byte);
	}

	spl0 ();

	return (0);

	/*
	 *	Se não achou, ...
	 */
   bad:
	kbd_restore_controller (command_byte);
	spl0 ();
	return (-1); 

}	/* ps2mattach */

/*
 ****************************************************************
 *	Função de "open" do "mouse"				*
 ****************************************************************
 */
void
ps2mopen (dev_t dev, int oflag)
{
	TTY		*tp = &ps2m_tty;
	int		status[3];
	int		command_byte;

	/*
	 *	Prólogo
	 */
	if (!ps2m_present)
		{ u.u_error = ENXIO; return; }

	if (ps2m_lock || (oflag & O_LOCK) && ps2m_nopen)
		{ u.u_error = EBUSY; return; }

	if (ps2m_nopen > 0)
		{ ttyopen (dev, tp, oflag); ps2m_nopen++; return; }

	/*
	 *	Liga o camundongo
	 */
	splps2m ();

	if (kbd_enable_aux_dev () < 0)
	{
		printf ("%g: Não consegui ligar o camundongo\n");
		{ spl0 (); u.u_error = ENXIO; return; }
	}

	if (kbd_get_mouse_status (status, 0, 3) < 3)
	{
		printf ("%g: Não consegui obter o estado do camundongo\n");
		{ spl0 (); u.u_error = ENXIO; return; }
	}

	if ((command_byte = kbd_get_controller_command_byte ()) < 0)
		{ spl0 (); u.u_error = ENXIO; return; }

	if
	(	kbd_set_controller_command_byte
		(
			KBD_KBD_CONTROL_BITS|KBD_AUX_CONTROL_BITS,
	  		KBD_ENABLE_KBD_PORT|KBD_ENABLE_KBD_INT|KBD_ENABLE_AUX_PORT|KBD_ENABLE_AUX_INT
		) < 0
	)
	{
		printf ("%g: Não consegui atribuir o comando\n");
		kbd_restore_controller (command_byte);
		{ spl0 (); u.u_error = ENXIO; return; }
	}

	/*
	 *	Prepara a estrutura
	 */
	if ((tp->t_state & ISOPEN) == 0)
	{
		tp->t_oproc  = ps2mstart;

		tp->t_iflag  = ICRNL|IXON;
		tp->t_oflag  = OPOST|ONLCR;
		tp->t_cflag  = B9600|CS8|CLOCAL;
		tp->t_lflag  = ISIG|ICANON|ECHO|ECHOE|ECHOK|CNTRLX|ISO|VIDEO;

		tp->t_nline  = 24;	/* Valores iniciais -std- */
		tp->t_ncol   = 80;
	}

	ttyopen (dev, tp, oflag);

	spl0 ();

	/*
	 *	Epílogo
	 */
	ps2m_nopen = 1;

	if (oflag & O_LOCK)
		ps2m_lock = 1;

}	/* end ps2mopen */

/*
 ****************************************************************
 *	Função de "close" do camundongo				*
 ****************************************************************
 */
void
ps2mclose (dev_t dev, int flag)
{
	TTY		*tp = &ps2m_tty;

	/*
	 *	Prólogo
	 */
	if (flag == 0)
		ttyclose (tp);

	if (--ps2m_nopen > 0)
		return;

	/*
	 *	Desabilita o camundongo
	 */
#if (0)	/*******************************************************/
	splps2m ();
#endif	/*******************************************************/
	splx (irq_pl_vec[PS2M_IRQ]);

	if
	(	kbd_set_controller_command_byte
		(
			KBD_KBD_CONTROL_BITS|KBD_AUX_CONTROL_BITS,
	  		KBD_DISABLE_KBD_PORT|KBD_DISABLE_KBD_INT|KBD_ENABLE_AUX_PORT|KBD_DISABLE_AUX_INT
		) < 0
	)
	{
		printf ("%g: Não consegui atribuir o comando\n");
	}

	spl0 ();

	kbd_empty_aux_buffer (10);

	if (kbd_disable_aux_dev () < 0)
		printf ("%g: Não consegui desligar o camundongo\n");

	if
	(	kbd_set_controller_command_byte
		(
			KBD_KBD_CONTROL_BITS|KBD_AUX_CONTROL_BITS,
	  		KBD_ENABLE_KBD_PORT|KBD_ENABLE_KBD_INT|KBD_DISABLE_AUX_PORT|KBD_DISABLE_AUX_INT
		) < 0
	)
	{
		printf ("%g: Não consegui atribuir o comando\n");
	}

	kbd_empty_aux_buffer (10);

	/*
	 *	Epílogo
	 */
	ps2m_lock = 0;

}	/* end ps2mclose */

/*
 ****************************************************************
 *	Função de "read"					*
 ****************************************************************
 */
void
ps2mread (IOREQ *iop)
{
	ttyread (iop, &ps2m_tty);

}	/* end ps2mread */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
ps2mwrite (IOREQ *iop)
{
	ttywrite (iop, &ps2m_tty);

}	/* end ps2mwrite */

/*
 ****************************************************************
 *	Função de IOCTL						*
 ****************************************************************
 */
int
ps2mctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY		*tp = &ps2m_tty;

	/*
	 *	Tratamento especial para certos comandos
	 */
	switch (cmd)
	{
		/*
		 *	Processa/prepara o ATTENTION
		 */
	    case U_ATTENTION:
#if (0)	/*******************************************************/
		splps2m ();
#endif	/*******************************************************/
		splx (irq_pl_vec[PS2M_IRQ]);

		if (EVENTTEST (&tp->t_inqnempty) == 0)
			{ spl0 (); return (1); }

		tp->t_uproc  = u.u_myself;
		tp->t_index = (int)arg;

		tp->t_attention_set = 1;
		*(char **)flag	= &tp->t_attention_set;

		spl0 ();

		return (0);

	}	/* end switch */

	/*
	 *	Demais IOCTLs
	 */
	return (ttyctl (tp, cmd, arg));

}	/* end ps2mctl */

/*
 ****************************************************************
 *	Inicia a escrita					*
 ****************************************************************
 */
void
ps2mstart (TTY *tp, int flag)
{
	int		c;

	SPINLOCK (&tp->t_outq.c_lock);

	while ((c = cget (&tp->t_outq)) >= 0)
		/* vazio */;

	SPINFREE (&tp->t_outq.c_lock);

   /***	CLEAR (&tp->t_obusy);	***/

	EVENTDONE (&tp->t_outqempty);

}	/* end ps2mstart */

/*
 ****************************************************************
 *	Logitech MouseMan+/FirstMouse+, IBM ScrollPoint Mouse	*
 ****************************************************************
 */
int
kbd_enable_mmanplus (void)
{
	int		data[3];

	/* the special sequence to enable the fourth button and the roller. */
	/*
	 * NOTE: for ScrollPoint to respond correctly, the SET_RESOLUTION
	 * must be called exactly three times since the last RESET command
	 * before this sequence. XXX
	 */
	if (kbd_set_mouse_scaling (1) < 0)
		return (-1);

	if (kbd_mouse_ext_command (0x39) < 0 || kbd_mouse_ext_command (0xDB) < 0)
		return (-1);

	if (kbd_get_mouse_status (data, 1, 3) < 3)
		return (-1);

	/*
	 * PS2++ protocl, packet type 0
	 *
	 *	    b7 b6 b5 b4 b3 b2 b1 b0
	 * byte 1:  *  1  p3 p2 1  *  *  *
	 * byte 2:  1  1  p1 p0 m1 m0 1  0
	 * byte 3:  m7 m6 m5 m4 m3 m2 m1 m0
	 *
	 * p3-p0: packet type: 0
	 * m7-m0: model ID: MouseMan+:0x50, FirstMouse+:0x51, ScrollPoint:0x58...
	 */

	/* check constant bits */

	if ((data[0] & MOUSE_PS2PLUS_SYNCMASK) != MOUSE_PS2PLUS_SYNC)
		return (-1);

	if ((data[1] & 0xc3) != 0xc2)
		return (-1);

	/* check d3-d0 in byte 2 */

	if (!MOUSE_PS2PLUS_CHECKBITS (data))
		return (-1);

	/* check p3-p0 */

	if (MOUSE_PS2PLUS_PACKET_TYPE (data) != 0)
		return (-1);

#if (0)	/*******************************************************/
	sc->hw.hwid &= 0x00ff;
	sc->hw.hwid |= data[2] << 8;	/* save model ID */
#endif	/*******************************************************/
	/*
	 * MouseMan+ (or FirstMouse+) is now in its native mode, in which
	 * the wheel and the fourth button events are encoded in the
	 * special data packet. The mouse may be put in the IntelliMouse mode
	 * if it is initialized by the IntelliMouse's method.
	 */
	return (0);

}	/* end kbd_enable_mmanplus */

/*
 ****************************************************************
 *	Restaura o modo anterior do controlador			*
 ****************************************************************
 */
void
kbd_restore_controller (int command_byte)
{
	kbd_empty_both_buffers (10);

	kbd_set_controller_command_byte (0xFF, command_byte);

	kbd_empty_both_buffers (10);

}	/* end kbd_restore_controller */

/*
 ****************************************************************
 *	Testa a porta do camundongo				*
 ****************************************************************
 */
int
kbd_test_aux_port (void)
{
	int		retry, again, c;

	for (retry = KBD_MAXRETRY; /* abaixo */; retry--)
	{
		if (retry <= 0)
			return (-1);

		kbd_empty_both_buffers (10);

		if (kbd_write_controller_command (KBDC_TEST_AUX_PORT) >= 0)
			break;
	}

	kbd_emptyq (&kbd_kbd_q);

	for (again = KBD_MAXRETRY; again > 0; again--)
	{
		if ((c = kbd_read_controller_data ()) >= 0)
			break;
	}

	return (c);

}	/* end kbd_test_aux_port */

/*
 ****************************************************************
 *	Inicializa a porta do camundongo			*
 ****************************************************************
 */
int
kbd_reset_aux_dev (void)
{
	int		retry, again, c = PSM_RESEND;

	for (retry = KBD_MAXRETRY; /* abaixo */; retry--)
	{
		if (retry <= 0)
			return (-1);

		kbd_empty_both_buffers (10);

		if (kbd_write_aux_command (PSMC_RESET_DEV) < 0)
			continue;

		kbd_emptyq (&kbd_mouse_q);

		for (again = KBD_MAXWAIT * 100; again > 0; again--)
		{
			if ((c = kbd_read_aux_data_no_wait ()) >= 0)
				break;

			DELAY (KBD_RESETDELAY * 10);
		}

#if (0)	/*******************************************************/
		for (again = KBD_MAXWAIT; again > 0; again--)
		{
			if ((c = kbd_read_aux_data_no_wait ()) >= 0)
				break;

			DELAY (KBD_RESETDELAY * 1000);
		}
#endif	/*******************************************************/

		if (c == PSM_ACK)	/* aux dev is about to reset... */
			break;
	}

	for (again = KBD_MAXWAIT * 100; again > 0; again--)
	{
		if ((c = kbd_read_aux_data_no_wait ()) >= 0)	/* RESET_DONE/RESET_FAIL */
			break;

		DELAY (KBD_RESETDELAY * 10);
	}

#if (0)	/*******************************************************/
	for (again = KBD_MAXWAIT; again > 0; again--)
	{
		DELAY (KBD_RESETDELAY * 1000);

		if ((c = kbd_read_aux_data_no_wait ()) >= 0)	/* RESET_DONE/RESET_FAIL */
			break;
	}
#endif	/*******************************************************/

	if (c != PSM_RESET_DONE)	/* reset status */
		return (-1);

	c = kbd_read_aux_data ();	/* device ID */

	return (0);

}	/* end kbd_reset_aux_dev */

/*
 ****************************************************************
 *	Liga o dispositivo auxiliar				*
 ****************************************************************
 */
int
kbd_enable_aux_dev (void)
{
	return (kbd_send_aux_command (PSMC_ENABLE_DEV) == PSM_ACK);

}	/* end kbd_enable_aux_dev */

/*
 ****************************************************************
 *	Desliga o dispositivo auxiliar				*
 ****************************************************************
 */
int
kbd_disable_aux_dev (void)
{
	return (kbd_send_aux_command (PSMC_DISABLE_DEV) == PSM_ACK);

}	/* end kbd_disable_aux_dev */

/*
 ****************************************************************
 *	Obtém o estado do camundongo				*
 ****************************************************************
 */
int
kbd_get_mouse_status (int *status, int flag, int len)
{
	int		cmd, res, index;

	switch (flag)
	{
	    case 0:
	    default:
		cmd = PSMC_SEND_DEV_STATUS;
		break;

	    case 1:
		cmd = PSMC_SEND_DEV_DATA;
		break;
	}

	kbd_empty_aux_buffer (5);

	if ((res = kbd_send_aux_command (cmd)) != PSM_ACK)
		return (-1);

	for (index = 0; index < len; index++)
	{
		status[index] = kbd_read_aux_data ();

		if (status[index] < 0)
			break;
	}

	return (index);

}	/* end kbd_get_mouse_status */

/*
 ****************************************************************
 *	Envia um comando estendido				*
 ****************************************************************
 */
int 
kbd_mouse_ext_command (int command)
{
	int		c;

	c = (command >> 6) & 0x03;

	if (kbd_set_mouse_resolution (c) != c)
		return (-1);

	c = (command >> 4) & 0x03;

	if (kbd_set_mouse_resolution (c) != c)
		return (-1);

	c = (command >> 2) & 0x03;

	if (kbd_set_mouse_resolution (c) != c)
		return (-1);

	c = (command >> 0) & 0x03;

	if (kbd_set_mouse_resolution (c) != c)
		return (-1);

	return (0);

}	/* end kbd_mouse_ext_command */

/*
 ****************************************************************
 *	Obtém a identificação do camundongo			*
 ****************************************************************
 */
int
kbd_get_aux_id (void)
{
	kbd_empty_aux_buffer (5);

	if (kbd_send_aux_command (PSMC_SEND_DEV_ID) != PSM_ACK)
		return (-1);

	DELAY (10000); 		/* 10 ms delay */

	return (kbd_read_aux_data ());

}	/* end kbd_get_aux_id */

/*
 ****************************************************************
 *	Atribui o fator de escala do camundongo			*
 ****************************************************************
 */
int
kbd_set_mouse_scaling (int scale)
{
	switch (scale)
	{
	    case 1:
	    default:
		scale = PSMC_SET_SCALING11;
		break;

	    case 2:
		scale = PSMC_SET_SCALING21;
		break;
	}

	if (kbd_send_aux_command (scale) == PSM_ACK)
		return (0);
	else
		return (-1);

}	/* end kbd_set_mouse_scaling */

/*
 ****************************************************************
 *	Atribui a resolução do camundongo			*
 ****************************************************************
 */
int
kbd_set_mouse_resolution (int val)
{
	int		res;

	res = kbd_send_aux_command_and_data (PSMC_SET_RESOLUTION, val);

	return ((res == PSM_ACK) ? val : -1);

}	/* end kbd_set_mouse_resolution */

/*
 ****************************************************************
 *	Obtém o número de botões do camundongo			*
 ****************************************************************
 */
int
kbd_get_mouse_buttons (void)
{
	int		nbuttons = 2;		/* assume two buttons by default */
	int		status[3];

	if (kbd_set_mouse_resolution (PSMD_RES_LOW) != PSMD_RES_LOW)
		return (nbuttons);

	if
	(	kbd_set_mouse_scaling (1) >= 0 && kbd_set_mouse_scaling (1) >= 0 &&
		kbd_set_mouse_scaling (1) >= 0 && kbd_get_mouse_status (status, 0, 3) >= 3
	)
	{
		if (status[1] != 0)
			return status[1];
	}

	return (nbuttons);

}	/* end kbd_get_mouse_buttons */

/*
 ****************************************************************
 *	Esvazia as duas áreas de entrada			*
 ****************************************************************
 */
void
kbd_empty_both_buffers (int wait)
{
	char		status;
	int		t;

	for (t = wait; t > 0; /* abaixo */)
	{ 
		if ((status = read_port (KBD_STATUS_PORT)) & KBDS_ANY_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME); read_port (KBD_DATA_PORT); t = wait;
		}
		else
		{
			t -= 2;
		}

		DELAY (2000);
	}

	kbd_emptyq (&kbd_kbd_q);
	kbd_emptyq (&kbd_mouse_q);

}	/* end kbd_empty_both_buffers */

/*
 ****************************************************************
 *	Esvazia a área auxiliar					*
 ****************************************************************
 */
void
kbd_empty_aux_buffer (int wait)
{
	char		status, data;
	int		t;

	for (t = wait; t > 0; /* abaixo */)
	{ 
		if ((status = read_port (KBD_STATUS_PORT)) & KBDS_ANY_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME); data = read_port (KBD_DATA_PORT); t = wait;

			if ((status & KBDS_BUFFER_FULL) == KBDS_KBD_BUFFER_FULL)
				kbd_putq (&kbd_kbd_q, data);
		}
		else
		{
			t -= 2;
		}

		DELAY (2000);
	}

	kbd_emptyq (&kbd_mouse_q);

}	/* end kbd_empty_aux_buffer */

/*
 ****************************************************************
 *	Obtém o valor do registro de comando			*
 ****************************************************************
 */
int
kbd_get_controller_command_byte (void)
{
	if (kbd_command_byte != -1)
		return (kbd_command_byte);

	if (kbd_write_controller_command (KBDC_GET_COMMAND_BYTE) < 0)
		return (-1);

	kbd_emptyq (&kbd_kbd_q);

	kbd_command_byte = kbd_read_controller_data ();

	return (kbd_command_byte);

}	/* end kbd_get_controller_command_byte */

/*
 ****************************************************************
 *	Atribui o valor do registro de comando			*
 ****************************************************************
 */
int
kbd_set_controller_command_byte (int mask, int command)
{
	if (kbd_get_controller_command_byte () == -1)
		return (-1);

	command = (kbd_command_byte & ~mask) | (command & mask);

	if (command & KBD_DISABLE_KBD_PORT)
	{
		if (kbd_write_controller_command (KBDC_DISABLE_KBD_PORT) < 0)
			return (-1);
	}

	if (kbd_write_controller_command (KBDC_SET_COMMAND_BYTE) < 0)
		return (-1);

	if (kbd_write_controller_data (command) < 0)
		return (-1);

	kbd_command_byte = command;

	return (0);

}	/* end kbd_set_controller_command_byte */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
kbd_write_controller_command (int command)
{
	if (kbd_wait_while_controller_busy () < 0)
		return (-1);

	write_port (command, KBD_COMMAND_PORT);

	return (0);

}	/* end kbd_write_controller_command */

/*
 ****************************************************************
 *	Espera por um ACK, RESEND, RESET_FAIL, ...		*
 ****************************************************************
 */
int
kbd_wait_for_aux_ack (void)
{
	int		retry;
	char		status, data;

	for (retry = 10000; /* abaixo */; retry--)
	{
		if (retry <= 0)
			return (-1);

		if ((status = read_port (KBD_STATUS_PORT)) & KBDS_ANY_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME);

			data = read_port (KBD_DATA_PORT);

			if   ((status & KBDS_BUFFER_FULL) == KBDS_AUX_BUFFER_FULL)
			{
				if (data == PSM_ACK || data == PSM_RESEND || data == PSM_RESET_FAIL)
					return (data);

				kbd_putq (&kbd_mouse_q, data);
			}
			elif ((status & KBDS_BUFFER_FULL) == KBDS_KBD_BUFFER_FULL)
			{
				kbd_putq (&kbd_kbd_q, data);
			}
		}

		DELAY (KBDC_DELAYTIME);
	}

}	/* end kbd_wait_for_aux_ack */

/*
 ****************************************************************
 *	Espera até que o controlador esteja desocupado		*
 ****************************************************************
 */
int
kbd_wait_while_controller_busy (void)
{
	/* Tempo máximo = 100 msec */

	int		retry = 5000;
	char		status;

	while ((status = read_port (KBD_STATUS_PORT)) & KBDS_INPUT_BUFFER_FULL)
	{
		if   ((status & KBDS_BUFFER_FULL) == KBDS_KBD_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME); kbd_putq (&kbd_kbd_q, read_port (KBD_DATA_PORT));
		}
		elif ((status & KBDS_BUFFER_FULL) == KBDS_AUX_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME); kbd_putq (&kbd_mouse_q, read_port (KBD_DATA_PORT));
		}

		DELAY (KBDC_DELAYTIME);

		if (--retry < 0)
			return (-1);
	}

	return (0);

}	/* end kbd_wait_while_controller_busy */

/*
 ****************************************************************
 *	Le um byte do controlador 				*
 ****************************************************************
 */
int
kbd_read_controller_data (void)
{
	if (kbd_kbd_q.count != 0)
		return (kbd_getq (&kbd_kbd_q));

	if (kbd_mouse_q.count != 0)
		return (kbd_getq (&kbd_mouse_q));

	if (kbd_wait_for_data () < 0)
		return  (-1);		/* timeout */

	return (read_port (KBD_DATA_PORT));

}	/* end kbd_read_controller_data */

/*
 ****************************************************************
 *	Escreve um byte do controlador 				*
 ****************************************************************
 */
int
kbd_write_controller_data (int data)
{
	if (kbd_wait_while_controller_busy () < 0)
		return (-1);

	write_port (data, KBD_DATA_PORT);

	return (0);

}	/* end kbd_write_controller_data */

/*
 ****************************************************************
 *	Escreve um commando na porta auxiliar			*
 ****************************************************************
 */
int
kbd_write_aux_command (int command)
{
	if (kbd_write_controller_command (KBDC_WRITE_TO_AUX) < 0)
		return (-1);

	return (kbd_write_controller_data (command));

}	/* end kbd_write_aux_command */

/*
 ****************************************************************
 *	Le um byte da porta auxiliar				*
 ****************************************************************
 */
int
kbd_read_aux_data (void)
{
	if (kbd_mouse_q.count != 0)
		return (kbd_getq (&kbd_mouse_q));

	if (kbd_wait_for_aux_data () < 0)
		return (-1);		/* timeout */

	return (read_port (KBD_DATA_PORT));

}	/* end kbd_read_aux_data */

/*
 ****************************************************************
 *	Le um byte da porta auxiliar sem esperar		*
 ****************************************************************
 */
int
kbd_read_aux_data_no_wait (void)
{
	char		status;

	if (kbd_mouse_q.count != 0)
		return (kbd_getq (&kbd_mouse_q));

	status = read_port (KBD_STATUS_PORT) & KBDS_BUFFER_FULL;

	if (status == KBDS_KBD_BUFFER_FULL)
	{
		DELAY (KBDD_DELAYTIME); kbd_putq (&kbd_kbd_q, read_port (KBD_DATA_PORT));

		status = read_port (KBD_STATUS_PORT) & KBDS_BUFFER_FULL;
	}

	if (status == KBDS_AUX_BUFFER_FULL)
	{
		DELAY (KBDD_DELAYTIME); return (read_port (KBD_DATA_PORT));
	}

	return (-1);		/* no data */

}	/* end kbd_read_aux_data_no_wait */

/*
 ****************************************************************
 *	Espera até haver dados na porta auxiliar		*
 ****************************************************************
 */
int
kbd_wait_for_aux_data (void)
{
	/* CPU will stay inside the loop for 200msec at most */

	int		retry = 10000;
	char		status;

	while ((status = read_port (KBD_STATUS_PORT) & KBDS_BUFFER_FULL) != KBDS_AUX_BUFFER_FULL)
	{
		if (status == KBDS_KBD_BUFFER_FULL)
		{
			DELAY (KBDD_DELAYTIME); kbd_putq (&kbd_kbd_q, read_port (KBD_DATA_PORT));
		}

		DELAY (KBDC_DELAYTIME);

		if (--retry < 0)
			return 0;
	}

	DELAY(KBDD_DELAYTIME);

	return (status);

}	/* end kbd_wait_for_aux_data */

/*
 ****************************************************************
 *	Espera haver dados em qualquer fonte			*
 ****************************************************************
 */
int
kbd_wait_for_data (void)
{
	/* CPU will stay inside the loop for 200msec at most */

	int		retry = 10000;
	char		status;

	while ((status = read_port (KBD_STATUS_PORT) & KBDS_ANY_BUFFER_FULL) == 0)
	{
		DELAY (KBDC_DELAYTIME);

		if (--retry < 0)
			return (-1);
	}

	DELAY (KBDD_DELAYTIME);

	return (status);

}	/* end kbd_wait_for_data */

/*
 ****************************************************************
 *	Envia um comando para a porta auxiliar e espera um ACK	*
 ****************************************************************
 */
int
kbd_send_aux_command (int command)
{
	int		retry, res = -1;

	for (retry = KBD_MAXRETRY; /* abaixo */; retry--)
	{
		if (retry <= 0)
			return (-1);

		if (kbd_write_aux_command (command) < 0)
			continue;

		kbd_emptyq (&kbd_mouse_q);

		if ((res = kbd_wait_for_aux_ack ()) == PSM_ACK)
			break;
	}

	return (res);

}	/* end kbd_send_aux_command */

/*
 ****************************************************************
 *	Envia um comando e um dado para a porta auxiliar	*
 ****************************************************************
 */
int
kbd_send_aux_command_and_data (int command, int data)
{
	int		retry, res = -1;

	for (retry = KBD_MAXRETRY; /* abaixo */; retry--)
	{
		if (retry <= 0)
			return (res);

		if (kbd_write_aux_command (command) < 0)
			continue;

		kbd_emptyq (&kbd_mouse_q);

		if   ((res = kbd_wait_for_aux_ack ()) == PSM_ACK)
			break;
		elif (res != PSM_RESEND)
			return (res);
	}

	for (retry = KBD_MAXRETRY, res = -1; retry > 0; retry--)
	{
		if (kbd_write_aux_command (data) < 0)
			continue;

		if ((res = kbd_wait_for_aux_ack ()) != PSM_RESEND)
			break;
	}

	return (res);

}	/* end kbd_send_aux_command_and_data */

/*
 ****************************************************************
 *	Coloca um caractere na fila				*
 ****************************************************************
 */
int
kbd_putq (KBDQ *q, int c)
{
	if (q->count >= KBDQ_BUFSIZE)
		return (-1);

	if (q->tail >= &q->q[KBDQ_BUFSIZE])
		q->tail = &q->q[0];

	*q->tail++ = c; q->count++;

	return (0);

}	/* end kbd_putq */

/*
 ****************************************************************
 *	Tira um caractere na fila				*
 ****************************************************************
 */
int
kbd_getq (KBDQ *q)
{
	int		c;

	if (q->count <= 0)
		return (-1);

	if (q->head >= &q->q[KBDQ_BUFSIZE])
		q->head = &q->q[0];

	c = *q->head++; q->count--;

	return (c);

}	/* end kbd_getq */

/*
 ****************************************************************
 *	Esvazia um fila						*
 ****************************************************************
 */
void
kbd_emptyq (KBDQ *q)
{
	q->count = 0;

	q->head = &q->q[0];
	q->tail = &q->q[0];

}	/* end kbd_getq */
