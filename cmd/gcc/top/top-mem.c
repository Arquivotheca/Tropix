/*
 ****************************************************************
 *								*
 *			top/top-mem.c				*
 *								*
 *	This is the top level of cc1.				*
 * 	It parses command args, opens files, invokes the	*
 *	various passes in the proper order, and counts the	*
 *	time used by each. Error messages and low-level		*
 *	interface to malloc also handled here.			*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 02.01.94				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */


/*
 ******	Inclusões ***********************************************
 */
#pragma	getenv (GEN)

#include <sys/types.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/flags.h"

#include "top.h"

/*
 ******	Definições locais ***************************************
 */
#define	NOVOID	(void *)NULL

entry int	float_handled;
entry jmp_buf	float_handler;

/*
 *	Here is a table, controlled by the tm-...h file, listing
 *	each -m switch and which bits in `target_switches' it
 *	should set or clear. If VALUE is positive, it is bits to set.
 *	If VALUE is negative, -VALUE is bits to clear. (The sign bit
 *	is not used so there is no confusion.)
 */
const struct
{
	char	*name;
	int	value;

}	target_switches[] = TARGET_SWITCHES;

/*
 ****************************************************************
 *	Return time used so far, in microseconds		*	
 ****************************************************************
 */
int
gettime (void)
{
	MUTMS		mp;
	register long	seg, mu;

	if (quiet_flag)
		return (0);

	mutimes (&mp);

	seg = mp.mu_utime  + mp.mu_stime  + mp.mu_cutime  + mp.mu_cstime;
	mu  = mp.mu_uutime + mp.mu_sutime + mp.mu_cuutime + mp.mu_csutime;

	return (seg * 1000000 + mu);

}	/* end gettime */

#if (0)	/*************************************/
/*
 ****************************************************************
 *	Imprime o tempo gasto					*
 ****************************************************************
 */
void
print_time (char *str, int total)
{
	fprintf
	(	stderr,
		"time in %s: %d.%06d\n",
		str, total / 1000000, total % 1000000
	);

}	/* end print_time */
#endif	/*************************************/

/*
 ****************************************************************
 *  Same as `malloc' but report error if no memory available	*
 ****************************************************************
 */
void *
xmalloc (int size)
{
	register void	*value = malloc (size);

	if (value == NOVOID)
		fatal ("Virtual memory exhausted.");

	return (value);

}	/* end xmalloc */

/*
 ****************************************************************
 *  Same as `realloc' but report error if no memory available 	*
 ****************************************************************
 */
void *
xrealloc (void *ptr, int size)
{
	void		*result = realloc (ptr, size);

	if (!result)
		fatal ("Virtual memory exhausted.");

	return (result);

}	/* end xrealloc */

/*
 ****************************************************************
 *  Return the logarithm of X, base 2, considering X unsigned	*
 ****************************************************************
 */
/*
 *	Return the logarithm of X, base 2, considering X unsigned,
 *	if X is a power of 2. Otherwise, returns -1
 */
int
exact_log2 (register unsigned int x)
{
	register int	log = 0;

	for (log = 0; log < HOST_BITS_PER_INT; log++)
	{
		if (x == (1 << log))
			return (log);
	}

	return (-1);

}	/* end exact_log2 */

/*
 ****************************************************************
 *	Return the largest int Y such that 2**Y <= X		*
 ****************************************************************
 */
/*
 *	Given X, an unsigned number, return the largest int Y
 *	such that 2**Y <= X. If X is 0, return -1
 */
int
floor_log2 (register unsigned int x)
{
	register int	log = 0;

	for (log = 0; log < HOST_BITS_PER_INT; log++)
	{
		if ((x & ((-1) << log)) == 0)
			return (log - 1);
	}

	return (HOST_BITS_PER_INT - 1);

}	/* end floor_log2 */

/*
 ****************************************************************
 *  Specify where to longjmp to when a floating error happens	*
 ****************************************************************
 */
/*
 *	Specify where to longjmp to when a floating arithmetic
 *	error happens. If HANDLER is 0, it means don't handle
 *	the errors any more
 */
void
set_float_handler (jmp_buf handler)
{
	float_handled = (handler != (int *)0); /* "jmp_buf" é um vetor de ints */

	if (handler)
		memmove (float_handler, handler, sizeof (float_handler));

}	/* end set_float_handler */

/*
 ****************************************************************
 *	Signals actually come here				*
 ****************************************************************
 */
void
float_signal (void)
{
	if (float_handled == 0)
		abort ();
	float_handled = 0;
	longjmp (float_handler, 1);

}	/* end float_signal */

/*
 ****************************************************************
 *	Handler for SIGPIPE					*
 ****************************************************************
 */
void
pipe_closed (void)
{
	fatal ("output pipe has been closed");

}	/* end pipe_closed */

/*
 ****************************************************************
 *	Decode the switch -mNAME				*
 ****************************************************************
 */
void
set_target_switch (char *name)
{
	register int	j;

	for (j = 0; j < sizeof target_switches / sizeof target_switches[0]; j++)
	{
		if (!strcmp (target_switches[j].name, name))
		{
			if (target_switches[j].value < 0)
				target_flags &= ~-target_switches[j].value;
			else
				target_flags |= target_switches[j].value;
			return;
		}
	}

	error ("Invalid option `%s'", name);

}	/* end set_target_switch */

/*
 ****************************************************************
 *	Print default target switches for -version		*
 ****************************************************************
 */
void
print_target_switch_defaults (void)
{
	register int	j;
	register int	mask = TARGET_DEFAULT;

	fprintf (stderr, "default target switches:");

	for (j = 0; j < sizeof target_switches / sizeof target_switches[0]; j++)
	{
		if
		(	target_switches[j].name[0] != '\0' &&
			target_switches[j].value > 0 &&
			(target_switches[j].value & mask) == target_switches[j].value
		)
			fprintf (stderr, " -m%s", target_switches[j].name);
	}

	fprintf (stderr, "\n");

}	/* end print_target_switch_defaults */
