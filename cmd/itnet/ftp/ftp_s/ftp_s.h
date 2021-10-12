/*
 ****************************************************************
 *								*
 *			ftp_s.h					*
 *								*
 *	File transfer protocol (servidor)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		4.0.0, de 29.03.01				*
 *								*
 *	Módulo: FTP_S						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****** Macros úteis ********************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	FTP_CNTL_PORT	21
#define	FTP_DATA_PORT	20

/*
 *****	Estados do servidor *************************************
 */
typedef enum
{
	S_RESET,	/* Esperando um USER (login) */
	S_USER,		/* Esperando um PASS (senha) */
	S_PASS,		/* Esperando um ACCT (senha) */
	S_READY,	/* Esperando um comando qualquer */
	S_RNFR		/* Esperando um RNTO (2a. parte de "rename" */

}	STATE;

extern STATE	server_state;	/* Estado atual do servidor */

/*
 *****	Tabela de comandos **************************************
 */
typedef struct tab
{
	const char	*c_nome;
	void		(*c_funcao) (int, const char *[]);
	const char	*c_args;
	const char	*c_help;

}	CMDTB;

extern const CMDTB	cmdtab[];

#define	NOFUNC	(void (*) ())NULL

/*
 ****** Variáveis globais ***************************************
 */
extern const char pgversion[];	/* Versão do programa */

extern const char *pgname;	/* Nome do programa */

extern const char tcp_dev_nm[];	/* Dispositivo TCP */

extern int	vflag;		/* Verbose */

extern int	cntl_fd;	/* Conexão de controle */

extern FILE	*cntl_fp;	/* Conexão de controle */

extern int	data_fd;	/* Conexão de dados */

extern int	snd_dis;	/* Em caso de erro, envia DIS */

extern INADDR	server_cntl_addr[2];	/* Endereço do servidor */

extern INADDR	client_data_addr;	/* Endereço da conexão de dados */

extern int	passive_mode;		/* Indica o modo passivo */

/*
 ******	Características da transferência ************************
 */
extern int	type;		/* Tipo do código */
extern int	stru;		/* Tipo da estrutura */
extern int	mode;		/* Modo de transferência */
extern int	form;		/* Forma  */

/*
 ******	Protótipos de funções ***********************************
 */
extern void	parse (const char *);
extern int 	check_ready_state (void);
extern char	*strtoupper (char *);
extern void	long_reply (int, const char *, ...);
extern void	reply (int, const char *, ...);
extern void	error (const char *, ...);
extern void	tcp_abort (int);
extern void	do_function (const char *, int (*) (const char *, FILE *), const char *);

extern void	do_user (int, const char *[]);
extern void	do_pass (int, const char *[]);
extern void	do_acct (int, const char *[]);
extern void	finish_login (void);
extern void	do_pasv (int, const char *[]);
extern void	do_port (int, const char *[]);
extern void	do_retr (int, const char *[]);
extern void	do_stor (int, const char *[]);
extern int	open_data_connection (void);

extern void	do_cwd (int, const char *[]);
extern void	do_cdup (int, const char *[]);
extern void	do_quit (int, const char *[]);
extern void	do_type (int, const char *[]);
extern void	do_stru (int, const char *[]);
extern void	do_mode (int, const char *[]);
extern void	do_rnfr (int, const char *[]);
extern void	do_rnto (int, const char *[]);
extern void	do_dele (int, const char *[]);
extern void	do_rmd (int, const char *[]);
extern void	do_mkd (int, const char *[]);
extern void	do_pwd (int, const char *[]);
extern void	do_list (int, const char *[]);
extern void	do_lc (int, const char *[]);
extern void	do_cmd (const char *, const char *);
extern void	do_nlst (int, const char *[]);
extern void	do_syst (int, const char *[]);
extern void	do_stat (int, const char *[]);
extern void	do_help (int, const char *[]);
extern void	do_noop (int, const char *[]);
extern int	proc_ls (const char *, FILE *);
