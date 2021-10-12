/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Módulo principal					*
 *								*
 *	Versão	3.0.0, de 04.10.94				*
 *		4.9.0, de 20.02.08				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mmu.h"
#include "../h/ipc.h"
#include "../h/disktb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
#include "../h/devmajor.h"
#include "../h/iotab.h"
#include "../h/cpu.h"
#include "../h/signal.h"
#include "../h/sysdirent.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Variáveis globais ***************************************
 */
entry SB		sb_head;		/* Cabeça da lista de SBs */

entry INODE		*rootdir;		/* Diretório da RAIZ Universal */

entry UPROCV		*uprocfreelist;		/* Lista das Entradas UPROC não usadas */

entry LOCK		uprocfreelist_lock;

/*
 ******	Protótipos de funções ***********************************
 */
void		iinit (void);
int		check_root_dev (const char *dev_nm, dev_t, SB *);

/*
 ****************************************************************
 *	Início do KERNEL					*
 ****************************************************************
 */
void
main (void)
{
	SCB		*sp = &scb;
	int		i;
	UPROCV		*uvp;
	UPROC		*up;
	int		major;
	const BIOTAB	*bdp;
	const CIOTAB	*cdp;
	const DISKTB	*pp;
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

#if (0)	/*******************************************************/
	/*
	 *	Nada de troca de processos durante a inicialização
	 */
	for (i = 0; i < NCPU; i++)
		preemption_flag[i] = 0;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	/*
	 *	Inicializa o coprocessador, se ativado e
	 *	sempre liga o bit da emulação EM
	 */
	if (sp->y_fpu_enabled)
		fpu_init ();
#endif	/*******************************************************/

	fpu_set_embit ();

	/*
	 *	Constroi Artesanalmente o Processo 0
	 */
	sp->y_uproc[0].u_uproc = uproc_0;
	u.u_uprocv	= &sp->y_uproc[0];

	u.u_state	= SRUN;
	u.u_flags	= SSYS;
	u.u_mask	= 0xFF;	/* == ALLCPUs */
	u.u_pri		= PSCHED;
	u.u_myself	= uproc_0;
	u.u_parent	= uproc_0;
	u.u_size	= USIZE;
	u.u_mmu_dir	= kernel_page_directory;
	strcpy (u.u_pgname, "swapper/pager");

   /*** u.u_machine	= 0; ***/
	u.u_cpu		= CPUID;
	u.u_pri		= PSCHED;
	u.u_cmask	= sp->y_umask;
	u.u_no_user_mmu	= 1;

	/*
	 *	Inicializa a tabela de processos
	 *
	 *	Já deixa a entrada do PROC 0 fora da Lista
	 */
   /***	SPINLOCK (&uprocfreelist_lock);	***/

	for (uvp = scb.y_enduproc - 1;  uvp > scb.y_uproc;  uvp--)
		{ uvp->u_next = uprocfreelist; uprocfreelist = uvp; }

   /***	SPINFREE (&uprocfreelist_lock);	***/

	/*
	 *	Cria os processos dos dispatchers
	 */
	for (i = 0; i < sp->y_ncpu; i++)
	{
		if (newproc (THREAD) == 0)
		{
			strcpy (u.u_pgname, "dispatcher");
			ctxtsto (u.u_ctxt);
			procswitch ();

			/****** sem retorno *****/
		}
	}

	/*
	 *	Retira os dispatchers da readylist
	 */
	for (uvp = &sp->y_uproc[1]; uvp <= &sp->y_uproc[sp->y_ncpu]; uvp++)
	{
		up = uvp->u_uproc;

		up->u_state  = SCORESLP;
		up->u_flags |= SSYS;
		up->u_pri    = 2000;

		remove_proc_from_corerdylist (up);
	}

	/*
	 *	Liga o relógio
	 */
	timeout_init_free_list ();

	init_clock ();

	block_lru_init ();	/* BHEAD */

	/*
	 *	Procura dispositivos PnP
	 */
	pnp_init ();

	/*
	 *	Procura dispositivos PCI
	 */
	pci_init ();

	/*
	 *	Conta o número de dispositivos e dá o "attach" neles
	 */
	for (bdp = biotab; bdp->w_open != NOFUNC; bdp++)
		sp->y_nblkdev++;

	for (cdp = ciotab, major = 0; cdp->w_attach != NOFUNC; cdp++, major++)
		{ (*cdp->w_attach) (major); sp->y_nchrdev++; }

	/*
	 *	Inicializa várias tabelas
	 */
	cinit ();		/* CLIST */

#if (0)	/*******************************************************/
	block_lru_init ();	/* BHEAD */
#endif	/*******************************************************/

	inode_lru_init ();	/* INODE */

	iinit ();		/* INODE */

	swapinit ();		/* SWAP */

	lfinit ();		/* LOCKF */

	ueventginit ();		/* UEVENTG */

	usemaginit ();		/* USEMAG */

	regionginit ();		/* REGIONG */

	/*
	 *	Inicializa o diretório RAIZ
	 */
	u.u_cdir = rootdir = iget (sp->y_rootdev, sb_head.sb_forw->sb_root_ino, 0);
	iinc (rootdir);
	SLEEPFREE (&rootdir->i_lock);

	u.u_rdir = NOINODE;

	/*
	 *	Inicializa o dispositivo RAIZ
	 */
	if ((ip = iname ("/dev", getschar, ISEARCH, &io, &de)) == NOINODE)
		printf ("NÃO consegui obter o INODE de \"/dev\"\n");
	else
		SLEEPFREE (&ip->i_lock);

	/*
	 *	Atualiza os dispositivos
	 */
	strcpy (de.d_name, "/dev/");

	for (pp = disktb; pp->p_name[0] != '\0'; pp++)
	{
		strcpy (de.d_name + 5, pp->p_name);
		kmkdev (de.d_name, IFBLK|0740, pp->p_dev);

		de.d_name[5] = 'r'; strcpy (de.d_name + 6, pp->p_name);
		kmkdev (de.d_name, IFCHR|0640, pp->p_dev);
	}

	/*
	 *	Carrega a "libt.o"
	 */
	if   (libt_nm[0] == '\0')
		/* Neste caso, não carrega */;
	elif (shlib_kernel_load (libt_nm) >= 0)
		printf ("Biblioteca compartilhada \"%s\" carregada\n", libt_nm);
	elif (sp->y_boot_verbose)
		printf ("NÃO consegui carregar \"%s\" (%s)\n", libt_nm, sys_errlist[u.u_error]);

	/*
	 *	Desliga o verboso
	 */
	sp->y_boot_verbose = 0;

	/*
	 *	Cria, se necessário, o "USB Explorer"
	 */
	if (usb_active > 0 && newproc (THREAD) == 0)
	{
		strcpy (u.u_pgname, "<USB Explorer>");
		usb_explorer ();

		/****** sem retorno *****/
	}

	/*
	 *	Cria o Processo do INIT
	 */
	if (newproc (FORK) == 0)
	{
		up = u.u_myself; strcpy (up->u_pgname, "init");

		sp->y_initpid = u.u_pid;

		/*
		 *	Cria uma região DATA para carregar o init embrião
		 */
		up->u_data.rgl_type  = RG_DATA;
		up->u_data.rgl_vaddr = USER_DATA_PGA;
		up->u_data.rgl_prot  = URW;

		if (regiongrow (&up->u_data, 0, BYUPPG (bcodesz), RG_CLR) < 0)
			panic ("main: Não consegui alocar o DATA do INIT embrião");

		/*
		 *	Copia o texto do INIT embrião para a região DATA
		 */
		memmove ((void *)PGTOBY (up->u_data.rgl_regiong->rgg_paddr), bootcode, bcodesz);

		/*
		 *	Cria uma região STACK para o init embrião
		 */
		up->u_stack.rgl_type  = RG_STACK;
		up->u_stack.rgl_vaddr = USER_STACK_PGA - 1;
		up->u_stack.rgl_prot  = URW;

		if (regiongrow (&up->u_stack, 0, USIZE, RG_CLR) < 0)
			panic ("main: Não consegui alocar a STACK do INIT embrião");

		/*
		 *	O Proc. initpid executa o Prog. de Boot copiado
		 */
		return;
	}

	/*
	 *	O Proc. 0 executa o Scheduler
	 */
	sched ();

	/* Não retorna */

}	/* end main */

/*
 ****************************************************************
 *	Le o superbloco da RAIZ					*
 ****************************************************************
 */
void
iinit (void)
{
	SB		*sp, *root_sp;
	DISKTB		*pp;
	int		i;
	dev_t		dev;
	extern int	fdtypes;
	char		dev_nm[16];

	/*
	 *	Obtém uma área para o SUPERBLOCO
	 */
	if ((sp = malloc_byte (sizeof (SB))) == NOSB)
		panic ("Não consegui memória para o SB da RAIZ");

	memclr (sp, sizeof (SB));

	/*
	 *	Inicialmente tenta o dispositivo dado
	 */
	if ((dev = scb.y_rootdev) != NODEV)
	{
		sprintf (dev_nm, "/dev/%v", dev);

		printf ("Tentando o dispositivo RAIZ: \"%s\"\n", dev_nm);

		if (check_root_dev (dev_nm, dev, sp) < 0)
			{ printf ("NÃO consegui abrir \"%s\" (%r)\n", dev_nm, u.u_error); dev = NODEV; }
	}

	/*
	 *	Em seguinda, tenta os META-DOS
	 */
	if (dev == NODEV)
	{
		strcpy (dev_nm, "/dev/md1");

		printf ("Tentando o dispositivo RAIZ: Meta-DOS (\"/dev/md1\")\n");

		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '0')
				{ printf ("NÃO achei \"/dev/md1\" na DISKTB\n"); break; }

			if (MAJOR (pp->p_dev) == MD_MAJOR && pp->p_target == MD_ROOT_MINOR)
			{
				if (check_root_dev (dev_nm, pp->p_dev, sp) >= 0)
					scb.y_rootdev = dev = pp->p_dev;

				break;
			}
		}
	}

	/*
	 *	Finalmente, tenta os disquetes
	 */
	if (dev == NODEV)
	{
		if ((fdtypes & 0xFF) != 4 && (fdtypes >> 8) == 4)
			i = 1; 	/* fd0 NÃO 3½ e fd1 3½ */
		else
			i = 0;

		dev = MAKEDEV (FD_MAJOR, i); sprintf (dev_nm, "/dev/fd%d", i);

		printf ("Tentando o dispositivo RAIZ: Disquete (\"%s\")\n", dev_nm);

		if (check_root_dev (dev_nm, dev, sp) < 0)
			dev = NODEV;
		else
			scb.y_rootdev = dev;
	}

	/*
	 *	Verifica se conseguiu
	 */
	if (dev == NODEV)
		panic ("iinit: NÃO achei nenhum dispositivo para a RAIZ");

	printf ("Dispositivo RAIZ \"%s\" (\"%f\") montado\n", dev_nm, sp->sb_code);

	/*
	 *	Abre o dispositivo do SWAP
	 */
	u.u_error = NOERR;

	if (scb.y_swapdev != NODEV)
		(*biotab[MAJOR(scb.y_swapdev)].w_open) (scb.y_swapdev, O_RW);

	if (u.u_error != NOERR)
		panic ("iinit: Erro de OPEN do SWAP (%s)", sys_errlist[u.u_error]);

	/*
	 *	Inicializa a lista de SBs e coloca o SB da RAIZ no início
	 */
	strcpy (sp->sb_dev_nm, dev_nm);
	strcpy (sp->sb_dir_nm, "/");

	memsetl (&sp->sb_mntent, -1, sizeof (MNTENT) / sizeof (long));
	sp->sb_mntent.me_flags = SB_DEFAULT_ON & ~SB_NOT_KERNEL;

   /***	sp->sb_mlock	= 0;	***/
   /***	sp->sb_sbmod	= 0;	***/
   /***	sp->sb_mbusy	= 0;	***/

   /***	sp->sb_mcount	= 0;	***/

   /***	sp->sb_inode	= NOINODE;	***/
	sp->sb_dev	= scb.y_rootdev;

	sp->sb_back	= &sb_head;
	sp->sb_forw	= &sb_head;

	sb_head.sb_forw = sb_head.sb_back = sp;

	disktb[MINOR (sp->sb_dev)].p_sb = sp;

	/*
	 *	Cria o SUPERBLOCO para PIPEs
	 */
	for (pp = disktb; /* abaixo */; pp++)
	{
		if (pp->p_name[0] == '0')
			panic ("NÃO achei \"pipe\" na DISKTB");

		if (streq (pp->p_name, "pipe"))
			break;
	}

	root_sp = sp;

	if ((sp = malloc_byte (sizeof (SB))) == NOSB)
		panic ("Não consegui memória para o SB do PIPE");

	memclr (sp, sizeof (SB));

	strcpy (sp->sb_dev_nm, "/dev/pipe");
	sp->sb_dir_nm[0] = '\0';

	memsetl (&sp->sb_mntent, -1, sizeof (MNTENT) / sizeof (long));
	sp->sb_mntent.me_flags = SB_DEFAULT_ON & ~SB_NOT_KERNEL;

   /***	sp->sb_mlock	= 0;	***/
   /***	sp->sb_sbmod	= 0;	***/
   /***	sp->sb_mbusy	= 0;	***/

   /***	sp->sb_mcount	= 0;	***/

   /***	sp->sb_inode	= NOINODE;	***/
	sp->sb_dev	= scb.y_pipedev = pp->p_dev;
	sp->sb_code	= FS_PIPE;

	root_sp->sb_forw = sp;
	sp->sb_back	 = root_sp;

	sb_head.sb_back  = sp;
	sp->sb_forw	 = &sb_head;

	pp->p_sb	 = sp;

}	/* end iinit */

/*
 ****************************************************************
 *	Verifica se o dispositivo pode ser a RAIZ		*
 ****************************************************************
 */
int
check_root_dev (const char *dev_nm, dev_t dev, SB *sp)
{
	const DISKTB	*up;
	int		code;

	/*
	 *	Verifica se consegue abrir o dispositivo
	 */
	u.u_error = NOERR;

	(*biotab[MAJOR (dev)].w_open) (dev, O_RW);

	if (u.u_error != NOERR)
	{
		if (u.u_error != EROFS)
			return (-1);

		/* Tenta abrir somente para leituras */

		u.u_error = NOERR;

		(*biotab[MAJOR (dev)].w_open) (dev, O_READ);

		if (u.u_error != NOERR)
			return (-1);

		printf ("Montando o dispositivo \"%s\" para leituras apenas\n", dev_nm);
	}

	/*
	 *	Verifica se o código da partição está disponível
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB || up->p_type == 0)
	{
		for (code = FS_V7; /* abaixo */; code++)
		{
			if (code >= FS_LAST)
			{
				printf
				(	"NÃO consegui identificar o sistema "
					"de arquivos do dispositivo \"%s\"\n",
					dev_nm
				);
				goto close;
			}

			if ((*fstab[code].fs_authen) (dev) >= 0)
				break;
		}
	}
	else
	{
		switch (up->p_type)
		{
		    case PAR_TROPIX_V7:
			if (v7_authen (dev) < 0)
				goto bad;

			code = FS_V7;
			break;

		    case PAR_TROPIX_T1:
			if (t1_authen (dev) < 0)
				goto bad;

			code = FS_T1;
			break;

		    default:
			printf
			(	"O sistema de arquivos do dispositivo \"%s\""
				" NÃO pode ser usado como RAIZ\n",
				dev_nm
			);
			goto close;
		}
	}

	/*
	 *	Realiza a montagem específica
	 */
	sp->sb_dev = dev;

	if ((*fstab[code].fs_mount) (sp) >= 0)
		return (0);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	printf ("O dispositivo \"%s\" NÃO contém um sistema de arquivos\n", dev_nm);

    close:
	(*biotab[MAJOR (dev)].w_close) (dev, 0);

	return (-1);

}	/* end check_root_dev */
