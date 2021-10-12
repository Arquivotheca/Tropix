/*
 ****************************************************************
 *								*
 *			mount.c					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 15.10.03				*
 *		4.5.0, de 22.11.03				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/mntent.h>
#include <sys/sb.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <errno.h>
#include <fcntl.h>

#include <X11/Intrinsic.h>

#include "../h/xfm.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
void		update_mounted_or_umounted_dir (NODE *dir_np);

/*
 ****************************************************************
 *	Verifica se é permitida a montagem/desmontagem		*
 ****************************************************************
 */
int
permission_mount (const NODE *np)
{
	if (np->n_mnt_point)
		return (1);
	else
		return (-1);

}	/* end permission_mount */

/*
 ****************************************************************
 *	Monta o dispositivo					*
 ****************************************************************
 */
void
mount_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	const FSTAB	*tp;
	NODE		*dir_np;
	MNTENT		rw_mntent, ro_mntent;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	if ((dir_np = mp->menu_node) == NONODE)
		return;

	/*
	 *	Procura no arquivo FSTAB
	 */
	if ((tp = search_dir_in_fstab (dir_np->n_path)) == NOFSTAB)
		{ draw_error_msg_text ("Não encontrei \"%s\" em \"%s\"", dir_np->n_path, fstab_nm); return; }

	/*
	 *	Tenta montar
	 */
	seteuid (0); setruid (real_uid);

	defmntent (&rw_mntent); defmntent (&ro_mntent);

	rw_mntent.me_flags = tp->fs_flags;
	ro_mntent.me_flags = tp->fs_flags|SB_RONLY;

	if   (mount (tp->fs_dev_nm, dir_np->n_path, &rw_mntent) >= 0)
	{
		draw_msg_text ("Dispositivo \"%s\" montado em \"%s\"", tp->fs_dev_nm, dir_np->n_path);

		update_mounted_or_umounted_dir (dir_np);
	}
	elif (errno == EROFS && mount (tp->fs_dev_nm, dir_np->n_path, &ro_mntent) >= 0)
	{
		draw_msg_text
		(	"Dispositivo \"%s\" protegido para escritas, "
			"montado em \"%s\" somente para leituras",
			tp->fs_dev_nm, dir_np->n_path
		);

		update_mounted_or_umounted_dir (dir_np);
	}
	else
	{
		draw_error_msg_text ("*Não consegui montar \"%s\" em \"%s\"", tp->fs_dev_nm, dir_np->n_path);
	}

	setruid (0); seteuid (real_uid);

} 	/* end mount_0 */

/*
 ****************************************************************
 *	Desmonta o dispositivo					*
 ****************************************************************
 */
void
umount_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*dir_np;
	const FSTAB	*tp;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	if ((dir_np = mp->menu_node) == NONODE)
		return;

	/*
	 *	Procura no arquivo FSTAB
	 */
	if ((tp = search_dir_in_fstab (dir_np->n_path)) == NOFSTAB)
		{ draw_error_msg_text ("Não encontrei \"%s\" em \"%s\"", dir_np->n_path, fstab_nm); return; }

	/*
	 *	Tenta desmontar
	 */
	seteuid (0); setruid (real_uid);

	if (umount (tp->fs_dev_nm, tp->fs_flags) >= 0)
	{
		draw_msg_text ("Dispositivo \"%s\" desmontado", tp->fs_dev_nm);

		update_mounted_or_umounted_dir (dir_np);
	}
	else
	{
		draw_error_msg_text ("*Não consegui desmontar \"%s\"", tp->fs_dev_nm);
	}

	setruid (0); seteuid (real_uid);

}	/* end umount_0 */

/*
 ****************************************************************
 *	Ejeta o dispositivo					*
 ****************************************************************
 */
void
eject_0 (void)
{
	const MENUDATA	*mp = &menudata[present_menu];
	NODE		*dir_np;
	int		fd;
	const FSTAB	*tp;

	/*
	 *	Prólogo
	 */
	XUnmapWindow (display, tree_menu.win);

	if ((dir_np = mp->menu_node) == NONODE)
		return;

	/*
	 *	Procura no arquivo FSTAB
	 */
	if ((tp = search_dir_in_fstab (dir_np->n_path)) == NOFSTAB)
		{ draw_error_msg_text ("Não encontrei \"%s\" em \"%s\"", dir_np->n_path, fstab_nm); return; }

	/*
	 *	Abre o dispositivo
	 */
	if ((fd = open (tp->fs_dev_nm, O_RDONLY)) < 0)
		{ draw_error_msg_text ("*Não consegui abrir \"%s\"", tp->fs_dev_nm); return; }

	/*
	 *	Libera o meio
	 */
	if (ioctl (fd, ZIP_LOCK_UNLOCK, 0) < 0)
		{ draw_error_msg_text ("*Não consegui retirar o meio de \"%s\" (1)", tp->fs_dev_nm); goto bad; }

	/*
	 *	Liga o motor
	 */
	if (ioctl (fd, ZIP_START_STOP, 1) < 0)
		{ draw_error_msg_text ("*Não consegui retirar o meio de \"%s\" (2)", tp->fs_dev_nm); goto bad; }

	/*
	 *	Desliga o motor
	 */
	if (ioctl (fd, ZIP_START_STOP, 2) < 0)
		{ draw_error_msg_text ("*Não consegui retirar o meio de \"%s\" (3)", tp->fs_dev_nm); goto bad; }

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	close (fd);

}	/* end eject_0 */

/*
 ****************************************************************
 *	O diretório mudou em função de montagem/desmontagem	*
 ****************************************************************
 */
void
update_mounted_or_umounted_dir (NODE *dir_np)
{
	NODE		*np, *next_np;

	/*
	 *	Atualiza a lista de SBs
	 */
	create_SB_list ();

	/*
	 *	Remove a subárvore anterior
	 */
	if (dir_np->n_children != NONODE)
	{
		for (np = dir_np->n_children; np != NONODE; np = next_np)
			{ next_np = np->n_forw; free_node (np); }

		dir_np->n_children = NONODE;
	}

	/*
	 *	Obtém o diretório novo de montagem
	 */
	if (lstat (dir_np->n_path, &dir_np->n_ls) < 0)
		draw_error_msg_text ("*Não consegui obter o estado de \"%s\"", dir_np->n_path);

	memmove (&dir_np->n_s, &dir_np->n_ls, sizeof (STAT));

	dir_np->n_sb_flags = get_mount_flags (dir_np->n_ls.st_dev);

	dir_np->n_child_cnt = 0;
	dir_np->n_line_size = 1;
	dir_np->n_expanded  = 0;

	/*
	 *	Analisa a janela do diretório
	 */
	if (dir_win_node == dir_np)
	{
	   /***	if (dir_np->n_children == NONODE) ***/
			create_child_dir_list (dir_np);

		dir_win_first_son = dir_np->n_children;

		XClearWindow (display, dir.win);
		draw_dir_win ();
	}
	else for (np = dir_win_node; np != NONODE; np = np->n_parent)
	{
		if (np != dir_np)
			continue;

		dir_win_node	  = NONODE;
		dir_win_first_son = NONODE;

		XClearWindow (display, dir.win);
		draw_dir_win ();

		break;
	}

	/*
	 *	Redesenha a janela da árvore
	 */
	XClearWindow (display, tree.win);
	draw_tree_win ();

}	/* end update_mounted_or_umounted_dir */
