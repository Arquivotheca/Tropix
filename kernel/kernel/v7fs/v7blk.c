/*
 ****************************************************************
 *								*
 *			v7blk.c					*
 *								*
 *	Alocação/desalocação de blocos do disco			*
 *								*
 *	Versão	3.0.0, de 22.11.94				*
 *		4.2.0, de 15.11.01				*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
#include "../h/bhead.h"
#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Mapeamento do endereco do bloco da V7			*
 ****************************************************************
 */
daddr_t
v7_block_map (INODE *ip, daddr_t lbn, int rw, daddr_t *rabno_ptr)
{
	long		i = 0;
	BHEAD		*bp;
	BHEAD		*nbp;
	int		j, sh;
	daddr_t 	pbn, rabno = 0, save_pbn;
	dev_t		dev;

	/*
	 *  Retorna:
	 *	< 0	=> READ:  O lbn não pertence ao Arquivo
	 *		   WRITE: Não conseguiu alocar um Bloco
	 *	= 0	=> Erro (lbn inválido ou erro de I/O)
	 *		   u.u_error contem o codigo de erro
	 *	> 0	=> O pbn
	 */

	/*
	 *	Verifica se o No. do BLOCO é válido
	 */
	if (lbn < 0)
		{ u.u_error = EFBIG; return (NODADDR); }

	dev = ip->i_dev;

	*rabno_ptr = NODADDR;

	/*
	 *	BLOCOS 0 a V7_NADDR-4 são diretos
	 */
	if (lbn < V7_NADDR-3)
	{
		i = lbn;

		/*
		 *	Verifica se este BLOCO já pertence ao Arquivo
		 */
		if ((pbn = ip->i_addr[i]) == NODADDR)
		{
			/*
			 *	O BLOCO não pertence ao Arquivo. Se ele deve
			 *	ser lido, ou se deve ser escrito e não há
			 *	blocos livres no disco entao retorna (-1)
			 */
			if (rw == B_READ || (pbn = v7_block_alloc (dev)) < 0)
				return (-1);

			bp = bread (dev, pbn, 0);
			memclr (bp->b_addr, BLSZ);
			bp->b_residual = 0;

			/*
			 *	Atualiza o INODE com este BLOCO. Repare que o
			 *	Bloco é marcado para ser escrito
			 */
			bdwrite (bp);

			ip->i_addr[i] = pbn;
			ip->i_mtime = time;
			inode_dirty_inc (ip);
		}

		/*
		 *	Calcula um BLOCO de "Lookahead"
		 */
		if (i < V7_NADDR-4)
			*rabno_ptr = ip->i_addr[i+1];

		return (pbn);
	}

	/*
	 *	Os Enderecos V7_NADDR-1, V7_NADDR-2 e V7_NADDR-3 são BLOCOS indiretos.
	 *	O Primeiro passo é determinar o nivel de indireção
	 */
	sh = 0;
	pbn = 1;
	lbn -= V7_NADDR-3;

	for (j = 3; j > 0; j--)
	{
		sh += V7_NSHIFT;
		pbn <<= V7_NSHIFT;

		if (lbn < pbn)
			break;

		lbn -= pbn;
	}

	/*
	 *	Neste ponto, 4-j indica o nivel de indireção.
	 *	j == 0 indica nivel 4 (não implementado)
	 */
	if (j == 0)
		{ u.u_error = EFBIG; return (NODADDR); }

	/*
	 *	Processa o Primeiro Bloco indireto
	 */
	if ((pbn = ip->i_addr[V7_NADDR-j]) == NODADDR)
	{
		if (rw == B_READ || (pbn = v7_block_alloc (dev)) < 0)
			return (-1);

		bp = bread (dev, pbn, 0);
		memclr (bp->b_addr, BLSZ);
		bp->b_residual = 0;

		/*
		 *	Atualiza o INODE com este BLOCO
		 */
		bdwrite (bp);

		ip->i_addr[V7_NADDR-j] = pbn;
		ip->i_mtime = time;
		inode_dirty_inc (ip);
	}

	/*
	 *	Calcula o No. do BLOCO, consultando os BLOCOS Indiretos
	 */
	for (/* vazio */;  j <= 3;  j++)
	{
		bp = bread (dev, pbn, 0);

		if (bp->b_flags & B_ERROR)
			{ block_put (bp); return (NODADDR); }

		save_pbn = pbn;

		sh -= V7_NSHIFT;
		i = (lbn >> sh) & (V7_NDAINBL-1);

#ifdef	LITTLE_ENDIAN
		pbn = long_endian_cv (((daddr_t *)bp->b_addr)[i]);
#else
		pbn = ((daddr_t *)bp->b_addr)[i];
#endif	LITTLE_ENDIAN

		rabno = ((daddr_t *)bp->b_addr)[i+1];

		block_put (bp);		/* Libera logo */

		if (pbn == 0)
		{
			if (rw == B_READ || (pbn = v7_block_alloc (dev)) < 0)
				return (-1);

			nbp = bread (dev, pbn, 0);
			memclr (nbp->b_addr, BLSZ);
			nbp->b_residual = 0;

			bdwrite (nbp);

			bp = bread (dev, save_pbn, 0);

			if (bp->b_flags & B_ERROR)
				{ block_put (bp); return (NODADDR); }

#ifdef	LITTLE_ENDIAN
			((daddr_t *)bp->b_addr)[i] = long_endian_cv (pbn);
#else
			((daddr_t *)bp->b_addr)[i] = pbn;
#endif	LITTLE_ENDIAN

			bdwrite (bp);
		}
	}

	/*
	 *	Calcula o "Read-ahead"
	 */
#ifdef	LITTLE_ENDIAN
	if (i < V7_NDAINBL-1)
		*rabno_ptr = long_endian_cv (rabno);
#else
	if (i < V7_NDAINBL-1)
		*rabno_ptr = rabno;
#endif	LITTLE_ENDIAN

	return (pbn);

}	/* end v7_block_map */

/*
 ****************************************************************
 *	Obtém um BLOCO Livre do Disco				*
 ****************************************************************
 */
daddr_t
v7_block_alloc (dev_t dev)
{
	daddr_t		blkno;
	SB		*sp;
	V7SB		*v7sp;
	BHEAD		*bp;
#ifdef	LITTLE_ENDIAN
	int		i;
	daddr_t		*sdp, *ddp;
#endif	LITTLE_ENDIAN

	if ((sp = sbget (dev)) == NOSB)
		{ printf ("%g: Dispositivo não montado: \"%v\"\n", dev); u.u_error = ENODEV; return (-1); }

	v7sp = sp->sb_ptr;

	sbput (sp);

	SLEEPLOCK (&v7sp->s_flock, PINODE);

	/*
	 *	Tenta obter um BLOCO da lista de blocos livres do SB
	 */
    again:
	if (v7sp->s_nfblk <= 0)
		goto nospace;

	if (v7sp->s_nfblk > V7_SBFBLK)
	{
		printf ("%g: Lista de blocos livres inválida em \"%v\"\n", dev);
		goto nospace;
	}

	if ((blkno = v7sp->s_fblk[--v7sp->s_nfblk]) == NODADDR)
		goto nospace;

	if (blkno < v7sp->s_isize || blkno >= *(daddr_t *)v7sp->s_fsize)
		{ printf ("%g: No. de bloco inválido (%v, %d)\n", dev, blkno); goto  again; }

	/*
	 *	A lista de BLOCOS livres do SB está vazia. Le um bloco
	 *	da lista livre do disco, e preenche a lista do SB
	 */
	if (v7sp->s_nfblk <= 0)
	{
		bp = bread (dev, blkno, 0);

		if ((bp->b_flags & B_ERROR) == 0)
		{
#ifdef	LITTLE_ENDIAN
			v7sp->s_nfblk = short_endian_cv (((V7FREEBLK *)bp->b_addr)->r_nfblk);

			sdp = (daddr_t *)(((V7FREEBLK *)bp->b_addr)->r_fblk);
			ddp = v7sp->s_fblk;

			for (i = 0; i < V7_SBFBLK; i++)
				*ddp++ = long_endian_cv (*sdp++);
#else
			v7sp->s_nfblk = ((V7FREEBLK *)bp->b_addr)->r_nfblk;

			memmove
			(	v7sp->s_fblk,
				((V7FREEBLK *)bp->b_addr)->r_fblk,
				sizeof (v7sp->s_fblk)
			);
#endif	LITTLE_ENDIAN
		}

		block_put (bp);

		if (v7sp->s_nfblk <= 0)
			goto nospace;
	}

	/*
	 *	Foi obtido um Bloco do Disco
	 */
	(*(daddr_t *)v7sp->s_tfblk)--;
	sp->sb_sbmod = 1;

	SLEEPFREE (&v7sp->s_flock);

	return (blkno);

	/*
	 *	O Sistema de Arquivos não tem mais BLOCOS livres,
	 *	ou então a Lista está destruida
	 */
    nospace:
	v7sp->s_nfblk = 0;

	SLEEPFREE (&v7sp->s_flock);

	printf ("%g: Sistema de arquivos cheio: \"%v\"\n", dev);

	u.u_error = ENOSPC; return (-1);

}	/* end v7_block_alloc */

/*
 ****************************************************************
 *	Devolve um BLOCO à Lista Livre do Disco			*
 ****************************************************************
 */
void
v7_block_release (dev_t dev, daddr_t blkno)
{
	SB		*sp;
	V7SB		*v7sp;
	BHEAD		*bp;
#ifdef	LITTLE_ENDIAN
	int		i;
	daddr_t		*sdp, *ddp;
#endif	LITTLE_ENDIAN

	if ((sp = sbget (dev)) == NOSB)
		{ printf ("%g: Dispositivo não montado: \"%v\"\n", dev); u.u_error = ENODEV; return; }

	v7sp = sp->sb_ptr;

	sbput (sp);

	if (blkno < v7sp->s_isize || blkno >= *(daddr_t *)v7sp->s_fsize)
		{ printf ("%g: No. de bloco inválido (%v, %d)\n", dev, blkno); return; }

	SLEEPLOCK (&v7sp->s_flock, PINODE);

	if (v7sp->s_nfblk <= 0)
	{
		/*
		 *	Se a Lista Livre do SB estiver vazia, coloca nela o
		 *	bloco de endereco nulo, que será interpretado como
		 *	o final da lista no disco
		 */
		v7sp->s_nfblk = 1;
		v7sp->s_fblk[0] = NODADDR;
	}
	elif (v7sp->s_nfblk >= V7_SBFBLK)
	{
		/*
		 *	Se a Lista Livre do SB estiver completa, escreve-a no
		 *	Sistema de Arquivos
		 */
#if (0)	/*******************************************************/
		bp = block_get (dev, blkno, 0);
#endif	/*******************************************************/
		bp = bread (dev, blkno, 0);

#ifdef	LITTLE_ENDIAN
		((V7FREEBLK *)bp->b_addr)->r_nfblk = short_endian_cv (v7sp->s_nfblk);

		sdp = v7sp->s_fblk;
		ddp = (daddr_t *)(((V7FREEBLK *)bp->b_addr)->r_fblk);

		for (i = 0; i < V7_SBFBLK; i++)
			*ddp++ = long_endian_cv (*sdp++);
#else
		((V7FREEBLK *)bp->b_addr)->r_nfblk = v7sp->s_nfblk;

		memmove
		(	((V7FREEBLK *)bp->b_addr)->r_fblk,
			v7sp->s_fblk,
			sizeof (v7sp->s_fblk)
		);
#endif	LITTLE_ENDIAN

		v7sp->s_nfblk = 0;

		/*
		 *	Escreve sem maiores delongas
		 */
		bwrite (bp);
	}

	/*
	 *	Finalmente coloque o bloco dado na Lista Livre do SB
	 */
	v7sp->s_fblk[v7sp->s_nfblk++] = blkno;
	(*(daddr_t *)v7sp->s_tfblk)++;
	sp->sb_sbmod = 1;

	SLEEPFREE (&v7sp->s_flock);

}	/* end v7_block_release */
