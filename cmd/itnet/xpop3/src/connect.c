/*
 ****************************************************************
 *								*
 *			connect.c				*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 07.09.02				*
 *		4.4.0, de 09.05.03				*
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

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <xti.h>
#include <errno.h>

#include "../h/xpop3.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	POP3_PORT	110

#define DIR_MODE 0700

#define	MAILBOX_PREFIX	"/var/mail/"

entry int		connection_state;	/* Estado da conexão */

entry char		remote_node_nm[64];	/* Nome do <nó remoto> */

entry char		remote_user_nm[32];	/* Nome do <usuário remoto> */

const char		tcp_dev_nm[] = "/dev/itntcp";

entry char		local_user_nm[32],	/* Nome do usuário local */
			old_local_user_nm[32];

entry int		local_user_uid;		/* UID do usuário local */
entry int		local_user_gid;		/* GID do usuário local */
entry const char	*local_user_pwd;	/* Senha do usuário local */

entry char		mailbox_nm[128];
entry char		*mailbox_ptr;

const char		status_nm[] = "status";

entry FILE		*local_status_fp;
entry int		mailbox_dev;		/* Dispositivo da caixa de correio */
entry int		mailbox_ino;		/* Nó-índice da caixa de correio */
entry time_t		mailbox_time;		/* Última modificação da caixa de correio */

entry int		tcp_fd = -1;		/* Descritor da linha de controle */

entry FILE		*tcp_fp;		/* FILE da linha de controle */

entry INADDR		local_tcp_addr;		/* Endereço local de controle */

entry char		local_node_nm[80];	/* Nome do nó local */

entry int		snd_dis;		/* Em caso de erro, desconecta */

entry char		first_reply_line[128];

entry char		*tcp_read_ptr,		/* Da leitura TCP */
			*tcp_read_end;

entry char		passwd_str[32];

/*
 ****************************************************************
 *	Obtém a caixa postal inicial				*
 ****************************************************************
 */
void
get_first_mailbox_nm (const char *env)
{
	const char	*user_nm;

	if (env == NOSTR)
		env = getenv ("XPOP3");

	if (env == NOSTR)
		return;

	if (strchr (env, '@') != NOSTR)
		{ strncpy (remote_mail_box_nm, env, sizeof (remote_mail_box_nm) - 1); return; }

	if ((user_nm = getenv ("USER")) == NOSTR)
		user_nm = getlogin ();

	if (user_nm == NOSTR)
		user_nm = (getpwuid (getuid ()))->pw_name;

	if (user_nm == NOSTR)
		return;

	snprintf
	(	remote_mail_box_nm, sizeof (remote_mail_box_nm), "%s@%s",
		user_nm, env
	);

}	/* end get_first_mailbox_nm */

/*
 ****************************************************************
 *	Conecta e processa conta/senha				*
 ****************************************************************
 */
void
do_connect_and_login (int unused)
{
	const PASSWD	*pw;

	/*
	 *	Verifica se já está connectado
	 */
	switch (connection_state)
	{
	    case DISCONNECTED:
		connection_state = INCONNECTION;
		break;

	    case INCONNECTION:
		return;

	    case CONNECTED:
		xerror ("Já está conectado com \"%s\"", remote_node_nm);
		return;

	    case LOGGED:
		xerror ("Já está em sessão com \"%s\"", remote_mail_box_nm);
		return;
	}

	/*
	 ******	Processa a caixa postal local ***************************
	 */
	if (local_status_fp != NOFILE)
	{
		if (streq (local_user_nm, old_local_user_nm))
			{ get_remote_and_connect (); return; }
#if (0)	/*******************************************************/
			{ xerror ("DIRETO 1"); get_remote_and_connect (); return; }
#endif	/*******************************************************/

		fclose (local_status_fp); local_status_fp = NOFILE;
	}

	/*
	 *	Examina o nome local
	 */
	if (local_user_nm[0] == '\0')
		{ xerror ("Não tenho o nome do usuário local"); return; }

	if ((pw = getpwnam (local_user_nm)) == NOPASSWD)
		{ xerror ("Usuário local \"%s\" desconhecido", local_user_nm); return; }

	local_user_uid = pw->pw_uid;
	local_user_gid = pw->pw_gid;
	local_user_pwd = pw->pw_passwd;

	/*
	 *	Verifica se é o mesmo do programa em execução
	 */
	xerror ("Caixa postal local: \"%s\"", local_user_nm);

	if (geteuid () == pw->pw_uid)
		{ open_local_status_file (); return; }
#if (0)	/*******************************************************/
		{ xerror ("DIRETO 2"); open_local_status_file (); return; }
#endif	/*******************************************************/

	/*
	 *	É necessário trocar de usuário
	 */
	if (geteuid () != 0)
		{ xerror ("NÃO posso trocar de usuário local pois NÃO sou superusuário"); return; }

	xerror ("Senha: ");

	prepare_to_read_passwd (check_local_passwd);

}	/* end do_connect_and_login */

/*
 ****************************************************************
 *	Confere a senha do novo usuário local			*
 ****************************************************************
 */
void
check_local_passwd (void)
{
	/*
	 *	Confere a senha local
	 */
#if (0)	/*******************************************************/
	xerror ("Obtive a senha \"%s\", enc = \"%s\"", passwd_str, local_user_pwd);
#endif	/*******************************************************/

	if (!streq (crypt (passwd_str, local_user_pwd), local_user_pwd))
	{
		xerror ("Senha inválida");

		xerror ("Senha: ");

		prepare_to_read_passwd (check_local_passwd);

		return;
	}

	/*
	 *	Troca o UID do usuário local
	 */
	if (setruid (local_user_uid) < 0)
		{ xerror ("*Não consegui trocar para o usuário \"%s\"", local_user_nm); return; }

	/*
	 *	Abre o arquivo de índice "status"
	 */
	open_local_status_file ();

}	/* end check_local_passwd */

/*
 ****************************************************************
 *	Atualiza o nome do usuário remoto			*
 ****************************************************************
 */
void
update_remote_user_nm (void)
{
	const char	*node;
	int		len = strlen (local_user_nm);

	/*
	 *	Atualiza também o nome remoto
	 */
	if ((node = strchr (remote_mail_box_nm, '@')) != NOSTR)
		memmove (remote_mail_box_nm + len, node, sizeof (remote_mail_box_nm) - len);
	else
		remote_mail_box_nm[len] = '\0';

	memmove (remote_mail_box_nm, local_user_nm, len);

}	/* end update_remote_user_nm */

/*
 ****************************************************************
 *	Abre o arquivo de índice "status"			*
 ****************************************************************
 */
void
open_local_status_file (void)
{
	STAT		s;

	strcpy (mailbox_nm, MAILBOX_PREFIX); strcat (mailbox_nm, local_user_nm);

	if (stat (mailbox_nm, &s) < 0)
	{
		if (errno != ENOENT)
			{ xerror ("*Não consegui abrir a caixa postal \"%s\"", mailbox_nm); return; }

		if (mkdir (mailbox_nm, DIR_MODE) < 0)
			{ xerror ("*Não consegui criar a caixa postal \"%s\"", mailbox_nm); return; }

		if (stat (mailbox_nm, &s) < 0)
			{ xerror ("*Não consegui abrir a caixa postal \"%s\"", mailbox_nm); return; }
	}

	if
	(	(s.st_uid != local_user_uid || s.st_gid != local_user_gid) &&
		chown (mailbox_nm, local_user_uid, local_user_gid) < 0
	)
	{
		xerror ("*Não consegui modificar o dono/grupo da caixa postal \"%s\"", mailbox_nm);
		return;
	}

	if (!S_ISDIR (s.st_mode))
		{ xerror ("A caixa postal \"%s\" NÃO é um diretório", mailbox_nm); return; }

#if (0)	/*******************************************************/
	if ((mailbox_fp = inopendir (s.st_dev, s.st_ino)) == NODIR)
		{ xerror ("*Não consegui abrir a caixa postal \"%s\"", mailbox_nm); return; }
#endif	/*******************************************************/

	strcat (mailbox_nm, "/"); mailbox_ptr = strend (mailbox_nm);

	mailbox_dev  = s.st_dev;
	mailbox_ino  = s.st_ino;
	mailbox_time = s.st_mtime;

	/*
	 *	Abre o arquivo de índices "status"
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((local_status_fp = fopen (mailbox_nm, "r+")) == NOFILE)
		{ xerror ("*Não consegui abrir o arquivo \"%s\"", mailbox_nm); return; }

	fprintf (local_status_fp, "R\n");

	strcpy (old_local_user_nm, local_user_nm);

	get_remote_and_connect ();

}	/* end open_local_status_file */

/*
 ****************************************************************
 *	Obtém os nomes remotos e conecta			*
 ****************************************************************
 */
void
get_remote_and_connect (void)
{
	const char	*str;
	char		*node;
	INADDR		serv_addr, remote_addr, bind_addr;
	T_BIND		bind;
	T_CALL		snd_call, rcv_call;
	const char	*cp;
	int		index, fd_vec[2];

	str = remote_mail_box_nm;

	if (str[0] == '\0')
		{ xerror ("NÃO foi dado o nome da caixa postal remota"); return; }

	if ((node = strchr (str, '@')) == NOSTR)
		{ xerror ("Faltando o \"@\" do nome da caixa postal remota"); return; }

	node[0] = '\0'; strncpy (remote_user_nm, str, sizeof (remote_user_nm) - 1); node[0] = '@';

	strncpy (remote_node_nm, node + 1, sizeof (remote_node_nm) - 1);

	xerror ("Conectando com \"%s@%s\"", remote_user_nm, remote_node_nm);

	/*
	 *	Abre o dispositivo TCP de contrôle
	 */
	if
	(	(tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0 ||
		(tcp_fp = fdopen (tcp_fd, "r")) == NOFILE
	)
		{ xerror ("*Não consegui abrir \"%s\"", tcp_dev_nm); return; }

	/*
	 *	Obtém o endereço nó remoto
	 */
	if ((remote_addr.a_addr = t_node_to_addr (tcp_fd, remote_node_nm, &local_tcp_addr.a_addr)) == -1)
		{ xerror ("*Não consegui obter o endereço de \"%s\"", remote_node_nm); goto bad; }

	remote_addr.a_port = POP3_PORT;

	/*
	 *	Obtém o nome do nó local
	 */
	if ((cp = t_addr_to_node (tcp_fd, local_tcp_addr.a_addr)) == NOSTR)
		{ xerror ("Não consegui obter o nome do nó local"); goto bad; }

	strcpy (local_node_nm, cp);

	/*
	 *	Associa um endereço local
	 */
	bind_addr.a_addr = 0;

	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &bind_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, &bind) < 0)
		{ xerror ("*Não consegui associar um endereço local"); goto bad; }

	/*
	 *	Executa o "connect"
	 */
	snd_call.addr.len    = sizeof (INADDR);
	snd_call.addr.buf    = &remote_addr;

	rcv_call.addr.maxlen = sizeof (INADDR);
	rcv_call.addr.buf    = &serv_addr;

	if (fcntl (tcp_fd, F_SETFL, O_NONBLOCK) < 0)
		xerror ("Não consegui alterar o TCP para assíncrono");

	if (t_connect (tcp_fd, &snd_call, &rcv_call) < 0)
	{
		if (errno != TNODATA)
			{ xerror ("*Não consegui estabelecer conexão"); goto bad; }
	}

	fd_vec[0] = tcp_fd;
	fd_vec[1] = xwin_fd;
	index	  = 1;

	for (EVER)
	{
		if ((index = attention (2, fd_vec, index, 0)) < 0)
			{ xerror ("*Não consegui estabelecer conexão"); goto bad; }

		if (index == 0)		/* TCP */
		{
			if (t_rcvconnect (tcp_fd, &rcv_call) >= 0)
				break;

			if (errno != TNODATA)
			{
				if   (errno == TLOOK && t_look (tcp_fd) == T_DISCONNECT)
					xerror ("A conexão foi recusada por \"%s\"", remote_node_nm);
				else
					xerror ("*Não consegui estabelecer conexão");

				goto bad;
			}
		}
		else			/* XWIN */
		{
			event_loop (0);
		}

	}	/* end for (EVER) */

	if (fcntl (tcp_fd, F_SETFL, 0 /* desliga O_NONBLOCK */) < 0)
		xerror ("Não consegui alterar o TCP para síncrono");

	/*
	 *	Conectamos ...
	 */
	snd_dis++;	/* De agora adiante, erros desconectam */

	connection_state = CONNECTED; draw_connection_state_button ();

#if (0)	/*******************************************************/
	xerror ("Conectado a \"%s\"", remote_node_nm);
#endif	/*******************************************************/

	if (get_reply () != OK)
		{ xerror ("NÃO recebi OK"); goto bad; }

	/*
	 *	Realiza o "login"
	 */
	if (comando (1, "USER %s", remote_user_nm) != OK)
	{
		xerror
		(	"Usuário \"%s\" desconhecido pelo servidor POP3 \"%s\"",
			remote_user_nm, remote_node_nm
		);
		goto bad;
	}

	xerror ("Senha: ");

	prepare_to_read_passwd (send_passwd);

	return;

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	connection_state = DISCONNECTED; draw_connection_state_button ();

	fclose (tcp_fp); tcp_fp = NOFILE; tcp_fd = -1;

}	/* end get_remote_and_connect */

/*
 ****************************************************************
 *	Envia a senha						*
 ****************************************************************
 */
void
send_passwd (void)
{
	if (comando (1, "PASS %s", passwd_str) == OK)
	{
#if (0)	/*******************************************************/
		xerror ("Conectado com \"%s@%s\"", remote_user_nm, remote_node_nm);
#endif	/*******************************************************/

		connection_state = LOGGED; draw_connection_state_button ();

		get_remote_UIDLs ();

		return;
	}

	xerror ("Senha inválida");

	if (comando (1, "USER %s", remote_user_nm) != OK)
	{
		xerror
		(	"Usuário \"%s\" desconhecido pelo servidor POP3 \"%s\"",
			remote_user_nm, remote_node_nm
		);
		return;
	}

	xerror ("Senha: ");

	prepare_to_read_passwd (send_passwd);

}	/* end send_passwd */

/*
 ****************************************************************
 *	Desconecta o TCP					*
 ****************************************************************
 */
void
do_disconnect (int unused)
{
	if (connection_state == DISCONNECTED)
		{ xerror ("NÃO está conectado"); return; }

	disconnection_center ();

	connection_state = DISCONNECTED; draw_connection_state_button ();

	draw_percent_percent (0, 100);

	clear_mail_window ();

	clear_scroll_window ();

	clear_mail_rcv_del_window ();

	free_remote_uidl_list ();

	xerror ("Desconectado");

}	/* end do_disconnect */

/*
 ****************************************************************
 *	Desconecta						*
 ****************************************************************
 */
void
disconnection_center (void)
{
	if (comando (1, "QUIT") != OK)
		xerror ("NÃO consegui enviar o comando QUIT");

	if (t_sndrel (tcp_fd) < 0)
		xerror ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		xerror ("$*Erro no RCVREL");

	if (tcp_fd >= 0)
		{ fclose (tcp_fp); tcp_fp = NOFILE; tcp_fd = -1; }

}	/* end disconnection_center */

/*
 ****************************************************************
 *	 Envia comandos	para o servidor				*
 ****************************************************************
 */
int
comando (int do_wait, const char *format, ...)
{
	va_list		args;
	int		r = OK;
	char		com[BLSZ];

	va_start (args, format);

	vsprintf (com, format, args);

	if (vflag)
		printf ("Enviando o comando \"%s\"\n", com);

	strcat (com, "\r\n");

	if (t_snd (tcp_fd, com, strlen (com), T_PUSH) < 0)
		error (NOSTR);

	if (do_wait)
		r = get_reply ();
	
	va_end (args);

	return (r);

}	/* end comando */

/*
 ****************************************************************
 *	Recebe uma mensagem do servidor				*
 ****************************************************************
 */
int
get_reply (void)
{
	int		code = OK;

	/*
	 *	Lê a primeira linha
	 */
	if (fnrgets_tcp (first_reply_line, sizeof (first_reply_line)) == NOSTR)
		xerror ("$*Erro na leitura de uma mensagem do servidor");

	if (vflag)
		printf ("Reply: \"%s\"\n", first_reply_line);

	if (strncmp (first_reply_line, "+OK", 3))
		code = ERR;

	return (code);

}	/* end get_reply */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Le uma linha de um arquivo (terminando por "\r\n")	*
 ****************************************************************
 */
char *
fnrgets (char *str, int n, FILE *fp)
{
	char		*cp = str;
	int		c = 0;

	while ((c = getc (fp)) >= 0)
	{
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
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Leitura de uma linha ("\r\n") da conexão TCP		*
 ****************************************************************
 */
char *
fnrgets_tcp (char *str, int n)
{
	char		*cp = str;
	int		c = 0;

	for (EVER)
	{
		if (tcp_read_ptr < tcp_read_end)
			c = *tcp_read_ptr++;
		else
			c = fnrgets_tcp_read ();

		if (c < 0)
			break;

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

}	/* end fnrgets_tcp */

/*
 ****************************************************************
 *	Obtém mais caracteres da conexão TCP			*
 ****************************************************************
 */
int
fnrgets_tcp_read (void)
{
	static char	tcp_area[256];
	int		n, index = 1;
	int		fd_vec[2];

	fd_vec[0] = tcp_fd;
	fd_vec[1] = xwin_fd;

    again:
	if ((index = attention (2, fd_vec, index, 0)) < 0)
		return (-1);

	if (index == 0)
	{
		if ((n = read (tcp_fd, tcp_area, sizeof (tcp_area))) <= 0)
			return (-1);

		tcp_read_ptr = tcp_area;
		tcp_read_end = tcp_area + n;

		return (*tcp_read_ptr++);
	}
	else
	{
		event_loop (0);
		goto again;
	}

}	/* end fnrgets_tcp_read */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
int
xerror (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	char		err, abrt;
	int		event, save_errno = errno;
	int		len;
	char		area[256];

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	len = vsnprintf (area, sizeof (area), fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (tcp_fd);
	
			snprintf (area + len, sizeof (area) - len, " (%s)", t_strevent (tcp_fd, event));
	
			if (event ==  T_DISCONNECT)
				return (-1);
		}
		elif (save_errno != 0)
		{
			snprintf (area + len, sizeof (area) - len, " (%s)", strerror (save_errno));
		}
	}

	write_msg (area);

	va_end (args);

	if (abrt == '$')
		return (-1);

	return (0);

}	/* end xerror */

