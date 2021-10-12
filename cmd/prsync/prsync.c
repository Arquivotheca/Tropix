/*
 ****************************************************************
 *								*
 *			prsync.c				*
 *								*
 *	Imprime o estado da sincronização dos processos		*
 *								*
 *	Versão	1.0.0, de 19.06.87				*
 *		4.8.0, de 26.10.05				*
 *								*
 *	Modulo: 						*
 *		Utilitário Especial				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
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

#include <sys/devmajor.h>
#include <sys/bhead.h>
#include <sys/mmu.h>

#include <sys/signal.h>
#include <sys/uerror.h>
#include <sys/uarg.h>
#include <sys/uproc.h>
#include <sys/kfile.h>
#include <sys/tty.h>
#include <sys/itnet.h>
#include <sys/nfs.h>

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
const char	pgversion[] =  "Versão:	4.8.0, de 26.10.05";

#define	SSEG	8192		/* Tamanho do trecho mapeado da pilha */

#define	NPID	20

/*
 ****************************************************************
 *	Variáveis e áreas globais				*
 ****************************************************************
 */
entry SCB		scb;			/* O bloco de controle */

/*
 ****** Diversos dispositivos ***********************************
 */
entry ITSCB		*itscb_begin;		/* INTERNET ITSCB */

entry TTY		*conbegin, *conend;	/* Tabelas de consoles */

entry TTY		*siobegin, *sioend;	/* Tabelas de linhas seriais */

entry const NFSINQ	*nfs_inq, *end_nfs_inq;	/* Eventos do NFS */

/*
 ****************************************************************
 *	Variáveis globais das várias áreas			*
 ****************************************************************
 */
entry int		exit_code;	/* Para o exit */

/*
 ******	Protótipos **********************************************
 */
const UPROC		*setuser (UPROC const *kup, UARG *uap);
void			print_process (const UPROC *up, int uproc_delta, const UARG *uap);
char			*scb_table_search (const UPROC *, int uproc_delta, int);
char			*tty_search (int);
char			*itscb_search (int);
char			*nfs_search (int addr);
char			*syncnmget (int);
void			help (void);

/*
 ****************************************************************
 *	Imprime o estado da sincronização dos processos		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	UPROCV		*uvp, *lastproc;
	UARG		uarg;
	const UPROC	*up;
	const void	**vpp;
	const char	*str;
	int		opt, len, uproc_delta;
	SCB		*sp;
	SYM		*sym_p;
	int		pid_cnt = 0;
	long		pid, pid_vec[NPID];

	/*
	 *	Verifica o usuário efetivo
	 */
	if (geteuid () != 0)
		error ("$Não sou o SUPERUSUÁRIO!");

	/*
	 *	Analisa a Linha de Argumentos do Programa
	 */
	while ((opt = getopt (argc, argv, "MH")) != -1)
	{
		switch (opt)
		{
		    case 'M':
			exit (0);

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
	 *	Aloca uma estrutura SYM de tamanho suficiente
	 */
	sym_p = alloca (SYM_SIZEOF (80));

	/*
	 *	Obtém o endereço das consoles do PC
	 */
	len = strlen ("_con"); sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_con");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_con\"");

	{ conbegin = (TTY *)sym_p->s_value; conend = conbegin + sp->y_NCON; }

	/*
	 *	Obtém o endereço dos terminais do PC
	 */
	len = strlen ("_sio"); sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_sio");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_sio\"");

	{ siobegin = (TTY *)sym_p->s_value; sioend = siobegin + sp->y_NSIO; }

	/*
	 *	Obtém o endereço do INTERNET ITSCB
	 */
	len = strlen ("_itscb"); sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_itscb");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_itscb\"");

	itscb_begin = (ITSCB *)sym_p->s_value;

	/*
	 *	Mapeia a Tabela de Processos
	 */
	if ((int)(uvp = (UPROCV *)phys (sp->y_uproc, sp->y_nproc * sizeof (UPROCV), O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear a tabela de processos");

	lastproc = (UPROCV *)((int)sp->y_lastuproc + (int)uvp - (int)sp->y_uproc);

	/*
	 *	Obtém o endereço do NFSINQ
	 */
	len = strlen ("_nfs_inq"); sym_p->s_nm_len = len; strcpy (sym_p->s_name, "_nfs_inq");

	if (kcntl (K_GETSYMGN, len, sym_p) < 0)
		error ("$*Não consegui obter o endereço de \"_nfs_inq\"");

	if ((int)(vpp = phys ((NFSINQ *)sym_p->s_value, sizeof (NFSINQ *), O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear a NFSINQ");

	nfs_inq = *vpp;

	phys (vpp, 0, 0);

	end_nfs_inq = nfs_inq + scb.y_n_nfs_daemon;

	/*
	 *	Coloca o Cabecalho
	 */
	printf ("  PID   TIPO  ENDEREÇO   MAX. TABELA          ENTRADA       CMD\n");

	/*
	 *	Malha Principal pela Tabela de Processos
	 */
	for (/* acima */; uvp <= lastproc; uvp++)
	{ 
		/*
		 *	Mapeia UPROC & PILHA
		 */
		if (uvp->u_uproc == NOUPROC)
			continue;

		if ((up = setuser (uvp->u_uproc, &uarg)) == NOUPROC)
			continue;

		uproc_delta = (int)up - (int)uvp->u_uproc;

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
		print_process (up, uproc_delta, &uarg); putchar ('\n');
	}

	return (exit_code);

}	/* end prsync */

/*
 ****************************************************************
 *	Prepara Ponteiros para UPROC e PILHA			*
 ****************************************************************
 */
const UPROC *
setuser (UPROC const *kup, UARG *uap)
{
	const UARG		*uapf;
	int			stack_delta;
	const UPROC		*up;
	void			*phys_addr;
	char			*sp;
	static const UPROC	*mapup;
	static const char	*mapsp;
	REGIONG			*rgp;

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
print_process (const UPROC *up, int uproc_delta, const UARG *uap)
{
	const SYNCH	*sp;
	SYM		*sym_p;
	char		*cp;
	int		n = 0, argc, c;

	sym_p = alloca (SYM_SIZEOF (80)); sym_p->s_nm_len = 80;

	printf ("%5u  ", up->u_pid);

	printf ("%s  ", syncnmget (up->u_stype));

	printf ("%P  ", up->u_sema);

	printf ("%3d  ", up->u_syncmax);

	if (up->u_stype == E_NULL)
	{
		printf ("%-30.30s", "");
	}
	elif (cp = nfs_search ((int)up->u_sema))
	{
		printf ("%-16.16s", cp);
		printf ("%-14.14s", cp + 16);
	}
	elif (kcntl (K_GETSYMGA, up->u_sema, sym_p), streq (sym_p->s_name, "_end"))
	{
		printf ("%-16.16s", (cp = scb_table_search (up, uproc_delta, (int)up->u_sema)));
		printf ("%-14.14s", cp + 16);
	}
	elif (cp = tty_search ((int)up->u_sema))
	{
		printf ("%-16.16s", cp);
		printf ("%-14.14s", cp + 16);
	}
	elif (cp = itscb_search ((int)up->u_sema))
	{
		printf ("%-16.16s", cp);
		printf ("%-14.14s", cp + 16);
	}
	elif (sym_p->s_name[0])
	{
		printf ("%-30.30s", (sym_p->s_name) + 1);
	}
	else
	{
		printf ("%-30.30s", "???");
	}

	/*
	 ******	Imprime os Argumentos ***********************************
	 *
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

	/*
	 ******	Imprime os semáforos alocados ***************************
	 */
	for (sp = &up->u_synch[0]; sp < &up->u_synch[NHIST]; sp++)
	{
		if (sp->e_state == E_GOT)
		{
			sym_p->s_nm_len = 80;

			kcntl (K_GETSYMGA, sp->e_sema, sym_p);

			printf ("\n\t\t\t      ");

			if   (cp = nfs_search ((int)up->u_sema))
			{
				printf ("%-16.16s", cp);
				printf ("%-14.14s", cp + 16);
			}
			elif (streq (sym_p->s_name, "_end"))
			{
				cp = scb_table_search (up, uproc_delta, (int)sp->e_sema);

				printf ("%-16.16s", cp);
				printf ("%-14.14s", cp + 16);
			}
			elif ((cp = tty_search ((int)sp->e_sema)))
			{
				printf ("%-16.16s", cp);
				printf ("%-14.14s", cp + 16);
			}
			elif (cp = itscb_search ((int)up->u_sema))
			{
				printf ("%-16.16s", cp);
				printf ("%-14.14s", cp + 16);
			}
			elif (sym_p->s_name[0])
			{
				printf ("%-30.30s", sym_p->s_name + 1);
			}
			else
			{
				printf ("%-30.30s", "???");
			}
			n++;
		}
	}

}	/* end print_process */

/*
 ****************************************************************
 *	Procura o endereço nas Tabelas do SCB			*
 ****************************************************************
 */
char *
scb_table_search (const UPROC *up, int uproc_delta, int addr)
{
	int		i, kup;
	static char	area[32];

	kup = (int)up - uproc_delta; area[16] = '\0';

 	if   (addr >= kup && addr < kup + sizeof (UPROC))
	{
		sprintf (area, "uproc");
#if (0)	/*******************************************************/
printf ("up = %P, addr = %P, uproc_delta = %P\n", up, addr, uproc_delta);

		sprintf
		(	area,
			"uproc[%d]",
			(PROC *)addr - scb.y_proc
		);

		i = (addr - (int)scb.y_proc) % sizeof (PROC);
#endif	/*******************************************************/

		i = addr - kup;

		switch (i)
		{
		    case offsetof (UPROC, u_lock):
			sprintf (area + 16, "lock");
			break;

		    case offsetof (UPROC, u_childlock):
			sprintf (area + 16, "childlock");
			break;

		    case offsetof (UPROC, u_deadchild):
			sprintf (area + 16, "deadchild");
			break;

		    case offsetof (UPROC, u_trace):
			sprintf (area + 16, "trace");
			break;

		    case offsetof (UPROC, u_attention_event):
			sprintf (area + 16, "attention");
			break;

		    case offsetof (UPROC, u_nfs.nfs_event):
			sprintf (area + 16, "nfs_event");
			break;

		    default:
			sprintf (area + 16, "+%d", i);
			break;
		}
	}
 	elif (addr >= (int)scb.y_inode && addr < (int)scb.y_endinode)
	{
		INODE		*ip;
		int		offset;
		char		*lock;

#if (0)	/*******************************************************/
#		define	i_notempty	i_xcount	/* Do PIPE */
#		define	i_notfull	i_xcorecnt
#endif	/*******************************************************/

		i = (INODE *)addr - scb.y_inode;

		offset = (int)addr - (int)&scb.y_inode[i];

		ip =   (INODE *)phys
		(
			scb.y_inode + i,
			sizeof (INODE),
			O_KERNEL|O_RDONLY
		);

		if   (offset == offsetof (INODE, i_lock))
			lock = "lock";
		elif (offset == offsetof (INODE, i_xlock))
			lock = "xlock";
		elif (offset == offsetof (INODE, i_notempty))
			lock = "Pnempty";
		elif (offset == offsetof (INODE, i_notfull))
			lock = "Pnfull";
		else
			lock = NOSTR;

		if (lock != NOSTR)
			sprintf (area, "inode[%d].%s", i, lock);
		else
			sprintf (area, "inode[%d].%d", i, offset);

		area[15] = '\0';

		sprintf
		(	area + 16,
			"(%d, %d), %d",
			MAJOR (ip->i_dev),
			MINOR (ip->i_dev),
			ip->i_ino
		);

		phys (ip, 0, 0);
	}
 	elif (addr >= (int)scb.y_ihash && addr < (int)scb.y_endihash)
	{
		IHASHTB		*ip;
		int		offset;
		char		*lock;

		i = (IHASHTB *)addr - scb.y_ihash;

		offset = (int)addr - (int)&scb.y_ihash[i];

		ip =   (IHASHTB *)phys
		(
			scb.y_ihash + i,
			sizeof (IHASHTB),
			O_KERNEL|O_RDONLY
		);

		if   (offset == offsetof (IHASHTB, h_lock))
			lock = "lock";
		else
			lock = NOSTR;

		if (lock != NOSTR)
			sprintf (area, "ihash[%d].%s", i, lock);
		else
			sprintf (area, "ihash[%d].%d", i, offset);

		area[15] = '\0';

#if (0)	/*******************************************************/
		sprintf
		(	area + 16,
			"(%d, %d), %d",
			MAJOR (ip->i_dev),
			MINOR (ip->i_dev),
			ip->i_ino
		);
#endif	/*******************************************************/

		phys (ip, 0, 0);
	}
 	elif (addr >= (int)scb.y_bhead && addr < (int)scb.y_endbhead)
	{
		BHEAD		*bp;
		int		offset;

		i = (BHEAD *)addr - scb.y_bhead;

		offset = (int)addr - (int)&scb.y_bhead[i];

		bp =   (BHEAD *)phys
		(
			scb.y_bhead + i,
			sizeof (BHEAD),
			O_KERNEL|O_RDONLY
		);

		sprintf
		(	area,
			"bhead[%d],%d",
			i,
			bp->b_blkno
		);

		switch (offset)
		{
		    case offsetof (BHEAD, b_lock):
			sprintf (area + 16, "b_lock");
			break;

		    case offsetof (BHEAD, b_done):
			sprintf (area + 16, "b_done");
			break;

		    default:
			sprintf (area + 16, "+%d", offset);
			break;
		}

		phys (bp, 0, 0);
	}
 	elif (addr >= (int)scb.y_kfile && addr < (int)scb.y_endkfile)
	{
		sprintf
		(	area,
			"file[%d]+%d",
			(KFILE *)addr - scb.y_kfile,
			(addr - (int)scb.y_kfile) % sizeof (KFILE)
		);
	}
 	elif (addr >= (int)scb.y_dns  &&  addr < (int)scb.y_end_dns)
	{
		sprintf
		(	area,
			"dns[%d]",
			(DNS *)addr - scb.y_dns
		);

		i = (addr - (int)scb.y_dns) % sizeof (DNS);

		switch (i)
		{
		    case offsetof (DNS, d_resolver_done):
			sprintf (area + 16, "resolver_done");
			break;

		    default:
			sprintf (area + 16, "+%d", i);
			break;
		}
	}
 	elif (addr >= (int)scb.y_raw_ep  &&  addr < (int)scb.y_end_raw_ep)
	{
		sprintf
		(	area,
			"raw_ep[%d]",
			(RAW_EP *)addr - scb.y_raw_ep
		);

		i = (addr - (int)scb.y_raw_ep) % sizeof (RAW_EP);

		switch (i)
		{
		    case offsetof (RAW_EP, rp_lock):
			sprintf (area + 16, "rp_lock");
			break;

		    case offsetof (RAW_EP, rp_inq_nempty):
			sprintf (area + 16, "inq_nempty");
			break;

		    default:
			sprintf (area + 16, "+%d", i);
			break;
		}
	}
 	elif (addr >= (int)scb.y_udp_ep  &&  addr < (int)scb.y_end_udp_ep)
	{
		sprintf
		(	area,
			"udp_ep[%d]",
			(UDP_EP *)addr - scb.y_udp_ep
		);

		i = (addr - (int)scb.y_udp_ep) % sizeof (UDP_EP);

		switch (i)
		{
		    case offsetof (UDP_EP, up_lock):
			sprintf (area + 16, "up_lock");
			break;

		    case offsetof (UDP_EP, up_inq_nempty):
			sprintf (area + 16, "inq_nempty");
			break;

		    default:
			sprintf (area + 16, "+%d", i);
			break;
		}
	}
 	elif (addr >= (int)scb.y_tcp_ep  &&  addr < (int)scb.y_end_tcp_ep)
	{
		const TCP_EP	*tp;
		int		offset;

		i = (TCP_EP *)addr - scb.y_tcp_ep;

		offset = (int)addr - (int)&scb.y_tcp_ep[i];

		tp = (TCP_EP *)phys (scb.y_tcp_ep + i, sizeof (TCP_EP), O_KERNEL|O_RDONLY);

		sprintf
		(	area,
			"tcp_ep[%d]",
			tp->tp_my_port
		);

#if (0)	/*******************************************************/
		sprintf
		(	area,
			"tcp_ep[%d]",
			(TCP_EP *)addr - scb.y_tcp_ep
		);

		i = (addr - (int)scb.y_tcp_ep) % sizeof (TCP_EP);
#endif	/*******************************************************/

		switch (offset)
		{
		    case offsetof (TCP_EP, tp_lock):
			sprintf (area + 16, "tp_lock");
			break;

		    case offsetof (TCP_EP, tp_inq_nempty):
			sprintf (area + 16, "inq_nempty");
			break;

		    case offsetof (TCP_EP, tp_good_snd_wnd):
			sprintf (area + 16, "good_snd_wnd");
			break;

		    case offsetof (TCP_EP, tp_pipe_event):
			sprintf (area + 16, "pipe_event");
			break;

		    case offsetof (TCP_EP, tp_rnd_in.tp_rnd_nfull):
			sprintf (area + 16, "rnd_in.nfull");
			break;

		    case offsetof (TCP_EP, tp_rnd_in.tp_rnd_nempty):
			sprintf (area + 16, "rnd_in.nempty");
			break;

		    case offsetof (TCP_EP, tp_rnd_out.tp_rnd_nfull):
			sprintf (area + 16, "rnd_out.nfull");
			break;

		    case offsetof (TCP_EP, tp_rnd_out.tp_rnd_nempty):
			sprintf (area + 16, "rnd_out.nempty");
			break;

		    default:
			sprintf (area + 16, "+%d", offset);
			break;
		}

		phys (tp, 0, 0);
	}
 	elif (addr >= (int)scb.y_pty  &&  addr < (int)scb.y_end_pty)
	{
		return (tty_search (addr));
	}
	else
	{
		sprintf (area, "??");
	}

	return (area);

}	/* end scb_table_search */

/*
 ****************************************************************
 *	Procura o endereço nas outras Tabelas			*
 ****************************************************************
 */
char *
tty_search (int addr)
{
	int		i;
	static char	area[32];

	area[16] = '\0';

 	if   (addr >= (int)conbegin  &&  addr < (int)conend)
	{
		sprintf (area, "con[%d]", (TTY *)addr - conbegin);
		i = (addr - (int)conbegin) % sizeof (TTY);
	}
 	elif (addr >= (int)siobegin  &&  addr < (int)sioend)
	{
		sprintf (area, "sio[%d]", (TTY *)addr - siobegin);
		i = (addr - (int)siobegin) % sizeof (TTY);
	}
 	elif (addr >= (int)scb.y_pty  &&  addr < (int)scb.y_end_pty)
	{
		sprintf (area, "pty[%d]", (PTY *)addr - scb.y_pty);
		i = (addr - (int)scb.y_pty) % sizeof (PTY);
	}
	else
	{
		return (NOSTR);
	}

	/*
	 *	Procura a entrada da tabela
	 */
	switch (i)
	{
	    case offsetof (TTY, t_olock):
		sprintf (area + 16, "olock");
		break;

	    case offsetof (TTY, t_obusy):
		sprintf (area + 16, "obusy");
		break;

	    case offsetof (TTY, t_inqnempty):
		sprintf (area + 16, "inqnempty");
		break;

	    case offsetof (TTY, t_outqnfull):
		sprintf (area + 16, "outqnfull");
		break;

	    case offsetof (TTY, t_outqempty):
		sprintf (area + 16, "outqempty");
		break;

	    case offsetof (TTY, t_outqstart):
		sprintf (area + 16, "outqstart");
		break;

	    case offsetof (TTY, t_outqnempty):
		sprintf (area + 16, "outqnempty");
		break;

	    default:
		sprintf (area + 16, "+%d", i);
		break;
	}

	return (area);

}	/* end tty_search */

/*
 ****************************************************************
 *	Procura o endereço nas outras Tabelas			*
 ****************************************************************
 */
char *
itscb_search (int addr)
{
	int		index;
	static char	area[32];

	area[16] = '\0';

 	if (addr < (int)itscb_begin || addr >= (int)itscb_begin + sizeof (ITSCB))
		return (NOSTR);

	sprintf (area, "itscb"); index = addr - (int)itscb_begin;

	/*
	 *	Procura a entrada da tabela
	 */
	switch (index)
	{
	    case offsetof (ITSCB, it_init_lock):
		sprintf (area + 16, "init_lock");
		break;

	    case offsetof (ITSCB, it_lock_free):
		sprintf (area + 16, "lock_free");
		break;

	    case offsetof (ITSCB, it_block_sema):
		sprintf (area + 16, "block_sema");
		break;

	    case offsetof (ITSCB, it_rep_lock):
		sprintf (area + 16, "rep_lock");
		break;

	    case offsetof (ITSCB, it_uep_lock):
		sprintf (area + 16, "uep_lock");
		break;

	    case offsetof (ITSCB, it_tep_lock):
		sprintf (area + 16, "tep_lock");
		break;

	    case offsetof (ITSCB, it_inq_lock):
		sprintf (area + 16, "inq_lock");
		break;

	    case offsetof (ITSCB, it_inq_nempty):
		sprintf (area + 16, "inq_nempty");
		break;

	    default:
		sprintf (area + 16, "+%d", index);
		break;
	}

	return (area);

}	/* end itscb_search */

/*
 ****************************************************************
 *	Procura o endereço nas filas NFS			*
 ****************************************************************
 */
char *
nfs_search (int addr)
{
	int		index;
	static char	area[32];

	area[16] = '\0';

 	if (addr < (int)nfs_inq || addr >= (int)end_nfs_inq)
		return (NOSTR);

	sprintf (area, "nfs_inq[%d]", (NFSINQ *)addr - nfs_inq);
	index = (addr - (int)nfs_inq) % sizeof (NFSINQ);

	/*
	 *	Procura a entrada da tabela
	 */
	switch (index)
	{
	    case offsetof (NFSINQ, nfs_inq_nempty):
		sprintf (area + 16, "inq_nempty");
		break;

	    default:
		sprintf (area + 16, "+%d", index);
		break;
	}

	return (area);

}	/* end nfs_search */

/*
 ****************************************************************
 *	Obtem o Nome do Tipo do Semaforo			*
 ****************************************************************
 */
char *
syncnmget (int tipo)
{
	switch (tipo)
	{
	    case E_NULL:
		return ("     ");

	    case E_SLEEP:
		return ("SLEEP");

	    case E_SEMA:
		return ("SEMA ");

	    case E_EVENT:
		return ("EVENT");
	}

	return ("???? ");

}	/* end syncnmget */

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
		"%s - imprime o estado da sincronização dos processos\n"
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
