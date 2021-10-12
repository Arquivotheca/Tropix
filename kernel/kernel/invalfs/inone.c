/*
 ****************************************************************
 *								*
 *			inone.c					*
 *								*
 *	Leitura e escrita em dispositivos inexistentes		*
 *								*
 *	Versão	4.2.0, de 26.11.01				*
 *		4.6.0, de 05.10.04				*
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

#include "../h/iotab.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Autenticação de um sistema de arquivos			*
 ****************************************************************
 */
int
fs_inval_authen (dev_t dev)
{
	return (-1);

}	/* end fs_inval_authen */


/*
 ****************************************************************
 *	Montagem de um sistema de arquivos inválido		*
 ****************************************************************
 */
int
fs_inval_mount (SB *unisp)
{
	printf ("%g: "); print_calls ();

	return (-1);

}	/* end fs_inval_mount */

/*
 ****************************************************************
 *	Estatísticas de um sistema de arquivos inválido		*
 ****************************************************************
 */
void
fs_inval_ustat (const SB *unisp, USTAT *up)
{
	printf ("%g: "); print_calls ();

}	/* end fs_inval_mount */

/*
 ****************************************************************
 * Atualização do Superbloco de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_update (const SB *unisp)
{
	printf ("%g: "); print_calls ();

}	/* end fs_inval_update */

/*
 ****************************************************************
 *	Leitura do INODE de um sistema de arquivos inválido	*
 ****************************************************************
 */
int
fs_inval_read_inode (INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

	return (-1);

}	/* end fs_inval_read_inode */

/*
 ****************************************************************
 *	Escrita do INODE de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_write_inode (INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_write_inode */

/*
 ****************************************************************
 *	Criação de INODE de um sistema de arquivos inválido	*
 ****************************************************************
 */
INODE *
fs_inval_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, mode);

	print_calls ();

	return (NOINODE);

}	/* end fs_inval_make_inode */

/*
 ****************************************************************
 *	Liberação dos atributos de um INODE			*
 ****************************************************************
 */
void
fs_inval_rel_inode_attr (INODE *ip)
{

}	/* end fs_inval_rel_inode_attr */

/*
 ****************************************************************
 *	Devolução dos blocos de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_trunc_inode (INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_trunc_inode */

/*
 ****************************************************************
 *	Libera um INODE de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_free_inode (const INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_free_inode */

/*
 ****************************************************************
 * Procura uma entrada de diretório de um sistema de inválido	*
 ****************************************************************
 */
int
fs_inval_search_dir (IOREQ *iop, DIRENT *dep, int mission)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

	return (-1);

}	/* end fs_inval_search_dir */

/*
 ****************************************************************
 *	Leitura de diretório de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_read_dir (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_read_dir */

/*
 ****************************************************************
 *  Escrita em um diretório de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip, int rename)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_write_dir */

/*
 ****************************************************************
 * Está vazio um diretório de um sistema de arquivos inválido?	*
 ****************************************************************
 */
int
fs_inval_empty_dir (INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

	return (-1);

}	/* end fs_inval_empty_dir */

/*
 ****************************************************************
 *  Criação de um diretório de um sistema de arquivos inválido	*
 ****************************************************************
 */
INODE *
fs_inval_make_dir (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, mode);

	print_calls ();

	return (NOINODE);

}	/* end fs_inval_make_dir */

/*
 ****************************************************************
 *	Zera um diretório de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *son_ip)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_clr_dir */

/*
 ****************************************************************
 *	Abre um arquivo de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_open (INODE *ip, int oflag)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_open */

/*
 ****************************************************************
 *	Fecha um arquivo de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_close (INODE *ip)
{
	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_close */

/*
 ****************************************************************
 *	Le de um arquivo de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_read (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_read */

/*
 ****************************************************************
 *   Escreve em um arquivo de um sistema de arquivos inválido	*
 ****************************************************************
 */
void
fs_inval_write (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

}	/* end fs_inval_write */

/*
 ****************************************************************
 *	Le um "link" simbólico inválido				*
 ****************************************************************
 */
int
fs_inval_read_symlink (IOREQ *iop)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	print_calls ();

	return (-1);

}	/* end fs_inval_read_symlink */

/*
 ****************************************************************
 *	Le um "link" simbólico inválido				*
 ****************************************************************
 */
INODE *
fs_inval_write_symlink (const char *old_path, int old_count, IOREQ *iop, DIRENT *dep)
{
	INODE		*ip = iop->io_ip;

	printf ("%g: (%v, %d), mode = %m\n", ip->i_dev, ip->i_ino, ip->i_mode);

	iput (ip);

	print_calls ();

	return (NOINODE);

}	/* end fs_inval_write_symlink */

/*
 ****************************************************************
 *   Controle em um arquivo de um sistema de arquivos inválido	*
 ****************************************************************
 */
int
fs_inval_ioctl (dev_t dev, int cmd, int arg, int flags)
{
	printf ("%g: dev = %v\n", dev);

	print_calls ();

	return (-1);

}	/* end fs_inval_ioctl */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Edita um tipo de arquivo				*
 ****************************************************************
 */
const char *
edit_file_mode (int mode)
{
	switch (mode &IFMT)
	{
	    case IFREG:
		return ("REG");

	    case IFDIR:
		return ("DIR");

	    case IFIFO:
		return ("FIFO");

	    case IFBLK:
		return ("BLK");

	    case IFCHR:
		return ("CHR");

	    default:
		return ("???");
	}

}	/* end edit_file_mode */
#endif	/*******************************************************/
