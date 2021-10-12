/*
 ****************************************************************
 *								*
 *			systree.c				*
 *								*
 *	Chamadas ao sistema: manipulação de árvores		*
 *								*
 *	Versão	3.0.0, de 05.01.95				*
 *		4.6.0, de 26.09.04				*
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

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/itnet.h"
#include "../h/nfs.h"
#include "../h/stat.h"
#include "../h/ustat.h"
#include "../h/disktb.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include 		"../h/nt.h"		/* PROVISÓRIO */
#include "../h/bhead.h"
#include "../h/iotab.h"
#include "../h/sysdirent.h"
#include "../h/devmajor.h"
#include "../h/ioctl.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais 					*
 ****************************************************************
 */
#undef	USE_NMOUNT

#ifdef	USE_NMOUNT
entry int	nmount;		/* No. de Dispositivos Montados */
#endif	USE_NMOUNT

/*
 ******	Protótipos de funções ***********************************
 */
int		nfs_do_mount (const char *dev_nm, const char *dir_nm, const struct mntent *mntent_ptr);
int		nfs_do_umount (const char *dev_nm, int flags);
int		klink (const char *name, const char *linkname, int rename);
int		kunlink (const char *path, int rename);

void		set_pgname (const char *pgnm);

/*
 ****************************************************************
 *	Chamada ao sistema "mount"				*
 ****************************************************************
 */
int
mount (const char *dev_nm, const char *dir_nm, const struct mntent *mntent_ptr)
{
	dev_t		dev;
	INODE		*dir_ip, *dev_ip;
	SB		*sp, *tsp;
	DISKTB		*up;
	int		oflag, code;
	IOREQ		io;
	MNTENT		mntent;
	DIRENT		de;

	/*
	 *	Esta operação é reservado ao SU
	 */
	if (superuser () < 0)
		return (-1);

	if (unimove (&mntent, mntent_ptr, sizeof (MNTENT), US) < 0)
		{ u.u_error = EFAULT; return (-1); }

	/*
	 *	Verifica se é uma montagem de NFS
	 */
	if (mntent.me_server_addr != -1)
		return (nfs_do_mount (dev_nm, dir_nm, &mntent));

	/*
	 *	Obtém o sistema de arquivos a ser montado e verifica se é PSEUDO
	 */
	if ((dev_ip = iname (dev_nm, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	oflag = (mntent.me_flags & SB_RONLY) ? O_READ: O_RW;

	if   (S_ISREG (dev_ip->i_mode))
	{
		/* Trata-se de um dispositivo simulado em um arquivo regular (PSEUDO) */

		if ((dev = pseudo_alloc_dev (dev_ip)) < 0)
			goto out_1;

		block_free (dev, NOINO);

		update (dev_ip->i_dev, 1 /* blocks_also */);
		block_free (dev_ip->i_dev, NOINO);

		if (dev_ip->i_sb->sb_mntent.me_flags & SB_RONLY)
			{ mntent.me_flags |= SB_RONLY; oflag = O_READ; }

		(*dev_ip->i_fsp->fs_open) (dev_ip, oflag|O_LOCK);

		if (u.u_error)
			goto out_2;
	}
	elif (S_ISBLK (dev_ip->i_mode))
	{
		/* Trata-se de um dispositivo de verdade */

		dev = dev_ip->i_rdev;

		if ((unsigned)(MAJOR (dev)) >= scb.y_nblkdev)
			{ u.u_error = ENXIO;   goto out_1; }
	}
	else
	{
		u.u_error = ENXIO;   goto out_1;
	}
	
	/*
	 *	Verifica se não excedeu o no. permitido de MOUNTs
	 */
	SPINLOCK (&sblistlock);

#ifdef	USE_NMOUNT
	if (nmount >= scb.y_nmount)
		{ SPINFREE (&sblistlock); u.u_error = EAGAIN; goto out_3; }
#endif	USE_NMOUNT

	/*
	 *	Verifica se o dispositivo, por acaso, já está montado
	 */
	for (tsp = sb_head.sb_forw; tsp != &sb_head; tsp = tsp->sb_forw)
	{
		if (tsp->sb_dev == dev)
			{ SPINFREE (&sblistlock); u.u_error = EBUSY; goto out_3; }
	}

#ifdef	USE_NMOUNT
	nmount++;
#endif	USE_NMOUNT

	SPINFREE (&sblistlock);

	/*
	 *	Obtem o diretório onde o sistema de arquivos vai ser montado
	 */
	if ((dir_ip = iname (dir_nm, getuchar, ISEARCH, &io, &de)) == NOINODE)
		goto out_4;

	if (dir_ip->i_count > 1)
		{ u.u_error = EBUSY; goto out_5; }

	if ((dir_ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto out_5; }

	/*
	 *	Verifica se o dispositivo está ONLINE
	 */
#if (0)	/*******************************************************/
	if (block_in_core (dev, 0) >= 0)
		printf ("Mount: O bloco 0 já estava no CACHE\n");
#endif	/*******************************************************/

	(*biotab[MAJOR (dev)].w_open) (dev, oflag);

	if (u.u_error)
		goto out_5;

	/*
	 *	Aloca o SB
	 */
	if ((sp = malloc_byte (sizeof (SB))) == NOSB)
		{ u.u_error = ENOMEM; goto out_6; }

	memclr (sp, sizeof (SB));

	/*
	 *	Copia o nome do dispositivo e do diretório
	 */
	if (get_user_str (sp->sb_dev_nm, dev_nm, sizeof (sp->sb_dev_nm)) < 0)
		{ u.u_error = EFAULT; goto out_7; }

	if (get_user_str (sp->sb_dir_nm, dir_nm, sizeof (sp->sb_dir_nm)) < 0)
		{ u.u_error = EFAULT; goto out_7; }

	/*
	 *	Preenche os campos restantes
	 */
	memmove (&sp->sb_mntent, &mntent, sizeof (MNTENT));

	sp->sb_uid	= u.u_ruid;
	sp->sb_gid	= u.u_rgid;

   /***	CLEAR (&sp->sb_mlock);	***/
   /***	sp->sb_sbmod	= 0;	***/
   /***	sp->sb_mbusy	= 0;	***/

   /***	sp->sb_mcount	= 0;	***/

	sp->sb_inode	= dir_ip;
	sp->sb_dev	= dev;

   /***	sp->sb_back	= ...;	/* abaixo ***/
   /***	sp->sb_forw	= ...;	/* abaixo ***/

   /***	sp->sb_code	= ...;	/* Na montagem específica ***/
   /***	sp->sb_ptr	= ...;	/* Na montagem específica ***/

	/*
	 *	Verifica se o código da partição está disponível
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB || up->p_type == 0)
	{
		for (code = FS_V7; /* abaixo */; code++)
		{
			if (code >= FS_LAST)
				goto out_8;

			if ((*fstab[code].fs_authen) (dev) >= 0)
				break;
		}
	}
	else
	{
		switch (up->p_type)
		{
		    case PAR_TROPIX_V7:
			if (v7_authen (dev) < 0)
				goto out_8;

			code = FS_V7;
			break;

		    case PAR_TROPIX_T1:
			if (t1_authen (dev) < 0)
				goto out_8;

			code = FS_T1;
			break;

		    case PAR_DOS_FAT12:
		    case PAR_DOS_FAT16_S:
		    case PAR_DOS_FAT16:
		    case PAR_DOS_FAT32:
		    case PAR_DOS_FAT32_L:
		    case PAR_DOS_FAT16_L:
			if (fat_authen (dev) < 0)
				goto out_8;

			code = FS_FAT;
			break;

		    case PAR_LINUX_EXT2:
			if (ext2_authen (dev) < 0)
				goto out_8;

			code = FS_EXT2;
			break;

		    case PAR_NTFS:
			if (nt_authen (dev) < 0)
				goto out_8;

			code = FS_NT;
			break;

		    default:
			goto out_8;
		}
	}

	/*
	 *	Realiza a montagem específica
	 */
	if ((*fstab[code].fs_mount) (sp) < 0)
		goto out_7;

	/* Coloca o novo SB no final da lista */

	SPINLOCK (&sblistlock);

	tsp		= sb_head.sb_back;

	tsp->sb_forw	= sp;
	sp->sb_back	= tsp;

	sp->sb_forw	= &sb_head;
	sb_head.sb_back	= sp;

	up->p_sb	= sp;

	SPINFREE (&sblistlock);

	/*
	 *	Epílogo
	 */
	dir_ip->i_flags |= IMOUNT;
	SLEEPFREE (&dir_ip->i_lock);

	SLEEPFREE (&dev_ip->i_lock);

	/*
	 *	Um epílogo para o "mount"
	 */
	if (code == FS_NT)		/* PROVISÓRIO */
		ntfs_mount_epilogue (sp);

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    out_8:
	u.u_error = ENOTFS;
    out_7:
	free_byte (sp);
    out_6:
	if (dev_ip->i_count > 1)
		(*biotab[MAJOR (dev)].w_close) (dev, 1);
	else
		(*biotab[MAJOR (dev)].w_close) (dev, 0);
    out_5:
	iput (dir_ip);
    out_4:
#ifdef	USE_NMOUNT
	SPINLOCK (&sblistlock);
	nmount--;
	SPINFREE (&sblistlock);
#endif	USE_NMOUNT
    out_3:
#if (0)	/*******************************************************/
	if (S_ISREG (dev_ip->i_mode))
		{ iinc (dev_ip); (*dev_ip->i_fsp->fs_close) (dev_ip); SLEEPLOCK (&dev_ip->i_lock, PINODE); }
#endif	/*******************************************************/
    out_2:
	if (S_ISREG (dev_ip->i_mode))
		pseudo_free_dev (dev);
    out_1:
	if (S_ISREG (dev_ip->i_mode))
		(*dev_ip->i_fsp->fs_close) (dev_ip);
	else
		iput (dev_ip);

	return (-1);

}	/* end mount */

/*
 ****************************************************************
 *	Montagem NFS						*
 ****************************************************************
 */
int
nfs_do_mount (const char *user_dev_nm, const char *user_dir_nm, const struct mntent *mntent_ptr)
{
	INODE		*dir_ip;
	NFSSB		*nfssp;
	DISKTB		*up;
	int		minor;
	long		pid;
	char		old_p_name_0;
	char		letter;
	dev_t		dev;
	SB		*sp, *back_sp;
	IOREQ		io;
	char		dev_nm[sizeof (sp->sb_dev_nm)];
	USTAT		us;
	DIRENT		de;

	/*
	 *	Verifica se, por acaso, já está montado
	 */
	if (get_user_str (dev_nm, user_dev_nm, sizeof (dev_nm)) < 0)
		{ u.u_error = EFAULT; return (-1); }

	SPINLOCK (&sblistlock);

	for (sp = sb_head.sb_forw; sp != &sb_head; sp = sp->sb_forw)
	{
		if (streq (sp->sb_dev_nm, dev_nm))
			{ SPINFREE (&sblistlock); u.u_error = EBUSY; return (-1); }
	}

	SPINFREE (&sblistlock);

	/*
	 *	Obtem o diretório onde o sistema de arquivos vai ser montado
	 */
	if ((dir_ip = iname (user_dir_nm, getuchar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	if (dir_ip->i_count > 1)
		{ u.u_error = EBUSY; goto out_5; }

	if ((dir_ip->i_mode & IFMT) != IFDIR)
		{ u.u_error = ENOTDIR; goto out_5; }

	/*
	 *	Aloca o SB
	 */
	if ((sp = malloc_byte (sizeof (SB))) == NOSB)
		{ u.u_error = ENOMEM; goto out_5; }

	memclr (sp, sizeof (SB));

	memmove (&sp->sb_mntent, mntent_ptr, sizeof (MNTENT));

	/*
	 *	Copia o nome do dispositivo e do diretório
	 */
	strcpy (sp->sb_dev_nm, dev_nm);

	if (get_user_str (sp->sb_dir_nm, user_dir_nm, sizeof (sp->sb_dir_nm)) < 0)
		{ u.u_error = EFAULT; goto out_7; }

	/*
	 *	Tenta comunicar-se com o servidor
	 */
	if ((nfssp = nfs2c_send_mntpgm_datagram (user_dev_nm, mntent_ptr, sp)) == NONFSSB)
		goto out_7;

	/*
	 *	Obtém um "dev" na tabela de partições
	 */
	for (letter = 'a'; /* abaixo */; letter += 1)
	{
		if (letter > 'z')
			{ printf ("%g: Mais do que 26 montagens NFS\n"); u.u_error = ENXIO; goto out_7; }

		for (up = next_disktb - 1; /* abaixo */; up--)
		{
			if (up < disktb)
				goto found_letter;

			if (up->p_name[0] != 'n' || up->p_name[1] != 'd' || up->p_name[3] != '\0')
				continue;

			if (up->p_name[2] == letter)
				break;
		}
	}

    found_letter:
	for (up = next_disktb - 1; /* abaixo */; up--)
	{
		if (up < disktb)
		{
			if ((up = next_disktb) >= end_disktb - 1)
				{ printf ("%g: DISKTB cheia\n"); u.u_error = ENXIO; goto out_7; }

			break;
		}

		if (up->p_name[0] == '*')
			break;
	}

	old_p_name_0 = up->p_name[0];

	memclr (up, sizeof (DISKTB));

	minor = up - disktb;

	up->p_name[0]	= 'n';		up->p_name[1] = 'd';
	up->p_name[2]	= letter;	up->p_name[3] = '\0';

	up->p_offset	= 0;		/* Ainda desconhecido */
	up->p_size	= 0;		/* Ainda desconhecido */

	up->p_dev	= dev = MAKEDEV (NFS_MAJOR, minor);
	up->p_unit	= 0;
	up->p_target	= 0;

   /***	up->p_type	= 0; ***/
   /***	up->p_flags	= 0; ***/

   /***	up->p_head	= 0; ***/
   /***	up->p_sect	= 0; ***/
   /***	up->p_cyl	= 0; ***/

   /***	up->p_nopen	= 0; ***/
   /***	up->p_lock	= 0; ***/

	/*
	 *	Cria o processo para a atualização dos blocos NFS
	 */
	if ((pid = newproc (FORK)) < 0)
		{ printf ("%g: NÃO consegui criar o processo de atualização NFS\n"); goto out_9; }

	if (pid == 0)
	{
		u.u_no_user_mmu = 1;

		snprintf (u.u_pgname, sizeof (u.u_pgname), "nfs_update_%s", up->p_name);

		set_pgname (u.u_pgname);

		if (regiongrow (&u.u_data, 0, 0, 0) < 0)
			{ u.u_error = ENOMEM; return (-1); }

		u.u_size -= u.u_dsize;
		u.u_dsize = 0;

		settgrp (0); u.u_pgrp = 0;

		nfs2_update_daemon (nfssp);

		/****** sem retorno *****/
	}

	nfssp->sb_update_pid = pid;

	/*
	 *	Preenche os campos restantes
	 */
	sp->sb_uid	= u.u_ruid;
	sp->sb_gid	= u.u_rgid;

   /***	CLEAR (&sp->sb_mlock);	***/
   /***	sp->sb_sbmod	= 0;	***/
   /***	sp->sb_mbusy	= 0;	***/

   /***	sp->sb_mcount	= 0;	***/

	sp->sb_inode	= dir_ip;
	sp->sb_dev	= dev;

   /***	sp->sb_back	= ...;	/* abaixo ***/
   /***	sp->sb_forw	= ...;	/* abaixo ***/

	sp->sb_code	= FS_NFS2;
	sp->sb_ptr	= nfssp;

#if (0)	/*******************************************************/
	strscpy (sp->sb_fname, "???", sizeof (sp->sb_fname));	/* PROVISÓRIO */
	strscpy (sp->sb_fpack, "???", sizeof (sp->sb_fpack));	/* PROVISÓRIO */
#endif	/*******************************************************/

	sp->sb_root_ino = nfssp->sb_root_ino;

   /***	nfssp->sb_sp = sp;	***/

	/*
	 *	Coloca o novo SB no final da lista
	 */
	SPINLOCK (&sblistlock);

	back_sp		= sb_head.sb_back;

	back_sp->sb_forw = sp; sp->sb_back = back_sp;
	sp->sb_forw = &sb_head; sb_head.sb_back	= sp;

	up->p_sb	= sp;

	SPINFREE (&sblistlock);

	/*
	 *	Obtém o USTAT do sistema de arquivos montado
	 */
	nfs2_get_ustat (sp, &us);

	up->p_size = (us.f_bsize / BLSZ) * us.f_fsize;

	/*
	 *	Epílogo
	 */
	dir_ip->i_flags |= IMOUNT;
	SLEEPFREE (&dir_ip->i_lock);

	if (up == next_disktb)
		{ up++; up->p_name[0] = '\0'; next_disktb = up; }

	return (0);

	/*
	 *	Em caso de ERRO
	 */
    out_9:
	up->p_name[0] = old_p_name_0;
    out_7:
	free_byte (sp);
    out_5:
	iput (dir_ip);
	return (-1);

}	/* end nfs_do_mount */

/*
 ****************************************************************
 *	Chamada ao sistema "umount"				*
 ****************************************************************
 */
int
umount (const char *user_dev_nm, int flags)
{
	dev_t		dev;
	int		sb_code; 
	INODE		*dir_ip, *dev_ip, *ip;
	SB		*sp;
	BHEAD		*bp;
	DISKTB		*up;
	IOREQ		io;
	char	    	dev_nm[sizeof (sp->sb_dev_nm)];
	DIRENT		de;

	/*
	 *	Esta operação é reservado ao SU
	 */
	if (superuser () < 0)
		return (-1);

	/*
	 *	Verifica se é uma montagem NFS
	 */
	if (get_user_str (dev_nm, user_dev_nm, sizeof (dev_nm)) < 0)
		{ u.u_error = EFAULT; return (-1); }

	if (strchr (dev_nm, ':'))
		return (nfs_do_umount (dev_nm, flags));

	/*
	 *	Obtem o dispositivo a ser desmontado
	 */
	if ((dev_ip = iname (dev_nm, getschar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	switch (dev_ip->i_mode & IFMT)
	{
	    case IFBLK:
		dev = dev_ip->i_rdev;
		break;

	    case IFREG:
		if ((dev = pseudo_search_dev (dev_ip)) == NODEV)
			goto bad;

		break;

	    default:
	    bad:
		{ u.u_error = EINVAL; iput (dev_ip); return (-1); }
	}

	iput (dev_ip);		/* "i_count" ainda deve ser > 0 (do mount) */

	if ((unsigned)(MAJOR (dev)) >= scb.y_nblkdev)
		{ u.u_error = ENXIO; return (-1); }

	if ((up = disktb_get_entry (dev)) == NODISKTB)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Não se pode desmontar o ROOT nem PIPE
	 */
	if (dev == scb.y_rootdev || dev == scb.y_pipedev)
		{ u.u_error = EBUSY; return (-1); }

	/*
	 *	Procura o Dispositivo na lista de SBs
	 */
	if ((sp = sbget (dev)) == NOSB)
		{ u.u_error = ENOTMNT; return (-1); }

	sb_code = sp->sb_code;

	if (flags == -1)
		flags = sp->sb_mntent.me_flags;

	/*
	 *	Se não é o superusuário, tem de ser o mesmo usuário
	 */
	if (u.u_ruid != 0 && u.u_ruid != sp->sb_uid)
		{ sbput (sp); u.u_error = EPERM; return (-1); }

	/*
	 *	Verifica se este dispositivo está sendo
	 *	desmontado por outro processador
	 */
	if (sp->sb_mbusy)
		{ sbput (sp); u.u_error = EBUSY; return (-1); }

	/*
	 *	Operação de UMOUNT em andamento: impede outra operação no
	 *	mesmo dispositivo assim como abrir arquivos
	 */
	sp->sb_mbusy = 1;

	sbput (sp);

	/*
	 *	Tenta remover os textos e atualiza o disco
	 */
	if (xumount (dev) < 0)
		{ SPINLOCK (&sp->sb_mlock); goto busy; }

	/*
	 *	Para alguns systemas de arquivos, ...
	 */
	if (sp->sb_code == FS_NT)		/* PROVISÓRIO */
	{
		INODE		*mft_ip = ((NTSB *)sp->sb_ptr)->s_mft_inode;

		if (mft_ip != NOINODE)
{
if (mft_ip < scb.y_inode || mft_ip >= scb.y_endinode || ((int)mft_ip - (int)scb.y_inode) % sizeof (INODE))
printf ("umount: INODE inválido %P\n", mft_ip);
			{ SLEEPLOCK (&mft_ip->i_lock, PINODE); iput (mft_ip); }
}
	}

	/*
	 *	Atualiza os blocos e INODEs no disco
	 */
	update (dev, 1 /* blocks_also */);

	/*
	 *	Verifica se sobrou algum arquivo referenciado no dispositivo
	 */
	SPINLOCK (&sp->sb_mlock);

	if (sp->sb_mcount != 0)
		goto busy;

	dir_ip = sp->sb_inode;

	SPINFREE (&sp->sb_mlock);

	/*
	 *	Desfaz a entrada
	 */
	SLEEPLOCK (&dir_ip->i_lock, PINODE);
	dir_ip->i_flags &= ~IMOUNT;
	iput (dir_ip);

	/*
	 *	Retira o SB da lista; repare que sp != scb.y_rootsb
	 */
	SPINLOCK (&sblistlock);

	up->p_sb = NOVOID;

	sp->sb_forw->sb_back = sp->sb_back;
	sp->sb_back->sb_forw = sp->sb_forw;

#ifdef	USE_NMOUNT
	nmount--;
#endif	USE_NMOUNT

	SPINFREE (&sblistlock);

	free_byte (sp->sb_ptr);
	free_byte (sp);

	/*
	 *	Fecha o dispositivo
	 */
	SLEEPLOCK (&dev_ip->i_lock, PINODE);

	if   (S_ISREG (dev_ip->i_mode))
		{ (*dev_ip->i_fsp->fs_close) (dev_ip); pseudo_free_dev (dev); }
	elif (dev_ip->i_count > 1)
		{ (*biotab[MAJOR (dev)].w_close) (dev, 1); iput (dev_ip); }
	else
		{ (*biotab[MAJOR (dev)].w_close) (dev, 0); iput (dev_ip); }

	/*
	 *	Remove INODEs e BHEADs do Dispositivo
	 */
	iumerase (dev, sb_code);
	block_free (dev, NOINO);

#define	SEGURO
#ifdef	SEGURO
	/*
	 *	Seguro morreu de velho: confere INODEs	
	 */
	for (ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_mode == 0)
			continue;

		if (ip->i_dev == dev)
			printf ("%g: INODE Residual: (%v, %d)\n", dev, ip->i_ino);
	}

	/*
	 *	Seguro morreu de velho: confere BUFFERs	
	 */
	for (bp = scb.y_bhead; bp < scb.y_endbhead; bp++)
	{
		if (bp->b_dev == dev)
			printf ("%g: BUFFER Residual: (%v, %d)\n", dev, bp->b_blkno);
	}
#endif	SEGURO

	/*
	 *	Tenta ejetar, se for o caso
	 */
	if (flags & SB_EJECT)
		(*biotab[MAJOR (dev)].w_ioctl) (dev, ZIP_START_STOP, 2, 0);

	return (0);

	/*
	 *	Ainda tem arquivos referenciados no dispositivo
	 */
    busy:
	sp->sb_mbusy = 0;
	SPINFREE (&sp->sb_mlock);

	u.u_error = EBUSY;

	return (-1);

}	/* end umount */

/*
 ****************************************************************
 *	Desmontagem NFS						*
 ****************************************************************
 */
int
nfs_do_umount (const char *dev_nm, int flags)
{
	SB		*sp;
	NFSSB		*nfssp;
	DISKTB		*up;
	BHEAD		*bp;
	INODE		*ip, *dir_ip;
	dev_t		dev;

	/*
	 *	Localiza o superbloco
	 */
	SPINLOCK (&sblistlock);

	for (sp = sb_head.sb_forw; /* abaixo */; sp = sp->sb_forw)
	{
		if (sp == &sb_head)
			{ SPINFREE (&sblistlock); u.u_error = ENXIO; return (-1); }

		if (streq (sp->sb_dev_nm, dev_nm))
			break;
	}

	dev = sp->sb_dev; nfssp = sp->sb_ptr;

	SPINFREE (&sblistlock);

	/*
	 *	Localiza a entrada da DISKTB
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica se este dispositivo está sendo desmontado por outro processador
	 */
	SPINLOCK (&sp->sb_mlock);

	if (sp->sb_mbusy)
		{ SPINFREE (&sp->sb_mlock); u.u_error = EBUSY; return (-1); }

	/*
	 *	Operação de UMOUNT em andamento
	 */
	sp->sb_mbusy = 1;

	SPINFREE (&sp->sb_mlock);

	/*
	 *	Tenta remover os textos e atualiza o disco
	 */
	if (xumount (dev) < 0)
		{ SPINLOCK (&sp->sb_mlock); goto busy; }

	/*
	 *	Atualiza os blocos e INODEs no disco
	 */
	update (dev, 1 /* blocks_also */);

	/*
	 *	Verifica se sobrou algum arquivo referenciado no dispositivo
	 */
	SPINLOCK (&sp->sb_mlock);

	if (sp->sb_mcount != 0)
		goto busy;

	dir_ip = sp->sb_inode;

	SPINFREE (&sp->sb_mlock);

	/*
	 *	Desfaz a entrada
	 */
	SLEEPLOCK (&dir_ip->i_lock, PINODE);
	dir_ip->i_flags &= ~IMOUNT;
	iput (dir_ip);

	/*
	 *	Envia a mensagem para desmontar ao servidor
	 */
	nfs2c_send_umntpgm_datagram (nfssp, strchr (dev_nm, ':') + 1);

	/*
	 *	Mata o processo de atualização
	 */
	if (kill (nfssp->sb_update_pid, SIGKILL) < 0)
		printf ("%g: NÃO consegui matar o processo de atualização NFS\n");

	/*
	 *	Retira o SB da lista; repare que sp != scb.y_rootsb
	 */
	SPINLOCK (&sblistlock);

	up->p_sb = NOVOID;

	sp->sb_forw->sb_back = sp->sb_back;
	sp->sb_back->sb_forw = sp->sb_forw;

#ifdef	USE_NMOUNT
	nmount--;
#endif	USE_NMOUNT

	SPINFREE (&sblistlock);

	nfs2_free_all_nfsdata (nfssp);
	free_byte (nfssp);
	free_byte (sp);

	/*
	 *	Remove INODEs e BHEADs do Dispositivo
	 */
	iumerase (dev, FS_NFS2);
	block_free (dev, NOINO);

	/*
	 *	Libera a entrada de DISKTB
	 */
	disktb_remove_partitions (up);

#define	SEGURO
#ifdef	SEGURO
	/*
	 *	Seguro morreu de velho: confere INODEs	
	 */
	for (ip = scb.y_inode; ip < scb.y_endinode; ip++)
	{
		if (ip->i_mode == 0)
			continue;

		if (ip->i_dev == dev)
			printf ("%g: INODE Residual: (%v, %d)\n", dev, ip->i_ino);
	}

	/*
	 *	Seguro morreu de velho: confere BUFFERs	
	 */
	for (bp = scb.y_bhead; bp < scb.y_endbhead; bp++)
	{
		if (bp->b_dev == dev)
			printf ("%g: BUFFER Residual: (%v, %d)\n", dev, bp->b_blkno);
	}
#endif	SEGURO

	return (0);

	/*
	 *	Ainda tem arquivos referenciados no dispositivo
	 */
    busy:
	sp->sb_mbusy = 0;
	SPINFREE (&sp->sb_mlock);

	u.u_error = EBUSY;

	return (-1);

}	/* end nfs_do_umount */

/*
 ****************************************************************
 *	Chamada ao sistema "link"				*
 ****************************************************************
 */
int
link (const char *name, const char *linkname)
{
	return (klink (name, linkname, 0 /* NO rename */));

}	/* end link */

/*
 ****************************************************************
 *	Forma interna da chamada ao sistema "link"		*
 ****************************************************************
 */
int
klink (const char *name, const char *linkname, int rename)
{
	INODE		*ip, *lp;
	IOREQ		io;
	DIRENT		de;

	/*
 	 *	Examina o Arquivo
	 */
	if ((ip = iname (name, getuchar, LSEARCH, &io, &de)) == NOINODE)
		return (-1);

	/*
	 *	Caso especial: RENAME do NFS
	 */
	if (rename && ip->i_sb->sb_code == FS_NFS2)
		return (nfs2_rename (ip, &io, &de, linkname));

	/*
	 *	Somento o Superusuário pode criar um DIR novo
	 */
	if ((ip->i_mode & IFMT) == IFDIR && superuser () < 0)
		{ iput (ip); return (-1); }

	SLEEPFREE (&ip->i_lock);

	/*
	 *	Examina o Nome novo
	 */
	if ((lp = iname (linkname, getuchar, ICREATE, &io, &de)) != NOINODE)
		{ u.u_error = EEXIST; iput (lp); goto bad; }

	if (u.u_error != NOERR)
		goto bad;

	/*
	 *	Os 2 Nomes devem estar no mesmo FS
	 */
   /***	if (io.io_dev != ip->i_dev)	***/

	if (io.io_ip->i_dev != ip->i_dev)
		{ iput (io.io_ip); u.u_error = EXDEV; goto bad; }

	/*
	 *	Escreve o Novo nome no DIR
 	 */
	de.d_ino = ip->i_ino;

	(*io.io_ip->i_fsp->fs_write_dir) (&io, &de, ip, rename);

	if (u.u_error == NOERR)
	{
		SLEEPLOCK (&ip->i_lock, PINODE);
		ip->i_nlink++; inode_dirty_inc (ip);
		iput (ip);

		return (0);
	}

	/*
	 *	Em caso de erro
	 */
    bad:
	SLEEPLOCK (&ip->i_lock, PINODE);
	iput (ip);

	return (u.u_error == NOERR ? 0 : -1);

}	/* end klink */

/*
 ****************************************************************
 *	Chamada ao sistema "unlink"				*
 ****************************************************************
 */
int
unlink (const char *path)
{
	return (kunlink (path, 0 /* NO rename */));

}	/* end unlink */

/*
 ****************************************************************
 *	Forma interna da chamada ao sistema "unlink"		*
 ****************************************************************
 */
int
kunlink (const char *path, int rename)
{
	INODE		*ip, *pp;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Obtem o Pai
	 */
	if ((pp = iname (path, getuchar, IDELETE, &io, &de)) == NOINODE)
		return (-1);

	/*
	 *	Isto não deve acontecer, mas ...
	 */
	if (pp->i_ino == de.d_ino)
		{ u.u_error = EINVAL; goto outp; }

	/*
	 *	Obtem o Filho (que deve ser apagado)
	 */
	if ((ip = iget (pp->i_dev, de.d_ino, 0)) == NOINODE)
		goto outp;

	/*
	 *	Não podemos mais remover diretórios ...
	 */
	if ((ip->i_mode & IFMT) == IFDIR && !rename)
		{ printf ("%g: Tentando remover o diretório \"%s\"\n", path); u.u_error = EISDIR; goto out; }

	/*
	 *	Verifica se é um arquivo de um FS montado
	 */
	if (ip->i_dev != pp->i_dev)
		{ u.u_error = EBUSY; goto out; }

	if (pseudo_search_dev (ip) != NODEV)
		{ u.u_error = EBUSY; goto out; }

	xuntext (ip, 0 /* no lock */);

	if ((ip->i_flags & ITEXT) && ip->i_nlink == 1)
		{ u.u_error = ETXTBSY; goto out; }

	io.io_ip = pp;

	(*pp->i_fsp->fs_clr_dir) (&io, &de, ip);

	if (ip->i_nm != NOSTR)
		{ free_byte (ip->i_nm); ip->i_nm = NOSTR; }

	ip->i_nlink--;
	inode_dirty_inc (ip);

	/*
	 *	Libera os INODEs
	 */
    out:
	iput (ip);
    outp:
	iput (pp);

	return (u.u_error == NOERR ? 0 : -1);

}	/* end kunlink */

/*
 ****************************************************************
 *	Chamada ao sistema "rename"				*
 ****************************************************************
 */
int
rename (const char *old_nm, const char *new_nm)
{
	int		code;

	if ((code = klink (old_nm,  new_nm, 1 /* YES, rename */)) < 0)
		return (-1);

	if (code == 1)		/* NFS */
		return (0);

	return (kunlink (old_nm, 1 /* YES, rename */));

}	/* end rename */

/*
 ****************************************************************
 *	Chamada ao sistema "symlink"				*
 ****************************************************************
 */
int
symlink (const char *old_path, const char *new_path)
{
	IOREQ		io;
	INODE		*ip;
	DIRENT		de;
	int		len;

	/*
	 *	Obtém o tamanho do conteúdo
	 */
	if ((len = get_user_str (NOSTR, old_path, 0)) < 0)
		{ u.u_error = EFAULT; return (-1); }

	/*
	 *	Prepara para criar o arquivo
	 */
	if ((ip = iname (new_path, getuchar, ICREATE, &io, &de)) != NOINODE)
		{ u.u_error = EEXIST; iput (ip); return (-1); }

	if (u.u_error != NOERR)
		return (-1);

	if ((ip = (*io.io_ip->i_fsp->fs_write_symlink) (old_path, len + 1, &io, &de)) != NOINODE)
		iput (ip);

	return (UNDEF);

}	/* end symlink */

/*
 ****************************************************************
 *	Chamada ao sistema "readlink"				*
 ****************************************************************
 */
int
readlink (const char *path, char *area, int count)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;
	int		value;

	if ((ip = iname (path, getuchar, LSEARCH, &io, &de)) == NOINODE)
		return (-1);

	if ((ip->i_mode & IFMT) != IFLNK)
		{ u.u_error = EINVAL; goto bad; }

	io.io_ip     = ip;
	io.io_area   = area;
	io.io_count  = count;
	io.io_cpflag = SU;

	value = (*ip->i_fsp->fs_read_symlink) (&io);

	iput (ip);

	return (value);

	/*
	 *	Em caso de erro
	 */
    bad:
	iput (ip);
	return (-1);

}	/* end readlink */

/*
 ****************************************************************
 *	Chamada ao sistema "ustat""				*
 ****************************************************************
 */
int
ustat (dev_t dev, USTAT *area)
{
	SB		*unisp;
	USTAT		us;
	int		code;

	/*
	 *	Procura o Dispositivo na lista de SB's
	 */
	if ((unisp = sbget (dev)) == NOSB)
		{ u.u_error = ENOTMNT; return (-1); }

	code = unisp->sb_code;

	sbput (unisp);

	(*fstab[code].fs_ustat) (unisp, &us);

	if (unimove (area, &us, sizeof (USTAT), SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end ustat */
