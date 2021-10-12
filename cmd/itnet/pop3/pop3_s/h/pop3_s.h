/*
 ****************************************************************
 *								*
 *			pop3_c.h				*
 *								*
 *	Post Office Protocol - Version 3 (cliente)		*
 *								*
 *	Versão	3.1.0, de 09.10.98				*
 *		3.1.0, de 25.10.98				*
 *								*
 *	Módulo: POP3_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	DATASZ	1024

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 *****	Variáveis globais ***************************************
 */
extern const char	pgversion[];		/* Versão */

extern const char	*pgname;		/* Nome do programa */

extern int		vflag;			/* Verbose */

extern int		global_log_fd;		/* Para "error" */
extern FILE		*global_log_fp;

extern char		tcp_dev_nm[];		/* Nome do dispositivo TCP */

extern int		tcp_fd;		/* Conexão de controle */
extern FILE		*tcp_fp;		/* Conexão de controle */

extern char		snd_dis;		/* Previsão para em erro grave enviar DIS */

extern const char	*client_node_nm;	/* Nome do nó do cliente */

extern int		user_uid;		/* UID do usuário */

extern int		user_gid;		/* GID do usuário */

extern const char	status_nm[];

extern int		last_read_index;	/* mensagem atual */

extern char		mailbox_nm[];

/*
 ******	Estados do servidor *************************************
 */
typedef enum
{
	S_RESET,	/* Esperando um USER (login) */
	S_USER,		/* Esperando um PASS (senha) */
	S_READY         /* Esperando um comando qualquer */

}	STATE;

extern STATE		server_state;		/* Estado atual do servidor */

/*
 ******	Lista de mensagens **************************************
 */
typedef struct mbox	MBOX;

struct mbox
{
	MBOX		*m_back;	/* Ponteiro para o antecessor */
	MBOX		*m_forw;	/* Ponteiro para o sucessor */

	char		*m_nm;		/* Nome do arquivo */
	char		*m_uidl;	/* Identificação única */
	off_t		m_size;		/* Tamanho da carta */
	ino_t		m_ino;		/* No. do inode do arquivo */

	int		m_index;	/* No. da carta ou linha */

	char		m_status;	/* Estado: "U" ou "R" */
	char		m_erased;	/* Carta removida */
};

#define	NOMBOX	(MBOX *)NULL

extern MBOX		mbox_list_head;		/* Cabeça da lista de mensagens */
extern int		mbox_list_size;		/* No. de cartas da caixa postal */
extern MBOX		**mbox_list_vector;	/* Vetor para acesso direto */
extern int		mailbox_dev;		/* Dispositivo da caixa de correio */
extern int		mailbox_ino;		/* Nó-índice da caixa de correio */

extern PASSWD		user_pw;		/* Do usuário corrente */

/*
 ******	Tabela de comandos **************************************
 */
typedef struct tab
{
	const char	*c_nome;
	void		(*c_funcao) (int, const char *[]);
	const char	*c_args;
	const char	*c_help;

}	CMDTB;

extern CMDTB	cmdtab[];

#define	NOFUNC	(void (*) ())NULL

/*
 ******	Protótipos de funções ***********************************
 */
extern void	do_help (int, const char *[]);
extern void	do_quit (int, const char *[]);
extern void	do_noop (int, const char *[]);
extern void	do_stat (int, const char *[]);
extern void	do_list (int, const char *[]);
extern void	do_uidl (int, const char *[]);
extern void	do_retr (int, const char *[]);
extern void	do_top  (int, const char *[]);
extern void	send_letter (const char *, int);
extern void	do_last (int, const char *[]);
extern void	do_dele (int, const char *[]);
extern void	delete_messages (void);
extern void	do_rset (int, const char *[]);
extern void	frnputs (const char *, FILE *);

extern void	parse (const char *);
extern int 	check_ready_state (void);
extern char	*strtoupper (char *);
extern void	reply (const char *, ...);
extern void	error (const char *, ...);
extern void	tcp_abort (int);

extern void	do_user (int, const char *[]);
extern void	do_pass (int, const char *[]);

extern void	get_mbox (void);
extern void	put_mbox (void);
