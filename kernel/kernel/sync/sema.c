/*
 ****************************************************************
 *								*
 *			sema.c					*
 *								*
 *	Funções de sincronização de recursos múltiplos		*
 *								*
 *	Versão	3.0.0, de 08.10.94				*
 *		4.6.0, de 04.08.04				*
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
 *	Aloca o Recurso						*
 ****************************************************************
 */
void
semalock (SEMA *sema, int pri)
{
	UPROC		*back_up, *up;
	PHASHTB		*hp;

	/*
	 *	Verifica se o recurso está desocupado
	 */
    again:
	SPINLOCK (&sema->e_lock);

	if (--sema->e_count >= 0)
	{
		SPINFREE (&sema->e_lock);

		if (sema->e_flag)
			syncset (E_SEMA, E_GOT, sema);

		return;
	}

	/*
	 *	O recurso está ocupado; Verifica se há sinais para este processo
	 */
	if (pri < PLIMIT && sigpend ())
	{
		sema->e_count++;
		SPINFREE (&sema->e_lock);

#if (0)	/***************************************/
		if (sema->e_flag)
			syncfree ();
#endif	/***************************************/

		jmpdo (u.u_qjmp);
	}

	/*
	 *	O processo irá dormir; Acha a fila deste semaforo
	 */
	hp = PHASH (sema);

	SPINLOCK (&hp->h_lock);

	/*
	 *	Coloca o Processo na fila de espera ordenado pela prioridade
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
	u.u_stype = E_SEMA;
	u.u_sema  = sema;
	u.u_pri   = pri;

	SPINFREE (&hp->h_lock);

	SPINFREE (&sema->e_lock);

	if (sema->e_flag)
		syncset (E_SEMA, E_WAIT, sema);

	dispatcher ();

	if (sema->e_flag)
		syncgot (E_SEMA, sema);

	/*
	 *	O processo é acordado neste ponto
	 */
	if ((u.u_flags & SIGWAKE) == 0)
		return;

	/*
	 *	Estava dormindo com pri < PLIMIT e foi acordado em
	 *	virtude de um sinal. O Recurso não foi obtido!
	 */
	u.u_flags &= ~SIGWAKE;

	if (sigpend ())
	{
		/*
		 *	Veio um sinal SIG_DFL ou (*func) ()
		 */
#if (0)	/***************************************/
		if (sema->e_flag)
			syncfree ();
#endif	/***************************************/

		if (sema->e_flag)
			syncclr (E_SEMA, sema);

		jmpdo (u.u_qjmp);
	}
	else
	{
		/*
		 *	Veio um sinal SIG_IGN
		 */
		goto again;
	}

}	/* end semalock */

/*
 ****************************************************************
 *	Libera um Recurso 					*
 ****************************************************************
 */
void
semafree (SEMA *sema)
{
	UPROC		*up, *back_up;
	PHASHTB		*hp;

	if (sema->e_flag)
		syncclr (E_SEMA, sema);

	/*
	 *	Verifica se tinha alguem esperando por este Semaforo
	 */
	SPINLOCK (&sema->e_lock);

	if (sema->e_count++ >= 0)
		{ SPINFREE (&sema->e_lock); return; }

	/*
	 *	Existe(m) processo(s) esperando por este semáforo
	 */
	hp = PHASH (sema);

	SPINLOCK (&hp->h_lock);

	/*
	 *	Acha o 1. Processo esperando por este processo, e tira-o da fila
	 */
	for (back_up = NOUPROC, up = hp->h_uproc; /* abaixo */; back_up = up, up = up->u_next)
	{
		if (up == NOUPROC)
		{
			printf ("semafree: Recurso não encontrado na fila: %P\n", sema);
			print_calls ();
			SPINFREE (&hp->h_lock);
			SPINFREE (&sema->e_lock);
			return;
		}

		if (up->u_sema == sema)
			break;
	}

	/*
	 *	Achou um Processo
	 */
	if (back_up == NOUPROC)
		hp->h_uproc	= up->u_next;
	else
		back_up->u_next	= up->u_next;

	up->u_stype = E_NULL;
	up->u_sema  = NOVOID;

	/*
	 *	Atualiza os tempos de proc. paralelo se eram semáforos de usuário
	 */
	if ((USEMAG *)sema >= scb.y_usemag && (USEMAG *)sema < scb.y_endusemag)
	{
		if (up->u_partime < u.u_partime)
			up->u_partime = u.u_partime;
	}

	/*
	 *	Insere o processo na Fila
	 */
	insert_proc_in_corerdylist (up);

	SPINFREE (&hp->h_lock);

	SPINFREE (&sema->e_lock);

}	/* end semafree */

/*
 ****************************************************************
 *	Testa um Recurso Lento					*
 ****************************************************************
 */
int
sematest (SEMA *sema)
{
	/*
	 *	Verifica se ainda tem um recurso desocupado
	 */
	SPINLOCK (&sema->e_lock);

	if (--sema->e_count >= 0)
	{
		SPINFREE (&sema->e_lock);

		if (sema->e_flag)
			syncset (E_SEMA, E_GOT, sema);

		return (0);
	}

	/*
	 *	Não há mais recurso livre
	 */
	sema->e_count++;

	SPINFREE (&sema->e_lock);

	return (-1);

}	/* end sematest */
