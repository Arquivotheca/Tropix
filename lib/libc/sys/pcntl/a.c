
#include <sys/types.h>
#include "./times.h"
#include "./pcntl.h"

#define	MIL	1000
#define	MILHAO	1000000

PMUTMS		buffer, obuffer;
PMUTMS		zbuffer = {0, 0, 0, 0, 0 ,0, 0, 0};
MUTM		before, after;

#define	NTHREADS	50
#define	NDIM		50

extd	a[NDIM][NDIM];
extd	b[NDIM][NDIM];
extd	c[NDIM][NDIM];
int	th[NDIM];

main ()
{
	int	i, j, k;
	int	thid;
	int	ndim, nthreads;

	printf ("Dê o no. de loops e o valor de cada um\n");
	scanf ("%d%d", &nthreads, &ndim);

	mutime (&before);
	pcntl (P_PMUTIMES, &obuffer);

	for (i = 0; i < nthreads; i++)
	{
		if ((th[i] = thread ()) == 0)
		{
			for (j = 0; j < ndim; j++)
			{
			}
			_exit (0);
		}

		if (th[i] < 0)
			printf ("Não consegui criar thread %d\n", i);

/*
		if (th[i] > 0)
			printf ("Consegui criar thread no. %d\n", th[i]);
*/
	}

/*
	printf ("\n");
*/

	for (i = 0; i < nthreads; i++)
	{
		thid = wait (0);
/*
		printf ("Terminou thread no. %d\n", thid);
*/
	}

	mutime (&after);
	pcntl (P_PMUTIMES, &buffer);

	printtime ();
}

/*
 ****************************************************************
 *	Imprime o tempo						*
 ****************************************************************
 */
printtime ()
{
	MUTM	*fp, *cp;

	printf ("\n");
	printmu ("REAL", &before, &after);
	
	fp = (MUTM *)&buffer.mu_seqtime;
	cp = (MUTM *)&buffer.mu_cseqtime;
	fp->mu_time += cp->mu_time;
	fp->mu_utime += cp->mu_utime;
	if (fp->mu_utime > MILHAO)
	{
		fp->mu_utime -= MILHAO;
		fp->mu_time += 1;
	}
	printmu ("SEQ", (MUTM *)&zbuffer.mu_seqtime, (MUTM *)&buffer.mu_cseqtime);

	fp = (MUTM *)&buffer.mu_partime;
	cp = (MUTM *)&buffer.mu_cpartime;
	if ((fp->mu_time > cp->mu_time) ||
		(fp->mu_time == cp->mu_time && fp->mu_utime > cp->mu_utime))
	printmu ("PARF", (MUTM *)&obuffer.mu_partime, (MUTM *)&buffer.mu_partime);
	else
	printmu ("PARC", (MUTM *)&obuffer.mu_partime, (MUTM *)&buffer.mu_cpartime);
	printf ("\n");

}

/*
 ****************************************************************
 *	Converte e imprime o tempo				*
 ****************************************************************
 */
printmu (s, ap, dp)
char		*s;
register MUTM	*ap, *dp;
{
	register	min, seg, ms, us; 

	if (dp->mu_utime < ap->mu_utime)
	{
		dp->mu_time--;
		dp->mu_utime += MILHAO;
	}

	seg = dp->mu_time  - ap->mu_time;
	us  = dp->mu_utime - ap->mu_utime;

	min = seg / 60;
	seg = seg % 60;

	ms  = us / MIL;
	us  = us % MIL;

	printf ("%s %3d:%02d.%03d          ", s, min, seg, ms);

}	/* end printmu */
