/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Coleção dos protótipos das funções			*
 *								*
 *	Versão	3.0.0, de 04.07.94				*
 *		4.9.0, de 04.10.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
extern void		*alloca (int);
extern int		askyesno (int);
extern dev_t		strtodev (const char *);
extern void		block_lru_init (void);
extern long		big_to_little_long (long);
extern int		big_to_little_short (short);
extern void		block_flush (dev_t);
extern void		block_update (void);
extern void		ext2_indir_block_release (SB *, daddr_t, int);
extern void		ext2_block_release (SB * , daddr_t);
extern void		t1_indir_block_release (SB *, daddr_t, int);
extern void		t1_block_release (SB * , daddr_t);
extern void		v7_block_release (dev_t, daddr_t);
extern void		cinit (void);
extern int		coredump (void);
extern void		check_arena (const char *, int);
extern int		ctxtsto (ctxt_t);
extern void		ctxtld (UPROC *);
extern void		DELAY (int);
extern const char	*edit_dev_nm (dev_t);
extern void		disable_int (void);
extern void		dispatcher (void);
extern void		dmaexternal (int);
extern void		dns_resolver_every_second_work (void);
extern const char	*edit_file_mode (int mode);
extern void		editpercent (char [], int, int);
extern const		char *edit_file_system_code (int code);
extern void		enable_int (void);
extern void		ether_init (void);
extern int		fd_open (int);
extern void		fd_close (int);
extern int		fd_strategy (int, int, long, void *);
extern int		ffbs (unsigned);
extern void		flushchar (void);
extern void		free_byte (void *);
extern int		getpercent (int numer, int denom);
extern long		little_to_big_long (long);
extern int		little_to_big_short (short);
extern void		lfinit (void);
extern long		long_endian_cv (long);
extern int		fat_get_fat_value (const SB *unisp, clusno_t clusno);
extern void		fat_put_fat_value (const SB *unisp, int value, clusno_t clusno);
extern void		fat_get_ustat (const SB *unisp, USTAT *up);
extern void		fat_put_time (time_t tempo, ushort *fat_time, ushort *fat_date);
extern void		fat_update (const SB *unisp);
extern void		fpu_init (void);
extern void		fpu_save (long *);
extern void		fpu_restore (const long *);
extern void		fpu_set_embit (void);
extern void		fpu_reset_embit (void);
extern void		fpu_test (void);
extern long		get_cr0 (void);
extern void		put_cr0 (long);
extern void		*get_cr2 (void);
extern void		*get_fp (void);
extern long		get_sr (void);
extern int		getchar (void);
extern int		getschar (const char *);
extern int		getuchar (const char *);
extern char		*gets (char *);
extern void		*getuframe (void);
extern int		gubyte (const void *);
extern long		gulong (const void *);
extern int		get_user_str (char *dst, const char *src, int count);
extern void		init_user_path (char *dst, const char *src, int count, int *next);
extern int		get_user_path (void);
extern int		haschar (void);
extern idp_t		idcan (idp_t, const char *);
extern idp_t		idcpy (idp_t, const idp_t);
extern idp_t		idclr (idp_t);
extern int		ideq (const idp_t, const idp_t);
extern void		idle (void);
extern void		idle_inc_color (void);
extern void		inval_TLB (pg_t, pg_t);
extern int		kmkdev (const char *, long, dev_t);
extern int		log2 (int number);
extern void		nfs2_get_ustat (const SB *unisp, USTAT *up);
extern const char	*region_nm_edit (int region);
extern int		t1_mount (SB *);
extern void		t1_update (const SB *sp);
extern int		t1_authen (dev_t);
extern void		t1_get_ustat (const SB *unisp, USTAT *up);
extern const char	*t1_edit_block_type (int type);
extern int		v7_mount (SB *);
extern void		v7_update (const SB *sp);
extern int		v7_authen (dev_t);
extern void		v7_get_ustat (const SB *unisp, USTAT *up);
extern int		ext2_mount (SB *);
extern int		ext2_authen (dev_t);
extern void		ext2_get_ustat (const SB *unisp, USTAT *up);
extern void		ext2_update (const SB *sp);
extern int		fat_mount (SB *);
extern int		fat_authen (dev_t);
extern int		cd_authen (dev_t);
extern int		cd_mount (SB *);
extern time_t		cd_get_time (const char *);
extern void		cd_get_ustat (const SB *, USTAT *);
extern int		nt_authen (dev_t);
extern int		nt_mount (SB *);
extern void		nt_update (const SB *sp);
extern void		nt_get_ustat (const SB *unisp, USTAT *up);
extern int		link (const char *, const char *);
extern int		unlink (const char *);

#ifdef	CDFS_H
extern	void		cd_get_nlink_dir (INODE *);
#endif	CDFS_H

extern void		v7_free_indir_blk (dev_t, daddr_t, int);
extern void		init_clock (void);
extern void		init_time_from_cmos (void);
extern void		inode_lru_init (void);
extern void		irelease (dev_t, ino_t);
extern void		iumerase (dev_t, int);
extern int		jmpset (jmp_t);
extern void		jmpdo (jmp_t);
extern void		kexit (int, void *, void *);
extern int		key_code_to_char (int);
extern void		key_init_table (void);
extern void		key_ready_wait (void);
extern int		kill (int, int);
extern idp_t		lidcan (idp_t, const char *);
extern int		lideq (const idp_t, const idp_t);
extern void		lfinit (void);
extern void		load_cr3 (void);
extern void		*malloc_byte (int size);
extern void 		map_init_free_list (void);
extern void		memclr (void *, int);
extern int		memeq (const void *, const void *, int);
extern int		memnocaseeq (const void *, const void *, int);
extern int		memmove (void *, const void *, int);
extern int		memcpy (void *, const void *, int);
extern int		memtst (void *, int);
extern void		memset (void *, int, int);
extern void		memsetw (void *, int, int);
extern void		memsetl (void *, int, int);
extern void		mem_dump (void);
extern void		mmu_load (void);
extern int		mmutst (pg_t, pg_t, pg_t);
extern ulong		mmu_map_phys_addr (ulong physaddr, int size);
extern ulong		mul_div_64 (ulong, ulong, ulong);
extern long		newproc (int);
extern int		nodev ();
extern int		nosys (void);
extern int		nulldev ();
extern void		panic (const char *, ...);
extern int		patmatch (const char *cp /* Cadeia */, const char *pp /* Padrão */);
extern int		pci_init (void);
extern void		pgclr (pg_t, int);
extern void		pgcpy (pg_t, pg_t, int);
extern void		pinit (void);
extern int		pipe (void);
extern int		pipe_attention (int, char **);
extern int		pipe_unattention (void);
extern void		pipe_get_ustat (const SB *unisp, USTAT *up);
extern void		pnp_init (void);
extern void		printf (const char *, ...);
extern void		print_calls (void);
extern void		print_part_type_nm (int);
extern void		procswitch (void);
extern void		prtable (void);
extern const char	*proc_state_nm_edit (int);
extern int		pubyte (void *, int);
extern int		pulong (void *, long);
extern void		putchar (int);
extern void		*realloc_byte (void *area, int size);
extern int		read_cmos (int);
extern int		read_port (int);
extern int		read_port_short (int);
extern long		read_port_long (int);
extern void		read_port_string_byte (int, void *, int);
extern void		read_port_string_short (int, void *, int);
extern void		read_port_string_long (int, void *, int);
extern void		releaseallprocregions (void);
extern void		releaseprocregion (int);
extern void		reset_cpu (void);
extern void		sched (void);
extern void		screen_saver_every_second_work (void);
extern void		screen_saver_inc (void);
extern void		screen_saver_on (void);
extern void		screen_saver_off (void);
extern void		screen_saver_clr (void);
extern void		set_pgname (const char *pgnm);
extern int		shlib_kernel_load (const char *);
extern int		shlib_attach (int);
extern int		shlib_extref_resolve (void *text, int, const void *reftb, int refsize);
extern int		sprintf (char *, const char *, ...);
extern int		snprintf (char *, int, const char *, ...);
extern char		*scsi_str_conv (char *, const char *, int);
extern int		short_endian_cv (int);
extern void		setprotprocregion (int, int);
extern int		setstacksz (void *);
extern int		set_dev_irq (int irq, int pl, int unit, void (*func) ());
extern void		set_idt (int, const void (*) (), int, int);
extern int		settgrp (int flag);
extern int		set_pl_mask (int, int);
extern void		sigexec (void *, void *);
extern int		sigpend (void);
extern void		sigtgrp (int, int);
extern void		siguser (void (*) (int, ...), int, void *, void *);
extern int		spl0 (void), spl1 (void), spl2 (void), spl3 (void);
extern int		spl4 (void), spl5 (void), spl6 (void), spl7 (void);
extern int		splx (int);
extern void		spyexcep (void);
extern int		streq (const char *, const char *);
extern int		strnocaseeq (const char *, const char *);
extern void		strcpy (char *, const char *);
extern void		strscpy (char *, const char *, int);
extern int		strlen (const char *);
extern char		*strchr (const char *str, int carac);
extern long		strtol (const char *, const char **, int);
extern int		strhash (const char *name, int len, int size);
extern int		superuser (void);
extern void		swap (pg_t, pg_t, pg_t, int);
extern void		swapinit (void);
extern void		switch_run_process (void);
extern void		timeout_init_free_list (void);
extern int		ttyconv (char *, char);
extern void		ttysiguser (void (*) ());
extern void		*user_area_to_phys_area (void *, int);
extern int		ufeget (void);
extern int		uni_dma_setup (int, void *, int, int);
extern char		*big_unicode2iso (char *dst, const void *src, int len);
extern char		*little_unicode2iso (char *dst, const void *src, int len);
extern int		unimove (void *, const void *, int, int);
extern void		update (dev_t, int);
extern void		usb_explorer (void);
extern void		video_clr_last_line (void);
extern void		put_video_char (int c, int line, int col, int char_color, int back_color);
extern void		video_cursor_off (void);
extern void		video_cursor_on (void);
extern void		video_change_display (int);
extern void		video_vt100_protocol (int);
extern void		video_clr_display (int copy_also);
extern void		video_load_display (void);
extern void		video_change_color (char new_ch_color, char new_bg_color);
extern void		vprintf (const char *, int *);
extern void		wait_for_ctl_Q (void);
extern int		wd_open (int);
extern void		wd_read_cmos (void);
extern int		wd_strategy (int, int, long, void *);
extern void		writechar (int);
extern void		write_port (int, int);
extern void		write_port_short (int, int);
extern void		write_port_long (long, int);
extern void		write_port_string_byte (int, const void *, int);
extern void		write_port_string_short (int, const void *, int);
extern void		write_port_string_long (int, const void *, int);
extern int		xumount (dev_t);

#ifdef A_OUT_H
extern const SYM	*getsyment (unsigned long);
extern int		shlib_status (int, const HEADER *);
#endif A_OUT_H

#ifdef BHEAD_H
extern daddr_t		t1_block_alloc (SB *, daddr_t, int);
extern daddr_t		v7_block_alloc (dev_t);
extern BHEAD		*bread (dev_t, daddr_t, ino_t);
extern BHEAD		*breada (dev_t, daddr_t, daddr_t, ino_t);
extern void		bwrite (BHEAD *);
extern void		bdwrite (BHEAD *);
extern int		block_in_core (dev_t, daddr_t, ino_t);
extern void		bsomeflush (void);
extern void		block_free (dev_t, ino_t);
extern int		geterror (BHEAD *);
extern BHEAD		*block_free_get (int);
extern BHEAD		*block_get (dev_t, daddr_t, ino_t);
extern void		block_put (BHEAD *);
extern void		brmhash (BHEAD	*, BHASHTB *);
extern void		physio (IOREQ *, int (*) (BHEAD *), BHEAD *, int, int);
extern void		pseudo_alloc_bhead (BHEAD *);
#endif BHEAD_H

#if	defined (BHEAD_H) && defined (DEVHEAD_H)
extern void		insdsort (DEVHEAD *, BHEAD *);
extern BHEAD		*remdsort (DEVHEAD *);
#endif	BHEAD_H && DEVHEAD_H

#if	defined (DISKTB_H)
extern const DISKTB	*get_dev (const char *);
extern DISKTB		*disktb_attach_entries (int, int, int, int); 
extern int		disktb_remove_partitions (DISKTB *);
extern int		disktb_close_entry (dev_t);
extern DISKTB		*disktb_get_entry (dev_t);
extern DISKTB		*disktb_get_first_entry (dev_t);
extern DISKTB		*disktb_create_partitions (DISKTB *);
#endif	DISKTB_H

#if	defined (FAT_H)
extern int		fat_get_cluster (const FATSB *, const FATDIR *);
extern void		fat_lfn_add (const FATDIR *, FATLFN *zp);
extern void		fat_lfn_get_nm (const SB *, const FATDIR *, FATLFN *zp);
#endif	FAT_H

#ifdef	INODE_H
extern void		inode_dirty_inc (INODE *ip);
extern void		inode_dirty_dec (INODE *ip);
extern int		lfaccess (INODE *, off_t, off_t, int);
extern int		lfclosefree (INODE *);
extern int		lffree (INODE *, off_t, off_t);
extern int		lflock (INODE *, off_t, off_t, int);
extern int		nfs2_read_inode (INODE *ip);
extern void		nfs2_write_inode (INODE *ip);
extern void		nfs2_empty_quantum (const INODE *ip);
extern INODE		*nfs2_make_inode (IOREQ *, DIRENT *, int);
extern void		nfs2_trunc_inode (INODE *);
extern void		nfs2_free_inode (const INODE *);
extern INODE		*nfs2_make_dir (IOREQ *iop, DIRENT *dep, int mode);
extern void		nfs2_write_dir (IOREQ *, const DIRENT *, const INODE *, int);
extern int		nfs2_empty_dir (INODE *);
extern void		nfs2_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern void		nfs2_open (INODE *, int);
extern void		nfs2_close (INODE *);
extern void		nfs2_read_dir (IOREQ *);
extern void		nfs2_read (IOREQ *);
extern void		nfs2_write (IOREQ *);
extern int		nfs2_read_symlink (IOREQ *iop);
extern int		nfs2_rename (INODE *ip, IOREQ *iop, const DIRENT *dep, const char *name);
extern INODE		*nfs2_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern void		pipe_kfile_close (KFILE *);
extern void		pipe_open (INODE *, int);
extern void		pipe_close (INODE *);
extern INODE		*pipe_alloc_inode (void);
extern int		pipe_read_inode (INODE *);
extern void		pipe_write_inode (INODE *);
extern void		pipe_free_inode (const INODE *ip);
extern void		pipe_trunc_inode (INODE *ip);
extern dev_t		pseudo_alloc_dev (INODE *ip);
extern dev_t		pseudo_search_dev (INODE *ip);
extern int		pseudo_free_dev (dev_t);
extern daddr_t		ext2_block_map (INODE *, daddr_t, int, daddr_t *);
extern int		ext2_read_inode (INODE *);
extern void		ext2_write_inode (INODE *);
extern INODE		*ext2_make_inode (IOREQ *, DIRENT *, int);
extern void		ext2_read_dir (IOREQ *);
extern void		ext2_write_dir (IOREQ *, const DIRENT *, const INODE *, int);
extern void		ext2_read (IOREQ *);
extern void		ext2_write (IOREQ *);
extern void		ext2_open (INODE *, int);
extern void		ext2_close (INODE *);
extern int		ext2_read_symlink (IOREQ *iop);
extern INODE		*ext2_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern void		ext2_trunc_inode (INODE *);
extern INODE		*ext2_make_dir (IOREQ *iop, DIRENT *dep, int mode);
extern int		ext2_empty_dir (INODE *);
extern void		ext2_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern void		ext2_free_inode (const INODE *);
extern void		t1_open (INODE *, int);
extern void		t1_close (INODE *);
extern int		t1_read_inode (INODE *);
extern void		t1_write_inode (INODE *);
extern INODE		*t1_make_inode (IOREQ *, DIRENT *, int);
extern INODE		*t1_alloc_inode (SB *, daddr_t);
extern void		t1_trunc_inode (INODE *);
extern void		t1_read (IOREQ *);
extern void		t1_write (IOREQ *);
extern void		t1_read_dir (IOREQ *);
extern void		t1_write_dir (IOREQ *, const DIRENT *, const INODE *, int);
extern INODE		*t1_make_dir (IOREQ *iop, DIRENT *dep, int mode);
extern void		t1_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern int		t1_empty_dir (INODE *);
extern void		t1_free_inode (const INODE *);
extern daddr_t		t1_block_map (INODE *, daddr_t, int, daddr_t *);
extern int		t1_read_symlink (IOREQ *iop);
extern INODE		*t1_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern INODE		*v7_alloc_inode (dev_t);
extern int		v7_read_inode (INODE *);
extern void		v7_write_inode (INODE *);
extern INODE		*v7_make_inode (IOREQ *, DIRENT *, int);
extern void		v7_trunc_inode (INODE *);
extern int		v7_empty_dir (INODE *);
extern void		v7_free_inode (const INODE *);
extern INODE		*fat_make_inode (IOREQ *, DIRENT *, int);
extern INODE		*fat_make_dir (IOREQ *iop, DIRENT *dep, int mode);
extern void		fat_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern void		fat_trunc_inode (INODE *);
extern void		fat_free_inode (const INODE *);
extern void		fat_truncate (INODE *ip);
extern int		fat_empty_dir (INODE *);
extern void		fat_get_nlink_dir (INODE *ip);
extern INODE		*fat_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern INODE		*v7_make_dir (IOREQ *iop, DIRENT *dep, int mode);
extern void		v7_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern int		cd_read_inode (INODE *);
extern void		cd_write_inode (INODE *);
extern int		cd_search_dir (IOREQ *, DIRENT *, int);
extern void		cd_read_dir (IOREQ *);
extern void		cd_read (IOREQ *);
extern int		cd_read_symlink (IOREQ *);
extern void		itrunc (INODE *);
extern void		idec (INODE *);
extern INODE		*iget (dev_t, ino_t, int);
extern void		iinc (INODE *);
extern void		iput (INODE *);
extern void		blk_open (INODE *, int);
extern void		blk_close (INODE *);
extern void		blk_read (IOREQ *);
extern void		blk_write (IOREQ *);
extern void		chr_open (INODE *, int);
extern void		chr_close (INODE *);
extern void		chr_read (IOREQ *);
extern void		chr_write (IOREQ *);

extern int		nt_read_inode (INODE *);
extern void		nt_write_inode (INODE *);
extern void		nt_trunc_inode (INODE *);
extern void		nt_free_inode (const INODE *);
extern void		nt_read_dir (IOREQ *);
extern void		nt_open (INODE *, int);
extern void		nt_close (INODE *);
extern void		nt_read (IOREQ *);
extern void		nt_write (IOREQ *);
extern void		ntfs_mount_epilogue (SB *);

extern void		ntfs_free_all_ntvattr (INODE *ip);
extern int		ntfs_bread (IOREQ *iop);

extern int		fs_inval_authen (dev_t dev);
extern int		fs_inval_mount (SB *unisp);
extern void		fs_inval_ustat (const SB *unisp, USTAT *up);
extern void		fs_inval_update (const SB *unisp);
extern int		fs_inval_read_inode (INODE *ip);
extern void		fs_inval_write_inode (INODE *ip);
extern INODE		*fs_inval_make_inode (IOREQ *iop, DIRENT *dep, int mode);
extern void		fs_inval_rel_inode_attr (INODE *ip);
extern void		fs_inval_trunc_inode (INODE *ip);
extern void		fs_inval_free_inode (const INODE *ip);
extern int		fs_inval_search_dir (IOREQ *iop, DIRENT *dep, int);
extern void		fs_inval_read_dir (IOREQ *iop);
extern void		fs_inval_write_dir (IOREQ *iop, const DIRENT *dep, const INODE *, int);
extern int		fs_inval_empty_dir (INODE *ip);
extern INODE		*fs_inval_make_dir (IOREQ *iop, DIRENT *dep, int);
extern void		fs_inval_clr_dir (IOREQ *iop, const DIRENT *dep, const INODE *);
extern void		fs_inval_open (INODE *ip, int oflag);
extern void		fs_inval_close (INODE *ip);
extern void		fs_inval_read (IOREQ *iop);
extern void		fs_inval_write (IOREQ *iop);
extern int		fs_inval_read_symlink (IOREQ *iop);
extern INODE		*fs_inval_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern int		fs_inval_ioctl (dev_t dev, int cmd, int arg, int flags);

extern void		v7_open (INODE *, int);
extern void		v7_close (INODE *);
extern void		v7_read (IOREQ *);
extern void		v7_write (IOREQ *);
extern void		v7_read_dir (IOREQ *);
extern void		v7_write_dir (IOREQ *, const DIRENT *, const INODE *, int);
extern int		v7_read_symlink (IOREQ *);
extern INODE		*v7_write_symlink (const char *, int, IOREQ *, DIRENT *);
extern daddr_t		v7_block_map (INODE *, daddr_t, int, daddr_t *);
extern void		fat_creat_clusvec (INODE *);
extern void		fat_free_clusvec (INODE *);
extern int		fat_cluster_read_map (INODE *, clusno_t);
extern int		fat_cluster_write_map (INODE *, clusno_t);
extern int		fat_read_inode (INODE *);
extern void		fat_write_inode (INODE *);
extern void		fat_read (IOREQ *);
extern void		fat_write (IOREQ *);
extern void		fat_read_dir (IOREQ *);
extern void		fat_write_dir (IOREQ *, const DIRENT *, const INODE *, int);
extern int		b_map_analysis (INODE *, daddr_t, daddr_t, daddr_t *);
extern void		fopen (INODE *, int, int);
extern int		kaccess (INODE *, long);
extern INODE		*owner (const char *);
extern int		xalloc (INODE *);
extern int		xrelease (INODE *);
extern int		xuntext (INODE *, int);
#endif	INODE_H

#if	defined (INODE_H) && defined (DIRENT_H)
extern INODE		*iname (const char *, int (*) (const char *), int, IOREQ *, DIRENT *);
extern INODE		*imake (IOREQ *, DIRENT *, int);
extern int		ext2_search_dir (IOREQ *, DIRENT *, int);
extern int		t1_search_dir (IOREQ *, DIRENT *, int);
extern int		v7_search_dir (IOREQ *, DIRENT *, int);
extern int		fat_search_dir (IOREQ *, DIRENT *, int);
extern ino_t		fat_lfn_dir_put (INODE *ip, const DIRENT *dep);
extern int		nt_search_dir (IOREQ *, DIRENT *, int);
extern int		nfs2_search_dir (IOREQ *, DIRENT *, int);
#endif	INODE_H & DIRENT_H

#if	defined (ITNET_H)
extern void		raw_ep_free_init (void);
extern RAW_EP		*get_raw_ep (void);
extern void		put_raw_ep (RAW_EP *);
extern void		delete_raw_queues (RAW_EP *);

extern void		udp_ep_free_init (void);
extern UDP_EP		*get_udp_ep (void);
extern void		put_udp_ep (UDP_EP *);
extern void		delete_udp_queues (UDP_EP *);

extern void		tcp_ep_free_init (void);
extern TCP_EP		*get_tcp_ep (void);
extern void		put_tcp_ep (TCP_EP *);
extern void		delete_tcp_queues (TCP_EP *);
extern int		delete_retrans_queue (TCP_EP *);

extern void		init_it_block (void);
extern ITBLOCK		*get_it_block (ITBLENUM);
extern void		put_it_block (ITBLOCK *);
extern void		put_all_it_block (void);

extern int		compute_checksum (void *, int);
extern int		incremental_checksum (int, int, int);
extern void		pr_itn_addr (IPADDR);
extern const char	*edit_ipaddr (IPADDR addr);

extern void		wake_up_in_daemon (IN_ENUM, void *);
extern void		wake_up_nfs_daemon (ITBLOCK *);
extern void		execute_in_daemon (void *);
extern void		execute_out_daemon (void);
extern void		execute_nfs_daemon (void);

extern void 		send_tcp_ctl_packet (int, TCP_EP *);
extern void		send_tcp_rst_packet (int, IPADDR, int, IPADDR, int, ulong, ulong);
extern void		queue_tcp_data_packet (IOREQ *, int, TCP_EP *);
extern void		pipe_tcp_data_packet (IOREQ *, int, TCP_EP *);
extern void 		send_tcp_data_packet (TCP_EP *, seq_t, int, int);
extern void		receive_tcp_packet (ITBLOCK *, void *, int);
extern void		remove_acked_segments (TCP_EP *, long);
extern void		insert_packet_in_ep_inq (TCP_EP *, ITBLOCK *);
extern ITBLOCK		*wait_tcp_segment (TCP_EP *, int, ITBLOCK **);

extern void		send_raw_datagram (RAW_EP *, ITBLOCK *);
extern int		receive_raw_datagram (ITBLOCK *, int, void *, int);

extern void		send_udp_datagram (UDP_EP *, ITBLOCK *);
extern void		receive_udp_datagram (ITBLOCK *, void *, int);

extern void		send_icmp_error_message (IPADDR, int, int, void *, int);
extern void		send_icmp_datagram (ITBLOCK *);
extern void		receive_icmp_datagram (ITBLOCK *, void *, int);

extern void		send_ip_datagram (PROTO_ENUM, ITBLOCK *);
extern void		receive_ip_datagram (ITBLOCK *);
extern void		receive_ppp_packet (ITBLOCK *);
extern ROUTE		*get_route_entry (IPADDR);
extern int		add_route_entry (IPADDR, const char *, INODE *);
extern int		del_route_entry (IPADDR);

extern void		send_frame (ITBLOCK *);
extern void		route_frame (ITBLOCK *);
extern void		receive_frame (ITBLOCK *);

extern int		ether_get_ether_addr (IPADDR, ETHADDR *);
extern void		ether_put_ether_addr (IPADDR, const ETHADDR *, int);
extern void		ether_receive_frame (ITBLOCK *);
extern void		ether_receive_arp_frame (ITBLOCK *);
extern int		ether_send_arp_request (ITBLOCK *, IPADDR);
extern void		ether_pr_ether_addr (const ETHADDR *);
extern const char	*ether_edit_ether_addr (const ETHADDR *);
extern void		dhcp_daemon (ROUTE *rp);
extern void		dhcp_wake_up (ITBLOCK *bp);

extern int		circular_area_alloc (TCP_EP *);
extern void		circular_area_mrelease (TCP_EP *);
extern int		circular_area_put (RND *, const char *, int, int);
extern int		circular_area_get (RND *, char *, int);
extern int		circular_area_read (RND *, char *, int, int);
extern int		circular_area_del (RND *, int);

extern const char	*get_state_nm (S_STATE);
extern void		do_eventdone_on_inqueue (TCP_EP *);
extern void		reset_the_connection (TCP_EP *, int);

extern int		k_addr_to_node (IPADDR, char *, int);
#endif	ITNET_H

#ifdef	XTI_H
extern void		k_accept (int, const T_CALL *);
extern void		k_bind (int, const T_BIND *, T_BIND *);
extern void		k_connect (const T_CALL *, T_CALL *);
extern void		k_getinfo (int, T_INFO *);
extern int		k_getstate (int);
extern void 		k_listen (T_CALL *);
extern int		k_look (int);
extern void		k_optmgmt (int, const T_OPTMGMT *, T_OPTMGMT *);
extern void		k_rcv (IOREQ *, int *);
extern void 		k_rcvconnect (T_CALL *);
extern void		k_rcvdis (T_DISCON *);
extern void		k_rcvrel (void);
extern void		k_rcvudata (IOREQ *, INADDR *, int, int *);
extern void		k_rcvuderr (T_UDERROR *);
extern void		k_snd (IOREQ *, int);
extern void		k_snddis (T_CALL *);
extern void		k_sndrel (IOREQ *);
extern void		k_sndudata (IOREQ *, INADDR *, int);
extern int		k_sync (int);
extern void		k_unbind (int);

extern void		k_push (IOREQ *);
extern int		k_nread (int);
extern int		k_strevent (int, char *);
extern void		k_getaddr (int, INADDR *);
extern void		k_rcvraw (IOREQ *, INADDR *, int, int *);
extern void		k_sndraw (IOREQ *, INADDR *, int);

extern int		k_attention (int, int, char **);
extern int		k_unattention (int);

extern int		k_mail_to_node (DNS *);
extern int		k_dns_wait_req (void *);
extern int		k_dns_put_info (void *);
#endif	XTI_H

extern int		k_node_to_addr (void *);

#ifdef	IPC_H
extern int		ueventalloc (KFILE *, int);
extern int		ueventget (const KFILE *, int);
extern UEVENTG		*ueventgaddr (int);
extern void		ueventlfcrelease (KFILE *);
extern void		ueventgicrelease (INODE *);
extern void		ueventgput (UEVENTG *);
extern void		ueventginit (void);
extern int		usemaalloc (KFILE *, int);
extern int		usemaget (const KFILE *, int, int);
extern USEMAG		*usemagaddr (int);
extern void		usemalfcrelease (KFILE *);
extern void		usemagicrelease (INODE *);
extern void		usemagput (USEMAG *);
extern void		usemaginit (void);
extern int		fregionalloc (KFILE *, int);
extern int		fregionget (KFILE *, int, int);
extern void		shmem_release (KFILE *);
extern void		fregiongicrelease (INODE *);
extern pg_t		shmem_vaddr_alloc (pg_t);
extern void		shmem_vaddr_release (pg_t, pg_t);
#endif	IPC_H

#ifdef	KFILE_H
extern void		fclose (KFILE *);
extern KFILE		*fget (int);
extern KFILE		*feget (void);
extern void		fclear (KFILE *);
extern void		pfifo (KFILE *);
extern void		pfifowait (KFILE *);
extern void		pipe_read (IOREQ *);
extern void		pipe_write (IOREQ *);
#endif	KFILE_H

#ifdef	LOCKF_H
extern int		lfdeadlock (LOCKF *);
extern LOCKF		*lfget (void);
extern void		lfput (LOCKF *);
#endif	LOCKF_H

#ifdef	MAP_H
extern pg_t		malloc (int, pg_t);
extern pg_t		malloce (int, pg_t);
extern pg_t		mallocp (int, pg_t, pg_t);
extern pg_t		malloct (int, pg_t);
extern pg_t		malloc_dma_24 (pg_t *);
extern void		mrelease_all (int);
extern void		mrelease (int, pg_t, pg_t);
#endif	MAP_H

#ifdef	MMU_H
extern mmu_t		*vaddr_to_page_table_entry (pg_t, int);
#endif	MMU_H

#ifdef	UPROC_H
extern void		insert_proc_in_corerdylist (UPROC *);
extern void		insswaprdyl (UPROC *);
extern void		nfs_retransmit_client_datagram (UPROC *up);
extern void		remove_proc_from_corerdylist (UPROC *);
extern int		signoget (UPROC *);
extern void		sigproc (UPROC *, int);
extern int		swapin (UPROC *);
extern void		swapout (UPROC *, int, pg_t, pg_t);
extern int		xswget (UPROC *);
extern UPROC		*zkill (int, int);
#endif	UPROC_H

#ifdef	REGION_H
extern REGIONL		*getprocregionl (int, pg_t, int);
extern REGIONL		*getprocregion (int, pg_t, pg_t);
extern REGIONL		*regionlalloc (int);
extern REGIONG		*regiongget (void);
extern void		regionlrelease (int, REGIONL *);
extern void		regionrelease (REGIONL *);
extern int		regiongrelease (REGIONG *, int);
extern void		regiongput (REGIONG *);
extern void		regionginit (void);
extern int		regiongrow (REGIONL *, pg_t, pg_t, int);
extern int		regiondup (const REGIONL *, REGIONL *, int);
extern void		regionclr (REGIONG *);
extern REGIONG		*regionloadfromcore (REGIONL *, void *, int);
extern REGIONG		*regionloadfrominode (REGIONL *, INODE *, int, int);
extern REGIONG		*regionloadfromswap (REGIONL *, daddr_t, int);
extern REGIONG		*regionstoretoswap (REGIONL *, daddr_t, int);
extern REGIONL		*growprocregion (int, pg_t);
extern REGIONL		*dupprocregion (int, REGIONL *, int);
extern int		mmu_page_table_alloc (REGIONL *, pg_t, pg_t);
extern void		mmu_page_table_release (REGIONG *);
extern void		mmu_dir_load (const REGIONL *);
extern void		mmu_dir_unload (const REGIONL *);
#endif	REGION_H

#ifdef	SB_H
extern dev_t		mdevget (char *);
extern SB		*sbget (dev_t);
extern SB		*sbiget (INODE *);
extern void		v7_sb_endian_conversion (V7SB *);
#endif	SB_H

#ifdef	SCSI_H
extern int		disktb_open_entries (dev_t, const SCSI *);
extern int		scsi_open (SCSI *, dev_t, int);
extern void		scsi_close (SCSI *, dev_t);
extern int		scsi_sense (SCSI *, int);
extern int		scsi_ctl (SCSI *, dev_t, int, int);
extern char		*scsi_cmd_name (int);
#endif	SCSI_H

#ifdef	SYNC_H
extern int		tas (LOCK *);

extern void		spinlock (LOCK *);
extern void		spinfree (LOCK *);

extern void		sleeplock (LOCK *, int);
extern void		sleepfree (LOCK *);
extern int		sleeptest (LOCK *);

extern void		eventwait (EVENT *, int);
extern void		eventcountwait (int, long *, EVENT *, int);
extern void		eventdone (EVENT *);
extern int		eventtest (EVENT *);

extern void		semalock (SEMA *, int);
extern void		semafree (SEMA *);
extern int		sematest (SEMA *);
extern int		semainit (SEMA *, int);

extern void		syncset (int, int, void *);
extern void		syncgot (int, void *);
extern void		syncclr (int, void *);
extern int		synccheck (void);
extern void		syncfree (void);
#ifdef	UPROC_H
extern void		syncundo (UPROC *);
extern void		syncwakeup (UPROC *, PHASHTB *);
#endif	UPROC_H
extern char		*syncnmget (int);
#endif	SYNC_H

#ifdef	TIMEOUT_H
extern TIMEOUT		*toutset (int, void (*) (), int);
extern void		toutreset (TIMEOUT *, void (*) (), int);
#endif	TIMEOUT_H

#ifdef	TTY_H
extern int		cget (CHEAD *);
extern int		chop (CHEAD *);
extern int		cread (CHEAD *, void *, int, int);
extern int		cput (int, CHEAD *);
extern int		cwrite (CHEAD *, void *, int, int);
extern void		putcvtb (TTY *);
extern void		ttyin (int, TTY *);
extern void		ttychars (TTY *);
extern void		ttyclose (TTY *);
extern int		ttyctl (TTY *, int, void *);
extern void		ttyiflush (TTY *);
extern int		ttylarg (int, TTY *);
extern void		ttyoflush (TTY *);
extern void		ttyopen (dev_t, TTY *, int);
extern void		ttyout (int, TTY *);
extern void		ttyread (IOREQ *, TTY *);
extern int		ttyretype (TTY *, int);
extern void		ttywipe (int, TTY *);
extern void		ttywrite (IOREQ *, TTY *);
#endif	TTY_H

#ifdef	NT_H
extern struct ntvattr	*ntfs_ntvattrget (const NTSB *, INODE *ip, int type, const char *nm, clusno_t vcn, int);
extern int		ntfs_readattr (const NTSB *ntsp, int type, const char *nm, IOREQ *iop);
extern int		ntfs_procfixups (const NTSB *ntsp, ulong magic, void *area, int count);
extern struct ntvattr	*ntfs_attrtontvattr (const NTSB *ntsp, const struct attr *rap);
extern const char	*ntfs_edit_attrib_type (int type);
extern int		ntfs_readntvattr_plain (const NTSB *ntsp, const struct ntvattr *vap, IOREQ *iop);
extern void		nt_get_nlink_dir (INODE *ip);
#endif	NT_H
