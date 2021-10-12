/*
 ****************************************************************
 *								*
 *			sysexec.c				*
 *								*
 *	Chamadas ao sistema: "exece"				*
 *								*
 *	Versão	3.0.0, de 11.12.94				*
 *		4.6.0, de 03.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/uarg.h"
#include "../h/map.h"
#include "../h/mmu.h"
#include "../h/frame.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/a.out.h"
#include "../h/sysdirent.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
INODE		*execfget (char *, int, INODE *, IOREQ *, DIRENT *);
HEADER		*exec_read_header (INODE *ip, IOREQ *iop);

/*
 ****************************************************************
 *	Chamada ao sistema "exece"				*
 ****************************************************************
 */
int
exece (char *path, char *argp[], char *envp[])
{
	KFILE		*fp;
	const HEADER	*hp;
	int		nc, cnt;
	char		*area, *cp, *ap;
	const char	*end_area;
	UARG		*up;
	char		*ucp;
	void		(**spp) (int, ...);
	int		na, ne;
	char		**pp;
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Em primeiro lugar, obtém o arquivo
	 */
	if ((ip = iname (path, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

#ifdef	MSG
	if (CSWT (25))
		printf ("%g: Dando EXECE no INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
#endif	MSG

	/*
	 *	Verifica se é permitido executar programas deste sistema de arquivos
	 */
	if ((ip->i_sb->sb_mntent.me_flags & SB_EXEC) == 0)
		{ u.u_error = ENOEXEC; iput (ip); return (-1); }

	/*
	 *	Aloca uma área da memória para copiar os argumentos;
	 *	repare que usa "malloce", do final da memória
	 */
	if ((u.u_xargs = malloce (M_CORE, BYUPPG (scb.y_ncexec))) == NOPG)
		{ u.u_error = ENOMEM; iput (ip); return (-1); }

	/*
	 *	Copia os argumentos e ambientes do processo para a área
	 */
	cp = area = (char *)PGTOBY (u.u_xargs); end_area = area + scb.y_ncexec;

	na = 0;		/* No. de ARGs + ENVs */
	ne = 0;		/* No. de ENVs */

	for (EVER)
	{
		/*
		 *	Tenta obter um ponteiro para um ARG ou ENV
		 */
		ap = NOSTR;

		if (argp != NOSTRP)
			ap = (char *)gulong (argp++);

		if (ap == NOSTR && envp != NOSTRP)
		{
			argp = NOSTRP;

			if ((ap = (char *)gulong (envp++)) == NOSTR)
				break;

			ne++;
		}

		if ((int)ap == -1)
			{ u.u_error = EFAULT; goto bad; }

		if (ap == NOSTR)
			break;

		/*
		 *	Obteve um ARG ou ENV; copia para a área alocada
		 */
		na++;

		if ((cnt = get_user_str (cp, ap, end_area - cp)) < 0)
			{ u.u_error = EFAULT; goto bad; }

		if ((cp += cnt + 1) >= end_area - 1)
			{ u.u_error = E2BIG; goto bad; }

	}	/* end for (EVER) */


	nc = (cp - area + sizeof (int) - 1) & ~(sizeof (int) - 1);

	/*
	 *	Processa o Programa a ser executado
	 */
	if ((ip = execfget (path, nc, ip, &io, &de)) == NOINODE || u.u_error != NOERR)
		goto bad;

	/*
	 *	Prepara a STACK inicial do processo, com os ARGs e ENVs
	 */
	load_cr3 ();	/* No código abaixo, uso a gerência do novo programa */

	ucp = (char *)PGTOBY (USER_STACK_PGA) - sizeof (UARG);

	up = (UARG *)ucp;

	ucp -= nc;	/* Início das cadeias */

	/*
	 *	Preeenche a estrutura UARG (exceto "envp")
	 */
	up->g_null = NULL;
	up->g_argc = na - ne;
	up->g_argp = ucp;
	up->g_envc = ne;

	/*
	 *	Deixa espaco para "na" ponteiros (os ARGPs e ENVPs)
	 *	e mais 3 longos: argc, NOSTR entre os ARGPs e
	 *	ENVPs e um NOSTR entre os ARGPs e as cadeias
	 */
	pp = (char **)ucp - (na + 3);

	u.u_frame->s.sf_usp = pp;

	*pp++ = (char *)(na - ne);


	/*
	 *	Copia as cadeias de volta
	 */
	cp = (char *)PGTOBY (u.u_xargs);

	for (EVER)
	{
		if (na == ne)
		{
			*pp++ = NOSTR;

			/*
			 *	Completa UARG
			 */
			if (ne > 0)
				up->g_envp = ucp;
			else
				up->g_envp = NOSTR;
		}

		if (--na < 0)
			break;

		*pp++ = ucp;

		do
			/* vazio */;
		while ((*ucp++ = *cp++) != '\0');
	}

	*pp = NOSTR;

	/*
	 *	Prepara Recursos do Novo Programa
	 */
	for (spp = &u.u_signal[0]; spp <= &u.u_signal[NSIG]; spp++)
	{
		if (*spp != SIG_IGN)
			*spp = SIG_DFL;
	}

	hp = u.u_inode->i_xheader;

	u.u_frame->s.sf_pc = (void *)hp->h_entry;
	u.u_frame->s.sf_fp = 0;

	/*
	 *	Zera os tempos de estimativa de processamento paralelo
	 */
	u.u_seqtime = 0;
	u.u_cseqtime = 0;
	u.u_partime = 0;
	u.u_cpartime = 0;

	/*
	 *	Fecha os arquivos que não devem atravessar "execs"
	 */
	for (nc = 0; nc < NUFILE; nc++)
	{
		if ((fp = u.u_ofile[nc]) == NOKFILE)
			continue;

		if (fp->f_union == KF_SHMEM)
		{
			fclose (fp);
			u.u_ofile[nc] = NOKFILE;
		}
		elif (u.u_ofilef[nc] & EXCLOSE)
		{
			fclose (fp);
			u.u_ofile[nc] = NOKFILE;
			u.u_ofilef[nc] &= ~EXCLOSE;
		}
	}

	/*
	 *	Indica que este processo com código novo ainda não usou a FPU;
	 *	a FPU só será inicializada quando/se o processo a utilizar
	 */
	u.u_fpu_used = 0;

	if (u.u_fpu_lately_used)
		{ u.u_fpu_lately_used = 0; fpu_set_embit (); }

	/*
	 *	Guarda o nome do programa em execucão
	 */
	strcpy (u.u_pgname, u.u_name);

	/*
	 *	Libera a memória alocada
	 */
    bad:
	if (ip != NOINODE)
		iput (ip);

	mrelease (M_CORE, BYUPPG (scb.y_ncexec), u.u_xargs);
	u.u_xargs = 0;

	return (UNDEF);

}	/* end exece */

/*
 ****************************************************************
 *	Obtem o Programa a ser Executado			*
 ****************************************************************
 */
INODE *
execfget (char *path, int nc, INODE *ip, IOREQ *iop, DIRENT *dep)
{
	pg_t		ts, ds, ss;
	HEADER		*hp = NOHEADER;
	INODE		*oip;
	REGIONL		*rlp, *rlp_t, *rlp_d, *rlp_s;
	const REGIONL	*end_rlp;
	REGIONG		*rgp;
	pg_t		taddr, daddr, saddr;
	int		index;
	unsigned	flag;

	/*
	 *	Analisa o acesso e o tipo do arquivo
	 */
    again:
	if (kaccess (ip, IEXEC) < 0)
		goto bad;

	if ((ip->i_mode & IFMT) != IFREG)
		{ u.u_error = EACCES; goto bad; }

	if ((ip->i_mode & (IEXEC|(IEXEC>>3)|(IEXEC>>6))) == 0)
		{ u.u_error = EACCES; goto bad; }

	/*
	 *	Obtém o cabeçalho do Programa
	 */
	if ((hp = exec_read_header (ip, iop)) == NOHEADER)
		goto bad;

	/*
	 *	Se for programa NÃO reentrante, NÃO executa
	 */
	if (hp->h_magic != 0410)
		{ u.u_error = ENOEXEC; goto bad; }

	/*
	 *	Verifica se o arquivo a ser executado por acaso não é
	 *	um TEXT que outro processo está lendo/escrevendo
	 */
	if (hp->h_tsize > 0  && (ip->i_flags & ITEXT) == 0  && ip->i_count != 1)
		{ u.u_error = ETXTBSY; goto bad; }

	/*
	 *	Calcula o tamanho das 3 áreas, e verifica se são "razoáveis"
	 */
	ts = BYUPPG (hp->h_tsize);
	ds = BYUPPG (hp->h_dsize + hp->h_bsize);
	ss = BYUPPG (nc + scb.y_stksz);

	if (mmutst (ts, ds, ss) < 0)
		goto bad;

	/*
	 *	Obtém os endereços virtuais de cada região padrão
	 */
	saddr = USER_STACK_PGA - ss;
	taddr = BYTOPG (hp->h_tstart);
	daddr = BYTOPG (hp->h_dstart);

	/*
	 *	Se o arquivo antigo tem TEXT, tenta liberá-lo; isto pode
	 *	ocasionar um DEADLOCK. Se não conseguiu, libera todos os
	 *	recursos, e tenta mais tarde de novo
	 */
	if ((oip = u.u_inode) != NOINODE)
	{
		if (oip != ip)
		{
			if (SLEEPTEST (&oip->i_lock) < 0)
			{
#ifdef	MSG
				if (CSWT (14))
					printf ("%g: Evitado um Deadlock!\n");
#endif	MSG
				iput (ip);

				SLEEPLOCK (&oip->i_lock, PINODE);
				SLEEPFREE (&oip->i_lock);

				if ((ip = iname (path, getuchar, ISEARCH, iop, dep)) == NOINODE)
					return (NOINODE);

				goto again;
			}

			if (xrelease (oip) > 0)
				iput (oip);
			else
				SLEEPFREE (&oip->i_lock);
		}
		else
		{
			if (xrelease (oip) > 0)
				idec (oip);

			if ((hp = exec_read_header (ip, iop)) == NOHEADER)
				goto bad;
		}

		u.u_inode = NOINODE;
	}

	/*
	 *	Desaloca os PHYS
	 */
	if (u.u_phn) for (rlp = u.u_phys_region, end_rlp = rlp + PHYSNO; rlp < end_rlp; rlp++)
		regionrelease (rlp);

	u.u_phn = 0;

	/*
	 *	Aloca a região STACK para a imagem nova
	 */
#ifdef	MSG
	if (CSWT (25))
		printf ("%g: Alocando região STACK tam. %d no end. %P\n", ss, saddr);
#endif	MSG

	rlp_s = &u.u_stack;

	if (rlp_s->rgl_regiong != NOREGIONG && rlp_s->rgl_regiong->rgg_count > 1)
		printf ("%g: STACK compartilhada?\n");

	if (regiongrow (rlp_s, saddr, ss, RG_CLR) < 0)
		{ printf ("%g: Não consegui região STACK\n"); goto nomem; }

	if ((rgp = rlp_s->rgl_regiong) != NOREGIONG)
		strcpy (rgp->rgg_name, u.u_name);

	/*
	 *	Aloca a região DATA para a imagem nova
	 */
	if (CSWT (25))
		printf ("%g: Alocando região DATA tam. %d no end. %P\n", ds, daddr);

	rlp_d = &u.u_data;

	if (rlp_d->rgl_regiong != NOREGIONG && rlp_d->rgl_regiong->rgg_count > 1)
		regionrelease (rlp_d);

	rlp_d->rgl_type    = RG_DATA;
	rlp_d->rgl_prot    = URW;

   /***	rlp_d->rgl_vaddr   = ...;	/* abaixo ***/
   /***	rlp_d->rgl_regiong = ...;	/* abaixo ***/

	if (regiongrow (rlp_d, daddr, ds, RG_CLR) < 0)
		{ printf ("%g: Não consegui região DATA\n"); goto nomem; }

	if ((rgp = rlp_d->rgl_regiong) != NOREGIONG)
		strcpy (rgp->rgg_name, u.u_name);

	/*
	 *	Aloca uma REGIONL para TEXT ainda RW
	 */
#ifdef	MSG
	if (CSWT (25))
		printf ("%g: Alocando região TEXT tam. %d no end. %P\n", ts, taddr);
#endif	MSG

	rlp_t = &u.u_text; rlp_t->rgl_vaddr = taddr;

	if (xalloc (ip) < 0)
		{ printf ("%g: Não consegui alocar e/ou carregar região TEXT\n"); goto nomem; }

	/*
	 *	Carrega a região DATA (direto da memória)
	 */
#ifdef	MSG
	if (CSWT (16))
		printf ("%g: Lendo DATA da memória\n");
#endif	MSG

	if (ds) memmove
	(
		(void *)PGTOBY (rlp_d->rgl_regiong->rgg_paddr),
		(void *)PGTOBY (rlp_t->rgl_regiong->rgg_paddr) + hp->h_tsize,
		hp->h_dsize
	);

	/*
	 *	Analisa as bibliotecas compartilhadas
	 */
	flag = 1;

	for (index = 0; index < NSHLIB; index++, flag <<= 1)
	{
		/* hp->h_flags foi preenchido por "xalloc" */

		if (flag & hp->h_flags)
		{
			if (shlib_attach (index) < 0)
				goto bad;
		}
		else
		{
			regionrelease (&u.u_shlib_text[index]);
			regionrelease (&u.u_shlib_data[index]);
		}
	}

	/*
	 *	Processa os Programas com "setuid" ou "setgid" ou TRACE
	 */
	if   (u.u_flags & STRACE)
	{
		sigproc (u.u_myself, SIGTRAP);
	}
	elif (ip->i_sb->sb_mntent.me_flags & SB_SUID)
	{
		if (ip->i_mode & ISUID  &&  u.u_euid != 0)
			u.u_euid = ip->i_uid;

		if (ip->i_mode & ISGID)
			u.u_egid = ip->i_gid;
	}

	/*
	 *	Armazena os tamanhos das 3 áreas
	 */
	u.u_tsize = ts;
	u.u_dsize = ds;
	u.u_ssize = ss;

	return (ip);

	/*
	 *	Em caso de erro, ...
	 */
    nomem:
	u.u_error = ENOMEM;

	if (hp)
		free_byte (hp);
    bad:
	iput (ip);

	return (NOINODE);

}	/* end execfget */

/*
 ****************************************************************
 *	Lê o cabeçalho do programa				*
 ****************************************************************
 */
HEADER *
exec_read_header (INODE *ip, IOREQ *iop)
{
	HEADER		*hp;

	/*
	 *	Obtém o cabeçalho do Programa
	 */
	if ((hp = ip->i_xheader) == NOHEADER)
	{
		if ((hp = malloc_byte (sizeof (HEADER))) == NOHEADER)
			{ u.u_error = ENOMEM; return (NOHEADER); }

		ip->i_xheader = hp;

	   /***	iop->io_fd	 = ...;	***/
	   /***	iop->io_fp	 = ...;	***/
		iop->io_ip	 = ip;
	   /***	iop->io_dev	 = ...;	***/
		iop->io_area	 = hp;
		iop->io_count	 = sizeof (HEADER);
		iop->io_offset_low = 0;
		iop->io_cpflag	 = SS;
	   /***	iop->io_rablock	 = ...;	***/

		(*ip->i_fsp->fs_read) (iop);

		if (u.u_error != NOERR)
			return (NOHEADER);

		if (iop->io_count > 0)
			{ u.u_error = ENOEXEC; return (NOHEADER); }
	}

	return (hp);

}	/* end exec_read_header */
