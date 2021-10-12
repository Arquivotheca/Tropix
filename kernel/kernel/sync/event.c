/*
 ****************************************************************
 *								*
 *			event.c					*
 *								*
 *	Rotinas de Sincronização de Eventos			*
 *								*
 *	Versão	3.0.0, de 05.10.94				*
 *		3.0.0, de 14.07.95				*
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

#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Aloca o recurso						*
 ****************************************************************
 */
void
eventwait (EVENT *sema, int pri)
{
	UPROC		*back_up, *up;
	PHASHTB		*hp;

	/*
	 *	Verifica se o evento já ocorreu
	 */
    again:
	if (*sema & 0x80)
		return;

	/*
	 *	O Evento ainda não ocorreu. Verifica se há sinais pendentes
	 */
	if (pri < PLIMIT && sigpend ())
		jmpdo (u.u_qjmp);

	/*
	 *	O processo irá dormir; acha a Fila deste Semáforo
	 */
	hp = PHASH (sema); SPINLOCK (&hp->h_lock);

	/*
	 *	Verifica se por acaso neste interim o evento ocorreu
	 */
	if (*sema & 0x80)
		{ SPINFREE (&hp->h_lock); return; }

	/*
	 *	No mínimo um aguardando o Evento ocorrer
	 */
	(*sema) = 0x01;
#if (0)	/*******************************************************/
	/*
	 *	Mais um aguardando o Evento ocorrer
	 */
	(*sema)++;
#endif	/*******************************************************/

	/*
	 *	Coloca o processo na fila de espera ordenado pela prioridade
	 *	com os processos de tempo real no início da fila
	 */
	for (back_up = NOUPROC, up = hp->h_uproc; up != NOUPROC; back_up = up, up = up->u_next)
	{
		if   (up->u_rtpri == u.u_rtpri)
		{
			if (up->u_pri < pri)
				break;
		}
		elif (up->u_rtpri < u.u_rtpri)
		{
			break;
		}
	}

	if (back_up == NOUPROC)
		hp->h_uproc	= u.u_myself;
	else
		back_up->u_next	= u.u_myself;

	u.u_next = up;

	/*
	 *	Atualiza a entrada da tabela de processos
	 */
	u.u_state = SCORESLP;
	u.u_stype = E_EVENT;
	u.u_sema  = sema;
	u.u_pri   = pri;

	SPINFREE (&hp->h_lock);

	syncset (E_EVENT, E_WAIT, sema);

	dispatcher ();

	syncclr (E_EVENT, sema);

	/*
	 *	O processo é acordado neste ponto
	 */
	if ((u.u_flags & SIGWAKE) == 0)
		return;

	/*
	 *	Estava dormindo com pri < PLIMIT e foi acordado
	 *	em virtude de um sinal. O Recurso não foi obtido!
	 */
	u.u_flags &= ~SIGWAKE;

	if (sigpend ())
		jmpdo (u.u_qjmp);  /* Veio um sinal SIG_DFL ou (*func) () */
	else
		goto again; 	   /* Veio um sinal SIG_IGN */

}	/* end eventwait */

/*
 ****************************************************************
 *	Libera um Recurso 					*
 ****************************************************************
 */
void
eventdone (EVENT *sema)
{
	UPROC		*up, *back_up, *next_up;
	PHASHTB		*hp;

	/*
	 *	Verifica se o evento já tinha ocorrido
	 */
	if (*sema & 0x80)
		return;

	/*
	 *	Verifica se tinha alguem esperando por este evento.
	 *	É necessario dar o spinlock na fila, para a proteção de *sema
	 */
	hp = PHASH (sema);

	SPINLOCK (&hp->h_lock);

	if (*sema == 0)
		{ *sema = 0x80; SPINFREE (&hp->h_lock); return; }

	*sema = 0x80;

	/*
	 *	Acorda todos os processos esperando pelo evento
	 */
	for (back_up = NOUPROC, up = hp->h_uproc; up != NOUPROC; up = next_up)
	{
		/* Confere o semáforo */

		next_up = up->u_next;

		if (up->u_sema != sema)
			{ back_up = up; continue; }

		/* Achou um processo */

		if (back_up == NOUPROC)
			hp->h_uproc	= next_up;
		else
			back_up->u_next	= next_up;

		up->u_stype = E_NULL;
		up->u_sema  = NOVOID;

		/* Atualiza os tempos de proc. paralelo se eram eventos de usuário */

		if ((UEVENTG *)sema >= scb.y_ueventg && (UEVENTG *)sema < scb.y_endueventg)
		{
			if (up->u_partime < u.u_partime)
				up->u_partime = u.u_partime;
		}

		/* Insere o processo na Fila */

		insert_proc_in_corerdylist (up);

	}	/* end for (varrendo a fila) */
 
	SPINFREE (&hp->h_lock);

}	/* end eventdone */

/*
 ****************************************************************
 *	Testa se o Evento Ocorreu				*
 ****************************************************************
 */
int
eventtest (EVENT *event)
{
	if (*event & 0x80)
		return (0);
	else
		return (-1);

}	/* end eventtest */

/*
 ****************************************************************
 *	Espera pelo eventno-ésimo evento do tio sema		*
 ****************************************************************
 */
void
eventcountwait (int eventno, long *countaddr, EVENT *sema, int pri)
{
	int		dif;
	UPROC		*back_up, *up;
	PHASHTB		*hp;

	/*
	 *	Verifica se o evento já ocorreu
	 */
    again:
	if (*sema & 0x80)
		return;

	/*
	 *	O Evento ainda não ocorreu. Verifica se há sinais pendentes
	 */
	if (pri < PLIMIT && sigpend ())
		jmpdo (u.u_qjmp);

	/*
	 *	O processo irá dormir; acha a Fila deste Semáforo
	 */
	hp = PHASH (sema); SPINLOCK (&hp->h_lock);

	/*
	 *	Verifica se por acaso neste interim o evento ocorreu
	 */
	if (*sema & 0x80)
		{ SPINFREE (&hp->h_lock); return; }

	/*
	 *	Verifica se por acaso neste interim o evento de número eventno já ocorreu
	 */
	if ((dif = eventno - *countaddr) <= 0)
		{ SPINFREE (&hp->h_lock); return; }

	/*
	 *	No mínimo um aguardando o Evento ocorrer
	 */
	(*sema) = 0x01;
#if (0)	/*******************************************************/
	/*
	 *	Mais um aguardando o Evento ocorrer
	 */
	(*sema)++;
#endif	/*******************************************************/

	/*
	 *	Coloca o processo na fila de espera ordenado pela prioridade
	 *	com os processos de tempo real no início da fila
	 */
	for (back_up = NOUPROC, up = hp->h_uproc; up != NOUPROC; back_up = up, up = up->u_next)
	{
		if   (up->u_rtpri == u.u_rtpri)
		{
			if (up->u_pri < pri)
				break;
		}
		elif (up->u_rtpri < u.u_rtpri)
		{
			break;
		}
	}

	if (back_up == NOUPROC)
		hp->h_uproc	= u.u_myself;
	else
		back_up->u_next	= u.u_myself;

	u.u_next = up;

	/*
	 *	Atualiza a entrada da tabela de processos
	 */
	u.u_state = SCORESLP;
	u.u_stype = E_EVENT;
	u.u_sema  = sema;
	u.u_pri   = pri;

	SPINFREE (&hp->h_lock);

	syncset (E_EVENT, E_WAIT, sema);

	dispatcher ();

	syncclr (E_EVENT, sema);

	/*
	 *	O processo é acordado neste ponto
	 */
	if ((u.u_flags & SIGWAKE) == 0)
		return;

	/*
	 *	Estava dormindo com pri < PLIMIT e foi acordado
	 *	em virtude de um sinal. O Recurso não foi obtido!
	 */
	u.u_flags &= ~SIGWAKE;

	if (sigpend ())
		jmpdo (u.u_qjmp);  /* Veio um sinal SIG_DFL ou (*func) () */
	else
		goto again; 	   /* Veio um sinal SIG_IGN */

}	/* end eventcountwait */
