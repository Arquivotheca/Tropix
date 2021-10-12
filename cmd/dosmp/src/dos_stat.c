/*
 ****************************************************************
 *								*
 *			dos_stat.c				*
 *								*
 *	Procura uma entrada de diretório dado o nome		*
 *								*
 *	Versão	3.0.0, de 20.11.93				*
 *		3.0.0, de 02.03.96				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****** (Pseudo-) diretório da raiz *****************************
 */
entry DOSDIR	off_root_dir =
{
	'.',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',		/* d_name */
	' ',	' ',	' ', 			/* d_ext */
	Z_DIR,		 			/* d_mode */
	0,	0,	0,	0,		/* d_reser0 */
	0,	0,	0,	0,
	0,	0, 
	0,	0, 				/* d_mtime */
	0,	0, 				/* d_mdate */
	0,	0, 				/* d_cluster */
	0,	0,	0,	0		/* d_size */
};

/*
 ****************************************************************
 *	Procura uma entrada de diretório dado o caminho		*
 ****************************************************************
 */
int
dos_stat (const char *path, DOSSTAT *zp)
{
	const char	*pp = path;
	char		*ep;
	int		clusno;
	char		c = '\0';

	/*
	 *	Uma pequena consistência
	 */
	if (zp == NODOSSTAT)
		{ error ("dos_stat: \"zp\" NULO"); return (-1); }

	/*
	 *	Verifica a origem e casos particulares
	 */
	if   (pp == NOSTR)
		{ error ("path é um ponteiro NULO\n"); return (-1); }
	elif (*pp == '\0')
		{ error ("path de comprimento NULO\n"); return (-1); }
	elif (*pp == '/')
		{ clusno = uni.u_root_cluster; pp++; }
	else
		clusno = cwd_cluster;

	if (*pp == '\0')
		pp = ".";

	/*
	 *	Processa um componente do caminho
	 */
	for (EVER)
	{
		if ((ep = strchr (pp, '/')) != NOSTR)
			{ c = *ep; *ep = '\0'; }

		if (dos_fstat (pp, clusno, zp) < 0)
			goto bad;

		clusno = zp->z_cluster;

		if (ep == NOSTR)
			return (0);

		*ep = c; pp = ep + 1;
	}

	/*
	 *	Não achou
	 */
    bad:
	if (ep != NOSTR)
		*ep = c;

   /*** errno = (posto por "fstat") ***/
	return (-1);

}	/* end dos_stat */

/*
 ****************************************************************
 *	Procura uma entrada de diretório dado o nome		*
 ****************************************************************
 */
int
dos_fstat (const char *nm, int par_clusno, DOSSTAT *zp)
{
	const UNI	*up = &uni;
	const DOSDIR	*dp;
	const DOSDIR	*end_dp;
	int		clusno;
	int		blkno, end_blkno;
	char		area[BLSZ];

	/*
	 *	Caso particular: o pedido é o diretório raiz
	 */
	if (par_clusno == uni.u_root_cluster && (streq (nm, ".") || streq (nm, "..")))
	{
		if (nm[1] == '.')		/* ".." */
			zp->z_name[1] = '.';
		else				/* "." */
			zp->z_name[1] = ' ';

		memmove (&zp->z_d, &off_root_dir, sizeof (DOSDIR));

		PUT_CLUSTER (par_clusno, zp);

		zp->z_cluster = par_clusno;
		zp->z_blkno  = -1;
		zp->z_offset = -1;

		zp->z_lfn_clusno    = -1;
		zp->z_lfn_blkno     = -1;
		zp->z_lfn_end_blkno = -1;
		zp->z_lfn_offset    = -1;
		zp->z_lfn_entries   = 1;

		strcpy (zp->z_lfn_area, nm); zp->z_lfn_nm = zp->z_lfn_area;

		return (0);
	}

	/*
	 *	Inicialização
	 */
	if ((clusno = par_clusno) == 0)
		ROOT_FIRST_AND_LAST_BLK (blkno, end_blkno);
	else
		FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);

	dp = bread (blkno, area);
	end_dp = dp + BLSZ / sizeof (DOSDIR);

	dos_lfn_init (zp); zp->z_lfn_entries = 0;

	/*
	 *	Malha pelo diretório
	 */
	for (/* acima */; /* abaixo */; dp++)
	{
		if (dp >= end_dp)
		{
			if (++blkno >= end_blkno)
			{
				if (clusno == 0)
					{ errno = ENOENT; return (-1); }

				if (C_ISBAD_OR_EOF (clusno = get_fat_value (clusno)))
					{ errno = ENOENT; return (-1); }

				FIRST_AND_LAST_BLK (clusno, blkno, end_blkno);
			}

			dp = bread (blkno, area);
		   /***	end_dp = dp + BLSZ / sizeof (DOSDIR); ***/
		}

		if (dp->d_name[0] == '\0')
			{ errno = ENOENT; return (-1); }

		if (dp->d_name[0] == Z_EMPTY)
			{ dos_lfn_reset (zp); zp->z_lfn_entries = 0; continue; }

		if (zp->z_lfn_entries++ == 0)
		{
			zp->z_lfn_clusno    = clusno;
			zp->z_lfn_blkno     = blkno;
			zp->z_lfn_end_blkno = end_blkno;
			zp->z_lfn_offset    = (int)dp - (int)area;
		}

		memmove (&zp->z_d, dp, sizeof (DOSDIR));

		if (Z_ISLFN (dp->d_mode))
			{ dos_lfn_add (zp); continue; }

		dos_lfn_get_nm (zp);

		if (streq (nm, zp->z_lfn_nm))
		{
			GET_CLUSTER (zp);
			zp->z_blkno   = blkno;
			zp->z_offset  = (int)dp - (int)area;

			return (0);
		}

		dos_lfn_reset (zp); zp->z_lfn_entries = 0;

	}	/* end for */

}	/* end dos_fstat */

/*
 ****************************************************************
 *	Função auxiliar para FAT-32				*
 ****************************************************************
 */
void
get_cluster (DOSSTAT *zp)
{
	const UNI	*up = &uni;

	zp->z_cluster = GET_SHORT (zp->z_low_cluster);

	if (up->u_fat_bits == 32)
	{
		zp->z_cluster |= (GET_SHORT (zp->z_high_cluster) << 16);

		if (zp->z_cluster == 0 && (streq (zp->z_lfn_nm, ".") || streq (zp->z_lfn_nm, "..")))
			zp->z_cluster = up->u_root_cluster;
	}

}	/* end get_cluster */
