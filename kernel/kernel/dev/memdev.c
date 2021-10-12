/*
 ****************************************************************
 *								*
 *			memdev.c				*
 *								*
 *	Driver para a memória, "null" e "grave"			*
 *								*
 *	Versão	1.0.0, de 27.01.86				*
 *		4.6.0, de 09.08.04				*
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
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/ioctl.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 *	"Minors" deste "driver":
 *
 *		MINOR 0 => Memória Fisica
 *		MINOR 1 => Memória Kernel
 *		MINOR 2 => SUMIDOURO
 *		MINOR 3 => Grave (para "wait")
 */

/*
 ****************************************************************
 *	Leitura da memória					*
 ****************************************************************
 */
void
memread (IOREQ *iop)
{
	void		*kernel_addr;

	switch (MINOR (iop->io_dev))
	{
	    case 0:		/* "mem" */
		kernel_addr = (void *)PHYS_TO_VIRT_ADDR (iop->io_offset_low);
		break;

	    case 1:		/* "kmem" */
		kernel_addr = (void *)iop->io_offset_low;
		break;

	    case 2:		/* "null" */
		return;

	    default:
		u.u_error = EINVAL;
		return;
	}

	if (unimove (iop->io_area, kernel_addr, iop->io_count, SU) >= 0)
		{ iop->io_offset_low += iop->io_count; iop->io_count = 0; }
	else
		u.u_error = EINVAL;

}	/* end memread */

/*
 ****************************************************************
 *	Escrita na Memória					*
 ****************************************************************
 */
void
memwrite (IOREQ *iop)
{
	void		*kernel_addr;

	switch (MINOR (iop->io_dev))
	{
	    case 0:		/* "mem" */
		kernel_addr = (void *)PHYS_TO_VIRT_ADDR (iop->io_offset_low);
		break;

	    case 1:		/* "kmem" */
		kernel_addr = (void *)iop->io_offset_low;
		break;

	    case 2:		/* "null" */
		iop->io_count = 0;
		return;

	    default:
		u.u_error = EINVAL;
		return;
	}

	if (unimove (kernel_addr, iop->io_area, iop->io_count, SU) >= 0)
		{ iop->io_offset_low += iop->io_count; iop->io_count = 0; }
	else
		u.u_error = EINVAL;

}	/* end memwrite */

/*
 ****************************************************************
 *	Funções de controle					*
 ****************************************************************
 */
int
memctl (dev_t dev, int cmd, void *arg, int flag)
{
	UPROC		*child_up;

	/*
	 *	Processa/prepara o ATTENTION para o GRAVE (minor 3)
	 */
	if (MINOR (dev) != 3)
		{ u.u_error = EINVAL; return (-1); }

	switch (cmd)
	{
	    case U_ATTENTION:
		SPINLOCK (&u.u_childlock);

		for (child_up = u.u_child; child_up != NOUPROC; child_up = child_up->u_sibling)
		{
			if (child_up->u_state == SZOMBIE)
				{ SPINFREE (&u.u_childlock); return (1); }
		}

		u.u_grave_index = (int)arg;

		u.u_grave_attention_set = 1;
		*(char **)flag	= &u.u_grave_attention_set;

		SPINFREE (&u.u_childlock);

		return (0);
	}

	/*
	 *	Demais IOCTLs
	 */
	u.u_error = EINVAL; return (-1);
	
}	/* end memctl */
