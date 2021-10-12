/*
 ****************************************************************
 *								*
 *			sysctl.c				*
 *								*
 *	Chamadas ao sistema: controles				*
 *								*
 *	Versão	3.0.0, de 19.12.94				*
 *		4.8.0, de 23.10.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/bcb.h"
#include "../h/region.h"

#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/mmu.h"
#include "../h/frame.h"
#include "../h/a.out.h"
#include "../h/kcntl.h"
#include "../h/pcntl.h"
#include "../h/tty.h"
#include "../h/disktb.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Tabela com os nomes dos erros do KERNEL			*
 ****************************************************************
 */
entry const char	* const sys_errlist[] =
{
	/* 00: NOERR		*/	"Erro ZERO",
	/* 01: EPERM		*/	"Não é o dono ou superusuário",
	/* 02: ENOENT		*/	"Nome de arquivo desconhecido",
	/* 03: ESRCH		*/	"Número de processo desconhecido",
	/* 04: EINTR		*/	"Chamada ao sistema interrompida",
	/* 05: EIO		*/	"Erro de entrada/saída",
	/* 06: ENXIO		*/	"Dispositivo desconhecido ou endereço fora dos limites",
	/* 07: E2BIG		*/	"Lista de argumentos longa demais",
	/* 08: ENOEXEC		*/	"Arquivo não-executável",
	/* 09: EBADF		*/	"Descritor de arquivo inválido",
	/* 10: ECHILD		*/	"O Processo não possui filhos",
	/* 11: EAGAIN		*/	"Recurso não disponível, tente mais tarde",
	/* 12: ENOMEM		*/	"Memória insuficiente",
	/* 13: EACCES		*/	"Permissão negada",
	/* 14: EFAULT		*/	"Endereço inválido de memória",
	/* 15: ENOTBLK		*/	"Este dispositivo não é de blocos",
	/* 16: EBUSY		*/	"Recurso ocupado",
	/* 17: EEXIST		*/	"O arquivo já existe",
	/* 18: EXDEV		*/	"Elo através de sistema de arquivos",
	/* 19: ENODEV		*/	"Dispositivo inexistente",
	/* 20: ENOTDIR		*/	"Não é um diretório",
	/* 21: EISDIR		*/	"É um diretório",
	/* 22: EINVAL		*/	"Argumento ou operação inválido",
	/* 23: ENFILE		*/	"Tabela FILE cheia",
	/* 24: EMFILE		*/	"Arquivos abertos demais",
	/* 25: ENOTTY		*/	"O dispositivo não é de caracteres",
	/* 26: ETXTBSY		*/	"Texto ocupado",
	/* 27: EFBIG		*/	"Arquivo excessivamente grande",
	/* 28: ENOSPC		*/	"Sistema de arquivos cheio",
	/* 29: ESPIPE		*/	"\"Seek\" em PIPE ou FIFO",
	/* 30: EROFS 		*/	"Proteção de escrita",
	/* 31: EMLINK		*/ 	"Número excessivo de elos físicos",
	/* 32: EPIPE		*/	"PIPE sem leitor",
	/* 33: EDOM		*/	"Argumento fora dos limites",
	/* 34: ERANGE		*/	"Resultado fora dos limites",
	/* 35: EDEADLK		*/	"Foi evitado um \"deadlock\"",
	/* 36: ENOLCK		*/	"Tabela de \"locks\" cheia",
	/* 37: EOLINE		*/	"Dispositivo não pronto",
	/* 38: EPFULL		*/	"FIFO Cheio",
	/* 39: ENOTFS		*/	"Este dispositivo não contem um sistema de arquivos",
	/* 40: EOLDVER		*/	"Referenciando biblioteca compartilhada antiga",
	/* 41: ENOTMNT		*/	"Dispositivo não montado",
	/* 42: ETIMEOUT		*/	"Tempo de espera esgotado",
	/* 43: ENOTREG		*/	"Não é um arquivo regular",
	/* 44: EBADFMT		*/	"Formato inválido",
	/* 45: ENOSHLIB		*/	"Biblioteca compartilhada não carregada",
	/* 46: ELOOP		*/	"Aninhamento excessivo de elos simbólicos",
	/* 48: 			*/	"?",
	/* 49: 			*/	"?",
	/* 50: 			*/	"?",
	/* 51: 			*/	"?",
	/* 52: 			*/	"?",
	/* 53: 			*/	"?",
	/* 54: 			*/	"?",
	/* 55: 			*/	"?",
	/* 56: 			*/	"?",
	/* 57: 			*/	"?",
	/* 58: 			*/	"?",
	/* 59: 			*/	"?",
	/* 60: 			*/	"?",
	/* 61: 			*/	"?",
	/* 62: 			*/	"?",
	/* 63: ENAMETOOLONG	*/	"Nome excessivamente comprido",
	/* 64: 			*/	"?",
	/* 65: 			*/	"?",
	/* 65: 			*/	"?",
	/* 66: ENOTEMPTY	*/	"Diretório não vazio",
	/* 67: 			*/	"?",
	/* 68: 			*/	"?",
	/* 69: EDQUOT 		*/	"Quota de disco excedida",
	/* 70: ESTALE 		*/	"Referência não mais válida a arquivo remoto",
	/* 71: EREMOTE 		*/	"Nível excessivo de referências remotas em um caminho",

	/* 72: TBADNET		*/	"Rede não inicializada",
	/* 73: TUNKNET		*/	"Nome/endereço de rede desconhecido",
	/* 74: TBADADDR		*/	"Erro no formato do endereço",
	/* 75: TADDRBUSY	*/	"Endereço já sendo utilizado",
	/* 76: TNOADDR		*/	"Não consegui alocar o endereço",
	/* 77: TBADOPT		*/	"Informações das opções inválidas",
	/* 78: TBADDATA		*/	"Informações do dado inválidas",
	/* 79: TNODATA		*/	"Ainda não há dados disponíveis",
	/* 80: TBADSEQ		*/ 	"Número de seqüência inválido",
	/* 81: TBADQLEN		*/	"Tamanho de fila inválido",
	/* 82: TBADFLAG		*/	"Indicadores inválidos",
	/* 83: TNODIS		*/	"Não há pedido de desconexão abortiva",
	/* 84: TNOREL		*/	"Não há pedido de desconexão ordenada",
	/* 85: TBUFOVFLW	*/	"Mensagem maior do que a área disponível",
	/* 86: TOUTSTATE	*/	"Estado inválido",
	/* 87: TLOOK		*/	"Evento ocorrido exige atenção",
	/* 88: TFLOW		*/	"Controle de fluxo impede transmissão",
	/* 89: TNOUDERR		*/	"Não houve erro no protocolo sem conexão",
	/* 90: TNOTSUPPORT	*/	"Função não implementada",
	/* 91: TNOROUTE		*/	"Não há rota para o nó destino",
	/* 92: TDNSSERVER	*/	"Erro no servidor de DNS"
};

entry const int	sys_nerr =  sizeof (sys_errlist) / sizeof (sys_errlist[0]);

/*
 ****************************************************************
 *	Chamada ao sistema "fcntl"				*
 ****************************************************************
 */
int
fcntl (int fd, int cmd, int arg)
{
	KFILE		*fp;
	int		fdnew;

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	switch (cmd)
	{
		/*
		 *	Duplicação de fds.
		 */
	    case F_DUPFD:
		if (arg < 0)
			break;

		for (fdnew = arg; fdnew < NUFILE; fdnew++)
		{
			if (u.u_ofile[fdnew] == NOKFILE)
				goto found;
		}
		break;

	    found:
		u.u_ofile[fdnew]  = fp;
		u.u_ofilef[fdnew] = 0;
		fp->f_count++;

		return (fdnew);

		/*
		 *	Obtem o indicador do descritor
		 */
	    case F_GETFD:
		return ((int)u.u_ofilef[fd]);

		/*
		 *	Atribui o indicador do descritor
		 */
	    case F_SETFD:
		u.u_ofilef[fd] = arg;
		return (0);

		/*
		 *	Obtem o indicador do arquivo
		 *	Repare a conversão dos formatos interno/externo
		 */
	    case F_GETFL:
		return (fp->f_flags - 1);

		/*
		 *	Atribui o indicador do arquivo
		 */
	    case F_SETFL:
		fp->f_flags &= ~(O_NDELAY|O_APPEND);
		fp->f_flags |= arg & (O_NDELAY|O_APPEND);
		return (0);

	}	/* end switch (cmd) */

	u.u_error = EINVAL;
	return (-1);

}	/* end fcntl */

/*
 ****************************************************************
 *	Chamada ao sistema "ioctl"				*
 ****************************************************************
 */
int
ioctl (int fd, int cmd, int arg)
{
	KFILE		*fp;
	INODE		*ip;
	dev_t		 dev;

	if ((fp = fget (fd)) == NOKFILE)
		return (-1);

	ip = fp->f_inode;

	u.u_oflag = fp->f_flags;

	dev = ip->i_rdev;

	/*
	 *	Chama a rotina de IOCTL do Dispositivo.
	 */
	switch (ip->i_mode & IFMT)
	{
	    case IFBLK:
		return ((*biotab[MAJOR(dev)].w_ioctl) (dev, cmd, arg, fp->f_flags));

	    case IFCHR:
		return ((*ciotab[MAJOR(dev)].w_ioctl) (dev, cmd, arg, fp->f_flags));

	    default:
		u.u_error = ENODEV;
		return (-1);
	}

}	/* end ioctl */


/*
 ****************************************************************
 *	Chamada ao sistema "kcntl"				*
 ****************************************************************
 */
int
kcntl (int cmd, int arg1, int arg2)
{
	switch (cmd)
	{
		/*
		 *	Obtem a estrutura SYM dado o nome
		 *
		 *	int kcntl (K_GETSYMGN, int len, SYM *sym);
		 */
	    case K_GETSYMGN:
	    {
#define	len	arg1
#define	sym	(SYM *)arg2

		SYM		*usp;
		const SYM	*sp, *endsymtb;
		int		sz;

		if (bcb.y_ssize == 0)
			{ u.u_error = ENXIO; return (-1); }

		if ((unsigned)len >= 64)
			{ u.u_error = EINVAL; return (-1); }

		sz = SYM_SIZEOF (len); usp = alloca (sz);

		if (unimove (usp, sym, sz, US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		endsymtb = (SYM *)((char *)&end + bcb.y_ssize);

		for (sp = (SYM *)&end; /* abaixo */; sp = SYM_NEXT_PTR (sp))
		{
			if (sp >= endsymtb)
				{ u.u_error = EINVAL; return (-1); }

			if (sp->s_nm_len != len)
				continue;

			if (streq (sp->s_name, usp->s_name))
				break;
		}

		if (unimove (sym, sp, sz, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);
#undef	len
#undef	sym
	    }

		/*
		 *	Obtem a estrutura SYM dado o endereço
		 *
		 *	int kcntl (K_GETSYMGA, ulong addr, SYM *sym);
		 */
	    case K_GETSYMGA:
	    {
#define	addr	(ulong)arg1
#define	sym	(SYM *)arg2

		const SYM	*sp, *spmax, *endsymtb;
		unsigned int 	max, value;
		SYM		s;

		if (bcb.y_ssize == 0)
			{ u.u_error = ENXIO; return (-1); }

		if (unimove (&s, sym, sizeof (SYM), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		max = 0; spmax = NOSYM;

		endsymtb = (SYM *)((char *)&end + bcb.y_ssize);

		for (sp = (SYM *)&end; sp < endsymtb; sp = SYM_NEXT_PTR (sp))
		{
			value = sp->s_value;

			if (value <= arg1 && value > max)
				{ max = value; spmax = sp; }
		}

		if (spmax == NOSYM)
			{ u.u_error = EINVAL; return (-1); }

		if (spmax->s_nm_len > s.s_nm_len)
			{ u.u_error = ENOMEM; return (-1); }

		if (unimove (sym, spmax, SYM_SIZEOF (spmax->s_nm_len), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

#undef	addr
#undef	sym
	    }

		/*
		 *	Obtem o (MAJOR + MINOR) do dispositivo dado o nome
		 *
		 *	int kcntl (K_GETDEV, const idp_t id);
		 */
	    case K_GETDEV:
	    {
		id_t		id;
		int		dev;

		if (unimove (id, (void *)arg1, sizeof (id_t), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if ((dev = strtodev ((char *)id)) == NODEV)
			{ u.u_error = EINVAL; return (-1); }

		return (dev);
	    }

		/*
		 *	Obtem o nome do dispositivo
		 *
		 *	idp_t kcntl (K_GETDEVNM, int dev, idp_t id);
		 */
	    case K_GETDEVNM:
	    {
		char		dev_nm[16];

		sprintf (dev_nm, "%v", arg1);

		if (unimove ((void *)arg2, dev_nm, sizeof (dev_nm), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return ((int)arg2);
	    }

		/*
		 *	Troca uma das tabelas de conversão de codigos.
		 *
		 *	int kcntl (K_PUTCVTB, int tb, const char *addr);
		 */
	    case K_PUTCVTB:
	    {
		char		*tbaddr;

		switch (arg1)
		{
		    case USER1:
			tbaddr = isotou1tb;
			break;

		    case USER2:
			tbaddr = isotou2tb;
			break;

		    default:
			u.u_error = EINVAL;
			return (-1);
		}

		if (superuser () < 0)
			return (-1);

		if (unimove (tbaddr, (void *)arg2, 256, US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);
	    }

		/*
		 *	Libera o recurso pelo qual um recurso está esperando
		 *
		 *	int kcntl (K_SEMAFREE, long pid);
		 */
	    case K_SEMAFREE:
	    {
		const UPROCV	*uvp;
		UPROC		*up;

		if (superuser () < 0)
			return (-1);

		for (uvp = &scb.y_uproc[scb.y_initpid]; /* vazio */; uvp++)
		{
			if (uvp > scb.y_lastuproc)
				{ u.u_error = ESRCH; return (-1); }

			if ((up = uvp->u_uproc) == NOUPROC)
				continue;

			if (up->u_state == SNULL || up->u_state == SZOMBIE)
				continue;

			if (up->u_pid == arg1)
				break;
		}

		if (up->u_state != SCORESLP && up->u_state != SSWAPSLP)
			{ u.u_error = EINVAL; return (-1); }

		switch (up->u_stype)
		{
		    case E_SLEEP:
			SLEEPFREE (up->u_sema);
			break;

		    case E_SEMA:
			SEMAFREE  (up->u_sema);
			break;

		    case E_EVENT:
			EVENTDONE (up->u_sema);
			break;
		}

		return (0);
	    }

		/*
		 *	Obtém os tamanhos do processo
		 *
		 *	int kcntl (K_GETPSZ, KPSZ *kpsz);
		 */
	    case K_GETPSZ:
	    {
		KPSZ		kpsz;

		kpsz.k_tsize = PGTOBY (u.u_tsize);
		kpsz.k_dsize = PGTOBY (u.u_dsize);
		kpsz.k_ssize = PGTOBY (u.u_ssize);

		if (unimove ((void *)arg1, &kpsz, sizeof (KPSZ), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);
	    }

#ifdef	INSTRUM
		/*
		 *	Gera um trace de recursos usados
		 *
		 *	int kcntl (K_KTRACE, long ktraceid);
		 */
	    case K_KTRACE:
	    {
		if (superuser () < 0)
			return (-1);

		ktraceid = arg1;

		return (0);
	    }
#endif	INSTRUM

		/*
		 *	Obtém informação sobre a MMU
		 *
		 *	int kcntl (K_GETMMU, UPROC *up, void *mmu);
		 */
	    case K_GETMMU:
	    {
#if (0)	/*******************************************************/
#define	up	(void *)arg1
#define	mmu	(void *)arg2

		if (unimove (up, &u, sizeof (UPROC), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (unimove (mmu + 0 * PGSZ, user_text_page_table, PGSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (unimove (mmu + 1 * PGSZ, user_data_page_table, PGSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (unimove (mmu + 2 * PGSZ, user_phys_page_table, PGSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (unimove (mmu + 3 * PGSZ, user_ipc_page_table, PGSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (unimove (mmu + 4 * PGSZ, user_stack_page_table, PGSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }
#endif	/*******************************************************/

		return (0);
#undef	up
#undef	mmu
	    }

		/*
		 *	Controla preemption no kernel
		 *
		 *	int kcntl (K_SETPREEMPTION, int flag, int cpuid);
		 */
	    case K_SETPREEMPTION:
	    {
#define	flag	arg1
#define	cpuid	arg2

		if (superuser () < 0)
			return (-1);

		if (cpuid < 0 || cpuid >= NCPU)
			{ u.u_error = EINVAL; return (-1); }

		if   (flag > 0)
			scb.y_preemption_mask[cpuid] = 0xFF;
		elif (flag < 0)
			scb.y_preemption_mask[cpuid] = 0;

		if (scb.y_preemption_mask[cpuid] == 0)
			return (0);
		else
			return (1);
#undef	flag
#undef	cpuid
	    }

		/*
		 *	Obtém o nome de um erro do KERNEL
		 *
		 *	int kcntl (K_ERRTOSTR, int errno, char *addr);
		 */
	    case K_ERRTOSTR:
	    {
#define	errno	arg1
#define	addr	(void *)arg2

		const char	*fmt;

		fmt = "?";

		if (errno >= 0 && errno < sys_nerr)
			fmt = sys_errlist[errno];

		if (fmt[0] == '?')
			{ fmt = alloca (40); snprintf ((char *)fmt, 40, "Erro Desconhecido (%d)", errno); }

		if (unimove (addr, fmt, strlen (fmt) + 1, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

#undef	errno
#undef	addr
	    }

		/*
		 *	Obtém uma (ou mais) entrada(s) da tabela de discos
		 *
		 *	int kcntl (K_GET_DEV_TB, dev_t dev, DISKTB *udp);
		 */
	    case K_GET_DEV_TB:
	    {
#define	dev	arg1
#define	udp	(DISKTB *)arg2

		const DISKTB	*dp;
		int		n;

		if (dev != NODEV)
		{
			for (dp = disktb; /* abaixo */; dp++)
			{
				if (dp->p_name[0] == '\0')
					{ u.u_error = EINVAL; return (-1); }

				if (dp->p_name[0] == '*')
					continue;

				if (dp->p_dev == dev)
					break;
			}

			if (unimove (udp, dp, sizeof (DISKTB), SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			return (0);
		}
		else	/* dev == NODEV */
		{
			n = next_disktb - disktb + 1;	/* Reserva espaço para a entrada vazia */

			if (udp != NODISKTB && unimove (udp, disktb, n * sizeof (DISKTB), SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			return (n);
		}
#undef	dev
#undef	udp
	    }		/* end case K_GET_DEV_TB */

		/*
		 *	Obtém o nome de um tipo de partição dos discos
		 *
		 *	int kcntl (K_GET_PARTNM, int type, char *nm)
		 */
	    case K_GET_PARTNM:
	    {
#define	type	arg1
#define	nm	(char *)arg2

		const PARTNM	*pnm;

		for (pnm = partnm; /* abaixo */; pnm++)
		{
			if (pnm->pt_type < 0 || pnm->pt_type == type)
				break;
		}

		if (unimove (nm, pnm->pt_nm, PARTNM_MAXSZ, SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (pnm->pt_type < 0)
			{ u.u_error = EINVAL; return (-1); }

		return (0);
#undef	type
#undef	nm
	    }

		/*
		 *	Obtém os tipos dos disquetes
		 *
		 *	int kcntl (K_GET_FD_TYPE)
		 */
	    case K_GET_FD_TYPE:
	    {
		extern int	fdtypes;

		return (fdtypes);
	    }

#if (0)	/*******************************************************/
		/*
		 *	Obtém/atribui o limite do MMU para usar páginas particulares
		 *
		 *	int kcntl (K_MMU_LIMIT, limit)
		 *
		 *	Se limit < 0, obtém; caso contrário atribui
		 */
	    case K_MMU_LIMIT:
	    {
#define	limit	arg1

		if (limit < 0)
			return (scb.y_mmu_pg_limit);

		if (superuser () < 0)
			return (-1);

		return (scb.y_mmu_pg_limit = limit);
#undef	limit
	    }
#endif	/*******************************************************/

		/*
		 *	Obtém/atribui o código de DEBUG do núcleo
		 *
		 *	int kcntl (K_DEBUG, value)
		 *
		 *	Se value < 0, obtém; caso contrário atribui
		 */
	    case K_DEBUG:
	    {
#define	value	arg1

		if (value < 0)
			return (scb.y_CSW);

		if (superuser () < 0)
			return (-1);

		return (scb.y_CSW = CSW = value);
#undef	value
	    }

		/*
		 *	Obtém/atribui o valor do registro "cr0"
		 *
		 *	unsigned long kcntl (K_DEBUG, code, value)
		 *
		 *	Se code == 0, obtém; caso contrário atribui
		 */
	    case K_CR0:
	    {
#define	code	arg1
#define	value	arg2

		if (code == 0)
			return (get_cr0 ());

		if (superuser () < 0)
			return (-1);

		put_cr0 (value);

		return (0);
#undef	value
#undef	code
	    }


		/*
		 *	Obtém á área de mensagens:
		 *
		 *		size = kcntl (K_DMESG, NOSTR, 0);
		 *
	 	 *	devolve o tamanho da área necessária "size"
		 *
		 *		count = kcntl (K_DMESG, area, size);
		 *
		 *	copia a mensagem para "area" de tamanho "size",
		 *	e retorna "count", o número de caracteres copiados
		 */
	    case K_DMESG:
	    {
#define	area	(char *)arg1
#define	size	arg2

		int		count, total = 0;

		if (area == NOSTR)
			return (scb.y_dmesg_sz);

		if ((count = dmesg_end - dmesg_ptr) > 0 && dmesg_ptr[0] != '\0')
		{
			if (count > size)
				count = size;

			if (unimove (area, dmesg_ptr, count, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			area  += count;
			size  -= count;
			total += count;
		}

		if ((count = dmesg_ptr - dmesg_area) > 0)
		{
			if (count > size)
				count = size;

			if (unimove (area, dmesg_area, count, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

		   /***	area  += count;	***/
		   /***	size  -= count;	***/
			total += count;
		}

		return (total);
#undef	area
#undef	size
	    }

	}	/* end switch (cmd) */

	u.u_error = EINVAL;
	return (-1);

}	/* end kcntl */

#if (0)	/*******************************************************/
		if (size > DMSGSZ)
			size = DMSGSZ;

		if (dmesg_ptr >= dmesg_end)
		{
			if (unimove (area, dmesg_area, size, SU) < 0)
				{ u.u_error = EFAULT; return (-1); }

			cnt = size;
		}
		else
		{
			if (dmesg_ptr[0] != '\0')
			{
				count = size;

				if (count > dmesg_end - dmesg_ptr)
					count = dmesg_end - dmesg_ptr;

				if (unimove (area, dmesg_ptr, count, SU) < 0)
					{ u.u_error = EFAULT; return (-1); }

				area  += count;
				cnt   += count;
				size  -= count;
			}

			if (size > 0)
			{
				if (unimove (area, dmesg_area, size, SU) < 0)
					{ u.u_error = EFAULT; return (-1); }

				cnt += size;
			}
		}
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Chamada ao sistema "pcntl"				*
 ****************************************************************
 */
int
pcntl (int cmd, int arg1, int arg2)
{
	switch (cmd)
	{
#ifdef	ICA
		PROC		*p;
		long		ulim, llim;

		p = u.u_proc;

		/*
		 *	Obtém estimativa de tempos em processamento paralelo
		 *
		 *	pcntl (P_PMUTIMES, tp)
		 *	PMUTIMES	*tp;
		 */
	    case P_PMUTIMES:
		return (pmutimes (arg1));

		/*
		 *	Obtém o tamanho da região de stack
		 *
		 *	pcntl (P_GETSTKRGSZ)
		 */
	    case P_GETSTKRGSZ:
		return (u.u_proc->p_stack.rgl_regiong->rgg_size << PGSHIFT);

#if (0)	/*******************************************************/
		return (u.u_regionlptr[RG_STACK].rgl_regiong->rgg_size
				 << PGSHIFT);
#endif	/*******************************************************/

		/*
		 *	Aumenta o tamanho da região de stack
		 *
		 *	pcntl (P_SETSTKRGSZ, size)
		 *	int	size;
		 */
	    case P_SETSTKRGSZ:
		setstacksz (STKADDR - arg1);

		return (u.u_proc->p_stack.rgl_regiong->rgg_size << PGSHIFT);

#if (0)	/*******************************************************/
		return (u.u_regionlptr[RG_STACK].rgl_regiong->rgg_size
				 << PGSHIFT);
#endif	/*******************************************************/

		/*
		 *	Obtém a prioridade do processo
		 *
		 *	pcntl (P_GETPRI)
		 */
	    case P_GETPRI:
		if (p->p_more_flags & RTPROC)
			return (p->p_rtpri);
		else
			return (p->p_pri);

		/*
		 *	Altera a prioridade do processo
		 *
		 *	pcntl (P_SETPRI, value)
		 *	int	value;
		 */
	    case P_SETPRI:

		if (superuser () < 0)
			return (-1);

		if (p->p_more_flags & RTPROC)
		{
			if (arg1 >= 0 && arg1 < 2000)
				return (p->p_rtpri = arg1);
		}
		else
		{
			if (arg1 >= 0 && arg1 < 1000)
				return (p->p_pri = arg1);
		}

		u.u_error = EINVAL;
		return (-1);

		/*
		 *	Transforma um processo comum em tempo-real e vv.
		 *
		 *	pcntl (P_SETRTPROC, value)
		 *	int	value;
		 */
	    case P_SETRTPROC:

		if (superuser () < 0)
			return (-1);

		if (arg1 == 0)
		{
			if (p->p_more_flags & RTPROC)
				return (1);
			else
				return (0);
		}

		if (arg1 == 1)
		{
			p->p_more_flags |= RTPROC;
			return (1);
		}

		if (arg1 == -1)
		{
			p->p_more_flags &= ~RTPROC;
			p->p_rtpri = 0;
			p->p_pri = PUSER;
			return (0);
		}

		break;

		/*
		 *	Indica para o kernel o endereço de usuário dos
		 *		registros de floating point de software
		 *
		 *	pcntl (P_SETFPREGADDR, addr)
		 *	caddr_t	addr;
		 */
	    case P_SETFPREGADDR:
		/*
		 *	Verifica se o endereço dado está na região de dados
		 */
		llim = u.u_regionlptr[RG_DATA].rgl_vaddr << PGSHIFT;
		ulim = llim + (u.u_regionlptr[RG_DATA].rgl_regiong->rgg_size
				<< PGSHIFT);

#define	SOFTFPREGSZ	168	/* No. de bytes dos regs. de fp de software */

		if (arg1 >= llim && arg1 <= ulim - SOFTFPREGSZ)
		{
			u.u_softfpregaddr = (caddr_t)arg1;
			return (0);
		}

		break;
#endif	ICA

		/*
		 *	Permite o usuário a ler/escrever nas portas
		 */
	    case P_ENABLE_USER_IO:
	    {
		struct syscall_frame	*sp;

		if (superuser () < 0)
			return (-1);

		sp = (struct syscall_frame *)u.u_frame;
		sp->sf_sr |= 0x3000;

		return (0);
	    }

	    case P_DISABLE_USER_IO:
	    {
		struct syscall_frame	*sp;

		sp = (struct syscall_frame *)u.u_frame;
		sp->sf_sr &= ~0x3000;

		return (0);
	    }

		/*
		 *	Obtém o endereço físico associado a um virtual
		 *
		 *	paddr = pcntl (P_GET_PHYS_ADDR, vaddr)
		 */
	    case P_GET_PHYS_ADDR:
	    {
#define		vaddr		arg1
		const mmu_t	*mmup;
		ulong		paddr;

		if ((mmup = vaddr_to_page_table_entry ((unsigned)vaddr >> PGSHIFT, 1 /* pg */)) == NOMMU)
			break;

		if (((paddr = *mmup) & 0x01) == 0)
			break;

		paddr &= ~PGMASK;	/* Tira os bits baixos da entrada da MMU */

		paddr |= ((int)vaddr & PGMASK); /* Coloca os bits baixos do endereço */

		return (paddr);		/* Em princípio */

#undef		vaddr
	    }

	}	/* end switch (cmd) */

	u.u_error = EINVAL;
	return (-1);

}	/* end pcntl */
