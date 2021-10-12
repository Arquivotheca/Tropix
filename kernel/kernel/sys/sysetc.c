/*
 ****************************************************************
 *								*
 *			sysetc.c				*
 *								*
 *	Chamadas ao sistema: diversos				*
 *								*
 *	Versão	3.0.0, de 19.12.94				*
 *		4.6.0, de 03.08.04				*
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
#include "../h/frame.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/iotab.h"
#include "../h/ioctl.h"
#include "../h/cpu.h"
#include "../h/times.h"
#include "../h/timeout.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições Globais				*
 ****************************************************************
 */
#define	PATTENTION	PTTYIN	/* Prioridade de ATTENTION */

/*
 ******	Portas e definições do teclado **************************
 */
#define	KEY_STATUS	0x64	/* Registro de estado */
#define	KEY_DATA	0x60	/* Registro de dados */

#define	KEY_AVAIL	0x01	/* Caracter disponível no teclado */
#define	KEY_BUSY	0x02	/* Reg. de entrada não pronto */
#define	KEY_SETLEDs	0xED	/* Prepara escrita nos LEDs */
#define KEY_ACK		0xFA	/* Acknowledge */
#define KEY_RESEND	0xFE	/* Tente novamente */

/*
 ******	Protótipos de funções ***********************************
 */
pg_t		phys_alloc (pg_t, int, pg_t);
int		phys_release (pg_t);

/*
 ****************************************************************
 *	Chamada ao sistema "brk"				*
 ****************************************************************
 */
int
brk (int newbyteaddr)
{
	int		newsz, diff;
	REGIONL		*rlp;

	/*
	 *	Calcula o incremento (que pode ser negativo)
	 */
	newsz = BYUPPG (newbyteaddr);

	/*
	 *	Retira do calculo o início do TEXT ou DATA
	 */
	rlp = &u.u_data;

	if   (u.u_tsize == 0)
		newsz -= u.u_text.rgl_vaddr;
	elif (rlp->rgl_regiong == NOREGIONG)
		newsz -= USER_DATA_PGA;
	else
		newsz -= rlp->rgl_vaddr;

#ifdef	MSG
	if (CSWT (3))
		printf ("%g: VA = %P, newsz = %d\n", newbyteaddr, newsz);
#endif	MSG

#ifdef	MSG
	if (CSWT (25))
		printf ("%g: Dando BRK para região DATA de tamanho %P\n", newsz);
#endif	MSG

	if (newsz < 0)
		{ u.u_error = EINVAL; return (-1); }

	if ((diff = newsz - u.u_dsize) == 0)
		return (0);

	/*
	 *	Analisa a gerência de memória
	 */
	if (mmutst (u.u_tsize, newsz, u.u_ssize) < 0)
		{ u.u_error = ENOMEM; return (-1); }

	/*
	 *	Muda o tamanho da região de dados
	 */
	if (rlp->rgl_regiong == NOREGIONG)
	{
		rlp->rgl_type	 = RG_DATA;
		rlp->rgl_prot	 = URW;

		rlp->rgl_vaddr	 = USER_DATA_PGA;
	   /***	rlp->rgl_regiong = ...;	/* abaixo ***/
	}

	if (regiongrow (rlp, 0, newsz, 0) < 0)
		{ u.u_error = ENOMEM; return (-1); }

	u.u_dsize = newsz;
	u.u_size += diff;
	return (0);

}	/* end brk */

/*
 ****************************************************************
 *	Aumenta a STACK do processo				*
 ****************************************************************
 */
int
setstacksz (void *new_sp)
{
	int		new_bytesz, pginc;
	int		new_stacksz;
	REGIONL		*rlp;

	/*
	 *	Retorno: A STACK
	 *	    +1: era Pequena e foi aumentada.
	 *	     0: era Suficiente e não foi alterada.
	 *	    -1: era Pequena, mas não foi possivel aumentar.
	 */
	new_bytesz = PGTOBY (USER_STACK_PGA) - (unsigned long)new_sp;

	if (new_bytesz <= PGTOBY (u.u_ssize))
		return (0);

	/*
	 *	Ela não é suficiente; calcula o incremento
	 */
	new_stacksz =  (new_bytesz >> PGSHIFT) + BYUPPG (scb.y_stkincr);
	pginc = new_stacksz - u.u_ssize;

	if (pginc <= 0)
		return (0);

	/*
	 *	Verifica se o novo tamanho pode ser mapeado
	 */
	if (mmutst (u.u_tsize, u.u_dsize, new_stacksz) < 0)
		return (-1);

#ifdef	MSG
	if (CSWT (25))
		printf ("%g: Aumentando a STACK de %d páginas\n", u.u_pid, pginc);
#endif	MSG

	/*
	 *	Aumenta o tamanho da região
	 */
	rlp = &u.u_stack;

   /***	rlp->rgl_type  = RG_STACK; ***/
   /***	rlp->rgl_vaddr = ...; ***/
   /***	rlp->rgl_prot  = URW; ***/

	if (regiongrow (rlp, rlp->rgl_vaddr - pginc, new_stacksz, 0) < 0)
		return (-1);

	u.u_ssize += pginc;
	u.u_size  += pginc;
	return (1);

}	/* end setstacksz */

/*
 ****************************************************************
 *	Chamada ao sistema "getscb"				*
 ****************************************************************
 */
SCB *
getscb (SCB *sp)
{
	if (sp == (SCB *)NULL)
		return (&scb);

	if (unimove (sp, &scb, sizeof (SCB), SU) < 0)
		{ u.u_error = EFAULT; return ((SCB *)-1); }
	else
		{ return (&scb); }

}	/* end getscb */

/*
 ****************************************************************
 *	Chamada ao sistema "setscb"				*
 ****************************************************************
 */
int
setscb (SCB *sp)
{
	SCB		uscb;

	if (superuser () < 0)
		return (-1);

	if (unimove (&uscb, sp, sizeof (SCB), US) < 0)
		{ u.u_error = EFAULT; return (-1); }

	scb.y_tzmin = uscb.y_tzmin;

	return (0);

}	/* end getscb */

/*
 ****************************************************************
 *	Chamada ao sistema "gettzmin"				*
 ****************************************************************
 */
int
gettzmin (void)
{
	return (scb.y_tzmin);

}	/* end gettzmin */

/*
 ****************************************************************
 *	Chamada ao sistema "spy"				*
 ****************************************************************
 */
int
spy (void)
{
	if (superuser () == 0)
		spyexcep ();

	return (UNDEF);

}	/* end spy */

/*
 ****************************************************************
 *	Chamada ao sistema "attention"				*
 ****************************************************************
 */
int
attention (int nfd, int fd_vector[], int index, int timeout)
{
	int		fd, r, dev, i;
	const int	*fdp;
	EVENT		*attention_event_addr;
	const KFILE	*fp;
	const INODE	*ip;
	char		**reset_vector, **reset_vector_ptr;
	TIMEOUT		*tp = NOTIMEOUT;
	int		value;

	/*
	 *	Consistência inicial
	 */
	value = -1;  u.u_frame->s.sf_r1 = -1;

	if (index < 0)
		index += nfd;

	if ((unsigned)nfd > NUFILE || (unsigned)index >= nfd)
		{ u.u_error = EINVAL; return (-1); }

	fdp = &fd_vector[++index];	/* O Próximo */

	reset_vector = reset_vector_ptr = alloca (nfd * sizeof (char *));

	/*
	 *	Inicializa o evento
	 */
	attention_event_addr = &u.u_myself->u_attention_event;

	EVENTCLEAR (attention_event_addr);

	u.u_attention_index = -1;	/* Retorno em caso de TIMEOUT */

	/*
	 *	Examina cada File Descriptor
	 */
	for (r = 0, i = nfd;   i > 0;   i--, index++, fdp++, reset_vector_ptr++)
	{ 
		if (index >= nfd)
			{ index = 0; fdp = fd_vector; }

		u.u_frame->s.sf_r1 = index;

		if ((fd = gulong (fdp)) < 0)
			{ u.u_error = EFAULT; break; }

		if ((fp = fget (fd)) == NOKFILE)
			break;

		if ((fp->f_flags & O_READ) == 0)
			{ u.u_error = EBADF; break; }

		ip = fp->f_inode; dev = ip->i_rdev;

		/*
		 *	Consulta cada "driver"
		 */
		switch (ip->i_mode & IFMT)
		{
		    case IFREG:
			if ((fp->f_flags & O_PIPE) == 0)
				{ u.u_error = ENODEV; r = -1; break; }

			r = pipe_attention (index, reset_vector_ptr);
			break;

		    case IFBLK:
			r = biotab[MAJOR(dev)].w_ioctl (dev, U_ATTENTION, index, (int)reset_vector_ptr);
			break;
	
		    case IFCHR:
			r = ciotab[MAJOR(dev)].w_ioctl (dev, U_ATTENTION, index, (int)reset_vector_ptr);
			break;
	
		    default:
			u.u_error = ENODEV; r = -1; break;

		}	/* end switch */

		/*
		 *	Avalia o resultado
		 */
		if   (r > 0)		/* Dados disponíveis */
			{ value = index; break; }
		elif (r < 0)		/* Erro */
			{ /*** value = -1; ***/ break; }

		/* r == 0: Ainda não achou dados disponíveis */

	}	/* end for */

	/*
	 *	Verifica se há dados ou houve erro
	 */
	if (r != 0 || u.u_error != NOERR)
		goto unattention;

	/*
	 *	Nenhum dos dispositivos está pronto;
	 *	Verifica qual a ação a tomar, de acordo com "timeout"
	 */
	u.u_frame->s.sf_r1 = -1;

	if (timeout < 0)
		{ u.u_error = EINTR; /*** value = -1; ***/ goto unattention; }

	/*
	 *	Prepara o TIMEOUT
	 *	repare que conhecemos a expansão da macro EVENTDONE
	 */
	if (timeout > 0)
	   tp = toutset (timeout * scb.y_hz, eventdone, (int)attention_event_addr);

	/*
	 *	Aguarda (com ou sem TIMEOUT)
	 */
	EVENTWAIT (attention_event_addr, PATTENTION);

	/*
	 *	Um dos dispositivos ficou pronto, ou passou o TIMEOUT
	 */
	if (u.u_attention_index < 0)
		u.u_error = EINTR;

	if (timeout > 0)
		toutreset (tp, eventdone, (int)attention_event_addr);

	u.u_frame->s.sf_r1 = u.u_attention_index;

	value = u.u_attention_index;	/* Retornando o "index" */

	/*
	 *	Desarma todos os "attentions"
	 */
    unattention:
	while (reset_vector < reset_vector_ptr)
		**reset_vector++ = 0;

	return (value);

}	/* end attention */

/*
 ****************************************************************
 *	Chamada ao sistema "select"				*
 ****************************************************************
 */
int
select (int nfd, int *read_set_ptr, int *write_set_ptr, int *excep_set_ptr, MUTM *timeout)
{
	char		**reset_vector, **reset_vector_ptr;
	MUTM		t;
	TIMEOUT		*tp = NOTIMEOUT;
	const KFILE	*fp;
	const INODE	*ip;
	EVENT		*attention_event_addr;
	int		read_set = 0, write_set = 0, excep_set = 0;
	int		fd, r, dev;
	int		mask, value;
	int		ready = 0;

	/*
	 *	Consistência inicial
	 *
	 *	Estamos supondo NUFILE <= 32
	 */
	if ((unsigned)nfd > NUFILE)
		{ u.u_error = EINVAL; return (-1); }

	if (read_set_ptr && (read_set = gulong (read_set_ptr)) < 0)
		{ u.u_error = EFAULT; return (-1); }

	if (write_set_ptr && (write_set = gulong (write_set_ptr)) < 0)
		{ u.u_error = EFAULT; return (-1); }

   /***	if (excep_set_ptr && (excep_set = gulong (excep_set_ptr)) < 0) ***/
	   /***	{ u.u_error = EFAULT; return (-1); } ***/

	reset_vector = reset_vector_ptr = alloca (nfd * sizeof (char *));

	/*
	 *	Inicializa o evento
	 */
	attention_event_addr = &u.u_myself->u_attention_event;

	EVENTCLEAR (attention_event_addr);

	u.u_attention_index = -1;	/* Retorno em caso de TIMEOUT */

	/*
	 *	Examina cada descritor
	 */
	for (mask = 1, fd = 0;   fd < nfd;   mask <<= 1, fd++)
	{ 
		if (write_set & mask)			/* Somente conta */
			ready++;

	   /***	if (excep_set & mask) ***/		/* Ignora */
		   /***	...; ***/

		if ((read_set & mask) == 0)
			continue;

		if ((fp = fget (fd)) == NOKFILE)
			break;

		if ((fp->f_flags & O_READ) == 0)
			{ u.u_error = EBADF; break; }

		ip = fp->f_inode; dev = ip->i_rdev;

		/*
		 *	Consulta cada "driver"
		 */
		switch (ip->i_mode & IFMT)
		{
		    case IFREG:
			if ((fp->f_flags & O_PIPE) == 0)
				{ u.u_error = ENODEV; r = -1; break; }

			r = pipe_attention (fd, reset_vector_ptr);
			break;

		    case IFBLK:
			r = biotab[MAJOR(dev)].w_ioctl (dev, U_ATTENTION, fd, (int)reset_vector_ptr);
			break;
	
		    case IFCHR:
			r = ciotab[MAJOR(dev)].w_ioctl (dev, U_ATTENTION, fd, (int)reset_vector_ptr);
			break;
	
		    default:
			u.u_error = ENODEV; r = -1; break;

		}	/* end switch */

		/*
		 *	Avalia o resultado
		 */
		if   (r > 0)			/* Dados disponíveis */
		{
			ready++; 		/* Deixa o bit de "read_set" ligado */
		}
		elif (r < 0)			/* Erro (u.u_error ligado) */
		{
			break;
		}
		else /* r == 0 */		/* Ainda não há dados disponíveis */
		{
			read_set &= ~mask;	/* Desliga o bit de "read_set" */
			reset_vector_ptr++;	/* O evento foi armado */
		}

	}	/* end for (fds) */

	/*
	 *	Verifica se há dados ou houve erro
	 */
	if (u.u_error != NOERR)
		{ value = -1; goto unattention; }

	/*
	 *	Verifica se há descritores prontos
	 */
	if (ready > 0)
		goto copy_sets;

	/*
	 *	Nenhum dos dispositivos está pronto;
	 *	Verifica qual a ação a tomar, de acordo com "timeout"
	 */
	if (timeout != (MUTM *)NULL)
	{
		int		ticks = 0;

		if (unimove (&t, timeout, sizeof (MUTM), US) < 0)
			{ u.u_error = EFAULT; value = -1; goto unattention; }

		if (t.mu_time != 0)
			ticks += t.mu_time * scb.y_hz;

		if (t.mu_utime != 0)
			ticks += t.mu_utime * scb.y_hz / 1000000;

		if (ticks == 0)
			{ goto copy_sets; }

		tp = toutset (ticks, eventdone, (int)attention_event_addr);
	}

	/*
	 *	Aguarda (com ou sem TIMEOUT)
	 */
	EVENTWAIT (attention_event_addr, PATTENTION);

	/*
	 *	Um dos dispositivos ficou pronto, ou passou o TIMEOUT
	 */
	if (tp)
		toutreset (tp, eventdone, (int)attention_event_addr);

	if (u.u_attention_index >= 0)
		{ read_set |= (1 << u.u_attention_index); ready++; }

	/*
	 *	Há descritores prontos: devolve o resultado
	 */
    copy_sets:
	if (read_set_ptr && pulong (read_set_ptr, read_set) < 0)
		{ u.u_error = EFAULT; return (-1); }

   /***	if (write_set_ptr && pulong (write_set_ptr, write_set) < 0) ***/
	   /***	{ u.u_error = EFAULT; return (-1); } ***/

   	if (excep_set_ptr && pulong (excep_set_ptr, excep_set) < 0)
	   	{ u.u_error = EFAULT; return (-1); }

	value = ready;

	/*
	 *	Desarma todos os "attentions"
	 */
    unattention:
	while (reset_vector < reset_vector_ptr)
		**reset_vector++ = 0;

	return (value);

}	/* end select */

/*
 ****************************************************************
 *	Reinicializa o computador (dá BOOT)			*
 ****************************************************************
 */
int
boot (void)
{
	int		c;

	if (superuser () < 0)
		return (-1);

	*(ushort *)(SYS_ADDR+0x472) = 0x1234;

	for (c = 100; c > 0; c--)
	{
		while (read_port (KEY_STATUS) & KEY_BUSY)
			/* vazio */;

		write_port (0xFE, KEY_STATUS);
	}

	reset_cpu ();

	return (-1);	/* To make LINT happy */

}	/* end boot */

/*
 ****************************************************************
 *	Chamada ao sistema "phys"				*
 ****************************************************************
 */
int
phys (void *area, int size, int oflag)
{
	unsigned long	byte_addr;
	pg_t		phys_pg_addr, virt_pg_addr, pg_size;
	int		offset;

	/*
	 *	No momento:
	 *		Endereço virtual inicial == USER_PHYS_PGA
	 *		Não há mais tamanho máximo por região;
	 *		apenas o tamanho total máximo
	 */
	if (superuser () < 0)
		return (-1);

	oflag += 1;	/* converte o tipo de acesso para a forma interna */

	/*
	 *	Verifica se é para desativar o mapeamento
	 */
	if   (size == 0)
		return (phys_release ((unsigned)area >> PGSHIFT));
	elif (size < 0)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	É para ativar o mapeamento; verifica se é espaço do kernel
	 */
	if (oflag & O_KERNEL)
		byte_addr = (unsigned)area;
	else
		byte_addr = PHYS_TO_VIRT_ADDR (area);

	/*
	 *	Converte tudo para páginas
	 */
	offset = byte_addr & PGMASK;	/* Offset dentro da página */

	phys_pg_addr = byte_addr >> PGSHIFT;

	pg_size = BYUPPG (byte_addr + size) - phys_pg_addr;

	/*
	 *	Tenta alocar na tabela
	 */
	if ((virt_pg_addr = phys_alloc (pg_size, oflag, phys_pg_addr)) == NOPG)
		return (-1);

	return (PGTOBY (virt_pg_addr) | offset);

}	/* end phys */

/*
 ****************************************************************
 *	Alloca uma área da tabela de PHYS			*
 ****************************************************************
 */
pg_t
phys_alloc (pg_t size, int oflag, pg_t paddr)
{
	REGIONL		*rlp;
	REGIONG		*rgp;
	pg_t		vaddr;
	pg_t		size_max;

	/*
	 *	Procura a primeira entrada livre
	 */
	for (rlp = &u.u_phys_region[PHYSNO - 1]; /* abaixo */; rlp--)
	{
		if (rlp < &u.u_phys_region[0])
		{
			vaddr	 = USER_PHYS_PGA;
			size_max = USER_PHYS_PGA_END - vaddr;

			if (size_max >= size)
				{ rlp++; goto found; }
			else
				{ u.u_error = EINVAL; return (NOPG); }
		}

		if (rlp->rgl_regiong == NOREGIONG)
			continue;

		if (rlp >= &u.u_phys_region[PHYSNO - 1])
			{ u.u_error = EBUSY; return (NOPG); }

#define	KBSZROUND(x)	(((unsigned)(x) + (KBSZ - 1)) & ~KBMASK)

		vaddr = rlp->rgl_vaddr + KBSZROUND (rlp->rgl_regiong->rgg_size);

		size_max = USER_PHYS_PGA_END - vaddr;

		if (size_max >= size)
			{ rlp++; goto found; }
		else
			{ u.u_error = EINVAL; return (NOPG); }
	}

	/*
	 *	Agora procura uma REGIONG vaga
	 */
    found:
	if ((rgp = regiongget ()) == NOREGIONG)
		{ u.u_error = EBUSY; return (NOPG); }

   /***	memclr (rgp, sizeof (REGIONG)); ***/

	/*
	 *	Tenta alocar as tabelas de páginas para a MMU
	 */
	rlp->rgl_regiong = rgp;
	rlp->rgl_vaddr	 = vaddr;

	rlp->rgl_prot	 = (oflag & O_WRITE) ? URW : URO;
	rlp->rgl_type	 = RG_PHYS;

   /***	rgp->rgg_pgtb	 = NOPG; ***/
   /***	rgp->rgg_pgtb_sz = NOPG; ***/

   /***	rgp->rgg_size	 = 0; ***/
   	rgp->rgg_paddr	 = paddr;

	if (mmu_page_table_alloc (rlp, vaddr, size) < 0)
	{
		regiongput (rgp);
		memclr (rlp, sizeof (REGIONL));
		u.u_error = ENOMEM;
		return (NOPG);
	}

   	rgp->rgg_size	= size;
   /***	rgp->rgg_paddr	= ...; ***/
   	rgp->rgg_type	= RG_PHYS;
   	rgp->rgg_count	= 1;

	mmu_dir_load (rlp);

	/*
	 *	Já tem todos os recursos; finaliza
	 */
	u.u_phn++;

	return (vaddr);

}	/* end phys_alloc */

/*
 ****************************************************************
 *	Libera a área da tabela de PHYS				*
 ****************************************************************
 */
int
phys_release (pg_t vaddr)
{
	REGIONL		*rlp;

	/*
	 *	x
	 */
	for (rlp = &u.u_phys_region[0]; /* abaixo */; rlp++)
	{
		if (rlp >= &u.u_phys_region[PHYSNO])
			{ u.u_error = EINVAL; return (-1); }

		if (rlp->rgl_vaddr == vaddr)
			break;
	}

	/*
	 *	Procura a REGIONL referenciada
	 */
	if (rlp->rgl_regiong == NOREGIONG)
		{ u.u_error = EINVAL; return (-1); }

	/*
	 *	Desaloca
	 */
	regionrelease (rlp);

	u.u_phn--;

	return (0);

}	/* end phys_release */
