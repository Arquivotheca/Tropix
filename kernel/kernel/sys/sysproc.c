/*
 ****************************************************************
 *								*
 *			sysproc.c				*
 *								*
 *	Chamadas ao sistema: estados dos processos		*
 *								*
 *	Versão	3.0.0, de 20.12.94				*
 *		4.6.0, de 03.08.04				*
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

#include "../h/default.h"
#include "../h/tty.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições Globais 				*
 ****************************************************************
 */
entry EVENT	impossibleevent;	/* EVENTO que nunca acontecerá */

/*
 ****************************************************************
 *	Chamada ao sistema "nice"				*
 ****************************************************************
 */
int
nice (int incnice)
{
	int		new, old;

	/*
	 *	A troca de sinal é necessária pela convenção NÃO
	 *	intuitiva da chamada ao sistema "nice"
	 */
	new = -incnice;

	/*
	 *	Somente o SUPERUSER pode aumentar o NICE
	 */
	if (new > 0 && superuser () < 0)
		return (-1);

	/*
	 *	Calcula o novo NICE
	 */
	old = u.u_nice; 	new += old;

	if (new > NBASE)
		new = NBASE;
	if (new < -NBASE)
		new = -NBASE;

	u.u_nice = new;
	u.u_pri += NFACTOR * (new - old); 

	return (NBASE - new);

}	/* end nice */

/*
 ****************************************************************
 *	Chamada ao sistema "signal"				*
 ****************************************************************
 */
int
ksignal (int signo, void (*func) (int, ...), void (*sigludium) ())
{
	void		(*oldfunc) (int, ...);

#ifdef	MSG
	if (CSWT (15))
		printf ("%g: signo = %d, func = %P, sigludium = %P\n", signo, func, sigludium);
#endif	MSG

	/*
	 *	Analisa a validade do sinal
	 */
	if (signo <= 0 || signo > NSIG || signo == SIGKILL)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Guarda a nova função
	 */
	u.u_sigludium	  = sigludium;
	oldfunc		  = u.u_signal[signo];
	u.u_signal[signo] = func;
	u.u_sig		 &= ~(1 << signo);

	return ((int)oldfunc);

}	/* end ksignal */

/*
 ****************************************************************
 *	Chamada ao sistema "sigchild"				*
 ****************************************************************
 */
int
sigchild (long pid)
{
	UPROC		*up;

	/*
	 *	Procura o filho dado
	 */
	for (up = u.u_child; /* abaixo */; up = up->u_sibling)
	{
		if (up == NOUPROC)
			{ u.u_error = ECHILD; return (-1); }

		if (up->u_pid == pid)
			{ up->u_flags |= SIGEXIT; return (0); }
	}

}	/* end sigchild */

/*
 ****************************************************************
 *	Chamada ao sistema "kill"				*
 ****************************************************************
 */
int
kill (int pid, int signo)
{
	const UPROCV	*uvp;
	UPROC		*up;

	/*
	 *	Verifica se o sinal é válido
	 */
	if ((unsigned)signo > NSIG)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Analisa para quais processos enviar o sinal
	 */
	if (pid > 0)						/* Processo individual */
	{
		if (pid == 1)
			pid = scb.y_initpid;

		if (pid < scb.y_initpid)
			{ u.u_error = EINVAL; return (-1); }

		for (uvp = &scb.y_uproc[scb.y_initpid]; /* abaixo */; uvp++)
		{
			if (uvp > scb.y_lastuproc)
				{ u.u_error = ESRCH; return (-1); }

			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			if (up->u_pid == pid)
				{ sigproc (up, signo); return (UNDEF); }
		}
	}
	elif (pid == 0)						/* Grupo implícito */
	{
		for (uvp = &scb.y_uproc[scb.y_initpid + 1]; uvp <= scb.y_lastuproc; uvp++)
		{
			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			if (up->u_pgrp == u.u_pgrp)
				sigproc (up, signo);
		}
	}
	elif (pid < -1)						/* Grupo dado */
	{
		for (uvp = &scb.y_uproc[scb.y_initpid]; uvp <= scb.y_lastuproc; uvp++)
		{
			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			if (up->u_pgrp == -pid)
				sigproc (up, signo);
		}
	}
	elif (/* pid == -1 && */ u.u_euid == 0)			/* Universal */
	{
		for (uvp = &scb.y_uproc[scb.y_initpid]; uvp <= scb.y_lastuproc; uvp++)
		{
			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			sigproc (up, signo);
		}
	}
	else /* pid == -1 && u.u_euid != 0 */			/* Dono implícito */
	{
		for (uvp = &scb.y_uproc[scb.y_initpid]; uvp <= scb.y_lastuproc; uvp++)
		{
			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			if (u.u_euid == up->u_ruid)
				sigproc (up, signo);
		}
	}

	return (UNDEF);

}	/* end kill */

/*
 ****************************************************************
 *	Chamada ao sistema "alarm"				*
 ****************************************************************
 */
int
alarm (int delta)
{
	int 		olddelta;

	olddelta = u.u_alarm; u.u_alarm = delta;

	return (olddelta);

}	/* end alarm */

/*
 ****************************************************************
 *	Chamada ao sistema "pause"				*
 ****************************************************************
 */
void
pause (void)
{
	for (EVER)
	{
		EVENTCLEAR (&impossibleevent);
		EVENTWAIT  (&impossibleevent, PPAUSE);
	}

}	/* end pause */

/*
 ****************************************************************
 *	Chamada ao sistema "getpid"				*
 ****************************************************************
 */
long
getpid (void)
{
	return (u.u_pid);

}	/* end getpid */

/*
 ****************************************************************
 *	Chamada ao sistema "getppid"				*
 ****************************************************************
 */
long
getppid (void)
{
	return (u.u_parent->u_pid);

}	/* end getppid */

/*
 ****************************************************************
 *	Chamada ao sistema "setppid"				*
 ****************************************************************
 */
long
setppid (long ppid)
{
	UPROC		*up, *back_up, *par_up;
	UPROC		*initp;

	/*
	 *	Introdução: consistências
	 */
	if (superuser () < 0)
		return (-1);

	initp = scb.y_uproc[scb.y_initpid].u_uproc;

	if (ppid != 0 || u.u_pid <= scb.y_initpid)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Verifica se o processo já não é um dos filhos do INIT
	 */
	if ((par_up = u.u_parent) == initp)
		return (0);

	/*
	 *	Retira este processo da lista de filhos do seu pai original
	 */
	SPINLOCK (&par_up->u_childlock);

	for (back_up = NOUPROC, up = par_up->u_child; /* abaixo */; back_up = up, up = up->u_sibling)
	{
		if (up == NOUPROC)
			{ printf ("%g: Não achei um processo na fila de filhos do pai\n"); break; }

		if (up == u.u_myself)
		{
			if (back_up == NOUPROC)
				par_up->u_child	   = u.u_sibling;
			else
				back_up->u_sibling = u.u_sibling;
			break;
		}
	}

	SPINFREE (&par_up->u_childlock);

	/*
	 *	Insere este Processo como filho do INIT
	 */
	SPINLOCK (&initp->u_childlock);

	u.u_sibling = initp->u_child; initp->u_child = u.u_myself;

	u.u_parent = initp;

	SPINFREE (&initp->u_childlock);

	return (0);

}	/* end setppid */

/*
 ****************************************************************
 *	Chamada ao sistema "getpgrp"				*
 ****************************************************************
 */
long
getpgrp (void)
{
	return (u.u_pgrp);

}	/* end getpgrp */

/*
 ****************************************************************
 *	Chamada ao sistema "setpgrp"				*
 ****************************************************************
 */
int
setpgrp (void)
{
	return (u.u_pgrp = u.u_pid);

}	/* end setpgrp */

/*
 ****************************************************************
 *	Chamada ao sistema "settgrp"				*
 ****************************************************************
 */
int
settgrp (int flag)
{
	/*
	 *	Introdução: consistências
	 */
	if (superuser () < 0)
		return (-1);

	/*
	 *	Analisa a função
	 */
	if (flag == 0)		/* Desconecta o processo da linha */
	{
		u.u_tgrp   = NULL;
		u.u_tty    = NOTTY;
		u.u_ttydev = NODEV;
	}
	else			/* Atribui o "tgrp" da linha */
	{
		if (u.u_tty == NOTTY)
			{ u.u_error = EINVAL; return (-1); }

		u.u_tty->t_tgrp = u.u_tgrp = u.u_pid;
	}

	return (0);

}	/* end settgrp */

/*
 ****************************************************************
 *	Chamada ao sistema "plock"				*
 ****************************************************************
 */
int
plock (int flag)
{
	if (superuser () < 0)
		return (-1);

	if (flag)
		u.u_flags |=  SULOCK;
	else
		u.u_flags &= ~SULOCK;

	return (0);

}	/* end plock */
