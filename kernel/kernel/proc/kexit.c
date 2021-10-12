/*
 ****************************************************************
 *								*
 *			kexit.c					*
 *								*
 *	Término de processos					*
 *								*
 *	Versão	3.0.0, de 03.12.94				*
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

#include "../h/map.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Termina um processo					*
 ****************************************************************
 */
void
kexit (int retvalue, void *fa, void *pc)
{
	int		i, dead_child_cnt = 0;
	UPROC		*up, *first_init_child, *back_up;
	UPROC		*init_up, *stopchild = NOUPROC;
	INODE		*ip;
	KFILE		*fp;
	REGIONL		*rlp;
	const REGIONL	*end_rlp;

	/*
	 *	Isto NÃO deve acontecer ...
	 */
	if (u.u_pid == scb.y_initpid)
		panic ("kexit: INIT deu Exit, fa = %P, pc = %P", fa, pc);

	/*
	 *	Ignora todos os Sinais e 
	 *	Fecha os Arquivos ainda Abertos
	 */
	u.u_flags &= ~(SULOCK|STRACE); u.u_alarm = 0;

	for (i = 0; i <= NSIG; i++)
		u.u_signal[i] = SIG_IGN;

	for (i = 0; i < NUFILE; i++)
		{ fp = u.u_ofile[i]; u.u_ofile[i] = NOKFILE; fclose (fp); }

	/*
	 *	Libera o diretório corrente e o "root" corrente
	 */
	if (u.u_cdir != NOINODE)
		{ SLEEPLOCK (&u.u_cdir->i_lock, PINODE); iput (u.u_cdir); }

	if (u.u_rdir != NOINODE)
		{ SLEEPLOCK (&u.u_rdir->i_lock, PINODE); iput (u.u_rdir); }

	/*
	 *	Desaloca os PHYS
	 */
	if (u.u_phn) for (rlp = &u.u_phys_region[0], end_rlp = rlp + PHYSNO; rlp < end_rlp; rlp++)
		regionrelease (rlp);

   /***	u.u_phn = 0; ***/

	/*
	 *	Libera as regiões das bibliotecas compartilhadas
	 */
	for (rlp = u.u_shlib_text, end_rlp = rlp + NSHLIB; rlp < end_rlp; rlp++)
		regionrelease (rlp);

	for (rlp = u.u_shlib_data, end_rlp = rlp + NSHLIB; rlp < end_rlp; rlp++)
		regionrelease (rlp);

	/*
	 *	Libera o TEXT e verifica o sincronismo
	 */
	if ((ip = u.u_inode) != NOINODE)
	{
		SLEEPLOCK (&ip->i_lock, PINODE);

		if (xrelease (ip) > 0)
			iput (ip);
		else
			SLEEPFREE (&ip->i_lock);

		u.u_inode = NOINODE;
	}

	/*
	 *	Libera todas as regiões do processo
	 */
	regionrelease (&u.u_text);
	regionrelease (&u.u_data);
	regionrelease (&u.u_stack);

	/*
	 *	Confere o sincronismo
	 */
	if (synccheck () < 0)
		syncfree ();

	/*
	 *	Entra no estado ZOMBIE
	 */
	u.u_state = SZOMBIE;

	u.u_rvalue  = retvalue;
	u.u_utime  += u.u_cutime;
	u.u_stime  += u.u_cstime;
	u.u_fa      = fa;
	u.u_pc      = pc;

	/*
	 *	Soma os tempos do processamento sequencial dos filhos e
	 *	transfere a maior estimativa de processamento paralelo
	 */
	u.u_seqtime += u.u_cseqtime;

	if (u.u_partime < u.u_cpartime)
		u.u_partime = u.u_cpartime;

	/*
	 *	Libera as áreas do diretório de PGs e da UPROC
	 */
	if (u.u_xargs)
		{ mrelease (M_CORE, BYUPPG (scb.y_ncexec), u.u_xargs); u.u_xargs = 0; }

	/*
	 *	Coloca os Filhos deste Processo como Filhos do INIT.
	 *	(Estamos supondo que INIT já tenha filhos)
	 */
	init_up = scb.y_uproc[scb.y_initpid].u_uproc;

	if ((up = u.u_child) != NOUPROC)
	{
		SPINLOCK (&u.u_childlock);

		SPINLOCK (&init_up->u_childlock);

		first_init_child = init_up->u_child; init_up->u_child = up;

		for (back_up = u.u_myself; up != NOUPROC; back_up = up, up = up->u_sibling)
		{
			up->u_parent = init_up;

			switch (up->u_state)
			{
			    case SZOMBIE:
				dead_child_cnt++;
				break;

			    case SCORESLP:
			    case SSWAPSLP:
				if (up->u_sema == &up->u_trace)
					stopchild = up;
				break;

			    default:
				break;
			}
		}

		back_up->u_sibling = first_init_child;

		SPINFREE (&init_up->u_childlock);

		SPINFREE (&u.u_childlock);

	}	/* end if (processo tem filhos) */

	/*
	 *	Notifica o(s) pai(s)
	 */
	up = u.u_parent;

	EVENTDONE (&up->u_deadchild);

	if (dead_child_cnt > 0 && up != init_up)
		EVENTDONE (&init_up->u_deadchild);

	if (stopchild != NOUPROC)
		EVENTDONE (&stopchild->u_trace);

	/*
	 *	Verifica se deve enviar o sinal "SIGCHLD"	
	 */
	if (u.u_flags & SIGEXIT && up != init_up)
		sigproc (up, SIGCHLD);

	/*
	 *	Verifica se tem ATTENTION do GRAVE
	 */
	if (up->u_grave_attention_set && up->u_attention_index < 0)
	{
		up->u_grave_attention_set = 0;
		up->u_attention_index = up->u_grave_index; 
		EVENTDONE (&up->u_attention_event);
	}

	dispatcher ();

	/* Não retorna */

}	/* end kexit */
