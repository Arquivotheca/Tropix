/*
 ****************************************************************
 *								*
 *			mail_s.c				*
 *								*
 *	Um servidor simples de correio eletrônico		*
 *								*
 *	Versão	3.0.0, de 14.12.97				*
 *		4.9.0, de 22.01.07				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOCSTR	(const char **)NULL
#define	NOVOID	(void *)NULL
#define	EVER	;;
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	NARG	10			/* No. máximo para os comandos recebidos */

#define	DATASZ	1024

#define DIR_MODE 0700
#define REG_MODE 0600

#define	MORE	1024			/* Para "reply" */

#define	FORWARD_NM	"/.forward"

/*
 ******	Variáveis ***********************************************
 */
entry const char	mailrc_nm[] = "/etc/mailrc";

entry int		do_relay_service;

const char		pgversion[] =  "Versão: 4.9.0, de 22.01.07";

const char		tcp_dev_nm[] = "/dev/itntcp";

const char		send_mail_nm[] = "/usr/lib/send_mail";
const char		* const send_mail_arg0 = send_mail_nm + 9;

entry int		vflag = 0;		/* Verbose */

entry long		local_addr,		/* Endereços INTERNET */
			remote_addr;

entry char		client_node_nm[80];	/* Nome do cliente */

entry int		global_tcp_fd;		/* Conexão de controle */

entry FILE		*tcp_in,		/* Conexão de controle */
			*tcp_out;

entry UTSNAME		uts;			/* Para tirar uma série de informações */

char			my_node_nm[256];	/* Nome do computador */
const char		*my_domain_nm;		/* Nome do domínio */
entry int		my_node_nm_len;		/* Comprimento apenas da primeira parte */

#define	MAIL_PREFIX	"/var/mail/"

entry char		mailbox_nm[64] = MAIL_PREFIX;
entry char		* const mailbox_ptr = mailbox_nm + sizeof (MAIL_PREFIX) - 1;

const char		status_nm[] = "status";	/* Arquivo de estado */

const PASSWD		nouser =		/* Para destinatário remoto */
{
	"",		/* Usuário */
	"",		/* Senha */
	"",		/* Comentário */
	0, 0,		/* UID, GID */
	0, 0,		/* Quota, Expir */
	"",		/* Lock */
	"",		/* Dir */
	""		/* Shell */
};

entry int		n_total, n_remote;	/* Contadores de cartas */

#undef	SAVE_IN
#ifdef	SAVE_IN
entry FILE		*save_file_fp;
#endif	SAVE_IN

/*
 ******	Variáveis alocadas dinamicamente ************************
 */
entry char	*client_domain;		/* Computador cliente */
entry char	*reverse_path;		/* Nome do remetente */

const char	*text_tmp_nm;		/* Nome do arquivo do texto */

/*
 ******	Estrutura da lista de destinatários *********************
 *
 *	Se "d_user_nm" for NOSTR, "d_dst_nm" dá o "forward" em
 *	outro computador.
 */
typedef struct dst	DST;

struct dst
{
	char		d_remote;	/* Indicador de destinatário remoto */
	const char	*d_dst_nm;	/* Nome completo do destinatário */
	const char	*d_user_nm;	/* Nome do usuário neste computador */
	int		d_uid;		/* UID do usuário neste computador */
	int		d_gid;		/* GID do usuário neste computador */
	const char	*d_dir;		/* Nome do diretório "home" */
	DST		*d_next;	/* Ponteiro para o próximo */
};

#define	NODST	(DST *)NULL

entry DST	*dst_head,		/* Cabeça da lista */
		*dst_tail;		/* Final da lista */

/*
 ******	Estrutura de controle da expansão de ".forward" *********
 */
typedef struct
{
	int		c_good_msg_no;	/* Número de "reply"s corretos enviados */
	int		c_bad_msg_no;	/* Número de "reply"s de erro enviados */

}	CNTL;

/*
 ******	Estrutura da tabela de comandos *************************
 */
typedef struct tab
{
	const char	*c_nome;
	void		(*c_funcao) (int, const char *[]);

}	CMDTB;

extern const CMDTB	cmdtab[];

#define	NOFUNC	(void (*) ())NULL

/*
 ******	Definição da tabela de comandos *************************
 */
void		do_helo (int, const char *[]);
void		do_mail (int, const char *[]);
void		do_rcpt (int, const char *[]);
void		do_data (int, const char *[]);
void		do_rset (int, const char *[]);
void		do_noop (int, const char *[]);
void		do_quit (int, const char *[]);

const CMDTB	cmdtab[] =
{
	"HELO",	do_helo,
	"MAIL",	do_mail,
	"RCPT",	do_rcpt,
	"DATA",	do_data,
	"RSET",	do_rset,
	"NOOP",	do_noop,
	"QUIT",	do_quit,
	NOSTR
};

/*
 ******	Tabela de nomes de meses ********************************
 */
typedef char	LINE[4];

const LINE	get_date_month_table[] =
{
	"jan", "feb", "mar", "apr", "may", "jun",
	"jul", "aug", "sep", "oct", "nov", "dec", ""
};

/*
 ******	Tabela de nomes de zonas de tempo ***********************
 */
typedef struct
{
	char	z_nm[4];	/* Nome da zona */
	int	z_zone;		/* Diferencial */

}	ZONE;

const ZONE	get_date_zone_table[] =
{
	"UT",	0,	"GMT",	0,
	"EST",	-5,	"EDT",	-4,
	"CST",	-6,	"CDT",	-5,
	"MST",	-7,	"MDT",	-6,
	"PST",	-8,	"PDT",	-7,
	""
};

/*
 ******	Protótipos **********************************************
 */
void		parse (const char *);
int		examine_dst_nm (const DST *, char *, CNTL *, int);
DST		*creat_dst_node (const char *, const char *, const PASSWD *);
int		get_forward (DST *, CNTL *);
int		put_letter_in_mailbox (const DST *, FILE *, time_t, const char *);
int		send_to_remote_dst (void);
void		free_all_dst (void);
void		reply (int, const char *, ...);
time_t		get_date (const char *);
char		*fnrgets (char *, int, FILE *);
void		read_rc_file (void);
char		*strtoupper (char *);
char		*strtolower (char *);
void		error (const char *, ...);
void		on_signal (int, ...);
void		tcp_abort (int);

/*
 ****************************************************************
 *	Um servidor simples de correio eletrônico		*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int			opt;
	const char		*cp;
	const char		*str;
	INADDR			client_addr;
	INADDR			serv_addr[2];
	T_BIND			bind;
	T_CALL			call;
	int			tcp_fd, listen_port, listen_fd, sequence;
	char			comando[DATASZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vM")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			 vflag++;
			 break;

		    case 'M':			/* Residente */
			 exit (0);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Obtém o UTS do sistema
	 */
	if (uname (&uts) < 0)
		error ("$*Não consegui obter o UTS");

	/*
	 *  tcmpto_s [-v] <listen_port> <listen_fd> <client_addr> <client_port> <sequence>
	 */
#undef	DEBUG
#ifdef	DEBUG
	if (Bflag) error
	(	"%s <%s> <%s> <%s> <%s> <%s>",
		vflag ? "-v" : "",
		argv[0], argv[1], argv[2], argv[3], argv[4]
	);
#endif	DEBUG

	if (argc != 5) error
	(	"$Número inválido de argumentos: %d",
		argc
	);

	listen_port	   = strtol (argv[0], &str, 0);
	listen_fd	   = strtol (argv[1], &str, 0);
	client_addr.a_addr = t_str_to_addr (argv[2]);
	client_addr.a_port = strtol (argv[3], &str, 0);
	sequence	   = strtol (argv[4], &str, 0);

	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	global_tcp_fd = tcp_fd;

	if ((tcp_in  = fdopen (tcp_fd, "r")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	if ((tcp_out = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	t_node_to_addr (tcp_fd, argv[2], &local_addr);

	remote_addr = client_addr.a_addr;

	if ((str = t_addr_to_node (tcp_fd, remote_addr)) == NOSTR)
		str = t_addr_to_str (remote_addr);

	strcpy (client_node_nm, str);

#ifdef	DEBUG
	error ("local_addr = %s, ", t_addr_to_str (local_addr));
	error ("remote_addr = %s\n", t_addr_to_str (remote_addr));
#endif	DEBUG

	/*
	 *	Vou realizar o "bind" (usa o mesmo port do MAIL)
	 */
	serv_addr[0].a_port = listen_port;
	serv_addr[1].a_addr = client_addr.a_addr;
	serv_addr[1].a_port = client_addr.a_port;

	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.len	 = 2 * sizeof (INADDR);
	bind.addr.buf	 = serv_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, &bind) < 0)
		error ("$*Não consegui atribuir endereço");

	/*
	 *	Vou realizar o "accept"
	 */
   /***	call.opt.len = 0; ***/
   /***	call.udata.len = 0; ***/
	call.sequence = sequence;

	call.addr.len	 = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &client_addr;

#ifdef	DEBUG
	if (Bflag) error
	(	"Vou dar accept para (%s, %d)",
		t_addr_to_str (client_addr.a_addr), client_addr.a_port
	);
#endif	DEBUG

	if (t_accept (listen_fd, tcp_fd, &call) < 0)
		error ("$*Erro no t_accept");

	t_close (listen_fd);

	/*
	 *	Obtém o nome do próprio computador
	 */
	cp = t_addr_to_node (tcp_fd, 0);

	if (cp[0] >= '0' && cp[0] <= '9')
		error ("$NÃO consegui obter o nome do próprio computador");

	strcpy (my_node_nm, cp); cp = strchr (my_node_nm, '.');

	if (cp == NOSTR)
		error ("$NÃO consegui obter o nome do próprio domínio");

	my_domain_nm = cp + 1; my_node_nm_len = cp - my_node_nm;

#ifdef	DEBUG
	error
	(	"my_node_nm = \"%s\", my_domain_nm = \"%s\", my_node_nm_len = %d",
		my_node_nm, my_domain_nm, my_node_nm_len
	);
#endif	DEBUG

	/*
	 *	Em caso de sinal, ...
	 */
	signal (SIGINT,  on_signal);
	signal (SIGTERM, on_signal);

	/*
	 *	Lê o arquivo de configuração
	 */
	read_rc_file ();

	/*
	 *	Envia a mensagem de saudação
	 */ 
	reply
	(	220, "%s, Servidor mail TROPIX, %s, pronto para receber cartas",
		my_node_nm, pgversion
	);

	/*
	 *	Salva a carta recebida, se for o caso
	 */
#ifdef	SAVE_IN
	save_file_fp = fopen ("/tmp/mail_letter_save", "w");
#endif	SAVE_IN

	/*
	 *	Entra no ciclo de atender pedidos
	 */
	for (EVER)
	{
		if (fnrgets (comando, DATASZ, tcp_in) == NOSTR || comando[0] == '\0')
			strcpy (comando, "QUIT\r\n");

		parse (comando);
	}

	/* Não retorna */

}	/* end mail_s */

/*
 ****************************************************************
 *	Analisador de  Comandos					*
 ****************************************************************
 */
void
parse (const char *comando)
{
	const CMDTB		*tp;
	const char		*cp;
	int			argc;
	const char		*argv[NARG+1];

	/*
	 *	Procura o comando na tabela 
	 */
	cp = strtoupper (strtok ((char *)comando, " \t\r\n"));

	for (tp = cmdtab; /* teste abaixo */; tp++)
	{
		if (tp->c_nome == NOSTR)
		{
			reply (500, "Comando \"%s\" desconhecido", cp);
			return;
		}

		if (streq (cp, tp->c_nome))
			break;
	}

	if (tp->c_funcao == NOFUNC)
	{
		reply (502, "Comando \"%s\" não implementado", cp);
		return;
	}

	/*
	 *	Retira a linha de argumentos
	 */
	for (argc = 0; argc < NARG; argc++)
	{
		if ((cp = strtok (NOSTR, "\r\n")) == NOSTR)
			break;

		while (*cp == ' ' || *cp == '\t')
			cp++;

		argv[argc] = cp;
	}

	argv[argc] = NOSTR;

#ifdef	DEBUG
	{
		int i;

		printf ("cmd_parse: (%d) \"%s", argc, tp->c_nome);

		for (i = 0; i < argc; i++)
			printf (" %s", argv[i]);

		printf ("\"\n");
	}
#endif	DEBUG

	/*
	 *	Executa a função pedida
	 */
	(*tp->c_funcao) (argc, argv);

}	/* end parse */

/*
 ****************************************************************
 *	Executa o "HELO"					*
 ****************************************************************
 */
void
do_helo (int argc, const char *argv[])
{ 
	if   (argc == 0)
	{
		reply (501, "Faltando o domínio do comando HELO");
	}
	elif (client_domain != NOSTR)
	{
		reply (503, "Já foi processado um comando HELO");
	}
	elif ((client_domain = malloc (strlen (argv[0]) + 1)) == NOSTR)
	{
		reply (552, "Memória insuficiente");
	}
	else
	{
		strcpy (client_domain, argv[0]);

		reply
		(	250, "%s Hello %s (%s), prazer em encontrá-lo",
			my_node_nm, client_domain, client_node_nm
		);
	}

}	/* end do_helo */

/*
 ****************************************************************
 *	Executa o "MAIL"					*
 ****************************************************************
 */
void
do_mail (int argc, const char *argv[])
{ 
	char		*path, *low, *high;

	if (argc == 0)
	{
		reply (501, "Faltando o remetente do comando MAIL");
		return;
	}

	if
	(	(path = strtok ((char *)argv[0], " \t:")) == NOSTR ||
		!streq (strtoupper (path), "FROM")
	)
	{
		reply (501, "Não foi encontrado o FROM do comando MAIL");
		return;
	}

	if ((path = strtok (NOSTR, "")) == NOSTR)
	{
		reply (501, "Não foi encontrado o FROM do comando MAIL");
		return;
	}

	if ((low = strchr (path, '<')) != NOSTR && (high = strrchr (low, '>')) != NOSTR)
		{ path = low + 1; *high = '\0'; }

	/*
	 *	Se não é o primeiro MAIL, libera os recursos
	 */
	if (reverse_path != NOSTR)
	{
		free (reverse_path);

		free_all_dst ();
	}

	if ((reverse_path = malloc (strlen (path) + 1)) == NOSTR)
	{
		reply (452, "Memória insuficiente");
	}
	else
	{
		strcpy (reverse_path, path);

		reply
		(	250, "<%s> ... remetente correto",
			reverse_path
		);
	}

}	/* end do_mail */

/*
 ****************************************************************
 *	Executa o "RCPT"					*
 ****************************************************************
 */
void
do_rcpt (int argc, const char *argv[])
{ 
	char		*dst_nm;
	CNTL		cntl;

	if (argc == 0)
		{ reply (501, "Faltando o remetente do comando RCPT"); return; }

	if
	(	(dst_nm = strtok ((char *)argv[0], " \t:")) == NOSTR ||
		!streq (strtoupper (dst_nm), "TO")
	)
	{
		reply (501, "Não foi encontrado o TO do comando RCPT");
		return;
	}

	if ((dst_nm = strtok (NOSTR, " \t:<>")) == NOSTR)
		{ reply (501, "Não foi encontrado o destinatário do comando RCPT"); return; }

	cntl.c_bad_msg_no  = 0;

	examine_dst_nm (NODST, dst_nm, &cntl, 0 /* Not from ".forward" */);

	if (cntl.c_bad_msg_no != 0)
		reply (550, "<%s> ... destinatário com problemas", dst_nm);
	else
		reply (250, "<%s> ... destinatário correto", dst_nm);

}	/* end do_rcpt */

/*
 ****************************************************************
 *	Examina a sintaxe do destinatário			*
 ****************************************************************
 */
int
examine_dst_nm (const DST *orig_dp, char *dst_nm, CNTL *cntl, int forward)
{
	DST			*dp;
	char			*user_nm, *cp;
	const char		*host_nm, *domain_nm;
	const PASSWD		*pwd = NOPASSWD;
	int			len, local = 0;		/* A princípio, remoto */
	long			addr;

	/*
	 *	Examina o nome do destinatário
	 *
	 *	Para ser local, deve ter um dos seguintes padrões:
	 *
	 *		"user"
	 *		"user@host"
	 *		"user@domain"
	 *		"user@host.domain"
	 */
	strtolower (dst_nm);

	/*
	 *	Obtém o nome do computador
	 */
	if ((host_nm = strchr (dst_nm, '@')) == NOSTR)
		{ len = strlen (dst_nm);	host_nm = my_node_nm; }	/* É da forma "user" */
	else
		{ len = host_nm - dst_nm;	host_nm++; }		/* É da forma "user@host ..." */

	user_nm = alloca (len + 1); strncpy (user_nm, dst_nm, len); user_nm[len] = '\0';

	/*
	 *	Obtém o domínio
	 */
	if ((domain_nm = strchr (host_nm, '.')) == NOSTR)		/* É da forma "user@host" */
	{
		cp = alloca (strlen (host_nm) + strlen (my_domain_nm) + 2);

		sprintf (cp, "%s.%s", host_nm, my_domain_nm); host_nm = cp; domain_nm = my_domain_nm;
	}
	else								/* É da forma "user@host.domain" */
	{
		domain_nm++;
	}

	/*
	 *	Classifica o destinatário
	 */
	if   (streq (host_nm, my_node_nm))
	{
		local = 1;			/* É exatamento o nome deste computador */
	}
	elif (!streq (domain_nm, my_domain_nm))
	{
	   /***	local = 0; ***/			/* É um outro domínio */
	}
	elif ((addr = t_node_to_addr (global_tcp_fd, host_nm, &local_addr)) == -1)
	{
	   /***	local = 0; ***/			/* Se não conseguiu obter o endereço, supõe remoto */
	}
	elif (addr == local_addr)
	{
		local = 1;			/* O computador é este (um alias) */
	}
	else
	{
	   /***	local = 0; ***/			/* É outro computador */
	}

	/*
	 *	Envia a mensagem
	 */
	if (forward)
	{
		reply
		(	250|MORE, "<%s> ... destinatário %s obtido de \".forward\"",
			dst_nm, local ? "local" : "remoto"
		);
	}

	if   (local)
	{
		pwd = getpwnam (user_nm);
	}
	elif (forward || do_relay_service)
	{
		if ((dp = creat_dst_node (dst_nm, user_nm, &nouser)) == NODST)
			{ cntl->c_bad_msg_no++; return (-1); }

		dp->d_remote = 1;

		if (dst_head == NODST)
			dst_head	 = dp;
		else
			dst_tail->d_next = dp;

		dst_tail = dp; dp->d_next = NODST;

		reply (250|MORE, "<%s> ... enviando para servidor remoto", dst_nm);

		return (0);
	}

	/*
	 *	Usuário local ou desconhecido
	 */
	if (pwd == NOPASSWD)
	{
		reply (550|MORE, "<%s> ... destinatário desconhecido", dst_nm);

		cntl->c_bad_msg_no++; return (-1);
	}

	if (orig_dp != NODST && streq (orig_dp->d_user_nm, user_nm))
	{
		reply (550|MORE, "<%s> ... destinatário em \".forward\" cíclico", dst_nm);

		cntl->c_bad_msg_no++; return (-1);
	}

	if ((dp = creat_dst_node (dst_nm, user_nm, pwd)) == NODST)
		{ cntl->c_bad_msg_no++; return (-1); }

	/*** dp->d_remote = 0; ***/

	return (get_forward (dp, cntl));

}	/* end examine_dst_nm */

/*
 ****************************************************************
 *	Cria um nó da lista de destinatários			*
 ****************************************************************
 */
DST *
creat_dst_node (const char *dst_nm, const char *user_nm, const PASSWD *pwd)
{
	DST			*dp;

	/*
	 *	Prepara uma estrutura DST
	 */
	if ((dp = malloc (sizeof (DST))) == NODST)
	{
		reply (452|MORE, "Memória insuficiente");

		return (NODST);
	}

	if ((dp->d_dst_nm = strdup (dst_nm)) == NOSTR)
	{
		reply (452|MORE, "Memória insuficiente");

		free (dp); return (NODST);
	}

	if ((dp->d_user_nm = strdup (pwd->pw_name)) == NOSTR)
	{
		reply (452|MORE, "Memória insuficiente");

		free ((char *)dp->d_dst_nm); free (dp); return (NODST);
	}

	if ((dp->d_dir = strdup (pwd->pw_dir)) == NOSTR)
	{
		reply (452|MORE, "Memória insuficiente");

		free ((char *)dp->d_user_nm); free ((char *)dp->d_dst_nm); free (dp); return (NODST);
	}

	dp->d_remote	= 0;
   /***	dp->d_dst_nm	= strdup (dst_nm); ***/
   /***	dp->d_user_nm	= strdup (user_nm); ***/
	dp->d_uid	= pwd->pw_uid;
	dp->d_gid	= pwd->pw_gid;
   /***	dp->d_dir	= strdup (pwd->pw_dir); ***/
   /***	dp->d_next	= ...; ***/

	return (dp);

}	/* end creat_dst_node */

/*
 ****************************************************************
 *	Verifica se o usuário tem ".forward"			*
 ****************************************************************
 */
int
get_forward (DST *dp, CNTL *cntl)
{
	char			*dst_nm;
	char			*forward_nm;
	FILE			*forward_fp;
	char			area[DATASZ], *area_ptr = NOSTR;
	int			value = 0;

	/*
	 *	Verifica se tem ".forward"
	 */
	forward_nm = alloca (strlen (dp->d_dir + sizeof (FORWARD_NM) + 1));
	strcpy (forward_nm, dp->d_dir); strcat (forward_nm, FORWARD_NM);

	if ((forward_fp = fopen (forward_nm, "r")) == NOFILE)
	{
		if (dst_head == NODST)
			dst_head	 = dp;
		else
			dst_tail->d_next = dp;

		dst_tail   = dp;
		dp->d_next = NODST;

		return (0);
	}

	/*
	 *	Le o arquivo ".forward"
	 */
	for (EVER)
	{
		if (area_ptr == NOSTR)
		{
			if (fngets (area, DATASZ, forward_fp) == NOSTR)
				break;

			area_ptr = area;

			dst_nm = strtok (area,  " \t,");
		}
		else
		{
			dst_nm = strtok (NOSTR, " \t,");
		}

		if (dst_nm == NOSTR)
			{ area_ptr = NOSTR; continue; }

		value += examine_dst_nm (dp, dst_nm, cntl, 1 /* From ".forward" */);

	}	/* end for (EVER) */

	fclose (forward_fp);

	return (value);

}	/* end get_forward */

/*
 ****************************************************************
 *	Executa o "DATA"					*
 ****************************************************************
 */
void
do_data (int argc, const char *argv[])
{ 
	FILE			*data_fp;
	char			*src, *key_word, *uidl = NOSTR;
	int			err = 0;
	char			end_of_header_seen = 0;
	time_t			tempo = -1;
	char			area[DATASZ];
	const DST		*dp;

	/*
	 *	Obtém um arquivo temporário para o DATA
	 */
	if ((text_tmp_nm = mktemp ("/var/tmp/mail_server_text_file_XXXXXX")) == NOSTR)
	{
		reply (452, "Não foi possível obter o nome de um arquivo temporário");
		return;
	}

	if ((data_fp = fopen (text_tmp_nm, "w+")) == NOFILE)
	{
		reply (452, "Não foi possível abrir o arquivo temporário");
		return;
	}

	reply (354, "Envie o texto, terminando com apenas \".\" em um linha");

	/*
	 *	Lê o texto, procurando o "date:" e "uidl:"
	 */
	for (EVER)
	{
		if (fnrgets (area, DATASZ, tcp_in) == NOSTR)
			error (NOSTR);

		if (area[0] == '.' && area[1] == '\0')
			break;

		fnputs (area, data_fp);

		if (end_of_header_seen)
			continue;

		if (area[0] == '\0')
			{ end_of_header_seen = 1; continue; }

		/* Examina o cabeçalho */

		if ((src = strchr (area, ':')) == NOSTR)
			continue;

		*src++ = '\0'; key_word = area;

		while (*src == ' ' || *src == '\t')
			src++;

		if (*src == '\0')
			continue;

		if   (streq (strtoupper (key_word), "DATE"))
			tempo = get_date (src);
		elif (streq (key_word, "UIDL"))
			{ uidl = alloca (strlen (src) + 1); strcpy (uidl, src); }
	}

	fflush (data_fp);

	if (ferror (data_fp))
	{
		reply (451, "Não há espaço suficiente para o texto da carta");

		fclose (data_fp); unlink (text_tmp_nm);

		return;
	}

	/*
	 *	Coloca o texto em todas as caixas postais locais
	 */
	for (dp = dst_head; dp != NODST; dp = dp->d_next)
	{
		n_total++;

		if (!dp->d_remote)
			err += put_letter_in_mailbox (dp, data_fp, tempo, uidl);
		else
			n_remote++;
	}

	fclose (data_fp);

	if (err)
	{
		reply (451, "Não consegui copiar o texto para caixa postal");

		unlink (text_tmp_nm);

		return;
	}

	/*
	 *	Se for o caso, invoca "send_mail" para as cartas remotas
	 */
	if   (n_remote > 0)
		err = send_to_remote_dst ();
	else
		unlink (text_tmp_nm);

	if (err)
		reply (451, "Não consegui enviar as cartas para destinos remotos");
	else
		reply (250, "Arquivo de texto ... recebido e armazenado corretamente");

}	/* end do_data */

/*
 ****************************************************************
 *	Coloca o texto em uma caixa postal			*
 ****************************************************************
 */
int
put_letter_in_mailbox (const DST *dp, FILE *data_fp, time_t tempo, const char *uidl)
{
	FILE			*fp;
	int			c;
	int			fd;
	int			i;
	STAT			s;

	/*
	 *	Tenta inicialmente o tempo dado para formar o nome único
	 */
	for (i = 20; /* abaixo */; i--, tempo++)
	{
		if (i <= 0)
			goto bad_without_close;

		sprintf (mailbox_ptr, "%s/l.%u.t", dp->d_user_nm, tempo);

		if ((fd = open (mailbox_nm, O_RDWR|O_CREAT|O_EXCL, REG_MODE)) >= 0)
			break;

		if (errno != EEXIST)
			goto bad_without_close;

	}	/* end for (letras) */

	if (chown (mailbox_nm, dp->d_uid, dp->d_gid) < 0)
		goto bad_with_close;

	if ((fp = fdopen (fd, "w")) == NOFILE)
	{
	    bad_with_close:
		close (fd);
	    bad_without_close:
		error ("*Não consegui criar o arquivo para a carta \"%s\"", mailbox_nm);
		return (1);
	}

	/*
	 *	Finalmente, escreve o conteúdo da carta
	 */
	fprintf (fp, "Status: U\n");
	fprintf (fp, "From %s", reverse_path);

	if (tempo != -1)
		fprintf (fp, " %24.24s\n", btime (&tempo));
	else
		fprintf (fp, "\n");

	rewind (data_fp);

	while ((c = getc (data_fp)) >= 0)
		putc (c, fp);

	fflush (fp); fstat (fileno (fp), &s); fclose (fp);

	/*
	 *	Cria/atualiza o arquivo de "status"
	 */
	sprintf (mailbox_ptr, "%s/%s", dp->d_user_nm, status_nm);

	if
	(	(fd = open (mailbox_nm, O_WRONLY|O_CREAT, REG_MODE)) < 0 ||
		(fp = fdopen (fd, "w")) == NOFILE
	)
	{
		error ("*Não consegui criar o arquivo de estado \"%s\"", mailbox_nm);
		return (0);
	}

	if (chown (mailbox_nm, dp->d_uid, dp->d_gid) < 0)
		error ("*Não consegui mudar o dono/grupo do arquivo de estado \"%s\"", mailbox_nm);

	fprintf (fp, "N\n");

	fseek (fp, 0, SEEK_END);

	fprintf (fp, "l.%u.t:", tempo);

	if (uidl != NOSTR)
		fprintf (fp, "%s:", uidl); 
	else
		fprintf (fp, "l.%u.t:", tempo);

	fprintf (fp, "%d:%d\n", s.st_size, s.st_ino);  

	fclose (fp);

#ifdef	DEBUG
	error ("Carta recebida: \"%s\"", mailbox_nm);
#endif	DEBUG

	return (0);

}	/* end put_letter_in_mailbox */

/*
 ****************************************************************
 *	Invoca o "send_mail" para destinos remotos		*
 ****************************************************************
 */
int
send_to_remote_dst (void)
{
	const DST	*dp;
#if (0)	/*******************************************************/
	FILE		*header_fp;
	const char	*header_tmp_nm;
#endif	/*******************************************************/
	const char	**vv, **vp;
	long		pid;

	/*
	 *	Gera um filho, para comunicar-se com o "send_mail"
	 */
	if ((pid = fork ()) < 0)
		{ error ("*Não consegui dar FORK"); return (-1); }

	if (pid > 0)
		return (0);

#if (0)	/*******************************************************/
	/*
	 *	Gera o arquivo de cabeaçalho, que tem o formato:
	 *
	 *		Date: Fri, 05 Feb 1999 13:05:41 -0200
	 *		From: Pedro Salenbauch <pedrosal@nce.ufrj.br>
	 *		To: pedrosal@nce.ufrj.br, luciana@cremerj.com.br
	 *		Subject: Assunto
	 */
	if ((header_tmp_nm = mktemp ("/tmp/mail_server_header_file_XXXXXX")) == NOSTR)
		error ("$*Não consegui obter um nome para o arquivo de cabeçalho");

	if ((header_fp = fopen (header_tmp_nm, "w+")) == NOFILE)
		error ("$*Não consegui abrir o arquivo de cabeçalho");

	fprintf (header_fp, "From: <%s>\r\n", reverse_path);

	fclose (header_fp);
#endif	/*******************************************************/

	/*
	 *	Tenta remeter as cartas para os ".forward" remotos
	 *
	 *	Invoca o programa "send_mail"
	 */
	vv = vp = alloca ((n_remote + 8) * sizeof (char *));

	*vp++ = send_mail_arg0;

	if (vflag)
		*vp++ = "-v";

#if (0)	/*******************************************************/
	*vp++ = "-f";	/* A carta já tem cabeçalho */

	*vp++ = header_tmp_nm;
#endif	/*******************************************************/

	*vp++ = text_tmp_nm;

	*vp++ = reverse_path;

	for (dp = dst_head; dp != NODST; dp = dp->d_next)
	{
		if (dp->d_remote)
			*vp++ = dp->d_dst_nm;
	}

	*vp = NOSTR;

#undef	SEND_MAIL_DEBUG
#ifdef	SEND_MAIL_DEBUG
	error ("SEND_MAIL: ");
	for (vp = vv; *vp != NOSTR; vp++)
		error ("%s ", *vp);
#endif	SEND_MAIL_DEBUG

	execv (send_mail_nm, vv);
	error ("$*Não consegui executar \"%s\"", send_mail_nm);

	return (0);

}	/* end send_to_remote_dst */

/*
 ****************************************************************
 *	Executa o "RSET"					*
 ****************************************************************
 */
void
do_rset (int argc, const char *argv[])
{ 
	if (client_domain != NOSTR)
		{ free (client_domain); client_domain = NOSTR; }

	if (reverse_path != NOSTR)
		{ free (reverse_path); reverse_path = NOSTR; }

	free_all_dst ();

	if (text_tmp_nm != NOSTR)
		unlink (text_tmp_nm);

	reply
	(	250,
		"Executei o comando RSET corretamente"
	);

}	/* end do_rset */

/*
 ****************************************************************
 *	Executa o "NOOP"					*
 ****************************************************************
 */
void
do_noop (int argc, const char *argv[])
{ 
	reply
	(	250,
		"Executei o comando NOOP corretamente"
	);

}	/* end do_noop */

/*
 ****************************************************************
 *	Executa o "QUIT"					*
 ****************************************************************
 */
void
do_quit (int argc, const char *argv[])
{ 
	reply
	(	221,
		"%s ... entregando o correio para %d destinatário(s), %d remoto(s)",
		my_node_nm, n_total, n_remote
	);

	/*
	 *	Epílogo
	 */
	if (t_sndrel (global_tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (global_tcp_fd) < 0)
		error ("$*Erro no RCVREL");

#ifdef	SAVE_IN
	fclose (save_file_fp);
#endif	SAVE_IN

	exit (0);

}	/* end do_quit */

/*
 ****************************************************************
 *	Libera toda a lista de destinatários			*
 ****************************************************************
 */
void
free_all_dst (void)
{ 
	DST		*dp, *next;

	for (dp = dst_head; dp != NODST; dp = next)
	{
		if (dp->d_dst_nm != NOSTR)
			free ((char *)dp->d_dst_nm);

		if (dp->d_user_nm != NOSTR)
			free ((char *)dp->d_user_nm);

		if (dp->d_dir != NOSTR)
			free ((char *)dp->d_dir);

		next = dp->d_next; free (dp);
	}

	dst_head = dst_tail = NODST;

}	/* end free_all_dst */

/*
 ****************************************************************
 *	Envia uma mensagem normal para o cliente		*
 ****************************************************************
 */
void
reply (int code, const char *format, ...)
{
	va_list		args;
	char		msg[BLSZ];

	va_start (args, format);

	if (code & MORE)
		sprintf  (msg, "%03d-", code & ~MORE);
	else
		sprintf  (msg, "%03d ", code);

	vsprintf (msg + 4, format, args);
	strcat (msg, "\r\n");

	if (t_snd (global_tcp_fd, msg, strlen (msg), T_PUSH) < 0)
		error (NOSTR);

	va_end (args);

}	/* end reply */

/*
 ****************************************************************
 *	Analisa o arquivo "/etc/mailrc" (se presente)		*
 ****************************************************************
 */
void
read_rc_file (void)
{
	FILE		*rc_fp;
	char		area[128];

	if ((rc_fp = fopen (mailrc_nm, "r")) == NOFILE)
		return;

	while (fnrgets (area, sizeof (area), rc_fp) == area)
	{
		if (area[0] == '\0' || area[0] == '#')
			continue;

		strtolower (area);

		if (strncmp (area, "do_relay_service", 16) == 0)
			do_relay_service++;
	}

	fclose (rc_fp);

}	/* end read_rc_file */

/*
 ****************************************************************
 *	Converte o tempo para o formato interno			*
 ****************************************************************
 */
time_t
get_date (const char *date_str)
{
	const LINE	*mp;
	const ZONE	*zp;
	char		*str, *cp;
	int		fuso = 0;
	char		sinal = 0;
	time_t		tempo;
	TM		tm;

	/*
	 *	Em primeiro lugar, copia a cadeia original
	 */
	if (date_str == NOSTR || date_str[0] == '\0')
		return (-1);

	str = alloca (strlen (date_str) + 1); strcpy (str, date_str);

	/*
	 *	Pula o dia da semana
	 */
	if ((cp = strchr (str, ',')) != NOSTR)
		str = cp + 1;

	/*
	 *	Retira o dia do mês
	 */
	if ((str = strtok (str, " \t")) == NOSTR)
		return (-1);

	tm.tm_mday = strtol (str, NOCSTR, 10);

	if (tm.tm_mday <= 0 || tm.tm_mday > 31)
		return (-1);

	/*
	 *	Retira o mês
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	for (cp = str; *cp != '\0'; cp++)
		*cp = _tolower (*cp);

	for (tm.tm_mon = 0, mp = get_date_month_table; /* abaixo */; tm.tm_mon++, mp++) 
	{
		if (mp[0][0] == '\0')
			return (-1);

		if (streq (str, mp[0]))
			break;
	}

	/*
	 *	Retira o ano
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	if ((tm.tm_year = strtol (str, NOCSTR, 10)) >= 1900)
		tm.tm_year -= 1900;

	if (tm.tm_year <= 37)
		tm.tm_year += 100;

	if (tm.tm_year < 70 || tm.tm_year > 137)
		return (-1);

	/*
	 *	Retira a hora
	 */
	if ((str = strtok (NOSTR, ":")) == NOSTR)
		return (-1);

	tm.tm_hour = strtol (str, NOCSTR, 10);

	if ((unsigned)tm.tm_hour > 23)
		return (-1);

	/*
	 *	Retira o minuto
	 */
	if ((str = strtok (NOSTR, ": \t")) == NOSTR)
		return (-1);

	tm.tm_min = strtol (str, NOCSTR, 10);

	if ((unsigned)tm.tm_min > 59)
		return (-1);

	/*
	 *	Retira o segundo
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	if (*str >= '0' && *str <= '9')
	{
		tm.tm_sec = strtol (str, NOCSTR, 10);

		if ((unsigned)tm.tm_sec > 59)
			return (-1);

		if ((str = strtok (NOSTR, " \t")) == NOSTR)
			return (-1);
	}
	else
	{
		tm.tm_sec = 0;
	}

	/*
	 *	Retira a zona em horas/minutos
	 */
	if   (*str == '+' || *str == '-')
	{
		if (*str++ == '-')
			sinal++;

		fuso = strtol (str, NOCSTR, 10);

		/* Converte de horas/minutos */

		fuso = (fuso / 100) * 60 + (fuso % 100);

		if (sinal)
			fuso = -fuso;
	}
	elif (*str >= 'A' && *str <= 'Z')
	{
		for (zp = get_date_zone_table; /* abaixo */; zp++)
		{
			if (zp->z_nm[0] == '\0')
				{ /*** fuso = 0; ***/ break; }

			if (streq (zp->z_nm, str))
				{ fuso = zp->z_zone * 60; break; }
		}
	}

	/*
	 *	Epílogo
	 */
	tm.tm_isdst = 0;	/* Horário de verão = 1 */

	tempo = mktime (&tm) - fuso * 60;	/* UT */

	return (tempo);

}	/* end get_date */

/*
 ****************************************************************
 *	Le uma linha de um arquivo (terminando por "\r\n")	*
 ****************************************************************
 */
char *
fnrgets (char *str, int n, FILE *fp)
{
	char		*cp = str;
	int		c;

	while ((c = getc (fp)) > 0)
	{
#ifdef	SAVE_IN
		putc (c, save_file_fp);
#endif	SAVE_IN
		if (c == '\n')
		{
			if (cp > str && cp[-1] == '\r')
				cp--;

			break;
		}

		if (--n > 0)
			*cp++ = c;
	}

	if (cp == str && c < 0)
		return (NOSTR);

	*cp = '\0';

	return (str);

}	/* end fnrgets */

/*
 ****************************************************************
 *	Converte uma cadeia para maiúsculas			*
 ****************************************************************
 */
char *
strtoupper (char *str)
{
	char	 *cp = str;

	if (cp == NOSTR)
		return (NOSTR);

	for (/* vazio */; *cp != '\0'; cp++)
		*cp = toupper (*cp);

	return (str);

}	/* end strtoupper */

/*
 ****************************************************************
 *	Converte uma cadeia para minúsculas			*
 ****************************************************************
 */
char *
strtolower (char *str)
{
	char	 *cp = str;

	if (cp == NOSTR)
		return (NOSTR);

	for (/* vazio */; *cp != '\0'; cp++)
		*cp = tolower (*cp);

	return (str);

}	/* end strtolower */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	FILE		*log_fp;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;
	static int	static_log_fd = 0;
	static FILE	*static_log_fp = NOFILE;

	va_start (args, format);

	/*
	 *	Procura o arquivo de "log"
	 */
	if (static_log_fd == 0)
	{
		if ((static_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
		{
			if ((static_log_fp = fdopen (static_log_fd, "w")) == NOFILE)
				static_log_fd = -1;
		}
	}

	log_fp = static_log_fp;

	/*
	 *	Processa os argumentos da cadeia
	 */
	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
	{
		fmt++;

		if (save_errno == TLOOK)
			event = t_look (global_tcp_fd);
	}

	/*
	 *	Escreve as mensagens
	 */
	if (log_fp != NOFILE)
	{
		/* Escreve a mensagem com o tempo */

		fprintf  (log_fp, pgname);

		time (&time_loc);
		fprintf (log_fp, " (%.24s): ", btime (&time_loc));

		vfprintf (log_fp, fmt, args);

		/* Escreve o tipo do erro */

		if (err == '*')
		{
			if (event != 0)
				fprintf (log_fp, " (%s)", t_strevent (global_tcp_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event ==  T_DISCONNECT)
		tcp_abort (1);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Se receber um sinal, ...				*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	tcp_abort (1);
	
}	/* end on_signal */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (int n)
{
	if (text_tmp_nm != NOSTR)
		unlink (text_tmp_nm);

	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */



#if (0)	/*******************************************************/
	/*
	 *	Tenta classifica o usuário como local/remoto
	 */
	if (host_nm == NOSTR)					/* É da forma "user" */
	{
		local = 1;				/* Se for um nome simples, "user" deve ser local */
	}
	else										/* Tem "@" */
	{
		/* Tenta achar o domínio */

		if   ((domain_nm = strchr (host_nm, '.')) != NOSTR)	/* É da forma "user@host.domain" */
		{
			domain_nm++;

			if (!streq (domain_nm, my_domain_nm))
			{
				local = 0;		/* É um outro domínio */
			}
			else				/* É o mesmo domínio */
			{
				if (streq (host_nm, my_node_nm))
				{
					local = 1;	/* É exatamento o nome deste computador */
				}
				else
				{
					if ((addr = t_node_to_addr (global_tcp_fd, host_nm, &local_addr)) == -1)
					{
						/* Se não conseguiu obter o endereço, supõe remoto */

						local = 0;
					}
					else
					{
						if (addr == local_addr)
							local = 1;
						else
							local = 0;
					}
				}
			}
		}
		else			/* É da forma "user@host" */
		{
		}
	}

	/*
	 *	É necessário obter o
	 */
#endif	/*******************************************************/





#if (0)	/*******************************************************/
	/*
	 *	Procura a identificação do usuário
	 */
	if (host_nm != NOSTR)							/* Tem "@", */
	{
		if   ((domain_nm = strchr (host_nm, '.')) != NOSTR)		/* e tem "." */
		{
			if (!streq (host_nm, my_node_nm) && !streq (host_nm, my_domain_nm))
			{
				if (forward || do_relay_service || !streq (domain_nm + 1, my_domain_nm))
					local = 0;
			}
		}
		elif (strncmp (host_nm, my_node_nm, my_node_nm_len))		/* mas não tem "." */
		{
			local = 0;
		}
	}
#endif	/*******************************************************/

