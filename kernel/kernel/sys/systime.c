/*
 ****************************************************************
 *								*
 *			systime.c				*
 *								*
 *	Chamadas ao sistema: tempos & Datas			*
 *								*
 *	Versão	3.0.0, de 05.01.95				*
 *		4.0.0, de 29.06.00				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/times.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
#define	MILHAO	1000000

#define PIT_FREQ 1193182	/* Freqüência do relógio do PIT */

/*
 ******	Protótipos de funções ***********************************
 */
void		mutimecvt (time_t, time_t []);

/*
 ****************************************************************
 *	Chamada ao sistema "time"				*
 ****************************************************************
 */
long
gtime (void)
{
	return (time);

}	/* end gtime */

/*
 ****************************************************************
 *	Chamada ao sistema "stime"				*
 ****************************************************************
 */
int
stime (const time_t *tp)
{
	time_t		t;

	if ((t = gulong (tp)) == -1)
		{ u.u_error = EFAULT; return (-1); }

	if (superuser () == 0)
		{ time = t; sb_head.sb_forw->sb_sbmod = 1; }

	return (UNDEF);

}	/* end stime */

/*
 ****************************************************************
 *	Chamada ao sistema "utime"				*
 ****************************************************************
 */
int
utime (const char *path, const time_t *tp)
{
	register INODE	*ip;
	time_t		tv[3];

	if ((ip = owner (path)) == NOINODE)
		return (-1);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		{ u.u_error = EROFS; goto bad; }

	/*
	 *	Altera os tempos
	 */
	if (tp == (time_t *)NULL)	/* Tempos de acesso e modificação */
	{
		ip->i_atime = time;
		ip->i_mtime = time;
	}
	else				/* Os 3 tempos */
	{
		if (unimove (tv, tp, sizeof (tv), US))
		{
			u.u_error = EFAULT;
			goto bad;
		}

		ip->i_atime = tv[0];
		ip->i_mtime = tv[1];
		ip->i_ctime = tv[2];
	}

	inode_dirty_inc (ip);

    bad:
	iput (ip);

	return (UNDEF);

}	/* end utime */

/*
 ****************************************************************
 *	Chamada ao sistema "times"				*
 ****************************************************************
 */
time_t
times (TMS *tp)
{
	if (unimove (tp, &u.u_utime, sizeof (TMS), SU) < 0)
		u.u_error = EFAULT;

	return (uptime);

}	/* end times */

/*
 ****************************************************************
 *	Chamada ao sistema "mutime"				*
 ****************************************************************
 */
int
mutime (MUTM *tp)
{
	register int	ticks;
	register int	PIT_val;
	MUTM		mutm;

	/*
	 *	Falta pensar sobre as CPUs N
	 */
	disable_int ();

	write_port (0x00, 0x43);	/* Latch */
	PIT_val = read_port (0x40) + (read_port (0x40) << 8);

	mutm.mu_time = time; 	ticks = hz;

	enable_int ();

	/*
	 *	Agora faz o cálculo
	 */
	mutm.mu_utime = mul_div_64 (ticks, MILHAO, scb.y_hz) +
			mul_div_64 (PIT_init - PIT_val, MILHAO, PIT_FREQ);

	if (unimove (tp, &mutm, sizeof (MUTM), SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end mutime */

/*
 ****************************************************************
 *	Chamada ao sistema "mutimes"				*
 ****************************************************************
 */
time_t
mutimes (MUTMS *tp)
{
	MUTMS	mutms;

	mutimecvt (u.u_utime,  &mutms.mu_utime);
	mutimecvt (u.u_stime,  &mutms.mu_stime);
	mutimecvt (u.u_cutime, &mutms.mu_cutime);
	mutimecvt (u.u_cstime, &mutms.mu_cstime);

	if (unimove (tp, &mutms, sizeof (MUTMS), SU) < 0)
		u.u_error = EFAULT;

	return (uptime);

}	/* end mutimes */

/*
 ****************************************************************
 *	Conversão do tempo de ticks para microsegundos		*
 ****************************************************************
 */
void
mutimecvt (time_t ticks, time_t museg[])
{
	register long	y_hz;

	y_hz = scb.y_hz;

	museg[0] =  ticks / y_hz;
	museg[1] = (ticks % y_hz) * MILHAO / y_hz;

}	/* end mutimecvt */


#if (0)	/*************************************/
/*
 ****************************************************************
 *	Chamada ao sistema "pmutimes"				*
 ****************************************************************
 */
time_t
pmutimes (tp)
PMUTMS	*tp;
{
	PMUTMS	mutms;

	mutimecvt (u.u_seqtime,  &mutms.mu_seqtime);
	mutimecvt (u.u_cseqtime, &mutms.mu_cseqtime);
	mutimecvt (u.u_partime,  &mutms.mu_partime);
	mutimecvt (u.u_cpartime, &mutms.mu_cpartime);

	if (unimove (tp, &mutms, sizeof (PMUTMS), SU) < 0)
		u.u_error = EFAULT;

	return (uptime);

}	/* end pmutimes */

#endif	/*************************************/
