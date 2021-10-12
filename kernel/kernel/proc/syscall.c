/*
 ****************************************************************
 *								*
 *			syscall.c				*
 *								*
 *	Atendimento de chamadas ao sistema			*
 *								*
 *	Versão	3.0.0, de 11.12.94				*
 *		4.6.0, de 24.07.04				*
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

#include "../h/sysent.h"
#include "../h/frame.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Chamada ao sistema					*
 ****************************************************************
 */
void
syscall (struct syscall_frame frame)
{
	const SYSENT	*sp;
	long		*usp;

	/*
	 *	Reparar que esta chamada não é típica: tudo se passa
	 *	como se o argumento "frame" fosse dado "por referência".
	 */
	u.u_error = NOERR;

#ifdef	MSG
	if (CSWT (2))
		printf ("%d  ", frame.sf_r0);
#endif	MSG

	if ((unsigned)frame.sf_r0 >= NSYSCALL)
		sp = &sysent[0];
	else
		sp = &sysent[frame.sf_r0];

	usp = (long *)frame.sf_usp + 1;	/* Pula o endereço de retorno */

	/*
	 *	Guarda um minicontexto para o caso de system-call interrompido
	 */
	if (jmpset (u.u_qjmp) == 0)
	{
		if (u.u_error == NOERR)
			u.u_error = EINTR;
	}
	else
	{
		switch (sp->s_narg)
		{
		    case 0:
			frame.sf_r0 = (*sp->s_syscall) ();
			break;

		    case 1:
			frame.sf_r0 = (*sp->s_syscall) (usp[0]);
			break;

		    case 2:
			frame.sf_r0 = (*sp->s_syscall) (usp[0], usp[1]);
			break;

		    case 3:
			frame.sf_r0 = (*sp->s_syscall) (usp[0], usp[1], usp[2]);
			break;

		    case 4:
			frame.sf_r0 = (*sp->s_syscall) (usp[0], usp[1], usp[2], usp[3]);
			break;

		    case 5:
			frame.sf_r0 = (*sp->s_syscall) (usp[0], usp[1], usp[2], usp[3], usp[4]);
			break;

		    default:
			printf ("%g: Chamada ao sistema com %d argumentos\n", sp->s_narg);
			u.u_error = EINVAL;
			break;

		}	/* end switch */

	}	/* end if */

	/*
	 *	Prepara o resultado da chamada ao sistema para o usuário
	 */
	frame.sf_r2 = u.u_error;

#ifdef	MSG
	if (frame.sf_r2 != NOERR && CSWT (2))
		printf ("(erro %d)  ", frame.sf_r2);
#endif	MSG

	/*
	 *	Analisa as prioridades
	 */
	if (u.u_pri < bestpri)
		switch_run_process ();

	if (u.u_sig)
		sigexec (NOVOID, (void *)frame.sf_pc);

}	/* end syscall */
