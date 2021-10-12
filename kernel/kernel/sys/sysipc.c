/*
 ****************************************************************
 *								*
 *			sysipc.c				*
 *								*
 *	Eventos, Semáforos e Memória Compartilhada		*
 *								*
 *	Versão	1.0.0, de 19.10.90				*
 *		4.6.0, de 03.08.04				*
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

#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/signal.h"
#include "../h/default.h"
#include "../h/cpu.h"
#include "../h/mmu.h"
#include "../h/ipc.h"
#include "../h/uerror.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Chamada ao sistema "ueventctl"				*
 ****************************************************************
 */
int
ueventctl (UEVENTCTLFUNC func, register int fd, int arg1)
{
	register KFILE		*fp;

	if (fd < 0 || fd > NUFILE)
		{ u.u_error = EINVAL; return (-1); }

	if ((fp = u.u_ofile[fd]) == NOKFILE)
		{ u.u_error = EINVAL; return (-1); }

	switch (func)
	{
		/*
		 *	arg1 é o número total de eventos a serem alocados
		 */
	    case UE_ALLOC:
		if (arg1 < 0 || fp->f_union != KF_NULL)
			{ u.u_error = EINVAL; return (-1); }

		return (ueventalloc (fp, arg1));


		/*
		 *	arg1 é o no. do evento do qual se quer o descritor
		 */
	    case UE_GET:
		if (arg1 < 0 || fp->f_union != KF_EVENT || arg1 > (int)(fp->f_ueventlno - 1))
			{ u.u_error = EFBIG; return (-1); }

		return ((fd << 16) | ueventget (fp, arg1));

	    default:
		u.u_error = EINVAL;	/* colocar o erro correto */
		return (-1);
	}

}	/* end ueventctl */

/*
 ****************************************************************
 *	Chamada ao sistema "ueventwait"				*
 ****************************************************************
 */
int
ueventwait (int eventdescr, int eventordno)
{
	UEVENTG		*egp;
	int		dif;

	if ((egp = ueventgaddr (eventdescr)) == (UEVENTG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Verifica se o evento já ocorreu
	 */
	for (EVER)
	{
		/*
		 *	Primeiro faz a atribuição, depois testa!
		 */
		dif = eventordno - egp->ueg_count;

		if (dif <= 0)
			return (egp->ueg_count);

		EVENTCOUNTWAIT (eventordno, &egp->ueg_count,
						&egp->ueg_event, PWAIT);
		EVENTCLEAR (&egp->ueg_event);
	}
	
}	/* end ueventwait */

/*
 ****************************************************************
 *	Chamada ao sistema "ueventdone"				*
 ****************************************************************
 */
int
ueventdone (int eventdescr)
{
	UEVENTG		*egp;

	if ((egp = ueventgaddr (eventdescr)) == (UEVENTG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Sinaliza a ocorrência do evento
	 */
	SPINLOCK (&ueventglock);
	++(egp->ueg_count);
	SPINFREE (&ueventglock);

	EVENTDONE (&egp->ueg_event);

	/*
	 *	Retorna o contador de eventos
	 */
	return (egp->ueg_count);

}	/* end ueventdone */

/*
 ****************************************************************
 *	Chamada ao sistema "ueventtest"				*
 ****************************************************************
 */
int
ueventtest (int eventdescr)
{
	const UEVENTG	*egp;

	if ((egp = ueventgaddr (eventdescr)) == (UEVENTG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Retorna o contador de eventos
	 */
	return (egp->ueg_count);

}	/* end ueventtest */

/*
 ****************************************************************
 *	Chamada ao sistema "usemactl"				*
 ****************************************************************
 */
int
usemactl (USEMACTLFUNC func, int fd, int arg1, int arg2)
{
	register KFILE		*fp = NOKFILE;
	register const USEMAG	*sgp = NOUSEMAG;
	int			i;

	switch (func)
	{
		/*
		 *	Aqui fd é o descritor do arquivo associado
		 */
	    case US_ALLOC:
	    case US_GET:
		if (fd < 0 || fd > NUFILE)
			{ u.u_error = EINVAL; return (-1); }
	
		if ((fp = u.u_ofile[fd]) == NOKFILE)
			{ u.u_error = EINVAL; return (-1); }
		break;

		/*
		 *	Aqui fd é o descritor do próprio semáforo
		 */
	    case US_AVAIL:
	    case US_WAIT:
		if ((sgp = usemagaddr (fd)) == (USEMAG *)-1)
			{ u.u_error = EINVAL; return (-1); }
		break;

	    default:
		u.u_error = EINVAL; return (-1);
	}

	switch (func)
	{
		/*
		 *	arg1 é o número total de eventos a serem alocados
		 */
	    case US_ALLOC:
		if (arg1 < 0 || fp->f_union != KF_NULL)
			{ u.u_error = EINVAL; return (-1); }

		return (usemaalloc (fp, arg1));

		/*
		 *	arg1 é o no. do semáforo do qual se quer o descritor
		 *	arg2 é o valor inicial do semáforo (16 bits)
		 */
	    case US_GET:
		if (arg1 < 0 || fp->f_union != KF_SEMA || arg1 > (int)(fp->f_usemalno - 1))
			{ u.u_error = EFBIG; return (-1); }

		return ((fd << 16) | usemaget (fp, arg1, arg2));

		/*
		 *	No. de recursos livres
		 */
	    case US_AVAIL:
		if ((i = sgp->usg_sema.e_count) > 0)
			return (i);
		else
			return (0);

		/*
		 *	No. de threads/processos esperando recurso
		 */
	    case US_WAIT:
		if ((i = sgp->usg_sema.e_count) < 0)
			return (-i);
		else
			return (0);

	    default:
		u.u_error = EINVAL; return (-1);
	}

}	/* end usemactl */

/*
 ****************************************************************
 *	Chamada ao sistema "usemalock"				*
 ****************************************************************
 */
int
usemalock (int semadescr)
{
	register USEMAL		*slp;
	register USEMAG		*sgp;

	if ((sgp = usemagaddr (semadescr)) == (USEMAG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	slp = scb.y_usemal + (semadescr & 0xFFFF);

	SEMALOCK (&sgp->usg_sema, PWAIT);

	/*
	 *	Remove do histórico de sincronismo este semáforo
	 *	pois poderá ser outro processo que irá liberá-lo
	 */
	syncclr (E_SEMA, &sgp->usg_sema);

	slp->usl_count++;

	if (sgp->usg_sema.e_count > 0)
		return (sgp->usg_sema.e_count);
	else
		return (0);

}	/* end usemalock */

/*
 ****************************************************************
 *	Chamada ao sistema "usemafree"				*
 ****************************************************************
 */
int
usemafree (int semadescr)
{
	register USEMAL		*slp;
	register USEMAG		*sgp;

	if ((sgp = usemagaddr (semadescr)) == (USEMAG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	slp = scb.y_usemal + (semadescr & 0xFFFF);

	/*
	 *	Não pode liberar mais do que alocou
	 */
	if (slp->usl_count > 0)
	{
		slp->usl_count--;

		/*
		 *	Cria um histórico fictício, como se este
		 *	processo houvesse dado o SEMALOCK
		 */
		syncset (E_SEMA,  E_GOT, &sgp->usg_sema);
		SEMAFREE (&sgp->usg_sema);
	}

	if (sgp->usg_sema.e_count > 0)
		return (sgp->usg_sema.e_count);
	else
		return (0);

}	/* end usemafree */

/*
 ****************************************************************
 *	Chamada ao sistema "usematestl"				*
 ****************************************************************
 */
int
usematestl (int semadescr)
{
	register USEMAL		*slp;
	register USEMAG		*sgp;

	if ((sgp = usemagaddr (semadescr)) == (USEMAG *)-1)
		{ u.u_error = EINVAL; return (-1); }

	slp = scb.y_usemal + (semadescr & 0xFFFF);

	if (SEMATEST (&sgp->usg_sema) == -1)
		return (-1);

	/*
	 *	Remove do histórico de sincronismo este semáforo
	 *	pois poderá ser outro processo que irá liberá-lo
	 */
	syncclr (E_SEMA, &sgp->usg_sema);

	slp->usl_count++;

	if (sgp->usg_sema.e_count > 0)
		return (sgp->usg_sema.e_count);
	else
		return (0);

}	/* end usematestl */

/*
 ****************************************************************
 *	Chamada ao sistema "shmem"				*
 ****************************************************************
 */
void *
shmem (int fd, int byte_size)
{
	pg_t		size = BYUPPG (byte_size);
	KFILE		*fp;
	REGIONL		*rlp;
	REGIONG		*rgp;
	INODE		*ip;	

	/*
	 *	Prólogo
	 */
	if ((unsigned)fd >= NUFILE)
		{ u.u_error = EINVAL; return ((void *)-1); }

	if ((fp = u.u_ofile[fd]) == NOKFILE)
		{ u.u_error = EINVAL; return ((void *)-1); }

	if (fp->f_union != KF_NULL)
		{ u.u_error = EBADF; return ((void *)-1); }

	rlp = &fp->f_shmem_region;

	if (rlp->rgl_regiong != NOREGIONG)
		{ u.u_error = EBUSY; return ((void *)-1); }

	/*
	 *	Verifica se o processo pode alocar uma região nova
	 */
	if ((u.u_size + size) > scb.y_maxpsz)
		{ u.u_error = ENOSPC; return ((void *)-1); }

	/*
	 *	Prepara a REGIONL
	 */
   /***	rlp->rgl_regiong = rgp; ***/

	if ((rlp->rgl_vaddr = shmem_vaddr_alloc (size)) == 0)
		{ u.u_error = ENOSPC; return ((void *)-1); }

	if ((fp->f_flags & O_RW) == O_READ)
		rlp->rgl_prot = URO;
	else
		rlp->rgl_prot = URW;

   	rlp->rgl_type = RG_SHMEM;

	/*
	 *	Verifica se já há região alocada por outro processo
	 */
	ip = fp->f_inode;

	SLEEPLOCK (&ip->i_lock, PINODE);

	if   (ip->i_union == IN_NULL)
	{
		if (regiongrow (rlp, 0, size, RG_CLR) < 0)
		{
			shmem_vaddr_release (size, rlp->rgl_vaddr);
				{ u.u_error = ENOMEM; goto bad; }
		}

		ip->i_union  = IN_SHMEM;
		ip->i_shmemg = rlp->rgl_regiong;
	}
	elif (ip->i_union != IN_SHMEM)
	{
		printf ("%g: Campo \"union\" %d inválido\n", ip->i_union);
		u.u_error = EINVAL; goto bad;
	}
	elif ((rgp = ip->i_shmemg)->rgg_size != size)
	{
		shmem_vaddr_release (size, rlp->rgl_vaddr);
			{ u.u_error = EINVAL; goto bad; }
	}
	else
	{
		rlp->rgl_regiong = rgp;

		SPINLOCK (&rgp->rgg_regionlock);
		rgp->rgg_count++;
		SPINFREE (&rgp->rgg_regionlock);

		mmu_dir_load (rlp);
	}

   /***	rlp->rgl_regiong = rgp; ***/

	u.u_size += size;

	fp->f_union = KF_SHMEM;

	SLEEPFREE (&ip->i_lock);

	return ((void *)(rlp->rgl_vaddr << PGSHIFT));

	/*
	 *	Em caso de erro
	 */
    bad:
	SLEEPFREE (&ip->i_lock);
	return ((void *)-1);

}	/* end shmem */

/*
 ****************************************************************
 *	Libera as regiões compartilhadas associadas ao arquivo	*
 ****************************************************************
 */
void
shmem_release (KFILE *fp)
{
	REGIONL		*rlp = &fp->f_shmem_region;
	REGIONG		*rgp;

	/*
	 *	Chamado por fclose
	 */
	if ((rgp = rlp->rgl_regiong) != NOREGIONG)
	{
		SPINLOCK (&rgp->rgg_regionlock);
		rgp->rgg_count--;
		SPINFREE (&rgp->rgg_regionlock);

		u.u_size -= rgp->rgg_size;

		shmem_vaddr_release (rgp->rgg_size, rlp->rgl_vaddr);

		mmu_dir_unload (rlp);
	}

	memclr (rlp, sizeof (REGIONL));

	fp->f_union = KF_NULL;

}	/* end shmem_release */

/*
 ****************************************************************
 *	Aloca um endereço virtual em páginas para uma shmem	*
 ****************************************************************
 */
pg_t
shmem_vaddr_alloc (pg_t size)
{
	int		i;
	pg_t		vaddr, inc;
	long		vaddrmask;

#ifdef	MSG
	if (CSWT (24))
	{
		printf
		(	"shmem_vaddr_alloc: Alocando shmem com size = %d páginas\n",
		 	size
		);
	}
#endif	MSG

	if (size > FREGIONVATOTSZ)
		return (0);	/* Zero é endereço inválido */

	inc = FREGIONVASEGSZ; vaddrmask = 1;

	while (inc < size)
	{
		vaddrmask = (vaddrmask << 1) | 1;
		inc += FREGIONVASEGSZ;
	}

	i = 0; vaddr = FREGIONVABASE;

	while ((vaddrmask & u.u_shmemvaddrmask) != 0)
	{
		vaddrmask <<= 1;
		vaddr += FREGIONVASEGSZ;

		if (++i >= FREGIONSEGNO)
			return (0);	/* Zero é endereço inválido */
	}

	u.u_shmemvaddrmask |= vaddrmask; u.u_shmemnp += size;

#ifdef	MSG
	if (CSWT (24))
	{
		printf
		(	"shmem_vaddr_alloc: Shmem alocada com vaddr = %P \n",
			vaddr
		);
	}
#endif	MSG

	return (vaddr);
		
}	/* end shmem_vaddr_alloc */

/*
 ****************************************************************
 *	Libera o endereço virtual de uma shmem 			*
 ****************************************************************
 */
void
shmem_vaddr_release (pg_t size, pg_t addr)
{
	pg_t		vaddr, inc;
	long		vaddrmask;

	inc = FREGIONVASEGSZ; vaddrmask = 1;

	while (inc < size)
	{
		vaddrmask = (vaddrmask << 1) | 1;
		inc += FREGIONVASEGSZ;
	}

	vaddr = FREGIONVABASE;

	while (vaddr < addr)
	{
		vaddrmask <<= 1;
		vaddr += FREGIONVASEGSZ;
	}

	u.u_shmemvaddrmask &= ~vaddrmask; u.u_shmemnp -= size;

#ifdef	MSG
	if (CSWT (24))
	{
		printf
		(	"shmem_vaddr_release: Liberando shmem de size = %d"
			" no endereço virtual = %P\n", size, addr
		);
	}
#endif	MSG
		
}	/* end shmem_vaddr_release*/
