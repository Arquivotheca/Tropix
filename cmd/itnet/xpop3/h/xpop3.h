/*
 ****************************************************************
 *								*
 *			xpop3.h					*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 07.09.02				*
 *		4.4.0, de 08.05.03				*
 *								*
 *	Módulo: xpop3						*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definições globais **************************************
 */
#define	EVER	;;
#define	elif	else if
#define KBSHIFT	10
#define KBSZ	(1 << KBSHIFT)
#define MBSHIFT	20
#define MBSZ	(1 << MBSHIFT)
#define	DATASZ	1024

/*
 ******	Lista de mensagens **************************************
 */
enum { IGNORE, DO, DONE, NOP };

typedef struct mbox	MBOX;

struct mbox
{
	MBOX		*m_back;	/* Ponteiro para o antecessor */
	MBOX		*m_forw;	/* Ponteiro para o sucessor */

	int		m_index;	/* No. da carta */

	int		m_no;		/* Número da carta */
	char		*m_uidl;	/* Identicação do POP3 */

	char		m_status;	/* Estado: "U" ou "R" */
	char		m_rcv;		/* Objetivo: Recebe ou não */
	char		m_del;		/* Objetivo: Remove ou não */

	char		*m_from;	/* Remetente */
	char		*m_to;		/* Destinatário */
	char		*m_date;	/* Data */
	time_t		m_time;		/* Data já convertida */
	char		*m_subject;	/* Assunto */
	int		m_size;		/* Tamanho da carta */

	char		*m_text;	/* Texto da linha/cabeçalho */
	int		m_len;		/* Tamanho do texto */
};

#define	NOMBOX	(MBOX *)NULL

/*
 ******	Código de retorno do POP3 *******************************
 */
enum { OK, ERR };

#define	OKSZ	4	/* Tamanho de "+OK " */

/*
 ******	Dados da janela das CARTAs ******************************
 */
extern int		NLINE, NCOL;		/* Número total de linhas e colunas */

extern int		NMAIL, FIRST_LINE;	/* Número total de cartas, no. da primeira linha da tela */

extern int		mail_vec_sz, mail_vec_elem;
extern MBOX		**mail_vec_tb;

/*
 ******	Variáveis globais ***************************************
 */
extern int		vflag;			/* Verbose */

extern char		remote_mail_box_nm[64];	/* Nome da caixa postal remota */

extern int		xwin_fd;

extern char		passwd_str[32];

enum { DISCONNECTED, INCONNECTION, CONNECTED, LOGGED };

extern int		connection_state;	/* Estado da conexão */

extern char		local_user_nm[32],	/* Nome do usuário local */
			old_local_user_nm[32];

extern char		remote_user_nm[32];	/* Dados do usuário remoto */

extern char		remote_node_nm[64];	/* Nome do <nó remoto> */

extern char		first_reply_line[];

extern FILE		*local_uidl_fp;		/* FILE do arquivo de UIDLs */

extern int		sizem_size_in_use;	/* Limite de tamanho das cartas */

extern char		*mailbox_ptr;
extern char		mailbox_nm[128];

extern int		local_user_uid;
extern int		local_user_gid;

extern const char	status_nm[];

extern FILE		*local_status_fp;

extern FILE		*tcp_fp;

/*
 ******	Protótipos de funções ***********************************
 */
extern void		get_first_mailbox_nm (const char *argv);
extern void		write_msg (const char *text);
extern int		xerror (const char *format, ...);
extern int		get_reply (void);
extern char		*fnrgets_tcp (char *str, int n);
extern int		fnrgets_tcp_read (void);
extern char		*fnrgets (char *str, int n, FILE *fp);
extern int		comando (int do_wait, const char *format, ...);
extern void		prepare_to_read_passwd (void (*function) (void));
extern void		event_loop (int from);
extern void		send_passwd (void);
extern void		draw_connection_state_button (void);
extern const char	*edit_size (int size);
extern void		draw_percent_percent (int done, int total);
extern int		get_remote_UIDLs (void);
extern void		do_XFlush (void);
extern int		get_local_user_nm (void);
extern void		do_mime_2 (const char *in_area, char *out_area);
extern int		get_line_info (const char *line, MBOX *mp);
extern int		get_msg_info (MBOX *mp);
extern void		free_remote_uidl_list (void);
extern void		get_default_local_user_nm (void);
extern void		clear_mail_window (void);
extern void		get_top (MBOX *mp);
extern void		draw_mail_line (int index);
extern void		update_scroll_bar (int y, int new_first_line);
extern void		reset_scroll_button (void);
extern void		get_message (MBOX *mp);
extern void		draw_mail_rcv_button (const MBOX *mp, int win_index);
extern void		draw_mail_del_button (const MBOX *mp, int win_index);
extern void		disconnection_center (void);
extern char		*strtoupper (char *str);
extern void		clear_scroll_window (void);
extern void		clear_mail_rcv_del_window (void);
extern void		open_local_status_file (void);
extern void		check_local_passwd (void);
extern void		get_remote_and_connect (void);
extern void		update_remote_user_nm (void);

extern void		option_function (int);
extern void		do_connect_and_login (int);
extern void		do_disconnect (int);
extern void		do_receive (int);
extern void		do_delete (int);
extern void		quit_function (int);
extern void		do_version (void);
extern int		do_reset (void);
