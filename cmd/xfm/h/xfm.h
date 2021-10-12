/*
 ****************************************************************
 *								*
 *			xfm.h					*
 *								*
 *	Gerenciador de arquivos					*
 *								*
 *	Versão	4.5.0, de 24.06.03				*
 *		4.8.0, de 13.04.05				*
 *								*
 *	Módulo: xfm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definições globais **************************************
 */
#define	NOVOID		(void *)NULL
#define	elif		else if
#define	EVER		;;
#define STR(x)		# x
#define XSTR(x)		STR (x)
#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

/*
 ****** Tamanhos das margens, sombras, ... **********************
 */
enum {	BORDER = 1, SHADOW = 3 };

enum {	MIN_DIR_LINE_Y = 24 };

#define	NAME_OFFSET		(3 * wp->font_width)

#define	MIN_TREE_WIDTH		(25 * wp->font_width)
#define	MIN_DIR_WIDTH		(75 * wp->font_width)

enum {	GAP_WIDTH = 12 };
enum {	MENU_BORDER = 10 };

enum {	SCROLL_WIDTH = 15 };
#define	TOTAL_SCROLL_WIDTH	(SCROLL_WIDTH + 4 * SHADOW)

enum {	ICON_HEIGHT = 16 };

enum {	BUTTON_WIDTH = 16, BUTTON_HEIGHT = 13 };

enum {	PERCENT_BORDER = MENU_BORDER, PERCENT_HEIGHT = 16 };

/*
 ****** Quasi-constantes dos tamanhos, ... **********************
 */
extern int	LINE_HEIGHT;	/* Altura total da linha */

extern int	TEXT_OFFSET;	/* Deslocamento do texto */

extern int	ICON_OFFSET;	/* Deslocamento do ícone */

extern int	BUTTON_OFFSET;	/* Deslocamento do botão */

extern int	TREE_OFFSET;	/* Deslocamento do texto do "tree" */

/*
 ******	Valores obtidos das cores *******************************
 */
typedef	struct
{
	Pixel		main_pixel, top_pixel,  bottom_pixel;
	GC		main_gc,    top_gc,	bottom_gc;

}	TRIPLEGC;

/*
 ******	Estrura de uma janela ***********************************
 */
typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	triple;
	GC		text_gc;
	int		font_width, font_height, font_ascent, font_descent;

}	WINDATA;

#define	NOWINDATA 	(WINDATA *)NULL

/*
 ******	Estrutura de um SUBMENU *********************************
 */
typedef struct
{
	const char		*m_text;	/* Texto a escrever */
	int			m_value;	/* Arquivos ocultos */

}	SUBMENU_VEC;

#define NOSUBMENU	(SUBMENU *)NULL

typedef struct
{
	WINDATA			windata;
	const SUBMENU_VEC	*vec;

	int			mapped;
	int			line_offset;
	int			longest_text_len;
	int			text_n;
	int			*value_in_use;
	int			highlight_line;

}	SUBMENU;

extern SUBMENU		zip;

/*
 ******	Estrura de uma janela (barra de rolagem) ****************
 */
typedef struct
{
	Window		win;
	int		width, height;
	int		x, y;
	TRIPLEGC	triple;		/* As tres cores */
	int		button_no;	/* No. do botão apertado */
	int		begin, end;	/* Começo e Final da janela */
	int		top, bottom;	/* Limites da barra (top: valor menor) */
	int		last_offset;	/* Último valor em cima da barra */

}	SCROLLDATA;

/*
 ******	Estrutura FSTAB *****************************************
 */
typedef struct fstab	FSTAB;

struct fstab
{
	char		fs_dev_nm[64];
	char		fs_dir_nm[64];
	int		fs_flags;
	FSTAB		*fs_next;
};

#define	NOFSTAB	(FSTAB *)NULL

/*
 ****** Estrutura de um nó da árvore ****************************
 */
typedef struct node	NODE;

struct node
{
	char		*n_path;	/* Nome do caminho completo */
	int		n_path_len;	/* Comprimento do caminho completo */
	char		*n_nm;		/* Ponteiro para o último nome */
	int		n_nm_len;	/* Comprimento do último nome */
	STAT		n_ls;		/* Estado (obtido por "lstat") */
	STAT		n_s;		/* Estado (real) */
	NODE		*n_forw;	/* PrÓximo (Irmão) */
	NODE		*n_back;	/* Anterior (Irmão) */
	NODE		*n_parent;	/* Ponteiro para o pai */
	NODE		*n_children;	/* Lista de filhos */
	int		n_child_cnt;	/* No. de filhos (dir. ou não) */
	int		n_line_size;	/* No. de linhas ocupadas (com descendentes) */
	XImage		*n_ximage;	/* O Ícone */
	int		n_line_first;	/* Primeiro filho visível do DIRETÓRIO */
	int		n_sb_flags;	/* Indicadores de montagem */

	char		n_expanded;	/* Indica se está expandido */
	char		n_exec;		/* < 0: Não executável; 0: Não sabe; > 0: Executável */
	char		n_selected;	/* Indica se está selecionado para mover/copiar */
	char		n_mnt_point;	/* Indica se este diretório está em "fstab" */
};

#define	NONODE	(NODE *)NULL

/*
 ******	Estrutura para a leitura do teclado *********************
 */
typedef struct
{
	char		k_code;
	char		k_changed;
	Display		*k_display;
	Window		k_win;
	GC		k_gc, k_rev_gc;
	int		k_font_width;
	char		*k_area;
	int		k_count;
	int		k_offset;
	int		k_size;
	int		k_x, k_y;
	void		(*k_function) (void);

}	KEYDATA;

extern KEYDATA		keydata;

/*
 ****** Estrutura da mensagem ***********************************
 */
typedef		void (*MSGFUNC) (int);

#define	NOMSGFUNC	((MSGFUNC)NULL)

/*
 ******	Estruturas do FILE MENU *********************************
 */
typedef struct
{
	const char	*e_cmd_nm;
	const char	*e_exec_text;
	void		(*e_func) (void);
	int		(*e_permission) (const NODE *);

}	FILE_MENU_VEC;

typedef struct
{
	WINDATA			*windata;
	NODE			*menu_node;
	const FILE_MENU_VEC	*menu_vec;
	int			menu_text_n;
	int			menu_highlight_line;

}	MENUDATA;

extern MENUDATA		menudata[2];

/*
 ****** Diversos ENUMs ******************************************
 */
enum { TREE_MENU, DIR_MENU };

enum { GOOD_MSG, ERROR_MSG, QUESTION_MSG };

enum { PATTERN_ALL, PATTERN_SOURCE };

enum { DOT_FLAG_OFF, DOT_FLAG_ON };

enum { WIN_EXTENSION_OFF, WIN_EXTENSION_ON };

enum { VOLUME_OFF, VOLUME_ON };

enum { GROUP_OFF, GROUP_ON };

enum { MODE_9, MODE_13 };

enum { EXEC_UNKNOWN, EXEC_ERROR, EXEC_IS_EXEC, EXEC_NO_EXEC };

enum { ZIP_RW, ZIP_RO, ZIP_RO_PASS, ZIP_RW_Q };

/*
 ****** Variáveis globais ***************************************
 */
extern int		real_uid;	/* O usuário REAL */

extern const char	fstab_nm[];

extern Display		*display;

extern Window		root_win;

extern int		present_root_width, present_root_height;

extern WINDATA		tree, tree_menu, msg, msg_yes, msg_no, percent;

extern WINDATA		dir, name, dir_menu, nm_edit, df_data, du_data;

extern SCROLLDATA	tree_scroll, dir_scroll;

extern TRIPLEGC		full_dir_triple, empty_dir_triple;

extern TRIPLEGC		fore_triple, back_triple, black_triple;

extern GC		reverse_text_gc;

extern GC		percent_gc;

extern GC		absent_text_gc;

extern GC		mode_gc_vector[16];	/* Cores dos modos dos arquivos */

extern char		background_line[32];

extern int		tree_total_lines;	/* Total de linhas da janela da árvore */

extern int		tree_first_line;	/* No. da primeira linha da janela da árvore */

extern int		dir_line_vis;		/* No. de linhas visíveis na janela */
extern int		dir_line_first;		/* No. da primeira linha visível */

extern int		tree_line_vis;		/* No. de linhas visíveis na janela da árvore */

extern NODE		*root_node;		/* Nó da RAIZ da árvore */

extern NODE		*dir_win_first_son;	/* Primeiro filho visível do DIRETÓRIO */

extern NODE		*dir_win_node;		/* Diretório da tela do DIRETÓRIO */

extern MSGFUNC		msg_func;

extern const NODE	*msg_rm_node;		/* Arquivos a remover */

extern int		present_menu;		/* TREE_MENU ou DIR_MENU, onde clicou */

extern int		selected_file_cnt,	/* Contadores de arquivos selecionados */
			selected_dir_cnt;

extern NODE		*select_dir_node;	/* O diretório selecionado */

extern NODE		*select_file_node;	/* O arquivo selecionado (se for apenas 1) */

extern int		hidden_in_use;		/* Ignora/processa os ocultos */
extern int		volume_in_use;		/* Volume em uso */
extern int		pattern_in_use;		/* Padrões em uso */
extern int		extension_in_use;	/* Estende ou não a janela de DIR */
extern int		group_in_use,		/* Inclui ou não o grupo dos arquivos */
			last_group_in_use;
extern int		mode_in_use,		/* Modo dos arquivos */
			last_mode_in_use;

extern char		edit_area[256];		/* Nome para editar */

extern int		edit_len;

extern NODE		*cmp_next_src_np;

/*
 ******	Os Ícones ***********************************************
 */
extern XImage		*small_dir_ximage;
extern XImage		*big_dir_ximage;
extern XImage		*small_mount_ximage;
extern XImage		*small_disk_ximage;
extern XImage		*big_disk_ximage;
extern XImage		*small_executable_ximage;
extern XImage		*big_executable_ximage;
extern XImage		*small_text_ximage;
extern XImage		*big_text_ximage;
extern XImage		*big_script_ximage;
extern XImage		*small_symlink_ximage;
extern XImage		*small_exclam_ximage;

/*
 ******	Protótipos de funções ***********************************
 */
extern void		create_root_tree (void);
extern void		draw_tree_win (void);
extern void		button_press_tree_window (int button, int x, int y);
extern void		button_3_press_tree_window (int x, int y, NODE *);
extern void		draw_shadow (Window, GC, GC, int, int, int width, int height);
extern void		draw_only_right_shadow (Window, GC, int, int, int width, int height);
extern void		convert_xpm_images (void);
extern void		draw_dir_win (void);
extern void		draw_name_win (void);
extern void		calculate_today_time (void);
extern void		draw_scroll_button (const SCROLLDATA *, int up_down, int state);
extern void		update_tree_scroll_bar (int motion, int y);
extern void		update_dir_scroll_bar (int motion, int y);
extern void		button_release_scroll_button (SCROLLDATA *);
extern void		draw_scroll_win (const SCROLLDATA *);
extern void		button_press_dir_window (int, int state, int, int);
extern void		draw_tree_or_dir_menu_win (int);
extern void		button_press_tree_or_dir_menu_window (int MENU);
extern void		leave_tree_or_dir_menu_window (int MENU);
extern void		motion_tree_or_dir_menu_window (int MENU, int y);
extern void		check_if_file_is_executable (NODE *np);
extern void		get_file_icon (NODE *np);
extern void		get_max_colsz (void);
extern void		draw_nm_edit_win (void);
extern void		recreate_root_tree (int);
extern void		do_key_press (XEvent event);
extern NODE		*create_new_node (NODE *pp, const char *dname, int d_namlen);
extern void		draw_msg_text (const char *format, ...);
extern void		draw_error_msg_text (const char *format, ...);
extern void		draw_error_msg_win (void);
extern void		draw_question_msg_text (int rm, const char *question, const char *format, ...);
extern void		draw_msg_win (void);
extern void		draw_percent_win (void);
extern void		clear_msg_win (void);
extern void		create_child_dir_list (NODE *np);
extern void		insert_node_lexicografically (NODE *pp, NODE *np);
extern void		select_all (int);
extern void		disselect_all (int);
extern void		button_release_dir_window (int y);
extern void		free_node (NODE *pp);
extern void		compare_root_tree (int);
extern void		draw_button (const WINDATA *, const TRIPLEGC *, int, int, int, int);
extern void		do_msg_button_press (int value);
extern void		draw_msg_yes_or_no_win (const WINDATA *wp);
extern NODE		*find_node_in_children_list (NODE *pp, const char *nm);
extern const FSTAB	*search_dir_in_fstab (const char *dir_nm);
extern int		read_fstab (void);
extern void		print_df_entry (void);
extern void		draw_df_data_window (void);
extern void		draw_du_data_window (void);
extern const char	*edit_sz_value (unsigned long value, char *area);
extern void		scroll_tree_window (int diff);
extern void		scroll_dir_window (int diff);
extern void		set_window_sizes (int width, int height, int tree_width_inc);
extern void		event_loop (int main_event_loop);
extern int		cancel_event_pending (void);
extern void		create_SB_list (void);
extern int		get_mount_flags (dev_t dev);
#ifdef	SB_H
extern const SB *	get_super_block (dev_t dev);
#endif	SB_H

extern int		permission_always (const NODE *np);
extern int		permission_mount (const NODE *np);
extern int		permission_copy (const NODE *np);
extern int		permission_mv (const NODE *np);
extern int		permission_rm (const NODE *np);
extern int		permission_rename (const NODE *np);
extern int		permission_sh (const NODE *np);
extern int		permission_edit (const NODE *np);
extern int		permission_mkdir (const NODE *np);
extern int		permission_du (const NODE *np);
extern int		permission_zip (const NODE *np);
extern int		permission_paint (const NODE *np);

extern void		mount_0 (void);
extern void		umount_0 (void);
extern void		eject_0 (void);
extern void		copy_file_0 (void);
extern void		cmp_file_0 (void);
extern void		mv_file_0 (void);
extern void		rm_file_dir_0 (void);
extern int		rm_file_dir_2 (NODE *np);
extern void		rename_0 (void);
extern void		rename_1 (void);
extern void		mkdir_0 (void);
extern void		du_0 (void);
extern void		zip_0 (void);
extern void		zip_tst_and_change_protection (void);
