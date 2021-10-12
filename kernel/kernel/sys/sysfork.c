/*
 ****************************************************************
 *								*
 *			sysfork.c				*
 *								*
 *	Chamadas ao sistema: criação e término de processos	*
 *								*
 *	Versão	3.0.0, de 13.12.94				*
 *		4.6.0, de 02.08.04				*
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

#include "../h/map.h"
#include "../h/frame.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Chamada ao sistema "exit"				*
 ****************************************************************
 */
void
exit (int retvalue)
{
	kexit ((retvalue & 0xFF) << 8, NOVOID, NOVOID);

}	/* end exit */

/*
 ****************************************************************
 *	Chamada ao sistema "wait"				*
 ****************************************************************
 */
long
wait (void)
{
	UPROCV		*uvp;
	UPROC		*back_up, *child_up;
	long		pid;
	EVENT		*deadchild_event_addr;

	/*
	 *	Verifica se tem um filho ZOMBIE ou STOP
	 */
    again:
	SPINLOCK (&u.u_childlock);

	back_up = NOUPROC; child_up = u.u_child;

	for (/* acima */; child_up != NOUPROC; back_up = child_up, child_up = child_up->u_sibling)
	{
		if (child_up->u_state == SZOMBIE)
			goto found;

		if (child_up->u_state != SCORESLP && child_up->u_state != SSWAPSLP)
			continue;

		if (child_up->u_sema != &child_up->u_trace)
			continue;

		if (child_up->u_flags & SWAITED)
			continue;

		child_up->u_flags |= SWAITED;
		u.u_frame->s.sf_r1 = (signoget (child_up) << 8) | 0x7F;
		SPINFREE (&u.u_childlock);
		return (child_up->u_pid);
	}

	SPINFREE (&u.u_childlock);

	/*
	 *	Não achou nenhum filho na fila de Terminados.
	 *	Verifica se o Processo tem algum filho
	 */
	if (u.u_child == NOUPROC)
		{ u.u_error = ECHILD; return (-1); }

	/*
	 *	Tem filho(s), mas nenhum ZOMBIE nem STOP
	 */
	deadchild_event_addr = &u.u_myself->u_deadchild;

	EVENTWAIT  (deadchild_event_addr, PWAIT);
	EVENTCLEAR (deadchild_event_addr);

	goto again;

	/*
	 *	Achou um filho terminado; tira-o da fila do pai
	 */
    found:
	if (back_up == NOUPROC)
		u.u_child	   = child_up->u_sibling;
	else
		back_up->u_sibling = child_up->u_sibling;

	SPINFREE (&u.u_childlock);

	/*
	 *	Coleta as informações do Filho.
	 */
	pid = child_up->u_pid; uvp = child_up->u_uprocv;

	u.u_frame->s.sf_r1	 = child_up->u_rvalue;

	u.u_cutime		+= child_up->u_utime;
	u.u_cstime		+= child_up->u_stime;

	u.u_cseqtime		+= child_up->u_seqtime;

	if (u.u_cpartime < child_up->u_partime)
		u.u_cpartime	 = child_up->u_partime;

	u.u_frame->s.sf_r3	 = (int)child_up->u_fa;
	u.u_frame->s.sf_r4	 = (int)child_up->u_pc;

	/*
	 *	Libera a memória restante
	 */
	mrelease (M_CORE, 1, BYTOPG (child_up->u_mmu_dir));

	mrelease (M_CORE, USIZE, BYTOPG (child_up));

	/*
	 *	Devolve a Entrada à Lista Livre
	 */
	SPINLOCK (&uprocfreelist_lock);

	uvp->u_uproc = NOUPROC; uvp->u_next = uprocfreelist; uprocfreelist = uvp;

	while (scb.y_lastuproc->u_uproc == NOUPROC)
		scb.y_lastuproc--;

	SPINFREE (&uprocfreelist_lock);

	return (pid);

}	/* end wait */

/*
 ****************************************************************
 *	Chamada ao sistema "fork"				*
 ****************************************************************
 */
long
fork (void)
{
	return (newproc (FORK));

}	/* end fork */

/*
 ****************************************************************
 *	Chamada ao sistema "thread"				*
 ****************************************************************
 */
long
thread (void)
{
	return (newproc (THREAD));

}	/* end thread */
