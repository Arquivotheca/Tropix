/*
 ****************************************************************
 *								*
 *			ppp.c					*
 *								*
 *	Comunicação PPP com computador remoto			*
 *								*
 *	Versão	3.0.0, de 11.06.96				*
 *		4.4.0, de 30.11.02				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/itnet.h>
#include <sys/ppp.h>
#include <sys/devmajor.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <termio.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#undef	DEBUG

const char	pgversion[] = "Versão:	4.4.0, de 30.11.02";

const char	*line_nm_str = "/dev/pppline";	/* Linha default */

#define DEF_SPEED B115200	/* Velocidade "default" */

#define	EVER	;;
#define	NOSTR	(char *)NULL
#define	elif	else if

entry int	jflag;		/* No Van Jacobson */
entry int	vflag;		/* Verbose */
entry int	dflag;		/* Debug */

entry time_t	begin_stamp;	/* Tempo de início da conexão */

const char	*user_str;	/* Nome do usuário */
const char	*pwd_str;	/* Senha */

/*
 ******	Variáveis do PPP ****************************************
 */
entry const char	*text_str;	/* Texto de inicialização */

entry const char	log_nm[] = "log"; /* Arquivo de registro */
entry FILE		*log_fp;

entry PPP_STATUS	ppp_status;	/* Estado PPP */

entry char		ppp_has_CTS;	/* Tem o CLEAR to SEND */

entry char		ppp_mode;	/* Já entrou no modo PPP */

const unsigned short	fcstab[];	/* Tabela para CRC */

entry int		id_source = 1;	/* Fonte de IDs */

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
 ****** Variáveis da linha/tela *********************************
 */
entry int		tty_fd;		/* fd da linha */
entry FILE		*tty_fp;	/* fp da linha */
entry FILE		*tty_fp_out;	/* fp da linha para saída */

entry int		ppp_fd;		/* fd do PPP */

entry TERMIO		o_mode, mode;	/* modos da linha */

entry TERMIO		ppp_termio;	/* modos do ppp */

entry TERMIO		o_video, video;	/* modos do terminal */

/*
 ******	Tabela de Velocidades ***********************************
 */
typedef	struct
{
	char	speedname[6];
	short	speed;

}	SPEED;

#define	NOSPEED	(SPEED *)NULL

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

entry int	speed_value;	/* Valor lido do MODEM */

/*
 ****** Protótipos de funções ***********************************
 */
void		pai (int);
void		filho (void);
int		analyse_modem_msg (const char *);
void		packet_proc (const char *, int);
void		lcp_proc (const char *);
void		lcp_configure_request (const char *);
void		lcp_terminate_request (void);
void		lcp_reply_echo_request (const char *);
void		lcp_send_echo_request (void);
void		lcp_authen_request (int);
void		pap_proc (const char *);
void		unknown_proc (const char *, int, int);
void		ipcp_proc (const char *);
void		ipcp_configure_request (const char *);
void		ipcp_configure_nak (const char *);
void		ipcp_configure_reject (const char *);
void		ppp_lcp_packet_snd (int, int, int, const void *, int);
int		pppfcs (int, const char *, int);
void		on_sig_term_from_parent (int, ...);
void		on_sig_term_from_child (int, ...);
void		on_sig_term_to_disconnect (int, ...);
void		help (void);

/*
 ****************************************************************
 *	Comunicação com estação remota				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const SPEED	*ep = NOSPEED;
	PPP_STATUS	*sp = &ppp_status;
	const char	*speed_str = NOSTR, *fifo_str = "16", *str;
	int		new_fifosz = 0, old_fifosz = 0;
	char		tty_dev_nm[16], ppp_dev_nm[16];
	long		pidfilho;
	MUTM		t;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "l:s:t:f:jp:dvH")) != EOF)
	{
		switch (opt)
		{
		    case 'l':			/* line name */
			line_nm_str = optarg;
			break;

		    case 's':			/* speed */
			speed_str = optarg;
			break;

		    case 't':			/* Texto de inicialização */
			text_str = optarg;
			break;

		    case 'f':			/* Tamanho do FIFO */
			fifo_str = optarg;
			break;

		    case 'j':			/* NO Van Jacobson */
			jflag++;
			break;

		    case 'p':			/* Conta/senha */
			user_str = optarg;
			break;

		    case 'd':			/* Debug */
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

	argv += optind;
	argc -= optind;

	if (argv[0])
		help ();

	/*
	 *	Processa a conta/senha
	 */
	if (user_str != NOSTR)
	{
		if ((str = strchr (user_str, ':')) != NOSTR)
			{ *(char *)str++ = '\0'; pwd_str = str; }
	}

	if (user_str == NOSTR || user_str[0] == '\0')
		user_str = getenv ("USER");

	if (user_str == NOSTR)
		user_str = getlogin ();

	if (user_str == NOSTR)
	{
		printf ("Usuário: ");

		user_str = alloca (80); fngets ((char *)user_str, 80, stdin);
	}

	if (pwd_str == NOSTR || pwd_str[0] == '\0')
		pwd_str = getpass ("Senha: ");

	if (vflag)
		printf ("%s:%s\n", user_str, pwd_str);

	/*
	 *	Analisa a velocidade da linha
	 */
	if (speed_str != NOSTR)
	{
		for (ep = speedtb; /* teste abaixo */; ep++)
		{
			if (ep->speedname[0] == '\0')
			{
				fprintf
				(	stderr,
					"%s: Velocidade inválida: \"%s\"\n",
					pgname, speed_str
				);
				exit (1);
			}

			if (streq (speed_str, ep->speedname))
				break;

		}	/* end for */

	}	/* end if */

	/*
	 *	Retira a cadeia do FIFO
	 */
	if (fifo_str != NOSTR)
	{
		if ((new_fifosz = strtol (fifo_str, &str, 0)) < 1 || new_fifosz > 16)
		{
			fprintf
			(	stderr,
				"%s: Tamanho do FIFO inválido: \"%s\"\n",
				pgname, fifo_str
			);
			exit (1);
		}
	}

	/*
	 *	Examina a entrada padrão
	 */
	if (ioctl (0, TCGETS, &o_video) < 0  ||  isatty (1) == 0)
	{
		fprintf
		(	stderr,
			"%s: O <stdin> e/ou <stdout> não é um terminal\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Obtém o UNIT da linha dada
	 */
	if (stat (line_nm_str, &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, line_nm_str, strerror (errno)
		);
		exit (1);
	}

	if (MAJOR (s.st_rdev) != PPP_MAJOR)
	{
		fprintf
		(	stderr,
			"%s: O dispositivo \"%s\" NÃO é PPP\n",
			pgname, line_nm_str
		);
		exit (1);
	}

	sprintf (tty_dev_nm, "/dev/tty%03d", MINOR (s.st_rdev));
	sprintf (ppp_dev_nm, "/dev/ppp%03d", MINOR (s.st_rdev));

	if (vflag)
		printf ("Dispositivos: \"%s\", \"%s\"\n", tty_dev_nm, ppp_dev_nm);

	/*
	 *	Analisa e prepara a linha
	 */
	if ((tty_fd = open (tty_dev_nm, O_RDWR)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir a linha \"%s\" (%s)\n",
			pgname, tty_dev_nm, strerror (errno)
		);
		exit (1);
	}

	if ((tty_fp = fdopen (tty_fd, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir a linha \"%s\" (%s)\n",
			pgname, tty_dev_nm, strerror (errno)
		);
		exit (1);
	}

	if ((tty_fp_out = fdopen (tty_fd, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir a linha \"%s\" (%s)\n",
			pgname, tty_dev_nm, strerror (errno)
		);
		exit (1);
	}

	if (ioctl (tty_fd, TCGETS, &o_mode) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui dar ioctl na linha \"%s\" (%s)\n",
			pgname, tty_dev_nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Abre o dispositivo PPP correspondente
	 */
#if (0)	/*******************************************************/
	if ((ppp_fd = open (ppp_dev_nm, O_RDWR|O_LOCK)) < 0)
#endif	/*******************************************************/
	if ((ppp_fd = open (ppp_dev_nm, O_RDWR)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, ppp_dev_nm, strerror (errno)
		);
		exit (1);
	}

	old_fifosz = ioctl (ppp_fd, TCFIFOSZ, 0);

	if (fifo_str != NOSTR && ioctl (ppp_fd, TCFIFOSZ, new_fifosz) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alterar o tamanho do FIFO (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	new_fifosz = ioctl (ppp_fd, TCFIFOSZ, 0);

	if (vflag) printf
	(
		"oldFIFO = %d, newFIFO = %d\n",
		old_fifosz, ioctl (ppp_fd, TCFIFOSZ, 0)
	);

#if (0)	/*******************************************************/
	if ((ppp_has_CTS = ioctl (tty_fd, TCGETCTS)) < 0)
	{
		if (vflag)
			printf ("%s: Erro no \"ioctl\" GETCTS\n", pgname);
	}
#endif	/*******************************************************/

	/*
	 *	Poe a Linha no modo de COMUNICAÇÕES
	 */
	mode = o_mode;

	mode.c_oflag &= ~(OPOST|CBAUD);

	if (speed_str != NOSTR)
		mode.c_cflag |= ep->speed;
	else
		mode.c_cflag |= DEF_SPEED;

	if ((ppp_has_CTS = ioctl (tty_fd, TCGETCTS)) < 0)
		fprintf (stderr, "%s: Erro no \"ioctl\" GETCTS\n", pgname);

	if (ppp_has_CTS > 0)
		mode.c_cflag &= ~CLOCAL;

	if (vflag)
		printf ("CTS = %d\n", ppp_has_CTS);

	mode.c_lflag &= ~(ICANON|ECHO|CODE);
	mode.c_lflag |=  (ISO|ICOMM);

	mode.c_cc[VMIN] =  1;

	ioctl (tty_fd, TCFLSH, 0);

	ioctl (tty_fd, TCSETAW, &mode);

	/*
	 *	Acerta o modo da linha PPP
	 */
	if (ioctl (ppp_fd, TCGETS, &ppp_termio) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui dar ioctl na linha \"%s\" (%s)\n",
			pgname, tty_dev_nm, strerror (errno)
		);
		exit (1);
	}

	if (ppp_has_CTS > 0)
		ppp_termio.c_cflag &= ~CLOCAL;

	ioctl (ppp_fd, TCSETNR, &ppp_termio);

	/*
	 *	Verifica se já existe o arquivo de registro
	 */
	if (dflag && access (log_nm, F_OK) == 0)
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" já existe. Reescreve? (n): ",
			pgname, log_nm
		);

		if (askyesno () <= 0)
			exit (0);
	}

	/*
	 *	Cria o arquivo de registro
	 */
	if (dflag && (log_fp = fopen (log_nm, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar o arquivo de registro (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Poe o Terminal em RAW
	 */
	video = o_video;

	video.c_oflag &= ~OPOST;

	video.c_lflag &= ~(ISIG|ICANON|ECHO|CODE);
	video.c_lflag |=  (ISO);

	ioctl (0, TCSETAW, &video);

	/*
	 *	Inicializa a estrutura de controle
	 */
	mutime (&t);

	sp->ppp_lcp_state	= LCP_STATE_CLOSED;
	sp->ppp_ipcp_state	= IPCP_STATE_CLOSED;
   /***	sp->ppp_my_addr_added	= 0; ***/

	sp->snd.ppp_magic_number = t.mu_time ^ t.mu_utime;
	sp->snd.ppp_async_char_map = 0xFFFFFFFF;
   /***	sp->snd.ppp_addr = 0; ***/
   /***	sp->snd.ppp_proto_field_compression = 0; ***/
   /***	sp->snd.ppp_addr_field_compression = 0; ***/
   /***	sp->snd.ppp_vj_compression = 0; ***/
   /***	sp->snd.ppp_vj_slot_compression = 0; ***/
	sp->snd.ppp_vj_max_slot = N_SLOT - 1;

   /***	sp->rcv.ppp_magic_number = 0; ***/
   /***	sp->rcv.ppp_async_char_map = 0; ***/
   /***	sp->rcv.ppp_addr = 0; ***/
	sp->rcv.ppp_proto_field_compression = 1;
	sp->rcv.ppp_addr_field_compression = 1;
   /***	sp->rcv.ppp_vj_compression = PPP_VJ_COM; ***/
   /***	sp->rcv.ppp_vj_slot_compression = 1; ***/
	sp->rcv.ppp_vj_max_slot = N_SLOT - 1;

   /***	memset (sp->ppp_char_map_vec, 0, sizeof (sp->ppp_char_map_vec)); ***/
   /***	sp->ppp_slcompress_ptr = NULL;	***/

	memset (sp->ppp_char_map_vec, 1, 32);
	sp->ppp_char_map_vec[PPP_FLAG] = 1;
	sp->ppp_char_map_vec[PPP_ESC] = 1;

	/*
	 *	Cria um filho para ler da linha e escrever no video.
	 *	O pai le do video e escreve na Linha
	 */
	signal (SIGINT, SIG_IGN);

	if ((pidfilho = thread ()) < 0)
	{
		printf ("%s: Não Consegui dar o fork (%s)\r\n", pgname, strerror (errno));
		exit (1);
	}

	if (pidfilho > 0)
		pai (pidfilho);
	else
		filho ();

	/* Sem retorno */

	return (1);

}	/* end ppp */

/*
 ****************************************************************
 *	Pai: Le do teclado e escreve na linha			*
 ****************************************************************
 */
void
pai (int pidfilho)
{
	int		state;	/* Estado: 0 => Esperando <nl> */
				/*	   1 => Esperando '~'  */
				/*	   2 => Esperando '.'  */
				/*	   3 => Esperando o 2. <nl> */
	char		cc;
	int		c;

	signal (SIGTERM, on_sig_term_from_child);
	state = 1;

	for (EVER)
	{
		read (0, &cc, 1); c = cc;

		if   (c == '\n' || c == '\r')
		{
			if (state == 3)
			{
				if (!ppp_mode)
				{
					cc = 0x15;	/* ^U */
					write (tty_fd, &cc, 1);
					printf ("\r\n");

					kill (pidfilho, SIGTERM);
					wait ((int *)NULL);
				}

				ioctl (0, TCSETAW, &o_video);

				_exit (0);
			}

			state = 1;

		}
		elif (c == '~')
		{
			if (state == 1)
				state = 2;

		}
		elif (c == '.')
		{
			if (state == 2)
				state = 3;

		}
		else
		{
				state = 0;
		}

		write (tty_fd, &cc, 1);

	}	/* end for ever */

}	/* end pai */

/*
 ****************************************************************
 *	Filho: Le da linha e escreve no video e em "log"	*
 ****************************************************************
 */
void
filho (void)
{
	FILE		*fp = tty_fp;
	int		c;
	int		n;
	char		area[4 * 512];
	char		*cp = area;
	char		*end_cp = cp + sizeof (area);
	char		cc, in_hdlc = 0;
	char		tentativas = 5 - 1;

	signal (SIGTERM, on_sig_term_from_parent);

	/*
	 *	Envia o prólogo
	 */
    again:
	if (text_str == NOSTR)
	{
		cc = '\n';		/* para dar o prompt */
		write (tty_fd, &cc, 1);
		(void)getc (fp);
		(void)getc (fp);
	}
	else
	{
		write (tty_fd, text_str, strlen (text_str));
		cc = '\r'; write (tty_fd, &cc, 1);
	}

	time (&begin_stamp);	/* Guarda o tempo de início da conexão */

	/*
	 *	Ainda NÃO recebeu nenhum pacote PPP
	 */
	for (cp = area; /* vazio */; /* vazio */)
	{
		c = getc (fp);

		if (dflag)
			putc (c, log_fp);

		switch (c)
		{
		    case '\0':
		    case '\r':
			continue;

		    default:
			cc = c; write (1, &cc, 1);

			*cp++ = c;

			if (cp - area == 7)	/* Levando em conta o branco */
			{
				if
				(	!strncmp (area, "login:", 6) ||
					!strncmp (area, "Login:", 6)
				)
				{
					fprintf (tty_fp_out, "%s\n", user_str); fflush (tty_fp_out);
				}
			}

			if (cp - area == 10)	/* Levando em conta o branco */
			{
				if   (!strncmp (area, "Username:", 9)) 
				{
					fprintf (tty_fp_out, "%s\n", user_str); fflush (tty_fp_out);
				}
				elif (!strncmp (area, "Password:", 9)) 
				{
					fprintf (tty_fp_out, "%s\n", pwd_str); fflush (tty_fp_out);
				}
			}

			continue;

		    case '\n':
			write (1, "\r\n", 2);
			*cp++ = '\0';

			if (analyse_modem_msg (area) < 0)
			{
				if (tentativas-- <= 0)
				{
					printf ("Terminando ...\r\n");
					kill (getppid (), SIGTERM);
					ioctl (0, TCSETAW, &o_video);
					_exit (0);
				}

				printf ("Tentando novamente ...\r\n");

				/* Desliga o MODEM */

				fprintf (tty_fp_out, "+++"); fflush (tty_fp_out);
				sleep (4);
		
				fprintf (tty_fp_out, "ATH0\r"); fflush (tty_fp_out);
				sleep (3);

				/* Reset do MODEM */

				fprintf (tty_fp_out, "ATZ\r"); fflush (tty_fp_out);
				sleep (4);

				if ((n = ioctl (tty_fd, TCNREAD)) > 0)
					read (tty_fd, area, n);

				goto again;
			}

			cp = area;
			continue;

		    case PPP_FLAG:
			break;

		}	/* end switch */

		/* Achou o PPP_FLAG; procura agora o PPP_ADDR */

	    got_flag:
		c = getc (fp);

		if (dflag)
			putc (c, log_fp);

		if (c == 0)
			continue;

		if (c == PPP_FLAG)
			{ goto got_flag; }

		if (c != PPP_ADDR)
			{ cc = PPP_FLAG; write (1, &cc, 1); cc = c; write (1, &cc, 1); continue; }

		/* Achou PPP_FLAG e PPP_ADDR - considera isto o início do PPP */

		cp = area; *cp++ = PPP_ADDR; break;

	}	/* end for (EVER) */

	/*
	 *	Já recebeu pacote PPP
	 */
	for (EVER)
	{
		c = getc (fp);

		if (dflag)
			putc (c, log_fp);

		if   (in_hdlc)
			{ c ^= PPP_EOR; in_hdlc = 0; }
		elif (c == PPP_ESC)
			{ in_hdlc++; continue; }

		if (c == PPP_FLAG)
		{
			if (cp == area)
				continue;

			if (dflag)
				fflush (log_fp);

			packet_proc (area, cp - area);
			cp = area;
			continue;
		}

		if (cp < end_cp)
			*cp++ = c;

	}	/* end for (EVER) */

}	/* end filho */

/*
 ****************************************************************
 *	Analisa a mensagem do MODEM				*
 ****************************************************************
 */
int
analyse_modem_msg (const char *area)
{
	const char	*str;
	int		status = 0;

	if   (!strncmp (area, "CONNECT", 7)) 
	{
		if ((speed_value = strtol (area + 8, &str, 10)) <= 0)
			speed_value = 0;
	}
	elif (!strncmp (area, "NO DIAL TONE", 12)) 
	{
		status = -1;
	}
	elif (!strncmp (area, "BUSY", 4)) 
	{
		status = -1;
	}
	elif (!strncmp (area, "NO CARRIER", 10)) 
	{
		status = -1;
	}

	return (status);

}	/* end analyse_modem_msg */

/*
 ****************************************************************
 *	Processa o pacote PPP					*
 ****************************************************************
 */
void
packet_proc (const char *area, int count)
{
	const char	*cp;
	int		proto;
	unsigned short	calculated_fcs, read_fcs;
	const char	*str;

	/*
	 *	Confere o CRC
	 *	Repare que estamos comparando coisas com o mesmo ENDIAN
	 */
	count -= 2;	/* Tira o FCS */

	calculated_fcs = ~(pppfcs (0xFFFF, area, count));

	read_fcs = *(unsigned short *)&area[count];

	if (calculated_fcs != read_fcs)
	{
#ifdef	DEBUG
		printf
		(	"PPP: Erro no FCS (0x%04X :: 0x%04X)\r\n",
			calculated_fcs, read_fcs
		);
#endif	DEBUG
		return;
	}

	/*
	 *	Retira o PROTOCOLO
	 */
	cp = area;

	if (*cp == PPP_ADDR)
		cp++;

	if (*cp == PPP_CTL)
		cp++;

	if (*cp & 1)
		{ proto = *cp++; }
	else
		{ proto = *cp++ << 8; proto |= *cp++; }

	/*
	 *	Examina o protocolo
	 */
	switch (proto)
	{
	    case PPP_IP:	/* Internet Protocol */
		str = "Internet Protocol";
		break;

	    case PPP_ISO:	/* ISO OSI Protocol */
		str = "ISO OSI Protocol";
		break;

	    case PPP_XNS:	/* Xerox NS Protocol */
		str = "Xerox NS Protocol";
		break;

	    case PPP_VJ_COM:	/* Van Jacobson Compressed TCP/IP */
		str = "Van Jacobson Compressed TCP/IP";
		break;

	    case PPP_VJ_UN:	/* Van Jacobson Uncompressed TCP/IP */
		str = "Van Jacobson Uncompressed TCP/IP";
		break;

	    case PPP_LCP:	/* Link Control Protocol */
		lcp_proc (cp);
		return;

	    case PPP_IPCP:	/* Internet Protocol Control Protocol */
		ipcp_proc (cp);
		return;

	    case PPP_AUTH_PAP:	/* Password Authentication Protocol */
		pap_proc (cp);
		return;

	    default:
		printf ("PPP: Protocolo 0x%04X desconhecido\r\n", proto);
		unknown_proc (cp, proto, count);
		return;
	}

	printf ("PPP: Protocolo \"%s\" recebido prematuramente\r\n");

}	/* end packet_proc */

/*
 ****************************************************************
 *	Processa o protocolo LCP				*
 ****************************************************************
 */
void
lcp_proc (const char *area)
{
	PPP_STATUS	*sp = &ppp_status;
	int		code;
	const char	*str;

	switch (code = *area)
	{
	    case LCP_CONF_REQ:		/* PPP LCP configure request */
		sp->ppp_lcp_state = LCP_STATE_CLOSED;
		printf ("\r\nNegociando LCP .... ");
		lcp_configure_request (area);
		return;

	    case LCP_CONF_ACK:		/* PPP LCP configure acknowledge */
		if (sp->ppp_lcp_state == LCP_STATE_ACK_SENT)
			sp->ppp_lcp_state = LCP_STATE_OPENED;

		printf ("OK:\r\n");

		if (vflag)
		{
			printf
			(	"SND: ASYNC_MAP = %P%s%s\r\n",
				sp->snd.ppp_async_char_map,
				sp->snd.ppp_proto_field_compression ? ", PROTO_COMP" : "",
				sp->snd.ppp_addr_field_compression ? ", ADDR_COMP" : ""
			);

			printf
			(	"RCV: ASYNC_MAP = %P%s%s\r\n",
				sp->rcv.ppp_async_char_map,
				sp->rcv.ppp_proto_field_compression ? ", PROTO_COMP" : "",
				sp->rcv.ppp_addr_field_compression ? ", ADDR_COMP" : ""
			);
		}
		return;

	    case LCP_CONF_NAK:		/* PPP LCP configure negative ack */
		str = "configure negative ack";
		break;

	    case LCP_CONF_REJ:		/* PPP LCP configure reject */
		str = "configure reject";
		break;

	    case LCP_TERM_REQ:		/* PPP LCP $erminate request */
		str = "configure reject";
		break;

	    case LCP_TERM_ACK:		/* PPP LCP terminate acknowledge */
		str = "terminate acknowledge";
		break;

	    case LCP_CODE_REJ:		/* PPP LCP code reject */
		str = "code reject";
		break;

	    case LCP_PROTO_REJ:		/* PPP LCP protocol reject */
		str = "protocol reject";
		break;

	    case LCP_ECHO_REQ:		/* PPP LCP echo request */
		lcp_reply_echo_request (area);
		return;

	    case LCP_ECHO_REPLY:	/* PPP LCP echo reply */
		str = "echo reply";
		break;

	    case LCP_DISC_REQ:		/* PPP LCP discard request */
		return;

	    default:
		printf ("LCP: Código %d desconhecido\r\n", code);
		return;
	}

	printf ("LCP: Código \"PPP LCP %s\" ainda NÃO processado\r\n");

}	/* end lcp_proc */

/*
 ****************************************************************
 *	LCP: Configure request					*
 ****************************************************************
 */
void
lcp_configure_request (const char *area)
{
	const char	*cp;
	PPP_STATUS	*sp = &ppp_status;
	const LCP_H	*lp;
	int		c;
	int		id, count, op, len;
	int		authen_prot = 0;
	const char	*begin;
	unsigned long	mask;
	const char	*str;
	struct
	{
		ASYNC_MAP_OPT	async_map_opt;
		MAGIC_OPT	magic_opt;
		PROTO_COMP_OPT	proto_comp_opt;
		ADDR_COMP_OPT	addr_comp_opt;

	}	options;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = cp = area + sizeof (LCP_H);

	/*
	 *	Inicializa as opções LCP
	 */
	sp->snd.ppp_async_char_map = 0xFFFFFFFF;
   /***	sp->snd.ppp_addr = 0; ***/
	sp->snd.ppp_proto_field_compression = 0;
	sp->snd.ppp_addr_field_compression = 0;

	sp->rcv.ppp_async_char_map = 0;		/* Tudo "ao natural" */
   /***	sp->rcv.ppp_addr = 0; ***/
	sp->rcv.ppp_proto_field_compression = 1;
	sp->rcv.ppp_addr_field_compression = 1;

	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0]; len = cp[1]; str = NOSTR;	/* Parte comum das opções */

		switch (op)
		{
		    case LCP_OPT_MRU:		/* maximum receive unit */
			str = "maximum receive unit";
			break;

		    case LCP_OPT_ASYNC_MAP:	/* async control character map */
			mask = ENDIAN_LONG (*(long *)(cp + 2));
			sp->snd.ppp_async_char_map = mask;

			for (c = 0; c < 32; c++)
				{ sp->ppp_char_map_vec[c] = (mask & 1); mask >>= 1; }
			break;

		    case LCP_OPT_AUTH_PROTO:	/* authentication protocol */
			switch (authen_prot = ENDIAN_SHORT (*(short *)(cp + 2)))
			{
			    case PPP_AUTH_PAP:
				break;

			    case PPP_AUTH_CHAP:
				printf
				(	"LCP: Configure request: "
					"Autenticação CHAP ainda não processada\r\n"
				);
				break;

			    default:
				printf
				(	"LCP: Configure request: "
					"Autenticação %04X desconhecida\r\n",
					authen_prot
				);
				break;
			}

			break;

		    case LCP_OPT_QUAL_PROTO:	/* quality protocol */
			str = "quality protocol";
			break;

		    case LCP_OPT_MAGIC:		/* magic number */
			sp->rcv.ppp_magic_number = ENDIAN_LONG (*(long *)(cp + 2));
			break;

		    case LCP_OPT_RESERVED:	/* reserved */
			str = "reserved";
			break;

		    case LCP_OPT_PROTO_COMP:	/* protocol field compression */
			sp->snd.ppp_proto_field_compression++;
			break;

		    case LCP_OPT_ADDR_COMP:	/* address/control field compression */
			sp->snd.ppp_addr_field_compression++;
			break;

		    default:
			printf ("LCP: Configure request: Opção %d desconhecida\r\n", op);
			break;

		}	/* end switch (opção) */

		if (str)
			printf ("LCP: Configure request: Opção \"%s\" ainda não processada\r\n", str);

		cp += len; count -= len;

	}	/* end for (EVER) */

	/*
	 *	Envia o meu "Configure request"
	 */
	options.async_map_opt.code = LCP_OPT_ASYNC_MAP;
	options.async_map_opt.len  = sizeof (ASYNC_MAP_OPT);
	*(long *)options.async_map_opt.map = sp->rcv.ppp_async_char_map;

	options.magic_opt.code = LCP_OPT_MAGIC;
	options.magic_opt.len  = sizeof (MAGIC_OPT);
	*(long *)options.magic_opt.magic = sp->snd.ppp_magic_number;

	options.proto_comp_opt.code = LCP_OPT_PROTO_COMP;
	options.proto_comp_opt.len  = sizeof (PROTO_COMP_OPT);

	options.addr_comp_opt.code = LCP_OPT_ADDR_COMP;
	options.addr_comp_opt.len  = sizeof (PROTO_COMP_OPT);

	ppp_lcp_packet_snd (PPP_LCP, LCP_CONF_REQ, id_source++, &options, sizeof (options));

	/*
	 *	Envia o ACK
	 */
	ppp_lcp_packet_snd (PPP_LCP, LCP_CONF_ACK, id, begin, cp - begin);

	/*
	 *	Se for o caso, envia a conta e senha
	 */
	if (authen_prot)
		lcp_authen_request (authen_prot);

	sp->ppp_lcp_state  = LCP_STATE_ACK_SENT;

}	/* end lcp_configure_request */

/*
 ****************************************************************
 *	Envia o pedido de autenticação				*
 ****************************************************************
 */
void
lcp_authen_request (int authen_prot)
{
	char		area[64];
	char		*cp = area;
	int		len;

	/*
	 *	Analisa o protocolo
	 */
	switch (authen_prot)
	{
	    case PPP_AUTH_PAP:
		break;

	    case PPP_AUTH_CHAP:
		printf
		(	"LCP: Configure request: "
			"Autenticação CHAP ainda não processada\r\n"
		);
		return;

	    default:
		printf
		(	"LCP: Configure request: "
			"Autenticação %04X desconhecida\r\n",
			authen_prot
		);
		return;
	}

#if (0)	/*******************************************************/
	/*
	 *	Obtém a conta
	 */
	if (user_str == NOSTR)
		user_str = getenv ("USER");

	if (user_str == NOSTR)
		user_str = getlogin ();

	if (user_str == NOSTR)
	{
		kill (getppid (), SIGTERM);
#if (0)	/*******************************************************/
		kill (pidfilho, SIGTERM);
		wait ((int *)NULL);
#endif	/*******************************************************/

		ioctl (0, TCSETAW, &o_video);

		_exit (0);
	}

	/*
	 *	Obtém a senha
	 */
	if (pwd_str == NOSTR)
		pwd_str = getpass ("Senha: ");
#endif	/*******************************************************/

	/*
	 *	Prepara a estrutura
	 */
	len = strlen (user_str); *cp++ = len;

	strcpy (cp, user_str); cp += len;

	len = strlen (pwd_str); *cp++ = len;

	strcpy (cp, pwd_str); cp += len;

#if (0)	/*******************************************************/
	for (rp = user_str; len > 0; len--)
		*cp++ = *rp++;

	len = strlen (pwd_str); *cp++ = len;

	for (rp = pwd_str; len > 0; len--)
		*cp++ = *rp++;
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	*cp++ = 2;
	*cp++ = 'i';
	*cp++ = 'g';

	*cp++ = 2;
	*cp++ = 'i';
	*cp++ = 'g';
#endif	/*******************************************************/

	ppp_lcp_packet_snd (PPP_AUTH_PAP, LCP_CONF_REQ, id_source++, area, cp - area);

	if (vflag)
		printf ("enviei %s:%s\r\n", user_str, pwd_str);

} 	/* end lcp_authen_request */

/*
 ****************************************************************
 *	Processa o protocolo PAP				*
 ****************************************************************
 */
void
pap_proc (const char *area)
{
	int		code;

	switch (code = *area)
	{
	    case LCP_CONF_REQ:		/* PPP LCP configure request */
		printf ("\r\nPAP: Ainda não aceitando pedidos de configuração\r\n");
		return;

	    case LCP_CONF_ACK:		/* PPP LCP configure acknowledge */
		printf ("\r\nPAP: A conta/senha foi aceita\r\n");
		return;

	    case LCP_CONF_NAK:		/* PPP LCP configure negative ack */
		printf ("\r\nPAP: A conta/senha foi rejeitada\r\n");
		return;

	    default:
		printf ("PAP: Código %d desconhecido\r\n", code);
		return;
	}

}	/* end pap_proc */

/*
 ****************************************************************
 *	Protocolo desconhecido					*
 ****************************************************************
 */
void
unknown_proc (const char *area, int proto, int count)
{
	char	info[16];

	info[0] = proto >> 8; info[1] = proto;
	memmove (info + 2, area, sizeof (info) - 2);

	ppp_lcp_packet_snd (PPP_LCP, LCP_PROTO_REJ, id_source++, &info, sizeof (info));

	printf ("unknown_proc: Rejeitei o protocolo %04X\r\n", proto);

}	/* unknown_proc */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	LCP: Terminate-Request					*
 ****************************************************************
 */
void
lcp_terminate_request (void)
{
	ppp_lcp_packet_snd (PPP_LCP, LCP_TERM_REQ, id_source++, NOSTR, 0);

}	/* end lcp_terminate_request */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	LCP: Reply echo request					*
 ****************************************************************
 */
void
lcp_reply_echo_request (const char *area)
{
	const LCP_H	*lp;
	PPP_STATUS	*sp = &ppp_status;
	const char	*begin;
	int		id, count;
	char		reply_area[80];
	char		*ap;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = area + sizeof (LCP_H);

	/*
	 *	Envia a resposta (o eco)
	 */
	ap = reply_area;

	memmove (ap, begin, count);
	*(long *)ap = sp->snd.ppp_magic_number;

	ppp_lcp_packet_snd (PPP_LCP, LCP_ECHO_REPLY, id, reply_area, count);

}	/* end lcp_reply_echo_request */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	LCP: Send echo request					*
 ****************************************************************
 */
void
lcp_send_echo_request (void)
{
	PPP_STATUS	*sp = &ppp_status;
	char		area[80];
	int		len;
	char		*str;
	char		*ap;

	ap = area;
	str = "Texto para ser ecoado"; len = strlen (str);

	*(long *)ap = sp->snd.ppp_my_magic_number;
	memmove (&ap[4], str, len);

	ppp_lcp_packet_snd (PPP_LCP, LCP_ECHO_REQ, id_source++, area, 4 + len);

}	/* end lcp_send_echo_request */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Processa o protocolo IPCP				*
 ****************************************************************
 */
void
ipcp_proc (const char *area)
{
	PPP_STATUS	*sp = &ppp_status;
	int		code;

	switch (code = *area)
	{
	    case IPCP_CONF_REQ:		/* PPP IPCP configure request */
		sp->ppp_ipcp_state = IPCP_STATE_CLOSED;

#if (0)	/*******************************************************/
	static char		Negociando_written = 0;
		if (Negociando_written++ == 0)
#endif	/*******************************************************/
			printf ("Negociando IPCP ... ");

		ipcp_configure_request (area);
		return;

	    case IPCP_CONF_ACK:		/* PPP IPCP configure acknowledge */
		if (sp->ppp_ipcp_state == IPCP_STATE_ACK_SENT)
			sp->ppp_ipcp_state = IPCP_STATE_OPENED;

		printf ("OK:\r\n");

		/* Parametros de transmissão */

		printf ("SND: ADDR = %s", t_addr_to_str (sp->snd.ppp_addr));

		if (sp->snd.ppp_vj_compression)
		{
			printf (", VJ_SND (%d)", sp->snd.ppp_vj_max_slot + 1);

			if (sp->snd.ppp_vj_slot_compression)
				printf (", VJ_SLOT");
		}

		printf ("\r\n");

		/* Parametros de recepção */

		printf ("RCV: ADDR = %s", t_addr_to_str (sp->rcv.ppp_addr));

		if (sp->rcv.ppp_vj_compression)
		{
			printf (", VJ_RCV (%d)", sp->rcv.ppp_vj_max_slot + 1);

			if (sp->rcv.ppp_vj_slot_compression)
				printf (", VJ_SLOT");
		}

		printf ("\r\n");

		/*
		 *	Prepara o funcionamento PPP
		 */
		ioctl (tty_fd, TCFLSH, 2);

		if (ioctl (ppp_fd, TCPPPINT, sp) < 0)
		{
			fprintf
			(	stderr,
				"%s: NÃO consegui inicializar o PPP (%s)\r\n",
				pgname, strerror (errno)
			);
		}
		printf ("Entrando em modo PPP, Velocidade = %d baud, pid = %d\r\n", speed_value, getpid ());

		ppp_mode++;

		ioctl (0, TCSETAW, &o_video);

		kill (getppid (), SIGTERM);

		signal (SIGTERM, on_sig_term_to_disconnect);

		pause ();
		return;

	    case IPCP_CONF_NAK:		/* PPP IPCP configure negative ack */
		ipcp_configure_nak (area);
		return;

	    case IPCP_CONF_REJ:		/* PPP IPCP configure reject */
		ipcp_configure_reject (area);
		return;

	/** case IPCP_TERM_REQ:		/* PPP IPCP terminate request */
	/** case IPCP_TERM_ACK:		/* PPP IPCP terminate acknowledge */
	/** case IPCP_CODE_REJ:		/* PPP IPCP code reject */

	    default:
		printf ("IPCP: Código ainda NÃO processado: %d\r\n", code);
		return;
	}

}	/* end ipcp_proc */

/*
 ****************************************************************
 *	IPCP: Configure request					*
 ****************************************************************
 */
void
ipcp_configure_request (const char *area)
{
	const char	*cp;
	PPP_STATUS	*sp = &ppp_status;
	const LCP_H	*lp;
	int		id, count, op, len;
	const char	*comp_cp = NOSTR, *begin;
	int		comp_len = 0;
	const VJ_OPT	*vp;
	struct
	{
		VJ_OPT		vj_opt;
		IPADDR_OPT	ipaddr_opt;

	}	options;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = cp = area + sizeof (LCP_H);

	/*
	 *	Analisa as opções
	 */
	sp->snd.ppp_vj_compression = 0;
	sp->snd.ppp_vj_slot_compression = 0;
	sp->snd.ppp_vj_max_slot = N_SLOT - 1;

	if (jflag)
		sp->rcv.ppp_vj_compression = 0;
	else
		sp->rcv.ppp_vj_compression = PPP_VJ_COM;

	sp->rcv.ppp_vj_slot_compression = 1;
	sp->rcv.ppp_vj_max_slot = N_SLOT - 1;

	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0]; len = cp[1];	/* Parte comum das opções */

		switch (op)
		{
		    case IPCP_OPT_VJ:		/* Compressão TCP/IP */
			comp_cp = cp; comp_len = len;

			vp = (VJ_OPT *)cp;
#ifdef	DEBUG
			printf
			(	"\r\nlen = %d, comp = %02X, slot_comp = %02X, max_slot = %02X\r\n",
				len, vp->p, vp->comp_slot, vp->max_slot
			);
#endif	DEBUG
			sp->snd.ppp_vj_compression = ENDIAN_SHORT (vp->p);

			if (len >= sizeof (VJ_OPT))
			{
				sp->snd.ppp_vj_slot_compression = vp->comp_slot;
				sp->snd.ppp_vj_max_slot = vp->max_slot;
			}
			break;

		    case IPCP_OPT_IPADDR:	/* Endereço IP */
			sp->rcv.ppp_addr = ENDIAN_LONG (*(long *)(cp + 2));
			break;

		    default:
			printf ("IPCP: Configure request: Opção %d ainda NÃO processada\r\n", op);
			break;
		}

		cp += len; count -= len;

	}	/* end for (EVER) */

	/*
	 *	Envia o meu "Configure request" com compressão
	 */
	if (jflag)
		goto no_vj;

	options.vj_opt.code	 = IPCP_OPT_VJ;
	options.vj_opt.len	 = sizeof (VJ_OPT);
	options.vj_opt.p	 = ENDIAN_SHORT (sp->rcv.ppp_vj_compression);
	options.vj_opt.max_slot  = sp->rcv.ppp_vj_max_slot;
	options.vj_opt.comp_slot = sp->rcv.ppp_vj_slot_compression;

	options.ipaddr_opt.code  = IPCP_OPT_IPADDR;
	options.ipaddr_opt.len   = sizeof (IPADDR_OPT);
	*(long *)options.ipaddr_opt.ipaddr = 0;

	ppp_lcp_packet_snd (PPP_IPCP, IPCP_CONF_REQ, id_source++, &options, sizeof (options));
	goto snd_ack;

	/*
	 *	Envia o meu "Configure request" sem compressão
	 */
    no_vj:
	sp->snd.ppp_vj_compression = 0;		/* NÃO usa VJ na saída */

	options.ipaddr_opt.code = IPCP_OPT_IPADDR;
	options.ipaddr_opt.len  = sizeof (IPADDR_OPT);
	*(long *)options.ipaddr_opt.ipaddr = 0;

	ppp_lcp_packet_snd (PPP_IPCP, IPCP_CONF_REQ, id_source++, &options.ipaddr_opt, sizeof (options.ipaddr_opt));

	/*
	 *	Envia o ACK
	 */
    snd_ack:
	ppp_lcp_packet_snd (PPP_IPCP, IPCP_CONF_ACK, id, begin, cp - begin);

	sp->ppp_ipcp_state = IPCP_STATE_ACK_SENT;

}	/* end ipcp_configure_request */

/*
 ****************************************************************
 *	IPCP: Configure NAK					*
 ****************************************************************
 */
void
ipcp_configure_nak (const char *area)
{
	const char	*cp = area;
	PPP_STATUS	*sp = &ppp_status;
	const LCP_H	*lp;
	int		id, count, op, len;
	const char	*begin;
	IPADDR_OPT	ipaddr_opt;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = cp = area + sizeof (LCP_H);

	/*
	 *	Analisa as opções
	 */
	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0]; len = cp[1];	/* Parte comum das opções */

		switch (op)
		{
		    case IPCP_OPT_IPADDR:	/* Endereço IP */
			sp->snd.ppp_addr = ENDIAN_LONG (*(long *)(cp + 2));
			break;

		    default:
			printf ("IPCP: Configure request: Opção %d ainda NÃO processada\r\n", op);
			break;
		}

		cp += len; count -= len;

	}	/* end for (EVER) */

	/*
	 *	Supondo que não recusou a compressão VJ
	 */
#if (0)	/*******************************************************/
	sp->snd.ppp_vj_compression++;
	sp->snd.ppp_vj_slot_compression = 1;
	sp->snd.ppp_vj_max_slot = N_SLOT - 1;
#endif	/*******************************************************/

	/*
	 *	Envia o meu novo "Configure request"
	 */
	ipaddr_opt.code = IPCP_OPT_IPADDR;
	ipaddr_opt.len  = sizeof (IPADDR_OPT);
	*(long *)ipaddr_opt.ipaddr = ENDIAN_LONG (sp->snd.ppp_addr);

	ppp_lcp_packet_snd (PPP_IPCP, IPCP_CONF_REQ, id_source++, &ipaddr_opt, sizeof (IPADDR_OPT));

}	/* end ipcp_configure_nak */

/*
 ****************************************************************
 *	IPCP: Configure REJECT					*
 ****************************************************************
 */
void
ipcp_configure_reject (const char *area)
{
	const char	*cp = area;
	PPP_STATUS	*sp = &ppp_status;
	const LCP_H	*lp;
	int		id, count, op, len;
	const char	*begin;
	IPADDR_OPT	ipaddr_opt;

	/*
	 *	Analisa o cabeçalho LCP
	 */
	lp = (LCP_H *)area;

	id    = lp->id;
	count = ENDIAN_SHORT (lp->len) - sizeof (LCP_H);
	begin = cp = area + sizeof (LCP_H);

	/*
	 *	Analisa as opções
	 */
	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0]; len = cp[1];	/* Parte comum das opções */

		switch (op)
		{
		    case IPCP_OPT_VJ:		/* Compressão TCP/IP */
			printf ("REJEITOU VJ ... ");

			jflag = 1;
			sp->rcv.ppp_vj_compression = 0;
			break;

		    default:
			printf ("IPCP: Configure reject: Opção %d ainda NÃO processada\r\n", op);
			break;
		}

		cp += len; count -= len;

	}	/* end for (EVER) */

	/*
	 *	Envia o meu novo "Configure request"
	 */
	ipaddr_opt.code = IPCP_OPT_IPADDR;
	ipaddr_opt.len  = sizeof (IPADDR_OPT);
	*(long *)ipaddr_opt.ipaddr = ENDIAN_LONG (sp->snd.ppp_addr);

	ppp_lcp_packet_snd (PPP_IPCP, IPCP_CONF_REQ, id_source++, &ipaddr_opt, sizeof (IPADDR_OPT));

}	/* end ipcp_configure_reject */

/*
 ****************************************************************
 *	Send LCP packet						*
 ****************************************************************
 */
void
ppp_lcp_packet_snd (int proto, int code, int id, const void *area, int count)
{
	char		*cp;
	int		c;
	int		len;
	char		*bp;
	char		edit_area[128], out_area[256];
	unsigned short	fcs;

	/*
	 *	Prepara o cabeçalho
	 */
	bp = edit_area;

	bp[0] = PPP_ADDR;		/* Address */
	bp[1] = PPP_CTL;		/* Control */
	bp[2] = proto >> 8;		/* Protocol */
	bp[3] = proto;

	len = count + 4;

	bp[4] = code;			/* Code */
	bp[5] = id;			/* ID */
	bp[6] = len >> 8;		/* Len */
	bp[7] = len;

	memmove (&bp[8], area, count);

	count += 4 + 4;			/* Incorpora o cabeçalho */

	/*
	 *	Insere o CRC
	 */
	fcs = ~(pppfcs (0xFFFF, edit_area, count));

	bp = &edit_area[count]; *bp++ = fcs; *bp++ = fcs >> 8;

	count += 2;			/* Incorpora o CRC */

	/*
	 *	Prepara a área do pacote
	 */
	bp = edit_area, cp = out_area;

	*cp++ = PPP_FLAG;	/* PROVISÓRIO */

	for (/* acima */; count > 0; count--)
	{
		c = *bp++;

		if (c < 0x20 || c == PPP_ESC || c == PPP_FLAG)
			{ *cp++ = PPP_ESC; *cp++ = c ^ PPP_EOR; }
		else
			*cp++ = c;
	}

	*cp++ = PPP_FLAG;

	write (tty_fd, out_area, cp - out_area);

	if (dflag)
	{
		fprintf (log_fp, "<<<");
		fwrite	(out_area, cp - out_area, 1, log_fp);
		fprintf (log_fp, ">>>");
		fflush  (log_fp);
	}

}	/* end ppp_lcp_packet_snd */

/*
 ****************************************************************
 *	Cálculo do CRC						*
 ****************************************************************
 */
int
pppfcs (int fcs, const char *cp, int count)
{
	while (count-- > 0)
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xFF];

	return (fcs);

}	/* end pppfcs */

/*
 ****************************************************************
 *	Tabela de CRC						*
 ****************************************************************
 */
const unsigned short	fcstab[256] =
{
	0x0000,0x1189,0x2312,0x329B,0x4624,0x57AD,0x6536,0x74BF,
	0x8C48,0x9DC1,0xAF5A,0xBED3,0xCA6C,0xDBE5,0xE97E,0xF8F7,
	0x1081,0x0108,0x3393,0x221A,0x56A5,0x472C,0x75B7,0x643E,
	0x9CC9,0x8D40,0xBFDB,0xAE52,0xDAED,0xCB64,0xF9FF,0xE876,
	0x2102,0x308B,0x0210,0x1399,0x6726,0x76AF,0x4434,0x55BD,
	0xAD4A,0xBCC3,0x8E58,0x9FD1,0xEB6E,0xFAE7,0xC87C,0xD9F5,
	0x3183,0x200A,0x1291,0x0318,0x77A7,0x662E,0x54B5,0x453C,
	0xBDCB,0xAC42,0x9ED9,0x8F50,0xFBEF,0xEA66,0xD8FD,0xC974,
	0x4204,0x538D,0x6116,0x709F,0x0420,0x15A9,0x2732,0x36BB,
	0xCE4C,0xDFC5,0xED5E,0xFCD7,0x8868,0x99E1,0xAB7A,0xBAF3,
	0x5285,0x430C,0x7197,0x601E,0x14A1,0x0528,0x37B3,0x263A,
	0xDECD,0xCF44,0xFDDF,0xEC56,0x98E9,0x8960,0xBBFB,0xAA72,
	0x6306,0x728F,0x4014,0x519D,0x2522,0x34AB,0x0630,0x17B9,
	0xEF4E,0xFEC7,0xCC5C,0xDDD5,0xA96A,0xB8E3,0x8A78,0x9BF1,
	0x7387,0x620E,0x5095,0x411C,0x35A3,0x242A,0x16B1,0x0738,
	0xFFCF,0xEE46,0xDCDD,0xCD54,0xB9EB,0xA862,0x9AF9,0x8B70,
	0x8408,0x9581,0xA71A,0xB693,0xC22C,0xD3A5,0xE13E,0xF0B7,
	0x0840,0x19C9,0x2B52,0x3ADB,0x4E64,0x5FED,0x6D76,0x7CFF,
	0x9489,0x8500,0xB79B,0xA612,0xD2AD,0xC324,0xF1BF,0xE036,
	0x18C1,0x0948,0x3BD3,0x2A5A,0x5EE5,0x4F6C,0x7DF7,0x6C7E,
	0xA50A,0xB483,0x8618,0x9791,0xE32E,0xF2A7,0xC03C,0xD1B5,
	0x2942,0x38CB,0x0A50,0x1BD9,0x6F66,0x7EEF,0x4C74,0x5DFD,
	0xB58B,0xA402,0x9699,0x8710,0xF3AF,0xE226,0xD0BD,0xC134,
	0x39C3,0x284A,0x1AD1,0x0B58,0x7FE7,0x6E6E,0x5CF5,0x4D7C,
	0xC60C,0xD785,0xE51E,0xF497,0x8028,0x91A1,0xA33A,0xB2B3,
	0x4A44,0x5BCD,0x6956,0x78DF,0x0C60,0x1DE9,0x2F72,0x3EFB,
	0xD68D,0xC704,0xF59F,0xE416,0x90A9,0x8120,0xB3BB,0xA232,
	0x5AC5,0x4B4C,0x79D7,0x685E,0x1CE1,0x0D68,0x3FF3,0x2E7A,
	0xE70E,0xF687,0xC41C,0xD595,0xA12A,0xB0A3,0x8238,0x93B1,
	0x6B46,0x7ACF,0x4854,0x59DD,0x2D62,0x3CEB,0x0E70,0x1FF9,
	0xF78F,0xE606,0xD49D,0xC514,0xB1AB,0xA022,0x92B9,0x8330,
	0x7BC7,0x6A4E,0x58D5,0x495C,0x3DE3,0x2C6A,0x1EF1,0x0F78
};

/*
 ****************************************************************
 *	O Filho recebe o sinal do pai				*
 ****************************************************************
 */
void
on_sig_term_from_parent (int signo, ...)
{
	if (dflag && log_fp != NOFILE)
		fclose (log_fp);

	ioctl (tty_fd, TCFLSH, 2);

	close (ppp_fd);
	close (tty_fd);

	exit (0);

}	/* end on_sig_term_from_parent */

/*
 ****************************************************************
 *	O Pai recebe o sinal do filho				*
 ****************************************************************
 */
void
on_sig_term_from_child (int signo, ...)
{
	_exit (0);

}	/* end on_sig_term_from_child */

/*
 ****************************************************************
 *	Desconecta a linha PPP					*
 ****************************************************************
 */
void
on_sig_term_to_disconnect (int signo, ...)
{
	int		seg, min, hor, dias;

	/*
	 *	Obtém o tempo de uso da linha PPP
	 */
	seg = time ((time_t *)NULL) - begin_stamp;

	/*
	 *	Calcula o tempo decorrido em dias, horas, ...
	 */
	min  = seg / 60;
	seg  = seg % 60;

	hor  = min / 60;
	min  = min % 60;

	dias = hor / 24;
	hor  = hor % 24;

	printf
	(	"\n%s: Fim da conexão, Velocidade = %d baud, Tempo = %02d:%02d:%02d\n",
		pgname, speed_value, hor, min, seg
	);

	exit (0);

}	/* end on_sig_term_to_disconnect */

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
		"%s - cria uma comunicação PPP com um nó remoto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-l <linha>] [-t <texto>] [-p <conta>:<senha>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Define a <linha> de comunicações a utilizar\n"
		"\t    (se não dada, é utilizada a linha \"/dev/pppline\")\n"
		"\t-t: Escreve o <texto> de inicialização do modem\n"
		"\t    (contendo o número do telefone)\n"
#if (0)	/*******************************************************/
		"\t-s: Atribui a velocidade <vel> da linha\n"
		"\t    (se não dada, será usada a maior velocidade possível)\n"
	"\t%s [-l <linha>] [-s <vel>] [-t <texto>] [-f <n>] [-j] [-p <conta:senha>]\n",
		"\t-f: Escreve no máximo <n> caracteres na linha serial por interrupção\n"
		"\t    (valores válidos de 1 a 16, dependendo da linha serial)\n"
		"\t-j: NÃO utiliza a compressão TCP/IP de \"Van Jacobson\"\n"
#endif	/*******************************************************/
		"\t-p: Usa a <conta:senha> dada\n"
		"\nObs.:\tPara desconectar use \"ps -k\" ou \"kill ...\"\n"
	);

	exit (2);

}	/* end help */
