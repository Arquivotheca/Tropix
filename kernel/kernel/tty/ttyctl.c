/*
 ****************************************************************
 *								*
 *			ttyctl.c				*
 *								*
 *	Interface geral para terminais (controle)		*
 *								*
 *	Versão	3.0.00, de 15.12.94				*
 *		3.0.28, de 22.01.98				*
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

#include "../h/tty.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define isupper(c)	(chartb[c] & 0x0080)
#define	ischave(c)	(chartb[c] & 0x0020)
#define	isespec(c)	(chartb[c] & 0x0008)

/*
 ****************************************************************
 *	Código de IOCTL para terminais				*
 ****************************************************************
 */
int
ttyctl (register TTY *tp, int cmd, void *arg)
{
	TERMIO	termio;

#ifdef	MSG
	if (CSWT (8))
		printf ("%g: dev = %X, cmd = %X\n", tp->t_dev, cmd);
#endif	MSG

	/*
	 *	Note que "t_olock" vem e volta LOCKED
	 */
	switch (cmd)
	{
		/*
		 ****** Obtém a estrutura TERMIO ****************
		 */
	    case TCGETS:
		if (unimove (arg, &tp->t_termio, sizeof (TERMIO), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

		/*
		 ****** Atribui a estrutura TERMIO **************
		 *
		 *	esperando esvaziar a fila de Saída
		 */
	    case TCSETAW:
		EVENTWAIT (&tp->t_outqempty, PTTYOUT);
		goto settermio;

		/*
		 ****** Atribui a estrutura TERMIO **************
		 *
		 *	esperando esvaziar a fila de Saída
		 *	e esvaziar a Fila de Entrada
		 */
	    case TCSETAF:
		EVENTWAIT (&tp->t_outqempty, PTTYOUT);
		ttyiflush (tp);
		/* continua */

		/*
		 ****** Atribui a estrutura TERMIO **************
		 *
		 *	imediatamente
		 */
	    case TCSETS:
	    case TCSETNR:
	    settermio:
		if (unimove (&termio, arg, sizeof (TERMIO), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (streq (termio.c_name, tp->t_termio.c_name) == 0)
			{ u.u_error = EINVAL; return (-1); }

		unimove (&tp->t_termio, &termio, sizeof (TERMIO), SS);

		putcvtb (tp);

		tp->t_lno = 0;

		/*
		 *	Verifica o novo modo
		 */
		SPINLOCK (&tp->t_inq.c_lock);

		if ((tp->t_lflag & (ICANON|ICOMM)) != ICANON)
		{
			/* COMM ou RAW */

			if (tp->t_inq.c_count < tp->t_min)
				EVENTCLEAR (&tp->t_inqnempty);
			else
				EVENTDONE (&tp->t_inqnempty);

			tp->t_state &= ~(TTYSTOP|ESCAPE);
		}
		else
		{
			/* Cooked */

			if (tp->t_inq.c_count == 0)
				EVENTCLEAR (&tp->t_inqnempty);
			else
				EVENTDONE (&tp->t_inqnempty);
		}

		SPINFREE (&tp->t_inq.c_lock);

		return (0);

		/*
		 ****** Espera a Fila de Saída Esvaziar *********
		 */
	    case TCSBRK:
		EVENTWAIT (&tp->t_outqempty, PTTYOUT);
		return (0);

		/*
		 ****** Controle de START/STOP ******************
		 */
	    case TCXONC:
		if ((int)arg == 0)
		{
			tp->t_state |= TTYSTOP;
#ifdef	T_STOP
			(*ciotab[MAJOR(tp->t_dev)].w_stop) (tp);
#endif	T_STOP
		}
		else /* arg == 1 */
		{
			tp->t_state &= ~TTYSTOP;
			tp->t_lno = 0;
			(*tp->t_oproc) (tp, 0);
		}
		return (0);

		/*
		 ****** Esvazia as Filas ************************
		 */
	    case TCFLSH:
		if   ((int)arg == 0)
		{
			ttyiflush (tp);
		}
		elif ((int)arg == 1)
		{
			ttyoflush (tp);
		}
		else /* arg == 2 */
		{
			ttyiflush (tp);
			ttyoflush (tp);
		}
		return (0);

		/*
		 ****** Dá um LOCK na estrutura TERMIO **********
		 */
	    case TCLOCK:
#if (0)	/*******************************************************/
		if (superuser () < 0)
			return (-1);

		SLEEPLOCK (&tp->t_olock, PTTYOUT);
#endif	/*******************************************************/
		return (0);

		/*
		 ****** Dá um FREE na estrutura TERMIO **********
		 */
	    case TCFREE:
#if (0)	/*******************************************************/
		if (superuser () < 0)
			return (-1);

		if (SLEEPTEST (&tp->t_olock) >= 0)
			u.u_error = EINVAL;

		SLEEPFREE (&tp->t_olock);
#endif	/*******************************************************/
		return (UNDEF);

		/*
		 ****** Obtem o tamanho da Fila de Entrada ******
		 */
	    case TCNREAD:
		return (tp->t_inq.c_count);

		/*
		 ****** Verifica apenas se é TTY ****************
		 */
	    case TCISATTY:
		return (0);

	}	/* end switch */

	u.u_error = EINVAL;

	return (-1);

}	/* end ttyctl */

/*
 ****************************************************************
 *	Coloca o endereco da tabela de conversão		*
 ****************************************************************
 */
void
putcvtb (register TTY *tp)
{
	switch (tp->t_lflag & CODE)
	{
	    case ISO: 
		tp->t_cvtb = isotoisotb;
		break;

	    case ASCII: 
		tp->t_cvtb = isotoatb;
		break;

	    case USER1: 
		tp->t_cvtb = isotou1tb;
		break;

	    case USER2: 
		tp->t_cvtb = isotou2tb;
		break;
	}

}	/* end putcvtb */

/*
 ****************************************************************
 *	Apaga um Caracter no Terminal				*
 ****************************************************************
 */
void
ttywipe (register int c, register TTY *tp)
{
	register int	n;

	/*
	 *	É chamado depois de ter removido o caracter "c" da "inq".
	 *	Só deve ser usado se o terminal tiver <bs> funcionando.
	 */
	if ((tp->t_lflag & ECHO) == 0)
		return;

	if (c == CBELL || c == CFF)
		return;

	if (c < 0)
		{ ttyout (CBELL, tp); return; }

	/*
	 *	Obtem a largura do Caracter, e verifica se é <ht>.
	 */
	if ((n = ttylarg (c, tp)) == -2)
		n = tp->t_col - ttyretype (tp, 0);

	/*
	 *	Apaga os caracteres.
	 */
	while (n-- > 0)
	{
		ttyout ('\b', tp);

		if (tp->t_lflag & ECHOE)
			{ ttyout (' ',  tp); ttyout ('\b', tp); }
	}

}	/* end ttywipe */

/*
 ****************************************************************
 *    Calcula o nr. de colunas ocupadas por um caracter 	*
 ****************************************************************
 */
int
ttylarg (register int c, register TTY *tp)
{
	/*
	 *	Retorna:
	 *	 0 para Caracteres que ocupam 0 colunas,
	 *	 1 para caracteres que ocupam 1 coluna,
	 *	 2 para caracteres que ocupam 2 colunas,
	 *	-1 para  <bs>,
	 *	-2 para  <ht>, e
	 *	<bell> retorna 1.
	 */
	if (isespec (c))
	{
		if (tp->t_lflag & CNTRLX)
			return (2);
		else
			return (0);
	}

	if (c == '\t')
		return (-2);

	if (c == '\b')
		return (-1);

	if ((tp->t_lflag & XCASE) == 0)
		return (1);

	if (isupper (c) || ischave (c) || c == '\\')
		return (2);

	return (1);

}	/* end ttylarg */

/*
 ****************************************************************
 *	Reescreve a linha no terminal				*
 ****************************************************************
 */
int
ttyretype (register TTY *tp, register int flag)
{
	register char	*cp;
	register int	i, col, n;

	/*
	 *	O flag indica a função a executar:
	 *		<> 0:	Reescrever a linha
	 *		== 0:	Calcular a coluna atual
	 */
	cp = tp->t_inq.c_cf;
	col = tp->t_wcol;

	for (i = tp->t_inq.c_count; i--; /* vazio */)
	{
		if (flag)
		{
			/*
			 *	Escreve o Caracter.
			 */
			if (tp->t_lflag & ECHO)
			{
				if (*cp == DELIM)
					ttyout ('\n', tp);
				else
					ttyout (*cp,  tp);
			}

			cp++;
		}
		else
		{
			/*
			 *	Calcula a Coluna.
			 */
			if ((n = ttylarg ((int)*cp++, tp)) != -2)
				col += n;
			else
				col += 8 - (col & 7);
		}

		if (((int) cp & CLMASK) == 0)
			cp = (((CBLK *)cp)-1)->c_next->c_info;
	}

	return (col);

}	/* end ttyretype */

/*
 ****************************************************************
 *	Rotina de conversão de caracteres			*
 ****************************************************************
 */
int
ttyconv (register char *string, register char c)
{
	while  (*string++)
		if (c == *string++)
			return (string[-2]);

	return (c);

}	/* end ttyconv */
