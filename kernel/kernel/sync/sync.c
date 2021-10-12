/*
 ****************************************************************
 *								*
 *			sync.c					*
 *								*
 *	Diversas funções de controle de sincronismo		*
 *								*
 *	Versão	3.0.0, de 08.10.94				*
 *		3.0.0, de 08.10.94				*
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
 *	Coloca uma entrada no histórico de sincronismo		*
 ****************************************************************
 */
void
syncset (int tipo, int wait, void *sema)
{
	SYNCH		*sh;

#ifdef	MSG
	if (CSWT (1))
		printf ("%g: tipo = %s, sema = %P\n", syncnmget (tipo), sema);
#endif	MSG

	for (sh = &u.u_synch[0]; sh < &u.u_synch[NHIST]; sh++)
	{
		if (sh->e_type != E_NULL)
			continue;

		sh->e_type  = tipo;
		sh->e_state = wait;
		sh->e_sema  = sema;

		if (++u.u_syncn > u.u_syncmax)
			u.u_syncmax++;

		return;
	}

	printf ("%g: Tabela Cheia\n");

}	/* end syncset */

/*
 ****************************************************************
 *	Obteve um Recurso esperado				*
 ****************************************************************
 */
void
syncgot (int tipo, void *sema)
{
	SYNCH		*sh;

#ifdef	MSG
	if (CSWT (1))
		printf ("%g: tipo = %s, sema = %P\n", syncnmget (tipo), sema);
#endif	MSG

	if (tipo == E_EVENT)
		{ printf ("%g: chamado com evento\n"); print_calls (); return; }

	for (sh = &u.u_synch[0]; sh < &u.u_synch[NHIST]; sh++)
	{
		if (sh->e_type == E_NULL)
			continue;

		if (sh->e_sema != sema)
			continue;

		if (sh->e_type != tipo)
			{ printf ("%g: Tipo inválido\n"); print_calls (); }

		sh->e_state = E_GOT;
		return;
	}

	printf ("%g: Semáforo nao encontrado: %P\n", sema);
	print_calls ();

}	/* end syncgot */

/*
 ****************************************************************
 *	Retira uma entrada no histórico de sincronismo		*
 ****************************************************************
 */
void
syncclr (int tipo, void *sema)
{
	SYNCH		*sh;

#ifdef	MSG
	if (CSWT (1))
		printf ("%g: tipo = %s, sema = %P\n", syncnmget (tipo), sema);
#endif	MSG

	for (sh = &u.u_synch[0]; sh < &u.u_synch[NHIST]; sh++)
	{
		if (sh->e_sema != sema)
			continue;

		if (sh->e_type != tipo)
			printf ("%g: Tipo inválido\n");

		memclr (sh, sizeof (SYNCH) );
		u.u_syncn--;
		return;
	}

	printf ("%g: Semáforo não encontrado: %P, %s\n", sema, syncnmget (tipo));
	print_calls ();

}	/* end syncclr */

/*
 ****************************************************************
 *	Verifica se o histórico está vazio			*
 ****************************************************************
 */
int
synccheck (void)
{
	SYNCH		*sh;

#ifdef	MSG
	if (CSWT (9))
		return (0);
#endif	MSG

	if (u.u_syncn == 0)
		return (0);

	for (sh = &u.u_synch[0]; sh < &u.u_synch[NHIST]; sh++)
	{
		if (sh->e_type != E_NULL)
		{
			printf ("%g: Semáforo Residual: %s, %P\n", syncnmget (sh->e_type), sh->e_sema);
			print_calls ();
		}
	}

	return (-1);

}	/* end synccheck */

/*
 ****************************************************************
 *	Libera todos os recursos obtidos do histórico		*
 ****************************************************************
 */
void
syncfree (void)
{
	SYNCH		*sh;

	for (sh = &u.u_synch[NHIST-1]; sh >= &u.u_synch[0]; sh--)
	{
		switch (sh->e_type)
		{
		    case E_NULL:
			continue;

		    case E_SLEEP:
			SLEEPFREE ((LOCK *)sh->e_sema);
			break;

		    case E_SEMA:
			SEMAFREE ((SEMA *)sh->e_sema);
			break;

		    case E_EVENT:
			printf ("%g: Liberando EVENTO?\n");
			print_calls ();
			break;

		    default:
			printf ("%g: Tipo do semáforo inválido\n");
			print_calls ();
		}
	}

}	/* end syncfree */

/*
 ****************************************************************
 *	Libera o último recurso obtido				*
 ****************************************************************
 */
void
syncundo (UPROC *up)
{
	void		*sema;

	sema = up->u_sema;

	/*
	 *	A Fila HASH do Processo vem e volta LOCKED!
	 */
	switch (up->u_stype)
	{
	    case E_SLEEP:
		(*(char *)sema)--;
		break;

	    case E_SEMA:
		((SEMA *)sema)->e_count++;
		break;

	    case E_EVENT:
		(*(char *)sema)--;
		break;

	    default:
		printf ("%g: Semáforo inválido\n");
		return;

	}	/* end switch */

	up->u_sema  = NOVOID;
	up->u_stype = E_NULL;

}	/* end syncundo */

/*
 ****************************************************************
 *	Acorda o processo sem receber o recurso			*
 ****************************************************************
 */
void
syncwakeup (UPROC *wake_up, PHASHTB *h)
{
	UPROC		*back_up, *up;

	/*
	 *	Recebe a fila HASH LOCKED e devolve FREE
	 */
	for (back_up = NOUPROC, up = h->h_uproc; /* abaixo */; back_up = up, up = up->u_next)
	{
		if   (up == NOUPROC)
		{
			printf ("%g: Processo não encontrado na fila HASH\n");
			break;
		}
		elif (up == wake_up)
		{
			if (back_up == NOUPROC)
				h->h_uproc	= up->u_next;
			else
				back_up->u_next	= up->u_next;

			break;
		}
	}

	/*
	 *	Encontrou e retirou o processo da fila
	 */
	syncundo (wake_up);
	
	SPINFREE (&h->h_lock);

	wake_up->u_flags |= SIGWAKE;

	if   (wake_up->u_state == SCORESLP)
	{
		insert_proc_in_corerdylist (wake_up);
	}
#if (0)	/*******************************************************/
	elif (wake_up->u_state == SSWAPSLP)
	{
		wake_up->u_state = SSWAPRDY; insswaprdyl (wake_up);
	}
#endif	/*******************************************************/
	else
	{
		printf
		(	"%g: O Proc. %d tem estado inválido: %s (%d)\n",
			wake_up->u_pid, proc_state_nm_edit (wake_up->u_state), wake_up->u_state
		);
	}

}	/* end syncwakeup */

/*
 ****************************************************************
 *	Obtem o nome do tipo do semáforo			*
 ****************************************************************
 */
char *
syncnmget (int tipo)
{
	switch (tipo)
	{
	    case E_SLEEP:
		return ("SLEEP");

	    case E_SEMA:
		return ("SEMA ");

	    case E_EVENT:
		return ("EVENT");
	}

	return ("???? ");

}	/* end syncnmget */
