/*
 ****************************************************************
 *								*
 *			etc.c					*
 *								*
 *	Rotinas auxiliares do núcleo				*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.7.0, de 09.12.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença" *   
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/bcb.h"
#include "../h/region.h"

#include "../h/mmu.h"
#include "../h/a.out.h"
#include "../h/inode.h"
#include "../h/sysdirent.h"
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
const char	*file_code_name_vec[] = FS_NAMES;

/*
 ****************************************************************
 *	Gera um DUMP do processo no diretório corrente		*
 ****************************************************************
 */
int
coredump (void)
{
#undef	COREDUMP
#ifdef	COREDUMP
	INODE		*ip;
	const REGIONL	*rlp;
	const PROC	*p;
	IOREQ		io;
	DIRENT		de;
#endif	COREDUMP

	u.u_error = NOERR;

#ifndef	COREDUMP
	return (-1);	/* sem coredump */
#else
	if ((ip = iname ("core", getschar, ICREATE, &io, &de)) == NOINODE)
	{
		if (u.u_error != NOERR)
			return (-1);

		if ((ip = (*io.io_iop->i_fsp->fs_make_inode) (&io, &de, 0666)) == NOINODE)
			return (-1);
	}

	if
	(	kaccess (ip, IWRITE) == 0 &&
		(ip->i_mode & IFMT) == IFREG &&
		u.u_euid == u.u_ruid
	)
	{
		(*io.io_iop->i_fsp->fs_trunc_inode) (ip);

		u.u_frame  = getuframe ();

	   /***	io.io_fd	= ...;	***/
	   /***	io.io_fp	= ...;	***/
		io.io_ip	= ip;
	   /***	io.io_dev	= ...;	***/
		io.io_area	= &u;
		io.io_count	= UPROCSZ;
		io.io_offset	= 0;
		io.io_cpflag	= SS;
	   /***	io.io_rablock	= ...;	***/

		(*ip->i_fsp->fs_write) (&io);

		/*
		 *	Coredump da região DATA
		 */
		p = u.u_proc;

		rlp = &p->p_data;

	   /***	io.io_fd	= ...;	***/
	   /***	io.io_fp	= ...;	***/
	   /***	io.io_ip	= ip;	***/
	   /***	io.io_dev	= ...;	***/
		io.io_area	= (void *)PGTOBY (rlp->rgl_vaddr);
		io.io_count	= PGTOBY (rlp->rgl_regiong->rgg_size);
	   /***	io.io_offset	= ...;	***/
		io.io_cpflag	= US;
	   /***	io.io_rablock	= ...;	***/
	
		(*ip->i_fsp->fs_write) (&io);

		/*
		 *	Coredump da região STACK
		 */
		rlp = &p->p_stack;

	   /***	io.io_fd	= ...;	***/
	   /***	io.io_fp	= ...;	***/
	   /***	io.io_ip	= ip;	***/
	   /***	io.io_dev	= ...;	***/
		io.io_area	= (void *)PGTOBY (rlp->rgl_vaddr);
		io.io_count	= PGTOBY (rlp->rgl_regiong->rgg_size);
	   /***	io.io_offset	= ...;	***/
	   /***	io.io_cpflag	= US;	***/
	   /***	io.io_rablock	= ...;	***/
	
		(*ip->i_fsp->fs_write) (&io);
	}

	iput (ip);

	return (u.u_error == NOERR ? 0 : -1);
#endif	COREDUMP

}	/* end coredump */

#ifdef	COREDUMP
/*
 ****************************************************************
 *	Obtem o "frame-pointer" do usuário (para "coredump")	*
 ****************************************************************
 */
void *
getuframe (void)
{
	typedef	struct frameb	FRAMEB;
	struct frameb {  FRAMEB	*fp;  void *ret; };
	FRAMEB			*p;

	for (p = (FRAMEB *)get_fp (); ((unsigned)p >> 24) == 0xE0; p = p->fp)
		/* vazio */;

	return ((void *)p);

}	/* end getuframe */
#endif	COREDUMP

/*
 ****************************************************************
 *	Fornece um caracter do Kernel				*
 ****************************************************************
 */
int
getschar (const char *cp)
{
	return (*cp);

}	/* end getschar */

/*
 ****************************************************************
 *	Fornece um caracter do usuário				*
 ****************************************************************
 */
int
getuchar (const char *cp)
{
	int	c;

	if ((c = gubyte (cp)) < 0)
		u.u_error = EFAULT;

	return (c);

}	/* end getuchar */

/*
 ****************************************************************
 *	Chamada inválida de driver				*
 ****************************************************************
 */
int
nodev (void)
{
	u.u_error = ENODEV;

	return (-1);

}	/* end nodev */

/*
 ****************************************************************
 *	Chamada sem efeito de driver				*
 ****************************************************************
 */
int
nulldev (void)
{
	return (0);

}	/* end nulldev */

/*
 ****************************************************************
 *	Chamada ao sistema inexistente				*
 ****************************************************************
 */
int
nosys (void)
{
	u.u_error = EINVAL;

	return (UNDEF);

}	/* end nosys */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Obtém o log (2) de um número inteiro 			*
 ****************************************************************
 */
int
log2 (int number)
{
	int		log, candidate;

	for (candidate = 1, log = 0; log < 31; candidate <<= 1, log += 1)
	{
		if (candidate == number)
			return (log);
	}

	return (-1);

}	/* end log2 */

/*
 ****************************************************************
 *	Chamada ao sistema ignorada				*
 ****************************************************************
 */
int
nullsys (void)
{
	return (UNDEF);

}	/* end nullsys */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Imprime o histórico de chamadas				*
 ****************************************************************
 */
void
print_calls (void)
{
	const struct frameb { struct frameb *fp;  unsigned long ret; } *p;
	const SYM	*sp;

	printf ("CALLs = ");

	for (p = get_fp (); /* abaixo */; p = p->fp)
	{
		if ((unsigned)p < UPROC_ADDR || (unsigned)p >= UPROC_ADDR + PGTOBY (USIZE))
			break;

		sp = getsyment (p->ret);

		if (streq (sp->s_name, "_panic") || streq (sp->s_name, "_excep") || streq (sp->s_name, "excep"))
			continue;

		if (sp != NOSYM)
			printf ("(%s, %P, %04X)  ", sp->s_name, p->ret, p->ret - sp->s_value);
		else
			printf ("%P  ", p->ret);
	}

	putchar ('\n');

}	/* end print_calls */

/*
 ****************************************************************
 *	Obtém uma entrada da tabela de símbolos do KERNEL	*
 ****************************************************************
 */
const SYM *
getsyment (unsigned long addr)
{
	const SYM	*sp, *endsymtb, *best_sp = NOSYM;
	unsigned long 	value, max_value = 0;
	static SYM	bad_sym;

	sp = (SYM *)&end; endsymtb = (SYM *)((char *)sp + bcb.y_ssize);

	for (/* acima */; sp < endsymtb; sp = SYM_NEXT_PTR (sp))
	{
		value = sp->s_value;

		if (value <= addr && value > max_value)
			{ max_value = value; best_sp = sp; }
	}

	if (best_sp != NOSYM)
		return (best_sp);
	else
		{ strcpy (bad_sym.s_name, "???"); return (&bad_sym); }

}	/* end getsyment */
