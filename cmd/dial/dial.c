/*
 ****************************************************************
 *								*
 *			dial.c					*
 *								*
 *	Disca através do "modem"				*
 *								*
 *	Versão	1.0.0, de 03.02.97				*
 *								*
 *	Módulo: dial.c						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termio.h>
#include <signal.h>
#include <errno.h>

/*
 ****************************************************************
 *	Nome e Versão do Programa				*
 ****************************************************************
 */
const char	pgversion[]	=  "Versão 3.0.0, de 03.02.97";

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	EVER	;;
#define	elif	else if

const char	*linename	= "/dev/znetline";	/* Linha default */
const char	dial_str[]	= "atx3pd";	/* Comando de discagem */

const char	*phone;
int		phone_len;

int		linefd;		/* fd da linha */
TERMIO		omode, mode;	/* modos da linha */
TERMIO		ovideo, video;	/* modos do terminal */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (int);
void		input (void);
int		process_command (char);
char		*get_phone_number (const char *);

/*
 ****************************************************************
 *	Comunicação com estação remota				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int	opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "l:H")) != EOF)
	{
		switch (opt)
		{
		    case 'l':			/* line name */
			linename = optarg;
			break;

		    case 'H':			/* Help */
			help (0);

		    default:			/* Erro */
			putc ('\n', stderr);
			help (2);

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (*argv == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: falta o nome da pessoa ou número do telefone\n",
			pgname
		);
		exit (1);
	}

	if ((phone = get_phone_number (*argv)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: não encontrei o telefone de %s\n",
			pgname, *argv
		);
		exit (1);
	}

	phone_len = strlen (phone);

	/*
	 *	Analisa e prepara a linha
	 */
	if ((linefd = open (linename, O_RDWR|O_LOCK|O_NDELAY)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir a linha \"%s\" (%s)\n",
			pgname, linename, strerror (errno)
		);
		exit (1);
	}

	if (ioctl (linefd, TCGETS, &omode) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui dar ioctl na linha \"%s\" (%s)\n",
			pgname, linename, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Examina a entrada padrão
	 */
	if (ioctl (0, TCGETS, &ovideo) < 0  ||  isatty (1) == 0)
	{
		fprintf
		(	stderr,
			"%s: O <stdin> e/ou <stdout> não é um terminal\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Poe a Linha no modo de COMUNICAÇÕES
	 */
	mode = omode;

	mode.c_oflag   &= ~OPOST;
	mode.c_lflag   &= ~(ICANON|ECHO|CODE);
	mode.c_lflag   |=  (ISO|ICOMM);
	mode.c_cc[VMIN] = 1;

	ioctl (linefd, TCFLSH, 0);
	ioctl (linefd, TCSETAW, &mode);

	/*
	 *	Poe o Terminal em RAW
	 */
	video = ovideo;

	video.c_oflag &= ~OPOST;
	video.c_lflag &= ~(ISIG|ICANON|ECHO|CODE);
	video.c_lflag |=  (ISO);

	ioctl (0, TCSETAW, &video);

	input ();

	ioctl (linefd, TCSETAW, &omode);
	ioctl (0, TCSETAW, &ovideo);

	close (linefd);

	return (0);

}	/* end phone */

/*
 ****************************************************************
 *	Realiza o "polling" dos descritores			*
 ****************************************************************
 */
void
input (void)
{
	int	fdvec[2], n;
	char	ch, buf[32];

	write (linefd, dial_str, sizeof (dial_str) - 1);
	write (linefd, phone, phone_len);
	write (linefd, "\r", 1);

	fdvec[0] = 0;
	fdvec[1] = linefd;

	for (EVER)
	{
		switch (attention (2, fdvec, -1, 0))
		{
		    case 0:
			read (0, &ch, 1);
			if (process_command (ch))
				return;
			break;

		    case 1:
			while ((n = read (linefd, buf, sizeof (buf))) > 0)
				write (1, buf, n);
			break;
		}
	}

}	/* end commands */

/*
 ****************************************************************
 *	Processa um comando					*
 ****************************************************************
 */
int
process_command (char ch)
{
	switch (ch)
	{
	    case '\r':
	    case '~':
	    case '.':
	    case 'q':
	    case 'Q':
	    case 27:
		return (1);

	    case 'h':
	    case 'H':
		write (linefd, "ath0\r", 5);
		break;

	    case 'z':
	    case 'Z':
		write (linefd, "atz\r", 4);
		break;

	    case 'r':
	    case 'R':
		write (linefd, dial_str, sizeof (dial_str) - 1);
		write (linefd, phone, phone_len);
		write (linefd, "\r", 1);
		break;

	    case '?':
		printf
		(	"\r\n"
			"Comandos:\r\n"
			"\th        - desliga o telefone\r\n"
			"\tz        - dá um reset no modem\r\n"
			"\tr        - torna a discar\r\n"
			"\tq, <Esc> - termina o programa\r\n"
			"\n"
		);
		break;

	    default:
		printf ("**** Comando inválido: %c\r\n", ch);
	}

	return (0);

}	/* end process_command */

/*
 ****************************************************************
 *	Obtém o número a discar					*
 ****************************************************************
 */
char *
get_phone_number (const char *name)
{
	static char	agenda[128], buf[80], cname [40];
	char		*cp, *ccp, c;
	const char	*home, *ncp;
	FILE		*fp;

	if ((c = name[0]) >= '0' && c <= '9')
		return ((char *)name);

	/*
	 *	Converte o nome para minúsculas
	 */
	for (ccp = cname, ncp = name; (c = *ncp) != '\0'; ncp++)
	{
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';

		*ccp++ = c;
	}

	*ccp = '\0';

	/*
	 *	A agenda fica no $HOME.
	 */
	if ((home = getenv ("HOME")) != NOSTR)
		strcpy (agenda, home);

	strcat (agenda, "/agenda");

	if ((fp = fopen (agenda, "r")) == NOFILE)
		return (NOSTR);

	while (fngets (buf, sizeof (buf) - 2, fp) != NOSTR)
	{
		for (cp = buf; (c = *cp) != ' ' && c != '\t'; cp++)
			/* vazio */;
		*cp = 0;

		if (streq (buf, cname))
		{
			while (c == ' ' || c == '\t')
				c = *++cp;

			return (cp);
		}
	}

	fclose (fp);

	return (NOSTR);

}	/* end get_phone_number */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (int exitval)
{
	fprintf
	(	stderr,
		"%s - disca através do \"modem\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-l <linha>] pessoa\n"
		"\t%s [-l <linha>] telefone\n",
		pgname, pgversion, pgname, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Define a <linha> de comunicações a utilizar\n"
		"\t    (default = \"/dev/znetline\")\n"
	);

	exit (exitval);

}	/* end help */
