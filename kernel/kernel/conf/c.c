/*
 ****************************************************************
 *								*
 *			c.c					*
 *								*
 *	Matrizes dos dispositivos (ligação com os drivers)	*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.6.0, de 02.09.04				*
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

#include "../h/inode.h"
#include "../h/sysdirent.h"
#include "../h/iotab.h"

#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define	null		nulldev
#define	void_nop 	((void (*) ())nulldev)
#define	int_nop		((int (*) ())nulldev)
#define	inode_nop	((INODE * (*) ())nulldev)
#define no		nodev
#define NO		NOTTY
#define WC		W_CLOSE
#define NODH		(DEVHEAD *)0

/*
 ******	Protótipos de funções de "ata" **************************
 */
extern int	ataattach ();
extern int	ataopen ();
extern int	ataclose ();
extern int	atastrategy ();
extern int	atactl ();
extern DEVHEAD	atatab;
extern int	ataread ();
extern int	atawrite ();

/*
 ******	Protótipos de funções de "fd" ***************************
 */
extern int	fdattach ();
extern int	fdopen ();
extern int	fdclose ();
extern int	fdstrategy ();
extern int	fdctl ();
extern DEVHEAD	fdtab;
extern int	fdread ();
extern int	fdwrite ();

/*
 ******	Protótipos de funções de "sd" ***************************
 */
extern int	sdattach ();
extern int	sdopen ();
extern int	sdclose ();
extern int	sdstrategy ();
extern int	sdctl ();
extern DEVHEAD	sdtab;
extern int	sdread ();
extern int	sdwrite ();

/*
 ******	Protótipos de funções de "ahc" **************************
 */
extern int	ahc_attach ();
extern int	ahc_open ();
extern int	ahc_close ();
extern int	ahc_strategy ();
extern int	ahc_ctl ();
extern DEVHEAD	ahc_tab;
extern int	ahc_read ();
extern int	ahc_write ();

/*
 ******	Protótipos de funções de "zip" **************************
 */
extern int	zipattach ();
extern int	zipopen ();
extern int	zipclose ();
extern int	zipstrategy ();
extern int	zipctl ();
extern DEVHEAD	ziptab;
extern int	zipread ();
extern int	zipwrite ();

/*
 ******	Protótipos de funções de "ramd" *************************
 */
extern int	ramdattach ();
extern int	ramdstrategy ();
extern int	ramdctl ();
extern DEVHEAD	ramdtab;
extern int	ramdread ();
extern int	ramdwrite ();

/*
 ******	Protótipos de funções de "meta_dos" *********************
 */
extern int	mdattach ();
extern int	mdopen ();
extern int	mdclose ();
extern int	mdstrategy ();
extern int	mdctl ();
extern DEVHEAD	mdtab;
extern int	mdread ();
extern int	mdwrite ();

/*
 ******	Protótipos de funções de "con" **************************
 */
extern int	conattach ();
extern int	conopen ();
extern int	conclose ();
extern int	conread ();
extern int	conwrite ();
extern int	conctl ();
extern TTY	con[];

/*
 ******	Protótipos de funções de "sio" **************************
 */
extern int	sioattach ();
extern int	sioopen ();
extern int	sioclose ();
extern int	sioread ();
extern int	siowrite ();
extern int	sioctl ();
extern TTY	sio[];

/*
 ******	Protótipos de funções de "tty" **************************
 */
extern int	teopen ();
extern int	teclose ();
extern int	teread ();
extern int	tewrite ();
extern int	tectl ();

/*
 ******	Protótipos de funções de "mem" **************************
 */
extern int	memread ();
extern int	memwrite ();
extern int	memctl ();

/*
 ******	Protótipos de funções de "itn" **************************
 */
extern int	itnopen ();
extern int	itnclose ();
extern int	itnread ();
extern int	itnwrite ();
extern int	itnctl ();

/*
 ******	Protótipos de funções de "pty" **************************
 */
extern int	ptycopen ();
extern int	ptysopen ();
extern int	ptycclose ();
extern int	ptysclose ();
extern int	ptycread ();
extern int	ptysread ();
extern int	ptycwrite ();
extern int	ptyswrite ();
extern int	ptycctl ();
extern int	ptysctl ();
extern TTY	pty[];

/*
 ******	Protótipos de funções de "slip" *************************
 */
extern int	slipopen ();
extern int	slipclose ();
extern int	slipwrite ();
extern int	slipctl ();
extern TTY	slip[];

/*
 ******	Protótipos de funções de "lp" ***************************
 */
extern int	lpattach ();
extern int	lpopen ();
extern int	lpclose ();
extern int	lpwrite ();
extern int	lpctl ();
extern TTY	lpb[];

/*
 ******	Protótipos de funções de "ed" ***************************
 */
extern int	edattach ();
extern int	edopen ();
extern int	edclose ();
extern int	edwrite ();
extern int	edctl ();

/*
 ******	Protótipos de funções de "ppp" **************************
 */
extern int	pppopen ();
extern int	pppclose ();
extern int	pppwrite ();
extern int	pppctl ();
extern TTY	ppp[];

/*
 ******	Protótipos de funções de "xcon" *************************
 */
extern int	xconopen ();
extern int	xconclose ();
extern int	xconread ();
extern int	xconctl ();
extern TTY	xcon;

/*
 ******	Protótipos de funções de "sb" ***************************
 */
extern int	sbattach ();
extern int	sbopen ();
extern int	sbclose ();
extern int	sbread ();
extern int	sbwrite ();
extern int	sbctl ();

/*
 ******	Protótipos de funções de "rtl" **************************
 */
extern int	rtlopen ();
extern int	rtlclose ();
extern int	rtlwrite ();
extern int	rtlctl ();

/*
 ******	Protótipos de funções de "ps2m" *************************
 */
extern int	ps2mattach ();
extern int	ps2mopen ();
extern int	ps2mclose ();
extern int	ps2mread ();
extern int	ps2mwrite ();
extern int	ps2mctl ();

/*
 ******	Protótipos de funções de "live" *************************
 */
extern int	liveopen ();
extern int	liveclose ();
extern int	livewrite ();

/*
 ******	Protótipos de funções de "ums" **************************
 */
extern int	umsopen ();
extern int	umsclose ();
extern int	umsread ();
extern int	umswrite ();
extern int	umsctl ();

/*
 ******	Protótipos de funções de "ulp" **************************
 */
extern int	ulpopen ();
extern int	ulpclose ();
extern int	ulpread ();
extern int	ulpwrite ();
extern int	ulpctl ();

/*
 ******	Protótipos de funções de "pseudo_dev" *******************
 */
extern int	pseudoattach ();
extern int	pseudoopen ();
extern int	pseudoclose ();
extern int	pseudostrategy ();
extern int	pseudoctl ();
extern DEVHEAD	pseudotab;
extern int	pseudoread ();
extern int	pseudowrite ();

/*
 ******	Protótipos de funções de "ud" ***************************
 */
extern int	udopen ();
extern int	udclose ();
extern int	udstrategy ();
extern DEVHEAD	udtab;
extern int	udread ();
extern int	udwrite ();
extern int	udctl ();

/*
 ******	Protótipos de funções de "nfs" **************************
 */
extern int	nfs2_strategy ();
extern DEVHEAD	nfs_tab;

/*
 ****************************************************************
 *	Dispositivos estruturados (de blocos)			*
 ****************************************************************
 */
const BIOTAB	biotab[]	=
{
/*	open		close		strategy	ioctl		DEVTAB	*/

	ataopen,	ataclose,	atastrategy,	atactl,		&atatab,	/*  0 */
	fdopen,		fdclose,	fdstrategy,	fdctl,		&fdtab,		/*  1 */
	sdopen,		sdclose,	sdstrategy,	sdctl,		&sdtab,		/*  2 */
	ahc_open,	ahc_close,	ahc_strategy,	ahc_ctl,	&ahc_tab,	/*  3 */
	zipopen,	zipclose,	zipstrategy,	zipctl,		&ziptab,	/*  4 */
	mdopen,		mdclose,	mdstrategy,	mdctl,		&mdtab,		/*  5 */
	null,		null,		ramdstrategy,	ramdctl,	&ramdtab,	/*  6 */
	null,		null,		pseudostrategy,	pseudoctl,	&pseudotab,	/*  7 */
	udopen,		udclose,	udstrategy,	udctl,		&udtab,		/*  8 */
	null,		null,		nfs2_strategy,	no,		&nfs_tab,	/*  9 */
	{ null },									/* 10 */
	{ null },									/* 11 */
	{ null },									/* 12 */
	{ null },									/* 13 */
	{ null },									/* 14 */
	{ null },									/* 15 */
	NOFUNC

}	/* end biotab */;

/*
 ****************************************************************
 *	Dispositivos Nao-Estruturados				*
 ****************************************************************
 */
const CIOTAB	ciotab[]	=
{
/*	attach		open		close		read		write		ioctl */

	ataattach,	ataopen,	ataclose,	ataread,	atawrite,	atactl,	    /*  0 */
	fdattach,	fdopen,		fdclose,	fdread,		fdwrite,	fdctl,	    /*  1 */
	sdattach,	sdopen,		sdclose,	sdread,		sdwrite,	sdctl,	    /*  2 */
	null,		ahc_open,	ahc_close,	ahc_read,	ahc_write,	ahc_ctl,    /*  3 */
	zipattach,	zipopen,	zipclose,	zipread,	zipwrite,	zipctl,	    /*  4 */
	mdattach,	mdopen,		mdclose,	mdread,		mdwrite,	mdctl,	    /*  5 */
	ramdattach,	null,		null,		ramdread,	ramdwrite,	ramdctl,    /*  6 */
	null,		null,		null,		no,		no,		no,	    /*  7 */
	null,		udopen,		udclose,	udread,		udwrite,	udctl,	    /*  8 */
	{ null },										    /*  9 */
	{ null },										    /* 10 */
	{ null },										    /* 11 */
	{ null },										    /* 12 */
	{ null },										    /* 13 */
	{ null },										    /* 14 */
	{ null },										    /* 15 */

	conattach,	conopen,	conclose,	conread,	conwrite,	conctl,	    /* 16 */
	null,		teopen,		teclose,	teread,		tewrite,	tectl,	    /* 17 */
	null,		null,		null,		memread,	memwrite,	memctl,	    /* 18 */
	sioattach,	sioopen,	sioclose,	sioread,	siowrite,	sioctl,	    /* 19 */
	null,		itnopen,	itnclose,	itnread,	itnwrite,	itnctl,	    /* 20 */
	null,		ptycopen,	ptycclose,	ptycread,	ptycwrite,	ptycctl,    /* 21 */
	null,		ptysopen,	ptysclose,	ptysread,	ptyswrite,	ptysctl,    /* 22 */
	null,		slipopen,	slipclose,	no,		slipwrite,	slipctl,    /* 23 */
	lpattach,	lpopen,		lpclose,	no,		lpwrite,	lpctl,	    /* 24 */
	edattach,	edopen,		edclose,	no,		edwrite,	edctl,	    /* 25 */
	null,		pppopen,	pppclose,	no,		pppwrite,	pppctl,	    /* 26 */
	null,		xconopen,	xconclose,	xconread,	no,		xconctl,    /* 27 */
	sbattach,	sbopen,		sbclose,	sbread,		sbwrite,	sbctl,	    /* 28 */
	null,		rtlopen,	rtlclose,	no,		rtlwrite,	rtlctl,	    /* 29 */
	ps2mattach,	ps2mopen,	ps2mclose,	ps2mread,	ps2mwrite,	ps2mctl,    /* 30 */
	null,		liveopen,	liveclose,	no,		livewrite,	null, 	    /* 31 */
	null,		umsopen,	umsclose,	umsread,	no,		umsctl,     /* 32 */
	null,		ulpopen,	ulpclose,	no,		ulpwrite,	no,	    /* 33 */
	NOFUNC

}	/* end ciotab */;

/*
 ****************************************************************
 *	Operações nos Sistemas de Arquivos			*
 ****************************************************************
 */
const FSTAB	fstab[] =
{
	/* 0: NONE */

	{
		FS_NONE,
		fs_inval_authen,		/* authen */
		fs_inval_mount,			/* mount */
		fs_inval_ustat,			/* ustat */
		fs_inval_update,		/* update */
		fs_inval_read_inode,		/* read_inode */
		fs_inval_write_inode,		/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		fs_inval_trunc_inode,		/* trunc_inode */
		fs_inval_free_inode,		/* free_inode */
		fs_inval_search_dir,		/* search_dir */
		fs_inval_read_dir,		/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		fs_inval_open,			/* open */
		fs_inval_close,			/* close */
		fs_inval_read,			/* read */
		fs_inval_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 1: CHR */

	{
		FS_CHR,
		fs_inval_authen,		/* authen */
		fs_inval_mount,			/* mount */
		fs_inval_ustat,			/* ustat */
		fs_inval_update,		/* update */
		fs_inval_read_inode,		/* read_inode */
		fs_inval_write_inode,		/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		fs_inval_trunc_inode,		/* trunc_inode */
		fs_inval_free_inode,		/* free_inode */
		fs_inval_search_dir,		/* search_dir */
		fs_inval_read_dir,		/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		chr_open,			/* open */
		chr_close,			/* close */
		chr_read,			/* read */
		chr_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 2: BLK */

	{
		FS_BLK,
		fs_inval_authen,		/* authen */
		fs_inval_mount,			/* mount */
		fs_inval_ustat,			/* ustat */
		fs_inval_update,		/* update */
		fs_inval_read_inode,		/* read_inode */
		fs_inval_write_inode,		/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		fs_inval_trunc_inode,		/* trunc_inode */
		fs_inval_free_inode,		/* free_inode */
		fs_inval_search_dir,		/* search_dir */
		fs_inval_read_dir,		/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		blk_open,			/* open */
		blk_close,			/* close */
		blk_read,			/* read */
		blk_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 3: PIPE */

	{
		FS_PIPE,
		fs_inval_authen,		/* authen */
		fs_inval_mount,			/* mount */
		pipe_get_ustat,			/* ustat */
		fs_inval_update,		/* update */
		pipe_read_inode,		/* read_inode */
		pipe_write_inode,		/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		pipe_trunc_inode,		/* trunc_inode */
		pipe_free_inode,		/* free_inode */
		fs_inval_search_dir,		/* search_dir */
		fs_inval_read_dir,		/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		pipe_open,			/* open */
		pipe_close,			/* close */
		fs_inval_read,			/* read */
		fs_inval_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 4: V7 */

	{
		FS_V7,
		v7_authen,			/* authen */
		v7_mount,			/* mount */
		v7_get_ustat,			/* ustat */
		v7_update,			/* update */
		v7_read_inode,			/* read_inode */
		v7_write_inode,			/* write_inode */
		v7_make_inode,			/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		v7_trunc_inode,			/* trunc_inode */
		v7_free_inode,			/* free_inode */
		v7_search_dir,			/* search_dir */
		v7_read_dir,			/* read_dir */
		v7_write_dir,			/* write_dir */
		v7_empty_dir,			/* empty_dir */
		v7_make_dir,			/* make_dir */
		v7_clr_dir,			/* clr_dir */
		v7_open,			/* open */
		v7_close,			/* close */
		v7_read,			/* read */
		v7_write,			/* write */
		v7_read_symlink,		/* read_symlink */
		v7_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 5: T1 */

	{
		FS_T1,
		t1_authen,			/* authen */
		t1_mount,			/* mount */
		t1_get_ustat,			/* ustat */
		t1_update,			/* update */
		t1_read_inode,			/* read_inode */
		t1_write_inode,			/* write_inode */
		t1_make_inode,			/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		t1_trunc_inode,			/* trunc_inode */
		t1_free_inode,			/* free_inode */
		t1_search_dir,			/* search_dir */
		t1_read_dir,			/* read_dir */
		t1_write_dir,			/* write_dir */
		t1_empty_dir,			/* empty_dir */
		t1_make_dir,			/* make_dir */
		t1_clr_dir,			/* clr_dir */
		t1_open,			/* open */
		t1_close,			/* close */
		t1_read,			/* read */
		t1_write,			/* write */
		t1_read_symlink,		/* read_symlink */
		t1_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 6: FAT */

	{
		FS_FAT,
		fat_authen,			/* authen */
		fat_mount,			/* mount */
		fat_get_ustat,			/* ustat */
		fat_update,			/* update */
		fat_read_inode,			/* read_inode */
		fat_write_inode,		/* write_inode */
		fat_make_inode,			/* make_inode */
		fat_free_clusvec,		/* rel_inode_attr */
		fat_trunc_inode,		/* trunc_inode */
		void_nop,			/* free_inode */
		fat_search_dir,			/* search_dir */
		fat_read_dir,			/* read_dir */
		fat_write_dir,			/* write_dir */
		fat_empty_dir,			/* empty_dir */
		fat_make_dir,			/* make_dir */
		fat_clr_dir,			/* clr_dir */
		v7_open,			/* open */
		v7_close,			/* close */
		fat_read,			/* read */
		fat_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fat_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 7: CD */

	{
		FS_CD,
		cd_authen,			/* authen */
		cd_mount,			/* mount */
		cd_get_ustat,			/* ustat */
		fs_inval_update,		/* update */
		cd_read_inode,			/* read_inode */
		cd_write_inode,			/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		fs_inval_trunc_inode,		/* trunc_inode */
		void_nop,			/* free_inode */
		cd_search_dir,			/* search_dir */
		cd_read_dir,			/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		v7_open,			/* open */
		v7_close,			/* close */
		cd_read,			/* read */
		fs_inval_write,			/* write */
		cd_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 8: EXT2 */

	{
		FS_EXT2,
		ext2_authen,			/* authen */
		ext2_mount,			/* mount */
		ext2_get_ustat,			/* ustat */
		ext2_update,			/* update */
		ext2_read_inode,		/* read_inode */
		ext2_write_inode,		/* write_inode */
		ext2_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		ext2_trunc_inode,		/* trunc_inode */
		ext2_free_inode,		/* free_inode */
		ext2_search_dir,		/* search_dir */
		ext2_read_dir,			/* read_dir */
		ext2_write_dir,			/* write_dir */
		ext2_empty_dir,			/* empty_dir */
		ext2_make_dir,			/* make_dir */
		ext2_clr_dir,			/* clr_dir */
		ext2_open,			/* open */
		ext2_close,			/* close */
		ext2_read,			/* read */
		ext2_write,			/* write */
		ext2_read_symlink,		/* read_symlink */
		ext2_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 9: NT */

	{
		FS_NT,
		nt_authen,			/* authen */
		nt_mount,			/* mount */
		nt_get_ustat,			/* ustat */
		fs_inval_update,		/* update */
		nt_read_inode,			/* read_inode */
		nt_write_inode,			/* write_inode */
		fs_inval_make_inode,		/* make_inode */
		ntfs_free_all_ntvattr,		/* rel_inode_attr */
		nt_trunc_inode,			/* trunc_inode */
		nt_free_inode,			/* free_inode */
		nt_search_dir,			/* search_dir */
		nt_read_dir,			/* read_dir */
		fs_inval_write_dir,		/* write_dir */
		fs_inval_empty_dir,		/* empty_dir */
		fs_inval_make_dir,		/* make_dir */
		fs_inval_clr_dir,		/* clr_dir */
		nt_open,			/* open */
		nt_close,			/* close */
		nt_read,			/* read */
		fs_inval_write,			/* write */
		fs_inval_read_symlink,		/* read_symlink */
		fs_inval_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	},

	/* 10: NFS2 */

	{
		FS_NFS2,
		fs_inval_authen,		/* authen */
		fs_inval_mount,			/* mount */
		nfs2_get_ustat,			/* ustat */
		fs_inval_update,		/* update */
		nfs2_read_inode,		/* read_inode */
		nfs2_write_inode,		/* write_inode */
		nfs2_make_inode,		/* make_inode */
		fs_inval_rel_inode_attr,	/* rel_inode_attr */
		nfs2_trunc_inode,		/* trunc_inode */
		nfs2_free_inode,		/* free_inode */
		nfs2_search_dir,		/* search_dir */
		nfs2_read_dir,			/* read_dir */
		nfs2_write_dir,			/* write_dir */
		nfs2_empty_dir,			/* empty_dir */
		nfs2_make_dir,			/* make_dir */
		nfs2_clr_dir,			/* clr_dir */
		nfs2_open,			/* open */
		nfs2_close,			/* close */
		nfs2_read,			/* read */
		nfs2_write,			/* write */
		nfs2_read_symlink,		/* read_symlink */
		nfs2_write_symlink,		/* write_symlink */
		fs_inval_ioctl			/* ioctl */
	}

}	/* end fstab */;
