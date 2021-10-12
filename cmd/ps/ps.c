/*
 ****************************************************************
 *								*
 *			ps.c					*
 *								*
 *	Imprime informações sobre os processos			*
 *								*
 *	Versão	1.0.0, de 01.07.86				*
 *		4.6.0, de 06.08.04				*
 *								*
 *	Módulo: ps						*
 *		Utilitário especial				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/region.h>
#include <sys/syscall.h>

#include <sys/signal.h>
#include <sys/uerror.h>
#include <sys/uproc.h>
#include <sys/uarg.h>
#include <sys/mmu.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 06.08.04";

entry int	exit_code = 0;	/* Código de retorno */

entry int	lflag;		/* Lista longa/super/sizes de informações */
entry int	xflag;		/* Lista processos mesmo sem TTY associado */
entry int	kflag;		/* Mata processos */
entry int	aflag;		/* Todos os processos */
entry int	vflag;		/* Verbose */
entry int	dflag;		/* Debug */

/*
 ******	Macro-definições ****************************************
 */
#define ADDRTOKB(x)	(((unsigned)(x) - (unsigned)SYS_ADDR) >> KBSHIFT)

#define	NPID	20

#define	SSEG	8192		/* Tamanho do trecho mapeado da pilha */

/*
 ******	Tabela de terminais *************************************
 */
const char	notty[] = "   "; /* Símbolo de "sem tty" */

typedef struct
{
	char	t_ttyid[16];	/* Nome da linha */
	dev_t	t_dev;		/* Dispositivo */

}	TTYTB;

extern const TTYTB	ttytb[];

/*
 ****** Protótipos de funções ***********************************
 */
const UPROC	*setuser (const UPROC *, UARG *);
void 		print_process (const UPROC *, const char *, int hz, const char *, const UARG *);
const char	*envuser (const UARG *uap);
const char 	*get_tty_nm (const UPROC *up);
void		help (void);

/*
 ****************************************************************
 *	Imprime informações sobre os processos			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	long		ps_pid, pid;
	int		ps_uid;
	const TTYTB	*dp;
	const char	*str;
	const UPROC	*up;
	const UPROCV	*uvp, *procv, *last_procv, *end_procv;
	int		procv_delta;
	const char	*ps_user_nm, *proc_user_nm;
	const char	*tty_nm, *tty_str = NOSTR;
	int		sigkill = SIGTERM;
	int		pid_cnt = 0;
	UARG		uarg;
	long		pid_vec[NPID];
	SCB		scb;

	/*
	 *	Verifica o usuário efetivo
	 */
	if (geteuid () != 0)
		error ("$Não sou o SUPERUSUÁRIO!");

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "axlLkt:vdMH")) != EOF)
	{
		switch (opt)
		{
		    case 'a': 			/* Todos os processos */
			aflag++;
			break;

		    case 'x' : 			/* Mesmo sem TTY associado */
			xflag++;
			aflag++;
			break;

		    case 'l': 			/* Lista longa de informações */
			lflag++;
			break;

		    case 'L': 			/* Lista super-longa */
			lflag += 2;
			break;

		    case 'k': 			/* Mata processos */
			kflag++;
			break;

		    case 't':			/* Terminal dado */
			tty_str = optarg;
			aflag++;

			if   (*tty_str == '?')
				{ xflag++; tty_str = notty; }
			elif (strncmp (tty_str, "tty", 3) == 0)
				{ tty_str += 3; }

			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd': 			/* Debug */
			dflag++;
			break;

		    case 'M':			/* Stick in memory */
			exit (0);

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	Guarda os números dos processos a listar
	 */
	for (argv += optind; *argv; argv++)
	{
		xflag++; aflag++;

		if ((pid = strtol (*argv, &str, 0)) < 0 || *str != '\0')
		{
			error ("<pid> de processo inválido: \"%s\"", *argv);
			continue;
		}

		if (pid_cnt < NPID)
			pid_vec[pid_cnt++] = pid;
		else
			error ("Número excessivo de <pid>s de processos");

	}	/* end for */

	/*
	 *	Prepara o terminal para consultas
	 */
	if (kflag && (askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
		error ("$*Não consegui abrir \"/dev/tty\"\n");

	/*
	 *	Prepara alguns parâmetros universais
	 */
	ps_user_nm = getenv ("USER");
	ps_uid	   = getuid ();
	ps_pid     = getpid ();

	if (kflag > 1)
		sigkill = SIGKILL;

	if (lflag > 5)
		lflag = 5;

	/*
	 *	Processa a tabela de terminais
	 */
	if (dflag) for (dp = ttytb; dp->t_ttyid[0] != 0; dp++)
		printf ("(%02d %02d) \"%s\"\n", MAJOR (dp->t_dev), MINOR (dp->t_dev), dp->t_ttyid);

	if (tty_str != NOSTR && tty_str[0] != ' ')
	{
		for (dp = ttytb; /* abaixo */; dp++)
		{
			if (dp->t_ttyid[0] == 0)
				error ("$Nome de terminal desconhecido: \"%s\"", tty_str);

			if (strncmp (dp->t_ttyid, tty_str, 3) == 0)
				break;
		}
	}

	/*
	 *	Obtem o "System Control Block"
	 */
	if ((int)getscb (&scb) == -1)
		error ("$*Não consegui obter o SCB");

	/*
	 *	Mapeia a Tabela de Processos
	 */
	if ((int)(procv = uvp = phys (scb.y_uproc, scb.y_nproc * sizeof (UPROCV), O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear a tabela de processos");

	procv_delta = (int)uvp - (int)scb.y_uproc;

	last_procv  = (UPROCV *)((int)scb.y_lastuproc + procv_delta);
	end_procv   = (UPROCV *)((int)scb.y_enduproc  + procv_delta);

	/*
	 *	Coloca o Cabecalho
	 */
	if   (lflag == 1)
		printf (" F S UID            US  ");
	elif (lflag == 2)
		printf (" F S RUID           ");
	elif (lflag >= 3)
		printf (" F S UID            ");

	printf ("TTY   PID ");

	if   (lflag == 1)
		printf (" PPID T  PRI  NI UTIME STIME");
	elif (lflag == 2)
		printf ("   ADDR MSK PGRP TGRP RCs");
#if (0)	/*******************************************************/
		printf ("   ADDR SIZE MSK PGRP TGRP RCs");
#endif	/*******************************************************/
	elif (lflag == 3)
		printf ("   ADDR TSIZE DSIZE SSIZE KFILE");
	elif (lflag == 4)
		printf (" ALARM  SIGALRM   ");
	elif (lflag == 5)
		printf (" KERNEL STACK  ");

	printf (" CMD\n");

	/*
	 *	Malha Principal pela Tabela de Processos
	 */
	for (/* acima */; uvp < end_procv; uvp++)
	{ 
		/*
		 *	Mapeia UPROC & PILHA
		 */
		if (uvp->u_uproc == NOUPROC)
			continue;

		if (uvp > last_procv)
		{
			printf
			(	"*** CUIDADO: entrada após \"last_proc\" (%d :: %d)\n",
				uvp - procv, last_procv - procv
			);
		}

		if ((up = setuser (uvp->u_uproc, &uarg)) == NOUPROC)
			continue;

		/*
		 *	Examina o estado
		 */
		if (up->u_state == SNULL)
			continue;

		if (up->u_tgrp == 0 && !xflag)
			continue;

		if (up->u_euid != ps_uid && up->u_ruid != ps_uid)
		{
			if (!aflag || ps_uid != 0)
				continue;
		}

		/*
		 *	Verifica se foram dados <pid>s
		 */
		if (pid_cnt > 0)
		{
			const long	*pid_ptr, *end_pid_vec;

			pid_ptr = pid_vec; end_pid_vec = pid_ptr + pid_cnt;

			for (/* acima */; pid_ptr < end_pid_vec; pid_ptr++)
			{
				if (*pid_ptr == up->u_pid)
					goto found;
			}

			continue;
		}

		/*
		 *	Analisa o "aflag"
		 */
	    found:
		proc_user_nm = envuser (&uarg);

		if (!aflag && ps_uid == 0)
		{
			if (proc_user_nm == NOSTR || ps_user_nm == NOSTR)
				continue;

			if (!streq (proc_user_nm, ps_user_nm))
				continue;
		}

		/*
		 *	Verifica se foi dado o terminal
		 */
		tty_nm = get_tty_nm (up);

		if (tty_str != NOSTR && strncmp (tty_str, tty_nm, 3) )
			continue;

		/*
		 *	Imprime o Processo
		 */
		print_process (up, tty_nm, scb.y_hz, proc_user_nm, &uarg); printf ("\n");

		/*
		 *	Verifica se deve enviar um sinal para o processo
		 */
		if (kflag && up->u_pid != ps_pid && up->u_state != SZOMBIE && up->u_pid > scb.y_initpid)
		{
			fprintf (stderr, "Kill? (n): ");

			if (askyesno () > 0 && kill (up->u_pid, sigkill) < 0)
				error ("*Erro no \"kill\"");
		}

	}	/* end malha pela tabela de processos */

	return (exit_code);

}	/* end ps */

/*
 ****************************************************************
 *	Prepara Ponteiros para UPROC e PILHA			*
 ****************************************************************
 */
const UPROC *
setuser (UPROC const *kup, UARG *uap)
{
	const UARG		*uapf;
	int			stack_delta;
	const UPROC		*up;
	void			*phys_addr;
	char			*sp;
	static const UPROC	*mapup;
	static const char	*mapsp;
	REGIONG			*rgp;

	/*
	 *	Mapeia a UPROC
	 */
	if (mapup)
		phys (mapup, 0, 0);

	if (mapsp)
		phys (mapsp, 0, 0);

	if ((int)(mapup = up = phys (kup, UPROCSZ, O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a UPROC"); mapup = NOUPROC; return (NOUPROC); }

	/*
	 *	Verifica se o processo possui PILHA
	 */
	if (up->u_flags & SSYS)
		{ memsetl (uap, 0, sizeof (UARG) / sizeof (long)); return (up); }

	/*
	 *	Mapeia e desmapeia a REGIONG da PILHA
	 */
	if ((int)(rgp = phys (up->u_stack.rgl_regiong, sizeof (REGIONG), O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a REGIONG"); return (NOUPROC); }

	phys_addr = (void *)PGTOBY (rgp->rgg_paddr + rgp->rgg_size);

	phys (rgp, 0, 0);

	/*
	 *	Mapeia a PILHA do processo
	 */
	if ((int)(mapsp = sp = phys (phys_addr - SSEG, SSEG, O_KERNEL|O_RDONLY)) == -1)
		{ error ("*Não consegui mapear a PILHA"); mapsp = NOSTR; return (NOUPROC); }

	/*
	 *	Prepara os diversos ponteiros da PILHA
	 */
	stack_delta = (int)(USER_STACK_PGA << PGSHIFT) - (int)sp - SSEG;

	uapf = (UARG *)((USER_STACK_PGA << PGSHIFT) - sizeof (UARG) - stack_delta);

	uap->g_envp = uapf->g_envp - stack_delta;
	uap->g_envc = uapf->g_envc;

	if (uap->g_envp < sp || uap->g_envp >= sp + SSEG)
		{ uap->g_envp = NOSTR; uap->g_envc = 0; }

	uap->g_argp = uapf->g_argp - stack_delta;
	uap->g_argc = uapf->g_argc;

	if (uap->g_argp < sp || uap->g_argp >= sp + SSEG)
		{ uap->g_argp = NOSTR; uap->g_argc = 0; }

	return (up);

}	/* end setuser */

/*
 ****************************************************************
 *	Imprime o Processo					*
 ****************************************************************
 */
void
print_process (const UPROC *up, const char *tty_nm, int hz, const char *proc_user_nm, const UARG *uap)
{
	KFILE		* const *fpp;
	time_t		tm;
	const char	*cp;
	int		argc, c;

	if (lflag > 0)
	{
		printf ("%02X ", up->u_flags & 0xFF);
		printf ("%c ", "0rRsS0Z"[up->u_state]);

		if   (lflag != 2 && proc_user_nm != NOSTR)
			cp = proc_user_nm;
		elif ((cp = pwcache (up->u_euid)) == NOSTR)
			cp = "???";

		printf ("%-14.14s ", cp);
	}

	if (lflag == 1)
	{
		if (up != (UPROC *)NULL)
			printf (up->u_euid ? (up->u_egid ? "    " : "sys ") : "SU  ");
		else
			printf ("    ");
	}

	printf ("%-3.3s ", tty_nm);
	printf ("%5u ", up->u_pid);

	/*
	 *	Imprime a parte variável
	 */
	if   (lflag == 1)
	{
		const UPROC	*par_up;

		/* Imprime: PID do pai, Text, Prioridade, Nice e tempos */

		par_up = phys (up->u_parent, sizeof (UPROC), O_KERNEL|O_RDONLY);

		if ((int)(par_up) != -1)
			{ printf ("%5u ", par_up->u_pid); phys (par_up, 0, 0); }
		else
			printf ("???   ");

		printf (up->u_inode ? "n " : "  ");
		printf ("%4u ", up->u_pri);
		printf ("%3d ", up->u_nice);

		if (up != NOUPROC)
		{
			tm = (up->u_utime + (hz >> 1)) / hz;
			printf ("%2d:%02d ", tm / 60, tm % 60);
			tm = (up->u_stime + (hz >> 1)) / hz;
			printf ("%2d:%02d ", tm / 60, tm % 60);
		}
		else
		{
			printf ("              ");
		}
	}
	elif (lflag == 2)
	{
		/* Imprime: Endereco, Mascara de CPU's, Lider do Grupo e de Terminal */

		printf ("%7d ", ADDRTOKB (up->u_myself));

		printf
		(	" %02X %4d %4d %1d %1d ",
#if (0)	/*******************************************************/
		(	"%4d  %02X %4d %4d %1d %1d ",
			PGTOKB (up->u_size),
#endif	/*******************************************************/
			up->u_mask,
			up->u_pgrp,
			up->u_tgrp,
			up->u_syncn,
			up->u_syncmax
		);
	}
	elif (lflag == 3)
	{
		/* Imprime: Tamanhos e número de KFILEs ocupados */

		printf ("%7d ", ADDRTOKB (up->u_myself));

		if (up != (UPROC *)NULL)
		{
			int		cnt;

			printf ("%5u ", PGTOKB (up->u_tsize));
			printf ("%5u ", PGTOKB (up->u_dsize));
			printf ("%5u ", PGTOKB (up->u_ssize));

			for (cnt = 0, fpp = up->u_ofile; fpp < &up->u_ofile[NUFILE]; fpp++)
			{
				if (*fpp != NOKFILE)
					cnt++;
			}

			printf ("%5d ", cnt);
		}
		else
		{
			printf ("                        ");
		}
	}
	elif (lflag == 4)
	{
		/* Imprime: Estado do ALARM */

		printf ("%6d  ", up->u_alarm);

		switch ((int)up->u_signal[SIGALRM])
		{
		    case SIG_DFL:
			printf ("   (D)     ");
			break;

		    case SIG_IGN:
			printf ("   (I)     ");
			break;

		    default:
			printf ("%P  ", up->u_signal[SIGALRM]);
			break;
		}
	}
	elif (lflag == 5)
	{
		/* Imprime: Uso da PILHA do KERNEL */

		int		n;

		for (cp = (char *)up + sizeof (UPROC); cp < (char *)up + UPROCSZ; cp++)
		{
			if (*cp != 0)
				break;
		}

		n = (char *)up + UPROCSZ - cp;

		printf ("  %5d (%2d %%)  ", n, (n * 100) / (UPROCSZ - sizeof (UPROC)));
	}

	if (lflag == 0)
		printf (" ");

	/*
	 ******	Imprime os Argumentos ***********************************
	 *
	 *	Trata o Swapper, dispatchers e zombies
	 */
	if (up->u_flags & SSYS)
	{
		if (up->u_pid == 0)
			printf ("<Swapper/Pager>");
		else
			printf ("<Dispatcher CPU %d>", up->u_pid);
		return;
	}

	if (up->u_state == SZOMBIE)
		{ printf ("<defunct>"); return; }

	/*
	 *	Obtem o nome do Programa Diretamente da UPROC
	 */
	if ((argc = uap->g_argc) == 0 || lflag == 2)
		{ printf (up->u_pgname); return; }

	/*
	 *	Imprime os Argumentos
	 */
	for (cp = uap->g_argp; argc > 0; argc--)
	{
		while (c = *cp++)
			putchar (c);

		putc (' ', stdout);

	}	/* end loop */

}	/* end print_process */

/*
 ****************************************************************
 *	Obtem o nome do usuário do processo	 		*
 ****************************************************************
 */
const char *
envuser (const UARG *uap)
{
	int		i;
	const char	*cp;

	/*
	 *	Varre o ambiente, procurando o nome do processo
	 */
	if ((cp = uap->g_envp) == NOSTR)
		return (NOSTR);

	for (i = uap->g_envc; i > 0; i--)
	{
		if (strncmp (cp, "USER=", 5) == 0)
			return (cp + 5);

		while (*cp++ != '\0')
			/* vazio */;
	}

	return (NOSTR);

}	/* end envuser */

/*
 ****************************************************************
 *	Obtém o nome do TTY, dado o dispositivo			*
 ****************************************************************
 */
const char *
get_tty_nm (const UPROC *up)
{
	const TTYTB	*dp;

	if (up == NOUPROC)
		return (notty);

	if (up->u_tty == NOTTY)
		return (notty);

	for (dp = ttytb; dp->t_ttyid[0] != 0; dp++)
	{
		if (dp->t_dev == up->u_ttydev)
			return (dp->t_ttyid);
	}

	return (notty);

}	/* end get_tty_nm */

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
		"%s - imprime informações sobre processos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-axlk] [-t <tty>] [<pid> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Lista simples de processos do usuário\n"
		"\t-a: Todos processos associados a um terminal\n"
		"\t-x: Todos processos\n"
		"\t-l: Lista longa de informações\n"
		"\t    (se dado duas vezes, lista longa com outras informações,\n"
		"\t    se dado três vezes, lista longa com tamanhos)\n"
		"\t-k: Envia o sinal SIGTERM interativamente\n"
		"\t    (se dado duas vezes, envia o sinal SIGKILL)\n"
		"\t-t: Lista apenas de processos associados ao terminal <tty>\n"
	);
	fprintf
	(	stderr,
		"\n<pid>:"
		"\tLista apenas informações sobre "
		"os processos cujos <pid>s são dados\n"
	);

	exit (2);

}	/* end help */
