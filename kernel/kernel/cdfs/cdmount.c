/*
 ****************************************************************
 *								*
 *			cdmount.c				*
 *								*
 *	Montagem do Sistema de Arquivos ISO9660			*
 *								*
 *	Versão	4.0.0, de 31.01.02				*
 *		4.8.0, de 19.01.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stddef.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"
#include "../h/map.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/cdfs.h"
#include "../h/cdio.h"
#include "../h/ustat.h"
#include "../h/iotab.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/kfile.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Certifica-se de que o sistema de arquivos é ISO9660	*
 ****************************************************************
 */
int
cd_authen (dev_t dev)
{
	BHEAD		*bp;
	int		status = -1;

	u.u_error = NOERR;

	bp = bread (dev, ISOBL_TO_BL (ISO_SBNO), 0);

	if   (u.u_error)
		/* vazio */;
	elif (!memeq (((CDVOL *)bp->b_addr)->cd_id, ISO_STANDARD_ID, 5))
		u.u_error = ENOTFS;
	else
		status = 0; 

	block_put (bp);

	return (status);

}	/* end cd_authen */

/*
 ****************************************************************
 *	Operação de MOUNT					*
 ****************************************************************
 */
int
cd_mount (SB *unisp)
{
	BHEAD		*bp = NOBHEAD;
	CDSB		*sp;
	const CDVOL	*cdvp;
	const CDDIR	*dp;
	daddr_t		last_good_session_offset = -1;
	TOC		*tp;
	int		nentries, sessions = 0;
	pg_t		pgno;
	dev_t		dev = unisp->sb_dev;
	const TOC_ENTRY	*tep;
	const char	*cp;
	char		pri_vol_id[32], sec_vol_id[32];
	daddr_t		pri_size = 0, sec_size = 0;
	int		pri_blsz = 0, sec_blsz = 0;
	daddr_t		other_sessions_size = 0;
	daddr_t		pri_root_first_block = 0, sec_root_first_block = 0;
	daddr_t		pri_blkno = 0, sec_blkno = 0, blkno;

	/*
	 *	Em primeiro lugar, lê a TOC
	 */
	if ((pgno = malloce (M_CORE, 1 /* Página */)) == NOPG)
		{ u.u_error = ENOMEM; return (-1); }

	tp = (TOC *)PGTOBY (pgno);

	if ((nentries = (*ciotab[MAJOR (dev)].w_ioctl) (dev, CD_READ_TOC, (int)tp, 0)) <= 0)
		goto bad1;

	/*
	 *	Percorre as várias sessões do CD
	 */
	for (tep = &tp->tab[nentries - 1]; tep >= &tp->tab[0]; tep--)
	{
		if ((tep->control & 4) == 0)	/* Se não for trilha de dados, ... */
			continue;

		sessions++;

		if (nentries <= 1)
			{ last_good_session_offset = tep->addr.lba; break; }

		if (bp != NOBHEAD)
			block_put (bp);

		bp = bread (dev, ISOBL_TO_BL (tep->addr.lba + ISO_SBNO), 0);

		if (u.u_error)
			{ block_put (bp); bp = NOBHEAD; continue; }

		cdvp = bp->b_addr;

		if
		(	!memeq (cdvp->cd_id, ISO_STANDARD_ID, 5) ||
			cdvp->cd_type != ISO_VD_PRIMARY && cdvp->cd_type != ISO_VD_SUPPLEMENTARY
		)
		{
			printf ("%v: Sessão %d com SUPERBLOCO inválido\n", unisp->sb_dev, nentries - sessions);
			continue;
		}

		if (last_good_session_offset == -1)
			last_good_session_offset = tep->addr.lba;
		else
			other_sessions_size += ISO_GET_LONG (cdvp->cd_volume_space_size);
	}

	if (sessions == 0)
	{
		printf ("%v: Aparentemente é um disco de ÁUDIO\n", unisp->sb_dev);
		u.u_error = ENOTFS; goto bad1;
	}

	if (last_good_session_offset == -1)
	{
		printf ("%v: NÃO consegui achar uma trilha de DADOs válida\n", unisp->sb_dev);
		u.u_error = ENOTFS; goto bad1;
	}

	/*
	 *	Aloca a parte específica do CDSB.
	 */
	if ((sp = malloc_byte (sizeof (CDSB))) == NOCDSB)
		{ u.u_error = ENOMEM; return (-1); }

	memclr (sp, sizeof (CDSB));

   /***	sp->s_high_sierra = 0;	***/

	unisp->sb_ptr = sp;

	/*
	 *	Lê os superblocos (blocos 16, 17, ...) da última sessão
	 */
	for (blkno = last_good_session_offset + ISO_SBNO; /* abaixo */; blkno++)
	{
		/*
		 *	Lê um dos superblocos e confere a assinatura
		 */
		if (bp != NOBHEAD)
			block_put (bp);

		bp = bread (dev, ISOBL_TO_BL (blkno), 0);

		if (u.u_error)
			goto bad2;

		cdvp = bp->b_addr;

		if (!memeq (cdvp->cd_id, ISO_STANDARD_ID, 5))
			break;

		/*
		 *	Identifica o Superbloco
		 */
		switch (cdvp->cd_type)
		{
		    case ISO_VD_PRIMARY:
			if (pri_blkno)
				continue;

			pri_blkno = blkno;
			pri_blsz  = ISO_GET_SHORT (cdvp->cd_logical_block_size);
			pri_size  = ISO_GET_LONG  (cdvp->cd_volume_space_size);

			dp = &cdvp->cd_root_dir;
			pri_root_first_block = ISO_GET_LONG (dp->d_first_block) + dp->d_ext_attr_len;

			memmove (pri_vol_id, cdvp->cd_volume_id, sizeof (pri_vol_id));

			break;

		    case ISO_VD_SUPPLEMENTARY:
			if ((unisp->sb_mntent.me_flags & SB_JOLIET) == 0)
				continue;

			if (sec_blkno)
				continue;

			if   (memeq (cdvp->cd_joliet_escape, "%/@", 3))
				sp->s_joliet_level = 1;
			elif (memeq (cdvp->cd_joliet_escape, "%/C", 3))
				sp->s_joliet_level = 2;
			elif (memeq (cdvp->cd_joliet_escape, "%/E", 3))
				sp->s_joliet_level = 3;

			if (cdvp->cd_joliet_flags & 1)
				sp->s_joliet_level = 0;

			if (sp->s_joliet_level == 0)
				continue;

			sec_blkno = blkno;
			sec_blsz  = ISO_GET_SHORT (cdvp->cd_logical_block_size);
			sec_size  = ISO_GET_LONG  (cdvp->cd_volume_space_size);

			dp = &cdvp->cd_root_dir;
			sec_root_first_block = ISO_GET_LONG (dp->d_first_block) + dp->d_ext_attr_len;

			big_unicode2iso (sec_vol_id, cdvp->cd_volume_id, sizeof (sec_vol_id) / 2);

			break;

		    default:
			continue;

		}	/* end switch (tipos de superblocos) */

	}	/* end for (lendo os superblocos) */

	if (bp != NOBHEAD)
		{ block_put (bp); bp = NOBHEAD; }

	/*
	 *	Verifica se é "Rock Ridge Extension"
	 */
	if (unisp->sb_mntent.me_flags & SB_ROCK)
	{
		bp = bread (dev, ISOBL_TO_BL (pri_root_first_block), 0);

		if (u.u_error)
			goto bad2;

		dp = bp->b_addr;

		cp = dp->d_name + 1;	/* Pula o '\0' do "." */

	   /***	sp->s_rock_ridge_skip0 = 0; ***/
	   /***	sp->s_root_first_block = pri_root_first_block; ***/

		if   (memeq (cp, "SP\7\1\276\357", 6))		/* CDROM Regular */
		{
			cd_rr_fields_analysis (dp, unisp, RR_START_MISSION, NOVOID);
		}
		elif (memeq (cp + 15, "SP\7\1\276\357", 6))	/* CDROM XA */
		{
			sp->s_rock_ridge_skip0 = 15;

			cd_rr_fields_analysis (dp, unisp, RR_START_MISSION, NOVOID);
		}

		block_put (bp); bp = NOBHEAD;
	}

	/*
	 *	Verifica se usa o descritor primário ou secundário
	 */
	if (sp->s_rock_ridge != 0 || sp->s_joliet_level == 0)	/* Primário */
	{
		sp->s_blsz = pri_blsz;
		sp->s_size = pri_size;

		sp->s_root_first_block = pri_root_first_block;

		strscpy (unisp->sb_fname, pri_vol_id, sizeof (unisp->sb_fname));
	}
	else							/* Secundário */
	{
		sp->s_blsz = sec_blsz;
		sp->s_size = sec_size;

		sp->s_root_first_block = sec_root_first_block;

		strscpy (unisp->sb_fname, sec_vol_id, sizeof (unisp->sb_fname));
	}

	sp->s_size += other_sessions_size;

	/*
	 *	Atualiza as entradas do SB
	 */
	if (sp->s_blsz != ISO_BLSZ)
		{ printf ("%v: blsz = %d (!= %d)\n", unisp->sb_dev, sp->s_blsz, ISO_BLSZ); goto bad3; }

	if (sp->s_root_first_block == 0)
		{ printf ("%v: NÃO achei o SUPERBLOCO\n", unisp->sb_dev); goto bad3; }

	unisp->sb_code	   = FS_CD;
	unisp->sb_sub_code = 0;
   /***	unisp->sb_ptr	   = sp; ***/
	unisp->sb_root_ino = (ISOBL_TO_BL (sp->s_root_first_block) << BLSHIFT);

	sprintf (unisp->sb_fpack, "%v", unisp->sb_dev);

	unisp->sb_mntent.me_flags |= SB_RONLY;

	mrelease (M_CORE, 1, pgno);

#define	VERBOSE
#ifdef	VERBOSE
	printf ("%v: CDROM ISO 9660", unisp->sb_dev);

	if (sessions > 1)
		printf (", %d sessões", sessions);

	if (sp->s_rock_ridge)
		printf (" (Rock Ridge %d, %d)", sp->s_rock_ridge_skip0, sp->s_rock_ridge_skip);

	if (sp->s_joliet_level)
		printf (" (Joliet %d)", sp->s_joliet_level);

	printf ("\n");
#endif	VERBOSE

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    bad3:
	u.u_error = ENOTFS;
    bad2:
	free_byte (sp);
    bad1:
	mrelease (M_CORE, 1, pgno);

	if (bp != NOBHEAD)
		block_put (bp);

	return (-1);

}	/* end cd_mount */

/*
 ****************************************************************
 *	Obtém as estatísticas do FS				*
 ****************************************************************
 */
void
cd_get_ustat (const SB *unisp, USTAT *up)
{
	const CDSB	*sp = unisp->sb_ptr;

	up->f_type	 = FS_CD;
	up->f_sub_type	 = 0;
	up->f_bsize	 = sp->s_blsz;
	up->f_nmsize	 = 255;

	up->f_fsize	 = sp->s_size;
	up->f_tfree	 = 0;
	up->f_isize	 = 0;
	up->f_tinode	 = 0;
	up->f_m		 = 1;
	up->f_n		 = 1;
	up->f_symlink_sz = 0;

	memclr (up->f_fname, sizeof (up->f_fname));
	memclr (up->f_fpack, sizeof (up->f_fpack));

}	/* end cd_get_ustat */
