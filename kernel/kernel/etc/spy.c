/*
 ****************************************************************
 *								*
 *			spy.c					*
 *								*
 *	Fornece várias informações sobre o KERNEL		*
 *								*
 *	Versão	3.0.0, de 24.11.94				*
 *		4.6.0, de 06.09.04				*
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

#include "../h/map.h"
#include "../h/mmu.h"
#include "../h/kfile.h"
#include "../h/lockf.h"
#include "../h/inode.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
#include "../h/bhead.h"
#include "../h/tty.h"
#include "../h/timeout.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais 				*
 ****************************************************************
 */
const char	regiong_flag_bits[] = RGG_FLAG_BITS;	/* Em region.h */

#if (0)	/*******************************************************/
const char *region_type_nm[] =
{
	"???",
	"TEXT",		/* Região de texto compartilhado */
	"DATA",		/* Região de data (compart. entre threads) */
	"STACK",	/* Região de stack nunca compartilhada */
	"PHYS",		/* Região de PHYS */
	"SHMEM"		/* Região de memória compartilhada */
};
#endif	/*******************************************************/

extern	TTY	con[];			/* Estruturas do video */
extern	TTY	sio[];			/* Estruturas das seriais */
extern	TTY	slip[];			/* Estruturas das SLIP/seriais */
extern	TTY	ppp[];			/* Estruturas das PPP/seriais */

extern char	CSW;			/* Código da depuração (para ASM) */

/*
 ******	Protótipos de funções ***********************************
 */
long	spypid (UPROC *);
int	spystype (int);
void	spycalls (void);
void	spyproc (void);
void	spymproc (void);
void	spycore (void);
void	spydump (void);
void	spyprtty (TTY *);
void	spyaddr (void);
void	spyscb (void);
void	spysync (void);
void	spybhead (void);
void	spyinode (void);
void	spytext (void);
void	spysb (void);
void	spykfile (void);
void	spycorerdy (void);
void	spymmu (void);
void	spytty (void);
void	spydebug (void);
void	spylockf (void);
void	spydtox (void);
void	spyxtod (void);
void	spyregionl (void);
void	spyregiong (void);
void	spytimeout (void);
void	spymalloc (void);
void	spydmesg (void);

/*
 ****************************************************************
 *	Tabela das funções					*
 ****************************************************************
 */
typedef	struct
{
	char	name[8];
	void	(*func) (void);

}	SPYTB;

const SPYTB	spytb[]	=
{
	"calls",	spycalls,
	"proc",		spyproc,
	"mproc",	spymproc,
	"core",		spycore,
	"dump",		spydump,
	"addr",		spyaddr,
	"scb",		spyscb,
	"sync",		spysync,
	"bhead",	spybhead,
	"inode",	spyinode,
	"text",		spytext,
	"sb",		spysb,
	"kfile",	spykfile,
	"corerdy",	spycorerdy,
	"mmu",		spymmu,
	"tty",		spytty,
	"debug",	spydebug,
	"lockf",	spylockf,
	"dtox",		spydtox,
	"xtod",		spyxtod,
	"regionl",	spyregionl,
	"regiong",	spyregiong,
	"timeout",	spytimeout,
	"malloc",	spymalloc,
	"dmesg",	spydmesg,
	"????"
};

/*
 ****************************************************************
 *	Ponto de entrada do módulo				*
 ****************************************************************
 */
void
spyexcep (void)
{
	const SPYTB	*sp;
	int		pl, i;
	char		area[80];
	static char	busy;

	if (busy)
		return;
	busy++;

	pl = spl7 ();

	printf ("\nKGB zu Befehl!\n");

    prompt:
	printf ("\n-> ");

	gets (area);

	busy = 0;

	if (area[0] == CEOT)
		{ splx (pl); return; }

	if (area[0] == '\0')
		goto prompt;

	for (sp = spytb; sp->name[0] != '?'; sp++)
	{
		if (streq (sp->name, area))
			{ (*sp->func) (); goto prompt; }
	}

	printf ("I didn't quite understand your message!\n");
	printf ("\nYou have the Options:\n\n");

	for (sp = spytb, i = 0; sp->name[0] != '?'; sp++, i++)
	{
		printf ("\t%s", sp->name);

		if (i % 5 == 4)
			putchar ('\n');
	}

	putchar ('\n');

	goto prompt;

}	/* end spyexcep */

/*
 ****************************************************************
 *	Imprime a seqüência de chamadas				*
 ****************************************************************
 */
void
spycalls (void)
{
	print_calls ();

}	/* end spycalls */

/*
 ****************************************************************
 *	Imprime a tabela de processos				*
 ****************************************************************
 */
void
spyproc (void)
{
	const UPROCV	*uvp;
	const UPROC	*up;
	int		i;
	char		area[80];

	printf ("--ENTRY-- NO. STATE  FLG  PID PPID CHLD SBLG NEXT ");
	printf ("--ADDR--- SIZE S --SEMA---\n\n");

	for (i = 0, uvp = scb.y_uproc; uvp <= scb.y_enduproc; i++, uvp++)
	{
		if ((up = uvp->u_uproc) == NOUPROC)
			continue;

		if (up->u_state == SNULL)
			continue;

		printf
		(	"%P%4d %s %2x%5d%5d%5d%5d%5d %P%5d %c %P\n",
			up,
			i,
			proc_state_nm_edit (up->u_state),
			up->u_flags,
			up->u_pid,
			spypid (up->u_parent),
			spypid (up->u_child),
			spypid (up->u_sibling),
			spypid (up->u_next),
			up->u_myself,
			up->u_size,
			spystype (up->u_stype),
			up->u_sema
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}
	}

}	/* end spyproc */

/*
 ****************************************************************
 *	Imprime o Resto da tabela de processos			*
 ****************************************************************
 */
void
spymproc (void)
{
	const UPROCV	*uvp;
	const UPROC	*up;
	int		i;
	char		area[80];

	printf ("--ENTRY-- NO. STATE  FLG  PID  PRI TIME NICE -SIGNAL-- ");
	printf (" UID PGRP ALARM\n\n");

	for (i = 0, uvp = scb.y_uproc; uvp <= scb.y_enduproc; i++, uvp++)
	{
		if ((up = uvp->u_uproc) == NOUPROC)
			continue;

		if (up->u_state == SNULL)
			continue;

		printf
		(	"%P%4d %s %2x%5d%5d%5d%5d %P%5d%5d%6d\n",
			up,
			i,
			proc_state_nm_edit (up->u_state),
			up->u_flags,
			up->u_pid,
			up->u_pri,
			up->u_time,
			up->u_nice,
			up->u_sig,
			up->u_euid,
			up->u_pgrp,
			up->u_alarm
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}
	}

}	/* end spyproc */

/*
 ****************************************************************
 *	Obtem o PID						*
 ****************************************************************
 */
long
spypid (UPROC *up)
{
	if (up == NOUPROC)
		return (-1);
	else
		return (up->u_pid);

}	/* end spypid */

/*
 ****************************************************************
 *	Obtém o tipo do semáforo				*
 ****************************************************************
 */
int
spystype (int stype)
{
	switch (stype)
	{
	    case E_NULL:
		return (' ');

	    case E_SLEEP:
		return ('L');

	    case E_SEMA:
		return ('M');

	    case E_EVENT:
		return ('E');

	    default:
		return ('?');
	}

}	/* end spystype */

/*
 ****************************************************************
 *	Imprime a tabela de memória				*
 ****************************************************************
 */
void
spycore (void)
{
#define	map_core_head	scb.y_map[0] /* Cabeça da tabela da Memória */

	MAP	*mp, *head = &map_core_head;


	printf ("--------ADDR-----------  ---------SIZE--------\n\n");

	for (mp = head->a_forw; mp != head; mp = mp->a_forw)
	{
		printf
		(	"%8d Kb (%P): %6d Kb (%P)\n",
			PGTOKB (mp->a_area - ((unsigned)SYS_ADDR >> PGSHIFT)),
			mp->a_area,
			PGTOKB (mp->a_size), mp->a_size
		);
	}

}	/* end spycore */

/*
 ****************************************************************
 *	Imprime os endereços das tabelas			*
 ****************************************************************
 */
void
spyaddr (void)
{
	const SCB	*sp = &scb;

	printf ("PROC     %P  %P\n", sp->y_uproc, sp->y_enduproc);
	printf ("PHASH    %P  %P\n", sp->y_phash, sp->y_endphash);
	printf ("INODE    %P  %P\n", sp->y_inode, sp->y_endinode);
	printf ("IHASH    %P  %P\n", sp->y_ihash, sp->y_endihash);
	printf ("BUF0     %P\n",     sp->y_buf0);
	printf ("BUF1     %P\n",     sp->y_buf1);
	printf ("BHEAD    %P  %P\n", sp->y_bhead, sp->y_endbhead);
	printf ("BHASH    %P  %P\n", sp->y_bhash, sp->y_endbhash);
	printf ("KFILE    %P  %P\n", sp->y_kfile, sp->y_endkfile);
	printf ("CLIST    %P  %P\n", sp->y_clist, sp->y_endclist);
	printf ("MAP      %P  %P\n", sp->y_map,    sp->y_endmap);
	printf ("TIMEOUT  %P  %P\n", sp->y_timeout, sp->y_endtimeout);

}	/* end spyaddr */

/*
 ****************************************************************
 *	Dump de memória virtual					*
 ****************************************************************
 */
void
spydump (void)
{
	mem_dump ();

}	/* end spydump */

/*
 ****************************************************************
 *	Impressão do SCB					*
 ****************************************************************
 */
void
spyscb (void)
{
	SCB		*sp;

	sp = &scb;

	printf ("ncpu\t= %d\n", sp->y_ncpu);
	printf ("initpid\t= %d\n", sp->y_initpid);
	printf ("nblkdev\t= %d\n", sp->y_nblkdev);
	printf ("nchrdev\t= %d\n", sp->y_nchrdev);

	printf ("tb0\t= %P, %P\n", sp->y_tb0, sp->y_endtb0);
	printf ("tb1\t= %P, %P\n", sp->y_tb1, sp->y_endtb1);

	printf ("ucore0\t= %d, %d\n", sp->y_ucore0, sp->y_enducore0);
	printf ("ucore1\t= %d, %d\n", sp->y_ucore1, sp->y_enducore1);

}	/* end spyscb */

/*
 ****************************************************************
 *	Imprime a tabela de terminais				*
 ****************************************************************
 */
void
spytty (void)
{
	TTY		*tp;

	printf ("OL OB NE NF EM  IN OUT   DEV STATE DCT COL WCL LN ");
	printf ("IFLG OFLG CFLG LFLG  TGRP\n\n");

	for (tp = con; tp < &con[8];  tp++)
		spyprtty (tp);

	for (tp = sio; tp < &sio[8 * 2];  tp++)
		spyprtty (tp);

	for (tp = slip; tp < &slip[8];  tp++)
		spyprtty (tp);

	for (tp = ppp; tp < &ppp[8];  tp++)
		spyprtty (tp);

}	/* end spytty */

void
spyprtty (TTY *tp)
{
	if (tp->t_state == 0)
		return;

	printf
	(
 	  "%2x %2x %2x %2x %2x%4d%4d  %4x  %4x%4d%4d%4d%3d %4x %4x %4x %4x %5d\n",
		tp->t_olock,		tp->t_obusy,
		tp->t_inqnempty,	tp->t_outqnfull,
		tp->t_outqempty,	tp->t_inq.c_count,
		tp->t_outq.c_count,	tp->t_dev,
		tp->t_state,		tp->t_delimcnt,
		tp->t_col,		tp->t_wcol,
		tp->t_lno,
		tp->t_iflag,		tp->t_oflag,
		tp->t_cflag,		tp->t_lflag,
		tp->t_tgrp
	);

}	/* end spyprtty */

/*
 ****************************************************************
 *	Imprime o histórico de sincronismo			*
 ****************************************************************
 */
void
spysync (void)
{
	const SYNCH	*sh;

	printf (" TIPO S    SEMA\n\n");

	for (sh = &u.u_synch[0]; sh < &u.u_synch[NHIST]; sh++)
	{
		if (sh->e_type == E_NULL)
			continue;

		printf ("%s", syncnmget (sh->e_type) );

		switch (sh->e_state)
		{
		    case E_GOT:
			printf (" G ");
			break;

		    case E_WAIT:
			printf (" W ");
			break;

		    default:
			printf (" ? ");
		}

		printf (" %P\n", sh->e_sema);

	}	/* end for */

	printf ("\nHIST SYNC end\n");

}	/* end spysync */

/*
 ****************************************************************
 *	Imprime a Tabela de BHEADs				*
 ****************************************************************
 */
void
spybhead (void)
{
	BHEAD		*bp;
	int		i;
	char		area[80];

	printf ("---ADDR-- LK DN FLGS -DEV-  BLKNO -VI ADDR- -PH ADDR- BCOUNT ERR\n\n");

	for (i = 0, bp = scb.y_bhead; bp < scb.y_endbhead; i++, bp++)
	{
		if (bp->b_dev == NODEV && bp->b_lock == 0)
			continue;

		printf
		(	"%P %2X %2X %4X %5v %6d %P %6d %3d\n",
			bp,
			bp->b_lock,
			bp->b_done,
			bp->b_flags,
			bp->b_dev,
			bp->b_blkno,
			bp->b_addr,
			bp->b_base_count,
			bp->b_error
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}

	}	/* end for */

}	/* end spybhead */

/*
 ****************************************************************
 *	Imprime a tabela de INODEs				*
 ****************************************************************
 */
void
spyinode (void)
{
	INODE		*ip;
	int		i;
	char		area[80];

	printf ("--ADDR--- LK CT ----SB--- FLAG --MODE--- -DEV- INO NL ");
	printf ("-SIZE- ----ADDR---\n\n");

	for (i = 0, ip = scb.y_inode; ip < scb.y_endinode; i++, ip++)
	{
		if (ip->i_mode == 0)
			continue;

		printf
		(	"%P %2X %2d %P %4X %P %5v %3d %2d %6d %5d %5d\n",
			ip,
			ip->i_lock,
			ip->i_count,
			ip->i_sb,
			ip->i_flags,
			ip->i_mode,
			ip->i_dev,
			ip->i_ino,
			ip->i_nlink,
			ip->i_size,
			ip->i_addr[0],
			ip->i_addr[1]
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}

	}	/* end for */

}	/* end spyinode */

/*
 ****************************************************************
 *	Imprime a tabela de INODEs, parte do TEXT		*
 ****************************************************************
 */
void
spytext (void)
{
	INODE		*ip;
	int		i;
	char		area[80];

	printf ("--ENTRY-- LK CT FLAG INO  CT DADDR --CADDR-- XSIZE\n\n");

	for (i = 0, ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_mode == 0 || (ip->i_flags & ITEXT) == 0)
			continue;

		printf
		(	"%P %2x %2d %4x %3d %3d %5d %P %5d\n",
			ip,
			ip->i_lock,
			ip->i_count,
			ip->i_flags,
			ip->i_ino,
			ip->i_xcount,
			ip->i_xdaddr,
			ip->i_xheader,
			ip->i_xsize
		);

		if (++i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}

	}	/* end for */

}	/* end spytext */

/*
 ****************************************************************
 *	Imprime os SUPERBLOCOs 					*
 ****************************************************************
 */
void
spysb (void)
{
	SB		*sp;
	V7SB		*v7sp;
	int		i = 0;
	char		area[80];

	printf ("---ADDR-- FLK ILK MOD FL MLK MBY MCT -DEV- ");
	printf ("--INODE-- ---NEXT-- --BHEAD--\n\n");

	for (sp = sb_head.sb_forw; sp != &sb_head; sp = sp->sb_forw)
	{
		v7sp = sp->sb_ptr;

		printf
		(	"%P  %2X  %2X  %2X %2X  %2X  %2X %3d %5v %P\n",
			v7sp,
			v7sp->s_flock,
			v7sp->s_ilock,
			sp->sb_sbmod,
			sp->sb_mntent.me_flags,
			sp->sb_mlock,
			sp->sb_mbusy,
			sp->sb_mcount,
			sp->sb_dev,
			sp->sb_inode
		);

		if (++i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}

	}	/* end for */

}	/* end spysb */

/*
 ****************************************************************
 *	Imprime a tabela de KFILEs				*
 ****************************************************************
 */
void
spykfile (void)
{
	KFILE		*fp;
	int		i;
	char		area[80];

	printf ("--ENTRY-- LK CT FLGS --INODE--  OFFSET\n\n");

	for (i = 0, fp = scb.y_kfile; fp < scb.y_endkfile; i++, fp++)
	{
		if (fp->f_count == 0)
			continue;

		printf
		(	"%P %2x %2d %4x %P %7d\n",
			fp,
			fp->f_lock,
			fp->f_count,
			fp->f_flags,
			fp->f_inode,
			fp->f_offset_low
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}

	}	/* end for */

}	/* end spykfile */

/*
 ****************************************************************
 *	Imprime a lista CORERDY					*
 ****************************************************************
 */
void
spycorerdy (void)
{
	UPROC		*up;

	for (up = corerdylist; up != NOUPROC; up = up->u_next)
	{
		printf ("%d ", up->u_pid);
	}

	printf ("\n\nCORERDY end\n");

}	/* end spycorerdy */

/*
 ****************************************************************
 *	Imprime os registros da MMU				*
 ****************************************************************
 */
void
spymmu (void)
{
	int		i, j;
	mmu_t		*dirp, *pgp;
	mmu_t		pge;
	pg_t		vaddr;
	int		l = 0;
	char		area[80];

	printf ("-----VIRTUAL ADDR------  -PHY ADDR- U/K RW\n\n");

	dirp = kernel_page_directory;	vaddr = 0;

	for
	(	i = 0;
		i < ((unsigned)SYS_ADDR >> (PGSHIFT+PGSHIFT-MMUSHIFT));
		i++, dirp++
	)
	{
		if ((*dirp & 0x01) == 0)
			{ vaddr += (PGSZ >> MMUSHIFT); continue; }

		pgp = (mmu_t *)((*dirp & ~PGMASK) + SYS_ADDR);

		for (j = 0; j < (PGSZ >> MMUSHIFT); j++, pgp++, vaddr++)
		{
			if (((pge = *pgp) & 0x01) == 0)
				continue;

			printf
			(	"%8d Kb (%P):   %5d Kb  %c  %2s   (%P)\n",
				PGTOKB (vaddr), PGTOBY (vaddr),
				PGTOKB (pge >> PGSHIFT),
				pge & 0x04 ? 'U'  : 'K',
				pge & 0x02 ? "RW" : "RO",
				pge
			);

			if (l++ % 5 == 4)
			{
				gets (area);

				if (area[0] == CEOT)
					return;
			}
		}
	}

}	/* end spymmu */

/*
 ****************************************************************
 *	Atribui um valor à variável de DEBUG			*
 ****************************************************************
 */
void
spydebug (void)
{
	char		area[80];

    again:
	printf ("\nDEBUG (%d) = ", CSW); gets (area);

	if (area[0] == '?')
	{
		printf
		(	"\n"
			"\t 0:	DEBUG desligado\n"
			"\t 1:	Sincronismo\n"
			"\t 2:	Chamadas ao sistema\n"
			"\t 3:	Informações sobre ADDR & BUS ERRORs\n"
			"\t 4:	Readahead\n"
			"\t 5:	Atividade de SWAP\n"
			"\t 6:	Update, bflush\n"
			"\t 7:	Open, creat, close, ...\n"
			"\t 8:	Obsoleto (iname, ioctl, ...)\n"
			"\t 9:	Desliga synccheck\n"
		);

		gets (area);

		printf
		(	"\t10:	Discos IDE\n"
			"\t11:	Terminais PEGASUS (ti)\n"
			"\t12:	Iget, iput, ...\n"
			"\t13:	Residual de entrada/saida\n"
			"\t14:	Deadlocks evitatos\n"
			"\t15:	Sinais\n"
			"\t16:	Area de data de um text lido da memoria\n"
			"\t17:	Atividade de timeout\n"
			"\t18:	Read/write\n"
			"\t19:	Lockf\n"
		);

		gets (area);

		printf
		(	"\t20:	Trace\n"
			"\t21:	Aumento do processo na parte inferior\n"
			"\t22:	Disquete\n"
			"\t23:	TCP/IP\n"
			"\t24:	IPC (eventos, semaforos e shmem)\n"
			"\t25:	Regioes de Memoria\n"
			"\t26:	Preemption do Kernel (escreve @)\n"
			"\t27:	Fast Read: escreve os segmentos lidos\n"
			"\t28:	Exec\n"
			"\t29:	Ethernet\n"
		);

		gets (area);

		printf
		(	"\t30:	PPP\n"
			"\t31:	Physio\n"
			"\t32:	Zip\n"
			"\t33:	DNS\n"
			"\t34:	Regiões grandes\n"
			"\t35:	Serviço de GATEWAY\n"
			"\t36:	Datagramas de BROADCAST\n"
			"\t37:	Tag queueing\n"
			"\t38:	Malloc\n"
			"\t39:	USB\n"
			"\n"
		);

		goto again;
	}

	if (area[0] != '\0')
		scb.y_CSW = CSW = strtol (area, (const char **)NULL, 0);

	printf ("\nDEBUG = %d\n", scb.y_CSW);

}	/* end spydebug */

/*
 ****************************************************************
 *	Imprime a tabela de LOCKF				*
 ****************************************************************
 */
void
spylockf (void)
{
	INODE		*ip;
	LOCKF		*lp;
	int		i;
	extern LOCKF	*lffreelist;

	for (i = 0, lp = lffreelist; lp != NOLOCKF; lp = lp->k_next)
		i++;

	printf
	(	"\n%d blocos ocupados de um total de %d\n",
		scb.y_nlockf - i,
		scb.y_nlockf
	);

	for (ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_mode == 0 || ip->i_lockf == NOLOCKF)
			continue;

		printf ("\nTabela de LOCKFs do arquivo (%v, %d)\n\n", ip->i_dev, ip->i_ino);

		printf (" BEGIN    END    PID PGNM\n\n");

		for (lp = ip->i_lockf; lp != NOLOCKF; lp = lp->k_next)
		{
			printf
			(	"%6d %6d %6d %s\n",
				lp->k_begin,
				lp->k_end,
				lp->k_uproc->u_pid,
				lp->k_uproc->u_pgname
			);
		}
	}

}	/* end spylockf */

/*
 ****************************************************************
 *	Conversão da Base 10 para 16 				*
 ****************************************************************
 */
void
spydtox (void)
{
	int		n;
	char		area[80];

	for (EVER)
	{
		printf ("N(10) = ");

		gets (area);

		if (area[0] == CEOT)
			return;

		n = strtol (area, (const char **)NULL, 10);

		printf ("N(16) = %X\n", n);
	}

}	/* end spydtox */

/*
 ****************************************************************
 *	Conversão da Base 16 para 10 				*
 ****************************************************************
 */
void
spyxtod (void)
{
	int		n;
	char		area[80];

	for (EVER)
	{
		printf ("N(16) = ");

		gets (area);

		if (area[0] == CEOT)
			return;

		n = strtol (area, (const char **)NULL, 16);

		printf ("N(10) = %d\n", n);
	}

}	/* end spydtox */

/*
 ****************************************************************
 *	Imprime a tabela de regiões locais			*
 ****************************************************************
 */
void
spyregionl (void)
{
	REGIONL		*p;
	int		i, j;
	char		area[80];

	printf ("--ENTRY--  NO.  -GREGION-  -VI ADDR-  PROT\n\n");

	for (i = j = 0, p = scb.y_regionl; p < scb.y_endregionl; i++, p++)
	{
		if (p->rgl_regiong == NOREGIONG)
			continue;

		printf
		(	"%P  %3d  %P  %P  %4X\n",
			p,
			i,
			p->rgl_regiong,
			PGTOBY (p->rgl_vaddr),
			p->rgl_prot
		);

		if (j++ % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}
	}

}	/* end spyregionl */

/*
 ****************************************************************
 *	Imprime Tabela de Regiões globais			*
 ****************************************************************
 */
void
spyregiong (void)
{
	REGIONG		*p;
	int		i;
	char		area[80];

	printf ("--ENTRY--  NO.  SIZE  -RE ADDR-  CNT/ID  INODE/NEXT  TYPE, FLAGS\n\n");

	for (i = 0, p = scb.y_regiong; p < scb.y_endregiong; i++, p++)
	{
		if (p->rgg_paddr == 0)
			continue;

		printf
		(	"%P  %3d  %4d  %P   %4d    %s, %b\n",
			p,
			i,
			p->rgg_size,
			PGTOBY (p->rgg_paddr),
			p->rgg_count,
			region_nm_edit (p->rgg_type),
			p->rgg_flags, regiong_flag_bits
		);

		if (i % 5 == 4)
		{
			gets (area);

			if (area[0] == CEOT)
				return;
		}
	}

}	/* end spyregiong */

/*
 ****************************************************************
 *	Imprime a fila HASH do TIMEOUT				*
 ****************************************************************
 */
void
spytimeout (void)
{
	const TIMEOUT_HASH	*hp, *last_hp;
	extern TIMEOUT_HASH	timeout_hash[];
	extern int		timeout_busy_count;

	printf ("busy = %d\n\n", timeout_busy_count);

	last_hp = &timeout_hash[TIMEOUT_HASH_SZ];

	for (hp = timeout_hash; hp < last_hp; hp++)
		printf ("%d ", hp->o_count);

#if (0)	/*******************************************************/
	printf ("\n\n");

	for (hp = timeout_hash; hp < last_hp; hp++)
	{
		for (n = 0, tp = hp->o_first; tp != NOTIMEOUT; tp = tp->o_next)
				n++;

		printf ("%d ", hp->o_count);
	}
#endif	/*******************************************************/

	printf ("\n");

}	/* end spytimeout */

/*
 ****************************************************************
 *	Imprime a ARENA de "malloc_byte"			*
 ****************************************************************
 */
void
spymalloc (void)
{
	check_arena ("spy", 1 /* imprime */);

}	/* end spymalloc */

/*
 ****************************************************************
 *	Imprime a área de DMESG					*
 ****************************************************************
 */
void
spydmesg (void)
{
	const char	*cp;
	int		c, nchar = 0, nline = 0;

	for (cp = dmesg_area; cp < dmesg_ptr; cp++)
	{
		if ((c = *cp) == '\n')
		{
			writechar ('\r');
			writechar ('\n');

			nchar = 0; nline++;
		}
		else
		{
			writechar (c);

			if (nchar++ >= 80)
				{ nchar = 0; nline++; }
		}

		if (nline >= 20)
			{ getchar (); nline = 0; nchar = 0; }
	}

}	/* end spydmesg */
