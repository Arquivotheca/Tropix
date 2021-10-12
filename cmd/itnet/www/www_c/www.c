/*
 ****************************************************************
 *								*
 *			www.c					*
 *								*
 *	Um pequeno cliente WWW para testes			*
 *								*
 *	Versão	3.0.0, de 02.11.97				*
 *		3.2.3, de 07.05.00				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.2.3, de 07.05.00";

#define WWW_PORT	80

#define	EVER	;;
#define	elif	else if

entry int	vflag;		/* Verbose */
entry int	pflag;		/* Na tela */
entry int	gflag;		/* Salva o arquivo */

/*
 ****** Variáveis globais ***************************************
 */
int		global_tcp_fd;	/* Para o TCP */

entry FILE	*tcp_in,	/* Conexão de controle */
		*tcp_out;

/*
 ****** Protótipos de funções ***********************************
 */
int		www_read_line (char *area, int count, int *);
void		error (const char *, ...);
void		on_SIGTERM (int, void *, void *);
void		tcp_abort (void);
void		quit (int);
void		help (void);

/*
 ****************************************************************
 *	Um pequeno cliente WWW para testes			*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	register char	*cp;
	const char	*field;
	const char	*node_nm = NOSTR;
	int		tcp_fd;
	T_BIND		bind;
	INADDR		addr;
	T_CALL		call;
	const char	*tcp_dev_nm;
	const char	*page_nm = NOSTR;
	long		serv_addr;
	char		area[BLSZ];
	int		n;
	FILE		*fp;
	int		byte_count;
	const char	*range_str = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "pgr:n:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'p':			/* Imprime na tela */
			if (gflag)
				help ();
			pflag++;
			break;

		    case 'g':			/* Salva o arquivo */
			if (pflag)
				help ();
			gflag++;
			break;

		    case 'n':			/* Nome do nó remoto */
			node_nm = optarg;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'r':			/* Intervalo */
			range_str = optarg;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if   (argc == 1)
		page_nm = argv[0];
	elif (argc > 1)
		help ();

	if (page_nm == NOSTR)
		page_nm = "/";
#if (0)	/*******************************************************/
	if (page_nm == NOSTR)
		page_nm = "index.html";
#endif	/*******************************************************/

	/*
	 *	O nó destino também pode ser dado no ambiente
	 */
	if ((cp = (char *)getenv ("TNET")) == NOSTR)
		cp = "";

	if ((cp = strdup (cp)) == NOSTR)
		cp = "";

	for (field = strfield (cp, ','); field != NOSTR; field = strfield (NOSTR, ','))
	{
		if   (strncmp (field, "node=", 5) == 0)
		{
			if (node_nm == NOSTR)
				node_nm = field + 5;
		}
	}

	if (vflag)
	{
		printf ("node_name = \"%s\"\n", node_nm);
		printf ("page_name = \"%s\"\n", page_nm);
	}

	/*
	 *	Abre o "endpoint"
	 */
	tcp_dev_nm = "/dev/itntcp";

	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	global_tcp_fd = tcp_fd;

	if ((tcp_in = fdopen (tcp_fd, "r")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	if ((tcp_out = fdopen (tcp_fd, "w")) == NOFILE)
		error ("$*Não consegui obter um FILE para \"%s\"", tcp_dev_nm);

	/*
	 *	Obtém o endereço do nó remoto
	 */
	if ((serv_addr = t_node_to_addr (tcp_fd, node_nm, (long *)NULL)) == -1)
		error ("$Não consegui obter o endereço de \"%s\"", node_nm);

	if (vflag)
		printf ("serv_addr dado: %08X (%s)\n", serv_addr, node_nm);

	/*
	 *	Associa um endereço local
	 */
	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &addr;

	bind.qlen	 = 0;

	addr.a_port = 0;

	if (t_bind (tcp_fd, &bind, &bind) < 0)
		error ("$Não consegui associar um endereço");

	/*
	 *	Dá o CONNECT
	 */
	addr.a_port	 = WWW_PORT;
	addr.a_addr	 = serv_addr;

	call.addr.len	 = sizeof (INADDR);
	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &addr;

	if (t_connect (tcp_fd, &call, &call) < 0)
	{
		if (errno == TLOOK && t_look (tcp_fd) == T_DISCONNECT)
		{
			error ("A conexão foi recusada por \"%s\"", node_nm);
			quit (1);
		}

		error ("$*Não consegui estabelecer conexão");
	}

	if (vflag)
		printf ("port_no do servidor obtido: %d\n", ((INADDR *)call.addr.buf)->a_port);

	/*
	 *	Envia o pedido
	 */
	fprintf (tcp_out, "GET %s HTTP/1.0\n", page_nm);

	if (range_str != NOSTR)
		fprintf (tcp_out, "Range: bytes=%s\n", range_str);

	fprintf (tcp_out, "Connection: close\n");

	fprintf (tcp_out, "\n");
	fflush (tcp_out);
	t_push (tcp_fd);

	/*
	 *	Agora lê linhas até uma linha em branco
	 */
	for (EVER)
	{
		if ((n = www_read_line (area, BLSZ, &byte_count)) < 0)
			goto eof;

		if (n == 0)
			break;

		printf ("%s\n", area);
	}

	/*
	 *	Lê o HTML
	 */
	if ((cp = strrchr (page_nm, '/')) != NOSTR)
		page_nm = cp + 1;

	if   (pflag)
	{
		while ((n = www_read_line (area, BLSZ, &byte_count)) >= 0)
			printf ("%s\n", area);
	}
	elif (gflag)
	{
		if (access (page_nm, F_OK) >= 0)
		{
			fprintf (stderr, "O arquivo \"%s\" Já existe. Reescreve? (n): ", page_nm);

			if (askyesno () <= 0)
				goto eof;
		}

		if ((fp = fopen (page_nm, "w")) == NOFILE)
		{
			error ("*Não consegui abrir \"%s\"", page_nm);
		}
		else
		{
			while ((n = fread (area, 1, sizeof (area), tcp_in)) > 0)
			{
				if (write (fileno (fp), area, n) != n)
					error ("$*Erro na escrita de \"%s\"", page_nm);
			}

			if (n < 0)
				error (NOSTR);
		}
	}
	else
	{
		int	linhas = 0;

		byte_count = 0;

		while ((n = www_read_line (area, BLSZ, &byte_count)) >= 0)
			linhas++;

		printf ("Recebi %d linhas (%d bytes)\n", linhas, byte_count);
	}

	/*
	 *	Termina a conexão
	 */
    eof:
	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	return (0);

}	/* end www */

/*
 ****************************************************************
 *	Lê uma linha do TCP até um <nl>				*
 ****************************************************************
 */
int
www_read_line (char *area, int count, int *byte_count)
{
	register char	*cp = area;
	const char	*end_area = cp + count - 1;
	register int	c;

	/*
	 *	Retorna o número de caracteres lidos
	 */
	for (EVER)
	{
		c = getc (tcp_in);

		if (c < 0)	/* EOF ou erro */
			return (-1);

		(*byte_count)++;

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
#undef	DEBUG
#ifdef	DEBUG
		if (vflag)
			error ("Li \"%s\"", area);
#endif	DEBUG
		return (cp - area);

	}	/* end for (EVER) */

}	/* end www_read_line */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	args;
	register const char	*fmt = format;
	char			err, abrt;
	int			event, save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (global_tcp_fd);
	
			fprintf (stderr, " (%s)", t_strevent (global_tcp_fd, event));
	
			if (event ==  T_DISCONNECT)
				{ fprintf (stderr, "\r\n"); quit (1); }
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	fprintf (stderr, "\r\n");

	if (abrt == '$')
		tcp_abort ();

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (void)
{
	t_snddis (global_tcp_fd, (T_CALL *)NULL);

	quit (1);

}	/* end tcp_abort */
/*
 ****************************************************************
 *	Restaura o estado do terminal e termina			*
 ****************************************************************
 */
void
quit (int exit_code)
{
	exit (exit_code);

}	/* end quit */

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
		"%s - pequeno cliente WWW para testes\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-pgv] [-r <intervalo>] [-n <nó_remoto>] [<página>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-p: Imprime a <página> remota na tela\n"
		"\t-g: Salva a <página> remota no diretório corrente\n"
		"\t-v: Verboso\n"
		"\t-r: Dá o <intervalo> no formato começo-final\n"
		"\t-n: Fornece o <nó_remoto> (ao invés de usar o do TNET do ambiente)\n"
	);

	exit (2);

}	/* end help */
