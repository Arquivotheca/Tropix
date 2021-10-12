/*
 ****************************************************************
 *								*
 *			cpio.h					*
 *								*
 *	Header do Formato CPIO					*
 *								*
 *	Versão	1.0.00, de 08.09.86				*
 *		3.1.06, de 04.05.97				*
 *								*
 *	Módulo: gar						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Estrutura do Header					*
 ****************************************************************
 */
typedef	struct
{
	ushort	c_magic;	/* No. Magico do CPIO */
	dev_t	c_dev;		/* Dispositivo */
	ino_t	c_ino;		/* No. do Inode */
	ushort	c_mode; 	/* Tipo do arquivo */
	ushort	c_uid;		/* Dono */
	ushort	c_gid;		/* Grupo */
	ushort	c_nlink;	/* No. de links */
	dev_t	c_rdev;		/* Dispositivo (Especial) */
	time_t	c_mtime;	/* tempo de modificação */
	short	c_namesz;	/* Tamanho do Nome do Arquivo */
	short	c_size[2]; 	/* Tamanho */

}	CPHEAD;

#define	CPHEADSZ	(13 * sizeof (short))

/*
 ****************************************************************
 *	Valores Mágicos						*
 ****************************************************************
 */
#define	CPIOMAGIC	0x71C7
#define	CPIORMAGIC	0xC771

#define	CPIOTRAILER	"TRAILER!!!"
