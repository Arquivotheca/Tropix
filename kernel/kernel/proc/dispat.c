/*
 ****************************************************************
 *								*
 *			dispat.c				*
 *								*
 *	Escalador de processos					*
 *								*
 *	Versão	3.0.0, de 08.10.94				*
 *		4.6.0, de 27.09.04				*
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

#include "../h/cpu.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis e Areas Globais				*
 ****************************************************************
 */
entry UPROC	*corerdylist;	/* Lista de Proc. Prontos na Memoria */
entry LOCK	corerdylist_lock;

entry short	bestpri;	/* Prioridade da cabeca da corerdylist */

/*
 ******	Protótipos de funções ***********************************
 */
UPROC		*proc_ready_search (void);

/*
 ****************************************************************
 *	Escalador de Processos					*
 ****************************************************************
 */
void
dispatcher (void)
{
	UPROC		*next_up;

	/*
	 *	Um dispatcher não pode chamar a si proprio
	 */
	if (u.u_pid > 0 && u.u_pid <= scb.y_ncpu)
		panic ("%g: Dispatcher: Tentada uma Chamada Recursiva");

	/*
	 *	Aqui começa a efetiva troca de contexto.
	 *	Preemption não é permitida deste ponto em diante
	 *	até que se efetive no sistema a completa troca do contexto 
 	 *	do processo corrente para o contexto do novo processo
	 */
	preemption_flag[CPUID] = 0;

	if (ctxtsto (u.u_ctxt) == 0)
	{
#if (0)	/*******************************************************/
		preemption_flag[CPUID] = 0;
#endif	/*******************************************************/

		/*
		 *	Se não se usa o macete do EM bit do PC,
		 *	neste ponto é preciso restaurar a FPU
		 */
#if (0)	/*******************************************************/
		fpu_restore (u.u_fpstate);
#endif	/*******************************************************/

		/*
		 *	Aqui o contexto do sistema foi inteiramente trocado
		 *	do contexto do processo anterior para o 
		 *	contexto do, agora, processo corrente e,
		 *	portanto, preemption volta a ser permitida
		 */
		if (scb.y_preemption_mask[CPUID])
			preemption_flag[CPUID]++;

#if (0)	/*******************************************************/
		/*
		 *	Ainda assim, verifica se um outro processo acordou
		 *	nesse meio tempo e tem prioridade maior que o corrente
		 */
		if (u.u_proc->u_pri < bestpri)
			switch_run_process ();
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		if (scb.y_preemption_mask[CPUID])
			preemption_flag[CPUID]++;
#endif	/*******************************************************/

#ifdef	MSG
#if (0)	/*******************************************************/
		if (CSWT (26) && u.u_ctxt_sw_type == 0)
		{
			printf ("#");
			printf ("<%c %s>", '#' + u.u_ctxt_sw_type, u.u_pgname);
		}
#endif	/*******************************************************/
#endif	MSG
		return;
	}

	/*
	 *	Se a FPU foi usada no presente time-slice,
	 *	neste ponto é preciso salvar o seu estado
	 */
	if (u.u_fpu_lately_used)
	{
		fpu_save (u.u_fpstate);
		u.u_fpu_lately_used = 0;
		fpu_set_embit ();
	}

	/*
	 *	Verifica se já há um processo pronto para rodar
	 */
	if ((next_up = proc_ready_search ()) != NOUPROC)
	{
		next_up->u_state = SRUN; u.u_pri = next_up->u_pri;
	}
	else	/* NÃO há; invoca o "dispatcher" desta CPU */
	{
		next_up = scb.y_uproc[CPUID + 1].u_uproc;
	}

	u.u_cpu = CPUID;

	ctxtld (next_up);

}	/* end dispatcher */

/*
 ******	Segunda parte do código do escalador ********************
 */
void
procswitch (void)
{
	UPROC		*next_up;

#ifdef	MSG
#if (0)	/*******************************************************/
	if (CSWT (26) && u.u_ctxt_sw_type == 0)
		printf ("#");
	if (CSWT (26))
		printf ("<%c %s>", '#' + u.u_ctxt_sw_type, u.u_pgname);
#endif	/*******************************************************/
#endif	MSG

	/*
	 *	Espera um processo pronto para rodar
	 */
	while ((next_up = proc_ready_search ()) == NOUPROC)
		idle ();

	/*
	 *	Transfere o Controle para o novo Processo
	 */
	next_up->u_state = SRUN;

	u.u_pri = next_up->u_pri;
	u.u_cpu = CPUID;

	ctxtld (next_up);

}	/* end  procswitch */

/*
 ****************************************************************
 *	Procura um processo excutável				*
 ****************************************************************
 */
UPROC *
proc_ready_search (void)
{
	UPROC		*back_up, *next_up;
	int		cpu, cpubit;

	/*
	 *	Procura um processo executável
	 */
	cpu = CPUID; cpubit = 1 << cpu;

	SPINLOCK (&corerdylist_lock);

	for
	(	back_up = NOUPROC, next_up = corerdylist;
		/* abaixo */;
		back_up = next_up, next_up = next_up->u_next
	)
	{
		if (next_up == NOUPROC)
			{ SPINFREE (&corerdylist_lock); return (NOUPROC); }

		if (cpubit & next_up->u_mask)
			break;
	}

	/*
	 *	Achou um Processo para Executar; Tira-o da fila e atualiza "bestpri"
	 */
	if (back_up == NOUPROC)
		corerdylist	= next_up->u_next;
	else
		back_up->u_next	= next_up->u_next;

	if ((back_up = corerdylist) == NOUPROC)
		bestpri = 0;
	else
		bestpri = back_up->u_pri;

	SPINFREE (&corerdylist_lock);

	/*
	 *	Certifica-se de que o processo está READY
	 */
	if ((next_up->u_state != SCORERDY))
		panic  ("%g: Ia executar o Proc. %d não READY", next_up->u_pid);

	return (next_up);

}	/* end proc_ready_search */

/*
 ****************************************************************
 *	Troca de Processo: SRUN -> SCORERDY			*
 ****************************************************************
 */
void
switch_run_process (void)
{
	insert_proc_in_corerdylist (u.u_myself); dispatcher ();

}	/* end switch_run_process */

/*
 ****************************************************************
 *	Insere um processo na "corerdylist"			*
 ****************************************************************
 */
void
insert_proc_in_corerdylist (UPROC *ready_up)
{
	UPROC		*up, *back_up;
	int		pri;

	/*
	 *	Processos de Tempo Real recuperam a sua prioridade original
	 */
	if (ready_up->u_flags & RTPROC)
		ready_up->u_pri = pri = ready_up->u_rtpri;
	else
		pri = ready_up->u_pri;

	/*
	 *	Troca de estado e insere na lista
	 */
	SPINLOCK (&corerdylist_lock);

	ready_up->u_state = SCORERDY;

	for (back_up = NOUPROC, up = corerdylist; up != NOUPROC; back_up = up, up = up->u_next)
	{
		if (up->u_pri < pri)
			break;
	}

	if (back_up == NOUPROC)
		corerdylist	= ready_up;
	else
		back_up->u_next	= ready_up;

	ready_up->u_next = up;

	/*
	 *	Há pelo menos um processo na lista
	 */
	bestpri = corerdylist->u_pri;
	
	SPINFREE (&corerdylist_lock);

}	/* end insert_proc_in_corerdylist */

/*
 ****************************************************************
 *	Remove um Processo da Corerdylist			*
 ****************************************************************
 */
void
remove_proc_from_corerdylist (UPROC *rm_up)
{
	UPROC		*up, *back_up;

	/*
	 *	Repare que o "dispatcher" tambem remove processos da "corerdylist" (da cabeça)
	 */
	SPINLOCK (&corerdylist_lock);

	for (back_up = NOUPROC, up = corerdylist; /* abaixo */; back_up = up, up = up->u_next)
	{
		if (up == NOUPROC)
		{
			SPINFREE (&corerdylist_lock);
			printf ("%g: O Processo %d NÃO está na lista READY\n", rm_up->u_pid);
			return;
		}

		if (up == rm_up)
			break;
	}

	if (back_up == NOUPROC)
		corerdylist	= rm_up->u_next;
	else
		back_up->u_next	= rm_up->u_next;

	/*
	 *	Atualiza a prioridade
	 */
	if ((up = corerdylist) == NOUPROC)
		bestpri = 0;
	else
		bestpri = up->u_pri;

	SPINFREE (&corerdylist_lock);

}	/* end remove_proc_from_corerdylist */
