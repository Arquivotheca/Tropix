/*
 ****************************************************************
 *								*
 *			shutdown.c				*
 *								*
 *	Reinicializa o computador após um certo tempo		*
 *								*
 *	Versão	3.0.0, de 08.09.97				*
 *		4.3.0, de 12.09.02				*
 *								*
 *	Módulo: shutdown					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <utmp.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.3.0, de 12.09.02";

const char	reboot_nm[] = "reboot";

#define STD_TIME 30			/* Tempo padrão em segundos */
entry int	TIME_INTERVAL =	10;	/* Tempo entre avisos em segundos */

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	pflag;		/* Panico */
entry int	vflag;		/* Verbose */

/*
 ****** Nome dos arquivos globais *******************************
 */
const char	utmp_nm[]   = "/var/adm/utmp";

/*
 ****** Protótipos de funções ***********************************
 */
void		send_warning (FILE *, const char *, int);
void		inform_user (const UTMP *, const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Reinicializa o computador após um certo tempo		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*str;
	int		i, pid, used_time = STD_TIME, intervals;
	FILE		*utmp_fp;
	UTSNAME		uts;
	char		reboot = 0;

	/*
	 *	Verifica se foi invocado como "reboot"
	 */
	if ((str = strrchr (argv[0], '/')) != NOSTR)
		str++;
	else
		str = argv[0];

	if (streq (str, reboot_nm))
		reboot++;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "t:pvH")) != EOF)
	{
		switch (opt)
		{
		    case 't':			/* Tempo */
			if ((used_time = strtol (optarg, &str, 0)) < 0 || *str != '\0')
			{
				fprintf
				( 	stderr,
					"%s: Tempo \"%s\" inválido\n",
					pgname, optarg
				);
				exit (1);
			}
			break;

		    case 'p':			/* Imediatamente */
			pflag++;
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

	if (argc != 0)
		help ();

	/*
	 *	Verifica o Usuário Efetivo
	 */
	if (geteuid () != 0)
	{
		fprintf
		(	stderr,
			"%s: Este comando pode ser executado apenas pelo SUPERUSUÁRIO\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Em caso de pânico, ...
	 */
	if (pflag)
	{
		if (boot () < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui executar a chamada ao sistema \"boot\" (%s)\n",
				pgname, strerror (errno)
			);
		}
		exit (1);
	}

	/*
	 *	Obtém a identificação do sistema
	 */
	if (uname (&uts) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter a identificação do sistema (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Entra em "background"
	 */
	if ((pid = fork ()) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar um novo processo (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	if (pid != 0)
		exit (0);

	signal (SIGHUP, SIG_IGN);

	/*
	 *	Se for "reboot", muda tudo um pouquinho
	 */
	TIME_INTERVAL =	2; used_time = 1;

	/*
	 *	Calcula o número de intervalos
	 */
	intervals = (used_time + TIME_INTERVAL - 1) / TIME_INTERVAL;

	/*
	 *	Envia as mensagens
	 */
	if ((utmp_fp = fopen (utmp_nm, "r")) != NOFILE)
	{
		for (EVER)
		{
			send_warning (utmp_fp, (const char *)uts.uts_nodename, intervals * TIME_INTERVAL);

			if (intervals-- <= 0)
				break;

			sleep (TIME_INTERVAL);
		}

		fclose (utmp_fp);
	}

	/*
	 *	Atualiza os blocos dos discos
	 */
	for (i = 2; i > 0; i--)
	{
		if (sync () < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui executar a chamada ao sistema \"sync\" (%s)\n",
				pgname, strerror (errno)
			);
			exit (1);
		}
	}

	/*
	 *	Epílogo FATAL ...
	 */
#undef	DEBUG
#ifdef	DEBUG
	fprintf (stderr, "PUM ...\n");
#else
	sleep (1);

	if (boot () < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui executar a chamada ao sistema \"boot\" (%s)\n",
			pgname, strerror (errno)
		);
	}
#endif	DEBUG

	return (1);

}	/* end shutdown */

/*
 ****************************************************************
 *	Envia a mensagem para todos os usuários			*
 ****************************************************************
 */
void
send_warning (FILE *utmp_fp, const char *nodename, int rest_time)
{
	UTMP		utmp;

	rewind (utmp_fp);

	while (fread (&utmp, sizeof (UTMP), 1, utmp_fp) == 1)
	{
		if (utmp.ut_name[0] == '\0')
			continue;

		inform_user (&utmp, nodename, rest_time);

	}	/* end for (EVER) */

}	/* end send_warning */

/*
 ****************************************************************
 *	Envia a mensagem para uma tela				*
 ****************************************************************
 */
void
inform_user (const UTMP *u, const char *nodename, int rest_time)
{
	FILE		*dst_fp;
	char		tty_nm[32];

	/*
	 *	Prepara o nome e abre o terminal
	 */
	strcpy (tty_nm, "/dev/");
	strcat (tty_nm, (char *)u->ut_line);

	if ((dst_fp = fopen (tty_nm, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\", \"%s\" (%s)\n",
			pgname, tty_nm, u->ut_name, strerror (errno)
		);

		return;
	}

	/*
	 *	Escreve a mensagem
	 */
	fprintf
	(	dst_fp,
		"\n\r\aCOMPUTADOR \"%s\" SAINDO DO AR ",
		nodename
	);

	if (rest_time <= 0)
		fprintf (dst_fp, "*** AGORA ***!!\n\r");
	else
		fprintf (dst_fp, "em %d segundos!!\n\r", rest_time);

   	fflush (dst_fp);

	if (ferror (dst_fp))
	{
		fprintf
		(	stderr,
			"%s: Não consegui enviar a mensagem para \"%s\" (%s)\n",
			pgname, u->ut_name, strerror (errno)
		);
	}

	fclose (dst_fp);

}	/* end proc_user */

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
		"%s - reinicializa o computador após um certo tempo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-t <tempo>] [-p] [-v]\n"
		"\t%s\n",
		pgname, pgversion, pgname, reboot_nm
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-t: Espera <tempo> segundos ao invés de " XSTR (STD_TIME) "\n"
		"\t-p: Reinicializa o computador imediatamente, sem mensagens\n"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
