/*
 ****************************************************************
 *								*
 *			init.c					*
 *								*
 *	Inicialização do sistema, pai de todos os processos	*
 *								*
 *	Versão	1.0.0, de 10.06.87				*
 *		4.9.0, de 30.07.06				*
 *								*
 *	Módulo: init						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/utsname.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <setjmp.h>
#include <pwd.h>
#include <init.h>
#include <signal.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições e variáveis globais				*
 ****************************************************************
 */
#define	EVER	;;
#define	elif	else if

/*
 ******	Variáveis globais ***************************************
 */
const char	pgversion[]	= "\nInit: Versão 4.9.0, de 30.07.06\n\n";

const char	desativada[]	= "\f\aLinha \"%s\" DESATIVADA\n";
const char	console[]	= "/dev/console";
const char	utmp[]		= "/var/adm/utmp";
const char	globalenv[]	= "/etc/globalenv";
entry char	shell[32];		/**** "/bin/sh" ****/
const char	login[]		= "/bin/login";
const char	menos[]		= "-sh";
const char	etc[]		= "/etc";
const char	rc[]		= "/etc/rc";
const char	data[]		= "/bin/data";

entry jmp_buf	jmp_point;			/* Estado para desvio não local */

#define	ENV_SZ	10

entry const char *env_vec[ENV_SZ + 1];		/* Para criar o ambiente */

entry char	print_child_status = ' ';	/* Imprime o estado do filho */

/*
 ****** Tabela de terminais *************************************
 */
#define NOLINETAB	(LINETAB *)NULL

typedef	struct linetab	LINETAB;

struct linetab
{
	long		l_pid;
	char		l_oldstate;
	char		l_newstate;
	int		l_slot;
	const char	*l_name;
	const char	*l_term;
	LINETAB		*l_next;
};

entry LINETAB	linetab =
{
	0, '*', '*', 0, "console", "vt100,iso", NOLINETAB		/* Para o caso de pânico */
};

entry LINETAB	*linetab_last = &linetab;

/*
 ******	Protótipos **********************************************
 */
void			readglobalenv (const SCB *scbp);
void			readinitab (void);
void			monousuario (void);
void			execrc (void);
void			multiusuario (void);
void			reproc (int, ...);
#undef	PRINITAB
#ifdef	PRINITAB
void			prinitab (void);
#endif	PRINITAB
int			lfork (LINETAB *);

/*
 ****************************************************************
 *	Ponto de Entrada do Programa				*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		status, fd;
	SCB		scb;

	/*
	 *	Abre os arquivos "padrão"
	 */
	open (console, O_RDWR);
	dup (0);
	dup (0);

	/*
	 *	Imprime o nome da versão
	 */
	printf (pgversion);

	environ = env_vec;

	/*
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("*Não consegui obter o SCB");

	/*
	 *	Verifica qual o "sh" desejado
	 */
	if (scb.y_sh_nm[0])
		strcpy (shell, scb.y_sh_nm);
	else
		strcpy (shell, "/bin/sh");

	/*
	 *	Põe a DATA	
	 */
	printf ("DATA = ");	fflush (stdout);

	if (fork () == 0)
	{
		execl (data, data, NOSTR);
		error ("$*Não consegui executar \"%s\"\n", data);
	}

	wait (&status);

	printf ("\n");

	signal (SIGINT, SIG_IGN);

	readglobalenv (&scb);

	/*
	 *	Entra nos vários modos
	 */
	settgrp (0);

	if (!scb.y_std_boot)
		monousuario ();

	execrc ();

	/*
	 *	Cria/trunca o arquivo "utmp"
	 */
	if ((fd = creat (utmp, 0644)) < 0)
		error ("*Não consegui criar \"%s\"\n", utmp);

	close (fd);

	if (chmod (utmp, 0644) < 0)	/* Em virtude de "umask" */
		error ("*Não consegui alterar o modo de \"%s\"\n", utmp);

	readinitab ();

#ifdef	PRINITAB
	prinitab ();
#endif	PRINITAB

	multiusuario ();

}	/* end init */

/*
 ****************************************************************
 *	Le a Tabela 						*
 ****************************************************************
 */
void
readinitab (void)
{
	LINETAB		*lp;
	INITAB		*initp;

	/*
	 *	Processa as entradas de "/etc/initab"
	 */
	while ((initp = getinitent ()) != NOINITAB)
	{
		if (streq (initp->it_name, "PRINT_CHILD_STATUS"))
			{ print_child_status = initp->it_state; continue; }

		for (lp = &linetab; /* abaixo */; lp = lp->l_next)
		{
			if   (lp == NOLINETAB)
			{
				if ((lp = malloc (sizeof (LINETAB))) == NOLINETAB)
					{ error ("Memória esgotada"); return; }

				lp->l_oldstate	= ' ';
				lp->l_next	= NOLINETAB;

				if ((lp->l_name = strdup (initp->it_name)) == NOSTR)
					{ error ("Memória esgotada"); return; }

				if ((lp->l_term = strdup (initp->it_term)) == NOSTR)
					{ error ("Memória esgotada"); return; }

				linetab_last->l_next = lp; linetab_last = lp;

				break;
			}
			elif (streq (lp->l_name, initp->it_name))
			{
				break;
			}
		}

		lp->l_newstate	= initp->it_state;
		lp->l_slot	= initp->it_slot;

		if (!streq (lp->l_term, initp->it_term))
		{
			free ((char *)lp->l_term);

			if ((lp->l_term = strdup (initp->it_term)) == NOSTR)
				{ error ("Memória esgotada"); return; }
		}
	}

	endinitent ();

}	/* end readinitab */

/*
 ****************************************************************
 *	Le o arquivo "globalenv"				*
 ****************************************************************
 */
void
readglobalenv (const SCB *scbp)
{
	int		i = 0;
	FILE		*fp;
	char		area[80];

	/*
	 *	A primeira variável do "ambiente" é o SISTEMA OPERACIONAL
	 */
	snprintf (area, sizeof (area), "SYS=%s", scbp->y_uts.uts_sysname);

	if ((env_vec[i] = strdup (area)) != NOSTR)
		i++;

	/*
	 *	Agora, processa o "globalenv"
	 */
	if ((fp = fopen (globalenv, "r")) == NOFILE)
		{ error ("*Não consegui ler \"%s\"\n", globalenv); return; }
	
	for (/* acima */; i < ENV_SZ && fngets (area, sizeof (area), fp) == area; /* vazio */)
	{
		if (strchr (area, '=') == NOSTR)
		{
			printf
			(	"%s: Linha inválida em \"%s\": \"%s\"\n\n",
				pgname, globalenv, area
			);
			continue;
		}

		if ((env_vec[i] = strdup (area)) == NOSTR)
			break;

		i++;
	}

	fclose (fp);

#undef	DEBUG
#ifdef	DEBUG
	for (i = 0; env_vec[i] != NOSTR; i++)
		printf ("env[%d] = \"%s\"\n", i, env_vec[i]);
#endif	DEBUG

}	/* end readglobalenv */

/*
 ****************************************************************
 *	Monousuario						*
 ****************************************************************
 */
void
monousuario (void)
{
	if (fork () == 0)
	{
		/*
		 *	Filho
		 */
		close (0);
		close (1);
		close (2);

		settgrp (0);

		open (console, O_RDWR);
		dup (0);
		dup (0);

		settgrp (1);

		if (chdir (etc) < 0)
			printf ("%s: Não consegui dar \"cd %s\"\n\n", pgname, etc);

		signal (SIGINT, SIG_DFL);

		setuid (0);		/* Superusuário */

		execl (shell, menos, NOSTR);

		printf ("%s: Não achei \"%s\"\n\n", pgname, shell);

		strcpy (shell, "/bin/sh");

		execl (shell, menos, NOSTR);

		printf ("%s: Não achei \"%s\"\n\n", pgname, shell);

		for (EVER)
			pause ();
	}

	/*
	 *	Pai
	 */
	wait ((int *)NULL);

}	/* end monousuario */

/*
 ****************************************************************
 *	Executa o "rc"						*
 ****************************************************************
 */
void
execrc (void)
{
	if (fork () == 0)
	{
		/*
		 *	Filho
		 */
		close (0);
		close (1);
		close (2);

		settgrp (0);

		open (console, 2);
		dup (0);
		dup (0);

		settgrp (1);

		printf ("\n");

		execl (shell, shell, "-v", rc, NOSTR);

		error ("*Não consegui executar \"%s\"\n", shell);

		for (EVER)
			pause ();
	}

	/*
	 *	Pai
	 */
	wait ((int *)NULL);

}	/* end execrc */

/*
 ****************************************************************
 *	Multiusuario						*
 ****************************************************************
 */
void
multiusuario (void)
{
	long		pid;
	LINETAB		*lp;
	int		status;

	/*
	 *	Cria um filho para cada linha ativa
	 */
	for (lp = &linetab; lp != NOLINETAB; lp = lp->l_next)
	{
		if (lp->l_newstate != '*')
			continue;

		if ((lp->l_pid = lfork (lp)) < 0)
			lp->l_newstate = ' ';
	}

	setjmp (jmp_point);
	
	signal (SIGTERM, reproc);

	/*
	 *	Agora, espera os filhos terminarem
	 */
	for (EVER)
	{
		/*
		 *	Espera um filho terminar
		 */
		pid = wait (&status);

		/*
		 *	Verifica se a terminação foi normal
		 */
		if (status & 0xFF && print_child_status != ' ')
		{
			int	termcode = status & 0x7F;

			printf
			(	"%s: Sinal \"%s\" na execução do processo %d\n",
				pgname, sigtostr (termcode), pid
			);

			printf ("%s: pc = %P", pgname, _fpc);

			if (termcode == SIGBUS || termcode == SIGADR || termcode == SIGSEGV)
			{
				printf (", fa = %P", _faddr);
			}
			elif (termcode == SIGILL)
			{
				printf (", inst = %04X", (int)_faddr & 0xFFFF);
			}

			printf ("\n");
	
			if (status & 0x80)
			{
				printf
				(	"%s: Processo %d: \"core dumped\"\n",
					pgname, pid
				);
			}
		}

		/*
		 *	Recria um filho
		 */
		for (lp = &linetab; lp != NOLINETAB; lp = lp->l_next)
		{
			if (lp->l_newstate != '*')
				continue;

			if (lp->l_pid == pid)
			{
				if ((lp->l_pid = lfork (lp)) < 0)
					lp->l_newstate = ' ';
				break;
			}
		}
	}

}	/* end multiusuario */

/*
 ****************************************************************
 *	Reprocessa a Tabela					*
 ****************************************************************
 */
void
reproc (int sig, ...)
{
	LINETAB		*lp;
	FILE		*fp;
	char		line_nm[32];

	/*
	 *	Relê a tabela
	 */
	for (lp = &linetab; lp != NOLINETAB; lp = lp->l_next)
		lp->l_oldstate = lp->l_newstate;

	readinitab ();

	/*
	 *	Altera os estados
	 */
	for (lp = &linetab; lp != NOLINETAB; lp = lp->l_next)
	{
		if (lp->l_newstate == lp->l_oldstate)
			continue;

		if (lp->l_newstate != '*')
		{
			kill (-lp->l_pid, SIGKILL);

			sprintf (line_nm, "/dev/%s", lp->l_name);

			fp = fopen (line_nm, "w");
			fprintf (fp, desativada, lp->l_name);
			fclose (fp);
		}
		else
		{
			if ((lp->l_pid = lfork (lp)) < 0)
				lp->l_newstate = ' ';
		}
	}

	longjmp (jmp_point, 1);

}	/* end reproc */

/*
 ****************************************************************
 *	Cria um filho						*
 ****************************************************************
 */
int
lfork (LINETAB *lp)
{
	long		pid;
	int		fd;
	char		line_nm[32];
	char		slot[10];

	/*
	 *	Verifica se o dispositivo pode ser aberto
	 */
	sprintf (line_nm, "/dev/%s", lp->l_name);

	if ((fd = open (line_nm, O_RDWR)) < 0)
		{ error ("*NÃO consegui abrir \"%s\"", line_nm); return (-1); }

	close (fd);

	/*
	 *	Cria o filho e retorna
	 */
	if ((pid = fork ()) != 0)
		return (pid);

	/*
	 *	Filho
	 */
	close (0);
	close (1);
	close (2);

	settgrp (0);

	open (line_nm, 2);
	dup (0);
	dup (0);

	settgrp (1);

	signal (SIGINT, SIG_DFL);

	sprintf (slot, "%d", lp->l_slot);

#ifdef	DEBUG
	printf
	(	"%s: Dando EXEC para login, "
		"slot = \"%s\", tty_nm = \"%s\", term = \"%s\"\n",
		pgname, slot, lp->l_name, lp->l_term
	);
#endif	DEBUG

	execl (login, "login", slot, lp->l_name, lp->l_term, NOSTR);

	for (EVER)
		pause ();

}	/* end lfork */

#ifdef	PRINITAB
/*
 ****************************************************************
 *	Imprime a Tabela					*
 ****************************************************************
 */
void
prinitab (void)
{
	LINETAB		*lp;

	printf ("\n");

	for (lp = linetab; lp->l_name[0] != '\0'; lp++)
	{
		printf
		(	"%c %c %-14s %-14s\n", lp->l_oldstate,
			lp->l_newstate, lp->l_name, lp->l_term
		);
	}

}	/* end prinitab */
#endif	PRINITAB
