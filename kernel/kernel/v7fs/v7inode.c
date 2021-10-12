/*
 ****************************************************************
 *								*
 *			v7inode.c				*
 *								*
 *	Alocação e desalocação de INODEs no disco		*
 *								*
 *	Versão	3.0.0, de 24.11.94				*
 *		4.2.0, de 18.11.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Lê e converte um INODE V7FS do disco para a memória	*
 ****************************************************************
 */
int
v7_read_inode (INODE *ip)
{
	BHEAD		*bp;
	V7DINO		*dp;
	char		*s, *d;
	int		i;
	long		ifmt;

	/*
	 *	Le o bloco correspondente
	 */
	bp = bread (ip->i_dev, V7_INTODA (ip->i_ino), 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	dp = (V7DINO *)bp->b_addr + V7_INTODO (ip->i_ino);

	/*
	 *	Copia diversos campos em comum
	 */
#ifdef	LITTLE_ENDIAN
	ip->i_mode  = short_endian_cv (dp->n_mode) | ((unsigned)dp->n_emode << 16);
	ip->i_nlink = short_endian_cv (dp->n_nlink);
	ip->i_uid   = short_endian_cv (dp->n_uid);
	ip->i_gid   = short_endian_cv (dp->n_gid);
	ip->i_size  = long_endian_cv  (dp->n_size);

	ip->i_atime = long_endian_cv (dp->n_atime);
	ip->i_mtime = long_endian_cv (dp->n_mtime);
	ip->i_ctime = long_endian_cv (dp->n_ctime);
#else
	ip->i_mode  = dp->n_mode | ((long)dp->n_emode << 16);
	ip->i_nlink = dp->n_nlink;
	ip->i_uid   = dp->n_uid;
	ip->i_gid   = dp->n_gid;
	ip->i_size  = dp->n_size;

	ip->i_atime = dp->n_atime;
	ip->i_mtime = dp->n_mtime;
	ip->i_ctime = dp->n_ctime;
#endif	LITTLE_ENDIAN

	/*
	 *	Converte os V7_NADDR endereços de blocos de disco do formato
	 *	de 3 bytes para 4 bytes. Repare que o byte no. 40 contem
	 *	a parte alta do modo
	 */
	d = (char *)ip->i_addr;
	s = (char *)dp->n_addr;

#ifdef	LITTLE_ENDIAN
	for (i = 0; i < V7_NADDR; i++)
	{
		d[0] = s[2];
		d[1] = s[1];
		d[2] = s[0];
		d[3] = 0;

		d += 4;
		s += 3;
	}
#else
	for (i = 0; i < V7_NADDR; i++)
	{
		*d++ = 0;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}
#endif	LITTLE_ENDIAN

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = ip->i_mode & IFMT) == IFCHR)
	{
		ip->i_fsp = &fstab[FS_CHR];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	elif (ifmt == IFBLK)
	{
		ip->i_fsp = &fstab[FS_BLK];
		ip->i_rdev = (dev_t)ip->i_addr[0];
	}
	else
	{
		ip->i_fsp = &fstab[FS_V7];
		ip->i_rdev = 0;
	}

	block_put (bp);

	return (0);

}	/* end v7_read_inode */

/*
 ****************************************************************
 *	Converte o formato do INODE: memória para disco		*
 ****************************************************************
 */
void
v7_write_inode (INODE *ip)
{
	V7DINO		*dp;
	char		*s, *d;
	BHEAD		*bp;
	int		i;
	long		ifmt;

	/*
	 *	Verifica se não houve modificação no INODE
	 *	ou entao ele está montado "Read only"
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

	/*
	 *	Le o bloco contendo o INODE do disco
	 */
	bp = bread (ip->i_dev, V7_INTODA (ip->i_ino), 0);

	if (bp->b_flags & B_ERROR)
	{
		printf ("%g: Erro na leitura do INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
		block_put (bp); return;
	}

	/*
	 *	Copia diversos campos em comum
	 */
	dp = (V7DINO *)bp->b_addr + V7_INTODO (ip->i_ino);

#ifdef	LITTLE_ENDIAN
	dp->n_mode  = short_endian_cv (ip->i_mode);
	dp->n_emode = ip->i_mode >> 16;
	dp->n_nlink = short_endian_cv (ip->i_nlink);
	dp->n_uid   = short_endian_cv (ip->i_uid);
	dp->n_gid   = short_endian_cv (ip->i_gid);
	dp->n_size  = long_endian_cv  (ip->i_size);
#else
	dp->n_mode  = ip->i_mode;
	dp->n_emode = ip->i_mode >> 16;
	dp->n_nlink = ip->i_nlink;
	dp->n_uid   = ip->i_uid;
	dp->n_gid   = ip->i_gid;
	dp->n_size  = ip->i_size;
#endif	LITTLE_ENDIAN

	/*
	 *	Converte os Enderecos do formato de 4 para 3 bytes
	 */
	if ((ifmt = ip->i_mode & IFMT) == IFBLK || ifmt == IFCHR)
		ip->i_addr[0] = (daddr_t)ip->i_rdev;

	d = (char *)dp->n_addr;
	s = (char *)ip->i_addr;

#ifdef	LITTLE_ENDIAN
	for (i = 0; i < V7_NADDR; i++)
	{
		if (s[3] != 0)
			printf ("%g: Dispositivo \"%v\" com endereço inválido (> 2^24)\n", ip->i_dev);

		d[0] = s[2];
		d[1] = s[1];
		d[2] = s[0];

		s += 4;
		d += 3;
	}
#else
	for (i = 0; i < V7_NADDR; i++)
	{
		if (*s++ != 0)
			printf ("%g: Dispositivo \"%v\" com endereço inválido (> 2^24)\n", ip->i_dev);

		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}
#endif	LITTLE_ENDIAN

	/*
	 *	Atualiza os tempos
	 */
#ifdef	LITTLE_ENDIAN
	dp->n_atime = long_endian_cv (ip->i_atime);
	dp->n_mtime = long_endian_cv (ip->i_mtime);
	dp->n_ctime = long_endian_cv (ip->i_ctime);
#else
	dp->n_atime = ip->i_atime;
	dp->n_mtime = ip->i_mtime;
	dp->n_ctime = ip->i_ctime;
#endif	LITTLE_ENDIAN

	bdwrite (bp);

#ifdef	MSG
	if (CSWT (6))
	    printf ("%g: Atualizando o INODE (%v, %d)\n", ip->i_dev, ip->i_ino);
#endif	MSG

}	/* end v7_write_inode */

/*
 ****************************************************************
 *	Cria um arquivo novo					*
 ****************************************************************
 */
INODE *
v7_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip;
	int		ifmt;

	/*
	 *	Tenta obter um INODE no Dispositivo do pai
	 */
	if ((ip = v7_alloc_inode (iop->io_ip->i_dev)) == NOINODE)
		{ iput (iop->io_ip); return (NOINODE); }

	/*
	 *	Prepara o INODE
	 */
	if ((mode & IFMT) == 0)
		mode |= IFREG;

	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_euid;
	ip->i_gid = u.u_egid;

	ip->i_atime = time;
	ip->i_mtime = time;
	ip->i_ctime = time;

	inode_dirty_inc (ip);

	/*
	 *	Obtém o código do sistema de arquivos
	 */
	if   ((ifmt = mode & IFMT) == IFCHR)
		ip->i_fsp = &fstab[FS_CHR];
	elif (ifmt == IFBLK)
		ip->i_fsp = &fstab[FS_BLK];
	else
		ip->i_fsp = &fstab[FS_V7];

	/*
	 *	Escreve o nome do novo arquivo no Diretorio pai
	 */
	dep->d_ino = ip->i_ino;

	v7_write_dir (iop, dep, ip, 0 /* NO rename */);

	return (ip);

}	/* end v7_make_inode */

/*
 ****************************************************************
 *	Libera todos os blocos de um arquivo			*
 ****************************************************************
 */
void
v7_trunc_inode (INODE *ip)
{
	int		i;
	long		ifmt;
	dev_t		dev;
	daddr_t		bn;

	/*
	 *	Somente trunca arquivos de certo tipo
	 */
	ifmt = ip->i_mode & IFMT;

	if (ifmt == IFBLK || ifmt == IFCHR)
		return;

	dev = ip->i_dev;

	/*
	 *	Percorre os diversos niveis de indireção
	 */
	for (i = V7_NADDR-1; i >= 0; i--)
	{
		if ((bn = ip->i_addr[i]) == NODADDR)
			continue;

		ip->i_addr[i] = NODADDR;

		/*
		 *	Verifica qual o nivel de indireção
		 */
		switch (i)
		{
			default:
				v7_block_release (dev, bn);
				break;

			case V7_NADDR-3:
				v7_free_indir_blk (dev, bn, 1);
				break;

			case V7_NADDR-2:
				v7_free_indir_blk (dev, bn, 2);
				break;

			case V7_NADDR-1:
				v7_free_indir_blk (dev, bn, 3);
		}
	}

	/*
	 *	Epilogo
	 */
	ip->i_size = 0;

	ip->i_mtime = time;
	inode_dirty_inc (ip);

}	/* end v7_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
v7_free_inode (const INODE *ip)
{
	SB		*sp;
	V7SB		*v7sp;

	/*
	 *	Obtem o SB do Dispositivo
	 */
	if ((sp = sbget (ip->i_dev)) == NOSB)
		{ printf ("%g: Dispositivo \"%v\" NÃO montado\n", ip->i_dev); return; }

	v7sp = sp->sb_ptr;

	sbput (sp);

	SLEEPLOCK (&v7sp->s_ilock, PINODE);

	v7sp->s_tfdino++;

	/*
	 *	Se a lista do SB está cheia, simplemente abandona este INO
	 */
	if (v7sp->s_nfdino < V7_SBFDINO)
	{
		v7sp->s_fdino[v7sp->s_nfdino++] = ip->i_ino;

		sp->sb_sbmod = 1;
	}

	SLEEPFREE (&v7sp->s_ilock);

}	/* end v7_free_inode */

/*
 ****************************************************************
 *	Aloca um INODE no disco (e na memória)			*
 ****************************************************************
 */
INODE *
v7_alloc_inode (dev_t dev)
{
	SB		*sp;
	V7SB		*v7sp;
	INODE		*ip;
	int		i;
	ino_t		ino;
	BHEAD		*bp;
	V7DINO		*dp;
	daddr_t 	daddr;
	IHASHTB 	*hp;

	/*
	 *	Obtem o SB do dispositivo
	 */
	if ((sp = sbget (dev)) == NOSB)
		{ printf ("%g: Dispositivo \"%v\" NÃO montado\n", dev); return (NOINODE); }

	v7sp = sp->sb_ptr;

	sbput (sp);

	SLEEPLOCK (&v7sp->s_ilock, PINODE);

	/*
	 *	Inicia uma Malha pela lista de INODEs
	 *	livres do SB, procurando um adequado
	 */
    again:
	if (v7sp->s_nfdino > 0)
	{
		if ((ino = v7sp->s_fdino[--v7sp->s_nfdino]) < V7_ROOT_INO)
			goto again;

		if ((ip = iget (dev, ino, 0)) == NOINODE)
			{ printf ("%g: Não obtive o INODE (%v, %d)\n", dev, ino); return (NOINODE); }

		/*
		 *	Verifica se o INODE está livre
		 */
		if (ip->i_mode != 0)
		{
			printf ("%g: INODE Inválido na lista livre do dispositivo \"%v\"\n", dev);
			iput (ip); goto again;
		}

		/*
		 *	Achou um INODE livre
		 */
		memclr (ip->i_addr, sizeof (ip->i_addr));

		v7sp->s_tfdino--;
		sp->sb_sbmod = 1;

		SLEEPFREE (&v7sp->s_ilock);

		return (ip);

	}	/* end if (busca pelos inodes livres do SB) */

	/*
	 *	A lista de INODES livres do SB está vazia. Faz uma busca pelo
	 *	Sistema de Arquivos para coletar INODES livres. Repare que
	 *	s_ilock está LOCKED
	 */
	ino = 1;

	for (daddr = V7_SBNO+1; daddr < (daddr_t)v7sp->s_isize; daddr++)
	{
		/*
		 *	Le um Bloco da I-list
		 */
		bp = bread (dev, daddr, 0);

		if (bp->b_flags & B_ERROR)
			{ block_put (bp); ino += V7_INOPBL; continue; }

		/*
		 *	Examina cada I-number deste bloco
		 */
		dp = (V7DINO *)bp->b_addr;

		for (i = 0; i < V7_INOPBL; i++)
		{
			if (dp->n_mode != 0)
				goto cont;

			/*
			 *	Verifica se este INODE livre está no cache de INODEs
			 */
			hp = IHASH (dev, ino);

			SPINLOCK (&hp->h_lock);

			for (ip = hp->h_inode; ip != NOINODE; ip = ip->i_next)
			{
				if (ino == ip->i_ino && dev == ip->i_dev)
				{
					/* Já está na Memoria */

					SPINFREE (&hp->h_lock);
					goto cont;
				}
			}

			SPINFREE (&hp->h_lock);

			/*
			 *	O INODE livre encontrado não está no cache na
			 *	memoria. Insere na lista livre do SB
			 */
			v7sp->s_fdino[v7sp->s_nfdino++] = ino;

			if (v7sp->s_nfdino >= V7_SBFDINO)
				break;

			/*
			 *	Examina o INODE seguinte
			 */
		    cont:
			ino++;
			dp++;

		}	/* end for (varrendo este bloco) */

		block_put (bp);

		if (v7sp->s_nfdino >= V7_SBFDINO)
			break;

	}	/* end for (varrendo a I-list do Disco) */

	if (v7sp->s_nfdino > 0)
		goto again;

	SLEEPFREE (&v7sp->s_ilock);

	printf ("%g: O sistema de arquivos \"%v\" não tem mais INODEs livres\n", dev);

	u.u_error = ENOSPC;

	return (NOINODE);

}	/* end v7_alloc_inode */

/*
 ****************************************************************
 *	Libera blocos indiretos					*
 ****************************************************************
 */
void
v7_free_indir_blk (dev_t dev, daddr_t bn, int level)
{
	int		i;
	BHEAD		*bp;
	daddr_t		ib;
	daddr_t		*addr;

	/*
	 *	Copia o bloco em uma área temporária, para
	 *	logo poder liberá-lo.
	 */
	if ((addr = malloc_byte (BLSZ)) == NOVOID)
		return;

	bp = bread (dev, bn, 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); free_byte (addr); return; }

	memmove (addr, bp->b_addr, BLSZ);

	block_put (bp);

	/*
	 *	Percorre todos os enderecos deste bloco
	 */
	for (i = V7_NDAINBL - 1; i >= 0; i--)
	{
		if ((ib = ENDIAN_LONG (addr[i])) == NODADDR)
			continue;

		if (level > 1)
			v7_free_indir_blk (dev, ib, level - 1);
		else
			v7_block_release (dev, ib);
	}

	/*
	 *	Epílogo: libera o bloco dado.
	 */
	v7_block_release (dev, bn);

	free_byte (addr);

}	/* end v7_free_indir_blk */
