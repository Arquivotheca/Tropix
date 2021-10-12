/*
 ****************************************************************
 *								*
 *			lockf.c					*
 *								*
 *	Algoritmos dos LOCKs de regiões de arquivos		*
 *								*
 *	Versão	3.0.0, de 24.11.94				*
 *		3.0.0, de 24.11.94				*
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

#include "../h/inode.h"
#include "../h/lockf.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	PLOCKF	PPIPE

/*
 ****************************************************************
 *	Variáveis locais					*
 ****************************************************************
 */
entry LOCKF	*lffreelist;	/* Lista livre */
entry LOCK	lffreelock;	/* Semáforo da lista livre */

/*
 ****************************************************************
 *	Testa se pode acessar uma região			*
 ****************************************************************
 */
int
lfaccess (INODE *ip, off_t begin_off, off_t end_off, int flag)
{
	LOCKF		*lp;

	/*
	 *	Todos as funções deste arquivo
	 *	recebem e devolvem o INODE locked 
	 *
	 *	"flag" indica qual o procedimento a tomar
	 *	em caso da área estar trancada:
	 *	   'W' => dorme aguardando liberação
	 *	   'T' => retorna imediatamente indicando erro
	 */
    again:
	for (lp = ip->i_lockf; lp != NOLOCKF; lp = lp->k_next)
	{
		if (lp->k_uproc == u.u_myself)
			continue;

		if (end_off > lp->k_begin && begin_off < lp->k_end)
			goto found;
	}

	return (0);

	/*
	 *	Foi encontrada uma região de litígio
	 */
    found:
	switch (flag)
	{
	    default:
		printf ("%g: Indicador errado\n");
	    case 'T':
		u.u_error = EAGAIN;
		return (-1);

	    case 'W':
		break;

	}	/* end switch */

	/*
	 *	A região está trancada, e devemos esperar
	 */
	if (lfdeadlock (lp) < 0)
		return (-1);

	SLEEPFREE (&ip->i_lock);

	EVENTWAIT (&lp->k_free, PLOCKF);

	SLEEPLOCK (&ip->i_lock, PINODE);

	goto again;

}	/* end lfaccess */

/*
 ****************************************************************
 *	Cria um LOCK						*
 ****************************************************************
 */
int
lflock (INODE *ip, off_t begin_off, off_t end_off, int flag)
{
	LOCKF		*lp, *ap, *np;
	LOCKF		*gp;

	/*
	 *	Certifica-se de que outro processo já não possui a região
	 */
	if (lfaccess (ip, begin_off, end_off, flag) < 0)
		return (-1);

	/*
	 *	Obtém um bloco para a lista (neste momento é mais seguro)
	 */
	if ((gp = lfget ()) == NOLOCKF)
		{ u.u_error = EDEADLK; return (-1); }

	/*
	 *	Faz o merge com outras áreas possuídas pelo processo 
	 */
	for
	(	ap = NOLOCKF, lp = ip->i_lockf;
		lp != NOLOCKF;
		ap = lp, lp = np
	)
	{
		np = lp->k_next;

		if (end_off < lp->k_begin || begin_off > lp->k_end)
			continue;

		if (lp->k_uproc != u.u_myself)
		{
			if (end_off != lp->k_begin && begin_off != lp->k_end)
				printf ("%g: Confusão com LOCKs alheios\n");

			continue;
		}

		/*
		 *	Calcula a união
		 */
		if (begin_off > lp->k_begin)
			begin_off = lp->k_begin;

		if (end_off   < lp->k_end)
			end_off   = lp->k_end;

		/*
		 *	Libera este LOCK; processos esperando
		 *	por ele analisarão a lista novamente
		 */
		EVENTDONE (&lp->k_free);

		lfput (lp);

		if (ap == NOLOCKF)
			ip->i_lockf = np;
		else
			ap->k_next  = np;

		lp = ap;

	}	/* end for */

	/*
	 *	Cria o novo bloco da lista
	 */
	lp = gp;

	EVENTCLEAR (&lp->k_free);
	lp->k_uproc = u.u_myself;
	lp->k_begin = begin_off;
	lp->k_end   = end_off;

	lp->k_next  = ip->i_lockf;
	ip->i_lockf = lp;

	return (0);

}	/* end lflock */

/*
 ****************************************************************
 *	Libera um LOCK						*
 ****************************************************************
 */
int
lffree (INODE *ip, off_t begin_off, off_t end_off)
{
	LOCKF		*lp, *ap, *np;
	LOCKF		*gp;
	off_t		k_end;

	/*
	 *	Obtém um bloco para a lista já prevendo um possível buraco
	 *	(neste momento é mais seguro)
	 */
	if ((gp = lfget ()) == NOLOCKF)
		{ u.u_error = EDEADLK; return (-1); }

	/*
	 *	Faz o merge com outras áreas possuídas pelo processo 
	 */
	for
	(	ap = NOLOCKF, lp = ip->i_lockf;
		lp != NOLOCKF;
		ap = lp, lp = np
	)
	{
		np = lp->k_next;

		if (lp->k_uproc != u.u_myself)
			continue;

		if (end_off <= lp->k_begin || begin_off >= lp->k_end)
			continue;

		/*
		 *	Libera este LOCK; processos esperando
		 *	por ele analisarão a lista novamente
		 */
		EVENTDONE (&lp->k_free);

		/*
		 *	Analisa o que resta da região
		 */
		if (begin_off <= lp->k_begin)
		{
			if (end_off >= lp->k_end)
			{
				/*
				 *	Nada resta
				 */
				lfput (lp);

				if (ap == NOLOCKF)
					ip->i_lockf = np;
				else
					ap->k_next  = np;

				lp = ap;
			}
			else
			{
				/*
				 *	Resta um pedaço em cima
				 */
				lp->k_begin = end_off;
			}
		}
		else
		{
			k_end = lp->k_end;
			lp->k_end = begin_off;

			/*
			 *	Verifica se restou um pedaço em cima
			 */
			if (end_off < k_end)
			{
				/*
				 *	A área será transformada em duas
				 */
				if (gp == NOLOCKF)
					panic ("lffree: 2 transformações?");

				EVENTCLEAR (&gp->k_free);
				gp->k_uproc  = u.u_myself;
				gp->k_begin = end_off;
				gp->k_end   = k_end;

				gp->k_next  = np;
				lp->k_next  = gp;

				ap = lp;
				lp = gp;

				gp = NOLOCKF;

			}	/* end if transformação de região? */ 
				
		}	/* end if interceptou em baixo? */  

	}	/* end for */

	/*
	 *	Se não precisou do novo bloco, devolve
	 */
	if (gp != NOLOCKF)
		lfput (gp);

	return (0);

}	/* end lffree */

/*
 ****************************************************************
 *	Verifica se há o perigo de DEADLOCK			*
 ****************************************************************
 */
int
lfdeadlock (LOCKF *lp)
{
	void		*sema;

	/*
	 *	Se o processo dono da região não está dormindo, não há perigo
	 */
    again:
	sema = lp->k_uproc->u_sema;

	if (sema == (void *)NULL)
		return (0);

	/*
	 *	O processo está dormindo; tenta identificar o semáforo
	 *	Se estiver dormindo por outras causas não há perigo.
	 */
	if ((int)sema < (int)scb.y_lockf || (int)sema >= (int)scb.y_endlockf)
		return (0);

	/*
	 *	Está dormindo em virtude de uma outra região; tenta achar o dono
	 *	dela; repare que o semáforo é o primeiro membro da entrada
	 */
	lp = (LOCKF *)sema;

	if (((int)lp - (int)scb.y_lockf) % sizeof (LOCKF))
		{ printf ("%g: Endereço de semáforo inválido\n"); goto bad; }

	/*
	 *	Verifica o dono da região; se for este processo é DEADLOCK
	 */
	if (lp->k_uproc != u.u_myself)
		goto again;

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	u.u_error = EDEADLK;
	return (-1);

}	/* end lfdeadlock */

/*
 ****************************************************************
 *	Libera todos os LOCKs possuídos por este processo	*
 ****************************************************************
 */
int
lfclosefree (INODE *ip)
{
	LOCKF		*ap, *lp, *np;

	/*
	 *	Percorre a lista, liberando todas as regiões deste processo
	 */
	for
	(	ap = NOLOCKF, lp = ip->i_lockf;
		lp != NOLOCKF;
		ap = lp, lp = np
	)
	{
		np = lp->k_next;

		if (lp->k_uproc != u.u_myself)
			continue;
		
		/*
		 *	Encontrou uma região deste processo
		 */
		EVENTDONE (&lp->k_free);

		lfput (lp);

		if (ap == NOLOCKF)
			ip->i_lockf = np;
		else
			ap->k_next  = np;

		lp = ap;

	}	/* end for */

	return (0);

}	/* end lfclosefree */

/*
 ****************************************************************
 *	Obtém uma estrutura LOCKF				*
 ****************************************************************
 */
LOCKF *
lfget (void)
{
	LOCKF		*lp;

	SPINLOCK (&lffreelock);

	if ((lp = lffreelist) != NOLOCKF)
		lffreelist = lp->k_next;

	SPINFREE (&lffreelock);

	return (lp);

}	/* end lfget */

/*
 ****************************************************************
 *	Libera uma estrutura LOCKF				*
 ****************************************************************
 */
void
lfput (LOCKF *lp)
{
	SPINLOCK (&lffreelock);

	lp->k_next = lffreelist;
	lffreelist = lp;

	SPINFREE (&lffreelock);

}	/* end lfput */

/*
 ****************************************************************
 *	Inicializa a lista de LOCKs				*
 ****************************************************************
 */
void
lfinit (void)
{
	LOCKF		*lp, *np;

	np = NOLOCKF;

	for (lp = scb.y_endlockf - 1; lp >= scb.y_lockf; lp--)
		{ lp->k_next = np; np = lp; }

	lffreelist = np;

}	/* end lfinit */
