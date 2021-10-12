/*
 ****************************************************************
 *								*
 *			sleep.c					*
 *								*
 *	Funções de sincronização de recursos lentos		*
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
sleeplock (LOCK *sema, int pri)
{
	UPROC		*back_up, *up;
	PHASHTB		*hp;

	/*
	 *	Verifica se o recurso está desocupado
	 */
    again:
	if (TAS (sema) == 0)
		{ syncset (E_SLEEP, E_GOT, sema); return; }

	/*
	 *	O recurso está ocupado; Verifica se há sinais para este processo
	 */
	if (pri < PLIMIT && sigpend ())
	{
#if (0)	/***************************************/
		syncfree ();
#endif	/***************************************/
		jmpdo (u.u_qjmp);
	}

	/*
	 *	O processo irá dormir; acha a fila deste semáforo
	 */
	hp = PHASH (sema);

	SPINLOCK (&hp->h_lock);

	/*
	 *	Verifica se por acaso neste interim
	 *	uma outra CPU liberou o recurso
	 */
	if (TAS (sema) == 0)
	{
		SPINFREE (&hp->h_lock);
		syncset (E_SLEEP, E_GOT, sema);
		return;
	}

	/*
	 *	Mais um aguardando o recurso ser liberado
	 */
	sema[0]++;

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
	u.u_stype = E_SLEEP;
	u.u_sema  = sema;
	u.u_pri   = pri;

	SPINFREE (&hp->h_lock);

	syncset (E_SLEEP, E_WAIT, sema);

	dispatcher ();

	syncgot (E_SLEEP, sema);

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
		syncfree ();
#endif	/***************************************/
		syncclr (E_SLEEP, sema);
		jmpdo (u.u_qjmp);
	}
	else
	{
		/*
		 *	Veio um sinal SIG_IGN
		 */
		goto again;
	}

}	/* end sleeplock */

/*
 ****************************************************************
 *	Libera um Recurso 					*
 ****************************************************************
 */
void
sleepfree (LOCK *sema)
{
	UPROC		*up, *back_up;
	PHASHTB		*hp;

	syncclr (E_SLEEP, sema);

	/*
	 *	Verifica se o recurso já estava desocupado
	 */
	if (sema[0] == 0)
		{ printf ("%g: Liberando recurso desocupado: %P\n", sema); print_calls (); return; }

	/*
	 *	Verifica se tinha alguem esperando por este semáforo.
	 *	É necessario dar o spinlock na fila, para a proteção de "sema[0]"
	 */
	hp = PHASH (sema);

	SPINLOCK (&hp->h_lock);

	if (sema[0] == 0x80)
		{ sema[0] = 0; SPINFREE (&hp->h_lock); return; }

	/*
	 *	Existe(m) processo(s) esperando por este recurso
	 */
	sema[0]--;

	/*
	 *	Procura o 1. Processo esperando por este processo, e tira-o da fila
	 */
	for (back_up = NOUPROC, up = hp->h_uproc; /* abaixo */; back_up = up, up = up->u_next)
	{
		if (up == NOUPROC)
		{
			printf ("%g: Recurso não encontrado na fila: %P, valor = %2X\n", sema, sema[0]);
			print_calls ();

			SPINFREE (&hp->h_lock);
			return;
		}

		if (up->u_sema == sema)
			break;
	}

	if (back_up == NOUPROC)
		hp->h_uproc	= up->u_next;
	else
		back_up->u_next	= up->u_next;

	/*
	 *	Altera o estado do Processo e insere-o na Fila
	 */
	up->u_stype = E_NULL;
	up->u_sema  = NOVOID;

	insert_proc_in_corerdylist (up);

	SPINFREE (&hp->h_lock);

}	/* end sleepfree */

/*
 ****************************************************************
 *	Testa um recurso lento					*
 ****************************************************************
 */
int
sleeptest (LOCK *sema)
{
	if (TAS (sema) == 0)
		{ syncset (E_SLEEP, E_GOT, sema); return (0); }

	return (-1);

}	/* end sleeptest */
