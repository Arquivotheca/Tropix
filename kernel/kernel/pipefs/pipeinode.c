/*
 ****************************************************************
 *								*
 *			pipeinode.c				*
 *								*
 *	Alocação e desalocação de INODEs no disco		*
 *								*
 *	Versão	4.3.0, de 04.09.02				*
 *		4.3.0, de 04.09.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/signal.h"
#include "../h/ustat.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Lê e converte um INODE T1FS do disco para a memória	*
 ****************************************************************
 */
int
pipe_read_inode (INODE *ip)
{
	/*
	 *	Copia diversos campos em comum
	 */
	ip->i_union	= IN_PIPE;

	ip->i_mode	= IFREG;
	ip->i_uid	= u.u_euid;
	ip->i_gid	= u.u_egid;
	ip->i_nlink	= 1;
	ip->i_size	= 0;
   /***	ip->i_high_size	= dp->n_high_size; ***/

	ip->i_atime	=
	ip->i_mtime	=
	ip->i_ctime	= time;

	ip->i_fsp	= &fstab[FS_PIPE];
	ip->i_rdev	= 0;

	EVENTSET   (&ip->i_notfull);
	EVENTCLEAR (&ip->i_notempty);

	return (0);

}	/* end pipe_read_inode */

/*
 ****************************************************************
 *	Converte o formato do INODE: memória para disco		*
 ****************************************************************
 */
void
pipe_write_inode (INODE *ip)
{

}	/* end pipe_write_inode */

/*
 ****************************************************************
 *	Aloca um INODE no disco (e na memória)			*
 ****************************************************************
 */
INODE *
pipe_alloc_inode (void)
{
	ino_t		ino;
	INODE		*ip;
	static ino_t	pipe_ino_source;

	/*
	 *	Gera um novo número de INODE
	 */
	ino = ++pipe_ino_source;

	if ((ip = iget (scb.y_pipedev, ino, 0)) == NOINODE)
		{ printf ("%g: Não obtive o INODE (%v, %d)\n", scb.y_pipedev, ino); return (NOINODE); }

	return (ip);

}	/* end pipe_alloc_inode */

/*
 ****************************************************************
 *	Libera um INODE do Disco				*
 ****************************************************************
 */
void
pipe_free_inode (const INODE *ip)
{

}	/* end pipe_free_inode */

/*
 ****************************************************************
 *	Libera os blocos de um INODE do Disco			*
 ****************************************************************
 */
void
pipe_trunc_inode (INODE *ip)
{
}	/* end pipe_trunc_inode */

/*
 ****************************************************************
 *	Obtém a estatística do sistema de arquivos PIPE		*
 ****************************************************************
 */
void
pipe_get_ustat (const SB *unisp, USTAT *up)
{
	memclr (up, sizeof (USTAT));

	up->f_type	= FS_PIPE;

}	/* end pipe_get_ustat */
