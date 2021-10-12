/*
 ****************************************************************
 *								*
 *			t1mount.c				*
 *								*
 *	Diversas funções da montagem de sistemas de arquivos	*
 *								*
 *	Versão	4.3.0, de 29.07.02				*
 *		4.5.0, de 21.11.03				*
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

#include "../h/disktb.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/t1.h"
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
 *	Certifica-se de que o sistema de arquivos é T1		*
 ****************************************************************
 */
int
t1_authen (dev_t dev)
{
	BHEAD		*bp;
	int		status = -1;
	DISKTB		*up;

	/*
	 *	Confere a assinatura
	 */
	u.u_error = NOERR;

	bp = bread (dev, T1_BL4toBL (T1_SBNO), 0);

	if   (u.u_error)
		/* vazio */;
	elif ((((T1SB *)bp->b_addr)->s_magic) != T1_SBMAGIC)
		u.u_error = ENOTFS;
	elif (status = 0, (up = disktb_get_entry (dev)) != NODISKTB && up->p_type == 0)
		up->p_type = PAR_TROPIX_T1;

	block_put (bp);

	return (status);

}	/* end t1_authen */

/*
 ****************************************************************
 *	Operação de MOUNT					*
 ****************************************************************
 */
int
t1_mount (SB *sp)
{
	BHEAD		*bp;
	T1SB		*t1sp;

	/*
	 *	Aloca a parte específica do T1SB
	 */
	if ((t1sp = malloc_byte (sizeof (T1SB))) == NOT1SB)
		{ u.u_error = ENOMEM; return (-1); }

	bp = bread (sp->sb_dev, T1_BL4toBL (T1_SBNO), 0);

	if (u.u_error)
		{ block_put (bp); goto bad; }

	memmove (t1sp, bp->b_addr, sizeof (T1SB));

	block_put (bp);

#define	MOUNTCHK
#ifdef	MOUNTCHK
	/*
	 *	Verifica se é realmente um SB de um sistema  de arquivos
	 */
	if (t1sp->s_magic != T1_SBMAGIC)
		{ u.u_error = ENOTFS; goto bad; }
#endif	MOUNTCHK

	/*
	 *	Verifica a validade das listas livres do SB
	 */
	if (t1sp->s_bl_sz != BL4SZ || t1sp->s_busy_bl > t1sp->s_fs_sz)
	{
		printf ("%g: Superbloco inválido no dispositivo \"%v\"\n", sp->sb_dev);

		u.u_error = ENOTFS; goto bad;
	}

	/*
	 *	Atualiza as entradas do SB
	 */
	strcpy (sp->sb_fname, t1sp->s_fname);
	strcpy (sp->sb_fpack, t1sp->s_fpack);

	sp->sb_code = FS_T1;
	sp->sb_ptr  = t1sp;

	sp->sb_root_ino	= T1_ROOT_INO;

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    bad:
	free_byte (t1sp);

	return (-1);

}	/* end t1_mount */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos T1		*
 ****************************************************************
 */
void
t1_get_ustat (const SB *unisp, USTAT *up)
{
	T1SB	*sp = unisp->sb_ptr;

	up->f_type	 = FS_T1;
	up->f_sub_type	 = 0;
	up->f_bsize	 = sp->s_bl_sz;
	up->f_nmsize	 = MAXNAMLEN;
	up->f_fsize	 = sp->s_fs_sz;
	up->f_tfree	 = sp->s_fs_sz - sp->s_busy_bl;
	up->f_isize	 = sp->s_fs_sz * T1_INOinBL4;
	up->f_tinode	 = up->f_isize - sp->s_busy_ino;
	strcpy (up->f_fname, sp->s_fname);
	strcpy (up->f_fpack, sp->s_fpack);
	up->f_m		 = 1;
	up->f_n		 = 1;
	up->f_symlink_sz = T1_NADDR * sizeof (daddr_t);

}	/* end t1_get_ustat */

/*
 ****************************************************************
 *	Atualiza um Super Bloco T1				*
 ****************************************************************
 */
void
t1_update (const SB *sp)
{
	BHEAD		*bp;

	/*
	 *	Prepara para escrever o SB no Disco
	 */
	bp = block_get (sp->sb_dev, T1_BL4toBL (T1_SBNO), 0);

	memmove (bp->b_addr, sp->sb_ptr, sizeof (T1SB));

	/*
	 *	Escreve o SB no Disco
	 */
	bwrite (bp);

#ifdef	MSG
	if (CSWT (6))
		printf ("%g: SB, dev = %v\n", sp->sb_dev);
#endif	MSG

}	/* end t1_update */
