/*
 ****************************************************************
 *								*
 *			nettime_c.c				*
 *								*
 *	Time protocol (cliente)					*
 *								*
 *	Versão	3.0.0, de 25.11.92				*
 *		3.0.1, de 23.08.97				*
 *								*
 *	Módulo: TIME_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.1, de 23.08.97";

#define	TIME_PORT 	37
#define	TIME_OFFSET	2208988800	/* No. de segundos 1970 - 1900 */

#define	elif	else if

const char	tcp_dev_nm[] = "/dev/itntcp";

entry int	sflag;		/* Atualiza o tempo */
entry int	cflag;		/* Comparação */
entry int	dflag;		/* Imprime sem conversão */
entry int	vflag;		/* Verbose */

entry char	local_node_nm[80]; /* Nome do nó local */

entry INADDR	local_cntl_addr; /* Endereço local de controle */

entry int	tcp_fd;	/* Descritor da linha de controle */

entry int	snd_dis;	/* Em caso de erro, desconecta */

/*
 ******	Protótipos de funções ***********************************
 */
void		times_cmp (const char *, int);
void		error (const char *, ...);
void		tcp_abort (int);
void		help (void);

/*
 ****************************************************************
 *	Protocolo de Tempo (cliente)				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	INADDR		serv_addr, remote_addr, bind_addr;
	T_BIND		bind;
	T_CALL		snd_call, rcv_call;
	char		*cp;
	time_t		remote_time, delta_time;
	const char	*remote_node_nm;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "scdvH")) != EOF)
	{
		switch (opt)
		{
		    case 's':			/* Atualiza */
			sflag++;
			break;

		    case 'c':			/* Compara tempos */
			cflag++;
			break;

		    case 'd':			/* Sem conversão */
			dflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	nettime [-sdv] <nó remoto>
	 */
	argv += optind;
	argc -= optind;

	if (argc > 1)
		help ();

	if ((remote_node_nm = argv[0]) == NOSTR)
		{ error ("Não foi dado o <nó remoto>\n"); help (); }

	/*
	 *	Abre o dispositivo TCP de contrôle
	 */
	if ((tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	/*
	 *	Obtém o endereço nó remoto
	 */
	if ((remote_addr.a_addr = t_node_to_addr (tcp_fd, remote_node_nm, &local_cntl_addr.a_addr)) == -1)
		error ("$*Não consegui obter o endereço de \"%s\"", remote_node_nm);

	remote_addr.a_port = TIME_PORT;

	/*
	 *	Obtém o nome do nó local
	 */
	if ((cp = t_addr_to_node (tcp_fd, local_cntl_addr.a_addr)) == NOSTR)
		error ("$Não consegui obter o nome do nó local");

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
		error ("$*Não consegui associar um endereço local\n");

	if (vflag > 1)
		printf ("Port_no de controle obtido: %d\n", ((INADDR *)bind.addr.buf)->a_port);

	/*
	 *	Executa o "connect"
	 */
	snd_call.addr.len    = sizeof (INADDR);
	snd_call.addr.buf    = &remote_addr;

	rcv_call.addr.maxlen = sizeof (INADDR);
	rcv_call.addr.buf    = &serv_addr;

	if (t_connect (tcp_fd, &snd_call, &rcv_call) < 0)
	{
		if (errno == TLOOK && t_look (tcp_fd) == T_DISCONNECT)
		{
			error ("A conexão foi recusada por \"%s\"", remote_node_nm);
			exit (1);
		}

		error ("$*Não consegui estabelecer conexão");
	}

	snd_dis++;	/* De agora adiante, erros desconectam */

	if (vflag)
		printf ("Conectado a \"%s\"\n", remote_node_nm);

	/*
	 *	Recebe o tempo
	 */
	if (t_rcv (tcp_fd, &remote_time, sizeof (remote_time), (int *)NULL) != sizeof (remote_time))
		error (NOSTR);

#ifdef	LITTLE_ENDIAN
	remote_time = long_endian_cv (remote_time);
#endif	LITTLE_ENDIAN

	if (dflag)
		printf ("%u\t%P\n", remote_time, remote_time);

	remote_time -= TIME_OFFSET;

	/*
	 *	Atualiza o tempo local, se foi pedido
	 */
	if (sflag && stime (&remote_time) < 0)
		error ("*Não consegui atualiza a data/hora");

	/*
	 *	Guarda logo a diferença de tempos
	 */
	delta_time = remote_time - time ((time_t *)NULL);

	/*
	 *	Imprime o tempo remoto
	 */
	printf ("%s", brasctime (localtime (&remote_time)));

	/*
	 *	Imprime a diferença de tempo
	 */
	if (cflag)
		times_cmp (remote_node_nm, delta_time);

	/*
	 *	Termina a conexão
	 */
	if (t_sndrel (tcp_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("$*Erro no RCVREL");

	return (0);

}	/* end nettime */

/*
 ****************************************************************
 *	Compara o tempo local/remoto				*
 ****************************************************************
 */
void
times_cmp (const char *remote_node_nm, int seg)
{
	int		dias, hor, min; 
	char		atrasado = 0;

	/*
	 *	Caso raro, ...
	 */
	if (seg == 0)
	{
		printf
		(	"\nO nó \"%s\" está com a mesma data/hora\n",
			remote_node_nm
		);
		return;
	}

	/*
	 *	Verifica se está adiantado/atrasado
	 */
	if (seg < 0)
		{ seg = -seg; atrasado++; }

	/*
	 *	Calcula a diferença em dias, horas, ...
	 */
	min  = seg / 60;
	seg  = seg % 60;

	hor  = min / 60;
	min  = min % 60;

	dias = hor / 24;
	hor  = hor % 24;

	printf
	(	"\nO nó \"%s\" está %s de",
		remote_node_nm,
		atrasado ? "atrasado" : "adiantado"
	);

	if (dias)
		printf (" %d dia(s),", dias);
	
	printf
	(	" %02d:%02d:%02d\n",
		hor, min, seg
	);

}	/* end cmd_times */

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
			event = t_look (tcp_fd);
	
			fprintf (stderr, " (%s)", t_strevent (tcp_fd, event));
	
			if (event ==  T_DISCONNECT)
				{ putc ('\n', stderr); exit (1); }
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	putc ('\n', stderr);

	if (abrt == '$')
		tcp_abort (1);

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
	if (snd_dis)
		t_snddis (tcp_fd, (T_CALL *)NULL);

	exit (n);

}	/* end tcp_abort */

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
		"%s - obtém a data/hora corrente de um nó remoto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-scdv] <nó_remoto>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Sem opções, imprime o tempo obtido (formato legível)\n"
		"\t-s: Atualiza o tempo local a partir do tempo remoto obtido\n"
		"\t-c: Compara os tempos local/remoto\n"
		"\t-d: Imprime o tempo obtido em segundos (sem conversão)\n"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
