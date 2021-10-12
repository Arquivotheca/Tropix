/*
 ****************************************************************
 *								*
 *			mail.h					*
 *								*
 *	Um cliente simples de correio eletrônico		*
 *								*
 *	Versão	3.0.0, de 19.12.97				*
 *		4.4.0, de 28.01.03				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL
#define	NOCSTR	(const char **)NULL

#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	DATASZ	1024

/*
 ******	Categorias Léxicas **************************************
 */
#define	ATOM	0x01		/* O caracter pode fazer parte de um átomo */
#define	SPACE	0x02		/* SPACE char (Separador) */
#define	KEYWORD	0x04		/* O caracter pode fazer parte de uma palavra chave */
#define	VALUE	0x08		/* O caracter pode fazer parte de um valor */

/*
 ******	Macros para Obtenção da Categoria & Índice **************
 */
#define	CATEG(cp)	categ[*(cp)]

extern const char	categ[];	/* Tabela de categorias */

/*
 ******	Variáveis globais ***************************************
 */
extern const char	pgversion[];	/* Versão do programa */

extern int		vflag;		/* Verbose */
extern int		rflag;		/* Não modifique a caixa postal */
extern int		dflag;		/* Dá o diretório */
extern int		sflag;		/* Tamanho da carta */
extern int		uflag;		/* Só considere cartas não lidas */

extern const char	*local_user_nm; /* Nome do usuário local */

extern char		mailbox_nm[];	/* Para compor o nome das cartas */

extern const char	base_64_tb[256]; /* Para a [de]codificação base 64 */
extern const char	base_64_vec[64];

extern const char	quote_vec[];	/* Para a conversão ISO */

extern char		*dead_letter_nm; /* Nome da cópia da carta */

extern FILE		*fp_tty;	/* Teclado */
extern FILE		*fp_out;	/* Tela do terminal */

extern int		LINE_SZ;	/* Número máximo de caracteres por linha */
extern int		TEXTLINE;	/* Última linha de texto (começando de 0) */
extern int		LASTLINE;	/* Linha da mensagem (começando de 0) */

extern char		*mailbox_ptr;

extern int		mailbox_dev;	/* Dispositivo da caixa de correio */
extern int		mailbox_ino;	/* Nó-índice da caixa de correio */
extern time_t		mailbox_time;	/* Última modificação da caixa de correio */

extern int		new_letters;	/* Indica recebimento de novas cartas */

extern const char	send_mail_nm[];
extern const char	*const send_mail_arg0;

extern char		*snd_nm;	/* Nome do remetente */

/*
 ****** Caracteres de controle **********************************
 */
#define		CTLA	0x01
#define		CTLB	0x02
#define		CTLC	0x03
#define		CTLD	0x04
#define		CTLE	0x05
#define		CTLF	0x06
#define		CTLG	0x07
#define		CTLI	0x09
#define		CTLL	0x0C
#define		CTLQ	0x11
#define		CTLR	0x12
#define		CTLS	0x13
#define		CTLU	0x15
#define		CTLV	0x16
#define		CTLY	0x19
#define		CTLZ	0x1A

/*
 ****************************************************************
 *	Estruturas das Cartas					*
 ****************************************************************
 */
typedef struct mdirec	MDIREC;
typedef struct part	PART;
typedef struct line	LINE;

#define NOMDIREC	(MDIREC *)NULL
#define NOPART		(PART *)NULL
#define NOLINE		(LINE *)NULL

/*
 ******	Estrutura das Linhas ************************************
 */
struct line
{
	int		l_index;	/* No. da linha */

	char		l_head;		/* É a cabeça da lista */

	char		*l_text;	/* Texto da linha/cabeçalho */
	int		l_len;		/* Tamanho do texto */

	LINE		*l_back;	/* Ponteiro para o antecessor */
	LINE		*l_forw;	/* Ponteiro para o sucessor */
};

/*
 ******	Estrutura de uma parte **********************************
 */
struct part
{
	int		p_index;	/* No. da parte */
	char		*p_name;	/* Nome da parte */

	int		p_size;		/* Tamanho da parte */

	char		p_head;		/* É a cabeça da lista */
	char		p_rfc822;	/* A parte é uma carta completa */
	char		p_is_text;	/* É Texto legível */
	char		p_base64;	/* Formato "base64" */

	char		p_quoted;	/* Caracteres na forma "=.." */

	char		*p_boundary;	/* Separador das várias sub-cartas */

	char		*p_text;	/* Texto da linha/cabeçalho */
	int		p_len;		/* Tamanho do texto */

	PART		*p_part_head;	/* Ponteiro para a lista de filhos */

	LINE		p_line_head;	/* Cabeçalho das linhas */

	PART		*p_back;	/* Ponteiro para o antecessor */
	PART		*p_forw;	/* Ponteiro para o sucessor */
};

/*
 ******	Estrutura das Cartas ************************************
 */
struct mdirec
{
	MDIREC		*m_back;	/* Ponteiro para o antecessor da lista */
	MDIREC		*m_forw;	/* Ponteiro para o sucessor da lista */

	char		m_head;		/* É a cabeça da lista */
	char		m_status;	/* Estado: "U" ou "R" */

	char		*m_nm;		/* Nome do arquivo */
	int		m_ino;		/* No. do inode do arquivo */

	int		m_index;	/* No. da carta */

	char		*m_from_text;	/* Remetente (Texto original) */
	char		*m_from_box;	/* Remetente (somente a caixa postal) */
	char		*m_to_text;	/* Destinatário (Texto original) */
	char		*m_to_box;	/* Destinatário (somente a caixa postal) */
	char		*m_cc_text;	/* Outros destinatários (Texto original) */
	char		*m_date;	/* Data */
	char		*m_subject;	/* Asssunto */

	char		*m_text;	/* Texto do cabeçalho */
	int		m_len;		/* Tamanho do texto */

	time_t		m_time;		/* Data convertida */
	fpos_t		m_offset;	/* Deslocamento do texto */

	PART		m_part_head;	/* Cabeça das partes */
};

extern MDIREC		delete_head;	/* para as cartas removidas */

/*
 ******	Estrutura de contrôle usada por "show_part" *************
 */
typedef struct
{
	PART	*b_screen_first_line;
	PART	*b_screen_last_line;
	PART	*b_cursor_mp;
	char	b_cmd;
	char	b_next_cmd;
	char	b_msg;

}	PARTBOX;

/*
 ******	Estrutura de contrôle usada por "show_direc" ************
 */
typedef struct
{
	MDIREC	*b_screen_first_line;
	MDIREC	*b_screen_last_line;
	MDIREC	*b_cursor_mp;
	char	b_cmd;
	char	b_next_cmd;
	char	b_msg;

}	MBOX;

/*
 ******	Códigos de retorno de "show" ****************************
 */
enum { NODE_BACK, NODE_FORW, NODE_UP, FORWARD, REPLY, DELETE, QUIT };

/*
 ****** Dados de "terminfo" *************************************
 */
extern const char *c_clear,	/* Limpa a tela */
		*c_cup,		/* Endereça o cursor */
		*c_ed,		/* Limpa até o resto da tela */
		*c_el,		/* Limpa até o resto da linha */
		*c_ri,		/* Scroll reverso */
		*c_rev,		/* Reverse video */
		*c_sgr0;	/* Termina um campo */

/*
 ******	Definição dos campos de "m_text" ************************
 */
#define	STATUS_COL	0
#define	CURSOR_COL	12
#define	INDEX_COL	2
#define	BEGIN_COL	7

/*
 ******	Protótipos de funções ***********************************
 */
extern void		mbox (void);
extern const char	*get_letter_info (MDIREC *);
extern char		*funrgets (char *area, int count, FILE *fp);
extern const char	*skip_blanks (const char *lp);
extern const char	*get_keyword (const char *lp, char *area, int count);
extern char		*strtoupper (char *str);
extern char		*alloc_and_copy (const char *str);
extern const char	*get_mailbox (const char *lp, char *area, int count);
extern time_t		get_date (const char *date_str);
extern const char	*skip_to_sm (const char *lp);
extern const char	*get_value (const char *lp, char *area, int count);
extern const char	*scan_date (const MDIREC *mp);
extern char		*fnrgets (char *str, int n, FILE *fp);
extern int		show_direc (MDIREC *, int);
extern int		show_part (const MDIREC *, PART *list_head);
extern int		show_line (const MDIREC *, LINE *line_head);
extern int		read_letter (MDIREC *letter_mp);
extern void		*mem_alloc (int size);
extern void		mem_free (void	*a);
extern LINE		*get_mime_line (char *area, int count, LINE *lp);
extern const char	*get_atom (const char *lp, char *area, int count);
extern int		get_base_64_sz (const LINE *lp);
extern char		*convert_quoted (const char *in_area, char *out_area);
extern void		convert_line_text (PART *part_head);
extern void		convert_part_text (PART *part_head);
extern void		text_line_insert (LINE *line_head, const char *format, ...);
extern void		text_part_insert (PART *line_head, const char *format, ...);
extern int		part_prolog (MDIREC *letter_mp);
extern int		show_base_64_text (const MDIREC *, PARTBOX *pp);
extern int		write_base_64_file (PARTBOX *pp);
extern int		append_lines (const char *file_nm, const LINE *line_head, const char *);
extern const char	*insert_attachments (char *, const char **);
extern const char	*last_id (const char *path);
extern void		save_text_file (FILE *fp);
extern void 		quit (int);
extern void 		update_status_file (void);
extern void 		convert_subject_to_ISO (char *subject, const char *local_subject, int size);
extern const char	*copy_text_file (const char *);
