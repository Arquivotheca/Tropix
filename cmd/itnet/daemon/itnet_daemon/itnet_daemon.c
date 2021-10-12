/*
 ****************************************************************
 *								*
 *			itnet_daemon.c				*
 *								*
 *   Inicializa a INTERNET e processa a entrada de datagramas	*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		4.9.0, de 31.08.06				*
 *								*
 *	Módulo: Internet					*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/itnet.h>
#include <sys/ppp.h>
#include <sys/tty.h>
#include <sys/utsname.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <setjmp.h>
#include <xti.h>

#pragma	getenv ($DIR)

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 31.08.06";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#ifndef	$DIR
#error	***** Falta definir DIR
#endif

#define SERV_DAEMON_NM	"serv_daemon"

/*
 ****** Variáveis diversas **************************************
 */
entry char	*big_pgname;	/* Nome integral do programa */

entry int	pg_nm_sz;	/* Tamanho do nome do programa */

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

entry long	in_daemon_pid,	/* Os PIDs dos filhos */
		out_daemon_pid,
		nfs_daemon_pid,
		name_daemon_pid;

entry jmp_buf	wait_env;	/* Estado para desvio não local */

entry char	msg[40];	/* Para mensagem de erro */

entry char	error_msg_to_log; /* Não nulo se mensagens para "itnetlog" */

entry int	global_fdaemon;	/* Cópia do descritor */

/*
 ****** Tabelas da INTERNET *************************************
 */
const char	itnet_dev_nm[] = "/etc/itnetdev";

const char	itnet_serv_nm[] = "/etc/itnetserv";

const char	itnet_export_nm[] = "/etc/exports";

entry int	N_DNS,		/* Quasi-constantes */
		N_ROUTE,
		N_EXPORT;

entry DNS	*dns;		/* Ponteiros para as tabelas */			
entry ROUTE	*route;
entry EXPORT	*export;

entry PPP_STATUS ppp_status;	/* Para o PPP */

/*
 ****** Tabela de servidores ************************************
 */
typedef struct
{
	long	s_pid;		/* O PID do servidor */
	char	s_old_state;	/* Estado antigo */
	char	s_new_state;	/* Estado antigo */
	char	s_prot_nm;	/* Nome do protocolo ('t' == TCP, 'u' == UDP) */
	int	s_listen_fd;	/* O FD do EP do servidor */
	int	s_port_no;	/* No. do port */
	int	s_max_conn_no;	/* No. máximo de conexões por cliente */
	char	*s_serv_nm;	/* Nome do servidor */
	char	*s_serv_cmd;	/* Comando completo para o servidor */

}	SERV;

entry SERV	*itnet_serv;	/* Ponteiro para a tabela */

entry int	itnet_serv_sz;	/* No. de entradas da tabela */

/*
 ****** Tabela de servidores de nomes ***************************
 */
#define DOMAIN_NAME_SERVER_NM	"name_daemon"

entry IPADDR	nmservtb[DNS_SERVER_CNT + 1];	/* A tabela de servidores de nomes */

entry int	nmservtb_index;	/* Próxima entrada da tabela de servidores */

/*
 ******	Tabela de Velocidades ***********************************
 */
typedef	struct
{
	char	speedname[6];
	short	speed;

}	SPEED;

const SPEED	speedtb[] =
{
	"110",		B110,
	"134",		B134,
	"150",		B150,
	"200",		B200,
	"300",		B300,
	"600",		B600,
	"1200",		B1200,
	"1.2",		B1200,
	"1800",		B1800,
	"1.8",		B1800,
	"2400",		B2400,
	"2.4",		B2400,
	"4800",		B4800,
	"4.8",		B4800,
	"9600",		B9600,
	"9.6",		B9600,
	"19200",	B19200,
	"19.2",		B19200,
	"38400",	B38400,
	"38.4",		B38400,
	"57600",	B57600,
	"57.6",		B57600,
	"115200",	B115200,
	"115.2",	B115200,
	""

}	/* end speed */;

/*
 ****** Protótipos de funções ***********************************
 */
void		exec_in_daemon (int);
void		exec_out_daemon (int);
void		exec_nfs_daemon (int);
void		read_itnet_dev (void);
char		*proc_options (char *, int);
char		*edit_dev_file_msg (int);
void		read_itnet_serv (void);
void		read_itnet_export (void);
char		*edit_serv_file_msg (int);
void		re_read_itnet_serv (void);
void		exec_nm_serv (int fd);
void		exec_serv (void);
void		re_exec_serv (int, ...);
int		listen_fork (SERV *);
void		put_server_in_core (char *);
void		out_of_memory (void);
void		sig_daemon_abort (int, ...);
void		daemon_abort (void);
void		error (const char *, ...);
void		help (void);

/*
 ****************************************************************
 *   Inicializa a INTERNET e processa a entrada de datagramas	*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt;
	int		fdaemon;
	long		pid;

	big_pgname = (char *)pgname; pg_nm_sz = strlen (big_pgname);

	if ((pgname = strrchr (pgname, '/')) != NOSTR)
		pgname++;
	else
		pgname = big_pgname;

#if (0)	/*******************************************************/
	big_pgname = (char *)argv[0];
	pg_nm_sz = strlen (argv[0]);

	if (pgname = strrchr (argv[0], '/'))
		pgname++;
	else
		pgname = argv[0];
#endif	/*******************************************************/

	/*
	 *	Verifica se é SUPERUSUÁRIO
	 */
	if (geteuid () != 0)
		error ("$O usuário efetivo não é SUPERUSUÁRIO");

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
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

	argv += optind;
	argc -= optind;

	/*
	 *	Passa para "background"
	 */
	settgrp (0);

	signal (SIGINT,  sig_daemon_abort);
	signal (SIGQUIT, SIG_IGN);

	if ((pid = fork ()) < 0)
		error ("$*Não consegui dar \"fork\"");

	if (pid > 0)
		exit (0);

	/*
	 *	Abre o dispositivo para os DAEMONs
	 */
	if ((fdaemon = open ("/dev/itndaemon", O_RDWR)) < 0)
		error ("$*Não consegui inicializar a rede");

	global_fdaemon = fdaemon;

	/*
	 *	Aloca espaço para a tabela DNS
	 */
	if ((N_DNS = ioctl (fdaemon, I_GET_DNS, NODNS)) < 0)
		error ("$*Não consegui obter o tamanho da tabela DNS");

	if (vflag)
		printf ("DNS com %d entradas\n", N_DNS);

	if ((dns = calloc (N_DNS, sizeof (DNS))) == NOVOID)
		error ("$Não consegui memória para a tabela DNS");

	/*
	 *	Aloca espaço para a tabela ROUTE
	 */
	if ((N_ROUTE = ioctl (fdaemon, I_GET_ROUTE, NOROUTE)) < 0)
		error ("$*Não consegui obter o tamanho da tabela ROUTE");

	if (vflag)
		printf ("ROUTE com %d entradas\n", N_ROUTE);

	if ((route = calloc (N_ROUTE, sizeof (ROUTE))) == NOVOID)
		error ("$Não consegui memória para a tabela ROUTE");

	/*
	 *	Aloca espaço para a tabela EXPORT
	 */
	if ((N_EXPORT = ioctl (fdaemon, I_GET_EXPORT, NOEXPORT)) < 0)
		error ("$*Não consegui obter o tamanho da tabela EXPORT");

	if (vflag)
		printf ("EXPORT com %d entradas\n", N_EXPORT);

	if ((export = calloc (N_EXPORT, sizeof (EXPORT))) == NOVOID)
		error ("$Não consegui memória para a tabela EXPORT");

	/*
	 *	Le a tabela de dispositivos
	 */
	read_itnet_dev ();

	if (exit_code)
		daemon_abort ();

	/*
	 *	Le o arquivos EXPORTs do NFS
	 */
	read_itnet_export ();

	/*
	 *	Cria um filho para o DAEMON de saída
	 */
	if ((out_daemon_pid = fork ()) < 0)
		error ("$*Não consegui dar \"fork\"");

	if (out_daemon_pid == 0)
		exec_out_daemon (fdaemon);

	/*
	 *	Cria um filho para o DAEMON de entrada
	 */
	if ((in_daemon_pid = fork ()) < 0)
		error ("$*Não consegui dar \"fork\"");

	if (in_daemon_pid == 0)
		exec_in_daemon (fdaemon);

	/*
	 *	Cria um filho para o DAEMON de RFC/NFS
	 */
	if ((nfs_daemon_pid = fork ()) < 0)
		error ("$*Não consegui dar \"fork\"");

	if (nfs_daemon_pid == 0)
		exec_nfs_daemon (fdaemon);

	/*
	 *	Le a tabelas de servidores
	 */
	read_itnet_serv ();

	if (exit_code)
		daemon_abort ();

	/*
	 *	Executa o servidor de nomes
	 */
	exec_nm_serv (fdaemon);

	/*
	 *	Executa o servidor
	 */
	exec_serv ();

}	/* end itnet_daemon */

/*
 ****************************************************************
 *	DAEMON de entrada					*
 ****************************************************************
 */
void
exec_in_daemon (int fdaemon)
{
	void		*vector[3] = { dns, route, export };

	if (pg_nm_sz >= 9)
		strcpy (big_pgname, "in_daemon");

	nice (-20);

	if (ioctl (fdaemon, I_IN_DAEMON, vector) < 0)
		error ("$*Não consegui executar o DAEMON de entrada");

	error ("O DAEMON de entrada retornou do ioctl");

	for (EVER)
		pause ();

}	/* end exec_in_daemon */

/*
 ****************************************************************
 *	DAEMON de saída						*
 ****************************************************************
 */
void
exec_out_daemon (int fdaemon)
{
	if (pg_nm_sz >= 10)
		strcpy (big_pgname, "out_daemon");

	nice (-20);

	if (ioctl (fdaemon, I_OUT_DAEMON) < 0)
		error ("$*Não consegui executar o DAEMON de saída");

	error ("O DAEMON de saída retornou do ioctl");

	for (EVER)
		pause ();

}	/* end exec_out_daemon */

/*
 ****************************************************************
 *	DAEMON RPC/NFS						*
 ****************************************************************
 */
void
exec_nfs_daemon (int fdaemon)
{
	if (pg_nm_sz >= 10)
		strcpy (big_pgname, "nfs_daemon");

   /***	nice (-20);	***/

	if (ioctl (fdaemon, I_NFS_DAEMON) < 0)
		error ("$*Não consegui executar o DAEMON de NFS");

	error ("O DAEMON de NFS retornou do ioctl");

	for (EVER)
		pause ();

}	/* end exec_nfs_daemon */

/*
 ****************************************************************
 *	Le a Tabela de dispositivos/endereços			*
 ****************************************************************
 */
void
read_itnet_dev (void)
{
	DNS		*dp;
	ROUTE		*rp;
	FILE		*fp;
	char		*cp;
	int		lineno = 0;
	char		first_last_addr = 1;
	char		last_dev_nm[5+16] = { '\0' };
	int		last_fd = -1;
	char		buf[128];
	UTSNAME		uts;
	IPADDR		last_addr, last_my_addr = -1;
	IPADDR		net_addr;

	if (uname (&uts) < 0)
		error ("$*Não consegui obter o UTS");

	if ((fp = fopen (itnet_dev_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", itnet_dev_nm);

	if (vflag)
		printf ("\n");

	/*
	 *	Prepara a entrada de ROUTE para o próprio nó
	 */
	dp = dns, rp = route;

	rp->r_mask = -1;	/* 255,255,255,255 */
	rp->r_net_addr = MY_IP_ADDR;
	rp->r_my_addr  = MY_IP_ADDR;
   /***	rp->r_gateway_addr = 0; ***/
   /***	rp->r_inode = 0; ***/
	strcpy (rp->r_dev_nm, "<local>");

	rp++;

	/*
	 *	Le e processa o arquivo
	 */
	for (/* acima */; fngets (buf, sizeof (buf), fp) == buf; /* abaixo */)
	{
		lineno++;

		if (vflag)
			printf ("%s\n", buf);

		if (buf[0] == '#')
			continue;

		if ((cp = strtok (buf, " \t,")) == NOSTR)
			continue;

		/*
		 *	Verifica se a linha começa por um endereço
		 */
		if (cp[0] >= '0' && cp[0] <= '9')
		{
			if ((last_addr = t_str_to_addr (cp)) == -1)
			{
			    bad_addr:
				error
				( 	"%s: Endereço inválido: \"%s\"",
					edit_dev_file_msg (lineno), cp
				);
				exit_code++; continue;
			}

			if (first_last_addr)
				{ first_last_addr = 0; last_addr = MY_IP_ADDR; }

			while ((cp = strtok (NOSTR, " \t,")) != NOSTR)
			{
				if (dp >= &dns[N_DNS - 1])
				{
					error
					(	"%s: O arquivo contém nomes de nós demais",
						edit_dev_file_msg (lineno)
					);
					exit_code++; continue;
				}

				strcpy (dp->d_host_nm, cp);
				dp->d_host_addr	= last_addr;

				dp++;

			}	/* end processando vários nomes de nós */

			continue;

		}	/* end linha começando por endereço */

		/*
		 *	Verifica se a linha começa por "name_server"
		 */
		if (streq (cp, "name_server"))
		{
			while ((cp = strtok (NOSTR, " \t,")) != NOSTR)
			{
				if (nmservtb_index >= DNS_SERVER_CNT)
				{
					error
					(	"%s: O arquivo \"%s\" contém servidores de nomes demais",
						edit_dev_file_msg (lineno), itnet_dev_nm
					);
					exit_code++; continue;
				}

				if ((nmservtb[nmservtb_index++] = t_str_to_addr (cp)) == -1)
					goto bad_addr;

			}	/* end processando vários endereços */

			continue;
		}

		/*
		 *	Verifica se a linha começa por "mask"
		 */
		if (strncmp (cp, "mask=", 5) == 0)
		{
			if (last_fd < 0)
				continue;

			if (last_dev_nm[0] == '\0')
			{
				error
				( 	"%s: Ainda não foi dado nenhum dispositivo",
					edit_dev_file_msg (lineno)
				);
				exit_code++; continue;
			}

			if (rp >= &route[N_ROUTE - 1])
			{
				error
				(	"%s: O arquivo contém rotas demais",
					edit_dev_file_msg (lineno)
				);
				exit_code++; continue;
			}

			cp += 5;		/* Pula "mask=" */

			if (*cp == '(')		/* Pula "(" */
				cp++;

			if (cp[0] < '0' || cp[0] > '9')
				goto bad_addr;

			rp->r_mask = t_str_to_addr (cp);

			if ((cp = strtok (NOSTR, " \t,)")) == NOSTR)
				goto prem_eof;

			if ((rp->r_net_addr = t_str_to_addr (cp)) == -1)
				goto bad_addr;

			rp->r_my_addr = last_my_addr;
		   /***	rp->r_gateway_addr = ...; ***/
			rp->r_inode = (INODE *)last_fd;
			strcpy (rp->r_dev_nm, last_dev_nm + 5); /* Pula "/dev/" */

			/*
			 *	Completa o endereço ETHERNET, se for o caso
			 */
			if (ioctl (last_fd, TC_IS_ETHERNET, NULL) >= 0)
			{
				rp->r_ether_dev = 1;

				if (ioctl (last_fd, TC_GET_ETHADDR, &rp->r_my_ether_addr) < 0)
				{
					error
					(	"*%s: Não consegui obter o endereço ETHERNET de \"%s\"",
						edit_dev_file_msg (lineno), last_dev_nm
					);
					exit_code++; continue;
				}
			}

			/*
			 *	Verifica se segue "gateway"
			 */
			if ((cp = strtok (NOSTR, " \t,")) == NOSTR)
				{ rp++; continue; }

			if (strncmp (cp, "gateway=", 8) != 0)
			{
				error
				( 	"%s: Esperava \"gateway\" ao invés de \"%s\"",
					edit_dev_file_msg (lineno), cp
				);
				exit_code++; continue;
			}

			cp += 8;	/* Pula "gateway" */

			if (*cp == '\0')
				goto prem_eof;

			if ((rp->r_gateway_addr = t_str_to_addr (cp)) == -1)
				goto bad_addr;

			rp++; continue;
		}

		/*
		 *	Verifica se a linha começa por "netmask"
		 */
		if (strncmp (cp, "netmask=", 8) == 0)
		{
			if (last_fd < 0)
				continue;

			if (last_dev_nm[0] == '\0')
			{
				error
				( 	"%s: Ainda não foi dado nenhum dispositivo",
					edit_dev_file_msg (lineno)
				);
				exit_code++; continue;
			}

			if (rp >= &route[N_ROUTE - 1])
			{
				error
				(	"%s: O arquivo contém rotas demais",
					edit_dev_file_msg (lineno)
				);
				exit_code++; continue;
			}

			cp += 8;		/* Pula "netmask=" */

			if (cp[0] < '0' || cp[0] > '9')
				goto bad_addr;

			rp->r_mask = t_str_to_addr (cp);
			rp->r_net_addr = net_addr = (rp->r_mask & last_my_addr);
			rp->r_my_addr = last_my_addr;
		   	rp->r_gateway_addr = 0;

			rp->r_inode = (INODE *)last_fd;
			strcpy (rp->r_dev_nm, last_dev_nm + 5); /* Pula "/dev/" */

			/*
			 *	Completa o endereço ETHERNET, se for o caso
			 */
			if (ioctl (last_fd, TC_IS_ETHERNET, NULL) >= 0)
			{
				rp->r_ether_dev = 1;

				if (ioctl (last_fd, TC_GET_ETHADDR, &rp->r_my_ether_addr) < 0)
				{
					error
					(	"*%s: Não consegui obter o endereço ETHERNET de \"%s\"",
						edit_dev_file_msg (lineno), last_dev_nm
					);
					exit_code++; continue;
				}
			}

			rp++;		/* Esta entrada está completa */

			/*
			 *	Verifica se há "default"
			 */
			if ((cp = strtok (NOSTR, " \t,)")) == NOSTR)
				continue;

			if (strncmp (cp, "default", 7) != 0)
			{
				error
				( 	"%s: Palavra-chave desconhecida: \"%s\"\n",
					edit_dev_file_msg (lineno), cp
				);
				exit_code++; continue;
			}

			/*
			 *	Cria a entrada "default"
			 */
			if (rp >= &route[N_ROUTE - 1])
			{
				error
				(	"%s: O arquivo contém rotas demais",
					edit_dev_file_msg (lineno)
				);
				exit_code++; continue;
			}

			memmove (rp, rp - 1, sizeof (ROUTE));

			rp->r_mask	   = 0;
		   	rp->r_net_addr	   = 0;
		   /***	rp->r_my_addr	   = last_my_addr; ***/
		   	rp->r_gateway_addr = net_addr + 1;
		   	rp->r_dhcp	   = 0;
		   	rp->r_default	   = 1;

		   /***	rp->r_inode = (INODE *)last_fd; ***/
		   /***	strcpy (rp->r_dev_nm, last_dev_nm + 5); /* Pula "/dev/" ***/

			rp++; continue;

		}	/* end "netmask" */

		/*
		 *	Linha começando por dispositivo
		 */
		strcpy (last_dev_nm, cp);

		if ((last_fd = open (last_dev_nm, O_RDWR)) < 0)	/* LOCK ? */
		{
			error
			( 	"*%s: Não consegui abrir \"%s\"",
				edit_dev_file_msg (lineno), last_dev_nm
			);
			/*** exit_code++; ***/ continue;	/* Continua assim mesmo */
		}

		if (ioctl (last_fd, TCINTERNET, NULL) < 0)
		{
			error
			( 	"%s: O dispositivo \"%s\" não é INTERNET",
				edit_dev_file_msg (lineno), last_dev_nm
			);
			exit_code++; continue;
		}

		/*
		 *	Processa as opções do dispositivo
		 */
		cp = proc_options (last_dev_nm, last_fd);

		if (cp == NOSTR)
		{
		    prem_eof:
			error
			(	"%s: Final inesperado da linha",
				edit_dev_file_msg (lineno)
			);
			exit_code++; continue;
		}

		if (strncmp (cp, "dhcp", 4) == 0)
			{ rp->r_dhcp = 1; continue; }

		if ((last_my_addr = t_str_to_addr (cp)) == -1)
			goto bad_addr;

	}	/* lendo itnet_dev */

	/*
	 *	Coloca os marcadores de final
	 */
	dp->d_host_nm[0] = '\0';
	rp->r_dev_nm[0] = '\0';

	fclose (fp);

#ifdef	DEBUG
	/*
	 *	Imprime as tabelas
	 */
	if (!vflag)
		return;

	printf ("\nDNS:\n");

	for (dp = dns; dp->d_host_nm[0] != '\0'; dp++)
	{
		printf
		(	"%-15s  %s\n",
			t_addr_to_str (dp->d_host_addr),
			dp->d_host_nm
		);
	}

	printf ("\nROUTE:\n");

	for (rp = route; rp->r_dev_nm[0] != '\0'; rp++)
	{
		printf ("%-15s ", t_addr_to_str (rp->r_mask));
		printf ("%-15s ", t_addr_to_str (rp->r_net_addr));
		printf ("%-15s ", t_addr_to_str (rp->r_my_addr));
		printf ("%-15s ", t_addr_to_str (rp->r_gateway_addr));
		printf ("%4d ", (int)rp->r_inode);
		printf ("%s\n", rp->r_dev_nm);
	}
#endif	DEBUG

}	/* end read_itnet_dev */

/*
 ****************************************************************
 *	Processa o campo de opções				*
 ****************************************************************
 */
char *
proc_options (char *dev_nm, int fd)
{
	char		*options;
	TERMIO		mode;
	const char	*str;
	int		old_fifosz, new_fifosz;

	/*
	 *	Verifica se é um dispositivo ETHERNET
	 */
	if (ioctl (fd, TC_IS_ETHERNET, NULL) >= 0)
		return (strtok (NOSTR, " \t,"));

	/*
	 *	Obtém a TERMIO
	 */
	if (ioctl (fd, TCGETS, &mode) < 0)
	{
		error ("*Não consegui dar IOCTL no dispositivo \"%s\"", dev_nm);
		exit_code++;
	}

	for
	(	options = strtok (NOSTR, " \t,");
		options != NOSTR;
		options = strtok (NOSTR, " \t,")
	)
	{
		if   (streq (options, "clocal"))
		{
			mode.c_cflag |= CLOCAL;
		}
		elif (streq (options, "-clocal"))
		{
			mode.c_cflag &= ~CLOCAL;
		}
		elif (!strncmp (options, "fifo=", 5))
		{
			if   ((new_fifosz = strtol (options + 5, &str, 0)) < 1 || new_fifosz > 16)
			{
				error ("Tamanho do FIFO inválido: \"%s\"", options + 5);
				exit_code++;
			}
			elif ((old_fifosz = ioctl (fd, TCFIFOSZ, new_fifosz)) < 0)
			{
				error ("*Não consegui alterar o tamanho do FIFO");
				exit_code++;
			}
		}
		elif (!strncmp (options, "ppp=", 4))
		{
			PPP_STATUS	*sp = &ppp_status;
			const char	*vj_str = options + 4;

			memset (sp, 0, sizeof (PPP_STATUS));

		   /***	sp->ppp_lcp_state	= LCP_STATE_CLOSED; ***/
		   /***	sp->ppp_ipcp_state	= IPCP_STATE_CLOSED; ***/
		   /***	sp->ppp_my_addr_added	= 0; ***/

		   /***	sp->snd.ppp_magic_number = t.mu_time ^ t.mu_utime; ***/
		   /***	sp->snd.ppp_async_char_map = 0; ***/
		   /***	sp->snd.ppp_addr = 0; ***/
		   /***	sp->snd.ppp_proto_field_compression = 0; ***/
		   /***	sp->snd.ppp_addr_field_compression = 0; ***/
		   /***	sp->snd.ppp_vj_compression = 0; ***/
		   /***	sp->snd.ppp_vj_slot_compression = 0; ***/
		   /***	sp->snd.ppp_vj_max_slot = 0; ***/

		   /***	sp->rcv.ppp_magic_number = 0; ***/
		   /***	sp->rcv.ppp_async_char_map = 0; ***/
		   /***	sp->rcv.ppp_addr = 0; ***/
		   /***	sp->rcv.ppp_proto_field_compression = 0; ***/
		   /***	sp->rcv.ppp_addr_field_compression = 0; ***/
		   /***	sp->rcv.ppp_vj_compression = 0; ***/
		   /***	sp->rcv.ppp_vj_slot_compression = 0; ***/
		   /***	sp->rcv.ppp_vj_max_slot = 0; ***/

		   /***	memset (sp->ppp_char_map_vec, 0, sizeof (sp->ppp_char_map_vec)); ***/
		   /***	sp->ppp_slcompress_ptr = NULL;	***/

		   /***	memset (sp->ppp_char_map_vec, 1, 32); ***/
			sp->ppp_char_map_vec[PPP_FLAG] = 1;
			sp->ppp_char_map_vec[PPP_ESC] = 1;

			if   (streq (vj_str, "vj"))
			{
				sp->snd.ppp_vj_compression++;
				sp->snd.ppp_vj_slot_compression++;
				sp->rcv.ppp_vj_compression++;
			}
			elif (streq (vj_str, "novj"))
			{
			   /***	sp->snd.ppp_vj_compression = 0;	***/
			   /***	sp->snd.ppp_vj_slot_compression = 0; ***/
			   /***	sp->rcv.ppp_vj_compression = 0;	***/
			}
			else
			{
				error ("Opção PPP inválida: \"%s\"", vj_str);
				exit_code++;
			}

			if (ioctl (fd, TCPPPINT, sp) < 0)
			{
				error ("*Não consegui inicializar o modo PPP");
				exit_code++;
			}
		}
		else	/* Deve ser uma velocidade */
		{
			const SPEED	*ep;

			for (ep = speedtb; /* teste abaixo */; ep++)
			{
				if (ep->speedname[0] == '\0')
					goto out;

				if (streq (options, ep->speedname))
				{
					mode.c_cflag &= ~CBAUD;
					mode.c_cflag |= ep->speed;
					break;
				}
			}

		}	/* end percorrendo velocidade */

	}	/* end processando as opções */

	/*
	 *	Põe o novo modo
	 */
    out:
	if (ioctl (fd, TCSETAW, &mode) < 0)
	{
		error ("*Não consegui dar IOCTL na linha \"%s\"", dev_nm);
		exit_code++;
	}

	return (options);

}	/* end proc_options */

/*
 ****************************************************************
 *	Edita o nome do arquivo e número da linha		*
 ****************************************************************
 */
char *
edit_dev_file_msg (int lineno)
{
	sprintf (msg, " (%s, %d)", itnet_dev_nm, lineno);

	return (msg);

}	/* end edit_dev_file_msg */

/*
 ****************************************************************
 *	Le a Tabela de servidores				*
 ****************************************************************
 */
void
read_itnet_serv (void)
{
	SERV		*sp;
	FILE		*fp;
	STAT		s;
	char		new_state;
	char		*prot_nm, *port_str, *max_conn_str, *serv_cmd, *serv_nm;
	char		*serv_cmd_copy;
	const char	*str;
	int		lineno = 0, port_no, max_conn_no;
	char		area[128];

	if ((fp = fopen (itnet_serv_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", itnet_serv_nm);

	/*
	 *	Cria a tabela de servidores
	 */
	if ((itnet_serv = sp = malloc (sizeof (SERV))) == (SERV *)NULL)
		out_of_memory ();

	itnet_serv_sz = 1;

	sp->s_serv_nm = NOSTR;

	if (vflag)
		printf ("\n");

	/*
	 *	Le e processa o arquivo
	 */
	while (fngets (area, sizeof (area), fp) == area)
	{
		lineno++;

		if (vflag)
			printf ("%s\n", area);

		if (area[0] == '#' || area[0] == '\0')
			continue;

		new_state	= *strfield (area,   ':');
		prot_nm		=  strtok (strfield (NOSTR, ':'), "\t ");
		port_str	=  strtok (strfield (NOSTR, ':'), "\t ");
		max_conn_str	=  strtok (strfield (NOSTR, ':'), "\t ");
		serv_cmd	=  strfield (NOSTR, ':');

		/* Retira logo o nome do servidor */

		if ((serv_cmd_copy = strdup (serv_cmd)) == NOSTR)
			out_of_memory ();

		serv_nm   =  strtok (serv_cmd, "\t ");
#ifdef	DEBUG
		if (vflag) printf
		(	"state = '%c', prot_nm = \"%s\", port_addr = \"%s\", "
			"serv_cmd = \"%s\", serv_nm = \"%s\"\n",
			new_state, prot_nm, port_str, serv_cmd, serv_nm
		);
#endif	DEBUG
		/*
		 *	Examina o protocolo
		 */
		if (!streq (prot_nm, "udp") && !streq (prot_nm, "tcp"))
		{
			error
			(	"%s: Protocolo inválido: \"%s\"",
				edit_serv_file_msg (lineno), prot_nm
			);
			exit_code++; continue;
		}

		/*
		 *	Examina o "port" dado
		 */
		if ((port_no = strtol (port_str, &str, 0)) < 1 || *str != '\0')
		{
			error
			(	"%s: No de \"port\" inválido: \"%s\"",
				edit_serv_file_msg (lineno), port_str
			);
			exit_code++; continue;
		}

		/*
		 *	Examina o número máximo de conexões por cliente dado
		 */
		if ((max_conn_no = strtol (max_conn_str, &str, 0)) < 0 || *str != '\0')
		{
			error
			(	"%s: No de conexões máximas inválido: \"%s\"",
				edit_serv_file_msg (lineno), max_conn_str
			);
			exit_code++; continue;
		}

		/*
		 *	Verifica se a tabela já contém este servidor
		 */
		for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
		{
			if (sp->s_prot_nm != prot_nm[0])
				continue;

			if (sp->s_port_no != port_no)
				continue;

			error
			( 	"%s: Há mais de um servidor para: (%s, %d)",
				edit_serv_file_msg (lineno),
				sp->s_prot_nm == 'u' ? "udp" : "tcp",
				sp->s_port_no
			);

			exit_code++;

			break;

		}	/* end for (tabela) */

		/*
		 *	Aloca espaço para mais uma entrada
		 */
		itnet_serv_sz++;

		if ((itnet_serv = realloc (itnet_serv, itnet_serv_sz * sizeof (SERV))) == (SERV *)NULL)
			out_of_memory ();

		sp = &itnet_serv[itnet_serv_sz - 2];

		(sp + 1)->s_serv_nm = NOSTR;

	   /***	sp->s_old_state = ' ';	***/
		sp->s_new_state = new_state;

		/*
		 *	Ainda não tem EP aberto
		 */
		sp->s_listen_fd = -1;

		/*
		 *	Guarda o protocolo e o "port"
		 */
		sp->s_prot_nm	  = prot_nm[0];
		sp->s_port_no	  = port_no;
		sp->s_max_conn_no = max_conn_no;

		/*
		 *	Examina o nome do servidor
		 */
		sp->s_serv_cmd = serv_cmd_copy;

		if ((sp->s_serv_nm = strdup (serv_nm)) == NOSTR)
			out_of_memory ();

		if (new_state != ' ')
		{
			if (stat (sp->s_serv_nm, &s) < 0)
			{
				error
				( 	"*%s: Não consegui obter o estado do servidor \"%s\"",
					edit_serv_file_msg (lineno), serv_nm
				);
				exit_code++; continue;
			}

			if (access (sp->s_serv_nm, X_OK) < 0)
			{
				error
				( 	"*%s: O servidor \"%s\" não é executável",
					edit_serv_file_msg (lineno), serv_nm
				);
				exit_code++; continue;
			}

			if (s.st_mode & S_IMETX)
				put_server_in_core (sp->s_serv_nm);
		}

	}	/* lendo itnet_serv */

	fclose (fp);

}	/* end read_itnet_serv */

/*
 ****************************************************************
 *	Le o arquivo EXPORTS dos sistemas de arquivos NFS	*
 ****************************************************************
 */
void
read_itnet_export (void)
{
	FILE		*fp;
	const char	*str, *options = NOSTR;
	char		*abre;
	EXPORT		*ep = export - 1;
	const EXPORT	*end_ep = export + (N_EXPORT - 1);
	int		lineno = 0;
	char		area[128];

	/*
	 *	Verifica se o arquivo existe
	 */
	if ((fp = fopen (itnet_export_nm, "r")) == NOFILE)
	{
		if (errno != ENOENT)
			error ("*Não consegui abrir \"%s\"", itnet_export_nm);

		return;
	}

	/*
	 *	Le e processa o arquivo (por enquanto, um padrão por linha)
	 */
	while (fngets (area, sizeof (area), fp) == area)
	{
		lineno++;

		if (area[0] == '#' || area[0] == '\0')
			continue;

		if ((str = strtok (area, " \t")) == NOSTR)
			continue;

		if (++ep >= end_ep)
			{ error ("Tabela EXPORTs esgotada"); fclose (fp); return; }

		ep->e_ronly = 1;	/* Em principio */

		/* Insere o nome do diretório que será exportado */

		strscpy (ep->e_dir_path, str, sizeof (ep->e_dir_path));

		if ((str = strtok (NOSTR, " \t")) == NOSTR)
		{
			error
			(	"Arquivo \"%s\", linha %d: Faltando o nome do cliente",
				itnet_export_nm, lineno
			);
			ep--; continue;
		}

		if ((abre = strchr (str, '(')) != NOSTR)
			{ *abre++ = '\0'; options = strtok (abre, ",)"); }

		/* Insere o nome do cliente autorizado a montar o diretório */

		strscpy (ep->e_client_nm, str, sizeof (ep->e_client_nm));

		/* Examina as opções */

		for (/* acima */; options != NOSTR; options = strtok (NOSTR, ",)"))
		{
			if   (streq (options, "secure"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					itnet_export_nm, lineno, "secure"
				);
				ep->e_secure = 1;
			}
			elif (streq (options, "insecure"))
			{
				ep->e_secure = 0;
			}
			elif (streq (options, "ro"))
			{
				ep->e_ronly = 1;
			}
			elif (streq (options, "rw"))
			{
				ep->e_ronly = 0;
			}
			elif (streq (options, "sync"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					itnet_export_nm, lineno, "sync"
				);
				ep->e_sync = 1;
			}
			elif (streq (options, "async"))
			{
				ep->e_sync = 0;
			}
			elif (streq (options, "hide"))
			{
				error
				(	"Arquivo \"%s\", linha %d: \"%s\" ainda NÃO implementado",
					itnet_export_nm, lineno, "hide"
				);
				ep->e_hide = 1;
			}
			elif (streq (options, "nohide"))
			{
				ep->e_hide = 0;
			}
			elif (streq (options, "no_root_squash"))
			{
				ep->e_no_root_squash = 1;
			}
			elif (streq (options, "root_squash"))
			{
				ep->e_no_root_squash = 0;
			}
			elif (streq (options, "all_squash"))
			{
				ep->e_all_squash = 1;
			}
			elif (!strncmp (options, "anonuid=", 8))
			{
				if ((ep->e_anon_uid = strtol (options + 8, &str, 0)) < 0 || *str != '\0')
					error ("UID inválido: \"%s\"", options + 8);
				else
					ep->e_use_anon_ids = 1;
			}
			elif (!strncmp (options, "anongid=", 8))
			{
				if ((ep->e_anon_gid = strtol (options + 8, &str, 0)) < 0 || *str != '\0')
					error ("GID inválido: \"%s\"", options + 8);
				else
					ep->e_use_anon_ids = 1;
			}
			else
			{
				error
				(	"Arquivo \"%s\", linha %d: Palavra chave \"%s\" desconhecida",
					itnet_export_nm, lineno, options
				);
			}

		}	/* end while (options != NOSTR) */

		ep->e_active = 1;

	}	/* end for (lendo as linhas) */

	fclose (fp);

}	/* end read_itnet_export */

/*
 ****************************************************************
 *	Edita o nome do arquivo e número da linha		*
 ****************************************************************
 */
char *
edit_serv_file_msg (int lineno)
{
	sprintf (msg, " (%s, %d)", itnet_serv_nm, lineno);

	return (msg);

}	/* end edit_serv_file_msg */

/*
 ****************************************************************
 *	Cria um filho para o servidor de nomes			*
 ****************************************************************
 */
void
exec_nm_serv (int fdaemon)
{
	const char	**ap;
	const char	*argv[4];

	/*
	 *	Fornece a tabela ao núcleo
	 */
	if (ioctl (fdaemon, I_PUT_DNS_SERVER, nmservtb) < 0)
		error ("*Não consegui fornecer a tabela de servidores DNS");

	/*
	 *	Cria um processo novo
	 */
	if ((name_daemon_pid = fork ()) < 0)
		{ error ("*Não consegui dar \"fork\""); exit (1); }

	if (name_daemon_pid > 0)
		return;

	setpgrp ();

	/*
	 *	Passa o controle para o servidor de nomes:
	 *
	 *	serv_daemon [-v]
	 */
	ap = argv;

	*ap++ = DOMAIN_NAME_SERVER_NM;

	if (vflag)
		*ap++ = "-v";

	*ap   = NOSTR;

	execv (XSTR ($DIR) "/" DOMAIN_NAME_SERVER_NM, (const char **)argv);

	error ("*Não consegui dar \"exec\" para \"%s\"", DOMAIN_NAME_SERVER_NM);
	exit (1);

}	/* end exec_nm_serv */

/*
 ****************************************************************
 *	Cria os servidores					*
 ****************************************************************
 */
void
exec_serv (void)
{
	SERV		*sp;
	long		pid;
	int		status;
	int		print_child_status = ' ';

	/*
	 *	Cria um filho para cada "well known port"
	 */
	for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
	{
		if (sp->s_new_state != ' ')
			sp->s_pid = listen_fork (sp);
	}

	/*
	 *	Marca o ponto para retorno do SINAL
	 */
	if (pg_nm_sz >= 12)
		strcpy (big_pgname, "itnet_daemon");

	error_msg_to_log++; /* De agora em diante, mensagens para "itnetlog" */

	setjmp (wait_env);

	signal (SIGTERM, re_exec_serv);
	signal (SIGINT,  sig_daemon_abort);

	/*
	 *	Agora, espera um filho terminar
	 */
	for (EVER)
	{
		pid = wait (&status);

		/*
		 *	Verifica se a terminação foi normal
		 */
		if (status & 0xFF && print_child_status != ' ')
		{
			error ("%s na execução do processo %d", sigtostr (status & 0x7F), pid);

			if (status & 0x80)
				error ("Processo %d: core dumped", pid);
		}

		for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
		{
			if (sp->s_pid == pid)
			{
				if (sp->s_new_state != ' ')
					sp->s_pid = listen_fork (sp);
				break;
			}
		}
	}

}	/* end exec_serv */

/*
 ****************************************************************
 *	Rele a Tabela de servidores				*
 ****************************************************************
 */
void
re_read_itnet_serv (void)
{
	SERV		*sp;
	FILE		*fp;
	char		new_state;
	char		*prot_nm, *port_str, *max_conn_str, *serv_cmd, *serv_nm;
	char		*serv_cmd_copy;
	int		lineno = 0, port_no, max_conn_no;
	const char	*str;
	char		area[128];

	if ((fp = fopen (itnet_serv_nm, "r")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\"", itnet_serv_nm);
		return;
	}

	/*
	 *	O estado atual passa a ser antigo, e o novo desligado
	 */
	for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
	{
		sp->s_old_state = sp->s_new_state;
		sp->s_new_state = ' ';
	}

#ifdef	DEBUG
	if (vflag)
		printf ("\n");
#endif	DEBUG

	/*
	 *	Le e processa o arquivo
	 */
	while (fngets (area, sizeof (area), fp) == area)
	{
		lineno++;

		if (area[0] == '#' || area[0] == '\0')
			continue;

		new_state	= *strfield (area,   ':');
		prot_nm		=  strtok (strfield (NOSTR, ':'), "\t ");
		port_str	=  strtok (strfield (NOSTR, ':'), "\t ");
		max_conn_str	=  strtok (strfield (NOSTR, ':'), "\t ");
		serv_cmd	=  strfield (NOSTR, ':');

		/* Retira logo o nome do servidor */

		if ((serv_cmd_copy = strdup (serv_cmd)) == NOSTR)
			out_of_memory ();

		serv_nm   =  strtok (serv_cmd, "\t ");
#ifdef	DEBUG
		if (vflag)
			printf ("%s\n", area);
#endif	DEBUG

		/*
		 *	Examina o protocolo
		 */
		if (!streq (prot_nm, "udp") && !streq (prot_nm, "tcp"))
		{
			error
			( 	"%s: Protocolo inválido: \"%s\"",
				edit_serv_file_msg (lineno), prot_nm
			);
			continue;
		}

		/*
		 *	Examina o "port" dado
		 */
		if ((port_no = strtol (port_str, &str, 0)) < 1 || *str != '\0')
		{
			error
			( 	"%s: No de \"port\" inválido: \"%s\"",
				edit_serv_file_msg (lineno), port_str
			);
			continue;
		}

		/*
		 *	Examina o número máximo de conexões por cliente dado
		 */
		if ((max_conn_no = strtol (max_conn_str, &str, 0)) < 0 || *str != '\0')
		{
			error
			(	"%s: No de conexões máximas inválido: \"%s\"",
				edit_serv_file_msg (lineno), max_conn_str
			);
			exit_code++; continue;
		}

		/*
		 *	Examina o servidor dado
		 */
		if (new_state != ' '  &&  access (serv_nm, X_OK) < 0)
		{
			error
			( 	"*%s: O servidor \"%s\" não é executável",
				edit_serv_file_msg (lineno), serv_nm
			);
			continue;
		}

		/*
		 *	Verifica se a tabela já contém este servidor
		 */
		for (sp = itnet_serv; /* abaixo */; sp++)
		{
			if (sp->s_serv_nm != NOSTR)
			{
				if (sp->s_prot_nm != prot_nm[0])
					continue;

				if (sp->s_port_no != port_no)
					continue;

				break;		/* Achou ! */
			}

			/*
			 *	Não achou: aloca espaço para mais uma entrada
			 */
			itnet_serv_sz++;

			if ((itnet_serv = realloc (itnet_serv, itnet_serv_sz * sizeof (SERV))) == (SERV *)NULL)
				out_of_memory ();

			sp = &itnet_serv[itnet_serv_sz - 2];

			(sp + 1)->s_serv_nm = NOSTR;

			sp->s_pid	  = -2;
			sp->s_old_state	  = ' ';
			sp->s_listen_fd	  = -1;
			sp->s_prot_nm	  = prot_nm[0];
			sp->s_port_no	  = port_no;
			sp->s_max_conn_no = max_conn_no;
			sp->s_serv_nm	  = "";
			sp->s_serv_cmd	  = "";

			break;

		}	/* end loop pela tabela de servidores */

		/*
		 *	Coloca o novo estado
		 */
		sp->s_new_state = new_state;

		/*
		 *	Armazena servidor
		 */
		if (!streq (sp->s_serv_nm, serv_nm) && (sp->s_serv_nm = strdup (serv_nm)) == NOSTR)
				out_of_memory ();

		if (!streq (sp->s_serv_cmd, serv_cmd_copy))
			sp->s_serv_cmd = serv_cmd_copy;

	}	/* lendo itnet_serv */

	fclose (fp);

}	/* end re_read_itnet_serv */

/*
 ****************************************************************
 *	Recria/destrói os servidores				*
 ****************************************************************
 */
void
re_exec_serv (int signo, ...)
{
	SERV		*sp;

	/*
	 *	Rele a tabela
	 */
	re_read_itnet_serv ();

	/*
	 *	Altera os estados
	 */
	for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
	{
		if (sp->s_new_state == sp->s_old_state)
			continue;

		if (vflag) printf
		(	"\n%s: Servidor (%s, %d): mudança de estado de '%c' para '%c'\n",
			pgname, sp->s_prot_nm == 'u' ? "udp" : "tcp",
			sp->s_port_no, sp->s_old_state, sp->s_new_state
		);

		if (sp->s_new_state == ' ')
		{
			if (sp->s_pid > 0)
				kill (-sp->s_pid, SIGTERM);
		}
		else
		{
			sp->s_pid = listen_fork (sp);
		}
	}

	longjmp (wait_env, 1);

}	/* end re_exec_serv */

/*
 ****************************************************************
 *	Cria um filho para o servidor de um "well known port"	*
 ****************************************************************
 */
int
listen_fork (SERV *sp)
{
	long		pid;
	char		**ap;
	char		well_known_port_str[8];
	char		max_conn_str[16];
	char		*argv[12];

	/*
	 *	Cria um processo novo para esperar nesta porta
	 */
	if ((pid = fork ()) < 0)
		{ error ("*Não consegui dar \"fork\""); exit (1); }

	if (pid > 0)
		return (pid);

	setpgrp ();

	/*
	 *	Passa o controle para o esqueleto de servidor de escuta:
	 *
	 *				    "udp"
	 *	serv_daemon [-n <max_conn>]	  <well_known_port_no> <serv_nm>
	 *				    "tcp"
	 */
	ap = argv;

	*ap++ = SERV_DAEMON_NM;

	if (sp->s_max_conn_no != 0)
		{ *ap++ = "-n"; sprintf (max_conn_str, "%d", sp->s_max_conn_no); *ap++ = max_conn_str; }

	if (sp->s_prot_nm == 'u')
		*ap++ = "udp";
	else
		*ap++ = "tcp";

	sprintf (well_known_port_str, "%d", sp->s_port_no); *ap++ = well_known_port_str;
	*ap++ = sp->s_serv_cmd;
	*ap   = NOSTR;

	execv (XSTR ($DIR) "/" SERV_DAEMON_NM, (const char **)argv);

	error ("*Não consegui dar \"exec\" para \"%s\"", SERV_DAEMON_NM);
	exit (1);

	return (0);	/* To make lint happy */

}	/* end listen_fork */

/*
 ****************************************************************
 *	Coloca um servidor residente na memória			*
 ****************************************************************
 */
void
put_server_in_core (char *serv_nm)
{
	long		pid;
	int		status;

	if ((pid = fork ()) < 0)
		error ("$*Não consegui dar \"fork\"");

	if (pid == 0)
		{ wait (&status); return; }

	execl (serv_nm, serv_nm, "-M", NOSTR);

	error ("*Não consegui executar \"%s\"", serv_nm);

	exit (1);

}	/* end put_server_in_core */

/*
 ****************************************************************
 *	Acabou a memória					*
 ****************************************************************
 */
void
out_of_memory (void)
{
	error ("$Memória esgotada");

}	/* out_of_memory */

/*
 ****************************************************************
 *	Interface para "daemon_abort"				*
 ****************************************************************
 */
void
sig_daemon_abort (int signo, ...)
{
	daemon_abort ();

}	/* end sig_daemon_abort */

/*
 ****************************************************************
 *	Aborta todo o conjunto de processos do DAEMON		*
 ****************************************************************
 */
void
daemon_abort (void)
{
	SERV		 *sp;

	if (in_daemon_pid > 0)
		kill (in_daemon_pid, SIGTERM);

	if (out_daemon_pid > 0)
		kill (out_daemon_pid, SIGTERM);

	if (name_daemon_pid > 0)
		kill (name_daemon_pid, SIGTERM);

	if (itnet_serv) for (sp = itnet_serv; sp->s_serv_nm != NOSTR; sp++)
	{
		if (sp->s_pid > 0)
			kill (sp->s_pid, SIGTERM);
	}

	sleep (2);

	exit (1);

}	/* end daemon_abort */

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
	FILE		*log_fp = NOFILE;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;
	static int	static_log_fd = 0;
	static FILE	*static_log_fp = NOFILE;

	va_start (args, format);

	/*
	 *	Procura o arquivo de "log"
	 */
	if   (error_msg_to_log == 0)
	{
		log_fp = stderr;
	}
	elif (static_log_fd == 0)
	{
		if ((static_log_fd = open ("/var/adm/itnetlog", O_WRONLY|O_APPEND)) >= 0)
		{
			if ((static_log_fp = log_fp = fdopen (static_log_fd, "w")) == NOFILE)
				static_log_fd = -1;
		}
	}
	else
	{
		log_fp = static_log_fp;
	}

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
			event = t_look (global_fdaemon);
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
				fprintf (log_fp, " (%s)", t_strevent (global_fdaemon, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$')
		daemon_abort ();

	va_end (args);

}	/* end error */

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
		"%s - Inicializa e gerencia a INTERNET\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: verboso\n"
	);

	exit (2);

}	/* end help */
