/*
 ****************************************************************
 *								*
 *			signal.c				*
 *								*
 *	Processa os sinais					*
 *								*
 *	Versão	3.0.0, de 08.10.94				*
 *		4.6.0, de 02.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/frame.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *   Envia um Sinal para Todos os Processos do Grupo de TTY	*
 ****************************************************************
 */
void
sigtgrp (int tgrp, int signo)
{
	const UPROCV	*uvp;
	UPROC		*up;

	/*
	 *	Pequeno teste
	 */
	if (tgrp == 0)
		return;

	/*
	 *	Percorre a lista
	 */
	for (uvp = scb.y_uproc; uvp <= scb.y_lastuproc; uvp++)
	{
		if ((up = uvp->u_uproc) == NOUPROC)
			continue;

		if (up->u_tgrp != tgrp)
			continue;

		if (up->u_state != SNULL && up->u_state != SZOMBIE)
			sigproc (up, signo);
	}

}	/* end sigtgrp */

/*
 ****************************************************************
 *	Envia um Sinal para um Processo				*
 ****************************************************************
 */
void
sigproc (UPROC *up, int signo)
{
	PHASHTB		*hp;

#ifdef	MSG
	if (CSWT (15))
	{
		printf ("%g: u pid = %d, p pid = %d ", u.u_pid, up->u_pid);
		printf ("u func = %P, signo = %d\n", u.u_signal[signo], signo);
	}
#endif	MSG

	if (u.u_euid != 0 && u.u_euid != up->u_euid && u.u_ruid != up->u_ruid)
	{
		/*
		 *	O sinal "SIGCHLD" é uma exceção,
		 *	pois foi o pai quem o invocou!
		 */
		if (signo != SIGCHLD)
			{ u.u_error = EPERM; return; }
	} 

	if (signo == 0)
		return;

	if ((unsigned)signo > NSIG)
		{ printf ("%g: Sinal inválido: %d\n", signo); return; }

	if (up->u_state == SNULL || up->u_state == SZOMBIE)
		{  printf ("%g: Processo %d NULL ou ZOMBIE\n", up->u_pid); return; }

	up->u_sig |= 1 << signo;

	/*
	 *	Analisa as Prioridades do Processo
	 */
	if   (up->u_pri < PUSER)
		up->u_pri = PUSER;
	elif (up->u_pri >= PLIMIT && signo == SIGKILL && u.u_euid == 0)
		up->u_pri = PUSER;

	/*
	 *	Dá o Lock na file HASH (Se esta existe)
	 */
	hp = PHASH (up->u_sema);

	SPINLOCK (&hp->h_lock);

	if (up->u_sema == (void *)NULL)
	{
		SPINFREE (&hp->h_lock);
	}
	else
	{
		/*
		 *	Verifica se tem de acordar o processo violentamente (sem
		 *	o recurso esperado). O estado é SCORESLP ou SSWAPSLP
		 */
		if (up->u_pri < PLIMIT)
			syncwakeup (up, hp);
		else
			SPINFREE (&hp->h_lock);
	}

}	/* end sigproc */

/*
 ****************************************************************
 *	Verifica se há um sinal para este processo 		*
 ****************************************************************
 */
int
sigpend (void)
{
	UPROC		*up;
	int		n;

	up = u.u_myself;

	/*
	 *	Verifica se tem um sinal
	 */
	while (up->u_sig)
	{
		n = signoget (up);

		if (u.u_signal[n] != SIG_IGN || up->u_flags & STRACE)
			return (n);

		/*
		 *	O sinal deve ser ignorado
		 */
		up->u_sig &= ~(1 << n);
	}

	/*
	 *	Não há sinal
	 */
	return (0);

}	/* end sigpend */

/*
 ****************************************************************
 *	Executa o sinal						*
 ****************************************************************
 */
void
sigexec (void *fa, void *pc)
{
	void		(*func) (int, ...);
	int		signo;

	/*
	 *	Verifica se está sendo feito um TRACE
	 */
#if (0)	/*************************************/
	if (u.u_flags & STRACE)			/* PROVISÓRIO */
		stop ();
#endif	/*************************************/

	/*
	 *	Se não há sinal, ou é SIG_IGN, retorna
	 */
	if ((signo = sigpend ()) == 0)
		return;

#ifdef	MSG
	if (CSWT (15))
		printf ("%g: func = %P, signo = %d\n", u.u_signal[signo], signo);
#endif	MSG

	/*
	 *	Apaga o sinal
	 */
	u.u_sig &= ~(1 << signo);

	/*
	 *	Analisa o tipo do Sinal
	 */
	if ((func = u.u_signal[signo]) != SIG_DFL)
	{
		/*
		 *	O Usuário interceptou este sinal
		 */
		u.u_error = NOERR;

		if (signo != SIGILL && signo != SIGTRAP)
			u.u_signal[signo] = SIG_DFL;

		siguser (func, signo, fa, pc);

		return;
	}

	/*
	 *	A função é SIG_DFL
	 */
	switch (signo)
	{
	    case SIGQUIT:
	    case SIGILL:
	    case SIGTRAP:
	    case SIGIOT:
	    case SIGEMT:
	    case SIGFPE:
	    case SIGBUS:
	    case SIGSEGV:
	    case SIGSYS:
	    case SIGADR:
	    case SIGDVZ:
	    case SIGCHK:
	    case SIGTRPV:
	    case SIGVIOL:
	    case SIGABRT:
		if (coredump () == 0)
			signo += 0x80;
		break;

	    case SIGWINCH:	/* Normalmente, deve ser ignorado */
		return;
	}

	kexit (signo, fa, pc);

}	/* end sigexec */

/*
 ****************************************************************
 *	Acha o número do sinal					*
 ****************************************************************
 */
int
signoget (UPROC *up)
{
	long		n;
	int		i;

	n = up->u_sig >> 1;

	/*
	 *	Procura o bit ligado mais à direita
	 */
	for (i = 1; i <= NSIG; i++)
	{
		if (n & 1)
			return (i);

		n >>= 1;
	}

	return (0);

}	/* end signoget */

/*
 ****************************************************************
 *   Envia o Sinal para o Usuário, simulando uma Interrupção	*
 ****************************************************************
 */
void
siguser (void (*func) (int, ...), int signo, void *fa,  void *pc) 
{
	long		*newsp;
	FRAME		*fp;
	long		**usp;
	void		**upc;
	int		usr;

#ifdef	MSG
	if (CSWT (15))
		printf ("%g: func = %P, signo = %d, fa = %P, sigludium = %P\n", func, signo, fa, u.u_sigludium);
#endif	MSG

	/*
	 *	Obtém o endereço do USP e PC do usuário no "frame",
	 *	além do valor do SR do usuário
	 */
	switch ((fp = u.u_frame)->type)
	{
	    case 0:		/* Exceção */
		usp = (long **)&fp->e.ef_usp;
		upc = (void **)&fp->e.ef_pc;
		usr = fp->e.ef_sr;
		break;

	    case 1:		/* Interrupção */
		usp = (long **)&fp->i.if_usp;
		upc = (void **)&fp->i.if_pc;
		usr = fp->i.if_sr;
		break;

	    case 2:		/* Chamada ao sistema */
		usp = (long **)&fp->s.sf_usp;
		upc = (void **)&fp->s.sf_pc;
		usr = get_sr ();
		break;

	    default:
		printf ("%g: Tipo inválido da exceção da PILHA: %d\n", fp->type);
		return;
	}

	/*
	 *	Aumenta a pilha do Usuário
	 */
	load_cr3 ();

	newsp = *usp;

	if (setstacksz (newsp - 30 /* longos */) < 0)
	{
		/*
		 *	Situação embaraçosa, provávelmente
		 *	vai dar um BUS ERROR de Usuário
		 */
		printf ("%g: Não foi possivel aumentar a pilha\n");
	}

	/*
	 *	Prepara a pilha do Usuário
	 */
	*--newsp = (long)*upc;
	*--newsp = usr;
	*--newsp = (long)func;
	*--newsp = (long)fa;
	*--newsp = (long)pc;
	*--newsp = signo;

	*usp = newsp;
	*upc = (void *)u.u_sigludium;

}	/* end siguser */
