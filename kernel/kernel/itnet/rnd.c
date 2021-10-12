/*
 ****************************************************************
 *								*
 *			rnd.c					*
 *								*
 *	Funções das filas circulares do protocolo TCP		*
 *								*
 *	Versão	3.0.36, de 04.04.98				*
 *		3.0.36, de 04.04.98				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/map.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Aloca as filas circulares				*
 ****************************************************************
 */
int
circular_area_alloc (register TCP_EP *tp)
{
	int		RND_SZ = tp->tp_rnd_in.tp_rnd_sz;
	pg_t		pgno, pgsz = BYUPPG (2 * RND_SZ);

	if (tp->tp_rnd_addr != 0)
		return (0);

	if ((pgno = malloce (M_CORE, pgsz)) == NOPG)
		{ u.u_error = ENOMEM; return (-1); }

	tp->tp_rnd_addr = pgno;
	tp->tp_rnd_size = pgsz;

	/*
	 *	Aloca a entrada
	 */
   /***	tp->tp_rnd_in.tp_rnd_sz    = RND_SZ; ***/
	tp->tp_rnd_in.tp_rnd_count = 0;
	tp->tp_rnd_in.tp_rnd_begin = (char *)PGTOBY (pgno);
	tp->tp_rnd_in.tp_rnd_end   = tp->tp_rnd_in.tp_rnd_begin + tp->tp_rnd_in.tp_rnd_sz;
	tp->tp_rnd_in.tp_rnd_head  = tp->tp_rnd_in.tp_rnd_begin;
	tp->tp_rnd_in.tp_rnd_tail  = tp->tp_rnd_in.tp_rnd_begin;

   /***	tp->tp_rnd_in.tp_rnd_lock  = 0; ***/
	tp->tp_rnd_in.tp_rnd_syn   = 0;
	tp->tp_rnd_in.tp_rnd_fin   = 0;

	/*
	 *	Aloca a saída
	 */
   /***	tp->tp_rnd_out.tp_rnd_sz    = RND_SZ; ***/
	tp->tp_rnd_out.tp_rnd_count = 0;
	tp->tp_rnd_out.tp_rnd_begin = tp->tp_rnd_in.tp_rnd_end;
	tp->tp_rnd_out.tp_rnd_end   = tp->tp_rnd_out.tp_rnd_begin + tp->tp_rnd_out.tp_rnd_sz;
	tp->tp_rnd_out.tp_rnd_head  = tp->tp_rnd_out.tp_rnd_begin;
	tp->tp_rnd_out.tp_rnd_tail  = tp->tp_rnd_out.tp_rnd_begin;

   /***	tp->tp_rnd_out.tp_rnd_lock  = 0; ***/
	tp->tp_rnd_out.tp_rnd_syn   = 0;
	tp->tp_rnd_out.tp_rnd_fin   = 0;

#ifdef	MSG
	if (itscb.it_list_info)
	{
		printf
		(	"%g: Aloquei as filas circulares, (%P, %d)\n",
			tp->tp_rnd_in.tp_rnd_begin, 2 * tp->tp_rnd_in.tp_rnd_sz
		);
	}
#endif	MSG

	return (0);

}	/* end circular_area_alloc */


/*
 ****************************************************************
 *	Desaloca as filas circulares				*
 ****************************************************************
 */
void
circular_area_mrelease (register TCP_EP *tp)
{
	if (tp->tp_rnd_addr == 0)
		return;

	mrelease (M_CORE, tp->tp_rnd_size, tp->tp_rnd_addr);
	tp->tp_rnd_addr = 0;

#ifdef	MSG
	if (itscb.it_list_info)
	{
		printf
		(	"%g: Desaloquei as filas circulares, (%P, %d)\n",
			tp->tp_rnd_in.tp_rnd_begin, 2 * tp->tp_rnd_in.tp_rnd_sz
		);
	}
#endif	MSG

}	/* end circular_area_mrelease */

/*
 ****************************************************************
 *	Escreve em uma fila circular				*
 ****************************************************************
 */
int
circular_area_put (register RND *rp, const char *area, int count, int cpflag)
{
	int		cnt, total = 0;

	/*
	 *	Verifica se tem espaço
	 */
	if (count == 0)
		return (0);

	if (rp->tp_rnd_count + count > rp->tp_rnd_sz)
		goto bad;

	/*
	 *	Escreve os dados
	 */
	if   (rp->tp_rnd_tail > rp->tp_rnd_head)
	{
		cnt = rp->tp_rnd_tail - rp->tp_rnd_head;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (rp->tp_rnd_head, area, cnt, cpflag) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_head  += cnt;
			rp->tp_rnd_count += cnt;

		   /***	area  += cnt; ***/
		   	count -= cnt;
			total += cnt;
		}
	}
	else	/* rp->tp_rnd_tail <= rp->tp_rnd_head */
	{
		cnt = rp->tp_rnd_end - rp->tp_rnd_head;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (rp->tp_rnd_head, area, cnt, cpflag) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_head  += cnt;
			rp->tp_rnd_count += cnt;

			area  += cnt;
			count -= cnt;
			total += cnt;
		}

		if (count == 0)
			return (total);

		cnt = rp->tp_rnd_tail - rp->tp_rnd_begin;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (rp->tp_rnd_begin, area, cnt, cpflag) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_head   = rp->tp_rnd_begin + cnt;
			rp->tp_rnd_count += cnt;

		   /***	area  += cnt; ***/
		   	count -= cnt;
			total += cnt;
		}
	}

	/*
	 *	Epílogo
	 */
	if (count != 0)
	{
	    bad:
#ifdef	MSG
		printf ("%g: Dados não couberam na fila circular\n");
#endif	MSG
		{ u.u_error = ENOMEM; total = -1; }
	}

	return (total);

} 	/* end circular_area_put */

/*
 ****************************************************************
 *	Lê de uma fila circular (removendo, somente SU)		*
 ****************************************************************
 */
int
circular_area_get (register RND *rp, char *area, int count)
{
	int		cnt, total = 0;

	/*
	 *	Verifica se a fila está vazia
	 */
	if (rp->tp_rnd_count == 0)
		return (0);

	/*
	 *	Le os dados
	 */
	if   (rp->tp_rnd_head > rp->tp_rnd_tail)
	{
		cnt = rp->tp_rnd_head - rp->tp_rnd_tail;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (area, rp->tp_rnd_tail, cnt, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_tail  += cnt;
			rp->tp_rnd_count -= cnt;

		   /***	area  += cnt; ***/
		   /***	count -= cnt; ***/
			total += cnt;
		}
	}
	else	/* rp->tp_rnd_head <= rp->tp_rnd_tail */
	{
		cnt = rp->tp_rnd_end - rp->tp_rnd_tail;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (area, rp->tp_rnd_tail, cnt, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_tail  += cnt;
			rp->tp_rnd_count -= cnt;

		   	area  += cnt;
		   	count -= cnt;
			total += cnt;
		}

		if (count <= 0)
			return (total);

		cnt = rp->tp_rnd_head - rp->tp_rnd_begin;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (unimove (area, rp->tp_rnd_begin, cnt, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			rp->tp_rnd_tail   = rp->tp_rnd_begin + cnt;
			rp->tp_rnd_count -= cnt;

		   /***	area  += cnt; ***/
		   /***	count -= cnt; ***/
			total += cnt;
		}
	}

	return (total);

} 	/* end circular_area_get */

/*
 ****************************************************************
 *	Lê de uma fila circular (sem remover, somente SS)	*
 ****************************************************************
 */
int
circular_area_read (register RND *rp, char *area, int count, int offset)
{
	register const char	*cp;
	int			cnt, total = 0;

	/*
	 *	Verifica se a fila está vazia
	 */
	if (rp->tp_rnd_count == 0)
		return (0);

	/*
	 *	Le os dados
	 */
	if   (rp->tp_rnd_head > rp->tp_rnd_tail)
	{
		cp  = rp->tp_rnd_tail + offset;
		cnt = rp->tp_rnd_head - cp;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (memmove (area, cp, cnt) < 0)
				{ u.u_error = EFAULT; return (-1); }

		   /***	cp += cnt; ***/

		   /***	area  += cnt; ***/
		   /***	count -= cnt; ***/
			total += cnt;
		}
	}
	else	/* rp->tp_rnd_head <= rp->tp_rnd_tail */
	{
		cp  = rp->tp_rnd_tail + offset;
		cnt = rp->tp_rnd_end - cp;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (memmove (area, cp, cnt) < 0)
				{ u.u_error = EFAULT; return (-1); }

		   /***	cp += cnt; ***/

		   	area  += cnt;
		   	count -= cnt;
			total += cnt;

			offset = 0;
		}
		else
		{
			offset = -cnt;
		}

		if (count <= 0)
			return (total);

		cp  = rp->tp_rnd_begin + offset;
		cnt = rp->tp_rnd_head - cp;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
			if (memmove (area, cp, cnt) < 0)
				{ u.u_error = EFAULT; return (-1); }

		   /***	cp += cnt; ***/

		   /***	area  += cnt; ***/
		   /***	count -= cnt; ***/
			total += cnt;
		}
	}

	return (total);

} 	/* end circular_area_read */

/*
 ****************************************************************
 *	Remove do final de uma fila circular			*
 ****************************************************************
 */
int
circular_area_del (register RND *rp, int count)
{
	int		cnt, total = 0;

	/*
	 *	Verifica se a fila está vazia
	 */
	if (rp->tp_rnd_count == 0)
		return (0);

	/*
	 *	Remove os dados
	 */
	if   (rp->tp_rnd_head > rp->tp_rnd_tail)
	{
		cnt = rp->tp_rnd_head - rp->tp_rnd_tail;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
		   	rp->tp_rnd_tail  += cnt;
		   	rp->tp_rnd_count -= cnt;

		   /***	area  += cnt; ***/
		   	count -= cnt;
			total += cnt;
		}
	}
	else	/* rp->tp_rnd_head <= rp->tp_rnd_tail */
	{
		cnt = rp->tp_rnd_end - rp->tp_rnd_tail;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
		   	rp->tp_rnd_tail  += cnt;
		   	rp->tp_rnd_count -= cnt;

		   /***	area  += cnt; ***/
		   	count -= cnt;
			total += cnt;
		}

		if (count <= 0)
			return (total);

		cnt = rp->tp_rnd_head - rp->tp_rnd_begin;

		if (cnt > count)
			cnt = count;

		if (cnt > 0)
		{
		   	rp->tp_rnd_tail   = rp->tp_rnd_begin + cnt;
		   	rp->tp_rnd_count -= cnt;

		   /***	area  += cnt; ***/
			count -= cnt;
			total += cnt;
		}
	}

	return (total);

} 	/* end circular_area_del */
