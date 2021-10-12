/*
 ****************************************************************
 *								*
 *			printr.c				*
 *								*
 *	Imprime a Tabela de Interrupções			*
 *								*
 *	Versão	4.5.0, de 11.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/intr.h>

#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <a.out.h>

/*
 ****************************************************************
 *	Definições Globais					*
 ****************************************************************
 */
#define	NIRQ	16	/* Número máximo de IRQs */

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	SYM		*sp;
	int		irq, ind;
	VECDEF		*vp;
	const char	vecdef_nm[] = "_vecdef";
	int		*pl_vec;
	const char	irq_pl_vec_nm[] = "_irq_pl_vec";

	/*
	 *	Obtém e mapeia o endereço da tabela
	 */
	sp = alloca (256);

	strcpy (sp->s_name, vecdef_nm);

	if (kcntl (K_GETSYMGN, strlen (vecdef_nm), sp) < 0)
		error ("$*Não obtive o endereço de \"%s\"", vecdef_nm);

	vp =	phys
		(	(void *)sp->s_value,
			NIRQ * VECLINESZ * sizeof (VECDEF),
			O_RDONLY|O_KERNEL
		);

	if ((int)vp == -1)
		error ("$*Não consegui mapear o endereço %P\n", sp->s_value);

	strcpy (sp->s_name, irq_pl_vec_nm);

	if (kcntl (K_GETSYMGN, strlen (irq_pl_vec_nm), sp) < 0)
		error ("$*Não obtive o endereço de \"%s\"", irq_pl_vec_nm);

	pl_vec=	phys
		(	(void *)sp->s_value,
			NIRQ * sizeof (int),
			O_RDONLY|O_KERNEL
		);

	if ((int)pl_vec == -1)
		error ("$*Não consegui mapear o endereço %P\n", sp->s_value);

	/*
	 *	Percorre a tabela, imprimindo
	 */
	for (irq = 0; irq < NIRQ; irq++)
	{
		printf ("IRQ %2d:  %d ", irq, pl_vec[irq]);
#if (0)	/*******************************************************/
		printf ("IRQ %2d:  ", irq);
#endif	/*******************************************************/

		for (ind = 0; ind < VECLINESZ; ind++, vp++)
		{
			if (vp->vec_func == NOVOID)
				continue;

			/* Obtém o nome da função, a partir do endereço */

			sp->s_nm_len = 256 - sizeof (SYM);
			sp->s_value  = (long)vp->vec_func;

			if (kcntl (K_GETSYMGA, (ulong)vp->vec_func, sp) < 0)
				sprintf (sp->s_name, "%P", sp->s_value);

			printf (" [%d, %s]", vp->vec_unit, sp->s_name);
		}

		putchar ('\n');
	}

	phys (vp,     0, 0);
	phys (pl_vec, 0, 0);

	return (0);

}	/* end main */
