/*
 ****************************************************************
 *								*
 *			ftp_c.h					*
 *								*
 *	File transfer protocol (cliente)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		4.0.0, de 28.03.01				*
 *								*
 *	Módulo: FTP_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	FTP_CNTL_PORT	21
#define	FTP_DATA_PORT	20

/*
 *****	Variáveis globais ***************************************
 */
extern const char *pgname;	/* Nome do programa */

extern const char pgversion[];	/* Versão */

extern int	snd_dis;	/* Em caso de erro, desconecta */

extern int	cntl_fd;	/* Descritor da linha de controle */

extern FILE	*cntl_fp;	/* FILE da linha de controle */

extern int	data_fd;	/* FD da linha de dados	*/

extern int	vflag;		/* Verbose */
extern int	nflag;		/* Sem login */
extern int	sflag;		/* Envia o port a cada operação */

extern const char tcp_dev_nm[];	/* Nome do dispositivo TCP */

extern INADDR	local_cntl_addr;	/* Endereço local de controle */

extern INADDR	local_data_addr;	/* Endereço local de dados */

extern INADDR	server_data_addr;	/* Para o modo passivo */

extern char	local_node_nm[];	/* Nome do nó local */

extern const char *remote_node_nm;	/* Nome do nó remoto */

extern const char *remote_user_nm;	/* Nome do usuário remoto */

extern int	last_data_port;		/* Port de dados */

extern int	received_intr;		/* Veio um <^C> */

/*
 ******	Características da transferência ************************
 */
extern int	type;		/* Tipo do código */
extern int	stru;		/* Tipo da estrutura */
extern int	mode;		/* Modo de transferência */
extern int	form;		/* Forma  */
extern int	passive_mode;	/* Indica modo passivo */

/*
 ******	Tabela de comandos **************************************
 */
typedef struct tab	CMDTB;

struct tab
{
	const char	*c_name;	/* Nome do comando */
	void		(*c_function) (int, const char *[], const CMDTB *);
	const char	*c_args;	/* Argumentos do comando */
	const char	*c_help;	/* Explicação do comando */

};

extern const CMDTB	cmdtab[];

#define	NOFUNC	(void (*) ())NULL

/*
 ******	Protótipos **********************************************
 */
extern void		do_connect (void);
extern void		exec_cmd (void);
extern void		exec_local_cmd (const char *);
extern const char	*env_var_scan (char *, const char *);
extern void		on_intr (int, ...);
extern void		error (const char *, ...);
extern void		tcp_abort (int);
extern void		help (void);

extern int		comando (int, const char *, ...);
extern int		get_reply (void);
extern void		do_login (int, const char *[], const CMDTB *);
extern void		recebe (const char *, const char *, const char *);
extern void		compara (const char *, const char *, const char *);
extern void		envia (const char *, const char *, const char *);
extern int		open_data_connection (const char *, const char *);
extern void		close_data_connection (void);

extern void		do_cd (int, const char *[], const CMDTB *);
extern void		do_cdup (int, const char *[], const CMDTB *);
extern void		do_quit (int, const char *[], const CMDTB *);
extern void		do_quote (int, const char *[], const CMDTB *);
extern void		do_ascii (int, const char *[], const CMDTB *);
extern void		do_image (int, const char *[], const CMDTB *);
extern void		do_settype (int, const char *[], const CMDTB *);
extern void		do_get (int, const char *[], const CMDTB *);
extern void		do_mget (int, const char *[], const CMDTB *);
extern void		do_put (int, const char *[], const CMDTB *);
extern void		do_mput (int, const char *[], const CMDTB *);
extern void		do_cmp (int, const char *[], const CMDTB *);
extern void		do_rename (int, const char *[], const CMDTB *);
extern void		do_reset (int, const char *[], const CMDTB *);
extern void		do_delete (int, const char *[], const CMDTB *);
extern void		do_rmdir (int, const char *[], const CMDTB *);
extern void		do_mkdir (int, const char *[], const CMDTB *);
extern void		do_pwd (int, const char *[], const CMDTB *);
extern void		do_list (int, const char *[], const CMDTB *);
extern void		do_lc (int, const char *[], const CMDTB *);
extern void		do_nlst (int, const char *[], const CMDTB *);
extern void		do_system (int, const char *[], const CMDTB *);
extern void		do_status (int, const char *[], const CMDTB *);
extern void		do_pasv (int, const char *[], const CMDTB *);
extern void		do_port (int, const char *[], const CMDTB *);
extern void		do_donothing (int, const char *[], const CMDTB *);
extern void		do_help (int, const char *[], const CMDTB *);
