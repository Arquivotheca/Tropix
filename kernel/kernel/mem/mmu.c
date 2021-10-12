/*
 ****************************************************************
 *								*
 *			mmu.c					*
 *								*
 *	Funções relativas à gerência de memória			*
 *								*
 *	Versão	3.0.0, de 27.11.94				*
 *		4.6.0, de 04.08.04				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/map.h"
#include "../h/cpu.h"
#include "../h/mmu.h"
#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry char	mmu_dirty[NCPU];	/* Indica que deve recarregar a MMU */

/*
 ****************************************************************
 *	Carrega toda a MMU para o processo			*
 ****************************************************************
 */
void
mmu_load (void)
{
	int			i;
	const KFILE		*fp;
	const REGIONL		*rlp, *end_rlp;

	/*
	 *	Carrega a MMU para o TEXT, DATA e STACK
	 */
	mmu_dir_load (&u.u_text);
	mmu_dir_load (&u.u_data);
	mmu_dir_load (&u.u_stack);

	/*
	 *	Carrega a MMU para as bibliotecas compartilhadas
	 */
	for (rlp = &u.u_shlib_text[0], end_rlp = rlp + NSHLIB; rlp < end_rlp; rlp++)
		mmu_dir_load (rlp);

	for (rlp = &u.u_shlib_data[0], end_rlp = rlp + NSHLIB; rlp < end_rlp; rlp++)
		mmu_dir_load (rlp);

	/*
	 *	Aloca os registros para o PHYS
	 */
	if (u.u_phn > 0) for (rlp = &u.u_phys_region[0], end_rlp = rlp + PHYSNO; rlp < end_rlp; rlp++)
		mmu_dir_load (rlp);

	/*
	 *	Aloca os registros para as regiões de memória compartilhada
	 */
	if (u.u_shmemnp > 0) for (i = 0; i < NUFILE; i++)
	{
		if ((fp = u.u_ofile[i]) == NOKFILE)
			continue;

		if (fp->f_union != KF_SHMEM)
			continue;

		mmu_dir_load (&fp->f_shmem_region);

	}	/* end for files */

}	/* end mmu_load */

/*
 ****************************************************************
 *	Testa a consistência dos parâmetros para a MMU		*
 ****************************************************************
 */
int
mmutst (pg_t nt, pg_t nd, pg_t ns)
{
	if (nt + nd + ns + USIZE > scb.y_maxpsz)
		{ u.u_error = ENOMEM; return (-1); }

	return (0);

}	/* end mmutst */

/*
 ****************************************************************
 *  Endereço virtual (páginas) => Entrada na tabela de páginas	*
 ****************************************************************
 */
mmu_t *
vaddr_to_page_table_entry (pg_t vaddr, int size)
{
	mmu_t	dir_entry;
	int	i;

	/*
	 *	Em primeiro lugar, obtém a entrada do diretório de páginas
	 */
	dir_entry = u.u_mmu_dir[(unsigned)vaddr >> (PGSHIFT-MMUSHIFT)];

	if ((dir_entry & 1) == 0)
	{
		printf ("%g: Entrada do diretório vazia, end = %P\n", vaddr);
		print_calls (); u.u_error = EINVAL; return (NOMMU);
	}

	/*
	 *	Em seguida, obtém o deslocamento dentro da tabela de páginas
	 */
	i = vaddr & ((unsigned)PGMASK >> MMUSHIFT);

	/*
	 *	Confere o tamanho
	 */
	if (i + size > (PGSZ >> MMUSHIFT))
	{
		printf ("%g: Tamanho excessivo: end = %P, size = %d\n", vaddr, size);
		print_calls (); u.u_error = EINVAL; return (NOMMU);
	}

	/*
	 *	Retorna o endereço da entrada da tabela de páginas
	 */
	return ((mmu_t *)((dir_entry & ~PGMASK) + SYS_ADDR) + i);

}	/* end vaddr_to_page_table_entry */

/*
 ****************************************************************
 *	Converte um endereço virtual de usuário em físico	*
 ****************************************************************
 */
void *
user_area_to_phys_area (void *uaddr, int count)
{
	ulong		ph_area;
	pg_t		tst_begin, tst_end;
	REGIONG		*rgp;
	mmu_t		*mmup;
	const char	*str;
	int		index;

	/*
	 *	Calcula o endereço físico
	 */
	mmup = vaddr_to_page_table_entry ((unsigned)uaddr >> PGSHIFT, 1 /* pg */);

	if (mmup == NOMMU)
		{ str = "Endereço virtual inexistente"; goto bad; }

	if (((ph_area = *mmup) & 0x01) == 0)
		{ str = "Endereço virtual não alocado"; goto bad; }

	ph_area &= ~PGMASK;	/* Tira os bits baixos da entrada da MMU */

	ph_area |= ((unsigned)uaddr & PGMASK); /* Coloca os bits baixos do endereço */

	/*
	 *	Calcula os limites em páginas (endereço do KERNEL!)
	 */
	tst_begin = BYTOPG (PHYS_TO_VIRT_ADDR (ph_area));
	tst_end   = BYUPPG (PHYS_TO_VIRT_ADDR (ph_area) + count);

	/*
	 *	Verifica se a operação está dentro da região DATA do usuário
	 *	Todas as contas são feitas em páginas
	 */
	rgp = u.u_data.rgl_regiong;

	if (tst_begin >= rgp->rgg_paddr && tst_end <= rgp->rgg_paddr + rgp->rgg_size)
		return ((void *)ph_area);

	/*
	 *	Verifica se a operação está dentro da região STACK do usuário
	 */
	rgp = u.u_stack.rgl_regiong;

	if (tst_begin >= rgp->rgg_paddr && tst_end <= rgp->rgg_paddr + rgp->rgg_size)
		return ((void *)ph_area);

	/*
	 *	Verifica se a operação está em uma das regiões das bibliotecas compartilhadas
	 */
	for (index = 0; /* abaixo */; index++)
	{
		if (index >= NSHLIB)
			{ str = "Área NÃO faz parte do processo"; break; }

		if ((rgp = u.u_shlib_data[index].rgl_regiong) == NOREGIONG)
			continue;

		if (tst_begin >= rgp->rgg_paddr && tst_end <= rgp->rgg_paddr + rgp->rgg_size)
			return ((void *)ph_area);
	}

	/*
	 *	Não achou
	 */
    bad:
	printf ("%g: %s (area = %P, count = %d)\n", str, uaddr, count);

	return (0);

}	/* end user_area_to_phys_area */

/*
 ****************************************************************
 *	Carrega o diretório de PGs para uma dada região		*
 ****************************************************************
 */
void
mmu_dir_load (const REGIONL *rlp)
{
	REGIONG		*rgp;
	mmu_t		*mmup;
	unsigned int	paddr;
	int		i;

	if ((rgp = rlp->rgl_regiong) == NOREGIONG)
		return;

	mmup = u.u_mmu_dir + ((unsigned)rlp->rgl_vaddr >> (PGSHIFT-MMUSHIFT));

	paddr = ((rgp->rgg_pgtb << PGSHIFT) - SYS_ADDR) | rlp->rgl_prot;

	for (i = rgp->rgg_pgtb_sz; i > 0; i--)
	{
#ifdef	MSG
		if (CSWT (34))
			printf ("%g: %P => %P\n", mmup, paddr);
#endif	MSG
		*mmup++ = paddr; paddr += PGSZ;
	}

	if (scb.y_cputype < 486)
	{
		mmu_dirty[CPUID] = 1;
	}
	else
	{
		inval_TLB (rlp->rgl_vaddr, rlp->rgl_regiong->rgg_size);

		if (CSWT (34))
		{
			printf
			(	"%g: Invalidando %s: %P (%d páginas)\n",
				region_nm_edit (rlp->rgl_type),
				rlp->rgl_vaddr, rlp->rgl_regiong->rgg_size
			);
		}
	}

}	/* end mmu_dir_load */

/*
 ****************************************************************
 *	Zera o diretório de PGs para uma dada região		*
 ****************************************************************
 */
void
mmu_dir_unload (const REGIONL *rlp)
{
	REGIONG		*rgp;
	mmu_t		*mmup;
	int		i;

	if ((rgp = rlp->rgl_regiong) == NOREGIONG)
		return;

	mmup = u.u_mmu_dir + ((unsigned)rlp->rgl_vaddr >> (PGSHIFT-MMUSHIFT));

	for (i = rgp->rgg_pgtb_sz; i > 0; i--)
	{
#ifdef	MSG
		if (CSWT (34))
			printf ("%g: %P => 0\n", mmup);
#endif	MSG
		*mmup++ = 0;
	}

	if (scb.y_cputype < 486)
		mmu_dirty[CPUID] = 1;
	else
		inval_TLB (rlp->rgl_vaddr, rlp->rgl_regiong->rgg_size);

}	/* end mmu_dir_unload */

/*
 ****************************************************************
 *	Aloca a tabela de páginas para uma região		*
 ****************************************************************
 */
int
mmu_page_table_alloc (REGIONL *rlp, pg_t new_vaddr, pg_t new_size)
{
	REGIONG		*rgp = rlp->rgl_regiong;
	pg_t		begin_addr, end_addr;
	pg_t		new_pgtb_size;
	mmu_t		*mmup;
	unsigned int	paddr;
	int		i;

	/*
	 *	Analisa o novo tamanho
	 */
	if (new_size <= 0)
		printf ("%g: Região %s com Tamanho NULO?\n", region_nm_edit (rlp->rgl_type));

	/*
	 *	Se o novo endereço virtual não é dado, não muda
	 */
	if (new_vaddr == 0)
		new_vaddr = rlp->rgl_vaddr;

	/*
	 *	Calcula o novo tamanho
	 */
	begin_addr =  new_vaddr & ~(PGSZ/MMUSZ - 1);
	end_addr   = (new_vaddr + new_size + PGSZ/MMUSZ - 1) & ~(PGSZ/MMUSZ - 1);

	new_pgtb_size = (end_addr - begin_addr) >> (PGSHIFT-MMUSHIFT);

	/*
	 *	Verifica se mudou o tamanho
	 */
	if (new_pgtb_size == rgp->rgg_pgtb_sz)
	{
		if (new_pgtb_size == 0)
			return (0);

		/* Repare que neste caso é necessário continuar pois */
		/* possivelmente mudou o "paddr" */
	}
	else 	/* new_pgtb_size != rgp->rgg_pgtb_sz */
	{
		if (rgp->rgg_pgtb_sz > 0)
		{
			mmu_dir_unload (rlp);

			mrelease (M_CORE, rgp->rgg_pgtb_sz, rgp->rgg_pgtb);
			rgp->rgg_pgtb = NOPG; rgp->rgg_pgtb_sz = 0;
		}

		if (new_pgtb_size == 0)
			return (0);

		if ((rgp->rgg_pgtb = malloc (M_CORE, new_pgtb_size)) == NOPG)
			return (-1);

		rgp->rgg_pgtb_sz = new_pgtb_size;
#ifdef	MSG
		if (CSWT (34))
		{
			printf
			(	"%g: %s: aloquei (%d, %P)\n",
				region_nm_edit (rlp->rgl_type), new_pgtb_size, rgp->rgg_pgtb
			);
		}
#endif	MSG
	}

	/*
	 *	Prepara os ponteiros para as páginas efetivas da região
	 */
	pgclr (rgp->rgg_pgtb, new_pgtb_size);

	mmup  = (mmu_t *)PGTOBY (rgp->rgg_pgtb) + (new_vaddr - begin_addr);

	paddr = ((rgp->rgg_paddr << PGSHIFT) - SYS_ADDR) | URW;

#ifdef	MSG
	if (CSWT (34))
	{
		printf ("%g: %s: %P => %P (de %d)\n", region_nm_edit (rlp->rgl_type), mmup, paddr, new_size);
	}
#endif	MSG

	for (i = new_size; i > 0; i--, paddr += PGSZ)
		*mmup++ = paddr;

#ifdef	MSG
	if (new_size > 1 && CSWT (34))
	{
		printf ("%g: ..................\n");
		printf ("%g: %s: %P => %P (de %d)\n", region_nm_edit (rlp->rgl_type), mmup - 1, paddr - PGSZ, new_size);
	}
#endif	MSG

#if (0)	/*******************************************************/
	mmu_dirty[CPUID] = 1;
#endif	/*******************************************************/

	return (0);

}	/* end mmu_page_table_alloc */

/*
 ****************************************************************
 *	Mapeia endereços físicos				*
 ****************************************************************
 */
ulong
mmu_map_phys_addr (ulong paddr, int size)
{
	mmu_t		*dp;
	pg_t		vaddr;
	int		offset, i;

#undef	DEBUG
#ifdef	DEBUG
	printf ("mmu_map_phys_addr: paddr = %P, size = %d\n", paddr, size);
#endif	DEBUG

	/*
	 *	Analisa o deslocamento e tamanho (tamanho máximo = 4 KB)
	 */
	if ((offset = (paddr & PGMASK)) + size > PGSZ)
	{
		printf ("mmu_map_phys_addr: paddr = %P, size = %d\n", paddr, size);
		u.u_error = EINVAL; return (NULL);
	}

	/*
	 *	Procura uma entrada na tabela de páginas
	 */
	for (i = 0, dp = extra_page_table; /* abaixo */; i++, dp++)
	{
		if (i >= (1 << (PGSHIFT-MMUSHIFT)))
			{ u.u_error = EAGAIN; return (NULL); }

		if (*dp == 0)
			break;
	}

	/*
	 *	Prepara a entrada da tabela de páginas (por enquanto apenas até 4 KB)
	 */
	*dp = (paddr & ~PGMASK) | SRW;

	/*
	 *	Marca a página como "suja"
	 */
	vaddr = (UPROC_ADDR >> PGSHIFT) + i;

	if (scb.y_cputype < 486)
		mmu_dirty[CPUID] = 1;
	else
		inval_TLB (vaddr, 1);

#ifdef	DEBUG
	printf ("mmu_map_phys_addr: vaddr = %P\n", (vaddr << PGSHIFT) | offset);
#endif	DEBUG

	return ((vaddr << PGSHIFT) | offset);

}	/* end mmu_map_phys_addr */
