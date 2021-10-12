/*
 ****************************************************************
 *								*
 *			who.c					*
 *								*
 *	Informa quem está no sistema				*
 *								*
 *	Versão	1.0.0, de 09.02.87				*
 *		4.6.0, de 16.07.04				*
 *								*
 *	Módulo: who						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utmp.h>
#include <init.h>
#include <stat.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 16.04.04";

#define	elif	else if
#define	EVER	;;

entry int	iflag;		/* Idle */
entry int	vflag;		/* Verbose */

/*
 ****** Nome dos arquivos globais *******************************
 */
const char	utmp_nm[]   = "/var/adm/utmp";

/*
 ****** Protótipos de funções ***********************************
 */
void		write_who_line (UTMP *, const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Informa quem está no sistema				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*my_tty_last_nm = NOSTR;
	FILE		*utmp_fp;
	const char	*file_nm, *local, *cp;
	UTMP		utmp;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ivH")) != EOF)
	{
		switch (opt)
		{
		    case 'i':			/* Idle */
			iflag++;
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
	 *	Pequena consistência
	 */
	argv += optind;
	argc -= optind;

	if (argc > 2)
		help ();

	if (geteuid () != 0)
		iflag = 0;

	/*
	 *	Na forma "who /usr/adm/wtmp", utiliza o arquivo dado
	 */
	if (argc == 1)
		file_nm = argv[0];
	else
		file_nm = utmp_nm;

	/*
	 *	Na forma "who am i", imprime apenas uma linha
	 */
	if (argc == 2)
	{
		if ((my_tty_last_nm = ttyname (2 /* stderr */)) == NOSTR)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o nome do terminal\n",
				pgname
			);
			exit (1);
		}

		if ((cp = strrchr (my_tty_last_nm, '/')) != NOSTR)
			my_tty_last_nm = cp + 1;
	}

	/*
	 *	Abre o arquivo do formato UTMP
	 */
	if ((utmp_fp = fopen (file_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Processa o arquivo do formato UTMP
	 */
	while (fread (&utmp, sizeof (UTMP), 1, utmp_fp) == 1)
	{
		const INITAB	*initp;

		if (utmp.ut_name[0] == '\0' && argc == 0)
			continue;

		if ((initp = getinitnam ((char *)utmp.ut_line)) != NOINITAB)
			local = initp->it_local;
		else
			local = "???";

		if (argc == 2)		/* "who am I" */
		{
			if (strcmp ((char *)utmp.ut_line, my_tty_last_nm))
				continue;

			write_who_line (&utmp, local, argc);
			exit (0);
		}

		write_who_line (&utmp, local, argc);
	}

	return (0);

}	/* end who */

/*
 ****************************************************************
 *	Imprime Uma linha					*
 ****************************************************************
 */
void
write_who_line (UTMP *up, const char *local, int argc)
{
	const char	*cp;
	time_t		idle_time;
	char		line_nm[32];
	STAT		s;

	/*
	 *	Põe o usuário e a linha
	 */
	printf ("%-16.16s %-8.8s", up->ut_name, up->ut_line);

	/*
	 *	Põe o tempo de "login"
	 */
	cp = btime (&up->ut_time);

	printf (" %-5.5s %-10.10s", cp, cp + 9);

	/*
	 *	Os dados restantes não são fornecidos para "/usr/adm/wtmp"
	 */
	if (argc == 1)
		{ printf ("\n"); return; }

	/*
	 *	Tenta obter o estado da linha
	 */
	strcpy (line_nm, "/dev/");
	strcat (line_nm, (char *)up->ut_line);

	if (stat (line_nm, &s) < 0)
		s.st_mode = 0;

	/*
	 *	Estado (aceita mensagens?)
	 */
	if   (s.st_mode == 0)
		printf ("  ?");
	elif ((s.st_mode & (S_IWGRP|S_IWOTH)) == (S_IWGRP|S_IWOTH))
		printf ("  +");
	else
		printf ("  -");

	/*
	 *	Põe o "idle time", se for o caso
	 */
	if (iflag)
	{
		if (s.st_mode)
			idle_time = (time ((time_t *)NULL) - s.st_atime) / 60;
		else
			idle_time = -1;

		if   (idle_time > 24 * 60)
			printf ("   OLD ");
		elif (idle_time > 0)
			printf ("  %2d:%02d", idle_time / 60,  idle_time % 60); 
		elif (idle_time == 0)
			printf ("    .  ");
		else	/*  < 0  */
			printf ("    ?  ");
	}

	/*
	 *	Local do terminal
	 */
	if (up->ut_node[0] != 0)
		printf ("  (%s)\n", up->ut_node);
	else
		printf ("  %s\n", local);

}	/* end write_who_line */

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
		"%s - informa quem está no sistema\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-i] [<arquivo>]\n"
		"\t%s am I\n"
		"\t%s am i\n",
		pgname, pgversion, pgname, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: Imprime também o tempo inativo\n"
		"\t    (esta opção pode ser usada apenas por superusuários)\n"
	);

	exit (2);

}	/* end help */
