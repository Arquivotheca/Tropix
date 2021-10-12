/*
 ****************************************************************
 *								*
 *			region.c				*
 *								*
 *	Funções para manuseio de regiões de memória		*
 *								*
 *	Versão	3.0.0, de 20.10.94				*
 *		4.6.0, de 11.08.04				*
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

#include "../h/cpu.h"
#include "../h/map.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry LOCK	regionglock;		/* Lock para a lista global de regiões */
entry REGIONG	*regiongfreelist;	/* Ponteiro para a lista livre de REGIONG */
entry int	regiong_list_count;	/* Contador de REGIONG usados */

#undef	REGIONLOG
#ifdef	REGIONLOG
/*
 ****** Protocolo das chamadas a "regiongrow" *******************
 */
typedef struct
{
	long	r_pid;
	int	r_type;
	pg_t	r_old_size;
	pg_t	r_new_size;

}	REGION_LOG;

#define NLOG	200

entry REGION_LOG	region_log[NLOG];
entry REGION_LOG	*region_log_ptr = region_log;

#endif	REGIONLOG

/*
 ****************************************************************
 *	Altera o tamanho de uma região				*
 ****************************************************************
 */
int
regiongrow (REGIONL *rlp, pg_t new_vaddr, pg_t new_size, int opflags)
{
	pg_t		old_size = 0;
	pg_t		old_paddr, new_paddr;
	pg_t		inc;
	REGIONG		*rgp;

	/*
	 *	Pequena consistência
	 */
	if (rlp->rgl_type == 0)
		{ printf ("%g: Tipo da região NULO ?\n"); return (-1); }

	if (rlp->rgl_prot == 0)
		{ printf ("%g: Proteção da região NULO ?\n"); return (-1); }

	/*
	 *	Se o novo endereço virtual não é dado, não muda
	 */
	if (new_vaddr == 0)
		new_vaddr = rlp->rgl_vaddr;

#ifdef	REGIONLOG
	/*
	 *	Guarda o LOG
	 */
	if (region_log_ptr >= &region_log[NLOG])
		region_log_ptr = &region_log[0];

	region_log_ptr->r_pid		= u.u_pid;
	region_log_ptr->r_type		= rlp->rgl_type;
	region_log_ptr->r_old_size	= 0;
	region_log_ptr->r_new_size	= new_size;

	if (rlp->rgl_regiong != NOREGIONG)
		region_log_ptr->r_old_size = rlp->rgl_regiong->rgg_size;

	region_log_ptr++;
#endif	REGIONLOG

	/*
	 ******	Verifica se a região vai desaparecer ************
	 */
	if (new_size <= 0)
	{
		if (new_size < 0)
			printf ("%g: Tamanho %d negativo\n", new_size);

		regionrelease (rlp);
		return (0);
	}

	/*
	 ******	Verifica se o endereço virtual mudou ************
	 */
	if (new_vaddr != rlp->rgl_vaddr && rlp->rgl_type != RG_STACK && rlp->rgl_vaddr != NOPG)
	{
		char	rgl_type = rlp->rgl_type;
		char	rgl_prot = rlp->rgl_prot;
#ifdef DEBUG
		printf ("regiongrow: Endereço virtual mudando de %P para %P\n", rlp->rgl_vaddr, new_vaddr);
#endif DEBUG
		regionrelease (rlp);

		rlp->rgl_type = rgl_type;
		rlp->rgl_prot = rgl_prot;
	}

	/*
	 ******	Já que a região não existia, cria uma nova ******
	 */
	if ((rgp = rlp->rgl_regiong) == NOREGIONG)
	{
		/*
		 *	Aloca uma entrada na tabela REGIONG
		 */
		if ((rlp->rgl_regiong = rgp = regiongget ()) == NOREGIONG)
			{ printf ("%g: Não consegui alocar entrada REGIONG\n"); return (-1); }

		/*
		 *	Aloca o espaço físico necessário
		 */
		if (rlp->rgl_type == RG_TEXT)	/* TEXT => fim da memória */
			rgp->rgg_paddr = malloce (M_CORE, new_size);
		else
			rgp->rgg_paddr = malloc (M_CORE, new_size);

		if (rgp->rgg_paddr == NOPG)
		{
			printf ("%g: Não consegui memória para região TEXT nova\n");
			regiongput (rgp); u.u_error = ENOMEM; return (-1);
		}

		if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
		{
			printf ("%g: Não consegui memória para as tabelas de região nova\n");
			mrelease (M_CORE, new_size, rgp->rgg_paddr);
			regiongput (rgp); u.u_error = ENOMEM; return (-1);
		}

		/*
		 *	Resumo das informações
		 */
	   /***	rlp->rgl_type		= (dado);			***/
	   /***	rlp->rgl_prot		= (dado);			***/
		rlp->rgl_vaddr		= new_vaddr;
	   /***	rlp->rgl_regiong	= (acima);			***/

	   /***	rgp->rgg_name		= (regiongget);			***/
		rgp->rgg_type		= rlp->rgl_type;
	   /***	rgp->rgg_flags		= (regiongget, zerado);		***/
	   /***	rgp->rgg_regionlock	= (regiongget, zerado);		***/
	   /***	rgp->rgg_pgtb_sz	= (mmu_page_table_alloc);	***/
		rgp->rgg_size		= new_size;
	   /*** rgp->rgg_paddr		= (acima);			***/
	   /*** rgp->rgg_pgtb		= (mmu_page_table_alloc;	***/
		rgp->rgg_count		= 1;
	   /*** rgp->rgg_next		= (regiongget, zerado);		***/

		/*
		 *	Epílogo
		 */
		if (opflags & RG_CLR)
			pgclr (rgp->rgg_paddr, new_size);

		if ((opflags & RG_NOMMU) == 0)
			mmu_dir_load (rlp);

		return (0);
	}

	/*
	 ******	A região já existia *****************************
	 *
	 *	Se for compartilhada, não posso modificar
	 */
	if (rgp->rgg_count != 1)
	{
		printf ("%g: Região tipo %d com %d usuários\n", rgp->rgg_type, rgp->rgg_count);
		return (-1);
	}

	if (rlp->rgl_type != rgp->rgg_type)
	{
		printf
		(	"%g: Tipo da região NÃO confere (%s :: %s)\n",
			region_nm_edit (rlp->rgl_type), region_nm_edit (rgp->rgg_type)
		);

		return (-1);
	}

	old_paddr = rgp->rgg_paddr;
	old_size  = rgp->rgg_size;

	/*
	 ******	A região mantém o tamanho ***********************
	 */
	if ((inc = new_size - old_size) == 0)
	{
		if (opflags & RG_CLR)
			pgclr (old_paddr, old_size);

		return (0);
	}

	/*
	 ******	A região diminui de tamanho *********************
	 */
	if (inc < 0)
	{
		if ((opflags & RG_NOMMU) == 0)
			mmu_dir_unload (rlp);

		if (rgp->rgg_type != RG_STACK)	/* Cresce para CIMA */
		{
			mrelease (M_CORE, -inc, old_paddr + new_size);
		}
		else				/* Cresce para BAIXO */
		{
			mrelease (M_CORE, -inc, old_paddr);

			rgp->rgg_paddr -= inc;
		}

	   /***	rgp->rgg_paddr = ...; ***/

		mmu_page_table_alloc (rlp, new_vaddr, new_size);

		rlp->rgl_vaddr = new_vaddr;
		rgp->rgg_size  = new_size;

		if ((opflags & RG_NOMMU) == 0)
			mmu_dir_load (rlp);

		if (opflags & RG_CLR)
			pgclr (rgp->rgg_paddr, new_size);

		return (0);
	}

	/*
	 ******	A região aumenta de tamanho no lugar ************ 
	 */
	if (rgp->rgg_type != RG_STACK)	/* NÃO-STACK: Cresce para CIMA */
	{
		/* Verifica se há memória disponivel logo depois da região */

		if (mallocp (M_CORE, inc, old_paddr + old_size) != NOPG)
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

		   /***	rgp->rgg_paddr = old_paddr; ***/

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, inc, old_paddr + old_size);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
				pgclr (old_paddr, new_size);
			else
				pgclr (old_paddr + old_size, inc);

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}

		/* Verifica se há memória disponível logo antes da região */

		if ((new_paddr = mallocp (M_CORE, inc, old_paddr - inc)) != NOPG)
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

			rgp->rgg_paddr = new_paddr;

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, inc, old_paddr - inc);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
			{
				pgclr (new_paddr, new_size);
			}
			else
			{
				pgcpy (new_paddr, old_paddr, old_size);
				pgclr (new_paddr + old_size, inc);
			}

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}
	}
	else				/* STACK: Cresce para BAIXO */
	{
		/* Verifica se há memória disponível logo antes da região */

		if ((new_paddr = mallocp (M_CORE, inc, old_paddr - inc)) != NOPG)
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

			rgp->rgg_paddr = new_paddr;

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, inc, old_paddr - inc);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
				pgclr (new_paddr, new_size);
			else
				pgclr (new_paddr, inc);

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}

		/* Verifica se há memória disponível logo depois da região */

		if (mallocp (M_CORE, inc, old_paddr + old_size) != NOPG)
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

		   /***	rgp->rgg_paddr = old_paddr; ***/

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, inc, old_paddr + old_size);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
			{
				pgclr (old_paddr, new_size);
			}
			else
			{
				pgcpy (old_paddr + inc, old_paddr, old_size);
				pgclr (old_paddr, inc);
			}

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}
	}

	/*
	 ******	A região aumenta de tamanho em outro lugar ****** 
	 */
	if ((new_paddr = malloc (M_CORE, new_size)) != NOPG)
	{
		if (rgp->rgg_type != RG_STACK)	/* NÃO-STACK: Cresce para CIMA */
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

			rgp->rgg_paddr = new_paddr;

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, new_size, new_paddr);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
			{
				pgclr (new_paddr, new_size);
			}
			else
			{
				pgcpy (new_paddr, old_paddr, old_size);
				pgclr (new_paddr + old_size, inc);
			}

			mrelease (M_CORE, old_size, old_paddr);

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}
		else				/* STACK: Cresce para BAIXO */
		{
			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_unload (rlp);

			rgp->rgg_paddr = new_paddr;

			if (mmu_page_table_alloc (rlp, new_vaddr, new_size) < 0)
			{
				printf ("%g: Não consegui memória para as tabelas de região nova\n");

				mrelease (M_CORE, new_size, new_paddr);
				{ u.u_error = ENOMEM; return (-1); }
			}

			rlp->rgl_vaddr = new_vaddr;
			rgp->rgg_size  = new_size;

			if (opflags & RG_CLR)
			{
				pgclr (new_paddr, new_size);
			}
			else
			{
				pgcpy (new_paddr + inc, old_paddr, old_size);
				pgclr (new_paddr, inc);
			}

			mrelease (M_CORE, old_size, old_paddr);

			if ((opflags & RG_NOMMU) == 0)
				mmu_dir_load (rlp);

			return (0);
		}
	}

	{ u.u_error = ENOMEM; return (-1); }

}	/* end regiongrow */

/*
 ****************************************************************
 *	Duplica física ou logicamente uma região		*
 ****************************************************************
 */
int
regiondup (const REGIONL *rlp_src, REGIONL *rlp_dst, int opflags)
{
	REGIONG		*rgp_src;

	/*
	 *	A região destino não deve existir ainda
	 */
	if (rlp_dst->rgl_regiong != NOREGIONG)
	{
		printf ("%g: Região %s destino já existente!\n", region_nm_edit (rlp_src->rgl_type));
		print_calls ();

		return (-1);
	}

	/*
	 *	Se a região fonte não existe, por definição, já está duplicada
	 */
	if ((rgp_src = rlp_src->rgl_regiong) == NOREGIONG)
		return (0);

	/*
	 *	Copia os campos comuns
	 */
	rlp_dst->rgl_type  = rlp_src->rgl_type;
	rlp_dst->rgl_prot  = rlp_src->rgl_prot;
	rlp_dst->rgl_vaddr = rlp_src->rgl_vaddr;

	/*
	 *	Verifica se duplica logicamente ou fisicamente
	 */
	if (opflags & RG_LOGDUP)
	{
#ifdef	MSG
		if (CSWT (25))
			printf ("%g: Duplicando logicamente rlp = %P, rgp = %P\n", u.u_pid, rlp_src, rgp_src);
#endif	MSG
		/*
		 *	Duplica logicamente uma região
		 */
		rlp_dst->rgl_regiong = rgp_src;

		SPINLOCK (&rgp_src->rgg_regionlock);
		rgp_src->rgg_count++;
		SPINFREE (&rgp_src->rgg_regionlock);
	}
	else
	{
#ifdef	MSG
		if (CSWT (25))
			printf ("%g: Duplicando fisicamente rlp = %P, rgp = %P\n", u.u_pid, rlp_src, rgp_src);
#endif	MSG
		/*
		 *	Duplica fisicamente uma região
		 */
		if (regiongrow (rlp_dst, 0, rgp_src->rgg_size, RG_NOMMU) < 0)
			{ printf ("%g: Região não pode ser duplicada\n"); return (-1); }

		pgcpy (rlp_dst->rgl_regiong->rgg_paddr, rgp_src->rgg_paddr, rgp_src->rgg_size);
	}

	return (0);

}	/* end regiondup */

/*
 ****************************************************************
 *	Libera uma região alocada mas não usada			*
 ****************************************************************
 */
void
regionrelease (REGIONL *rlp)
{
	REGIONG		*rgp;

	if ((rgp = rlp->rgl_regiong) == NOREGIONG)
		return;

	mmu_dir_unload (rlp);

	regiongrelease (rgp, 0);

	memclr (rlp, sizeof (REGIONL));

}	/* end regionrelease */

/*
 ****************************************************************
 *	Libera uma região REGIONG				*
 ****************************************************************
 */
int
regiongrelease (REGIONG *rgp, int untext)
{
	/*
	 *	Decrementa o contador (exceto "untext")
	 */
	SPINLOCK (&rgp->rgg_regionlock);

	if (!untext)
		rgp->rgg_count--;

	if (rgp->rgg_count > 0)
	{
		SPINFREE (&rgp->rgg_regionlock);
		return (1);
	}

	SPINFREE (&rgp->rgg_regionlock);

	/*
	 *	Verifica se é para manter na memória
	 */
	if (rgp->rgg_flags & RGG_METX)
		return (1);

	/*
	 *	Não é para manter na memória: libera a memória alocada
	 */
	if (rgp->rgg_pgtb != NOPG)
		mrelease (M_CORE, rgp->rgg_pgtb_sz, rgp->rgg_pgtb);

	if (rgp->rgg_type != RG_PHYS)
		mrelease (M_CORE, rgp->rgg_size, rgp->rgg_paddr);

	regiongput (rgp);

	return (0);

}	/* end regiongrelease */

/*
 ****************************************************************
 *	Obtém uma entrada na tabela REGIONG			*
 ****************************************************************
 */
REGIONG *
regiongget (void)
{
	REGIONG		*rgp;

	SPINLOCK (&regionglock);

	if ((rgp = regiongfreelist) == NOREGIONG)
		{ SPINFREE (&regionglock); return (NOREGIONG); }

	regiongfreelist = rgp->rgg_next; rgp->rgg_next = NOREGIONG;

	regiong_list_count++;

	SPINFREE (&regionglock);

	strcpy (rgp->rgg_name, u.u_pgname);

	return (rgp);

}	/* end regiongget */

/*
 ****************************************************************
 *	Libera uma estrutura REGIONG para a regiongfreelist	*
 ****************************************************************
 */
void
regiongput (REGIONG *rgp)
{
	memclr (rgp, sizeof (REGIONG));

	SPINLOCK (&regionglock);

	rgp->rgg_next = regiongfreelist; regiongfreelist = rgp;

	regiong_list_count--;

	SPINFREE (&regionglock);

}	/* end regiongput */

/*
 ****************************************************************
 *	Inicializa a lista de REGIONG				*
 ****************************************************************
 */
void
regionginit (void)
{
	REGIONG		*rp, *np;

   /***	SPINLOCK (&regionglock); ***/

	np = NOREGIONG;

	for (rp = scb.y_endregiong - 1; rp >= scb.y_regiong; rp--)
		{ rp->rgg_next = np; np = rp; }

	regiongfreelist = np;
   /***	regiong_list_count = 0; ***/

   /***	SPINFREE (&regionglock); ***/

}	/* end regionginit */

/*
 ****************************************************************
 *	Edita o nome de uma região				*
 ****************************************************************
 */
const char	region_nm_vec[][12] = RG_NAMES;

const char *
region_nm_edit (int region_type)
{
	if (region_type < RG_NONE || region_type > RG_SHMEM)
		return ("???");
	else
		return (region_nm_vec[region_type]);

}	/* end mmu_region_nm_edit */
