/*
 ****************************************************************
 *								*
 *			echomon_pr.c				*
 *								*
 *	Imprime o arquivo de registro de "echomon"		*
 *								*
 *	Versão	3.0.0, de 24.03.96				*
 *		3.0.0, de 03.05.96				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <termio.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 03.05.96";

#define TIME_INTERVAL	15	/* Para o modo contínuo */

#define	EVER	;;
#define	NOSTR   (char *)NULL
#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

/*
 ****** Opções e parâmetros *************************************
 */
entry const char *pgname;	/* Nome do programa */

entry const char *my_node_name;	/* Nome do próprio nó */

entry int	time_interval = TIME_INTERVAL;

entry int	vflag;		/* Verbose */

/*
 ****** Variáveis globais ***************************************
 */
entry char	echomonlog[] = "/usr/pub/etc/echomonlogXXXX";

#define echomonlog_index 23

#if (0)	/*******************************************************/
const char	*echomonlog = "/usr/pub/etc/echomonlog";
#endif	/*******************************************************/

entry TERMIO	old_termio, new_termio;

/*
 ******	Tabela dos diversos nós *********************************
 */
entry int	n_node;		/* Número real de nós */

#define NNODE	100

typedef struct
{
	char	*n_name;	/* Nome do nó */
	char	*n_city;	/* Local do nó */
	long	n_addr;		/* Endereço do nó */
	int	n_time;		/* Round Trip time */

}	NODE;

entry NODE	node_status[NNODE + 1];

/*
 ****** Protótipos de funções ***********************************
 */
void		print_node_info (const NODE *);
void		sig_catch (int, void *, void *);
void		help (void);

/*
 ****************************************************************
 *	Imprime o arquivo de registro de "echomon"		*
 ****************************************************************
 */
main (int argc, register const char *argv[])
{
	register int	opt;
	register FILE	*fp;
	register NODE	*np;
	register char	*cp;
	const char	*time_str = NOSTR, *str;
	const char	*echomonlog_str = NOSTR;
	int		i, c, entry_size, continuous = 0;
	off_t		offset, first_entry_offset;
	STAT		s;
	char		area[128];

	/*
	 *	Pequena inicialização
	 */
	pgname = argv[0];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vt:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 't':			/* Interval time */
			time_str = optarg;
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
		echomonlog_str = argv[0];
	elif (argc > 1)
		help ();

	/*
	 *	Retira o tempo dado
	 */
	if (time_str != NOSTR && ((time_interval = strtol (time_str, &str, 0)) < 1 || *str != '\0'))
	{
		fprintf
		(	stderr,
			"%s: Intervalo de tempo inválido: \"%s\"\n",
			pgname, time_str
		);
		exit (1);
	}

	/*
	 *	Acha um Nome para "log"
	 */
	if (echomonlog_str == NOSTR)
	{
		for (i = 0; /* abaixo */; i++)
		{
			sprintf (echomonlog + echomonlog_index, "%04d", i);
	
			if (access (echomonlog, F_OK) < 0)
				break;
		}

		sprintf (echomonlog + echomonlog_index, "%04d", i - 1);

		echomonlog_str = echomonlog;
	}

	/*
	 *	Abre o "echomonlog"
	 */
	if ((fp = fopen (echomonlog_str, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, echomonlog, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Lê o nome do próprio nó
	 */
	if (fngets (area, sizeof (area), fp) != area)
	{
	    prem_eof:
		fprintf
		(	stderr,
			"%s: EOF prematuro do arquivo \"%s\"\n",
			pgname, echomonlog
		);
		exit (1);
	}

	if ((cp = strchr (area, '.')) != NOSTR)
		cp++;
	else
		cp = area;

	if ((my_node_name = strdup (cp)) == NOSTR)
	{
	    no_mem:
		fprintf
		(	stderr,
			"%s: Memória esgotada\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Lê o número real de nós
	 */
	if ((unsigned)(n_node = getw (fp)) > 200)
	{
		fprintf
		(	stderr,
			"%s: %d nós? O arquivo \"%s\" está provavelmente "
			"com o formato errado\n",
			pgname, n_node, echomonlog
		);
		exit (1);
	}

	/*
	 *	Le os nomes do nós remotos
	 */
	for (np = node_status, i = n_node; i > 0; np++, i--)
	{
		if (np >= &node_status[NNODE])
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" contém nós demais\n",
				pgname, echomonlog
			);
			exit (1);
		}

		if (fngets (area, sizeof (area), fp) != area)
			goto prem_eof;

		if ((np->n_name = strdup (area)) == NOSTR)
			goto no_mem;

		if (fngets (area, sizeof (area), fp) != area)
			goto prem_eof;

		if ((np->n_city = strdup (area)) == NOSTR)
			goto no_mem;

		np->n_addr = getw (fp);

	}	/* end (lendo o arquivo "echomonlog") */

	/*
	 *	Guarda o "offset" da primeira entrada
	 */
	first_entry_offset = ftell (fp);

	/*
	 *	Coloca a entrada em modo "raw"
	 */
	ioctl (1, TCGETS, &old_termio);
	new_termio = old_termio;
	new_termio.c_lflag &= ~(ICANON|ECHO);
	new_termio.c_cc[VPAGE] = 0;
	ioctl (1, TCSETNR, &new_termio);

	/*
	 *	Prepara o término do programa
	 */
	signal (SIGINT,  sig_catch);
	signal (SIGTERM, sig_catch);

	/*
	 *	Posiciona para a última entrada
	 */
	if (fstat (fileno (fp), &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\"\n",
			pgname, echomonlog
		);
		ioctl (1, TCSETAW, &old_termio);
		exit (1);
	}

	entry_size = (1 + 1 * n_node) * sizeof (int);

	offset = s.st_size - entry_size;

	/*
	 *	Verifica se já tem dados computados
	 */
	if (offset < first_entry_offset)
		{ ioctl (1, TCSETAW, &old_termio); exit (0); }

	/*
	 *	Imprime a tabela
	 */
	for (EVER)
	{
		time_t		Time;

		/*
		 *	Posiciona o arquivo
		 */
		if (fseek (fp, offset, SEEK_SET) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui posicionar \"%s\" no endereço %d\n",
				pgname, echomonlog, offset
			);
			ioctl (1, TCSETAW, &old_termio);
			exit (1);
		}

		/*
		 *	Le o tempo
		 */
		Time = getw (fp);
		
		/*
		 *	Le os valores
		 */
		for (np = node_status; np < &node_status[n_node]; np++)
		{
			np->n_time = getw (fp);
		}

		if (feof (fp))
		{
			fprintf
			(	stderr,
				"%s: EOF prematuro do arquivo \"%s\"\n",
				pgname, echomonlog
			);
			ioctl (1, TCSETAW, &old_termio);
			exit (1);
		}

		/*
		 *	Limpa a tela e põe o cabeçalho
		 */
		printf
		(	"\n\tTROPIX ECHOMONITOR (%s, %-24.24s)\n\n",
			my_node_name, btime (&Time)
		);

		printf ("Round Trip (ms)\n\n");

		/*
		 *	Escreve a tabela
		 */
		for (i = 0, np = node_status; np < &node_status[NNODE]; np++, i++)
		{
			if (np->n_name == NOSTR)
				break;

			if (i > 0 && i >= 15)
			{
				if (!continuous)
				{
					printf ("\nTecle para a página seguinte: ");
					fflush (stdout); getchar ();
					printf ("\n\n");
				}
				else
				{
					sleep (time_interval);
				}

				i = 0;

#if (0)	/*******************************************************/
				printf ("\nTecle para continuar\n");
				getchar (); i = 0; putchar ('\n');
#endif	/*******************************************************/
			}

			print_node_info (np);
		}

		/*
		 *	Emite o MENU
		 */
		printf ("\nTecle \"?\" para instruções\n");

		if (!continuous)
			{ prompt: printf ("\n> "); }
		else
			sleep (time_interval);

		/*
		 *	Obtém o novo tamanho do arquivo
		 */
		if (fstat (fileno (fp), &s) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o estado de \"%s\"\n",
				pgname, echomonlog
			);
			ioctl (1, TCSETAW, &old_termio);
			exit (1);
		}

		if (continuous && ioctl (0, TCNREAD) <= 0)
			{ offset = s.st_size - entry_size; continue; }
		else
			continuous = 0;

		for (i = 0; /* vazio */; /* vazio */)
		{
			c = getchar ();

			if (c >= '0' && c <= '9')
				i = i * 10 + c - '0';
			else
				break;
		}

		if (i <= 0)
			i = 1;

		switch (c)
		{
		    case '\n':
		    case '\r':
		    case '+':
			if (offset + (i + 1) * entry_size <= s.st_size)
				offset += i * entry_size;
			else
				offset = s.st_size - entry_size;
			break;

		    case '-':
			if (offset - i * entry_size >= first_entry_offset)
				offset -= i * entry_size;
			else
				offset  = first_entry_offset;
			break;

		    case 'c':
			continuous++;
			break;

		    case 'q':
		    case 'Q':
			putchar ('\n');
			ioctl (1, TCSETAW, &old_termio);
			exit (0);

		    case '?':
		    default:
			printf
			(	"\nTecle:\n"
				"	\"N+\" ou \"N<nl>\" para listar a N-ésima amostra seguinte,\n"
				"	\"N-\" para listar a N-ésima amostra anterior,\n"
				"	\"c\" para entrar no modo contínuo, ou\n"
				"	\"q\" para terminar o programa\n"
			);

			goto prompt;

		}	/* end echomon_pr */

	}	/* end for (EVER) */

}	/* end echomon_pr */

/*
 ****************************************************************
 *	Imprime os dados deste nó				*
 ****************************************************************
 */
void
print_node_info (register const NODE *np)
{
	if (np->n_time == -1)
	{
		printf
		(	"      -  %-32.32s  %-16.16s %s\n",
			np->n_name, np->n_city,
			t_addr_to_str (np->n_addr)
		);
	}
	else
	{
		printf
		(	"%7d  %-32.32s  %-16.16s %s\n",
			np->n_time, np->n_name, np->n_city,
			t_addr_to_str (np->n_addr)
		);
	}

}	/* end print_node_info */

/*
 ****************************************************************
 *	Intercepta o SIGINT e SIGTERM				*
 ****************************************************************
 */
void
sig_catch (int signo, void *pc, void *fa)
{
	ioctl (1, TCSETAW, &old_termio);

	exit (0);

}	/* end sig_catch */

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
		"%s - imprime o arquivo de registro de \"echomon\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [-t <intervalo>] [<echomonlog>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\t-t: Imprime amostragens a cada <intervalo> seg., ao invés "
			"de " XSTR (TIME_INTERVAL) " seg.\n"
	);

	exit (2);

}	/* end help */
