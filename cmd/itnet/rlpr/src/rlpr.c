/*
 ****************************************************************
 *								*
 *			rlpr.c					*
 *								*
 *	Envia arquivos para uma impressora remota		*
 *								*
 *	Versão	4.2.0, de 21.05.02				*
 *		4.4.0, de 07.08.03				*
 *								*
 *	Módulo: rlpr						*
 *		Utilitários Supérfluos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/scb.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

#include "../h/tcp.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.4.0, de 07.08.03";

#define	elif		else if

#define	LPR_PORT	515
#define	DATA_FILE	3
#define	CONTROL_FILE	2

/*
 ****** Variáveis Globais ***************************************
 */
entry int		vflag;		/* Verbose */
entry int		Nflag;		/* Nomes dos pgms no <stdin> */

entry int		send_to_spooler;/* Envia para o "spooler" ou direto */

entry char		default_spooler[] = "eniac.nce.ufrj.br";
entry char		default_queue[]   = "sec_aep2";

entry char		*spooler_name;	/* Nome do "spooler" */
entry char		*queue_name;	/* Nome da fila */
entry int		printer_fd;	/* "fd" da conexão */
entry FILE		*control_fp;	/* "fp" do arquivo de controle */

/*
 ****** Protótipos de funções ***********************************
 */
void			help (void);
int			get_spooler_and_queue_names (const char *);
int			proc_file (const char *, const char *);
int			send_file (int, const char *, const char *);
int			send_printer_cmd (int, const char *, int);
int			connect (int, const char *);
int			set_param (int);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	int		opt;
	int		exit_code = 0;
	char		doc_name[64], last_char = 'Z';
	char		node_name[128];
	const char	*user_name = NOSTR;
	char		*control_path;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "P:u:vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'P':			/* Impressora */
			get_spooler_and_queue_names (optarg);
			break;

		    case 'u':
			user_name = optarg;
			break;

		    case 'v':			/* Verboso */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Ajuda */
			help ();

		    default:			/* Erro */
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
		error ("Os argumentos supérfluos serão ignorados");
		*argv = NOSTR;
	}

	/*
	 *	Obtém o nome da impressora e da fila de impressão
	 */
	if
	(	spooler_name == NOSTR &&
		get_spooler_and_queue_names (getenv ("PRINTER")) < 0
	)
	{
		spooler_name	= default_spooler;
		queue_name	= default_queue;
		send_to_spooler = 1;
	}

	/*
	 *	Abre a conexão com a impressora
	 */
	if ((printer_fd = tcp_connect (LPR_PORT, spooler_name)) < 0)
		error ("$*Não consegui estabelecer a conexão com \"%s\"", spooler_name);

	if (send_printer_cmd (2, queue_name, 0) < 0)
		error ("$A impressora \"%s@%s\" recusa-se a conversar", queue_name, spooler_name);

	/*
	 *	Obtém o nome do usuário
	 */
	if (user_name == NOSTR && (user_name = getenv ("USER")) == NOSTR)
		user_name = "Anônimo";

	/*
	 *	Obtém o nome da máquina
	 */
	strcpy
	(	node_name,
		t_addr_to_node
		(	printer_fd,
			t_node_to_addr (printer_fd, NOSTR, (long *)NULL)
		)
	);

	if (vflag)
	{
		printf ("=> Impressora: \"%s/%s\"\n", spooler_name, queue_name);
		printf ("=> Nome do usuário: \"%s\"\n", user_name);
		printf ("=> Nome do nó: \"%s\"\n", node_name);
	}

	/*
	 *	Inicia o arquivo de controle
	 */
	if ((control_path = strdup (mktemp ("/tmp/lprXXXXXX"))) == NOSTR)
		control_path = "/tmp/lpr_bobo";

	if ((control_fp = fopen (control_path, "w")) == NOFILE)
		error ("$*Não consegui criar \"%s\"", control_path);

	fprintf (control_fp, "H%s\n", node_name);
	fprintf (control_fp, "P%s\n", user_name);

	sprintf (doc_name, "dfA%03d%s", 30, node_name);

	/*
	 *	Procura os argumentos
	 */
	if (Nflag)			/* Lê os nomes do <stdin> */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
		{
			exit_code += proc_file (area, doc_name);

			if (++doc_name[2] > last_char)
			{
				if (last_char == 'Z')
					{ last_char = 'z'; doc_name[2] = 'a'; }
				else
					{ error ("Número excessivo de arquivos"); break; }
			}
		}
	}
	elif (*argv == NOSTR)
	{
		exit_code += proc_file (NOSTR, doc_name);
	}
	else				/* Foram dados argumentos */
	{
		while (*argv != NOSTR)
		{
			exit_code += proc_file (*argv++, doc_name);

			if (++doc_name[2] > last_char)
			{
				if (last_char == 'Z')
					{ last_char = 'z'; doc_name[2] = 'a'; }
				else
					{ error ("Número excessivo de arquivos"); break; }
			}
		}
	}

	fclose (control_fp);

	if (send_to_spooler && exit_code == 0)
	{
		/* Ao final, envia o arquivo de controle */

		doc_name[0] = 'c'; doc_name[2] = 'A';

		if (send_file (CONTROL_FILE, control_path, doc_name) < 0)
			exit_code++;
	}

	tcp_close (printer_fd);

	unlink (control_path);

	free (control_path);

	return (exit_code);

}	/* end main */

/*
 ****************************************************************
 *	Obtém o nome do "spooler" e da fila de impressão	*
 ****************************************************************
 */
int
get_spooler_and_queue_names (const char *arg)
{
	char		*cp;

	if (arg == NOSTR)
		return (-1);

	/*
	 *	Formatos possíveis:
	 *	   @printer		envia direto para a impressora
	 *	   queue@spooler	envia para a fila "queue" de "spooler"
	 *	   queue		envia para a fila "queue" de "eniac"
	 */
	if (arg[0] == '@')
	{
		spooler_name	= (char *)arg + 1;
		queue_name	= "no_queue";

		send_to_spooler	= 0;	/* Direto para a impressora */
	}
	else
	{
		queue_name = strdup (arg);

		if ((cp = strchr (queue_name, '@')) == NOSTR)
			spooler_name = default_spooler;
		else
			{ *cp++ = '\0'; spooler_name = cp; }

		send_to_spooler = 1;	/* Envia para o "spooler" */
	}

	return (0);

}	/* end get_spooler_and_queue_names */

/*
 ****************************************************************
 *	Envia um arquivo					*
 ****************************************************************
 */
int
proc_file (const char *file_path, const char *doc_name)
{
	fprintf (control_fp, "l%s\nU%s\n\n", doc_name, doc_name);

	if (send_file (DATA_FILE, file_path, doc_name) < 0)
		return (1);

	return (0);

}	/* end proc_file */

/*
 ****************************************************************
 *	Envia um Arquivo					*
 ****************************************************************
 */
int
send_file (int file_type, const char *file_path, const char *doc_name)
{
	int		fd, nbytes, count, file_size;
	STAT		st;
	char		area[512], resp;
	char		*tmp_path = NOSTR;

	/*
	 *	Abre o arquivo e determina seu tamanho
	 */
	if (file_path == NOSTR)
	{
		/*
		 *	Como não podemos dar "stat" no <stdin>, é preciso
		 *	copiar seu conteúdo para um arquivo temporário
		 */
		file_path = "<stdin>";

		if ((tmp_path = strdup (mktemp ("/tmp/lprXXXXXX"))) == NOSTR)
			{ error ("*Não conseguir obter um nome temporário"); return (-1); }

		if ((fd = open (tmp_path, O_CREAT|O_TRUNC|O_EXCL|O_RDWR, 0600)) < 0)
			{ error ("*Não consegui criar o arquivo \"%s\"", tmp_path); return (-1); }

		while ((nbytes = read (fileno (stdin), area, sizeof (area))) > 0)
		{
			if (write (fd, area, nbytes) != nbytes)
				{ error ("*Erro de escrita em \"%s\"", tmp_path); goto bad; }
		}

		if (nbytes < 0)
			{ error ("*Erro na leitura de \"%s\"", file_path); goto bad; }

		if (lseek (fd, 0, SEEK_SET) < 0)
			{ error ("*Erro no posicionamento de \"%s\"", file_path); goto bad; }
	}
	elif ((fd = open (file_path, O_RDONLY)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", file_path);
		return (-1);
	}

	if (fstat (fd, &st) < 0)
		{ error ("*Não consegui obter o estado de \"%s\"", file_path); goto bad; }

	/*
	 *	Prepara e envia o comando inicial
	 */
	sprintf (area, "%c%d %s\n", file_type, st.st_size, doc_name);

	nbytes = strlen (area);

	for (count = 0; /* abaixo */; count++)
	{
		if (tcp_write (printer_fd, area, nbytes) != nbytes)
			{ error ("*Erro de escrita na conexão com a impressora"); goto bad; }

		if (vflag)
			{ printf ("=> <%s: ", file_path); fflush (stdout); }

		if (tcp_read (printer_fd, &resp, 1) != 1)
			{ error ("*Erro de leitura na conexão com a impressora"); goto bad; }

		if (vflag)
			printf ("ACK = %02X>\n", resp);

		if (++count > 10)
			goto bad;

		if (resp == 0)
			break;
	}

	/*
	 *	Envia o conteúdo do arquivo
	 */
	file_size = 0;

	if (vflag)
		{ printf ("   Enviando: "); fflush (stdout); }

	while ((nbytes = read (fd, area, sizeof (area))) > 0)
	{
		if (tcp_write (printer_fd, area, nbytes) != nbytes)
		{
			if (vflag)
				putchar ('\n');

			error ("*Erro de escrita na conexão com a impressora");
			goto bad;
		}

		file_size += nbytes;

		if (vflag)
			{ putchar ('.'); fflush (stdout); }
	}

	if (vflag)
		putchar ('\n');

	if (nbytes < 0)
		{ error ("*Erro na leitura de \"%s\"", file_path); goto bad; }

	close (fd);

	if (send_to_spooler)
	{
		if (tcp_write (printer_fd, "", 1) != 1)
		{
			error ("*Erro de escrita na conexão com a impressora");
			goto bad;
		}

		if (tcp_read (printer_fd, &resp, 1) != 1)
			{ error ("*Erro de leitura na conexão com a impressora"); goto bad; }

		if (resp != 0)
		{
			if (vflag)
				printf ("ACK final = %02X\n", resp);
		}
	}

	if (vflag && file_size == st.st_size)
		printf ("   Foram corretamente enviados %d bytes do arquivo \"%s\"\n", file_size, file_path);

	if (tmp_path != NOSTR)
		{ unlink (tmp_path); free (tmp_path); }

	return (0);

    bad:
	close (fd);

	if (tmp_path != NOSTR)
		{ unlink (tmp_path); free (tmp_path); }

	return (-1);

}	/* end proc_file */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - cliente para impressão remota\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-P <impressora>] [-vN] [<arquivo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-P: realiza a conexão com a <impressora> remota\n"
		"\t-v: verboso\n"
		"\t-N: Lê os nomes dos <arquivo>s de \"stdin\"\n"
	);

	exit (2);

}	/* end help */

/*
 ****************************************************************
 *	Inicia a conexão com a impressora			*
 ****************************************************************
 */
int
send_printer_cmd (int cmd, const char *prqueue, int wanted)
{
	char		area[64], resp;
	int		nbytes;

	sprintf (area, "%c%s\n", cmd, prqueue);

	nbytes = strlen (area);

	if (tcp_write (printer_fd, area, nbytes) != nbytes)
		{ error ("*Erro de escrita na conexão com a impressora"); return (-1); }

	if (vflag)
		{ printf ("=> <cmd = %02X: ", cmd); fflush (stdout); }

	if (tcp_read (printer_fd, &resp, 1) != 1)
		{ error ("*Erro de leitura na conexão com a impressora"); return (-1); }

	if (vflag)
		printf ("ACK = %02X>\n", resp);

	return (resp == wanted ? 0 : -1);

}	/* send_printer_cmd */
