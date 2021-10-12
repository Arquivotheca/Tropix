/*
 ****************************************************************
 *								*
 *			v7mount.c				*
 *								*
 *	Diversas funções da montagem de sistemas de arquivos	*
 *								*
 *	Versão	3.0.0, de 24.11.94				*
 *		4.2.0, de 11.12.01				*
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

#include "../h/disktb.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/v7.h"
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
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry LOCK	sblistlock;	/* Lock para inserir/remover entradas */

/*
 ****************************************************************
 *	Certifica-se de que o sistema de arquivos é V7		*
 ****************************************************************
 */
int
v7_authen (dev_t dev)
{
	BHEAD		*bp;
	int		status = -1;
	DISKTB		*up;

	/*
	 *	Confere a assinatura mágica
	 */
	u.u_error = NOERR;

	bp = bread (dev, V7_SBNO, 0);

	if   (u.u_error)
		/* vazio */;
	elif (ENDIAN_LONG (((V7SB *)bp->b_addr)->s_magic) != V7_SBMAGIC)
		u.u_error = ENOTFS;
	elif (status = 0, (up = disktb_get_entry (dev)) != NODISKTB && up->p_type == 0)
		up->p_type = PAR_TROPIX_V7;

	block_put (bp);

	return (status);

}	/* end v7_authen */

/*
 ****************************************************************
 *	Operação de MOUNT					*
 ****************************************************************
 */
int
v7_mount (SB *sp)
{
	BHEAD		*bp;
	V7SB		*v7sp;

	/*
	 *	Aloca a parte específica do V7SB
	 */
	if ((v7sp = malloc_byte (sizeof (V7SB))) == NOV7SB)
		{ u.u_error = ENOMEM; return (-1); }

	bp = bread (sp->sb_dev, V7_SBNO, 0);

	if (u.u_error)
		{ block_put (bp); goto out_6; }

	memmove (v7sp, bp->b_addr, sizeof (V7SB));

	block_put (bp);

#ifdef	LITTLE_ENDIAN
	/*
	 *	Se for o caso, converte, ...
	 */
	v7_sb_endian_conversion (v7sp);
#endif	LITTLE_ENDIAN

#define	MOUNTCHK
#ifdef	MOUNTCHK
	/*
	 *	Verifica se é realmente um SB de um sistema  de arquivos
	 */
	if (v7sp->s_magic != V7_SBMAGIC)
		{ u.u_error = ENOTFS; goto out_6; }
#endif	MOUNTCHK

	/*
	 *	Verifica a validade das listas livres do SB
	 */
	if ((unsigned)v7sp->s_nfblk > V7_SBFBLK || (unsigned)v7sp->s_nfdino > V7_SBFDINO)
	{
		printf ("%g: Dispositivo \"%v\" com lista livre inválida\n", sp->sb_dev);

		u.u_error = ENOTFS; goto out_6;
	}

	/*
	 *	Atualiza as entradas do SB
	 */
	strcpy (sp->sb_fname, v7sp->s_fname);
	strcpy (sp->sb_fpack, v7sp->s_fpack);

	sp->sb_code	= FS_V7;
	sp->sb_ptr	= v7sp;

	sp->sb_root_ino	= V7_ROOT_INO;

	/*
	 *	Inicializa as variáveis da V7
	 */
	SLEEPCLEAR (&v7sp->s_flock);
	SLEEPCLEAR (&v7sp->s_ilock);

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    out_6:
	free_byte (v7sp);

	return (-1);

}	/* end v7_mount */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos V7		*
 ****************************************************************
 */
void
v7_get_ustat (const SB *unisp, USTAT *up)
{
	V7SB	*sp = unisp->sb_ptr;

	up->f_type	 = FS_V7;
	up->f_sub_type	 = 0;
	up->f_bsize	 = BLSZ;
	up->f_nmsize	 = IDSZ;
	up->f_fsize	 = *(daddr_t *)sp->s_fsize - sp->s_isize;
	up->f_tfree	 = *(daddr_t *)sp->s_tfblk;
	up->f_isize	 = (sp->s_isize - (V7_SBNO + 1)) * V7_INOPBL;
	up->f_tinode	 = sp->s_tfdino;
	strcpy (up->f_fname, sp->s_fname);
	strcpy (up->f_fpack, sp->s_fpack);
	up->f_m		 = sp->s_m;
	up->f_n		 = sp->s_n;
	up->f_symlink_sz = 0;

}	/* end v7_get_ustat */

/*
 ****************************************************************
 *	Atualiza um Super Bloco V7				*
 ****************************************************************
 */
void
v7_update (const SB *sp)
{
	V7SB	*v7sp = sp->sb_ptr;
	BHEAD		*bp;

	/*
	 *	Tenta trancar as listas Livres
	 */
	if (SLEEPTEST (&v7sp->s_ilock) < 0)
		return;

	if (SLEEPTEST (&v7sp->s_flock) < 0)
		{ SLEEPFREE (&v7sp->s_ilock); return; }

	/*
	 *	Prepara para escrever o SB no Disco
	 */
	bp = bread (sp->sb_dev, V7_SBNO, 0);

	*(time_t *)v7sp->s_time = time;

	memmove (bp->b_addr, v7sp, sizeof (V7SB));

#ifdef	LITTLE_ENDIAN
	/*
	 *	Se for o caso, converte, ...
	 */
	v7_sb_endian_conversion ((V7SB *)bp->b_addr);
#endif	LITTLE_ENDIAN

	/*
	 *	Escreve o SB no Disco
	 */
	bwrite (bp);

#ifdef	MSG
	if (CSWT (6))
		printf ("%g: SB, dev = %v\n", sp->sb_dev);
#endif	MSG

	SLEEPFREE (&v7sp->s_flock);
	SLEEPFREE (&v7sp->s_ilock);

}	/* end v7_update */

#ifdef	LITTLE_ENDIAN
/*
 ****************************************************************
 *	Converte um SB de de BIG para LITTLE endian		*
 ****************************************************************
 */
void
v7_sb_endian_conversion (V7SB *sp)
{
	int		i;

	sp->s_isize = short_endian_cv (sp->s_isize);
	*(long *)sp->s_fsize = long_endian_cv (*(long *)sp->s_fsize);
	sp->s_nfblk = short_endian_cv (sp->s_nfblk);

	for (i = 0; i < V7_SBFBLK; i++)
		sp->s_fblk[i] = long_endian_cv (sp->s_fblk[i]);

	sp->s_nfdino = short_endian_cv (sp->s_nfdino);

	for (i = 0; i < V7_SBFDINO; i++)
		sp->s_fdino[i] = short_endian_cv (sp->s_fdino[i]);

	*(long *)sp->s_time = long_endian_cv (*(long *)sp->s_time);
	*(long *)sp->s_tfblk = long_endian_cv (*(long *)sp->s_tfblk);
	sp->s_tfdino = short_endian_cv (sp->s_tfdino);
	sp->s_m = short_endian_cv (sp->s_m);
	sp->s_n = short_endian_cv (sp->s_n);
	sp->s_magic = long_endian_cv (sp->s_magic);

}	/* end v7_sb_endian_conversion */
#endif	LITTLE_ENDIAN
