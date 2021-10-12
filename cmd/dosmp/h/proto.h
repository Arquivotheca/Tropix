/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Protótipos das funções usadas				*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.8.0, de 07.07.05				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
extern void		alloc_fat (void);
extern int		alloc_file_clusters (int);
extern void		*bread (int, void *);
extern const void	*bwrite (int, const void *);
extern int		clr_dir_by_clus (char *, int, int *, int *);
extern int		clus_alloc_write (int, void *, int, int);
extern void		clus_read (int, void *, int);
extern void		clus_write (int, const void *, int);
extern void		cluster_block_clr (int);
extern int		cmd_getopt (const char *[], const char *);
extern char		*cv_blocks_to_KB_MB_or_GB (long);
extern char		*cv_blocks_to_MB (long);
extern void		do_arena (int, const char *[]);
extern void		do_bin (int, const char *[]);
extern void		do_cat (int, const char *[]);
extern void		do_cd (int, const char *[]);
extern void		do_chmod (int, const char *[]);
extern void		do_chsize (int, const char *[]);
extern void		do_clri (int, const char *[]);
extern void		do_cmp (int, const char *[]);
extern void		do_compac (int, const char *[]);
extern void		do_df (int, const char *[]);
extern void		do_diff (int, const char *[]);
extern void		do_du (int, const char *[]);
extern void		do_echo (int, const char *[]);
extern void		do_edbad (int, const char *[]);
extern void		do_fsck (int, const char *[]); 
extern void		do_get (int, const char *[]);
extern void		do_gettree (int, const char *[]);
extern void		do_iso (int, const char *[]);
extern void		do_lc (int, const char *[]);
extern void		do_lfn (int, const char *[]);
extern void		do_ls (int, const char *[]);
extern void		do_mkdir (int, const char *[]);
extern void		do_mkfile (int, const char *[]);
extern void		do_mkfs (int, const char *[]); 
extern void		do_mkv (int, const char *[]);
extern void		do_mount (int, const char *[]); 
extern void		do_mv (int, const char *[]);
extern void		do_mvtree (int, const char *[]);
extern void		do_param (int, const char *[]);
extern void		do_put (int, const char *[]);
extern void		do_puttree (int, const char *[]);
extern void		do_pwd (int, const char *[]);
extern void		do_quit (int, const char *[]);
extern void		do_rm (int, const char *[]);
extern void		do_rmdir (int, const char *[]);
extern void		do_rmtree (int, const char *[]);
extern void		do_rmv (int, const char *[]);
extern void		do_ro (int, const char *[]);
extern void		do_rw (int, const char *[]);
extern void		do_show (int, const char *[]);
extern void		do_stat (int, const char *[]);
extern void		do_sync (int, const char *[]);
extern void		do_sys (int, const char *[]);
extern void		do_touch (int, const char *[]);
extern void		do_umount (int, const char *[]); 
extern void		do_walk (int, const char *[]);
extern void		do_waste (int, const char *[]);
extern void		do_xd (int, const char *[]);
extern int		dos_mkdir (const char *);
extern void 		dos_nm_expand (void (*f) (int, const char *[]), const char *[]);
extern int		dos_rmdir (const char *);
extern const char	*editpercent (int);
extern char		*env_var_scan (char *, char *);
extern void		error (const char *, ...);
extern void		exec_cmd (const char *);
extern void		exec_local_cmd (char *);
extern void 		expand_arg_free (void);
extern int		file_creat_prolog (char **, int *, int *);
extern int		file_color_convert (int);
extern char		*file_mode_edit (int);
extern int		file_rm_prolog (char *, int *, int *);
extern int		file_type_edit (int);
extern int		fsck_cycle_check (int);
extern int		get_fat_value (int);
extern int		get_new_cluster (int);
extern int		get_uni_block (void);
extern int		getpercent (long, long);
extern void		get_whole_fat (void);
extern void		help (void);
extern void		mk_one_vol (const char *path);
extern const char	*last_nm (const char *);
extern char		*path_can (char *);
extern int		pattern_accept (const char *);
extern const char	*plural (int);
extern void		print_time (time_t);
extern void		put_cluster_list (int);
extern void		put_fat_value (int, int);
extern int		put_one_file (const char *, int);
extern int		put_p_option (const char *, const char *[], int *);
extern void		put_s_option (void);
extern void		put_str_with_blanks (const char *, char *, int);
extern void		put_whole_fat (void);
extern void		quit (int);
extern void		simple_mount (void);
extern int		simple_open (const char *, int);
extern void 		sofix_nm_expand (void (*f) (int, const char *[]), const char *[]);
extern char		*strlwr (char *);
extern long		ls_get_cluster_list_size (int);
extern void 		update_fat32_info (void);
