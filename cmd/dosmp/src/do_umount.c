/*
 ****************************************************************
 *								*
 *			do_umount.c				*
 *								*
 *	Desmonta o dispositivo com o sistema de arquivos DOS	*
 *								*
 *	Versão	3.0.0, de 19.11.93				*
 *		3.0.0, de 10.06.95				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
void		simple_umount (void);
void		do_umount_help (void);

/*
 ****************************************************************
 *	Desmonta o dispositivo com o sistema de arquivos DOS	*
 ****************************************************************
 */
void
do_umount (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = cmd_getopt (argv, "H")) >= 0)
	{
		switch (opt)
		{
		    case 'H':			/* Help */
			do_umount_help ();
			return;

		    default:			/* Erro */
			fprintf (stderr, "\n");
			do_umount_help ();
			return;
		}
	}

	/*
	 *	Pequena consistência
	 */
	argc -= cmd_optind;
	argv += cmd_optind;

	if (argc > 0)
	{
		do_umount_help ();
		return;
	}

	/*
	 *	Processa o dispositivo
	 */
	simple_umount ();

}	/* end do_umount */

/*
 ****************************************************************
 *	Desmonta e fecha o dispositivo				*
 ****************************************************************
 */
void
simple_umount (void)
{
   	switch (fs_status)
	{
      /***  case FS_CLOSED:	***/
      /***  case FS_OPEN:	***/
	    default:
		printf
		(	"Não há sistema de arquivos DOS montado\n"
		);
		return;

	    case FS_MOUNT_RW:
	   /***	put_fat (); ***/
		update_fat32_info ();
	    case FS_MOUNT_RO:
		close (dev_fd);

	   	fs_status = FS_CLOSED;
	   /***	dev_nm[0] = '\0';	Conserva o nome do dispositivo ***/
		vol_nm[0] = '\0';
	   /***	cwd_cluster = 0;	***/

	}	/* end switch */

}	/* end simple_umount */

/*
 ****************************************************************
 *	Atualiza o bloco de informações da FAT32		*
 ****************************************************************
 */
void
update_fat32_info (void)
{
	UNI		*up = &uni;
	FSINFO		fsinfo;

	/*
	 *	Só é chamada se for RW
	 */
	if (up->u_fs_info == 0)
		return;
#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"u_disk_infree = %d, u_infree = %d\n",
		up->u_disk_infree, up->u_infree
	);
#endif	DEBUG

	if (up->u_disk_infree == up->u_infree)
		return;

	/*
	 *	Atualiza o bloco de informações
	 */
#ifdef	DEBUG
	printf ("Atualizando ...\n");
#endif	DEBUG

	bread (up->u_fs_info,	  &fsinfo);
   /***	bread (up->u_fs_info + 1, (void *)&fsinfo + BLSZ); ***/

	PUT_LONG (up->u_infree, &fsinfo.fs_infree);

	bwrite (up->u_fs_info,	  &fsinfo);
   /***	bwrite (up->u_fs_info + 1, (void *)&fsinfo + BLSZ); ***/

	up->u_disk_infree = up->u_infree;

}	/* update_fat32_info */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
do_umount_help (void)
{
	fprintf
	(	stderr,
		"%s - desmonta o sistema de arquivos DOS corrente\n"
		"\nSintaxe:\n"
		"\t%s\n",
		cmd_nm, cmd_nm
	);

}	/* end do_umount_help */
