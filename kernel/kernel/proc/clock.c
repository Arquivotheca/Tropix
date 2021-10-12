/*
 ****************************************************************
 *								*
 *			clock.c					*
 *								*
 *	Atendimento à interrupção de relógio			*
 *								*
 *	Versão	3.0.0, de 07.10.94				*
 *		4.6.0, de 24.07.04				*
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

#include "../h/seg.h"
#include "../h/frame.h"
#include "../h/cpu.h"
#include "../h/timeout.h"
#include "../h/mon.h"
#include "../h/a.out.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variveis  e Definições Globais 				*
 ****************************************************************
 */
entry int		hz;			/* Contador de interrupções do relógio */

entry time_t		time;			/* Tempo em segundos desde o BIG BANG */
entry time_t		uptime;			/* Tempo em segundos desde o BOOT */

entry EVENT		every_second_event;	/* Evento de segundo em segundo */

entry int		tslice[NCPU];		/* Contador de TSLICEs para as CPUs */

entry MON		mon;			/* A estrutura de monitoração */

entry int		ahc_busy,		/* Para verificar o uso */
			usb_busy;

extern int		sd_busy,		/* Para verificar o uso */
			ata_busy,
			ed_busy,
			ppp_in_busy,
			ppp_out_busy;

/*
 ******	Variáveis do timeout ************************************
 */
#define	TIMEOUT_DEBUG				/* Depuração */

entry ulong		abs_ticks;		/* "ticks" absolutos do relógio */

entry ulong 		timeout_abs_ticks = 1; /* Próximo "tick" absoluto a processar */

#define TIMEOUT_HASH_MASK	(TIMEOUT_HASH_SZ - 1)

entry TIMEOUT_HASH	timeout_hash[TIMEOUT_HASH_SZ];	/* Para o "hash" */

entry LOCK		timeout_free_lock;	/* Semáforo da tabela */

entry TIMEOUT		*timeout_free_list;	/* Para a lista livre */

entry int		timeout_busy_count;	/* Contador em uso */

entry int		timeout_global_count;	/* Contador geral */

entry int		timeout_overflow_count;	/* Se um tick não foi suficiente */

/*
 ******	Definições externas *************************************
 */
extern char		stopaddr;		/* Endereço do STOP em idle */

/*
 ******	Protótipos de funções ***********************************
 */
TIMEOUT			*timeout_get_free_entry (void);
void			timeout_put_free_entry (TIMEOUT *);

/*
 ****************************************************************
 *	Interrução do relógio 					*
 ****************************************************************
 */
void
clock_int (struct intr_frame frame)
{
	UPROC		*up;
	int		cpu, curpri, maxpri;
	TIMEOUT_HASH	*hp;
	TIMEOUT		*ap, *tp;
	void		(*func) (int);
	int		arg;
	int		n;
	static char	timeout_in_operation;

	cpu = CPUID;

	/*
	 *	Atualiza tempos do processo, uso da CPU, e uso do SASI
	 */
	if (SEL_PL (frame.if_cs) == 0)	/* KERNEL */
	{
		u.u_stime++;

		if (frame.if_pc == &stopaddr)
			mon.y_stop[cpu]++;
	}
	else	/* USER */
	{
		u.u_utime++;
	}

	u.u_seqtime++; 	u.u_partime++;

	mon.y_ticks[cpu]++;

	if (cpu == 0)
	{
		if (sd_busy || ahc_busy || ata_busy)
			mon.y_sasi++;

		if (usb_busy)
			mon.y_usb++;

		if (ed_busy)
			mon.y_ed++;

		if (ppp_in_busy)
			mon.y_ppp_in++;

		if (ppp_out_busy)
			mon.y_ppp_out++;
	}

	/*
	 *	Processamento do Time-slice
	 */
	if (++tslice[cpu] >= scb.y_tslice)
	{
		tslice[cpu] -= scb.y_tslice;

		/*
		 *	Diminui a prioridade do processo corrente
		 *	se não é processo de tempo real
		 */
		curpri = u.u_pri;

		if (u.u_pid >= scb.y_initpid && (u.u_flags & RTPROC) == 0)
		{
			maxpri = PUSER + scb.y_nfactor * u.u_nice;

			if (curpri > maxpri)
				curpri = maxpri;

			if ((curpri -= scb.y_decpri) < 0)
				curpri = 0;

			u.u_pri = curpri;
		}

		/*
		 *	Se for a CPU 0, aumenta a prioridade dos
		 *	Processos CORERDY que não são de tempo real
		 */
		if (cpu == 0)
		{
			SPINLOCK (&corerdylist_lock);

			for (up = corerdylist; up != NOUPROC; up = up->u_next)
			{
				if ((up->u_flags & RTPROC) == 0)
					up->u_pri += scb.y_incpri;
#define	TST
#ifdef	TST
				if (up->u_state != SCORERDY)
				{
					printf
					(	"clock: Proc. (%d, %s) "
						"não SCORERDY\n",
						up->u_pid, proc_state_nm_edit (up->u_state)
					);
				}

#if (0)	/*******************************************************/
				if (up != uproc_0 && up->u_pid == 0)
				{
					printf
					(	"clock: Proc. (%d, %s) "
						"pid NULO\n",
						up - scb.y_proc,
						proc_state_nm_edit (up->u_state)
					);
				}
#endif	/*******************************************************/
#endif	TST
			}

			if ((up = corerdylist) != NOUPROC)
				bestpri = up->u_pri;
			else
				bestpri = 0;

			SPINFREE (&corerdylist_lock);
		}

	}	/* end time slice */

	/*
	 *	Processamento de Segundo em Segundo
	 */
	if (cpu == 0 && ++hz >= scb.y_hz)
	{
		hz -= scb.y_hz;

		time++; uptime++;

		EVENTDONE  (&every_second_event);
		EVENTCLEAR (&every_second_event);
	}

#if (0)	/*******************************************************/
	/*
	 *	Guarda o valor do PIT neste ponto da interrupção
	 */
	write_port (0, 0x43);
	PIT_val_proc = read_port (0x40) + (read_port (0x40) << 8);
#endif	/*******************************************************/

	/*
	 *	Somente a CPU 0 trata do TIMEOUT
	 */
	if (cpu != 0)
		return;

	abs_ticks++;		/* "ticks" absolutos do relógio */

	/*
	 * 	Se interrompeu uma região de PL > 0,
	 *	simplesmente adia o tratamento de "timeout"
	 */
	if (frame.if_cpl > 0)
		return;

	if (timeout_in_operation)
	{
		timeout_overflow_count++;	/* Um tick não foi suficiente */
		return;
	}

	timeout_in_operation = 1;

	spl0 ();	/* As funções de "timeout" são chamadas com nível 0 */

	/*
	 *	Repare a aritmética circular
	 */
	while ((n = abs_ticks - timeout_abs_ticks) >= 0)
	{
		/*
		 *	Procura na fila
		 */
		hp = &timeout_hash[timeout_abs_ticks & TIMEOUT_HASH_MASK];

	    next:
		SPINLOCK (&hp->o_lock);

#ifdef	TIMEOUT_DEBUG
		n = 0;
#endif	TIMEOUT_DEBUG

		for (ap = NOTIMEOUT, tp = hp->o_first; tp != NOTIMEOUT; ap = tp, tp = tp->o_next)
		{
#ifdef	TIMEOUT_DEBUG
			n++;
#endif	TIMEOUT_DEBUG
			if (tp->o_time == timeout_abs_ticks)
				break;
		}

#ifdef	TIMEOUT_DEBUG
		if (hp->o_count < n)
			hp->o_count = n;
#endif	TIMEOUT_DEBUG

		if (tp == NOTIMEOUT)
			{ SPINFREE (&hp->o_lock); timeout_abs_ticks++; continue; }

		/*
		 *	Retira da fila
		 */
		func = tp->o_func;
		arg  = tp->o_arg;

		tp->o_func = NULL;

		if (ap == NOTIMEOUT)
			hp->o_first = tp->o_next;
		else
			ap->o_next  = tp->o_next;

		SPINFREE (&hp->o_lock);

		timeout_put_free_entry (tp);

		/*
		 *	Chama a função
		 */
#ifdef	MSG
		if (CSWT (17))
		{
			const SYM	*sp;

			printf ("TOUT chamando: ");

			sp = getsyment ((unsigned long)func);

			if (sp != NOSYM)
				printf ("%s, ", sp->s_name);
			else
				printf ("%P, ", func);

			printf ("busy = %d; ", timeout_busy_count);
		}
#endif	MSG
		(*func) (arg);

		goto next;

	}	/* end while */

	timeout_in_operation = 0;

}	/* end clock_int */

/*
 ****************************************************************
 *	Prepara um "timeout"					*
 ****************************************************************
 */
TIMEOUT *
toutset (int ticks, void (*func) (), int arg)
{
	TIMEOUT		*tp;
	TIMEOUT_HASH	*hp;

	/*
	 *	Obtém uma entrada
	 */
	if ((tp = timeout_get_free_entry ()) == NOTIMEOUT)
		panic ("timeout: Não há mais entradas disponíveis");

	if (ticks <= 0)
		ticks = 1;

	ticks += abs_ticks;	/* Transforma em absoluto */

	/*
	 *	Insere no início da lista
	 */
	hp = &timeout_hash[ticks & TIMEOUT_HASH_MASK];

	SPINLOCK (&hp->o_lock);

	tp->o_time = ticks;
	tp->o_func = func;
	tp->o_arg  = arg;
   /***	tp->o_next = ...; /* abaixo ***/

   	tp->o_next = hp->o_first; hp->o_first = tp;

	timeout_global_count++;	/* Contador global */

	SPINFREE (&hp->o_lock);

#ifdef	MSG
	if (CSWT (17))
	{
		printf ("%g: ticks = %d (%d s), ", ticks, ticks / scb.y_hz);
		printf ("abs_ticks = %d, timeout_abs_ticks = %d\n", abs_ticks, timeout_abs_ticks);
	}
#endif	MSG

	/*
	 *	Retorna o ponteiro para a entrada,
	 *	que servirá para uma eventual remoção
	 */
	return (tp);

}	/* end toutset */

/*
 ****************************************************************
 *	Desfaz um "timeout"					*
 ****************************************************************
 */
void
toutreset (TIMEOUT *reset_tp, void (*func) (), int arg)
{
	TIMEOUT		*tp;
	TIMEOUT_HASH	*hp;
	char		found = 0;

	/*
	 *	Acha o elemento na fila e retira-o
	 */
	if (reset_tp == NOTIMEOUT)
		return;

	hp = &timeout_hash[reset_tp->o_time & TIMEOUT_HASH_MASK];

	SPINLOCK (&hp->o_lock);

	if (reset_tp->o_func != func || reset_tp->o_arg != arg)
		{ SPINFREE (&hp->o_lock); return; }

	if ((tp = hp->o_first) == reset_tp)
	{
		hp->o_first = reset_tp->o_next; found++;
	}
	else for (/* acima */; tp != NOTIMEOUT; tp = tp->o_next)
	{
		if (tp->o_next == reset_tp)
			{ tp->o_next = reset_tp->o_next; found++; break; }
	}

	reset_tp->o_func = NULL;

	SPINFREE (&hp->o_lock);

#ifdef	MSG
	if (CSWT (17))
		printf ("%g: ticks = %d (%d s)\n", reset_tp->o_time, reset_tp->o_time / hz);
#endif	MSG

	/*
	 *	Devolve a entrada para a lista livre
	 */
	if (found)
		timeout_put_free_entry (reset_tp);

}	/* end toutreset */

/*
 ****************************************************************
 *	Obtém uma estrutura TIMEOUT				*
 ****************************************************************
 */
TIMEOUT *
timeout_get_free_entry (void)
{
	TIMEOUT		*tp;

	SPINLOCK (&timeout_free_lock);

	if ((tp = timeout_free_list) != NOTIMEOUT)
	{
		timeout_free_list = tp->o_next;
		timeout_busy_count++;
	}

	SPINFREE (&timeout_free_lock);

	return (tp);

}	/* end timeout_get_free_entry */

/*
 ****************************************************************
 *	Libera uma estrutura TIMEOUT				*
 ****************************************************************
 */
void
timeout_put_free_entry (TIMEOUT *tp)
{
	SPINLOCK (&timeout_free_lock);

	tp->o_next = timeout_free_list;
	timeout_free_list = tp;
	timeout_busy_count--;

	SPINFREE (&timeout_free_lock);

}	/* end timeout_put_free_entry */

/*
 ****************************************************************
 *	Inicializa a lista de LOCKs				*
 ****************************************************************
 */
void
timeout_init_free_list (void)
{
	TIMEOUT		*tp, *np;

	np = NOTIMEOUT;

   /***	SPINLOCK (&timeout_free_lock); ***/

	for (tp = scb.y_endtimeout - 1; tp >= scb.y_timeout; tp--)
		{ tp->o_next = np; np = tp; }

	timeout_free_list = np;

   /***	SPINFREE (&timeout_free_lock); ***/

}	/* end timeout_put_free_entry */
