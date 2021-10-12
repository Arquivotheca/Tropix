/*
 ****************************************************************
 *								*
 *			xcon.c					*
 *								*
 *	"Driver" para o vídeo no modo XWINDOWS			*
 *								*
 *	Versão	3.0.5, de 05.01.97				*
 *		4.9.0, de 13.08.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/frame.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/tty.h"
#include "../h/video.h"
#include "../h/ioctl.h"
#include "../h/intr.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições Globais				*
 ****************************************************************
 */
#define	CON_IRQ	1		/* No. do IRQ do teclado */
#define PL	1		/* Prioridade de interrupção */
#define splxcon	spl1		/* Função de prioridade de interrupção */

/*
 ******	Portas e definições do teclado **************************
 */
#define	KEY_STATUS	0x64	/* Registro de estado */
#define	KEY_DATA	0x60	/* Registro de dados */

#define	KEY_AVAIL	0x01	/* Caractere disponível no teclado */
#define	KEY_BUSY	0x02	/* Reg. de entrada não pronto */

/*
 ******	Estrutura TTY para o Modo XWINDOWS **********************
 */
entry TTY		xcon;

/*
 ******	Variáveis de outros módulos *****************************
 */
extern time_t		key_last_time;		/* em dev/con.c */

/*
 ******	Protótipos de funções ***********************************
 */
void		xconstart (TTY *, int);
void		xconint (struct intr_frame frame);

extern void	DELAY (int);
extern void	conint (void);
extern void	video_beep (int, int);
extern void	key_set_leds (int);

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
xconopen (dev_t dev, int oflag)
{
	TTY	*tp = &xcon;

	if (tp->t_state & ISOPEN)
		{ u.u_error = EBUSY; return; }

	tp->t_irq = CON_IRQ;

	/*
	 *	Desabilita o "screen saver".
	 *	Troca a rotina de interrupção do teclado.
	 */
	splx (irq_pl_vec[tp->t_irq]);

	scb.y_screen_saver_time = -scb.y_screen_saver_time;

	vecdef[tp->t_irq][0].vec_func = xconint;

	idle_intr_active = 0;

	video_data->video_mode |= 2;	/* Transforma 0/1 em 2/3 */

	spl0 ();

/***	tp->t_oproc = ...;
	tp->t_iflag = ...;
	tp->t_oflag = ...;
	tp->t_cflag = ...;	***/
	tp->t_lflag = ICOMM;

	ttyopen (dev, tp, oflag);

}	/* end xconopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
xconclose (dev_t dev, int flag)
{
	TTY		*tp;

	if (flag != 0)
		return;

	tp = &xcon; ttyclose (tp);

	/*
	 *	Restaura o "screen saver".
	 *	Restaura a rotina de interrupção original do teclado.
	 */
	splx (irq_pl_vec[tp->t_irq]);

	scb.y_screen_saver_time = -scb.y_screen_saver_time;
	key_last_time		= time;

	vecdef[tp->t_irq][0].vec_func = conint;

	video_data->video_mode &= ~2;	/* Transforma 2/3 em 0/1 */

	if (video_data->video_mode == 0)
		idle_intr_active = 1;

	spl0 ();

}	/* end xconclose */

/*
 ****************************************************************
 *	Função  de "read"					*
 ****************************************************************
 */
void
xconread (IOREQ *iop)
{
	ttyread (iop, &xcon);

}	/* end xconread */

/*
 ****************************************************************
 *	Função  de "ioctl"					*
 ****************************************************************
 */
int
xconctl (dev_t dev, int cmd, void *arg, int flag)
{
	TTY	*tp;
	int	pitch;

	tp = &xcon;

	switch (cmd)
	{
		/*
		 ******	Processa/prepara o ATTENTION ************
		 */
	    case U_ATTENTION:
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
		 ******	Desarma o ATTENTION *********************
		 */
	    case U_UNATTENTION:
		tp->t_state &= ~ATTENTION;

		return (0);
#endif	UNATTENTION

		/*
		 ******	Apita (por enquanto, sem parâmetros) ****
		 */
	    case XC_BEEP:
		pitch = (int)arg;
		if (pitch <= 0)
			pitch = 880;

		video_beep (1193180 / pitch, scb.y_hz >> 2);
		return (0);

		/*
		 ****** Atualiza os leds do teclado *************
		 */
	    case XC_SETLEDS:
		key_set_leds ((int)arg);
		return (0);

		/*
		 ******	Esvazia a fila de entrada ***************
		 */
	    case TCFLSH:
		ttyiflush (tp);
		return (0);

		/*
		 ****** Obtém o tamanho da Fila de Entrada ******
		 */
	    case TCNREAD:
		return (tp->t_inq.c_count);

		/*
		 ****** Verifica apenas se é TTY ****************
		 */
	    case TCISATTY:
		break;

	    default:
		u.u_error = EINVAL;
		break;

	}	/* end switch */

	return (-1);

}	/* end xconctl */

/*
 ****************************************************************
 *	Função de interrupção do teclado			*
 ****************************************************************
 */
void
xconint (struct intr_frame frame)
{
	int		i;

	/*
	 *	Espera o teclado estar pronto
	 */
	for (i = 1000; i > 0; i--)
	{
		if ((read_port (KEY_STATUS) & KEY_BUSY) == 0)
			break;

		DELAY (10);
	}

#if (0)	/*******************************************************/
	/*
	 *	Lê o caractere e retorna sem conversão.
	 */
	if (!(read_port (KEY_STATUS) & KEY_AVAIL))
		return;
#endif	/*******************************************************/

	ttyin (read_port (KEY_DATA), &xcon);

}	/* end xconint */
