/*
 ****************************************************************
 *								*
 *			inode.c					*
 *								*
 *	Algoritmos de manipulação de INODEs			*
 *								*
 *	Versão	3.0.0, de 22.11.94				*
 *		4.8.0, de 29.11.05				*
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
#include "../h/region.h"

#include "../h/mon.h"
#include "../h/disktb.h"
#include "../h/bhead.h"
#include "../h/inode.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/ipc.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry INODE	inode_lru_head;	/* Cabeça da lista LRU */

entry LOCK	inode_lru_lock;	/* Lock da lista LRU */

/*
 ******	Protótipos de funções ***********************************
 */
int		inode_insert_hash (INODE *ip, IHASHTB *hp);
void		inode_remove_hash (INODE *ip, IHASHTB *hp);

/*
 ****************************************************************
 *	Obtem um INODE na memória				*
 ****************************************************************
 */
INODE *
iget (dev_t dev, ino_t ino, int mission)
{
	INODE		*ip;
	SB		*sp;
	IHASHTB		*hp;
	int	 	cycle;

	/*
	 *	Incrementa o contador do DEV na tabela SB
	 */
	if (ino == -1)
		{ u.u_error = EINVAL; return (NOINODE); }

	sp = sbget (dev);
    new:
	if (sp == NOSB)
	{
		printf ("%g: O Dispositivo \"%v\" não está montado\n", dev);
		u.u_error = ENODEV; return (NOINODE);
	}

	/*
	 *	Verifica se há operação de MOUNT/UMOUNT em andamento neste dispositivo
	 */
	if (sp->sb_mbusy)
		{ sbput (sp); u.u_error = EBUSY; return (NOINODE); }

	sp->sb_mcount++;

	sbput (sp);

	/*
	 *	Inicia a Busca na Tabela HASH
	 */
	hp = IHASH (dev, ino);

    again:
	SPINLOCK (&hp->h_lock);

	for (ip = hp->h_inode; ip != NOINODE; ip = ip->i_next)
	{
		if (ip->i_ino != ino || ip->i_dev != dev)
			continue;

		/* Achou o Inode desejado */

		SPINFREE (&hp->h_lock);

		SLEEPLOCK (&ip->i_lock, PINODE);

		/* Verifica se ainda é o INODE desejado */

		if (ip->i_ino != ino || ip->i_dev != dev)
			{ SLEEPFREE (&ip->i_lock); goto again; }

		/* Retorna se este INODE não for a RAIZ de um sistema montado */

		if (ip->i_count++ == 0)
			mon.y_inode_in_use_cnt++;

		if ((ip->i_flags & IMOUNT) == 0 || mission & INOMNT)
			goto put_end_lru;

		/* Este INODE é na realidade a RAIZ de um sistema montado */

		iput (ip);

		sp = sbiget (ip);

		dev = sp->sb_dev;
		ino = sp->sb_root_ino;

 		goto new;

	}	/* end for (fila HASH) */

	SPINFREE (&hp->h_lock);

	/*
	 *	O INODE pedido não está na Tabela
	 */
	cycle = 0;

	SPINLOCK (&inode_lru_lock);

	for (ip = inode_lru_head.i_lru_forw; /* abaixo */; ip = ip->i_lru_forw)
	{
		if (ip == &inode_lru_head)
		{
			if (cycle++ <= 0)
				continue;

			/* A Tabela está cheia */

			SPINFREE (&inode_lru_lock);

			SPINLOCK (&sp->sb_mlock);
			sp->sb_mcount--;
			SPINFREE (&sp->sb_mlock);

			printf ("%g: Tabela de INODEs Cheia\n");

			u.u_error = ENFILE;

			return (NOINODE);
		}

		/* Tenta obter um candidato trancado */

		if (SLEEPTEST (&ip->i_lock) < 0)
			continue;

		if (ip->i_count != 0)
			{ SLEEPFREE (&ip->i_lock); continue; }

		SPINFREE (&inode_lru_lock);

		break;

	}	/* end for (fila LRU) */

	/*
	 *	Verifica se está em alguma lista HASH
	 *
	 *	Neste ponto, a lista LRU já está liberada
	 */
	if (ip->i_ino != NOINO)
	{
		inode_remove_hash (ip, IHASH (ip->i_dev, ip->i_ino));

		(*fstab[ip->i_sb->sb_code].fs_rel_inode_attr) (ip);
	}

	/*
	 *	Prepara a entrada nova
	 *
	 *	Neste ponto está trancado o INODE
	 */
	ip->i_dev	= dev;
	ip->i_ino	= ino;
	ip->i_par_ino	= NOINO;	/* Ainda desconhecido */
	ip->i_nm	= NOSTR;	/* Ainda desconhecido */

	ip->i_count	= 1;
	ip->i_sb	= sp;
	ip->i_flags	= 0;
	ip->i_lastr	= 0;

	memclr (&ip->u, sizeof (ip->u));

	/*
	 *	Agora insere na fila nova
	 */
	if (inode_insert_hash (ip, hp) < 0)
{
printf ("%g: Evitada a duplicação da alocação do INODE\n");
		{ SLEEPFREE (&ip->i_lock); goto again; }
}

	mon.y_inode_in_use_cnt++;

	/*
	 *	Le o INODE do disco
	 */
	if ((mission & INOREAD) == 0)
	{
		if ((*fstab[sp->sb_code].fs_read_inode) (ip) < 0)
		{
			/* Não conseguiu ler o INODE do disco */

			inode_remove_hash (ip, hp);

			ip->i_count	= 0;
			ip->i_ino	= NOINO;
			ip->i_flags	= 0;

			mon.y_inode_in_use_cnt--;

			SLEEPFREE (&ip->i_lock);

			SPINLOCK (&sp->sb_mlock);
			sp->sb_mcount--;
			SPINFREE (&sp->sb_mlock);

			printf ("%g: Erro na leitura do INODE (%v, %d) (%r)\n", dev, ino, u.u_error);

			return (NOINODE);
		}
	}
	else	/* mission & INOREAD */
	{
		ip->i_size  = 0;

		ip->i_mtime = 0;
		ip->i_atime = 0;
		ip->i_ctime = 0;
	}

	/*
	 *	Coloca no final da lista LRU e retorna com o "ip" trancado
	 */
   put_end_lru:
	SPINLOCK (&inode_lru_lock);

	if (ip != inode_lru_head.i_lru_back)
	{
		ip->i_lru_back->i_lru_forw = ip->i_lru_forw;
		ip->i_lru_forw->i_lru_back = ip->i_lru_back;

		inode_lru_head.i_lru_back->i_lru_forw = ip;
		ip->i_lru_back = inode_lru_head.i_lru_back;

		inode_lru_head.i_lru_back = ip;
		ip->i_lru_forw = &inode_lru_head;
	}

	SPINFREE (&inode_lru_lock);

#ifdef	MSG
	if (CSWT (12))
		printf ("%g: %v, %d, count = %d\n", dev, ino, ip->i_count);
#endif	MSG

	return (ip);

}	/* end iget */

/*
 ****************************************************************
 *	Libera a Uso do Inode					*
 ****************************************************************
 */
void
iput (INODE *ip)
{
	SB		*sp = ip->i_sb;

#ifdef	MSG
	if (CSWT (12))
		printf ("%g: %v, %d, count = %d\n", ip->i_dev, ip->i_ino, ip->i_count);
#endif	MSG

	/*
	 *	Decrementa o uso na tabela INODE
	 */
	if (ip->i_count > 1)
	{
		/*
		 *	Primeiro caso: Ainda tem outros usuarios; nada faz
		 */
	}
	elif (ip->i_nlink > 0)
	{
		/*
		 *	Segundo caso: Não tem mais usuarios, mas tem ELOs
		 */
		if ((ip->i_mode & IFMT) == IFIFO)
			(*fstab[sp->sb_code].fs_trunc_inode) (ip);

		(*fstab[sp->sb_code].fs_write_inode) (ip);

#if (0)	/*******************************************************/
		ip->i_flags &= ~ILOCK;
#endif	/*******************************************************/
		
		/*
		 *	Libera eventos, semáforos e regiões associados ao inode
		 */
		switch (ip->i_union)
		{
		    case IN_EVENT:
			ueventgicrelease (ip);
			break;

		    case IN_SEMA:
			usemagicrelease (ip);
			break;

		    case IN_SHMEM:
			regiongrelease (ip->i_shmemg, 0);
			ip->i_shmemg = NOREGIONG;
			break;

		}	/* end switch */

		ip->i_union = IN_NULL;
	}
	else 	/* ip->i_count == 0 e ip->i_nlink == 0 */
	{
		/*
		 *	Terceiro caso: Não tem mais usuarios nem ELOs
		 *	i_mode = 0 significa o INODE desalocado no disco
		 */
		if (ip->i_mode != 0)
		{
			if (ip->i_nm != NOSTR)
				{ free_byte (ip->i_nm); ip->i_nm = NOSTR; }

			(*fstab[sp->sb_code].fs_trunc_inode) (ip);

			/* Zera os campos que em geral são copiados para disco */

			memclr (&ip->c, sizeof (ip->c));


#if (0)	/*******************************************************/
			/* Zera alguns campos que em geral são copiados para disco */

			memclr (&ip->i_atime, 3 * sizeof (time_t));
			memclr (ip->i_addr, sizeof (ip->i_addr));

			ip->i_rdev  = 0;
			ip->i_mode  = 0;
#endif	/*******************************************************/

			inode_dirty_inc (ip);

			(*fstab[sp->sb_code].fs_free_inode) (ip);

			(*fstab[sp->sb_code].fs_write_inode) (ip);
		}

		ip->i_flags = 0;

#if (0)	/*******************************************************/
		/*
		 *	Insere no início da lista LRU.
		 */
		SPINLOCK (&inode_lru_lock);

		ip->i_lru_back->i_lru_forw = ip->i_lru_forw;
		ip->i_lru_forw->i_lru_back = ip->i_lru_back;

		inode_lru_head.i_lru_forw->i_lru_back = ip;
		ip->i_lru_forw = inode_lru_head.i_lru_forw;

		inode_lru_head.i_lru_forw = ip;
		ip->i_lru_back = &inode_lru_head;

		SPINFREE (&inode_lru_lock);
#endif	/*******************************************************/
	}

	/*
	 *	Decrementa os Contadores
	 */
   /***	sp = ip->i_sb; ***/

	SPINLOCK (&sp->sb_mlock);
	sp->sb_mcount--;
	SPINFREE (&sp->sb_mlock);
			
	if (--ip->i_count <= 0)
		mon.y_inode_in_use_cnt--;

	SLEEPFREE (&ip->i_lock);

}	/* end iput */

/*
 ****************************************************************
 *	Incrementa o Uso do INODE				*
 ****************************************************************
 */
void
iinc (INODE *ip)
{
	SB		*sp;

#ifdef	MSG
	if (CSWT (12))
		printf ("%g: %v, %d, count = %d\n", ip->i_dev, ip->i_ino, ip->i_count);
#endif	MSG

	/*
	 *	Esta rotina, uma versão simplificada da "iget", pode ser usada
	 *	quando já se tem o "ip" e sabe-se que i_count já é > 0.
	 *	Recebe e Devolve o ip trancado
	 */
	sp = ip->i_sb;

	SPINLOCK (&sp->sb_mlock);
	sp->sb_mcount++;
	SPINFREE (&sp->sb_mlock);
			
	if (ip->i_lock == 0)
		printf ("%g: O INODE não estava TRANCADO\n");

	if (ip->i_count == 0)
		mon.y_inode_in_use_cnt++;
#if (0)	/*******************************************************/
		{ printf ("%g: O INODE não estava referenciado\n"); mon.y_inode_in_use_cnt++; }
#endif	/*******************************************************/

	ip->i_count++;

}	/* end iinc */

/*
 ****************************************************************
 *	Decrementa o Uso do INODE				*
 ****************************************************************
 */
void
idec (INODE *ip)
{
	SB		*sp;

#ifdef	MSG
	if (CSWT (12))
		printf ("%g: %v, %d, count = %d\n", ip->i_dev, ip->i_ino, ip->i_count);
#endif	MSG

	/*
	 *	Recebe e Devolve o ip Locked
	 */
	if (ip->i_count == 1)
		{ printf ("%g: Era a ultima referencia do INODE\n"); mon.y_inode_in_use_cnt--; }

	sp = ip->i_sb;

	SPINLOCK (&sp->sb_mlock);
	sp->sb_mcount--;
	SPINFREE (&sp->sb_mlock);
			
	ip->i_count--;

}	/* end idec */

/*
 ****************************************************************
 *	Insere um INODE da lista hash				*
 ****************************************************************
 */
int
inode_insert_hash (INODE *ip, IHASHTB *hp)
{
	INODE		*tst_ip;

	/*
	 *	Devolve:
	 *		 0 => Inseriu o INODE na fila HASH
	 *		-1 => O INODE já se encontra na fila HASH
	 *
	 */
	SPINLOCK (&hp->h_lock);

	for (tst_ip = hp->h_inode; /* abaixo */; tst_ip = tst_ip->i_next)
	{
		if (tst_ip == NOINODE)
			{ ip->i_next = hp->h_inode; hp->h_inode = ip; SPINFREE (&hp->h_lock); return (0); }

		if (tst_ip->i_dev == ip->i_dev && tst_ip->i_ino == ip->i_ino)
			{ SPINFREE (&hp->h_lock); return (-1); }
	}

}	/* end inode_insert_hash */

/*
 ****************************************************************
 *	Remove um INODE da fila HASH				*
 ****************************************************************
 */
void
inode_remove_hash (INODE *ip, IHASHTB *hp)
{
	INODE		*aip;

	SPINLOCK (&hp->h_lock);

	if (hp->h_inode == ip)
		{ hp->h_inode = ip->i_next; SPINFREE (&hp->h_lock); return; }

	for (aip = hp->h_inode; aip != NOINODE; aip = aip->i_next)
	{
		if (aip->i_next == ip)
			{ aip->i_next = ip->i_next; SPINFREE (&hp->h_lock); return; }
	}

	SPINFREE (&hp->h_lock);

	printf ("%g: INODE (%v, %d) não encontrado na fila HASH\n", ip->i_dev, ip->i_ino);

}	/* end inode_remove_hash */

/*
 ****************************************************************
 *	Inicialização da lista LRU				*
 ****************************************************************
 */
void
inode_lru_init (void)
{
	INODE		*ip, *aip;

	/*
	 *	Coloca todos os INODEs na lista LRU
	 */
	aip = &inode_lru_head;

   /***	SPINLOCK (&inode_lru_lock); ***/

	for (ip = scb.y_inode; ip < scb.y_endinode; aip = ip, ip++)
	{
		aip->i_lru_forw = ip;
		ip->i_lru_back  = aip;
	}

	/*
	 *	A lista LRU é circular.
	 */
	aip->i_lru_forw      = &inode_lru_head;
	inode_lru_head.i_lru_back = aip;

   /***	SPINFREE (&inode_lru_lock); ***/

}	/* end inode_lru_init */

/*
 ****************************************************************
 *	Remoção de INODEs (para UMOUNT)				*
 ****************************************************************
 */
void
iumerase (dev_t dev, int sb_code)
{
	INODE		*ip;

	/*
	 *	Percorre os INODEs
	 */
	for (ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_dev != dev)
			continue;

		if (SLEEPTEST (&ip->i_lock) < 0)
		{
		    resid:
			printf ("%g: INODE Residual: (%s: %v, %d)\n", ip->i_nm, dev, ip->i_ino);
			continue;
		}

		if (ip->i_count != 0)
			{ SLEEPFREE (&ip->i_lock); goto resid; }

		(*fstab[ip->i_sb->sb_code].fs_rel_inode_attr) (ip);

		inode_remove_hash (ip, IHASH (ip->i_dev, ip->i_ino));

		ip->i_ino  = NOINO;
		ip->i_dev  = NODEV;
		ip->i_mode = 0;

		/*
		 *	Insere no início da lista LRU.
		 */
		SPINLOCK (&inode_lru_lock);

		ip->i_lru_back->i_lru_forw = ip->i_lru_forw;
		ip->i_lru_forw->i_lru_back = ip->i_lru_back;

		inode_lru_head.i_lru_forw->i_lru_back = ip;
		ip->i_lru_forw = inode_lru_head.i_lru_forw;

		inode_lru_head.i_lru_forw = ip;
		ip->i_lru_back = &inode_lru_head;

		SPINFREE (&inode_lru_lock);

		SLEEPFREE (&ip->i_lock);

	}	/* end for (INODEs) */

}	/* end iumerase */

/*
 ****************************************************************
 *	Incrementa o contador de INODEs a atualizar		*
 ****************************************************************
 */
void
inode_dirty_inc (INODE *ip)
{
	/*
	 *	Supõe, naturalmente, que o INODE esteja trancado
	 */
	if (ip->i_flags & ICHG)
		return;

	ip->i_flags |= ICHG; mon.y_inode_dirty_cnt++;

}	/* inode_dirty_inc */

/*
 ****************************************************************
 *	Decrementa o contador de INODEs a atualizar		*
 ****************************************************************
 */
void
inode_dirty_dec (INODE *ip)
{
	/*
	 *	Supõe, naturalmente, que o INODE esteja trancado
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	ip->i_flags &= ~ICHG; mon.y_inode_dirty_cnt--;

}	/* inode_dirty_dec */

/*
 ****************************************************************
 *	Obtem o SB dado o DEV					*
 ****************************************************************
 */
SB *
sbget (dev_t dev)
{
	SB		*sp;
	const DISKTB	*up;

	/*
	 *	Devolve a entrada TRANCADA
	 */
	up = &disktb[MINOR (dev)];

	if (up->p_dev != dev)
		return (NOSB);

	if ((sp = up->p_sb) == NOSB)
		return (NOSB);

	SPINLOCK (&sp->sb_mlock);

	if (sp->sb_dev != dev)
		{ SPINFREE (&sp->sb_mlock); return (NOSB); }

	return (sp);

}	/* end sbget */

/*
 ****************************************************************
 *	Obtem o SB dado o INODE					*
 ****************************************************************
 */
SB *
sbiget (INODE *ip)
{
	SB		*sp;

	/*
	 *	Devolve a entrada TRANCADA
	 */
   again:
	SPINLOCK (&sblistlock);

	for (sp = sb_head.sb_forw; sp != &sb_head; sp = sp->sb_forw)
	{
		if (sp->sb_inode != ip)
			continue;

		SPINFREE (&sblistlock);

		SPINLOCK (&sp->sb_mlock);

		if (sp->sb_inode != ip)
			{ SPINFREE (&sp->sb_mlock); goto again; }

		return (sp);
	}

	SPINFREE (&sblistlock);

	return (NOSB);

}	/* end sbiget */
