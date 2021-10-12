/*
 ****************************************************************
 *								*
 *			prregion.c				*
 *								*
 *	Imprime as regiões dos processos			*
 *								*
 *	Versão	3.1.0, de 10.09.98				*
 *		4.6.0, de 07.08.04				*
 *								*
 *	Modulo: 						*
 *		Utilitário Especial				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/region.h>

#include <sys/syscall.h>
#include <sys/kcntl.h>
#include <sys/inode.h>

#include <sys/bhead.h>
#include <sys/mmu.h>
#include <sys/signal.h>
#include <sys/uerror.h>
#include <sys/uarg.h>
#include <sys/uproc.h>
#include <sys/kfile.h>
#include <sys/tty.h>
#include <sys/itnet.h>
#include <sys/shlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <a.out.h>
#include <xti.h>

/*
 ****************************************************************
 *	Definições						*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 07.08.04";

#define	SSEG	8192		/* Tamanho do trecho mapeado da stack */

#define	NPID	20

#define PG_SYS_ADDR	((unsigned)SYS_ADDR >> PGSHIFT)
#define ADDRTOKB(x)	(x == 0 ? 0 : (((unsigned)(x) - PG_SYS_ADDR) << (PGSHIFT-KBSHIFT)))

/*
 ****************************************************************
 *	Variáveis e áreas globais				*
 ****************************************************************
 */
entry SCB	scb;

entry int	lflag;			/* Lista Longa de Informações */
entry int	vflag;			/* Verboso */

entry REGIONL	text_rgl, data_rgl, stack_rgl;
entry REGIONG	text_rgg, data_rgg, stack_rgg;

entry REGIONL	phys_rgl[PHYSNO];
entry REGIONG	phys_rgg[PHYSNO];

entry REGIONG	shlib_text_rgg[NSHLIB];
entry REGIONG	shlib_data_rgg[NSHLIB];

entry REGIONG	empty_rgg;

entry char	*region_global_busy_vector;

const char *region_type_nm[] =
{
	"NULO",		/* Região NULA */
	"TEXT",		/* Região de texto compartilhado */
	"DATA",		/* Região de data (compart. entre threads) */
	"STACK",	/* Região de stack nunca compartilhada */
	"PHYS",		/* Região de PHYS */
	"SHMEM"		/* Região de memória compartilhada */
};

/*
 ******	Protótipos **********************************************
 */
void		analyse_texts (void);
void		analyse_shlib (void);
const UPROC	*setuser (UPROC const *kup, UARG *uap);
void		print_process (const UPROC *up, const UARG *uap);
void		print_args (const UPROC *up, const UARG *uap);
void		print_local_region (const REGIONL *);
void		print_global_region (const REGIONG *, pg_t vaddr);
void		add_regiong_to_vector (const REGIONG *rgp);
const char	*plural (int);
void		help (void);

/*
 ****************************************************************
 *	Imprime as regiões dos processos			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	SCB		*sp;
	int		i, n, len;
	SYM		*sym_p;
	long		pid;
	const char	*str;
	const UPROC	*up;
	const UPROCV	*uvp, *last_procv;
	int		procv_delta;
	const REGIONG	*rgp;
	int		*int_ptr;
	int		pid_cnt = 0;
	UARG		uarg;
	long		pid_vec[NPID];
	int		regiong_list_count;

	/*
	 *	Verifica o usuário efetivo
	 */
	if (geteuid () != 0)
		error ("$Não sou o SUPERUSUÁRIO!");

	/*
	 *	Analisa a linha de argumentos do programa
	 */
	while ((opt = getopt (argc, argv, "lvH")) != -1)
	{
		switch (opt)
		{
		    case 'l':
			lflag++;
			break;

		    case 'v':
			vflag++;
			break;

		    case 'H':
			help ();

		    default:
			putchar ('\n');
			help ();
		}
	}

	/*
	 *	Guarda os números dos processos a listar
	 */
	for (argv += optind; *argv; argv++)
	{
		if ((pid = strtol (*argv, &str, 0)) < 0 || *str != '\0')
		{
			error ("<pid> de processo inválido: \"%s\"", *argv);
			continue;
		}

		if (pid_cnt < NPID)
			pid_vec[pid_cnt++] = pid;
		else
			error ("Número excessivo de <pid>s de processos");

	}	/* end for */

	/*
	 *	Obtem o "System Control Block"
	 */
	if ((int)getscb (sp = &scb) == -1)
		error ("$*Não consegui obter o SCB");

	/*
	 *	Conta o número de REGIONGs ocupadas
	 */
	len = strlen ("_regiong_list_count"); sym_p = alloca (SYM_SIZEOF (len));

	sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_regiong_list_count");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_regiong_list_count\"");

	if ((int)(int_ptr = phys ((void *)sym_p->s_value, sizeof (int), O_KERNEL|O_RDONLY)) == -1)
	    	error ("$*Não consegui mapear \"regiong_list_count\"");

	regiong_list_count = *int_ptr;

	phys (int_ptr, 0, 0);

	if (vflag)
		printf ("regiong_list_count = %u\n", regiong_list_count);

	/*
	 *	Aloca um vetor de entradas REGIONG ocupadas
	 */
	if ((region_global_busy_vector = malloc (sp->y_nregiong)) == NOSTR)
	    	error ("$Não consegui alocar o mapa de REGIONGs ocupadas");

	memset (region_global_busy_vector, 0, sp->y_nregiong);

	if (vflag)
		printf ("Aloquei um vetor para %u REGIONGs\n", sp->y_nregiong);

	/*
	 *	Analisa os textos
	 */
	analyse_texts ();

	/*
	 *	Analisa as bibliotecas compartilhadas
	 */
	analyse_shlib ();

	/*
	 *	Mapeia a Tabela de Processos
	 */
	if ((int)(uvp = phys (sp->y_uproc, sp->y_nproc * sizeof (UPROCV), O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear a tabela de processos");

	procv_delta = (int)uvp - (int)sp->y_uproc;

	last_procv  = (UPROCV *)((int)sp->y_lastuproc + procv_delta);

	/*
	 *	Malha Principal pela Tabela de Processos
	 */
	for (/* acima */; uvp < last_procv; uvp++)
	{ 
		/*
		 *	Mapeia UPROC & PILHA
		 */
		if (uvp->u_uproc == NOUPROC)
			continue;

		if ((up = setuser (uvp->u_uproc, &uarg)) == NOUPROC)
			continue;

		/*
		 *	Examina o estado
		 */
		if (up->u_state == SNULL)
			continue;

		/*
		 *	Verifica se foram dados <pid>s
		 */
		if (pid_cnt > 0)
		{
			const long	*pid_ptr, *end_pid_vec;

			pid_ptr = pid_vec; end_pid_vec = pid_ptr + pid_cnt;

			for (/* acima */; pid_ptr < end_pid_vec; pid_ptr++)
			{
				if (*pid_ptr == up->u_pid)
					goto found;
			}

			continue;
		}

		/*
		 *	Imprime o Processo
		 */
	    found:
		print_process (up, &uarg);
	}

	/*
	 *	Imprime o total de REGIONGs ocupados
	 */
	for (n = i = 0; i < sp->y_nregiong; i++)
	{
		if (region_global_busy_vector[i])
			n++;
	}

	printf
	(	"\nREGIONGs: kernel = %u, vetor = %u\n",
		regiong_list_count, n
	);

	if (n == regiong_list_count)
		exit (0);

	/*
	 *	Descobre quais REGIONGs estão sobrando
	 */
	if (pid_cnt > 0)
		exit (0);

	printf ("\n\nTabela de REGIONGs sobrando:\n\n");

	if ((int)(rgp = phys (sp->y_regiong, sp->y_nregiong * sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
	    	error ("$*Não consegui mapear a tabela de REGIONGs");

	for (i = 0; i < sp->y_nregiong; rgp++, i++)
	{
		if (region_global_busy_vector[i])
			continue;

		if (!memcmp (rgp, &empty_rgg, sizeof (REGIONG) - sizeof (rgp->rgg_next)))
			continue;

		printf ("\n\t%u (%s): ", i, region_type_nm[rgp->rgg_type]);
		print_global_region (rgp, 0); printf ("\n");
	}

   /***	phys (rgp, 0, 0); ***/

	return (0);

}	/* end prregion */

/*
 ****************************************************************
 *	Examina os INODEs, procurando TEXTs			*
 ****************************************************************
 */
void
analyse_texts (void)
{
	const SCB	*sp = &scb;
	const INODE	*ip;
	int		i;
	const REGIONG	*rgp;

	/*
	 *	Mapeia os INODEs
	 */
	printf ("\n************** Textos ***************\n\n");

	if ((int)(ip = phys (sp->y_inode, sp->y_ninode * sizeof (INODE), O_KERNEL|O_READ)) == -1)
	    	error ("$*Não consegui mapear os INODEs");

	for (i = sp->y_ninode;  i > 0;  i--, ip++)
	{
		if ((ip->i_flags & ITEXT) == 0)
			continue;

		if (ip->i_xregiong == NOREGIONG)
			continue;

		if ((int)(rgp = phys (ip->i_xregiong, sizeof (REGIONG), O_KERNEL|O_READ)) == -1)
		    	error ("$*Não consegui mapear uma REGIONG de um INODE");

		printf ("\n\t"); print_global_region (rgp, 0);

		add_regiong_to_vector (ip->i_xregiong);

		phys (rgp, 0, 0);
	}

	phys (ip, 0, 0);

} 	/* analyse_texts */

/*
 ****************************************************************
 *	Examina as bibliotecas compartilhadas			*
 ****************************************************************
 */
void
analyse_shlib (void)
{
	int		i, len;
	const REGIONG	*rgp;
	const SHLIB	*shp;
	SYM		*sp;

	/*
	 *	Obtém o endereço da tabela
	 */
	len = strlen ("_shlib_tb"); sp = alloca (SYM_SIZEOF (len));

	sp->s_nm_len = len; strcpy (sp->s_name, "_shlib_tb");

	if (kcntl (K_GETSYMGN, len, sp) < 0)
		error ("$*Não consegui obter o endereço de \"_shlib_tb\"");

	if ((int)(shp = phys ((void *)sp->s_value, NSHLIB * sizeof (SHLIB), O_KERNEL|O_RDONLY)) == -1)
	    	error ("$*Não consegui mapear \"shlib_tb\"");

	/*
	 *	Imprime as tabelas
	 */
	printf ("\n************** Bibliotecas compartilhadas ***************\n\n");

	for (i = NSHLIB;  i > 0;  i--, shp++)
	{
		if (shp->sh_region == NOREGIONG)
			continue;

		if ((int)(rgp = phys (shp->sh_region, sizeof (REGIONG), O_KERNEL|O_READ)) == -1)
		    	error ("$*Não consegui mapear uma REGIONG de uma SHLIB");

		printf ("\n\t"); print_global_region (rgp, 0);

		add_regiong_to_vector (shp->sh_region);

		phys (rgp, 0, 0);
	}

	phys (shp, 0, 0);

} 	/* analyse_shlib */

/*
 ****************************************************************
 *	Prepara Ponteiros para UPROC e PILHA			*
 ****************************************************************
 */
const UPROC *
setuser (UPROC const *kup, UARG *uap)
{
	const UARG		*uapf;
	int			i, stack_delta;
	const UPROC		*up;
	void			*phys_addr;
	char			*sp;
	static const UPROC	*mapup;
	static const char	*mapsp;
	REGIONG			*rgp;
	const REGIONL		*rlp;

	/*
	 *	Mapeia a UPROC
	 */
	if (mapup)
		phys (mapup, 0, 0);

	if (mapsp)
		phys (mapsp, 0, 0);

	if ((int)(mapup = up = phys (kup, UPROCSZ, O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a UPROC"); mapup = NOUPROC; return (NOUPROC); }

	/*
	 *	Verifica se o processo possui PILHA
	 */
	if (up->u_flags & SSYS)
		{ memsetl (uap, 0, sizeof (UARG) / sizeof (long)); return (up); }

	/*
	 *	Zera todas as REGIONLs e REGIONGs
	 */
	memset (&text_rgl,  0, sizeof (REGIONL));
	memset (&data_rgl,  0, sizeof (REGIONL));
	memset (&stack_rgl, 0, sizeof (REGIONL));

	memset (&text_rgg,  0, sizeof (REGIONG));
	memset (&data_rgg,  0, sizeof (REGIONG));
	memset (&stack_rgg, 0, sizeof (REGIONG));

	memset (&phys_rgg, 0, PHYSNO * sizeof (REGIONG));

	/*
	 *	Copia as REGIONLs principais
	 */
	memmove (&text_rgl,  &up->u_text,  sizeof (REGIONL));
	memmove (&data_rgl,  &up->u_data,  sizeof (REGIONL));
	memmove (&stack_rgl, &up->u_stack, sizeof (REGIONL));

	/*
	 *	Mapeia a REGIONG do TEXT
	 */
	if (text_rgl.rgl_regiong)
	{
		if ((int)(rgp = phys (text_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT");

		memmove (&text_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	text_rgl.rgl_regiong = &text_rgg; ***/
	}

	/*
	 *	Mapeia a REGIONG do DATA
	 */
	if (data_rgl.rgl_regiong)
	{
		if ((int)(rgp = phys (data_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do DATA");

		memmove (&data_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	data_rgl.rgl_regiong = &data_rgg; ***/
	}

	/*
	 *	Mapeia a REGIONG da STACK
	 */
	if (stack_rgl.rgl_regiong)
	{
		if ((int)(rgp = phys (stack_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do STACK");

		memmove (&stack_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	stack_rgl.rgl_regiong = &stack_rgg; ***/
	}

	/*
	 *	Copia as REGIONLs do PHYS
	 */
	memmove (phys_rgl, up->u_phys_region, PHYSNO * sizeof (REGIONL));

	/*
	 *	Mapeia as REGIONGs do PHYS
	 */
	for (i = 0, rlp = &phys_rgl[0]; rlp < &phys_rgl[PHYSNO]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		if ((int)(rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do PHYS");

		memmove (&phys_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &phys_rgg[i]; ***/
	}

	/*
	 *	Mapeia as REGIONGs do SHMEM
	 */
	for (i = 0, rlp = &up->u_shlib_text[0]; rlp < &up->u_shlib_text[NSHLIB]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		if ((int)(rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT da SHLIB");

		memmove (&shlib_text_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &shlib_text_rgg[i]; ***/
	}

	for (i = 0, rlp = &up->u_shlib_data[0]; rlp < &up->u_shlib_data[NSHLIB]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		if ((int)(rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT da SHLIB");

		memmove (&shlib_data_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &shlib_data_rgg[i]; ***/
	}

	/*
	 *	Mapeia e desmapeia a REGIONG da PILHA
	 */
	if ((int)(rgp = phys (up->u_stack.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a REGIONG"); return (NOUPROC); }

	phys_addr = (void *)PGTOBY (rgp->rgg_paddr + rgp->rgg_size);

	phys (rgp, 0, 0);

	/*
	 *	Mapeia a PILHA do processo
	 */
	if ((int)(mapsp = sp = phys (phys_addr - SSEG, SSEG, O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a PILHA"); mapsp = NOSTR; return (NOUPROC); }

	/*
	 *	Prepara os diversos ponteiros da PILHA
	 */
	stack_delta = (int)(USER_STACK_PGA << PGSHIFT) - (int)sp - SSEG;

	uapf = (UARG *)((USER_STACK_PGA << PGSHIFT) - sizeof (UARG) - stack_delta);

	uap->g_envp = uapf->g_envp - stack_delta;
	uap->g_envc = uapf->g_envc;

	if (uap->g_envp < sp || uap->g_envp >= sp + SSEG)
		{ uap->g_envp = NOSTR; uap->g_envc = 0; }

	uap->g_argp = uapf->g_argp - stack_delta;
	uap->g_argc = uapf->g_argc;

	if (uap->g_argp < sp || uap->g_argp >= sp + SSEG)
		{ uap->g_argp = NOSTR; uap->g_argc = 0; }

	return (up);

}	/* end setuser */

/*
 ****************************************************************
 *	Imprime o Processo					*
 ****************************************************************
 */
void
print_process (const UPROC *up, const UARG *uap)
{
	const REGIONL	*rlp;
	int		i;

	/*
	 *	Imprime os argumentos do processo
	 */
	printf ("\n************** Processo %u:\t", up->u_pid);

	print_args (up, uap); printf ("\n\n");

	/*
	 *	Imprime a região TEXT
	 */
	if (text_rgl.rgl_regiong)
	{
		printf ("\t"); 	print_local_region (&text_rgl); printf ("\n");
		printf ("\t"); 	print_global_region (&text_rgg, text_rgl.rgl_vaddr); printf ("\n");
	}

	if (data_rgl.rgl_regiong)
	{
		printf ("\t"); 	print_local_region (&data_rgl); printf ("\n");
		printf ("\t"); 	print_global_region (&data_rgg, data_rgl.rgl_vaddr); printf ("\n");
	}

	if (stack_rgl.rgl_regiong)
	{
		printf ("\t"); 	print_local_region (&stack_rgl); printf ("\n");
		printf ("\t"); 	print_global_region (&stack_rgg, stack_rgl.rgl_vaddr); printf ("\n");
	}

	/*
	 *	Examina os PHYSs
	 */
	for (i = 0, rlp = &phys_rgl[0]; /* abaixo */; i++, rlp++)
	{
		if (rlp >= &phys_rgl[PHYSNO])
			break;

		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		printf ("\t");
		print_local_region (rlp);
		printf ("\n\t");
		print_global_region (&phys_rgg[i], rlp->rgl_vaddr);
		printf ("\n");
	}

	/*
	 *	Examina as SHLIBs
	 */
	for (i = 0; i < NSHLIB; i++)
	{
		rlp = &up->u_shlib_text[i];

		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		printf ("\tSHLIB(%d) ", i);
		print_local_region (rlp);
		printf ("\n\t");
		print_global_region (&shlib_text_rgg[i], rlp->rgl_vaddr);
		printf ("\n");

		rlp = &up->u_shlib_data[i];

		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		printf ("\tSHLIB(%d) ", i);
		print_local_region (rlp);
		printf ("\n\t");
		print_global_region (&shlib_data_rgg[i], rlp->rgl_vaddr);
		printf ("\n");
	}

	printf ("\n");

}	/* end print_process */

/*
 ****************************************************************
 *	Imprime os argumentos do processo			*
 ****************************************************************
 */
void
print_args (const UPROC *up, const UARG *uap)
{
	int		argc, c;
	const char	*cp;

	/*
	 *	Trata o Swapper, dispatchers e zombies
	 */
	if (up->u_flags & SSYS)
	{
		if (up->u_pid == 0)
			printf ("<Swapper/Pager>");
		else
			printf ("<Dispatcher CPU %d>", up->u_pid);
		return;
	}

	if (up->u_state == SZOMBIE)
		{ printf ("<defunct>"); return; }

	/*
	 *	Obtem o nome do Programa Diretamente da UPROC
	 */
	if ((argc = uap->g_argc) == 0)
		{ printf (up->u_pgname); return; }

	/*
	 *	Imprime os Argumentos
	 */
	for (cp = uap->g_argp; argc > 0; argc--)
	{
		while (c = *cp++)
			putchar (c);

		putc (' ', stdout);

	}	/* end loop */

}	/* end print_args */

/*
 ****************************************************************
 *	Imprime uma entrada de REGION LOCAL			*
 ****************************************************************
 */
void
print_local_region (const REGIONL *rgl)
{
	printf
	(	"%s: regiong = %P, vaddr = %P, prot = %s",
		region_type_nm[rgl->rgl_type],
		rgl->rgl_regiong, rgl->rgl_vaddr,
		rgl->rgl_prot == URO ? "RO" : (rgl->rgl_prot == URW) ? "RW" : "??"
	);

	/*
	 *	Verifica se o endereço REGIONG é válido e conta
	 */
	add_regiong_to_vector (rgl->rgl_regiong);

#if (0)	/*******************************************************/
	int		i;

	if (rgl->rgl_regiong == NOREGIONG)
		return;

	i = (int)rgl->rgl_regiong - (int)scb.y_regiong;

	if
	(	rgl->rgl_regiong < scb.y_regiong ||
		rgl->rgl_regiong >= scb.y_endregiong ||
		i % sizeof (REGIONG)
	)
	{
		printf ("\t****** Endereço REGIONG %P inválido\n", rgl->rgl_regiong);
		return;
	}

	i /= sizeof (REGIONG);

	region_global_busy_vector[i]++;
#endif	/*******************************************************/

}	/* print_local_region */

/*
 ****************************************************************
 *	Imprime uma entrada de REGION GLOBAL			*
 ****************************************************************
 */
void
print_global_region (const REGIONG *rgp, pg_t vaddr)
{
	pg_t		begin_addr, end_addr;
	pg_t		pgtb_size;
	mmu_t		*pgtb_ptr, *mmup;
	mmu_t		first = 0, second = 0, last = 0;
	int		first_i = -1, second_i = -1, last_i = -1;
	int		i, used = 0;

	printf
	(	"size = %4u KB, caddr = %5u KB (%P), count = %2u, nome = \"%s\"\n",
		PGTOKB (rgp->rgg_size),
		ADDRTOKB (rgp->rgg_paddr), rgp->rgg_paddr,
		rgp->rgg_count, rgp->rgg_name
	);

	if (rgp->rgg_pgtb == NOPG)
		return;

	/*
	 *	Calcula a tabela de páginas particular do processo
	 */
	begin_addr = vaddr; end_addr = begin_addr + rgp->rgg_size;

	begin_addr &= ~(PGSZ/MMUSZ - 1);
	end_addr   +=  (PGSZ/MMUSZ - 1); end_addr &= ~(PGSZ/MMUSZ - 1);

	pgtb_size = (end_addr - begin_addr) / (PGSZ/MMUSZ);

	/*
	 *	Coleta os dados da tabela de páginas particular do processo
	 */
	if ((int)(pgtb_ptr = phys ((void *)PGTOBY (rgp->rgg_pgtb), pgtb_size << PGSHIFT, O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear uma tabela de páginas particular");

	for (mmup = pgtb_ptr, i = 0; i < (pgtb_size << (PGSHIFT-MMUSHIFT)); i++, mmup++)
	{
		if (*mmup == 0)
			continue;

		used++;

		if   (first == 0)
			{ first = *mmup; first_i = i; }
		elif (second == 0)
			{ second = *mmup; second_i = i; }

		{ last = *mmup; last_i = i; }
	}

	phys (pgtb_ptr, 0, 0);

	/*
	 *	Imprime a tabela de páginas particular do processo
	 */
	printf
	(	"\tTabela de páginas: (%P, %u página%s), %u entrada%s usada%s (%s)\n\n",
		rgp->rgg_pgtb, pgtb_size, plural (pgtb_size),
		used, plural (used), plural (used), 
		used == rgp->rgg_size ? "OK" : "**** ERROR"
	);

	if (lflag)
	{
		printf ("\tPrimeira entrada: %5u, %P\n", first_i, first);
		printf ("\tSegunda  entrada: %5u, %P\n", second_i, second);
		printf ("\tÚltima   entrada: %5u, %P\n\n", last_i, last);
	}

}	/* print_global_region */

/*
 ****************************************************************
 *	Acrescente a REGIONG no vetor				*
 ****************************************************************
 */
void
add_regiong_to_vector (const REGIONG *rgp)
{
	int		i;

	/*
	 *	Verifica se o endereço REGIONG é válido e conta
	 */
	if (rgp == NOREGIONG)
		return;

	i = (int)rgp - (int)scb.y_regiong;

	if (rgp < scb.y_regiong || rgp >= scb.y_endregiong || i % sizeof (REGIONG))
		{ printf ("\t****** Endereço REGIONG %P inválido\n", rgp); return; }

	i /= sizeof (REGIONG);

	region_global_busy_vector[i]++;

}	/* end add_regiong_to_vector */

/*
 ****************************************************************
 *	Obtém o plural						*
 ****************************************************************
 */
const char *
plural (int n)
{
	if (n == 1)
		return ("");
	else
		return ("s");

}	/* end plural */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - imprime as regiões dos processos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [<pid> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nObs.:\tSe não forem dados <pid>s, serão impressas "
		"informações sobre\n\ttodos os processos\n"
	);

	exit (2);

}	/* end help */
#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Prepara Ponteiros para PPDA e STACK			*
 ****************************************************************
 */
int
setuser (PROC *pp)
{
	UARG		*uapf, *uapd;
	int		SDIFF;
	UPROC		*up;
	void		*phys_addr;
	char		*sp;
	REGIONL		*rlp;
	REGIONG		*rgp;
	int		i;

	/*
	 *	Se estava mapeado, desfaz o mapeamento
	 */
	if (userp != NOUPROC)
		{ phys (userp, 0, 0); userp = NOUPROC; }

	if (stackp != NOSTR)
		{ phys (stackp, 0, 0); stackp = NOSTR; }

	/*
	 *	Se o estado for ZOMBIE, não tem sentido mapear PPDA ou STACK
	 */
	if (pp->u_state == SZOMBIE)
		return (1);

	/*
	 *	Mapeia a PPDA
	 */
	up = phys ((void *)PGTOBY (pp->u_caddr), sizeof (UPROC), O_KERNEL|O_RDONLY)) == -1)

	if ((int)(userp = up) == -1)
	    	error ("$*Não consegui mapear a PPDA");

	/*
	 *	Prepara a STACK de processos sem STACK
	 */
	if (pp->u_flags & SSYS)
	{
		uapd = &uarg;		uargp = uapd;

		uapd->g_envp = NOSTR; 	uapd->g_envc = 0;
		uapd->g_argp = NOSTR; 	uapd->g_argc = 0;

		return (1);
	}

	/*
	 *	Zera todas as REGIONLs e REGIONGs
	 */
	memset (&text_rgl,  0, sizeof (REGIONL));
	memset (&data_rgl,  0, sizeof (REGIONL));
	memset (&stack_rgl, 0, sizeof (REGIONL));

	memset (&text_rgg,  0, sizeof (REGIONG));
	memset (&data_rgg,  0, sizeof (REGIONG));
	memset (&stack_rgg, 0, sizeof (REGIONG));

	memset (&phys_rgg, 0, PHYSNO * sizeof (REGIONG));

	/*
	 *	Copia as REGIONLs principais
	 */
	memmove (&text_rgl,  &pp->u_text,  sizeof (REGIONL));
	memmove (&data_rgl,  &pp->u_data,  sizeof (REGIONL));
	memmove (&stack_rgl, &pp->u_stack, sizeof (REGIONL));

	/*
	 *	Mapeia a REGIONG do TEXT
	 */
	if (text_rgl.rgl_regiong)
	{
		rgp = phys (text_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT");

		memmove (&text_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	text_rgl.rgl_regiong = &text_rgg; ***/
	}

	/*
	 *	Mapeia a REGIONG do DATA
	 */
	if (data_rgl.rgl_regiong)
	{
		rgp = phys (data_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do DATA");

		memmove (&data_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	data_rgl.rgl_regiong = &data_rgg; ***/
	}

	/*
	 *	Mapeia a REGIONG da STACK
	 */
	if (stack_rgl.rgl_regiong)
	{
		rgp = phys (stack_rgl.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do STACK");

		memmove (&stack_rgg, rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	stack_rgl.rgl_regiong = &stack_rgg; ***/
	}

	/*
	 *	Copia as REGIONLs do PHYS
	 */
	memmove (phys_rgl, up->u_phys_region, PHYSNO * sizeof (REGIONL));

	/*
	 *	Mapeia as REGIONGs do PHYS
	 */
	for (i = 0, rlp = &phys_rgl[0]; rlp < &phys_rgl[PHYSNO]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do PHYS");

		memmove (&phys_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &phys_rgg[i]; ***/
	}

	/*
	 *	Mapeia as REGIONGs do SHMEM
	 */
	for (i = 0, rlp = &pp->u_shlib_text[0]; rlp < &pp->u_shlib_text[NSHLIB]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT da SHLIB");

		memmove (&shlib_text_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &shlib_text_rgg[i]; ***/
	}

	for (i = 0, rlp = &pp->u_shlib_data[0]; rlp < &pp->u_shlib_data[NSHLIB]; i++, rlp++)
	{
		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		rgp = phys (rlp->rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)

		if ((int)rgp == -1)
		    	error ("$*Não consegui mapear a REGIONG do TEXT da SHLIB");

		memmove (&shlib_data_rgg[i], rgp, sizeof (REGIONG));

		phys (rgp, 0, 0);

	   /***	rlp->rgl_regiong = &shlib_data_rgg[i]; ***/
	}

	/*
	 *	Mapeia a STACK
	 */
	phys_addr = (void *)PGTOBY (stack_rgg.rgg_paddr + stack_rgg.rgg_size);

	sp = phys (phys_addr - SSEG, SSEG, O_KERNEL|O_RDONLY)) == -1)

	if ((int)(stackp = sp) == -1)
	    	error ("$*Não consegui mapear a STACK");

	/*
	 *	Prepara os diversos ponteiros da STACK
	 */
	SDIFF = (int)STKADDR - (int)sp - SSEG;

	uapd = &uarg;
	uapf = (UARG *)(STKADDR - sizeof (UARG) - SDIFF);

	uapd->g_envp = uapf->g_envp - SDIFF;
	uapd->g_envc = uapf->g_envc;

	if (uapd->g_envp < sp || uapd->g_envp >= sp + SSEG)
	{
		uapd->g_envp = NOSTR;
		uapd->g_envc = 0;
	}

	uapd->g_argp = uapf->g_argp - SDIFF;
	uapd->g_argc = uapf->g_argc;

	if (uapd->g_argp < sp || uapd->g_argp >= sp + SSEG)
	{
		uapd->g_argp = NOSTR;
		uapd->g_argc = 0;
	}

	uargp = uapd;

	return (1);

}	/* end setuser */
#endif	/*******************************************************/

