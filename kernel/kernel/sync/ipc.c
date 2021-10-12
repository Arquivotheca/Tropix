/*
 ****************************************************************
 *								*
 *			ipc.c					*
 *								*
 *	Funções para Comunicação entre Processos,		*
 *	eventos, semáforos e memória compartilhada		*
 *								*
 *	Versão	1.0.0, de 19.10.90				*
 *		3.1.6, de 13.01.99				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/map.h"
#include "../h/mmu.h"
#include "../h/inode.h"
#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/ipc.h"
#include "../h/default.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry LOCK	ueventllock;		/* Lista local de eventos */
entry LOCK	ueventglock;		/* Lista global de eventos */
entry LOCK	usemallock;		/* Lista local de semáforos */
entry LOCK	usemaglock;		/* Lista global de semáforos */
entry UEVENTG	*ueventgfreelist;	/* Lista livre de UEVENTG */
entry USEMAG	*usemagfreelist;	/* Lista livre de UEVENTG */

/*
 ****************************************************************
 *	Aloca um conjunto de eventos associados a um file	*
 ****************************************************************
 */
int
ueventalloc (KFILE *fp, int nueventl)
{
	int		i = 0, j = 0;
	UEVENTL		*elp;

	if (fp->f_union != KF_NULL)
		{ u.u_error = EBADF; return (-1); }

	if ((u.u_ueventno + nueventl) > MAXNUEVENT)
		{ u.u_error = ENOSPC; return (-1); }

	SPINLOCK (&ueventllock);

	for (elp = scb.y_ueventl; elp < scb.y_endueventl; elp++)
	{
		if (elp->uel_uevent == NOUEVENTG)
			j++;
		else
			j = 0;

		i++; 

		/*
		 *	Marca com -1 as entradas alocadas
		 */
		if (j >= nueventl)
		{
			for (j = nueventl; j > 0; j--)
				(elp--)->uel_uevent = (UEVENTG *)-1;

			fp->f_union = KF_EVENT;

			fp->f_ueventlno = nueventl;
			fp->f_ueventloff = i - nueventl;

			SPINFREE (&ueventllock);

			return (0);
		}
	}

	SPINFREE (&ueventllock);
	u.u_error = ENOSPC;
	return (-1);

}	/* end ueventalloc */

/*
 ****************************************************************
 *	Obtém o descritor de um evento específico		*
 ****************************************************************
 */
int
ueventget (const KFILE *fp, int eventid)
{
	UEVENTG		*egp;
	UEVENTL		*elp;
	INODE		*ip;	
	int		offset;

	if (fp->f_union != KF_EVENT)
		{ u.u_error = EBADF; return (-1); }

	offset = fp->f_ueventloff + eventid;

	elp = scb.y_ueventl + offset;

	if (elp->uel_uevent != (UEVENTG *)-1)
		return (offset);

	ip = fp->f_inode;

	/*
	 *	Procura na lista
	 */
	SPINLOCK (&ueventglock);

	if (ip->i_union == IN_EVENT)
	{
		for (egp = ip->i_ueventg; egp != NOUEVENTG; egp = egp->ueg_next)
		{
			if (egp->ueg_eventid == eventid) 
			{
				SPINFREE (&ueventglock);
				elp->uel_uevent = egp;
				return (offset);
			} 
		}
	}
	else
	{
		if (ip->i_union != IN_NULL)
		{
			printf ("%g: Campo \"union\" inválido\n");
			SPINFREE (&ueventglock);
			u.u_error = EINVAL; return (-1);
		}
	}

	/*
	 *	Não encontrou; insere
	 */
	if ((egp = ueventgfreelist) == NOUEVENTG)
		{ SPINFREE (&ueventglock); u.u_error = ENOSPC; return (-1); }

	ueventgfreelist = ueventgfreelist->ueg_next;
	egp->ueg_next = ip->i_ueventg;
	ip->i_union = IN_EVENT;
	ip->i_ueventg = egp;
	egp->ueg_eventid = eventid;

	SPINFREE (&ueventglock);

#ifdef	MSG
	if (CSWT (24))
		printf ("ueventget: Pegou o %P na ueventgfreelist\n", egp);
#endif	MSG

	elp->uel_uevent = egp;
	return (offset);

}	/* end ueventget */

/*
 ****************************************************************
 *	Obtém o endereço de um UEVENTG dado o descritor		*
 ****************************************************************
 */
UEVENTG *
ueventgaddr (int eventdescr)
{
	int		fd, offsetl, offsetg;
	const KFILE	*fp;
	UEVENTG		*egp;

	fd = eventdescr >> 16; offsetg = eventdescr & 0xFFFF;

	if (fd < 0 || fd > NUFILE)
		{ u.u_error = EINVAL; return ((UEVENTG *)-1); }

	if ((fp = u.u_ofile[fd]) == NOKFILE)
		{ u.u_error = EINVAL; return ((UEVENTG *)-1); }

	if (fp->f_union != KF_EVENT)
		{ u.u_error = EBADF; return ((UEVENTG *)-1); }

	/*
	 *	Verifica se o evento é consistente
	 */
	if ((offsetl = offsetg - fp->f_ueventloff) < 0 || offsetl > fp->f_ueventlno - 1)
		{ u.u_error = EINVAL; return ((UEVENTG *)-1); }

	/*
	 *	Verifica se o evento existe (falta colocar o erro correto)
	 */
	if ((egp = (scb.y_ueventl + offsetg)->uel_uevent) == (UEVENTG *)-1)
		{ u.u_error = ENOENT; return ((UEVENTG *)-1); }

	return (egp);

}	/* end ueventgaddr */

/*
 ****************************************************************
 *	Libera todos os eventos associados ao file		*
 ****************************************************************
 */
void
ueventlfcrelease (KFILE *fp)
{
	UEVENTL		*elp;
	int		i;

#if (0)	/*******************************************************/
	if (fp->f_union != KF_EVENT)
		{ u.u_error = EBADF; return; }
#endif	/*******************************************************/

	elp = scb.y_ueventl + fp->f_ueventloff; 

	for (i = fp->f_ueventlno; i > 0; i--)
		(elp++)->uel_uevent = NOUEVENTG;

	fp->f_union = KF_NULL;

}	/* end ueventlfcrelease */

/*
 ****************************************************************
 *	Libera todos os eventos associados ao inode		*
 ****************************************************************
 */
void
ueventgicrelease (INODE *ip)
{
	UEVENTG		*ep;

#ifdef	MSG
	if (CSWT (24))
	{
		printf
		(	"ueventgicrelease: Realmente liberando eventos "
			"do inode %P\n", ip
		);
	}
#endif	MSG

	/*
	 *	Percorre a lista, liberando todos os eventos deste inode
	 */
	while ((ep = ip->i_ueventg) != NOUEVENTG)
	{
		ip->i_ueventg = ep->ueg_next;

		EVENTDONE (&ep->ueg_event);

		ueventgput (ep);
#ifdef	MSG
		if (CSWT (24))
		{
			printf
			(	"ueventgicrelease: Liberou o %P para a "
				"ueventgfreelist\n",
				 ep
			);
		}
#endif	MSG
	}

}	/* end ueventgicrelease */

/*
 ****************************************************************
 *	Libera uma estrutura UEVENTG				*
 ****************************************************************
 */
void
ueventgput (UEVENTG *ep)
{
	SPINLOCK (&ueventglock);

	ep->ueg_next = ueventgfreelist;
	ueventgfreelist = ep;

	ep->ueg_event = 0;
	ep->ueg_eventid = 0;
	ep->ueg_count = 0;

	SPINFREE (&ueventglock);

}	/* end ueventgput */

/*
 ****************************************************************
 *	Inicializa a lista de UEVENTG				*
 ****************************************************************
 */
void
ueventginit (void)
{
	UEVENTG		*ep, *np = NOUEVENTG;

	for (ep = scb.y_endueventg - 1; ep >= scb.y_ueventg; ep--)
		{ ep->ueg_next = np; np = ep; }

	ueventgfreelist = np;

}	/* end ueventginit */

/*
 ****************************************************************
 *	Aloca um conjunto de semáforos associados a um file	*
 ****************************************************************
 */
int
usemaalloc (KFILE *fp, int nusemal)
{
	int		i = 0, j = 0;
	USEMAL		*slp;

	if (fp->f_union != KF_NULL)
		{ u.u_error = EBADF; return (-1); }

	if ((u.u_usemano + nusemal) > MAXNUSEMA)
		{ u.u_error = ENOSPC; return (-1); }

	SPINLOCK (&usemallock);

	for (slp = scb.y_usemal; slp < scb.y_endusemal; slp++)
	{
		if (slp->usl_usema == NOUSEMAG)
			j++;
		else
			j = 0;

		i++; 

		/*
		 *	Marca com -1 as entradas alocadas
		 */
		if (j >= nusemal)
		{
			for (j = nusemal; j > 0; j--)
				(slp--)->usl_usema = (USEMAG *)-1;

			fp->f_union = KF_SEMA;

			fp->f_usemalno = nusemal;
			fp->f_usemaloff = i - nusemal;

			SPINFREE (&usemallock);

			return (0);
		}
	}

	SPINFREE (&usemallock);
	u.u_error = ENOSPC;
	return (-1);

}	/* end usemaalloc */

/*
 ****************************************************************
 *	Obtém o descritor de um semáforo específico		*
 ****************************************************************
 */
int
usemaget (const KFILE *fp, int semaid, int initval)
{
	USEMAG		*sgp;
	USEMAL		*slp;
	INODE		*ip;	
	int		offset;

	if (fp->f_union != KF_SEMA)
		{ u.u_error = EBADF; return (-1); }

	offset = fp->f_usemaloff + semaid;

	slp = scb.y_usemal + offset;

	if (slp->usl_usema != (USEMAG *)-1)
		return (offset);

	ip = fp->f_inode;

	/*
	 *	Procura na lista
	 */
	SPINLOCK (&usemaglock);

	if (ip->i_union == IN_SEMA)
	{
		for (sgp = ip->i_usemag; sgp != NOUSEMAG; sgp = sgp->usg_next)
		{
			if (sgp->usg_semaid == semaid) 
			{
				SPINFREE (&usemaglock);
				slp->usl_usema = sgp;
				return (offset);
			} 
		}
	}
	else
	{
		if (ip->i_union != IN_NULL)
		{
			printf ("%g: Campo \"union\" inválido\n");
			SPINFREE (&usemaglock);
			u.u_error = EINVAL; return (-1);
		}
	}

	/*
	 *	Não encontrou; insere
	 */
	if ((sgp = usemagfreelist) == NOUSEMAG)
		{ SPINFREE (&usemaglock); u.u_error = ENOSPC; return (-1); }

	usemagfreelist = usemagfreelist->usg_next;
	sgp->usg_next = ip->i_usemag;
	ip->i_union = IN_SEMA;
	ip->i_usemag = sgp;
	sgp->usg_semaid = semaid; 
	
	SEMAINIT (&sgp->usg_sema, initval, 1 /* com histórico */);

	SPINFREE (&usemaglock);

#ifdef	MSG
	if (CSWT (24))
		printf ("usemag: Pegou o %P na usemagfreelist\n", sgp);
#endif	MSG

	slp->usl_usema = sgp;
	return (offset);
	
}	/* end usemaget */

/*
 ****************************************************************
 *	Obtém o endereço de um USEMAG dado o descritor		*
 ****************************************************************
 */
USEMAG *
usemagaddr (int semadescr)
{
	int		fd, offsetl, offsetg;
	KFILE		*fp;
	USEMAG		*sgp;

	fd = semadescr >> 16; offsetg = semadescr & 0xFFFF;

	if (fd < 0 || fd > NUFILE)
		{ u.u_error = EINVAL; return ((USEMAG *)-1); }

	if ((fp = u.u_ofile[fd]) == NOKFILE)
		{ u.u_error = EINVAL; return ((USEMAG *)-1); }

	if (fp->f_union != KF_SEMA)
		{ u.u_error = EBADF; return ((USEMAG *)-1); }

	/*
	 *	Verifica se o semáforo é consistente
	 */
	if ((offsetl = offsetg - fp->f_usemaloff) < 0 || offsetl > fp->f_usemalno - 1)
		{ u.u_error = EINVAL; return ((USEMAG *)-1); }

	/*
	 *	Verifica se o semáforo existe (falta colocar o erro correto)
	 */
	if ((sgp = (scb.y_usemal + offsetg)->usl_usema) == (USEMAG *)-1)
		{ u.u_error = ENOENT; return ((USEMAG *)-1); }

	return (sgp);

}	/* end usemagaddr */

/*
 ****************************************************************
 *	Libera todos os semáforos associados ao file		*
 ****************************************************************
 */
void
usemalfcrelease (KFILE *fp)
{
	USEMAL		*slp;
	USEMAG		*sgp;
	int		i;

#if (0)	/*******************************************************/
	if (fp->f_union != KF_SEMA)
		{ u.u_error = EBADF; return; }
#endif	/*******************************************************/

	slp = scb.y_usemal + fp->f_usemaloff; 

	/*
	 *	Devolve os recursos alocados
	 */
	for (i = fp->f_usemalno; i > 0; i--)
	{
		while (slp->usl_count > 0)
		{
			sgp = slp->usl_usema;
			syncset (E_SEMA,  E_GOT, &sgp->usg_sema);
			SEMAFREE (&sgp->usg_sema);
			slp->usl_count--;
		}

		(slp++)->usl_usema = NOUSEMAG;
	}

	fp->f_union = KF_NULL;

}	/* end usemalfcrelease */

/*
 ****************************************************************
 *	Libera todos os semáforos associados ao inode		*
 ****************************************************************
 */
void
usemagicrelease (INODE *ip)
{
	USEMAG		*sp;

#ifdef	MSG
	if (CSWT (24))
	{
		printf
		(	"usemagicrelease: Realmente liberando semaforos "
			"do inode %P\n", ip
		);
	}
#endif	MSG

	/*
	 *	Percorre a lista, liberando todos os semáforos deste inode
	 */
	while ((sp = ip->i_usemag) != NOUSEMAG)
	{
		ip->i_usemag = sp->usg_next;

		usemagput (sp);
#ifdef	MSG
		if (CSWT (24))
		{
			printf
			(	"usemagicrelease: Liberou o %P para a "
				"usemagfreelist\n", sp
			);
		}
#endif	MSG
	}

}	/* end usemagicrelease */

/*
 ****************************************************************
 *	Libera uma estrutura USEMAG				*
 ****************************************************************
 */
void
usemagput (USEMAG *sp)
{
	SPINLOCK (&usemaglock);

	sp->usg_next = usemagfreelist;
	usemagfreelist = sp;

	sp->usg_sema.e_lock = 0;
	sp->usg_sema.e_count = 0;
	sp->usg_semaid = 0;

	SPINFREE (&usemaglock);

}	/* end usemagput */

/*
 ****************************************************************
 *	Inicializa a lista de USEMAG				*
 ****************************************************************
 */
void
usemaginit (void)
{
	USEMAG		*sp, *np = NOUSEMAG;

	for (sp = scb.y_endusemag - 1; sp >= scb.y_usemag; sp--)
		{ sp->usg_next = np; np = sp; }

	usemagfreelist = np;

}	/* end usemaginit */
