/*
 ****************************************************************
 *								*
 *			editscb.c				*
 *								*
 *	Edita/imprime o bloco de controle da INTERNET		*
 *								*
 *	Versão	2.3.0, de 08.08.91				*
 *		4.9.0, de 30.08.06				*
 *								*
 *	Módulo: editscb						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/itnet.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 30.08.06";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	eflag;		/* TCP EPs */
entry int	dflag;		/* Debug */
entry int	pflag;		/* Parametros */
entry int	tflag;		/* Tabela de dispositivos */
entry int	fflag;		/* Tamanho do FIFO */
entry int	vflag;		/* Verbose */

entry int	global_itn_fd;	/* Para "error" */

/*
 ******	Nomes dos estados ***************************************
 */
const char	*state_nm[] =
{
	"NULL",
	"CLOSED",
	"UNBOUND",
	"BOUND",
	"LISTEN",
	"SYN_SENT",
	"SYN_RECEIVED",
	"ESTABLISHED",
	"FIN_WAIT_1",
	"FIN_WAIT_2",
	"CLOSE_WAIT",
	"CLOSING",
	"LAST_ACK",
	"TIME_WAIT"
};

/*
 ****** Nomes dos parâmetros ************************************
 */
typedef struct
{
	char	*p_param_nm;
	int	p_param_offset;
	int	p_param_op_code;

}	PARAM_TB;

const PARAM_TB	param_tb[] =
{
	"WND_SZ",	offsetof (ITSCB, it_WND_SZ),	0,	/* Como dado */
	"GOOD_WND",	offsetof (ITSCB, it_GOOD_WND),	0,	/* Como dado */
	"ALPHA",	offsetof (ITSCB, it_ALPHA),	2,	/* Percentagem */
	"BETA",		offsetof (ITSCB, it_BETA),	0,	/* Como dado */
	"SRTT",		offsetof (ITSCB, it_SRTT),	3,	/* * 1024 */
	"N_TRANS",	offsetof (ITSCB, it_N_TRANS),	0,	/* Como dado */
	"WAIT",		offsetof (ITSCB, it_WAIT),	0,	/* Como dado */
	"SILENCE",	offsetof (ITSCB, it_SILENCE),	0,	/* Como dado */
	"MAX_SGSZ",	offsetof (ITSCB, it_MAX_SGSZ),	0,	/* Como dado */
	NOSTR
};

/*
 ****** Nomes dos indicadores de depuração **********************
 */
typedef struct
{
	char	f_flag_code[4];
	char	*f_flag_desc;

}	FLAG_TB;

const FLAG_TB	flag_tb[] =
{
	"wi",  "Grava a entrada no arquivo \"itnet.debug\"",
	"wo",  "Grava a saída   no arquivo \"itnet.debug\"",
	"li",  "Relata a entrada de datagramas",
	"lo",  "Relata a saída   de datagramas",
	"rt",  "Relata retransmissões",
	"oq",  "Relata remoções da fila de retransmissão",
	"ce",  "Causa erros propositais de checksum",
	"er",  "Relata erros",
	"in",  "Lista algumas informações adicionais",
	"bs",  "Relata segmentos recusados",
	"lw",  "Relata tamanhos das janelas",
	"tt",  "Relata cálculos do SRTT",
	"gw",  "Executa a função de GATEWAY",
	"pi",  "Usa a otimização para conexões TCP internas",
	""
};

#if (0)	/*******************************************************/
/*
 ******	Código DNS **********************************************
 */
const char	dns_entry_bits[] =
{
	"\x01" "CANON"
	"\x02" "ALIAS"
	"\x03" "MAIL"
	"\x05" "REQ_CANON"
	"\x07" "REQ_MAIL"
};

const char	dns_error_bits[] =
{
	"\x01" "CANON_ERROR"
	"\x03" "MAIL_ERROR"
	"\x08" "SERVER_MAIL"
};
#endif	/*******************************************************/


#if (0)	/*******************************************************/
/*
 ****** Tabela dos nomes dos controles **************************
 */
typedef struct
{
	int	t_ctl;		/* O bit de controle */
	char	t_ctl_nm[12];	/* O nome do bit de controle */

}	CTL_TB;

const CTL_TB	ctl_tb[] =
{
	C_ACK_of_FIN, 	"ACK_of_FIN",
	C_ACK_of_SYN, 	"ACK_of_SYN",
	C_DATA,		"DATA",

	C_URG, 		"URG",
	C_ACK, 		"ACK",
	C_PSH, 		"PSH",
	C_RST, 		"RST",
	C_SYN, 		"SYN",
	C_FIN, 		"FIN",

	0
};
#endif	/*******************************************************/

/*
 ****** Protótipos de funções ***********************************
 */
void		pr_param (int);
void		ed_param (int, const char *);
void		ed_debug (int, const char *);
void		pr_debug (int);
void		pr_itscb (int);
void		pr_dns (int);
const char	*edit_save_dns_ptr (const DNS *, int, const SCB *);
const DNS	*get_save_mail_ptr (const DNS *, int, const SCB *);
const char	*edit_kernel_code (enum DNS_ERROR_CODE);
void		pr_route (int);
void		pr_ether (int);
void		pr_tcp_eps (int, const char *[]);
void		pr_tcp_queue (int, int, int);
void		ed_fifosz (int, const char *, const char *);
const char	*editpercent (int);
int		getpercent (long, long);
const char	*get_plural (int);
void		error (const char *, ...);
void		help (void);

/*
 ****************************************************************
 *	Edita/imprime o bloco de controle da INTERNET		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt, fd;
	const char	*debugstr = NOSTR, *paramstr = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ed:p:tfvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':			/* TCP EPs */
			eflag++;
			break;

		    case 'd':			/* DEBUG */
			dflag++;
			debugstr = optarg;
			break;

		    case 'p':			/* Parâmetros */
			pflag++;
			paramstr = optarg;
			break;

		    case 't':			/* Tabela de dispositivos */
			tflag++;
			break;

		    case 'f':			/* Tamanho do FIFO */
			fflag++;
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

	argv += optind;
	argc -= optind;

	if ((fd = t_open ("/dev/itndaemon", O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui inicializar a rede");

	global_itn_fd = fd;

	/*
	 *	Altera parâmetros
	 */
	if (pflag)
		{ ed_param (fd, paramstr); exit (exit_code); }

	/*
	 *	Altera o DEBUG
	 */
	if (dflag)
		{ ed_debug (fd, debugstr); exit (exit_code); }

	/*
	 *	Imprime a DEVTB
	 */
	if (tflag)
		{ pr_dns (fd); pr_route (fd); pr_ether (fd); exit (exit_code); }

	/*
	 *	Imprime os ENDPOINTS
	 */
	if (eflag)
		{ pr_tcp_eps (fd, argv); exit (exit_code); }

	/*
	 *	Altera o tamanho do FIFO
	 */
	if (fflag)
	{
		if (argc != 1 && argc != 2)
			help ();

		ed_fifosz (fd, argv[0], argv[1]);
		exit (exit_code);
	}

	/*
	 *	Imprime o ITSCB
	 */
	if (vflag || pflag + dflag + tflag + eflag == 0)
		{ pr_itscb (fd); pr_param (fd); }

	return (exit_code);

}	/* end editscb */

/*
 ****************************************************************
 *	Imprime parâmetros 					*
 ****************************************************************
 */
void
pr_param (int fd)
{
	register const PARAM_TB *pp;
	register int		i, n;
	register int		*ip;
	ITSCB			itscb;

	/*
	 *	Le o ITSCB
	 */
	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	/*
	 *	Imprime os parâmetros
	 */
	for (n = 0, pp = param_tb; pp->p_param_nm != NOSTR; n++, pp++)
	{
		if ((n & 1) == 0)
			printf ("\n");
		else
			printf ("\t");

		printf ("%s", pp->p_param_nm);

		for (i = 12 - strlen (pp->p_param_nm); i > 0; i--)
			putchar (' ');

		printf ("= ");

		ip = (int *)((int)&itscb + pp->p_param_offset);

		i = *ip;

		switch (pp->p_param_op_code)
		{
		    case 0:
			printf ("%-5d\t\t", i);
			break;

		    case 1:
			printf ("%d		(%d)\t", i / 512, i);
			break;

		    case 2:
			printf
			(	"%s	(%d)\t",
				editpercent (getpercent (i, 1024)), i
			);
			break;

		    case 3:
			printf
			(	"%s	(%d)\t",
				editpercent (getpercent (i, 1024 * 100)), i
			);
			break;

		}	/* end switch */

	}	/* for (tabela de parâmetros) */

	printf ("\n");

}	/* end pr_param */

/*
 ****************************************************************
 *	Edita parâmetros 					*
 ****************************************************************
 */
void
ed_param (int fd, const char *paramstr)
{
	register const PARAM_TB	*pp;
	register char		*cp;
	register int		i;
	register int		*ip;
	const char		*param_nm, *str;
	ITSCB			itscb;

	/*
	 *	Le o ITSCB
	 */
	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	/*
	 *	Percorre os vários parâmetros
	 */
	for
	(	param_nm = strfield ((char *)paramstr, ',');
		param_nm != NOSTR;
		param_nm = strfield (NOSTR, ',')
	)
	{
		if ((cp = strchr (param_nm, '=')) == NOSTR)
			error ("$Não achei o '=' em \"%s\"", param_nm);

		*cp++ = '\0';

		if ((i = strtol (cp, &str, 0)) <= 0 || *str)
			error ("$Valor inválido: \"%s\"", cp);

		for (pp = param_tb; /* abaixo */; pp++)
		{
			if (pp->p_param_nm == NOSTR)
				error ("$Parâmetro inválido: \"%s\"", param_nm);

			if (streq (pp->p_param_nm, param_nm))
			{
				switch (pp->p_param_op_code)
				{
				    case 1:
					i *= 512;
					break;

				    case 2:
					i = ((i * 1024) + 50) / 100;
					break;

				    case 3:
					i *= 1024;
					break;

				}	/* end switch */

				ip = (int *)((int)&itscb + pp->p_param_offset);

				*ip = i;

				break;
			}

		}	/* for (tabela de indicadores) */

	}	/* for (vários indicadores) */

	/*
	 *	Escreve de volta o ITSCB
	 */
	if (ioctl (fd, I_PUT_ITSCB, &itscb) < 0)
		error ("$*Não consegui escrever o ITSCB");

}	/* end ed_param */

/*
 ****************************************************************
 *	Imprime o estado dos indicadores de depuração		*
 ****************************************************************
 */
void
pr_debug (int fd)
{
	register const FLAG_TB	*ft;
	register char		*cp;
	ITSCB			itscb;

	/*
	 *	Le o ITSCB
	 */
	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	/*
	 *	Imprime o estado dos indicadores
	 */
	cp = &itscb.it_write_input;

	for (ft = flag_tb; ft->f_flag_code[0]; ft++, cp++)
	{
		if (*cp)
			printf ("*** (%s) %s\n", ft->f_flag_code, ft->f_flag_desc);
		else
			printf ("    (%s) %s\n", ft->f_flag_code, ft->f_flag_desc);
	}

#if (0)	/*******************************************************/
	int			debug;
	memmove (&debug, &itscb.debug, sizeof (debug));

	/*
	 *	Imprime o estado dos indicadores
	 */
#ifdef	LITTLE_ENDIAN
	for (ft = flag_tb, m = 0x00000001; ft->f_flag_code[0]; ft++, m <<= 1)
#else
	for (ft = flag_tb, m = 0x80000000; ft->f_flag_code[0]; ft++, m >>= 1)
#endif	LITTLE_ENDIAN
	{
		if (debug & m)
			printf ("*** (%s) %s\n", ft->f_flag_code, ft->f_flag_desc);
		else
			printf ("    (%s) %s\n", ft->f_flag_code, ft->f_flag_desc);
	}
#endif	/*******************************************************/

}	/* end pr_debug */

/*
 ****************************************************************
 *	Edita o DEBUG						*
 ****************************************************************
 */
void
ed_debug (int fd, register const char *debugstr)
{
	register char		*flag_nm, *cp;
	register const FLAG_TB	*ft;
	register int		yes_no;
	ITSCB			itscb;

	/*
	 *	Le o ITSCB
	 */
	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	/*
	 *	Verifica se é para simplesmente imprimir o estado atual
	 */
	if (streq (debugstr, "-"))
		{ pr_debug (fd); return; }

	/*
	 *	Analisa a cadeia de indicadores de DEBUG
	 */
	for
	(	flag_nm = strfield ((char *)debugstr, ',');
		flag_nm != NOSTR;
		flag_nm = strfield (NOSTR, ',')
	)
	{
		if   (flag_nm[0] == '+')
			{ yes_no = 1; flag_nm++; }
		elif (flag_nm[0] == '-')
			{ yes_no = 0; flag_nm++; }
		else
			{ yes_no = 1; }

		cp = &itscb.it_write_input;

		for (ft = flag_tb; /* abaixo */; ft++, cp++)
		{
			if (ft->f_flag_code[0] == '\0')
				error ("$Indicador de depuração/controle inválido: \"%s\"", flag_nm);

			if (streq (ft->f_flag_code, flag_nm))
			{
				if (yes_no == 1)
					*cp = 1;
				else
					*cp = 0;

				break;
			}

		}	/* for (tabela de indicadores) */

	}	/* for (vários indicadores) */

	/*
	 *	Escreve de volta o ITSCB
	 */
	if (ioctl (fd, I_PUT_ITSCB, &itscb) < 0)
		error ("$*Não consegui escrever o ITSCB");

}	/* end ed_debug */



#if (0)	/*******************************************************/
	register unsigned	m;
	int			debug;
	memmove (&debug, &itscb.debug, sizeof (debug));
	/*
	 *	Analisa a cadeia de indicadores de DEBUG
	 */
	for
	(	flag_nm = strfield ((char *)debugstr, ',');
		flag_nm != NOSTR;
		flag_nm = strfield (NOSTR, ',')
	)
	{
		if   (flag_nm[0] == '+')
			{ yes_no = 1; flag_nm++; }
		elif (flag_nm[0] == '-')
			{ yes_no = 0; flag_nm++; }
		else
			{ yes_no = 1; }
#ifdef	LITTLE_ENDIAN
		for (ft = flag_tb, m = 0x00000001; /* abaixo */; ft++, m <<= 1)
#else
		for (ft = flag_tb, m = 0x80000000; /* abaixo */; ft++, m >>= 1)
#endif	LITTLE_ENDIAN
		{
			if (ft->f_flag_code[0] == '\0')
				error ("$Indicador de depuração/controle inválido: \"%s\"", flag_nm);

			if (streq (ft->f_flag_code, flag_nm))
			{
				if (yes_no == 1)
					debug |= m;
				else
					debug &= ~m;

				break;
			}

		}	/* for (tabela de indicadores) */

	}	/* for (vários indicadores) */
	memmove (&itscb.debug, &debug, sizeof (debug));

#endif	/*******************************************************/

/*
 ****************************************************************
 *	Imprime o ITSCB						*
 ****************************************************************
 */
void
pr_itscb (int fd)
{
	ITSCB		itscb;

	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	/*
	 *	Imprime os dados sobre blocos e EPs
	 */
	printf
	(	"ITBLOCKs: (%d, %d), "
		"RAW EPs: (%d, %d), "
		"UDP EPs: (%d, %d), "
		"TCP EPs: (%d, %d)\n",
		itscb.it_bl_count,  itscb.it_N_BLOCK,
		itscb.it_rep_count, itscb.it_rep_total,
		itscb.it_uep_count, itscb.it_uep_total,
		itscb.it_tep_count, itscb.it_tep_total
	);

}	/* end pr_itscb */

/*
 ****************************************************************
 *	Imprime a tabela DNS 					*
 ****************************************************************
 */
void
pr_dns (int fd)
{
	register const DNS	*dp, *next_dp;
	const DNS		*dns;
	int			N_DNS, i, j;
	unsigned long		DIFF;
	IPADDR			nm_serv_tb[DNS_SERVER_CNT + 1];
	IPADDR			*ip;
	SCB			scb;

	if (getscb (&scb) == (SCB *)-1)
		error ("$*Não consegui obter o tamanho da DNS");

	if ((N_DNS = ioctl (fd, I_GET_DNS, 0)) <= 0)
		error ("$*Não consegui obter o tamanho da DNS");

	if ((dns = alloca (N_DNS * sizeof (DNS))) == NOVOID)
		error ("$Não consegui obter memória para a tabela DNS");

	if (ioctl (fd, I_GET_DNS, dns) < 0)
		error ("$*Não consegui obter a tabela DNS");

	DIFF = (int)scb.y_dns - (int)dns;

	/*
	 *	Imprime a tabela DNS
	 */
	printf
	(	"--- ENDEREÇO ---"
		" D C E I PRE"
#if (0)	/*******************************************************/
		" D C A M c m s I PRE"
#endif	/*******************************************************/
		"  ------- NOME DO NÓ -------\n"
	);

	for (dp = dns, i = N_DNS; i > 0; i--, dp++)
	{
		if (dp->d_host_nm[0] == '\0' && dp->d_entry_code != 'R')
			continue;

		if (dp->d_host_addr)
			printf ("%-16s", t_addr_to_str (dp->d_host_addr));
		else
			printf ("%-16s", "");

		printf (" %c", (dp->d_resolver_done & 0x80) ? 'D' : ' ');
		printf (" %c", dp->d_entry_code);
		printf (" %s", edit_kernel_code (dp->d_error_code));

#if (0)	/*******************************************************/
		printf (" %c", dp->d_entry_code & DNS_CANON ? 'C' : ' ');
		printf (" %c", dp->d_entry_code & DNS_ALIAS ? 'A' : ' ');
		printf (" %c", dp->d_entry_code & DNS_MAIL ?  'M' : ' ');

		printf (" %c", dp->d_error_code & DNS_CANON_ERROR  ? 'c' : ' ');
		printf (" %c", dp->d_error_code & DNS_MAIL_ERROR   ? 'm' : ' ');
		printf (" %c", dp->d_error_code & DNS_SERVER_ERROR ? 's' : ' ');
#endif	/*******************************************************/

		if (dp->d_server_index == 255)
			printf (" -");
		else
			printf (" %d", dp->d_server_index);

		printf (" %3d", dp->d_preference);

		if (vflag)
			printf (" %P", dp->d_mail_ptr);

		printf ("  %s", dp->d_host_nm);

		if   (dp->d_entry_code == 'A')
		{
			printf (" => %s", edit_save_dns_ptr (dp->d_can_ptr, DIFF, &scb));
		}
		elif (dp->d_entry_code == 'M')
		{
			next_dp = dp->d_mail_ptr;

			for (j = N_DNS; j > 0; j--)
			{
				if (next_dp == NODNS)
					break;

				printf (" => %s", edit_save_dns_ptr (next_dp, DIFF, &scb));
				next_dp = get_save_mail_ptr (next_dp, DIFF, &scb);
			}
		}

#if (0)	/*******************************************************/
		if   (dp->d_entry_code == 'C')
			printf (" CANONICAL");
		elif (dp->d_entry_code == 'A')
			printf (" ALIAS");

		if (dp->d_name_server)
			printf (" NM_SERVER");

		if (dp->d_error_code != DNS_NO_ERROR)
			printf (" %s", edit_kernel_code (dp->d_error_code));
#endif	/*******************************************************/

		printf ("\n");

		if (!vflag)
			continue;

		printf
		(	"expir_time = %-24.24s, ",
			dp->d_expir_time ? btime (&dp->d_expir_time) : ""
		);

		printf
		(	"last_use_time = %-24.24s\n",
			dp->d_last_use_time ? btime (&dp->d_last_use_time) : ""
		);
	}

	if (ioctl (fd, I_GET_DNS_SERVER, nm_serv_tb) >= 0)
	{
		printf ("\nSERVIDORES de NOME:\t");

		for (ip = nm_serv_tb; *ip != 0; ip++)
			printf ("  %s", t_addr_to_str (*ip));

		printf ("\n");
	}

}	/* end pr_dns */

/*
 ****************************************************************
 *	Confere o valor de um ponteiro DNS			*
 ****************************************************************
 */
const char *
edit_save_dns_ptr (const DNS *ptr, int DIFF, const SCB *sp)
{
	const DNS	*dp = (DNS *)((int)ptr - DIFF);

	if (ptr == NODNS)
		return ("(Ponteiro NULO)");
	elif (ptr < sp->y_dns || ptr >= sp->y_end_dns)
		return ("(Ponteiro fora da tabela)");
	elif (((int)ptr - (int)sp->y_dns) % sizeof (DNS))
		return ("(Ponteiro desalinhado)");
	else
		return (dp->d_host_nm);

}	/* end edit_save_dns_ptr */

/*
 ****************************************************************
 *	Confere o valor de um ponteiro DNS			*
 ****************************************************************
 */
const DNS *
get_save_mail_ptr (const DNS *ptr, int DIFF, const SCB *sp)
{
	const DNS	*dp = (DNS *)((int)ptr - DIFF);

	if (ptr == NODNS)
		printf ("(Ponteiro NULO)");
	elif (ptr < sp->y_dns || ptr >= sp->y_end_dns)
		printf ("(Ponteiro fora da tabela)");
	elif (((int)ptr - (int)sp->y_dns) % sizeof (DNS))
		printf ("(Ponteiro desalinhado)");
	else
		return (dp->d_mail_ptr);

	return (NODNS);

}	/* end edit_save_mail_ptr */

/*
 ****************************************************************
 *	Decodifica o código de retorno DNS			*
 ****************************************************************
 */
const char *
edit_kernel_code (enum DNS_ERROR_CODE code)
{
	switch (code)
	{
	    case DNS_NO_ERROR:
		return (" ");

	    case DNS_NAME_ERROR:
		return ("N");

	    case DNS_SERVER_ERROR:
		return ("S");
	}

	return ("?");

}	/* end edit_kernel_code */

/*
 ****************************************************************
 *	Imprime a tabela ROUTE 					*
 ****************************************************************
 */
void
pr_route (int fd)
{
	register ROUTE	*rp;
	ROUTE		*route;
	int		N_ROUTE;

	if ((N_ROUTE = ioctl (fd, I_GET_ROUTE, 0)) <= 0)
		error ("$*Não consegui obter o tamanho da tabela ROUTE");

	if ((route = alloca (N_ROUTE * sizeof (ROUTE))) == NOVOID)
		error ("$Não consegui obter memória para a tabela ROUTE");

	if (ioctl (fd, I_GET_ROUTE, route) < 0)
		error ("$*Não consegui obter a tabela ROUTE");

	/*
	 *	Imprime a tabela ROUTE
	 */
	printf ("\n--- MÁSCARA --- ");
	printf (  "---- REDE ----- ");
	printf (  "---- LOCAL ---- ");
	printf (  "--- GATEWAY --- ");
	printf (  "DISPOSITIVO\n");

	for (rp = route; rp->r_dev_nm[0] != '\0'; rp++)
	{
		printf ("%-15s ", t_addr_to_str (rp->r_mask));
		printf ("%-15s ", t_addr_to_str (rp->r_net_addr));
		printf ("%-15s ", t_addr_to_str (rp->r_my_addr));
		printf ("%-15s ", t_addr_to_str (rp->r_gateway_addr));
		printf ("%s\n", rp->r_dev_nm);

#if (0)	/*******************************************************/
	register char   *cp;

		cp = rp->r_my_ether_addr.addr;

		if ((cp[0]|cp[1]|cp[2]|cp[3]|cp[4]|cp[5]) == 0)
			continue;

		printf
		(	"\t\t\t%02X%02X %02X%02X %02X%02X\n",
			cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]
		);
#endif	/*******************************************************/
	}

}	/* end pr_route */

/*
 ****************************************************************
 *	Imprime a tabela ETHER 					*
 ****************************************************************
 */
void
pr_ether (int fd)
{
	register ETHER	*ep;
	register char   *cp;
	ETHER		*ether, *ether_end;
	int		N_ETHER, n_ether = 0;

	if ((N_ETHER = ioctl (fd, I_GET_ETHER, 0)) <= 0)
		error ("$*Não consegui obter o tamanho da tabela ETHER");

	if ((ether = alloca (N_ETHER * sizeof (ETHER))) == NOVOID)
		error ("$Não consegui obter memória para a tabela ETHER");

	ether_end = ether + N_ETHER;

	if (ioctl (fd, I_GET_ETHER, ether) < 0)
		error ("$*Não consegui obter a tabela ETHER");

	/*
	 *	Imprime a tabela ETHER
	 */
	printf
	(	"\n----- IP ------ "
		"-- ETHERNET -- -TEMPO-\t"
		"\t----- IP ------ "
		"-- ETHERNET -- -TEMPO-\n"
	);

	for (ep = ether; ep < ether_end; ep++)
	{
		if (ep->e_ip_addr == 0)
			continue;

		if   (n_ether & 1)
			printf ("\t\t");
		elif (n_ether)
			printf ("\n");

		printf ("%-15s ", t_addr_to_str (ep->e_ip_addr));

		cp = ep->e_ether_addr.addr;

		printf
		(	"%02X%02X %02X%02X %02X%02X %7d",
			cp[0], cp[1], cp[2], cp[3], cp[4], cp[5], time (NULL) - ep->e_ether_time
		);

		n_ether++;
	}

	if (n_ether)
		printf ("\n\n%d entradas ocupadas de %d (%d %%)\n", n_ether, N_ETHER, 100 * n_ether / N_ETHER);

}	/* end pr_ether */

/*
 ****************************************************************
 *	Imprime os TCP EPs ativos				*
 ****************************************************************
 */
void
pr_tcp_eps (int fd, const char *argv[])
{
	register int		n, i;
	register const TCP_EP	*tp;
	const TCP_EP		*tcp_ep;
	int			port = 0;
	char			found = 0;
	const char		*str;
	ITSCB			itscb;
	MUTM			now;

	/*
	 *	Verifica quantos são os EPs
	 */
	if (ioctl (fd, I_GET_ITSCB, &itscb) < 0)
		error ("$*Não consegui obter o ITSCB");

	if ((tcp_ep = alloca (itscb.it_tep_count * sizeof (TCP_EP))) == NOVOID)
		error ("$*Não consegui obter memória para os TCP EPs");

	if ((n = ioctl (fd, I_GET_TCP_EP, tcp_ep)) < 0)
		error ("$*Não consegui obter os EPs");

	/*
	 *	Retira o número da porta local, se for o caso
	 */
	if (argv[0] != NOSTR)
	{
		if ((port = strtol (argv[0], &str, 0)) <= 0 || *str)
			error ("$Porta \"%s\" inválida", argv[0]);
	}

	printf ("Foram obtidos %d EPs\n\n", n);

	mutime (&now);

	/*
	 *	Imprime os EPs
	 */
	for (tp = tcp_ep, i = 0; i < n; i++, tp++)
	{
		if (port != 0)
		{
			if (tp->tp_my_port != port)
				continue;

			found++;
		}

		printf
		(	"my_addr = (%s, %d)\t\t",
			t_addr_to_str (tp->tp_my_addr), tp->tp_my_port
		);

		printf
		(	"dst_addr = (%s, %d)\n",
			t_addr_to_str (tp->tp_dst_addr), tp->tp_dst_port
		);

		printf
		(	"state = %s (%d), pipe = %s\n",
			state_nm[tp->tp_state], tp->tp_state,
			tp->tp_pipe_mode ? "PIPE" : "-"
		);

		printf
		(	"(snd) una = %u (%u), nxt = %u (%u), wnd = %d\n",
			tp->tp_snd_una, tp->tp_snd_una - tp->tp_snd_iss,
			tp->tp_snd_nxt, tp->tp_snd_nxt - tp->tp_snd_iss,
			tp->tp_snd_wnd - tp->tp_snd_una
		);

		printf
		(	"(rcv) nxt = %u (%u), wnd = %d\n",
			tp->tp_rcv_nxt, tp->tp_rcv_nxt - tp->tp_rcv_irs,
			tp->tp_rcv_wnd - tp->tp_rcv_nxt
		);

		printf
		( 	"listen_maxqlen = %d, listen_qlen = %d\n",
			tp->tp_listen_maxqlen, tp->tp_listen_qlen
		);

		if (tp->tp_listen_qlen > 0)
		{
			const LISTENQ		*lp;

			for (lp = tp->tp_listen_q; lp < &tp->tp_listen_q[LISTENQ_MAXSZ]; lp++)
			{
				if (lp->tp_listen_seq == 0)
					continue;

				printf
				(	"seq = %d, listen_addr = %s, tempo = %d\n",
					lp->tp_listen_seq, t_addr_to_str (lp->tp_listen_addr),
					time - lp->tp_listen_time
				);
			}
		}

#if (0)	/*******************************************************/
		printf
		( 	"pipe_mode = %s, idle = %d\n",
			tp->tp_pipe_mode ? "PIPE" : "-",
			now.mu_time - tp->tp_last_rcv_time
		);
#endif	/*******************************************************/

		printf
		( 	"idle = %d, retransmitted = %d, closed_wnd = %d\n",
			now.mu_time - tp->tp_last_rcv_time,
			tp->tp_retransmitted,
			tp->tp_closed_wnd_time ? now.mu_time - tp->tp_closed_wnd_time : 0
		);

		/*
		 *	Imprime as opções modificáveis por "t_optmgmt"
		 */
		printf
		(	"Tamanho máximo do segmento = %d\n"
			"Tempos máximos permitidos: sem conexão = %d, sem entrada/saída = %d\n"
			"Tamanhos das áreas circulares: entrada = %d, saída = %d\n"
			"Número máximo de conexões por cliente = %d\n",
			tp->tp_max_seg_sz,  tp->tp_max_wait, tp->tp_max_silence,
			tp->tp_rnd_in.tp_rnd_sz, tp->tp_rnd_out.tp_rnd_sz,
			tp->tp_max_client_conn
		);

		/*
		 *	Imprime os tamanhos das filas
		 */
		printf
		(	"Fila de entrada = %d (%s%s), ",
			tp->tp_rnd_in.tp_rnd_count,
			tp->tp_rnd_in.tp_rnd_syn ? "SYN" : "",
			tp->tp_rnd_in.tp_rnd_fin ? " FIN" : ""
		);

		printf
		(	"Fila de saída = %d (%s%s)\n",
			tp->tp_rnd_out.tp_rnd_count,
			tp->tp_rnd_out.tp_rnd_syn ? "SYN" : "",
			tp->tp_rnd_out.tp_rnd_fin ? " FIN" : ""
		);

		printf ("\n\n");

	}	/* end percorrendo os EPs */

	if (port != 0 && found == 0)
		error ("Não encontrei a porta \"%s\"", argv[0]);

}	/* end pr_tcp_eps */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Imprime uma fila do TCP/IP				*
 ****************************************************************
 */
void
pr_tcp_queue (int fd, int port, int I_GET_TCP_which)
{
	ITBLOCK			q[200];
	register const CTL_TB	*cp;
	struct { ITBLOCK *q_q; long q_port; }	q_bl;
	register int		k;
	register ITBLOCK	*bp;
	int			n_blocks, flag;

	q_bl.q_q  = q;
	q_bl.q_port = port;

	if ((n_blocks = ioctl (fd, I_GET_TCP_which, &q_bl)) < 0)
		error ("$*Não consegui obter a FILA");

	printf
	(	"Foram obtidos %d ITBLOCK%s da FILA:\n\n",
		n_blocks, get_plural (n_blocks)
	);

	for (bp = q, k = 0; k < n_blocks; k++, bp++)
	{
		printf ("  (");

		for (cp = ctl_tb, flag = 0; cp->t_ctl != 0; cp++)
		{
			if (bp->it_ctl & cp->t_ctl)
			{
				if (flag)
					printf ("|");

				printf (cp->t_ctl_nm); 	flag++;
			}
		}

		printf (")  ");

		printf ("seq = %d, ", bp->it_seg_seq);
		printf ("len = %d, ", bp->it_seg_len);
		printf ("nxt = %d\n", bp->it_seg_nxt);

	}	/* end percorrendo os BLOCOs */

	printf ("\n\n");

}	/* end pr_tcp_queue */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Altera o tamanho do FIFO				*
 ****************************************************************
 */
void
ed_fifosz (int fd, const char *dev, const char *fifosz_str)
{
	int		new_fifosz, old_fifosz;
	const char	*str;

	if   (fifosz_str == NOSTR)
	{
		new_fifosz = 0;
	}
	elif ((new_fifosz = strtol (fifosz_str, &str, 0)) < 0 || *str)
	{
		error ("$Tamanho inválido de FIFO: \"%s\"", fifosz_str);
	}

	if ((fd = open (dev, O_RDONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", dev);

	if ((old_fifosz = ioctl (fd, TCFIFOSZ, new_fifosz)) < 0)
		error ("$*Não consegui consultar/alterar o tamanho do FIFO");

	if (new_fifosz == 0)
		printf ("tamanho do FIFO = %d\n", old_fifosz);
	else
		printf ("tamanho do FIFO = (%d => %d)\n", old_fifosz, new_fifosz);

}	/* end ed_fifosz */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
const char *
editpercent (register int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);

	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (register long numer, register long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Obtém o plural						*
 ****************************************************************
 */
const char *
get_plural (int n)
{
	if (n == 1)
		return ("");
	else
		return ("s");

}	/* end get_plural */
#endif	/*******************************************************/

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
			event = t_look (global_itn_fd);
	
			fprintf (stderr, " (%s)", t_strevent (global_itn_fd, event));
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	putc ('\n', stderr);

	if (abrt == '$')
		exit (1);

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
		"%s - edita/imprime o bloco de controle da INTERNET\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s\n"
		"\t%s -e [-v] [<porta>]\n"
		"\t%s -t [-v]\n"
		"\t%s -f [-v] <dev> [<sz>]\n"
		"\t%s -d [-v] [+-][<arg>]\n"
		"\t%s -p [-v] <arg>=<valor>\n",

		pgname, pgversion, pgname,
		pgname, pgname, pgname, pgname,	pgname
	);

#if (0)	/*******************************************************/
		"\t%s [-etv] [-f <dev> [<sz>]] [-d [+-][<arg>]] [-p <arg>=<valor>]\n",
#endif	/*******************************************************/

	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Imprime os valores atuais dos parâmetros\n"
		"\t-e: Imprime as estruturas TCP em uso (ou somente da <porta> dada)\n"
		"\t-t: Imprime as tabelas de endereços IP, roteamento e \"ethernet\"\n"
		"\t-f: Consulta/altera o tamanho do FIFO de saída de <dev>\n"
		"\t-d: Depuração/controle (ver abaixo)\n"
		"\t-p: Altera parâmetros (ver abaixo)\n"
		"\t-v: Verboso\n"
	);

	fprintf
	(	stderr,
		"\nArgumentos de depuração/controle:\n"
		"\t-:      Imprime os estados atuais dos argumentos\n"
		"\t<arg>:  Liga o argumento <arg>\n"
		"\t-<arg>: Desliga o argumento <arg>\n\n"
		"\twi: Grava a entrada no arquivo \"itnet.debug\"\n"
		"\two: Grava a saída   no arquivo \"itnet.debug\"\n"
		"\tli: Relata a entrada de datagramas\n"
		"\tlo: Relata a saída   de datagramas\n"
		"\trt: Relata retransmissões\n"
		"\toq: Relata remoções da fila de retransmissão\n"
	);
	fprintf
	(	stderr,
		"\tce: Causa erros propositais de \"checksum\"\n"
		"\ter: Relata erros\n"
		"\tin: Lista algumas infomações adicionais\n"
		"\tbs: Relata segmentos recusados\n"
		"\tlw: Relata tamanhos das janelas\n"
		"\ttt: Relata cálculos do SRTT\n"
		"\tgw: Executa a função de GATEWAY\n"
		"\tlc: Lista concatenação de blocos\n"
		"\tpi: Usa a otimização para conexões TCP internas\n"
	);

	fprintf
	(	stderr,
		"\nArgumentos dos PARÂMETROS:\n"
		"\tWND_SZ:   Tamanho total da janela\n"
		"\tGOOD_WND: Tamanho \"bom\" de janela\n"
		"\tALPHA:    Amortecimento do SRTT (porcentagem)\n"
		"\tBETA:     Fator de variância do SRTT\n"
		"\tSRTT:     Valor inicial de SRTT em segundos\n"
		"\tN_TRANS:  No. de transmissões máximo para TCP\n"
		"\tWAIT:     Tempo máximo (seg.) permitido para TCP ainda não completado\n"
		"\tSILENCE:  Tempo máximo (segundos) permitido para TCP sem I/O\n"
		"\tMAX_SGSZ: Tamanho máximo para segmentos TCP\n"
	);

	exit (2);

}	/* end help */
