/*
 ****************************************************************
 *								*
 *			pseudo_dev.c				*
 *								*
 *	Simula um dispositivo a partir de um arquivo regular	*
 *								*
 *	Versão	4.6.0, de 26.07.04				*
 *		4.6.0, de 04.10.04				*
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
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/disktb.h"
#include "../h/bhead.h"
#include "../h/inode.h"
#include "../h/iotab.h"
#include "../h/devmajor.h"
#include "../h/devhead.h"
#include "../h/t1.h"
#include "../h/fat.h"
#include "../h/cdfs.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
enum { NPSEUDODEV = 4 };	/* Número máximo */

#undef	DEBUG			/* Controla a depuração */

entry DEVHEAD	pseudotab;	/* Cabeca da lista de dp's e do major */

entry DISKTB	*pd_disktb,	/* Começo e final das entradas de "pd" */
		*end_pd_disktb;

entry LOCK	pd_disktb_lock;	/* O semáforo da tabela */

/*
 ****************************************************************
 *	Aloca um elemento do vetor de INODEs			*
 ****************************************************************
 */
dev_t
pseudo_alloc_dev (INODE *ip)
{
	dev_t		dev;
	DISKTB		*up;
	int		i;
	char		dev_nm[16];

	/*
	 *	Prepara a tabela de PSEUDO discos na DISKTB
	 */
	SPINLOCK (&pd_disktb_lock);

	if (pd_disktb == NODISKTB)
	{
		if (next_disktb + NPSEUDODEV + 1 >= end_disktb)
		{
			printf ("pseudo_alloc_dev: Tabela de Discos/Partições cheia\n");
			SPINFREE (&pd_disktb_lock);
			u.u_error = EBUSY; return (-1);
		}

		pd_disktb = next_disktb; next_disktb = end_pd_disktb = pd_disktb + NPSEUDODEV;

		next_disktb->p_name[0] = '\0';

		memclr (pd_disktb, NPSEUDODEV * sizeof (DISKTB));

		for (up = pd_disktb, i = 0; i < NPSEUDODEV; up++, i++)
		{
			sprintf (up->p_name, "pd%d", i);
			up->p_dev = MAKEDEV (PSEUDO_MAJOR, up - disktb);

			sprintf (dev_nm, "/dev/pd%d", i);
			kmkdev (dev_nm, IFBLK|0740, up->p_dev);

			sprintf (dev_nm, "/dev/rpd%d", i);
			kmkdev (dev_nm, IFCHR|0640, up->p_dev);
		}
	}

	/*
	 *	Procura uma entrada livre
	 */
	for (up = pd_disktb; /* abaixo */; up++)
	{
		if (up >= end_pd_disktb)
			{ SPINFREE (&pd_disktb_lock); u.u_error = EBUSY; return (-1); }

		if (up->p_inode == NOINODE)
			break;
	}

	/*
	 *	Prepara a entrada
	 */
	memclr (up, sizeof (DISKTB));

	sprintf (up->p_name, "pd%d", up - pd_disktb);

	up->p_inode  = ip;
   /***	up->p_type   = 0; ***/
   /***	up->p_offset = 0; ***/
	up->p_size   = BYTOBL (ip->i_size);
	up->p_dev    = dev = MAKEDEV (PSEUDO_MAJOR, up - disktb);

	SPINFREE (&pd_disktb_lock);

	return (dev);

}	/* end pseudo_alloc_dev */

/*
 ****************************************************************
 *	Procura a entrada do vetor dado o INODE			*
 ****************************************************************
 */
dev_t
pseudo_search_dev (INODE *ip)
{
	const DISKTB	*up;

	if ((up = pd_disktb) == NODISKTB)
		return (NODEV);

	for (/* acima */; up < end_pd_disktb; up++)
	{
		if (up->p_inode == ip)
			return (up->p_dev);
	}

	return (NODEV);

}	/* end pseudo_search_dev */

/*
 ****************************************************************
 *	Libera um elemento do vetor de INODEs			*
 ****************************************************************
 */
int
pseudo_free_dev (dev_t dev)
{
	DISKTB		*up;

	/*
	 *	Obtém a entrada DISKTB
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		{ u.u_error = ENXIO; return (-1); }

	memclr (up, sizeof (DISKTB));

	sprintf (up->p_name, "pd%d", up - pd_disktb);
	up->p_dev = dev;

	return (0);

}	/* end pseudo_free_dev */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
pseudostrategy (BHEAD *bp)
{
	const DISKTB	*up;
	INODE		*ip;
	daddr_t		lbn;

	/*
	 *	Obtém o INODE
	 */
	up = &disktb[MINOR (bp->b_dev)];

	if ((ip = up->p_inode) == NOINODE)
		goto bad;

	if ((lbn = bp->b_blkno) < 0 || lbn + BYTOBL (bp->b_base_count) > BYTOBL (ip->i_size))
		goto bad;

   /***	SLEEPLOCK (&ip->i_lock, PINODE); ***/

	/*
	 *	Analisa o sistema de arquivos REAL
	 */
	switch (ip->i_fsp->fs_code)
	{
	    case FS_FAT:
	    {
		const FATSB	*sp = ip->i_sb->sb_ptr;

		if (sp->s_CLUSTER_SZ < BL4SZ)
			goto case_default;
	    }

	    /* continua abaixo */

	    case FS_T1:
	    case FS_CD:
	    {
		if (MAJOR (bp->b_phdev) == PSEUDO_MAJOR)
			goto bad_with_msg;

		if ((*biotab[MAJOR (bp->b_phdev)].w_strategy) (bp) < 0)
			goto bad;

		break;
	    }

	    case_default:
	    default:
	    {
		IOREQ		io;

		io.io_ip	= ip;
		io.io_area	= bp->b_base_addr;
		io.io_count	= bp->b_base_count;
		io.io_offset	= (loff_t)lbn << BLSHIFT;
		io.io_cpflag	= SS;

		if ((bp->b_flags & (B_READ|B_WRITE)) == B_WRITE)
			(*ip->i_fsp->fs_write) (&io);
		else
			(*ip->i_fsp->fs_read) (&io);

		if (u.u_error != 0)
			goto bad;

		EVENTDONE (&bp->b_done);

		break;
	    }
	}

	/*
	 *	Epílogo
	 */
   /***	SLEEPFREE (&ip->i_lock); ***/

	return (0);

	/*
	 *	Houve algum erro
	 */
    bad_with_msg:
	printf ("%g: Bloco (%f, %v, %d) ainda NÃO convertido\n", ip->i_fsp->fs_code, bp->b_dev, bp->b_blkno);
    bad:
	bp->b_error = ENXIO;
	bp->b_flags |= B_ERROR;
   /***	SLEEPFREE (&ip->i_lock); ***/
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end pseudostrategy */

/*
 ****************************************************************
 *	Aloca um bloco para o PSEUDO				*
 ****************************************************************
 */
void
pseudo_alloc_bhead (BHEAD *bp)
{
	const DISKTB	*up;
	INODE		*ip;
	daddr_t		lbn;

	/*
	 *	Obtém o INODE
	 */
	up = &disktb[MINOR (bp->b_dev)];

	if ((ip = up->p_inode) == NOINODE)
		{ printf ("%g: Pseudodisco %v sem INODE\n", bp->b_dev); u.u_error = ENXIO; return; }

	if ((lbn = bp->b_blkno) < 0 || lbn + BYTOBL (bp->b_base_count) > BYTOBL (ip->i_size))
		{ u.u_error = ENXIO; return; }

	/*
	 *	Analisa o sistema de arquivos REAL
	 */
	switch (ip->i_fsp->fs_code)
	{
	    case FS_T1:
	    {
		daddr_t		pbn, rabn;

		pbn = t1_block_map (ip, lbn >> (BL4SHIFT - BLSHIFT), B_WRITE, &rabn);

		if (u.u_error)
			return;

		if (rabn == (daddr_t)-1)	/* Bloco acabou de ser alocado */
			memclr (bp->b_base_addr, BL4SZ);

		bp->b_phdev = ip->i_dev; bp->b_phblkno = T1_BL4toBL (pbn);
#ifdef	DEBUG
		printf
		(	"pseudo_alloc_bhead: aloquei (%v, %d) => (%v, %d)\n",
			bp->b_dev, bp->b_blkno, bp->b_phdev, bp->b_phblkno
		);
#endif	DEBUG
		break;
	    }

	    case FS_FAT:
	    {
		const FATSB	*sp = ip->i_sb->sb_ptr;
		off_t		offset;
		int		off, log_clusno, ph_clusno;

		if (sp->s_CLUSTER_SZ < BL4SZ)
			break;

		offset = (lbn << BLSHIFT);

		log_clusno = offset >> sp->s_CLUSTER_SHIFT;
		off	   = offset &  sp->s_CLUSTER_MASK;

		if (C_ISBAD_OR_EOF (ph_clusno = fat_cluster_read_map (ip, log_clusno)))
			{ printf ("%g: CLUSTER NÃO alocado, dev = \"%v\"\n", ip->i_dev); break; }

		bp->b_phdev   = ip->i_dev;
		bp->b_phblkno = FIRST_BLK (ph_clusno) + T1_BL4toBL (off >> BL4SHIFT);
#ifdef	DEBUG
		printf
		(	"pseudo_alloc_bhead: lbn = %d, log_clusno = %d, ph_clusno = %d\n",
			lbn, log_clusno, ph_clusno
		);
		printf
		(	"pseudo_alloc_bhead: aloquei (%v, %d) => (%v, %d)\n",
			bp->b_dev, bp->b_blkno, bp->b_phdev, bp->b_phblkno
		);
#endif	DEBUG
		break;
	    }

	    case FS_CD:
	    {
		daddr_t		pbn;

		pbn = ip->i_first_block + (lbn >> (ISO_BLSHIFT - BLSHIFT));

		bp->b_phdev = ip->i_dev; bp->b_phblkno = ISOBL_TO_BL (pbn);

		break;
	    }
	}

}	/* end pseudo_alloc_bhead */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
pseudoctl (dev_t dev, int cmd, int arg, int flag)
{
	const DISKTB	*up;
	const INODE	*ip;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if ((ip = up->p_inode) == NOINODE)
		{ u.u_error = ENXIO; return (-1); }

	dev = ip->i_dev;

	/*
	 *	Chama a rotina de IOCTL do Dispositivo
	 */
	return ((*biotab[MAJOR (dev)].w_ioctl) (dev, cmd, arg, flag));

}	/* end pseudoctl */
