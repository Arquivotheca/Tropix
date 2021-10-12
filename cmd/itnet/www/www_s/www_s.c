/*
 ****************************************************************
 *								*
 *			www_s.c					*
 *								*
 *	Servidor WWW						*
 *								*
 *	Versão	3.0.0, de 01.11.97				*
 *		4.9.0, de 21.09.06				*
 *								*
 *	Módulo: WWW						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
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
#include <locale.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "www_s: 4.9.0, de 21.09.06";

#define	WWW_USER_NM	"www"
#define	PAGE_PREFIX	"/home/www"
#define	PAGE_POSFIX	"/index.html"

#define	EVER	;;
#define	elif	else if
#define	NOSTR	(char *)NULL

const char	forward_nm[] = "/.forward";

const char	tcp_dev_nm[] = "/dev/itntcp";

entry int	vflag = 0;	/* Verbose */

entry long	local_addr,	/* Endereços INTERNET */
		remote_addr;

entry char	local_node_nm[80];	/* Nome do próprio servidor */
entry char	client_node_nm[80];	/* Nome do cliente */

entry int	global_tcp_fd;	/* Conexão de controle */

entry FILE	*tcp_in,	/* Conexão de controle */
		*tcp_out;

entry FILE	*www_log_fp;	/* Para o nome dos arquivos enviados */

entry int	global_log_fd;	/* Para "error" */
entry FILE	*global_log_fp;

/*
 ******	Estruturas **********************************************
 */
typedef struct	get_op
{
	char	*g_file_nm;	/* Nome do arquivo */
	int	g_begin;	/* Início */
	int	g_end;		/* Final */
	char	g_close;	/* Deve fechar a conexão */
	char	g_http;		/* -1: ?, 0: 1.0, 1: 1.1 */

}	GET_OP;

/*
 ******	Tabela de "Content-type" ********************************
 */
typedef struct content
{
	char	t_suffix[6];	/* Sufixo */
	char	*t_type;	/* O "Content-type" */

}	CONTENT;

const CONTENT	content[] =
{
	"aif",		"audio/x-aiff",
	"aiff",		"audio/x-aiff",
	"art",		"image/x-jg",
	"au",		"audio/basic",
	"z",		"application/x-compress",
	"gif",		"image/gif",
	"gz",		"application/x-gzip",
	"htm",		"text/html",
	"html",		"text/html",
	"pcx",		"image/pcx",
	"jpg",		"image/jpeg",
	"jpeg",		"image/jpeg",
	"jpe",		"image/jpeg",
	"jfif",		"image/jpeg",
	"pjpeg",	"image/jpeg",
	"pjp",		"image/jpeg",
	"hqx",		"application/mac-binhex40",
	"xls",		"application/vnd.ms-excel",
	"ppt",		"application/vnd.ms-powerpoint",
	"doc",		"application/vnd.ms-msword",
	"dot",		"application/vnd.ms-word",
	"txt",		"text/plain",
	"text",		"text/plain",
	"pdf",		"application/pdf",
	"ai",		"application/postscript",
	"eps",		"application/postscript",
	"ps",		"application/postscript",
	"mov",		"video/quicktime",
	"rtf",		"application/rtf",
	"rmi",		"audio/midi",
	"mid",		"audio/midi",
	"tar",		"application/x-tar",
	"avi",		"video/avi",
	"wrl",		"x-worlds/x-vrml",
	"wrz",		"x-worlds/x-vrml",
	"wav",		"audio/x-wav",
	"mp2",		"audio/mpeg",
	"mp3",		"audio/mpeg",
	"smi",		"application/smil",
	"rm",		"application/vnd.rn-realmedia",
	"ra",		"audio/vnd.rn-realaudio",
	"rmm",		"audio/x-pn-realaudio",
	"ram",		"audio/x-pn-realaudio",
	"bmp",		"image/x-MS-bmp",
	"rf",		"image/vnd.rn-realflash",
	"rv",		"video/vnd.rn-realvideo",
	"zip",		"application/x-zip-compressed",
	"lha",		"application/zip",
	"arj",		"application/zip",
	"arc",		"application/zip",
	"tar",		"application/zip",
	"tgz",		"application/zip",
	"gz",		"application/zip",
	"xbm",		"application/x-bitmap",
	"exe",		"application/octet-stream",
	"com",		"application/octet-stream",
	""
};

/*
 ******	Variáveis globais ***************************************
 */
entry UTSNAME	uts;		/* Para tirar uma série de informações */

/*
 ******	Protótipos **********************************************
 */
int		www_get_cmd (GET_OP *);
int		www_read_line (char *, int);
int		www_send_file (GET_OP *);
void		www_send_msg (int, const char *, const char *, ...);
char		*utf2iso (char *str);
int		hexa2iso (const char *num);
const char	*str_to_upper (const char *);
char		*edit_date (char *, int);
void		on_sigterm (int, void *, void *);
void		log_msg (const char *, ...);
void		error (const char *, ...);
void		tcp_abort (int);

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	char		file_nm[BLSZ];
	int		fd, tcp_fd;
	int		opt;
	const char	*str;
	INADDR		client_addr;
	INADDR		serv_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		listen_port, listen_fd, sequence;
	GET_OP		op;

	/*
	 *	Usa o inglês para datas
	 */
	setlocale (LC_ALL, "E");

	/*
	 *	Prepara variáveis de "error" ainda como superusuário
	 */
	if ((global_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
	{
		if ((global_log_fp = fdopen (global_log_fd, "w")) == NOFILE)
			global_log_fd = -1;
	}

	/*
	 *	Abre o "log" para coletar o envio de cartas
	 */
	if ((fd = open ("/var/adm/wwwlog", O_WRONLY|O_APPEND)) >= 0)
		www_log_fp = fdopen (fd, "w");
	else
		www_log_fp = NOFILE;

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
		error ("$*Não consegui obter o UTS\n");

	/*
	 *  www_s [-v] <listen_port> <listen_fd> <client_addr> <client_port> <sequence>
	 */
#undef	DEBUG
#ifdef	DEBUG
	if (Bflag) error
	(	"%s <%s> <%s> <%s> <%s> <%s>",
		vflag ? "-v" : "",
		argv[0], argv[1], argv[2], argv[3], argv[4]
	);
#endif	DEBUG

	if (argc != 5)
		error ("$Número inválido de argumentos: %d", argc);

	listen_port	   = strtol (argv[0], &str, 0);
	listen_fd	   = strtol (argv[1], &str, 0);
	client_addr.a_addr = t_str_to_addr (argv[2]);
	client_addr.a_port = strtol (argv[3], &str, 0);
	sequence	   = strtol (argv[4], &str, 0);

	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	global_tcp_fd = tcp_fd;

	if ((tcp_in = fdopen (tcp_fd, "r")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	if ((tcp_out = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	t_node_to_addr (tcp_fd, argv[2], &local_addr);

	remote_addr = client_addr.a_addr;

	if ((str = t_addr_to_node (tcp_fd, remote_addr)) == NOSTR)
		str = t_addr_to_str (remote_addr);

	strcpy (client_node_nm, str);

	if ((str = t_addr_to_node (tcp_fd, local_addr)) == NOSTR)
		error ("$*Não consegui obter o nome do nó local");

	strcpy (local_node_nm, str);

#ifdef	DEBUG
	error ("local_addr = %s, ", t_addr_to_str (local_addr));
	error ("remote_addr = %s\n", t_addr_to_str (remote_addr));
#endif	DEBUG

	/*
	 *	Vou realizar o "bind" (usa o mesmo port do WWW)
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

	/*
	 *	Agora não precisa mais do "listen_fd"
	 */
	t_close (listen_fd);

#ifdef	DEBUG
	error ("Iniciando para \"%s\"", client_node_nm);
#endif	DEBUG

	/*
	 ******	Obtém um comando da linha ***********************
	 */
	op.g_file_nm = file_nm;

	for (EVER)
	{
		if (www_get_cmd (&op) < 0)
			break;

		www_send_file (&op);

		if (op.g_close)
			break;

	}	/* end for (EVER) */

	/*
	 *	Final do programa
	 */
#ifdef	DEBUG
	error ("Finalisando");
#endif	DEBUG

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Errno no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Errno no RCVREL");

#ifdef	DEBUG
	error ("Terminando para \"%s\"", client_node_nm);
#endif	DEBUG

	return (0);

}	/* end www_s */

/*
 ****************************************************************
 *	Lê uma mensagem completa do cliente (só GET)		*
 ****************************************************************
 */
int
www_get_cmd (GET_OP *gp)
{
	char		area[BLSZ];
	int		n;
	const char	*str;
	const char	*cp, *cmd_nm;

	/*
	 *	Exemplo de mensagem:
	 *
	 *	GET /tropix/tropix.tgz HTTP/1.1
	 *	Host: jupiter.nce.ufrj.br
	 *	Range: bytes=2631506-
	 *	User-Agent: GetRight/4.2
	 *	Accept: *.*
	 */

	/*
	 *	Inicialização
	 */
	gp->g_file_nm[0] = '\0';
	gp->g_begin	 = -1;
	gp->g_end	 = -1;
	gp->g_close	 = 1;

	/*
	 *	Lê várias linhas
	 */
	for (EVER)
	{
		if ((n = www_read_line (area, sizeof (area))) < 0)
			return (-1);

		if (n == 0)
		{
			if (gp->g_file_nm[0] != '\0')
				return (0);
			else
				return (-1);
		}
#ifdef	DEBUG
		log_msg ("Obtive o comando \"%s\"", area);
#endif	DEBUG
		if ((cmd_nm = strtok (area, " \t")) == NOSTR)
			continue;

		str_to_upper (cmd_nm);

		if   (streq (cmd_nm, "GET"))
		{
			if ((cp = strtok (NOSTR, " \t")) == NOSTR)
				return (-1);

			strcpy (gp->g_file_nm, cp);

			gp->g_http = -1;

			if ((cp = strtok (NOSTR, " \t")) != NOSTR)
			{
				if   (streq (cp, "HTTP/1.0"))
				{
					gp->g_http  = 0;
				}
				elif (streq (cp, "HTTP/1.1"))
				{
					gp->g_close = 0;
					gp->g_http  = 1;
				}
			}
		}
		elif (streq (cmd_nm, "RANGE:"))
		{
			if ((cp = strtok (NOSTR, "=")) == NOSTR)
				continue;

			str_to_upper (cp);

			if (!streq (cp, "BYTES"))
				continue;

			if ((cp = strtok (NOSTR, " \t")) == NOSTR)
				continue;

			if (cp[0] != '-')
			{
				gp->g_begin = strtol (cp, &str, 0);

				if (str <= cp)
					continue;

				cp = (char *)str;

				if (cp[0] != '-')
					continue;
			}

			cp++;	/* Pula o "-" */

			if (cp[0] == '\0')
				continue;

			gp->g_end = strtol (cp, &str, 0) + 1;

			if (str <= cp)
				continue;
		}
		elif (streq (cmd_nm, "CONNECTION:"))
		{
			if ((cp = strtok (NOSTR, "=")) == NOSTR)
				continue;

			str_to_upper (cp);

			if (streq (cp, "CLOSE"))
				gp->g_close++;
		}

	}	/* end for (EVER) */

}	/* end www_read_msg */

/*
 ****************************************************************
 *	Lê uma linha do TCP até um <nl>				*
 ****************************************************************
 */
int
www_read_line (char *area, int count)
{
	char		*cp = area;
	const char	*end_area = cp + count - 1;
	int		c;

	/*
	 *	Retorna o número de caracteres lidos
	 */
	for (EVER)
	{
		c = getc (tcp_in);

		if (c < 0)	/* EOF ou erro */
			return (-1);

		if (c == '\r')	/* Ignora <cr> */
			continue;

		if (c != '\n')
		{
			if (cp < end_area)
				*cp++ = c;

			continue;
		}

		/* É um <nl> */

		*cp = '\0';
#ifdef	DEBUG
		error ("Li \"%s\"", area);
#endif	DEBUG
		return (cp - area);

	}	/* end for (EVER) */

}	/* end www_read_line */

/*
 ****************************************************************
 *	Envia um arquivo					*
 ****************************************************************
 */
int
www_send_file (GET_OP *gp)
{
	const UTSNAME		*up = &uts;
	const PASSWD 		*pw;
	int			fd;
	char			file_nm[BLSZ];
	char			file_nm_cp[BLSZ];
	char			moved_file_nm[BLSZ];
	char			*file_nm_ptr;
	char			*slash_ptr, *str;
	char			*error_file_nm = gp->g_file_nm;
	char			*user_nm;
	char			had_slash = 0, had_tilde = 0;
	STAT			s;
	char			area[BLSZ];
	int			save_errno;
	const CONTENT		*tp;
	char			*cp;
	char			partial = 0;
	int			count;
	char			read_error = 0;
	char			date_area[64];

#ifdef	DEBUG
	error ("Obtive o nome de arquivo \"%s\"", gp->g_file_nm);
#endif	DEBUG

	/*
	 *	Copia o nome original
	 */
	strcpy (file_nm_cp, gp->g_file_nm); file_nm_ptr = file_nm_cp;

	/*
	 *	Verifica se o nome do arquivo começa por "~"
	 */
	if (file_nm_ptr[0] == '/')
		file_nm_ptr++;

	user_nm = WWW_USER_NM;

	if   (file_nm_ptr[0] == '~')
	{
		had_tilde++;

		if ((user_nm = strtok (file_nm_ptr + 1, "/ \t")) == NOSTR)
			goto not_found;

		file_nm_ptr = strtok (NOSTR, "");
	}
	elif (file_nm_ptr[0] == '%')
	{
		if
		(	 file_nm_ptr[1] == '7' &&
			(file_nm_ptr[2] == 'E' || file_nm_ptr[2] == 'e')
		)
		{
			had_tilde++;

			if ((user_nm = strtok (file_nm_ptr + 3, "/ \t")) == NOSTR)
				goto not_found;

			file_nm_ptr = strtok (NOSTR, "");
		}
	}

	if ((pw = getpwnam (user_nm)) == NOPASSWD)
		goto not_found;

	strcpy (file_nm, pw->pw_dir);		/* "/home/www" */
	setruid (pw->pw_uid);
	setrgid (pw->pw_gid);

	endpwent ();

	/*
	 *	Analisa o nome do arquivo
	 */
	strcat (file_nm, "/");			/* "/home/www/" */

	if (file_nm_ptr != NOSTR)		/* "/home/www/tropix/" */
		strcat (file_nm, file_nm_ptr);

	slash_ptr = strend (file_nm) - 1;

	if (*slash_ptr == '/')			/* "/home/www/tropix" */
		{ *slash_ptr = '\0'; had_slash++; }

	/*
	 *	Verifica se há "%.." no nome do arquivo
	 */
#undef	NAME_DEBUG
#ifdef	NAME_DEBUG
log_msg ("Pedindo o arquivo \"%s\"", file_nm);
#endif	NAME_DEBUG

	for (cp = file_nm; (cp = strchr (cp, '%')) != NOSTR; /* abaixo */)
		cp = utf2iso (cp);

#ifdef	NAME_DEBUG
log_msg ("Passou para \"%s\"", file_nm);
fflush (www_log_fp);
#endif	NAME_DEBUG

#if (0)	/*******************************************************/
	while ((cp = strstr (file_nm, "%20")) != NOSTR)
		{ *cp = ' '; strcpy (cp + 1, cp + 3); }

#ifdef	NAME_DEBUG
log_msg ("Pedindo o arquivo \"%s\"", file_nm);
#endif	NAME_DEBUG

	while ((cp = strchr (file_nm, '%')) != NOSTR)
	{
		unsigned int	n;
		char		tmp;
		const char	*rostr;

		tmp = cp[3]; cp[3] = '\0';

		if ((n = strtol (cp + 1, &rostr, 16)) < 256 && *rostr == '\0')
			{ cp[0] = n; cp[1] = tmp; strcpy (cp + 2, cp + 4); }
		else
			cp[3] = tmp;
	}

#ifdef	NAME_DEBUG
log_msg ("Passou para \"%s\"", file_nm);
fflush (www_log_fp);
#endif	NAME_DEBUG
#endif	/*******************************************************/

	/*
	 *	Verifica se é um diretório
	 */
	s.st_mode = 0;

	if (stat (file_nm, &s) >= 0 && S_ISDIR (s.st_mode))
	{
		strcpy (moved_file_nm, gp->g_file_nm);

		/* Procura o ".forward" */

		file_nm_ptr = strend (file_nm); strcat (file_nm_ptr, forward_nm);

		if ((fd = open (file_nm, O_RDONLY)) >= 0 && read (fd, area, BLSZ) > 0)
		{
			cp = strend (area) - 1;

			if (cp >= area && cp[0] == '\n')
				cp[0] = '\0';

			strcpy (moved_file_nm, area);
			close (fd);
#undef	TROPIX
#ifdef	TROPIX
			error ("Pedido do TROPIX \"%s\"", client_node_nm);
#endif	TROPIX
			goto moved;
		}

		if (moved_file_nm[0] != '\0' && (strend (moved_file_nm))[-1] == '/')
		{
			strcat (moved_file_nm, PAGE_POSFIX + 1);
		}
		else
		{
			strcat (moved_file_nm, PAGE_POSFIX);
		}

		/*
		 *	HTTP/1.1 301 Moved Permanently
		 *	Date: Tue, 13 Oct 1998 22:36:35 GMT
		 *	Server: Apache/1.2.6
		 *	Location: http://www.geocities.com/Heartland/Woods/6144/
		 *	Connection: close
		 *	Content-Type: text/html
		 *	<HTML><HEAD>
		 *	<TITLE>301 Moved Permanently</TITLE>
		 *	</HEAD><BODY>
		 *	<H1>Moved Permanently</H1>
		 *	The document has moved
		 *		<A HREF="http://www.geocities.com/Heartland/Woods/6144/">here</A>.<P>
		 *	</BODY></HTML>
		 */
	    moved:
		fprintf
		(	tcp_out,
			"HTTP/1.1 301 Mudança de endereço\r\n"
			"Server: %s/%s (%s)\r\n"
			"Date: %s\r\n"
			"Location: http://%s%s\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: 0\r\n"
			"\r\n",

			up->uts_sysname, up->uts_nodename, pgversion,
			edit_date (date_area, sizeof (date_area)),
			moved_file_nm[0] == '/' ? local_node_nm : "",
			moved_file_nm
		);

		fflush (tcp_out);

		if (t_push (global_tcp_fd) < 0)
			error ("*Erro no T_PUSH");

		return (0);
	}

	/*
	 *	Verifica se tem ".." no nome
	 */
	if (strstr (file_nm, "/../"))
	{
		www_send_msg
		(	404,
			"Não é permitido \"..\" no nome do arquivo",
			"Não consegui abrir o arquivo \"%s\"",
			file_nm
		);

#ifdef	DEBUG
		error
		(	"Não consegui abrir o arquivo \"%s\" (%s)",
			file_nm,
			"Não é permitido \"..\" no nome do arquivo"
		);
#endif	DEBUG
		return (-1);
	}

	/*
	 *	Não esquece de verificar a proteção
	 */
	if
	(	!S_ISREG (s.st_mode) ||
		access (file_nm, R_OK) < 0 ||
		(fd = open (file_nm, O_RDONLY)) < 0
	)
	{
		error_file_nm = file_nm;

	    not_found:
		save_errno = errno;

		www_send_msg
		(	404,
			strerror (save_errno),
			"Não consegui abrir o arquivo \"%s\"",
			error_file_nm
		);

		errno = save_errno;

#ifdef	DEBUG
		error
		(	"*Não consegui abrir o arquivo \"%s\"",
			error_file_nm
		);
#endif	DEBUG
		return (-1);
	}

#ifdef	DEBUG
	error ("Abri o arquivo \"%s\"", file_nm);
#endif	DEBUG


	/*
	 *	Calcula o intervalo
	 */
	if (gp->g_begin >= 0 || gp->g_end >= 0)
	{
		if  (gp->g_begin < 0)
		{
			gp->g_begin = s.st_size - gp->g_end;
			gp->g_end   = s.st_size;
		}
		elif (gp->g_begin >= s.st_size)
		{
			gp->g_begin = 0;
		}

		if   (gp->g_end < 0)
			gp->g_end = s.st_size;
		if (gp->g_end >= s.st_size)
			gp->g_end = s.st_size;

		if (gp->g_end - gp->g_begin < s.st_size)
			partial++;
	}
	else
	{
		gp->g_begin = 0; gp->g_end = s.st_size;
	}

	/*
	 *	Exemplo de cabeçalho:
	 *
	 *		HTTP/1.1 200 Document follows
	 *		Date: Tue, 22 Sep 1998 01:18:01 GMT
	 *		Server: NCSA/1.4
	 *		Content-type: image/gif
	 *		Last-modified: Tue, 23 Jan 1996 17:02:41 GMT
	 *		Content-length: 6102
	 */
	if (!partial)
		fprintf (tcp_out, "HTTP/1.1 200 O conteúdo do arquivo segue\r\n");
	else
		fprintf (tcp_out, "HTTP/1.1 206 O conteúdo parcial do arquivo segue\r\n");

	fprintf (tcp_out, "Date: %s\r\n", edit_date (date_area, sizeof (date_area)));

	fprintf
	(	tcp_out,
		"Server: %s/%s (%s)\r\n",
		up->uts_sysname, up->uts_nodename, pgversion
	);

	fprintf (tcp_out, "Accept-Ranges: bytes\r\n");

	/*
	 *	Procura o sufixo do nome do arquivo
	 */
	if ((str = strrchr (file_nm, '.')) != NOSTR)
	{
		str++;

		for (tp = content; /* abaixo */; tp++)
		{
			if (tp->t_suffix[0] == 0)
			{
				fprintf
				(	tcp_out,
					"Content-type: text/plain\r\n"
				);
				break;
			}

			if (!streq (str, tp->t_suffix))
				continue;

			fprintf
			(	tcp_out,
				"Content-Type: %s\r\n",
				tp->t_type
			);

			break;
		}
	}

	fprintf (tcp_out, "Content-Length: %d\r\n", gp->g_end - gp->g_begin);

	if (partial)
		fprintf (tcp_out, "Content-Range: bytes %d-%d/%d\r\n", gp->g_begin, gp->g_end - 1, s.st_size);

	fprintf (tcp_out, "\r\n");

#if (0)	/*******************************************************/
		"Accept-Ranges: bytes\r\n"
#endif	/*******************************************************/

	fflush (tcp_out);

	/*
	 *	Envia o conteúdo
	 */
	lseek (fd, gp->g_begin, SEEK_SET);

	for (count = gp->g_end - gp->g_begin; count > 0; count -= BLSZ)
	{
		int		cnt;

		if (count >= BLSZ)
			cnt = BLSZ;
		else
			cnt = count;

		if (read (fd, area, cnt) != cnt)
			{ read_error++; break; }

		if (t_snd (global_tcp_fd, area, cnt, 0) != cnt)
			error (NOSTR);
	}

	close (fd);

	/*
	 *	Verifica se deu erro
	 */
	if (read_error)
	{
		save_errno = errno;

		www_send_msg
		(	403,
			strerror (save_errno),
			"Erro na leitura do arquivo \"%s\"\r\n",
			file_nm
		);

		errno = save_errno;
#ifdef	DEBUG
		error
		(	"*Erro na leitura do arquivo \"%s\"",
			file_nm
		);
#endif	DEBUG
		return (-1);
	}

	if (t_push (global_tcp_fd) < 0)
		error ("*Erro no T_PUSH");

	log_msg
	(	"Arquivo%s \"%s\" => \"%s\", HTTP 1/%c",
		partial ? " parcial" : "",
		file_nm, client_node_nm,
		gp->g_http == -1 ? '?' : (gp->g_http == 1 ? '1' : '0')
#if (0)	/*******************************************************/
		gp->g_close ? ", C" : ", NC"
#endif	/*******************************************************/
	);

#ifdef	DEBUG
	error ("Enviei o arquivo \"%s\"", file_nm);
#endif	DEBUG

	return (0);

}	/* end www_send_file */

/*
 ****************************************************************
 *	Converte UTF para ISO					*
 ****************************************************************
 */
char *
utf2iso (char *str)
{
	int		n0, n1;

	/*
	 *	Por hipótese, str[0] == '%'
	 */
	if ((n0 = hexa2iso (str + 1)) < 0)
		return (str + 1);

	/*
	 *	Apenas um caractere
	 */
	if ((n0 & 0x80) == 0 || str[3] != '%')
		{ str[0] = n0; strcpy (str + 1, str + 3); return (str + 1); }

	/*
	 *	Dois caracteres
	 */
	if ((n1 = hexa2iso (str + 4)) < 0)
		return (str + 4);

	if ((n0 & 0xE0) != 0xC0)
		return (str + 4);

	if ((n1 & 0xC0) != 0x80)
		return (str + 4);

	str[0] = (n0 << 6) | (n1 & 0x3F);

	strcpy (str + 1, str + 6); return (str + 1);

}	/* end utf2iso */

/*
 ****************************************************************
 *	Converte 2 dígitos hexadecimais em um caractere		*
 ****************************************************************
 */
int
hexa2iso (const char *num)
{
	int		n;
	const char	*str;
	char		vec[4];

	vec[0] = num[0]; vec[1] = num[1]; vec[2] = '\0';

	if ((n = strtol (vec, &str, 16)) >= 256 || *str != '\0')
		return (-1);

	return (n);

}	/* end hexa2iso */

/*
 ****************************************************************
 *	Converte uma cadeia para maiúscula			*
 ****************************************************************
 */
const char *
str_to_upper (const char *dst)
{
	char		c, *cp;

	/*
	 *	Converte para maíuscula
	 */
	for (cp = (char *)dst; (c = *cp) != '\0'; cp++)
	{
		if (c >= 'a' && c <= 'z')
			*cp = c - ('a' - 'A');
	}

	return (dst);

}	/* end str_to_upper */

/*
 ****************************************************************
 *	Edita a data GMT					*
 ****************************************************************
 */
char *
edit_date (char *area, int size)
{
	time_t		tempo;
	TM		*tmp;

	time (&tempo);
	tmp = gmtime (&tempo);
	strftime (area, size, "%a, %d %b %Y %X GMT", tmp);

	return (area);

}	/* end edit_date */

/*
 ****************************************************************
 *	Envia uma mensagem de erro para o cliente		*
 ****************************************************************
 */
void
www_send_msg (int code, const char *err_str, const char *format, ...)
{
	va_list		 args;
	const char	*fmt = format;
	char		date_area[64];

	va_start (args, format);

	fprintf
	(	tcp_out,
		"HTTP/1.1 %d\r\n"
		"Date: %s\r\n"
		"\r\n"
		"<H2>%s (%s):\r\n",
		code,
		edit_date (date_area, sizeof (date_area)),
		uts.uts_nodename, pgversion
	);

	vfprintf (tcp_out, fmt, args);

	if (err_str)
		fprintf (tcp_out, " (%s)", err_str);

	fprintf (tcp_out, "</H2>\r\n");

	if (fflush (tcp_out) < 0)
		error ("*Erro no FFLUSH");

	if (t_push (global_tcp_fd) < 0)
		error ("*Erro no T_PUSH");

	va_end (args);

}	/* end www_send_msg */

/*
 ****************************************************************
 *	Escreve uma mensagem no arquivo do "www"		*
 ****************************************************************
 */
void
log_msg (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	time_t		time_loc;

	if (www_log_fp == NOFILE)
		return;

	va_start (args, format);

	fprintf  (www_log_fp, pgname);
	fprintf  (www_log_fp, ".%d", getpid ());

	time (&time_loc);
	fprintf (www_log_fp, " (%.24s): ", btime (&time_loc));

	vfprintf (www_log_fp, fmt, args);
	fprintf (www_log_fp, "\n");

	va_end (args);

}	/* end log_msg */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		 args;
	const char	*fmt = format;
	FILE		*log_fp;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;

	va_start (args, format);

	log_fp = global_log_fp;

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
		fprintf (log_fp, " (%.24s)", btime (&time_loc));

		fprintf  (log_fp, ": ", pgname);
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

	if (abrt == '$')
	{
		error ("Terminando para \"%s\"", client_node_nm);

		if (event != T_DISCONNECT)
			tcp_abort (1);
		else
			exit (1);
	}

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (int n)
{
	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */
