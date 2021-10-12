/*
 ****************************************************************
 *								*
 *			login.c					*
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <xti.h>

#include "ftp_s.h"
#include "../ftp.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define FILE_MODE	0666

entry STATE	server_state = S_RESET;	/* Estado atual do servidor */

entry int	data_fd;

/*
 ******	Contas & senhas *****************************************
 */
#define	PWSZ	13		/* No. de caracteres da senha */

const char	no_passwd[] = "-------------"; /* Senha nula */

entry PASSWD	bad_user = {"", "?????????????", "", 100, 100 };

#undef	RETYPE
#ifdef	RETYPE
const char	super_user_nm[] = "<superuser>"; /* Nome do Retype */
entry char	super_user_passwd[PWSZ + 1];
entry char	user_passwd[PWSZ + 1];
#endif	RETYPE

entry PASSWD	user_pw;	/* Do usuário corrente */

/*
 ******	Endereços ***********************************************
 */
entry INADDR	client_data_addr = { 0, 0, FTP_DATA_PORT };

entry INADDR	server_data_addr;	/* Para o modo passivo */

entry int	passive_mode;		/* Indica o modo passivo */

entry INADDR	serv_addr;

extern char	client_node_nm[]; /* Nome do cliente */

/*
 ******	Protótipos de funções ***********************************
 */
int	open_active_data_connection (void);
int	open_passive_data_connection (void);

/*
 ****************************************************************
 *	Processa o comando USER					*
 ****************************************************************
 */
void
do_user (int argc, const char *argv[])
{
	register const PASSWD	*pw = &user_pw;
	const char		*user_nm;

	user_nm = argv[0];

	/*
	 *	Volta a ser superusuário
	 */
	if (seteuid (0) < 0)
		error ("$*Não consegui voltar a ser SUPERUSUÁRIO");

	if (pw->pw_name != NOSTR)
	{
		error
		(	"Logout para \"%s@%s\"",
			pw->pw_name, client_node_nm
		);
	}

	server_state = S_RESET;

#ifdef	RETYPE
	/*
	 *	Procura a entrada do <superusuário>
	 */
	if ((pw = getpwnam (super_user_nm)) != NOPASSWD)
		strcpy (super_user_passwd, pw->pw_passwd);
	else
		strcpy (super_user_passwd, bad_user.pw_passwd);
#endif	RETYPE

	/*
	 *	Procura a entrada do usuário e copia as entradas relevantes
	 */
	if ((pw = getpwnam (user_nm)) == NOPASSWD)
		pw = &bad_user;

	user_pw.pw_name   = strdup (pw->pw_name);
	user_pw.pw_passwd = strdup (pw->pw_passwd);
	user_pw.pw_uid    = pw->pw_uid;
	user_pw.pw_gid    = pw->pw_gid;

	if   (pw->pw_dir == NOSTR || pw->pw_dir[0] == '\0')
		user_pw.pw_dir = "/";
	else
		user_pw.pw_dir = strdup (pw->pw_dir);

	if
	(	user_pw.pw_name	  == NOSTR ||
		user_pw.pw_passwd == NOSTR ||
		user_pw.pw_dir	  == NOSTR
	)
		error ("$Memória esgotada");

	pw = &user_pw;		/* Passa a usar a cópia */

	/*
	 *	Verifica se precisa de senha ou senha do <superusuário>
	 *
	 *	Repare que para o superusuário, se não tiver a
	 *	1a. senha, pede a segunda como se fosse a primeira
	 */
	if (streq (pw->pw_passwd, no_passwd))
	{
#ifdef	RETYPE
		if (pw->pw_uid != 0 || streq (super_user_passwd, no_passwd))
			{ finish_login (); return; }

		strcpy (user_passwd, super_user_passwd);
		pw->pw_passwd = user_passwd;

		strcpy (super_user_passwd, no_passwd);
#else
		{ finish_login (); return; }
#endif	RETYPE
	}

	reply (331, "Necessito de senha para o usuário \"%s\"", user_nm);

	server_state = S_USER;

}	/* end do_user */

/*
 ****************************************************************
 *	Processa o comando PASS					*
 ****************************************************************
 */
void
do_pass (int argc, const char *argv[])
{
	register const PASSWD	*pw = &user_pw;

	if (server_state != S_USER)
	{
		reply (503, "Seqüência errônea de comandos");
		return;
	}

	if (!streq (crypt (argv[0], pw->pw_passwd), pw->pw_passwd))
	{
		reply (530, "Senha incorreta");
		server_state = S_RESET;
		return;
	}

#ifdef	RETYPE
	if (pw->pw_uid != 0 || streq (super_user_passwd, no_passwd))
		{ finish_login (); return; }

	reply (332, "Repita a senha do usuário \"%s\"", pw->pw_name);

	server_state = S_PASS;
#else
	{ finish_login (); return; }
#endif	RETYPE

}	/* end do_pass */

/*
 ****************************************************************
 *	Processa o comando ACCT					*
 ****************************************************************
 */
void
do_acct (int argc, const char *argv[])
{
#ifdef	RETYPE
	if (server_state != S_PASS)
	{
		reply (503, "Seqüência errônea de comandos");
		return;
	}

	if (!streq (crypt (argv[0], super_user_passwd), super_user_passwd))
	{
		reply (530, "Senha incorreta");
		server_state = S_RESET;
		return;
	}

	finish_login ();

	server_state = S_READY;
#endif	RETYPE

}	/* end do_acct */

/*
 ****************************************************************
 *	Detalhes finais do "login"				*
 ****************************************************************
 */
void
finish_login (void)
{
	register const PASSWD	*pw = &user_pw;

	setgid  (pw->pw_gid);
	seteuid (pw->pw_uid);

	if (chdir (pw->pw_dir) < 0)
	{
		chdir ("/");

		reply
		(	231,
			"Usuário \"%s\" em sessão "
			"(diretório HOME \"%s\" não disponível)",
			pw->pw_name, pw->pw_dir
		);
	}
	else
	{
		reply (230, "Usuário \"%s\" em sessão", pw->pw_name);
	}

	error
	(	"Login para \"%s@%s\"",
		pw->pw_name, client_node_nm
	);

	endpwent ();

	server_state = S_READY;

}	/* end finish_login */

/*
 ****************************************************************
 *	Comando PASV						*
 ****************************************************************
 */
void
do_pasv (int argc, const char *argv[])
{
	T_BIND		bind;
	const char	*ap, *dp;
	INADDR		remote_data_addr;
	T_CALL		call;

	if (argc != 0)
	{
		reply (501, "No. incorreto de argumentos: %d", argc);
		return;
	}

	if ((data_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		reply
		(	425,
			"Não pude abrir a conexão de dados (%s)",
			strerror (errno)
		);
		return;
	}

	server_data_addr.a_port = 0;

	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &server_data_addr;

	bind.qlen = 1;

	if (t_bind (data_fd, &bind, &bind) < 0)
	{
		reply
		(	425,
			"Não consegui atribuir um endereço para a conexão de dados (%s)",
			strerror (errno)
		);
		t_close (data_fd);
		return;
	}

	/*
	 *	Informa a porta alocada
	 */
	ap = (char *)&server_cntl_addr[0].a_addr;
	dp = (char *)&server_data_addr.a_port;

	reply
	(	227,
		"Entrando no modo passivo (%u,%u,%u,%u,%u,%u)",
#ifdef	LITTLE_ENDIAN
		ap[3], ap[2], ap[1], ap[0],
		dp[1], dp[0]
#else
		ap[0], ap[1], ap[2], ap[3],
		dp[0], dp[1]
#endif	LITTLE_ENDIAN
	);

	/*
	 *	Espera o "connect"
	 */
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &remote_data_addr;

	if (t_listen (data_fd, &call) < 0)
	{
		reply
		(	425,
			"Erro no \"listen\" para a conexão de dados (%s)",
			strerror (errno)
		);
		t_close (data_fd);
		return;
	}

	/*
	 *	Aceita a conexão
	 */
	if (t_accept (data_fd, data_fd, &call) < 0)
	{
		reply
		(	425,
			"Erro no \"accept\" para a conexão de dados (%s)",
			strerror (errno)
		);
		t_close (data_fd);
		return;
	}

	passive_mode = 1;

}	/* end do_pasv */

/*
 ****************************************************************
 *	Comando PORT						*
 ****************************************************************
 */
void
do_port (int argc, const char *argv[])
{
	register char	*cp, c = ',';
	register int	i, n;
	const char	*str;

	if (argc != 1)
	{
		reply (501, "No. incorreto de argumentos: %d", argc);
		return;
	}

	/*
	 *	Retira o endereço
	 */
	str = argv[0];

	cp = (char *)&client_data_addr.a_addr;

#ifdef	LITTLE_ENDIAN
	cp += 4;
#endif	LITTLE_ENDIAN

	for (i = 0; i < 4; i++)
	{
		if ((n = strtol (str, &str, 0)) & ~0xFF || *str != c)
			goto bad;

#ifdef	LITTLE_ENDIAN
		*--cp = n; 	str++;
#else
		*cp++ = n; 	str++;
#endif	LITTLE_ENDIAN
	}

	/*
	 *	Retira o "port"
	 */
	cp = (char *)&client_data_addr.a_port;

#ifdef	LITTLE_ENDIAN
	cp += 2;
#endif	LITTLE_ENDIAN

	for (/* i = 4 */; i < 6; i++)
	{
		if (i == 5)
			c = '\0';

		if ((n = strtol (str, &str, 0)) & ~0xFF || *str != c)
			goto bad;

#ifdef	LITTLE_ENDIAN
		*--cp = n; 	str++;
#else
		*cp++ = n; 	str++;
#endif	LITTLE_ENDIAN
	}

	reply
	(	200,
		"PORT: comando completado para %s, %d",
		t_addr_to_str (client_data_addr.a_addr),
		client_data_addr.a_port
	);

	return;

	/*
	 *	Em caso de erro
	 */
    bad:
	reply
	(	501,
		"PORT: endereço ou \"port\" inválido: \"%s\"",
		argv[0]
	);

}	/* end do_port */

/*
 ****************************************************************
 *	Comando RETR						*
 ****************************************************************
 */
void
do_retr (int argc, const char *argv[])
{
	register const char	*remote_file_nm;
	register FILE		*file_fp, *data_fp;
	register int		file_fd;
	register int		c, n;
	char			buf[BLSZ];
	STAT			s;
	char			err = 0;

	if (check_ready_state () < 0)
		return;

	remote_file_nm = argv[0];

	/*
	 ******	Processa o tipo "ASCII" *************************
	 */
	if (type == TYPE_A)
	{
		if ((file_fp = fopen (remote_file_nm, "r")) == NOFILE)
		{
			reply
			(	550,
				"Não consegui abrir o arquivo \"%s\" (%s)",
				remote_file_nm, strerror (errno)
			);
			return;
		}

		fstat (fileno (file_fp), &s);

		if (open_data_connection () < 0)
			return;

		if ((data_fp = fdopen (data_fd, "w")) == NOFILE)
		{
			reply
			(	425,
				"Não consegui associar um FILE à conexão de dados (%s)",
				strerror (errno)
			);

			fclose (file_fp);

			t_snddis (data_fd, (T_CALL *)NULL);
			t_close (data_fd);

			return;
		}

		reply
		(	150, "Abrindo conexão de dados ISO (ASCII) para \"%s\" (%ld bytes)",
			remote_file_nm, s.st_size
		);

		while ((c = getc (file_fp)) >= 0)	/* not EOF */
		{
			if (c == '\n')
				putc ('\r', data_fp);

			putc (c, data_fp);
		}

		fflush (data_fp); 	t_push (data_fd);

		if (ferror (file_fp))
			error ("*Erro na leitura do arquivo \"%s\"", remote_file_nm);

		fclose (file_fp);

		if (ferror (data_fp))
			error ("*Erro na escrita da conexão de dados");

		if   (t_sndrel (data_fd) < 0)
		{
			error ("*Não consegui enviar a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}
		elif (t_rcvrel (data_fd) < 0)
		{
			error ("*Não consegui receber a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}

		fclose (data_fp);

		reply (226, "Transferência completa");

		return;

	}	/* end ASCII */

	/*
	 ******	Processa o tipo "IMAGEM" ************************
	 */
	if (type == TYPE_I)
	{
		if ((file_fd = open (remote_file_nm, O_RDONLY)) < 0)
		{
			reply
			(	550,
				"Não consegui abrir o arquivo \"%s\" (%s)",
				remote_file_nm, strerror (errno)
			);
			return;
		}

		fstat (file_fd, &s);

		if (open_data_connection () < 0)
			return;

		reply
		(	150, "Abrindo conexão de dados IMAGEM para \"%s\" (%ld bytes)",
			remote_file_nm, s.st_size
		);

		while ((n = read (file_fd, buf, sizeof (buf))) > 0)
		{
			if (write (data_fd, buf, n) != n)
			{
				error ("*Erro na escrita da conexão de dados");
				err++; break;
			}
		}

		t_push (data_fd);

		if (n < 0)
			error ("*Erro na leitura do arquivo \"%s\"", remote_file_nm);

		close (file_fd);

		if   (t_sndrel (data_fd) < 0)
		{
			error ("*Não consegui enviar a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}
		elif (t_rcvrel (data_fd) < 0)
		{
			error ("*Não consegui receber a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}

		t_close (data_fd);

		if (err)
			reply (554, "Erro na escrita da conexão de dados");
		else
			reply (226, "Transferência completa");

		return;

	}	/* end IMAGEM */

	/*
	 *	No momento só suportado ASCII e IMAGEM
	 */
	error ("Tipo não suportado de dados");

}	/* end do_retr */

/*
 ****************************************************************
 *	Comando STORE						*
 ****************************************************************
 */
void
do_stor (int argc, const char *argv[])
{
	register const char	*remote_file_nm;
	register FILE		*file_fp, *data_fp;
	register int		file_fd;
	register int		c, c_next, n;
	char			buf[BLSZ];
	STAT			s;
	int			file_err = 0;

	if (check_ready_state () < 0)
		return;

	remote_file_nm = argv[0];

	if (stat (remote_file_nm, &s) >= 0 && !S_ISREG (s.st_mode))
	{
		if (unlink (remote_file_nm) < 0)
		{
			reply
			(	550,
				"O arquivo \"%s\" não é regular, e não consegui remove-lo (%s)",
				remote_file_nm, strerror (errno)
			);
			return;
		}
	}

	/*
	 ******	Processa o tipo "ASCII" *************************
	 */
	if (type == TYPE_A)
	{
		if ((file_fp = fopen (remote_file_nm, "w")) == NOFILE)
		{
			reply
			(	550,
				"Não consegui criar o arquivo \"%s\" (%s)",
				remote_file_nm, strerror (errno)
			);
			return;
		}

		if (open_data_connection () < 0)
			return;

		if ((data_fp = fdopen (data_fd, "r")) == NOFILE)
		{
			reply
			(	550,
				"Não consegui associar um FILE à conexão de dados"
			);

			fclose (file_fp);

			t_snddis (data_fd, (T_CALL *)NULL);
			t_close (data_fd);

			return;
		}

		reply
		(	150, "Abrindo conexão de dados ISO (ASCII) para \"%s\"",
			remote_file_nm
		);

		while ((c = getc (data_fp)) >= 0)	/* not EOF */
		{
			if (c == '\r')
			{
				if ((c_next = getc (data_fp)) == '\n')
					c = '\n';
				else
					ungetc (c_next, data_fp);
			}

			putc (c, file_fp);
		}

		fflush (file_fp);

		if   (ferror (file_fp))
		{
			reply
			(	554,
				"Erro na escrita do arquivo \"%s\"",
				remote_file_nm
			);
		}
		elif (ferror (data_fp))
		{
			reply
			(	554,
				"Erro na leitura da conexão de dados"
			);
		}

		fclose (file_fp);

		if   (t_sndrel (data_fd) < 0)
		{
			error ("*Não consegui enviar a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}
		elif (t_rcvrel (data_fd) < 0)
		{
			error ("*Não consegui receber a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}

		fclose (data_fp);

		reply (226, "Transferência completa.");

		return;

	}	/* end ASCII */

	/*
	 ******	Processa o tipo "IMAGE" *************************
	 */
	if (type == TYPE_I)
	{
		if ((file_fd = open (remote_file_nm, O_WRONLY|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
		{
			reply
			(	550,
				"Não consegui criar o arquivo \"%s\" (%s)",
				remote_file_nm, strerror (errno)
			);
			return;
		}

		if (open_data_connection () < 0)
			return;

		reply
		(	150, "Abrindo conexão de dados IMAGEM para \"%s\"",
			remote_file_nm
		);

		while ((n = read (data_fd, buf, sizeof (buf))) > 0)
		{
			if (write (file_fd, buf, n) != n)
				file_err++;
		}

		if   (file_err)
		{
			reply
			(	554,
				"Erro na escrita do arquivo \"%s\"",
				remote_file_nm
			);
		}
		elif (n < 0)
		{
			reply
			(	554,
				"Erro na leitura da conexão de dados"
			);
		}

		close (file_fd);

		if   (t_sndrel (data_fd) < 0)
		{
			error ("*Não consegui enviar a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}
		elif (t_rcvrel (data_fd) < 0)
		{
			error ("*Não consegui receber a desconexão ordenada DATA");
			t_snddis (data_fd, (T_CALL*)NULL);
		}

		close (data_fd);

		reply (226, "Transferência completa.");

		return;

	}	/* end IMAGE */

}	/* end do_stor */

/*
 ****************************************************************
 *	Prepara a conexão de dados				*
 ****************************************************************
 */
int
open_data_connection (void)
{
	if (passive_mode)
		{ passive_mode = 0; return (0); } /* Já foi feito no PASV */
	else
		return (open_active_data_connection ());

}	/* end open_data_connection */

/*
 ****************************************************************
 *	Prepara a conexão de dados ativa			*
 ****************************************************************
 */
int
open_active_data_connection (void)
{
	T_CALL		call;
	T_BIND		bind;
	INADDR		serv_data_addr;

	if ((data_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		reply
		(	425,
			"Não pude abrir a conexão de dados (%s)",
			strerror (errno)
		);
		return (-1);
	}

	serv_data_addr.a_addr = serv_addr.a_addr;
	serv_data_addr.a_port = 0;

	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &serv_data_addr;

	bind.qlen = 0;

	if (t_bind (data_fd, &bind, &bind) < 0)
	{
		reply
		(	425,
			"Não consegui atribuir um endereço para a conexão de dados (%s)",
			strerror (errno)
		);
		t_close (data_fd);
		return (-1);
	}

	call.addr.len	 = sizeof (INADDR);
   /***	call.addr.maxlen = sizeof (INADDR); ***/
	call.addr.buf	 = &client_data_addr;

	if (t_connect (data_fd, &call, (T_CALL *)NULL) < 0)
	{
		if (errno == TLOOK && t_look (data_fd) == T_DISCONNECT)
		{
			reply
			(	425,
				"A conexão de dados foi recusada"
			);
		}
		else
		{
			reply
			(	425,
				"Erro no \"connect\" para a conexão de dados (%s)",
				strerror (errno)
			);
		}

		t_close (data_fd);
		return (-1);
	}

	return (0);

}	/* end  open_active_data_connection */
