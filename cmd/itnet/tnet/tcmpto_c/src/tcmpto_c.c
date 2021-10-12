/*
 ****************************************************************
 *								*
 *			tcmpto_c.c				*
 *								*
 *	Comparação remota de árvores (cliente) 			*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.5.0, de 03.06.03				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ftw.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <termio.h>
#include <errno.h>
#include <time.h>
#include <xti.h>

#include "../../tnet.h"
#include "../../reduce.h"
#include "../h/tcmpto_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.5.0, de 03.06.03";

#define	TCMPTO_PORT	117

const char	TCMPTO_VERSION[] = "TCMPTO,3.0.5";

/*
 ****** Os diversos (e muitos) indicadores **********************
 */
entry int		fromflag;	/* Caminhada na árvore REMOTA */
entry int		cpflag;		/* Copie diretamente (sem comparação) */
entry int		pipeflag;	/* Pipe */

entry int		tflag,		/* copy flags */
			fflag,
			dflag;

entry int		Cflag;		/* Usa compressão */
entry int		gflag;		/* Gere o "garfield" */
entry int		Lflag;		/* local = Não entre em diretórios */
entry int		iflag = 1;	/* Iterativo */
entry int		vflag;		/* verbose flag */
entry int		qflag;		/* Comparação rápida de objetos */
entry int		mflag;		/* Copia também o estado do arquivo */
entry int		dotflag;	/* Não ignorar ".old" */
entry int		Nflag;		/* Nomes do "stdin" */

entry int		nflag;		/* Foi dado o nome do nó */
entry int		uflag;		/* Foi dado o nome do usuário remoto */
entry int		Dflag;		/* Foi dado o diretório remoto */

entry int		Zflag;		/* Do not execute Commands */
entry int		Bflag;		/* Debug */

entry const char	**color_vector;	/* Para as cores dos modos */

/*
 ******	Os padrões **********************************************
 */
entry const char	*inc_pat[NPAT+1];	/* Os Padrões de inclusão */
entry int		inc_pati;

entry const char	*exc_pat[NPAT+1];	/* Os Padrões de exclusão */
entry int		exc_pati;

/*
 ****** Variáveis diversas **************************************
 */
const char		tcp_dev_nm[] = "/dev/itntcp";

entry int		tcp_fd;		/* Cópia do "fd" */

entry int		snd_dis = 0;	/* Disconecta em caso de erro (se != 0) */

entry char		local_node_nm[80]; /* Nome do nó local */

entry const char	*remote_node_nm; /* Nome do nó remoto */

entry const char	*local_user_nm;	/* Nome do usuário local */

entry int		local_user_euid; /* Identificação efetiva do usuário local */

entry const char	*remote_user_nm; /* Nome do usuário remoto */

entry const char	*remote_dir_nm;	/* Nome do diretório remoto */

entry const char	*local_cwd_nm;	/* Nome do diretório corrente local */

#ifdef	REPORTs
entry TERMIO	old_video, video;	/* modos do terminal */
#endif	REPORTs

const char		garfield[] = "garfield";

entry FILE		*tty_fp;		/* Arquivo para "askyesno" */

entry FILE		*gar_fp;		/* Arquivo "garfield" */

entry int		diff_files_cnt;		/* Contador de nr. de Arq. diferentes */

extern int		MAX_BIT_SZ;	/* O algoritmo funciona no máximo com 16 bits */ 

#ifdef	DEBUG
void			print_op_list (void);
#endif	DEBUG

/*
 ****************************************************************
 *	Comparação remota de árvores (cliente) 			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd;
	const char	*field;
	const char	**p;
	char		*cp;
	char		*rmsg;
	char		buf[BLSZ];
	INADDR		serv_addr, snd_addr, my_addr;
	T_BIND		bind;
	T_CALL		snd_call, rcv_call;
	SEGFLAGS	segflags;
	char		id[16];
	const char	*str, *bit_sz_str = NOSTR;

	/*
	 *	Verifica logo se a caminhada é na árvore local ou remota ou pipe
	 */
	str = "tfdCgLFvsqm.Nn:u:D:p:P:MHBZ";

	if   (patmatch (pgname, "*cmpfrom"))
		fromflag++;
	elif (patmatch (pgname, "*cpto"))
		cpflag++;
	elif (patmatch (pgname, "*cpfrom"))
		{ cpflag++; fromflag++; }
	elif (patmatch (pgname, "*pipe"))
		{ pipeflag++; str = "CNn:u:D:MHB"; }

	/*
	 *	tcmpto   [-tfdgLFvm.N] <ENV> <PAD> [<árvore> ...]
	 *	tcmpfrom [-tfdgLFvm.N] <ENV> <PAD> [<árvore> ...]
	 *	tcpto    [-LFvm.N] <ENV> <PAD> [<árvore> ...]
	 *	tcpfrom  [-LFvm.N] <ENV> <PAD> [<árvore> ...]
	 *	tpipe    [-N] <ENV> [<árvore> ...]
	 *
	 *	Valores de <ENV>: [-C] [-n <nó>] [-u <usuário>] [-D <rdir>]
	 *
	 *	Valores de <PAD>: [-s] [-p <p>] [-P <P>]
	 */
	while ((opt = getopt (argc, argv, str)) != EOF)
	{
		switch (opt)
		{
		    case 't':		/* Copy-to flag */
			tflag++;
			if (fflag || dflag || gflag)
				help ();
			break;

		    case 'f':		/* Copy-from flag */
			fflag++;
			if (tflag || dflag || gflag)
				help ();
			break;

		    case 'd':		/* Data-flag */
			dflag++;
			if (tflag || fflag || gflag)
				help ();
			break;

		    case 'C':		/* Comprime */
			Cflag++;
			break;

		    case 'g':		/* Gera "garfield" */
			gflag++;
			if (fflag || dflag || tflag)
				help ();
			break;

		    case 'L':		/* Local */
			Lflag = 1;
			break;

		    case 'F':		/* Force */
			iflag = 0;
			break;

		    case 'v':		/* Verbose flag */
			vflag++;
			break;

		    case 's':			/* Padrões = 	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':		/* Padrões de inclusão */
			put_p_option (optarg, inc_pat, &inc_pati);
			break;

		    case 'P':		/* Padrões de exclusão */
			put_p_option (optarg, exc_pat, &exc_pati);
			break;

		    case 'q':		/* Comparação rápida de objetos */
			qflag++;
			break;

		    case 'm':		/* Copia o estado do arquivo */
			mflag++;
			break;

		    case '.':		/* Não ignora ".old" */
			dotflag++;
			break;

		    case 'N':		/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'n':		/* Nó */
			nflag++;
			remote_node_nm = optarg;
			break;

		    case 'u':		/* Usuário remoto */
			uflag++;
			remote_user_nm = optarg;
			break;

		    case 'D':		/* Diretório remoto */
			Dflag++;
			remote_dir_nm = optarg;
			break;

		    case 'M':		/* Stick im memory */
			exit (0);

		    case 'H':		/* Help */
			help ();

		    case 'B':		/* Debug flag */
			Bflag++;
			vflag++;
			break;

		    case 'Z':		/* Não executa os comandos */
			Zflag++;
			break;

		    default:		/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Os argumentos supérfluos serão ignorados\n",
			pgname
		);

		*argv = NOSTR;
	}

#if (0)	/*******************************************************/
	/*
	 *	Lendo de arquivo deve ser aceito ".old"
	 */
	if (*argv == NOSTR)
		dotflag++;
#endif	/*******************************************************/

	/*
	 *	Abre o "garfield"
	 */
	if (!pipeflag && gflag && (gar_fp = fopen (garfield, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", garfield);

	/*
	 *	Abre o terminal para "askyesno"
	 */
	if (!pipeflag && (askyesno_stdin = tty_fp = fopen ("/dev/tty", "r")) == NOFILE)
		error ("$*Não consegui abrir \"/dev/tty\"");

	askyesno_set (1);	/* Em princípio, força */

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Encontra o nome do usuário local
	 */
	if ((local_user_nm = getlogin ()) == NOSTR)
		local_user_nm = strdup (getenv ("USER"));

	if (local_user_nm == NOSTR)
		error ("$Não consegui obter o nome do usuário local");

	/*
	 *	Ajusta os sinais
	 */
	if (signal (SIGINT,  SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_signal);

	if (signal (SIGTERM,  SIG_IGN) != SIG_IGN)
		signal (SIGTERM, on_signal);

#ifdef	REPORTs
	/*
	 *	Poe o Terminal sem PAGE na entrada
	 */
	if (!pipeflag && ioctl (fileno (tty_fp), TCGETS, &old_video) >= 0)
	{
		video = old_video;
#if (0)	/*************************************/
		video.c_cc[VPAGE] = 0;
		ioctl (fileno (tty_fp), TCSETNR, &video);
#endif	/*************************************/
	}
#endif	REPORTs

	/*
	 *	O nó destino, usuário remoto e o diretório remoto
	 *	também podem ser dados no ambiente
	 */
	if ((cp = (char *)getenv ("TNET")) == NOSTR)
		cp = "";

	if ((cp = strdup (cp)) == NOSTR)
		cp = "";

	for (field = strfield (cp, ','); field != NOSTR; field = strfield (NOSTR, ','))
	{
		if   (strncmp (field, "node=", 5) == 0)
		{
			if (remote_node_nm == NOSTR)
				remote_node_nm = field + 5;
		}
		elif (strncmp (field, "user=", 5) == 0)
		{
			if (remote_user_nm == NOSTR)
				remote_user_nm = field + 5;
		}
		elif (strncmp (field, "rdir=", 5) == 0)
		{
			if (remote_dir_nm == NOSTR)
				remote_dir_nm = field + 5;
		}
		elif (streq (field, "reduce") || streq (field, "compress"))
		{
			Cflag++;
		}
		elif (*field >= '0'  &&  *field <= '9')
		{
			if (bit_sz_str == NOSTR)
				bit_sz_str = field;
		}
	}

	/*
	 *	Analisa o número de bits
	 */
	if (bit_sz_str != NOSTR)
	{
		if
		(	(MAX_BIT_SZ = strtol (bit_sz_str, &str, 0)) < 9 ||
			MAX_BIT_SZ > 16 ||
			*str != '\0'
		)
		{
			error ("$No. de bits inválido: \"%s\"", bit_sz_str);
		}
	}
	else
	{
		MAX_BIT_SZ = DEF_BIT_SZ;
	}

	if (remote_node_nm == NOSTR)
		error ("$Faltou dar o nome do nó destino");

	if (remote_user_nm == NOSTR)
		remote_user_nm = local_user_nm;

	if (remote_dir_nm != NOSTR && streq (remote_dir_nm, "-"))
		remote_dir_nm = NOSTR;

	/*
	 ******	Abre o dispositivo TCP **************************
	 */
	if ((tcp_fd = fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	if ((snd_addr.a_addr = t_node_to_addr (fd, remote_node_nm, &my_addr.a_addr)) == -1)
		error ("$*Não consegui obter o endereço de \"%s\"", remote_node_nm);

	snd_addr.a_port = TCMPTO_PORT;

	if ((cp = t_addr_to_node (fd, my_addr.a_addr)) == NOSTR)
		error ("$Não consegui obter o nome do nó local");

	strcpy (local_node_nm, cp);

	/*
	 *	Executa o "bind" para o ENDPOINT local
	 */
	my_addr.a_port = 0;
	my_addr.a_addr = 0;

	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &my_addr;

	bind.qlen = 0;

	if (t_bind (fd, &bind, &bind) < 0)
		error ("$*Não consegui atribuir endereço local");

	/*
	 *	Executa o "connect"
	 */
	snd_call.addr.len    = sizeof (INADDR);
	snd_call.addr.buf    = &snd_addr;

	rcv_call.addr.maxlen = sizeof (INADDR);
	rcv_call.addr.buf    = &serv_addr;

	if (t_connect (fd, &snd_call, &rcv_call) < 0)
	{
		if (errno == TLOOK && t_look (fd) == T_DISCONNECT)
			error ("$A conexão foi recusada");
		else
			error ("$*Não consegui estabelecer conexão");
	}

	snd_dis++;	/* De agora adiante, com "^C" desconecta */

	/*
	 *	Recebe o nome do servidor e versão do protocolo
	 */
	t_rcv_msg (&rmsg);

	if (!streq (rmsg, TCMPTO_VERSION))
		error ("$Erro de versão de servidor: %s :: %s", rmsg, TCMPTO_VERSION);

	/*
	 *	Envia o segmento de flags
	 */
	segflags.seg_type      = ENDIAN_LONG (FFLAGS);
	segflags.seg_flags[0]  = ENDIAN_LONG (fromflag);
	segflags.seg_flags[1]  = ENDIAN_LONG (cpflag);
	segflags.seg_flags[2]  = ENDIAN_LONG (gflag);
	segflags.seg_flags[3]  = ENDIAN_LONG (Lflag);
	segflags.seg_flags[4]  = ENDIAN_LONG (vflag);
	segflags.seg_flags[5]  = ENDIAN_LONG (qflag);
	segflags.seg_flags[6]  = ENDIAN_LONG (mflag);
	segflags.seg_flags[7]  = ENDIAN_LONG (dotflag);
	segflags.seg_flags[8]  = ENDIAN_LONG (Bflag);
	segflags.seg_flags[9]  = ENDIAN_LONG (Cflag);
	segflags.seg_flags[10] = ENDIAN_LONG (MAX_BIT_SZ);

	if (t_snd (fd, &segflags, sizeof (segflags), 0) < 0)
		error (NOSTR);

	/*
	 *	Envia os nomes dos usuários local e remoto
	 */
	local_user_euid = ENDIAN_LONG (geteuid ());

	if (t_snd (fd, &local_user_euid, sizeof (local_user_euid), 0) < 0)
		error (NOSTR);

	strscpy (id, local_user_nm, sizeof (id));

	if (t_snd (fd, id, sizeof (id), 0) < 0)
		error (NOSTR);

	strscpy (id, remote_user_nm, sizeof (id));

	if (t_snd (fd, id, sizeof (id), T_PUSH) < 0)
		error (NOSTR);

	/*
	 *	Processa a senha
	 */
	if (t_rcv_msg (&rmsg) != 0)
	{
		sprintf (buf, "Senha para \"%s@%s\": ", remote_user_nm, remote_node_nm);

		t_snd_msg (0, getpass (buf));

		if (t_rcv_msg (&rmsg) < 0)
			{ fprintf (stderr, "%s\n", rmsg); goto rel; }
	}

	/*
	 *	Processa a (segunda) senha do superusuário
	 */
	if (t_rcv_msg (&rmsg) != 0)
	{
		sprintf (buf, "Repita a senha para \"%s@%s\": ", remote_user_nm, remote_node_nm);

		t_snd_msg (0, getpass (buf));

		if (t_rcv_msg (&rmsg) < 0)
			{ fprintf (stderr, "%s\n", rmsg); goto rel; }
	}

	/*
	 *	Agora, mantém apenas os finais dos nomes
	 */
	if ((cp = strchr (local_node_nm, '.')) != NOSTR)
		cp[0] = '\0';

	if ((cp = strchr (remote_node_nm, '.')) != NOSTR)
		cp[0] = '\0';

	/*
	 *	Envia o nome do diretório remoto
	 */
	if   (remote_dir_nm == NOSTR)		/* Não dado */
	{
		t_snd_msg (0, NOSTR);
	}
	elif (remote_dir_nm[0] == '/')		/* /beta */
	{
		t_snd_msg (0, remote_dir_nm);
	}
	else					/* beta */
	{
		if ((local_cwd_nm = getcwd (NOSTR, BLSZ)) == NOSTR)
			error ("$Não consegui obter o diretório corrente");

		strcpy (buf, local_cwd_nm);

		if (!streq (remote_dir_nm, "."))
		{
			strcat (buf, "/");
			strcat (buf, remote_dir_nm);
		}

		t_snd_msg (0, buf);
	}

	/*
	 *	Envia os padrões
	 */
	if (fromflag)
	{
		for (p = inc_pat; *p != NOSTR; p++)
			t_snd_msg_no_push (0, *p);

		t_snd_msg (-1, NOSTR);

		for (p = exc_pat; *p != NOSTR; p++)
			t_snd_msg_no_push (0, *p);

		t_snd_msg (-1, NOSTR);
	}

	/*
	 *	Ajusta o sinal SIGINT para esperar o final de cada arquivo
	 */
	if (signal (SIGINT,  SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_sigint);

	/*
	 *	Verifica se é "pipe"
	 */
	if (pipeflag)
		{ execute_pipe (argv); goto rel; }

	/*
	 *	Processa as árvores
	 */
	walk (argv);

	/*
	 *	Emita a mensagem de totais de diferenças
	 */
	if   (cpflag)
	{
		/* Nenhuma mensagem */
	}
	elif (diff_files_cnt > 1)
	{
		fprintf
		(	stderr,
			"Foram encontrados %d arquivos diferentes em \"%s\"\n",
			diff_files_cnt, remote_node_nm
		); 
	}
	elif (diff_files_cnt == 1)
	{
		fprintf
		(	stderr,
			"Foi encontrado 1 arquivo diferente em \"%s\"\n",
			remote_node_nm
		); 
	}
	else
	{
		fprintf
		(	stderr,
			"Não foi encontrado nenhum arquivo diferente em \"%s\"\n",
			remote_node_nm
		); 
	}

	/*
	 *	Imprime a lista de operações
	 */
	if (Bflag)
		print_op_list ();

#ifdef	REPORTs
	/*
	 *	Poe o Terminal em RAW
	 */
	video.c_lflag &= ~(ICANON|ECHO);
	video.c_cc[VPAGE] = 0;
	video.c_cc[VMIN] = 1;

	ioctl (fileno (tty_fp), TCSETNR, &video);
#endif	REPORTs

	/*
	 *	Copia os arquivos escolhidos
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
	if (t_sndrel (fd) < 0)
		error ("$*Não consegui terminar a conexão");

	if (t_rcvrel (fd) < 0)
		error ("$*Não consegui terminar a conexão");

	t_close (fd);

	quit (0);

	return (1);

}	/* end tcmpto */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf (stderr, "%s - ", pgname);

	if   (pipeflag)
	{
		fprintf
		(	stderr,
			"escreve arquivos remotos na saída padrão\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-N] <ENV> [<árvore> ...]\n",
			pgversion, pgname
		);
	}
	elif (!cpflag)
	{
		fprintf
		(	stderr,
			"comparação/atualização remota de árvores "
			"(caminhando árvores %s)\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-tfdgLFvm.N] <ENV> <PAD> [<árvore> ...]\n",
			fromflag ? "remotas" : "locais", pgversion, pgname
		);
	}
	else	/* cpflag */
	{
		fprintf
		(	stderr,
			"cópia remota de árvores (caminhando árvores %s)\n"
			"\n%s\n"
			"\nSintaxe:\n"
			"\t%s [-LFvm.N] <ENV> <PAD> [<árvore> ...]\n",
			fromflag ? "remotas" : "locais", pgversion, pgname
		);
	}

	fprintf
	(	stderr,
		"\n\tValores de <ENV>: [-C] [-n <nó>] [-u <usuário>] [-D <rdir>]\n"
		"\n\tValores de <PAD>: [-s] [-p \"<p>\"] [-P \"<P>\"]\n"
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-t: Atualiza as <árvore>s do <nó> remoto\n"
		"\t-f: Atualiza as <árvore>s locais\n"
		"\t-d: Atualiza as <árvore>s locais ou do <nó> remoto,\n"
		"\t    dependendo das datas de modificação dos arquivos\n"
		"\t-g: Processa o arquivo \"garfield\"\n"
		"\t-L: Modo local: não entra em subdiretórios\n"
		"\t-F: Força: atualiza/copia sem esperar por uma confirmação do usuário\n"
	);

	fprintf
	(	stderr,
		"\t-v: Verboso: imprime estatísticas, ...\n"
		"\t-m: Atualiza/copia também o estado do arquivo\n"
		"\t-.: Não ignora arquivos \"ocultos\" (da forma \".old\")\n"
		"\t-s: Adiciona os padrões dos fontes em \"C\"\n"
		"\t-p: Seleciona arquivos através dos padrões <p> de inclusão\n"
		"\t-P: Seleciona arquivos através dos padrões <P> de exclusão\n"
	);

	fprintf
	(	stderr,
		"\t-C: Transmite os conteúdos dos arquivos em forma comprimida\n"
		"\t-n: Nome ou endereço do <nó> remoto\n"
		"\t-u: Nome do usuário do <nó> remoto\n"
		"\t-D: Nome para compor o caminho completo das <árvore>s\n"
		"\t-N: Le os nomes das <árvore>s de \"stdin\"\n"
		"\t    Esta opção é implícita se não forem dados argumentos\n"
		"\nObs.:\tAs opções \"-t\", \"-f\" e \"-d\" são mutuamente exclusivas\n"
	);

	exit (2);

}	/* end help */
