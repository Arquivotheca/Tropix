/*
 ****************************************************************
 *								*
 *			tcmpto_s.c				*
 *								*
 *	Comparação remota de árvores (servidor)			*
 *								*
 *	Versão	2.3.0, de 29.10.91				*
 *		4.5.0, de 03.06.03				*
 *								*
 *	Módulo: TCMPTO_S/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
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
#include <signal.h>
#include <pwd.h>
#include <xti.h>

#include "../h/tcmpto_s.h"
#include "../../tnet.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	TCMPTO_PORT	137

const char	TCMPTO_VERSION[] = "TCMPTO,3.0.5";

/*
 ****** Os diversos (e muitos) indicadores **********************
 */
entry int		fromflag;	/* Caminhada na árvore REMOTA */
entry int		cpflag;		/* Copie diretamente (sem comparação) */

entry int		pflag;		/* Senha necessária */
entry int		vflag;		/* verbose flag */

entry int		Cflag;		/* Usando compressão */
entry int		gflag;		/* Gere o "garfield" */
entry int		Lflag;		/* local = Não entre em diretórios */
entry int		qflag;		/* Comparação rápida de objetos */
entry int		mflag;		/* Copia também o estado do arquivo */
entry int		dotflag;	/* Não ignorar ".old" */

entry int		Bflag;		/* Debug */

entry int		tcp_fd;		/* Cópia do "fd" */

entry int		snd_dis = 0;	/* Disconecta em caso de erro (se != 0) */

entry int		global_log_fd;	/* Para "error" */
entry FILE		*global_log_fp;

/*
 ****** Estruturas deste módulo *********************************
 */
entry char		local_user_nm[16];

entry char		remote_user_nm[16];

entry int		local_user_uid;

entry int		local_user_gid;

entry int		remote_user_euid;

entry char		file_prefix_nm[2*BLSZ];		/* Prefixo para os arquivos */

entry char		*file_prefix_ptr;		/* Ponteiro para o '\0' */

entry PASSWD		no_user  = {"", "?????????????", "", 1 }; /* Inexistente */

const char		no_pwd[]  =	"-------------";	/* Senha nula */

#undef	RETYPE
#ifdef	RETYPE
const char		superuser[] = "<superuser>";		/* Nome do Retype */
#endif	RETYPE

extern int		MAX_BIT_SZ;	/* O algoritmo funciona no máximo com 16 bits */ 

/*
 ******	Os padrões **********************************************
 */
entry const char	*inc_pat[NPAT+1];	/* Os Padrões de inclusão */

entry const char	*exc_pat[NPAT+1];	/* Os Padrões de exclusão */

/*
 ****************************************************************
 *	Comparação remota de árvores (servidor)			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	**p;
	const PASSWD	*pwd;
	INADDR		client_addr, serv_addr[2];
	INADDR		tcp_addr[2];
	T_BIND		bind;
	T_CALL		call;
	int		sequence, listen_fd;
	int		listen_port;
	SEGFLAGS	segflags;
	char		*rmsg;

	/*
	 *	Prepara variáveis de "error" ainda como superusuário
	 */
	if ((global_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
	{
		if ((global_log_fp = fdopen (global_log_fd, "w")) == NOFILE)
			global_log_fd = -1;
	}

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "pvMB")) != EOF)
	{
		switch (opt)
		{
		    case 'p':			/* Senha necessária */
			pflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'M':			/* Stick in memory */
			exit (0);

		    case 'B':			/* debug flag */
			Bflag++;
			vflag++;
			break;

		    default:			/* Erro */
			error ("Opção inválida: '%c'", opt);
			break;

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Nos seguintes sinais, desconecta a linha
	 */
	signal (SIGINT,  on_signal);
	signal (SIGTERM, on_signal);

	/*
	 *	tcmpto_s [-v] <listen_port> <listen_fd> <client_addr> <client_port> <sequence>
	 */
#ifdef	DEBUG
	if (Bflag) error
	(	"%s: %s <%s> <%s> <%s> <%s> <%s>",
		pgname, vflag ? "-v" : "",
		argv[0], argv[1], argv[2], argv[3], argv[4]
	);
#endif	DEBUG

	if (argc != 5) error
	(	"$%s: Número inválido de argumentos: %d",
		pgname, argc
	);

	listen_port	   = atoi (argv[0]);
	listen_fd	   = atoi (argv[1]);
	client_addr.a_addr = t_str_to_addr (argv[2]);
	client_addr.a_port = atoi (argv[3]);
	sequence	   = atoi (argv[4]);

	if ((tcp_fd = t_open ("/dev/itntcp", O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir o dispositivo");

	/*
	 *	Vou realizar o "bind"
	 */
	serv_addr[0].a_port = listen_port;	/* Usa o TNET_PORT */
	serv_addr[1].a_addr = client_addr.a_addr;
	serv_addr[1].a_port = client_addr.a_port;

	bind.addr.maxlen = 0;
	bind.addr.len	 = 2 * sizeof (INADDR);
	bind.addr.buf	 = serv_addr;

	bind.qlen	 = 0;

	if (t_bind (tcp_fd, &bind, (T_BIND *)NULL) < 0)
		error ("$*Não consegui atribuir endereço");

	/*
	 *	Vou realizar o "accept"
	 */
	call.opt.len = 0;
	call.udata.len = 0;
	call.sequence = sequence;

	call.addr.len = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf = &client_addr;

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

	snd_dis++;	/* Aborta em caso de erro fatal */

	/*
	 *	Envia o nome do servidor e o no. do protocolo
	 */
	t_snd_msg (0, TCMPTO_VERSION);

	/*
	 *	Recebe o segmento de flags
	 */
	if (t_rcv (tcp_fd, &segflags, sizeof (segflags), (int *)NULL) != sizeof (segflags))
		error (NOSTR);

	if (ENDIAN_LONG (segflags.seg_type) != FFLAGS)
		error ("$Esperava um segmento FFLAGS");

	fromflag    = ENDIAN_LONG (segflags.seg_flags[0]);
	cpflag      = ENDIAN_LONG (segflags.seg_flags[1]);
	gflag	    = ENDIAN_LONG (segflags.seg_flags[2]);
	Lflag	    = ENDIAN_LONG (segflags.seg_flags[3]);
	vflag	   += ENDIAN_LONG (segflags.seg_flags[4]);
	qflag	    = ENDIAN_LONG (segflags.seg_flags[5]);
	mflag	    = ENDIAN_LONG (segflags.seg_flags[6]);
	dotflag	    = ENDIAN_LONG (segflags.seg_flags[7]);
	Bflag	   += ENDIAN_LONG (segflags.seg_flags[8]);
	Cflag	    = ENDIAN_LONG (segflags.seg_flags[9]);
	MAX_BIT_SZ  = ENDIAN_LONG (segflags.seg_flags[10]);

	/*
	 *	Recebe os nomes dos usuários local e remoto
	 */
	if (t_rcv (tcp_fd, &remote_user_euid, sizeof (remote_user_euid), (int *)NULL) < 0)
		error (NOSTR);

#ifdef	LITTLE_ENDIAN
	remote_user_euid = ENDIAN_LONG (remote_user_euid);
#endif	LITTLE_ENDIAN

	if (t_rcv (tcp_fd, remote_user_nm, sizeof (remote_user_nm), (int *)NULL) < 0)
		error (NOSTR);

	if (t_rcv (tcp_fd, local_user_nm, sizeof (local_user_nm), (int *)NULL) < 0)
		error (NOSTR);

	/*
	 *	Decide se vai ou não pedir a senha do usuário
	 */
	if ((pwd = getpwnam (local_user_nm)) == NOPASSWD)
		pwd = &no_user;
	else
		strcpy (file_prefix_nm, pwd->pw_dir);

	local_user_uid = pwd->pw_uid;
	local_user_gid = pwd->pw_gid;

#if (0)	/*******************************************************/
	if (!ideq (local_user_nm, remote_user_nm) && !streq (pwd->pw_passwd, no_pwd))
		passwd_needed++;
#endif	/*******************************************************/

#if (0)	/*************************************/
	if (pwd->pw_uid == 0 && remote_user_euid != 0)
		passwd_needed++;
#endif	/*************************************/

	/*
	 *	Processa a senha do usuário
	 */
	if (t_getaddr (tcp_fd, tcp_addr) < 0)
		error ("$*Não consegui obter os endereços TCP");

	if
	(	!pflag || streq (pwd->pw_passwd, no_pwd)
#define	LOCAL_NETWORK_NOPWD
#ifdef	LOCAL_NETWORK_NOPWD
		|| (tcp_addr[0].a_addr & 0xFFFFFF00) == (tcp_addr[1].a_addr & 0xFFFFFF00)
#if (0)	/*******************************************************/
		|| (client_addr.a_addr & 0xFFFF0000) == 0xC0A80000	/* 192.168..... */
#endif	/*******************************************************/
#endif	LOCAL_NETWORK_NOPWD
	)
	{
		t_snd_msg (0, NOSTR);
	}
	else
	{
		t_snd_msg (1, NOSTR);

		t_rcv_msg (&rmsg);

		if (!streq (crypt (rmsg, pwd->pw_passwd), pwd->pw_passwd))
			{ t_snd_msg (-1, "Senha incorreta"); goto rel; }
		else
			t_snd_msg (0, NOSTR);
	}

	/*
	 *	Processa a (segunda) senha do superusuário
	 */
#ifdef	RETYPE
	if (pwd->pw_uid != 0 || remote_user_euid == 0)
	{
		t_snd_msg (0, NOSTR);
	}
	else
	{
		if ((pwd = getpwnam (superuser)) == NOPASSWD)
		{
			error ("Não encontrei a senha do SUPERUSUÁRIO");
			t_snd_msg (0, NOSTR);
		}
		else
		{
			t_snd_msg (1, NOSTR);

			t_rcv_msg (&rmsg);

			if (!streq (crypt (rmsg, pwd->pw_passwd), pwd->pw_passwd))
				{ t_snd_msg (-1, "Senha incorreta"); goto rel; }
			else
				t_snd_msg (0, NOSTR);
		}
	}
#else
	t_snd_msg (0, NOSTR);
#endif	RETYPE

	/*
	 *	Recebe o diretório prefixo dos nomes dos arquivo
	 */
	t_rcv_msg (&rmsg);

	if (rmsg != NOSTR)
		strcpy (file_prefix_nm, rmsg);

#if (0)	/*************************************/
	strcat (file_prefix_nm, "/");
#endif	/*************************************/

	file_prefix_ptr = strend (file_prefix_nm);

	/*
	 *	Recebe e guarda os padrões
	 */
	if (fromflag)
	{
		for (p = inc_pat; /* vazio */; /* vazio */)
		{
			if (t_rcv_msg (&rmsg) < 0)
				break;

			*p++ = strdup (rmsg);

		}	/* end for (EVER) */

		for (p = exc_pat; /* vazio */; /* vazio */)
		{
			if (t_rcv_msg (&rmsg) < 0)
				break;

			*p++ = strdup (rmsg);

		}	/* end for (EVER) */

	}	/* end if (fromflag) */

	/*
	 *	Processa as <árvores>
	 */
	setgid (local_user_gid);
	setuid (local_user_uid);

	error
	(	"Login para \"%s@%s\"",
		local_user_nm,
		t_addr_to_str (client_addr.a_addr)
	);

	walk ();

	/*
	 *	Executa as atualizações
	 */
	execute_op_list ();

	/*
	 *	Realiza as comparações
	 */
	execute_cmp_list ();

	/*
	 *	Termina graciosamente
	 */
    rel:
	error
	(	"Logout para \"%s\"",
		local_user_nm
	);

	if (t_sndrel (tcp_fd) < 0)
		error ("$*Não consegui terminar conexão");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Não consegui terminar conexão");

	t_close (tcp_fd);

	return (0);

}	/* end tcmpto_s */
