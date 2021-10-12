/*
 ****************************************************************
 *								*
 *			nproc.c					*
 *								*
 *	Criação de novos processos				*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.6.0, de 03.08.04				*
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
#include "../h/map.h"
#include "../h/mmu.h"
#include "../h/uarg.h"
#include "../h/default.h"
#include "../h/inode.h"
#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Criação de um novo Processo				*
 ****************************************************************
 */
long
newproc (int flag /* FORK ou THREAD */)
{
	INODE		*ip;
	KFILE		*fp;
	UPROCV		*uvp;
	UPROC		*chiup;
	const REGIONL	*rlp, *end_rlp, *end_par_rlp, *par_rlp;
	REGIONL		*chi_rlp;
	REGIONG		*rgp;
	int		i;
	pg_t		chi_uproc, chi_mmu_dir;
	int		dupflag;
	static long	pid_source;

	/*
	 *	Retorna para o FILHO:
	 *	   = 0
	 *
	 *	Retorna para o PAI:
	 *	   > 0 => pid do filho
	 *	   < 0 => Não conseguiu criar novo processo
	 */
	if (flag != FORK && flag != THREAD)
		panic ("newproc: Indicador %d inválido\n", flag);

	/*
	 *	Obtém a UPROC para o FILHO e copia o conteúdo do PAI
	 */
	if ((chi_uproc = malloce (M_CORE, USIZE)) == NOPG)
		goto free_nothing;

	chiup = (UPROC *)(PGTOBY (chi_uproc));

	memmove (chiup, &u, UPROCSZ);

	/*
	 *	Obtém uma entrada livre da tabela UPROC
	 */
	SPINLOCK (&uprocfreelist_lock);

	uvp = uprocfreelist;

	if (uvp == NOUPROCV || uvp->u_next == NOUPROCV && u.u_euid != 0)
		{ SPINFREE (&uprocfreelist_lock); u.u_error = EAGAIN; goto free_uproc; }

	uprocfreelist = uvp->u_next;

	if (uvp > scb.y_lastuproc)
		scb.y_lastuproc = uvp;

	uvp->u_uproc = chiup; chiup->u_pid = ++pid_source;

	SPINFREE (&uprocfreelist_lock);

	/*
	 *	Prepara a UPROC do FILHO (só altera os campos diferentes do PAI)
	 */
	chiup->u_state		= SNULL;
	chiup->u_sig		= 0;
   /***	chiup->u_mmu_dir	= NOMMU;	***/
	chiup->u_pri		= PUSER + scb.y_nfactor * u.u_nice;

	chiup->u_flags		= 0;
   /***	chiup->u_mask		= u.u_mask;	***/

	chiup->u_lock		= 0;
	chiup->u_childlock	= 0;
	chiup->u_deadchild	= 0;

	chiup->u_time		= 0;
   /***	chiup->u_nice		= u.u_nice;	***/
	chiup->u_trace		= 0;

   /***	chiup->u_euid		= u.u_euid;	***/
   /***	chiup->u_ruid		= u.u_ruid;	***/

   /***	chiup->u_pid		= ++pid_source;	***/
   /***	chiup->u_pgrp		= u.u_pgrp;	***/
   /***	chiup->u_tgrp		= u.u_tgrp;	***/

	chiup->u_uprocv		= uvp;
	chiup->u_myself		= chiup;
	chiup->u_parent		= u.u_myself;
	chiup->u_child		= NOUPROC;
	chiup->u_sibling	= NOUPROC;

	chiup->u_sema		= 0;
	chiup->u_stype		= 0;
	chiup->u_alarm		= 0;
   /***	chiup->u_inode		= u.u_inode;	***/

	chiup->u_syncn		= 0;
	chiup->u_syncmax	= 0;
   /***	chiup->u_rtpri		= u.u_rtpri;	***/

	chiup->u_attention_event = 0;
	chiup->u_attention_index = 0;

	chiup->u_grave_index	= 0;
	chiup->u_grave_attention_set = 0;

	memclr (chiup->u_synch, sizeof (chiup->u_synch));

	/* Apaga regiões do TEXT, DATA, STACK e SHLIB */

	memclr (&chiup->u_text, (3 + 2 * NSHLIB) * sizeof (REGIONL));

	/*
	 *	Aqui será iniciado o FILHO através do ctxtld do dispatcher.
	 *	Durante o armazenamento do contexto, a preempção não é permitida
	 */
	preemption_flag[CPUID] = 0;

	if (ctxtsto (chiup->u_ctxt) == 0)
	{
#if (0)	/*******************************************************/
		preemption_flag[CPUID] = 0;
#endif	/*******************************************************/

		/*
		 *	Carrega a MMU para o processo filho
		 */
		mmu_load ();

#if (0)	/*******************************************************/
		/*
		 *	Se não se usa o macete do EM bit do PC,
		 *	aqui é preciso restaurar a FPU
		 */
		fpu_restore (u.u_fpstate);
#endif	/*******************************************************/

		/*
		 *	O pai pode ter usado a FPU no time-slice que
		 *	criou o filho fazendo fpu_lately_used = 1 e
		 *	passando na UPROC para o filho.
		 *	Neste ponto, entretanto, o filho certamente
		 *	ainda não a usou. Portanto, fpu_lately_used = 0.
		 */
		u.u_fpu_lately_used = 0;

		/*
		 *	Aqui o contexto do filho já está inteiramente carregado
		 *	e, portanto, preemption passa a ser permitida novamente
		 */
		if (scb.y_preemption_mask[CPUID])
			preemption_flag[CPUID]++;

#if (0)	/*******************************************************/
		if (u.u_proc->p_pri < bestpri)
			switch_run_process ();
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		if (scb.y_preemption_mask[CPUID])
			preemption_flag[CPUID]++;
#endif	/*******************************************************/

		u.u_cstime	= 0;
		u.u_stime	= 0;
		u.u_cutime	= 0;
		u.u_utime	= 0;
		u.u_start	= time;

		u.u_cseqtime	= 0;
		u.u_seqtime	= 0;
		u.u_cpartime	= 0;

		return (0);
	}

	/*
	 *	Se a FPU foi usada no presente time-slice,
	 *	é preciso salvar o seu estado neste ponto
	 *	para que a UPROC do filho receba o estado corrente
	 */
	if (u.u_fpu_lately_used)
	{
		fpu_save    (chiup->u_fpstate);		/* No PC, o save reseta a FPU */
		fpu_restore (chiup->u_fpstate);		/* daí este restore */
	}

	/*
	 *	Aqui todo contexto do PAI já foi carregado precisando
	 *	apenas copiá-lo para o FILHO.
	 *	Portanto, preemption passa a ser permitida novamente
	 */
	if (scb.y_preemption_mask[CPUID])
		preemption_flag[CPUID]++;

	/*
	 *	Estabelece o diretório de páginas para o FILHO
	 */
	if (flag == THREAD && u.u_pid == 0)
	{
		/* Usa o diretório de páginas do processo 0 */

		chi_mmu_dir		= NOPG;
		chiup->u_mmu_dir	= kernel_page_directory;
		chiup->u_no_user_mmu	= 1;
	}
	else
	{
		if ((chi_mmu_dir = malloce (M_CORE, 1)) == NOPG)
			goto free_uproc;

		chiup->u_mmu_dir = (mmu_t *)(PGTOBY (chi_mmu_dir));

		/*
		 *	Inicializa o diretório de páginas, tendo como base a
		 *	kernel_page_directory (diretório do processo 0).
		 *	Desta forma, as entradas correspondentes
		 *	aos endereços virtuais de usuário já estão zeradas.
		 */
		pgcpy (chi_mmu_dir, BYTOPG (kernel_page_directory), 1);

		chiup->u_no_user_mmu	= 0;
	}

	/*
	 *	Duplica fisicamente a região STACK do PAI
	 */
	if (regiondup (&u.u_stack, &chiup->u_stack, RG_PHYSDUP) < 0)
		goto free_mmu_dir;

	/*
	 *	Duplica fisicamente/logicamente a região DATA do PAI
	 */
	dupflag = (flag == FORK ? RG_PHYSDUP : RG_LOGDUP);

	if (regiondup (&u.u_data, &chiup->u_data, dupflag) < 0)
		goto free_stack;

	/*
	 *	Duplica logicamente a região TEXT do PAI
	 */
	if (regiondup (&u.u_text, &chiup->u_text, RG_LOGDUP) < 0)
		goto free_data;

	/*
	 *	Duplica as bibliotecas compartilhadas
	 */
	par_rlp = &u.u_shlib_text[0]; end_par_rlp = par_rlp + NSHLIB;
	chi_rlp = &chiup->u_shlib_text[0];

	for (/* acima */; par_rlp < end_par_rlp; par_rlp++, chi_rlp++)
		regiondup (par_rlp, chi_rlp, RG_LOGDUP);

	par_rlp = &u.u_shlib_data[0]; end_par_rlp = par_rlp + NSHLIB;
	chi_rlp = &chiup->u_shlib_data[0];

	for (/* acima */; par_rlp < end_par_rlp; par_rlp++, chi_rlp++)
	{
		if (regiondup (par_rlp, chi_rlp, dupflag) < 0)
			goto free_shlib;
	}

	/*
	 *	Duplica o PHYS (Está na UPROC, não deve usar "regiondup")
	 */
	if (u.u_phn)
	{
		end_rlp = &u.u_phys_region[PHYSNO];

		for (rlp = &u.u_phys_region[0]; rlp < end_rlp; rlp++)
		{
			if ((rgp = rlp->rgl_regiong) == NOREGIONG)
				continue;

			SPINLOCK (&rgp->rgg_regionlock);
			rgp->rgg_count++;
			SPINFREE (&rgp->rgg_regionlock);
		}
	}

	/*
	 *	Já conseguiu todos os recursos para o processo filho
	 *	Incrementa o contador dos vários recursos
	 */
	for (i = 0; i < NUFILE; i++)
	{
		if ((fp = u.u_ofile[i]) != NOKFILE)
		{
			SPINLOCK (&fp->f_lock);
			fp->f_count++;
			SPINFREE (&fp->f_lock);

			if (fp->f_union == KF_SHMEM)
			{
				rgp = fp->f_shmem_region.rgl_regiong;

				SPINLOCK (&rgp->rgg_regionlock);
				rgp->rgg_count++;
				SPINFREE (&rgp->rgg_regionlock);
			}
		}
	}

	if ((ip = u.u_inode) != NOINODE)
	{
		SLEEPLOCK (&ip->i_lock, PSCHED);
		ip->i_xcount++;
		SLEEPFREE (&ip->i_lock);
	}

	if ((ip = u.u_cdir) != NOINODE)
	{
		SLEEPLOCK (&ip->i_lock, PSCHED);
		iinc (ip);
		SLEEPFREE (&ip->i_lock);
	}

	if ((ip = u.u_rdir) != NOINODE)
	{
		SLEEPLOCK (&ip->i_lock, PSCHED);
		iinc (ip);
		SLEEPFREE (&ip->i_lock);
	}

	/*
	 *	Insere o FILHO na fila de filhos do PAI
	 */
	SPINLOCK (&u.u_childlock);

	chiup->u_sibling = u.u_child; u.u_child = chiup;

	SPINFREE (&u.u_childlock);

	/*
	 *	Indica o contexto de retorno do FILHO e o insere na fila READY
	 */
	insert_proc_in_corerdylist (chiup);

	return (chiup->u_pid);	/* Retorna a identificação do filho para o PAI */

	/*
	 *	NÃO conseguiu criar o processo
	 *	libera os diversos recursos
	 */
    free_shlib:
	for (chi_rlp--; chi_rlp >= &chiup->u_shlib_data[0]; chi_rlp--)
		regionrelease (chi_rlp);

	chi_rlp = &chiup->u_shlib_text[NSHLIB - 1];

	for (/* acima */; chi_rlp >= &chiup->u_shlib_text[0]; chi_rlp--)
		regionrelease (chi_rlp);

    free_data:
	regionrelease (&chiup->u_data);
    free_stack:
	regionrelease (&chiup->u_stack);
    free_mmu_dir:
	if (chi_mmu_dir != NOPG)
		mrelease (M_CORE, 1, chi_mmu_dir);
    free_uproc:
	mrelease (M_CORE, USIZE, chi_uproc);
    free_nothing:
	if (u.u_error == NOERR)
		u.u_error = ENOMEM;

	return (-1);

}	/* end newproc */

/*
 ****************************************************************
 *  Altera o nome do próprio programa (somente modo supervisor)	*
 ****************************************************************
 */
void
set_pgname (const char *pgnm)
{
	char		*ucp;
	UARG		*up;

	ucp = (char *)PGTOBY (USER_STACK_PGA) - sizeof (UARG);

	up = (UARG *)ucp;

	ucp -= strlen (pgnm) + 1;

	up->g_null = NULL;
	up->g_argc = 1;
	up->g_argp = ucp;
	up->g_envc = 0;

	strcpy (ucp, pgnm);

}	/* end set_pgname */

/*
 ****************************************************************
 *	Obtém o nome do estado do processo			*
 ****************************************************************
 */
const char	* const proc_state_nm_table[] =
{
	"NULL   ", 	"SWAPRDY",	"CORERDY", 	"SWAPSLP",
	"CORESLP", 	"RUN    ", 	"ZOMBIE "
};

const char *
proc_state_nm_edit (int state)
{
	if ((unsigned)state > SZOMBIE)
		return ("???");
	else
		return (proc_state_nm_table[state]);

}	/* end proc_state_nm_edit */
