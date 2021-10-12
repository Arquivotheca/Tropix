/*
 ****************************************************************
 *								*
 *			pallwd.c				*
 *								*
 *	Imprime todos os diretório correntes			*
 *								*
 *	Versão	1.0.0, de 03.02.87				*
 *		4.6.0, de 07.08.04				*
 *								*
 *	Módulo: pallwd						*
 *		Utilitários Trancendentais			*
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

#include <sys/inode.h>
#include <sys/region.h>
#include <sys/signal.h>
#include <sys/uproc.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <fcntl.h>
#include <utmp.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.6.0, de 07.08.04";

#define	NUTMP	50
#undef	DEBUG

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	aflag;		/* Não ignora ninguen */
entry int	vflag;		/* Verbose */

/*
 ******	Tipos ***************************************************
 */
typedef struct	 			/* Célula de um diretório */
{
	INODE		*w_inop;	/* Os ponteiros para inodes */
	const char	*w_pdir;	/* Ponteiros para os nomes dos dir. */

}	DIRCELL;

typedef struct 				/* Tabela dos usuários */
{
	char		w_name[16];	/* Login name */
	char		w_ttyn[16];	/* Nome do terminal */
	dev_t		w_ttydev;	/* Device do terminal */

	char		w_suser;	/* Indica que é super-user */
	char		w_gsys;		/* Indica que está no grupo sys */
	char		w_nice;		/* Indica que tem nice < 0 */

	DIRCELL		w[50];		/* Os diretórios */

}	WHOTB;

/*
 ******	Variáveis globais ***************************************
 */
#undef	IGNORE_DEV
#ifdef	IGNORE_DEV
const char	*igfilelist[] = /* lista de ignore files */
{
   /***	"/dev/root", ***/
	NOSTR
};

entry dev_t	ignoredev[20];	/* Dispositivos a serem ignorados */
#endif	IGNORE_DEV

entry SCB	scb;

entry WHOTB	whotb[NUTMP];

entry WHOTB	*othertty;	/* Entrada para outros tty's */

#define	lastwho	othertty	/* Outros tty's == ultima normal */

entry WHOTB	*notty;		/* Entrada para processos sem tty */

entry INODE 	*imem;		/* Para mapear a tabela de inodes */

entry int	nwho;		/* Nr. de usuários */

/*
 ****** Protótipos de funções ***********************************
 */
void		read_utmp_file (void);
void		getignore (int, const char *[]);
void		getuser (void);
void		getdir (const UPROC *up);
void		print_cwd_table (void);
int		ignore (dev_t);
void		printcent (int);
void		help (void);

/*
 ****************************************************************
 *	Imprime todos os diretório correntes			*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	long		my_pid;
	int		procv_delta;
	const UPROCV	*uvp, *last_procv;
	const UPROC	*up = NOUPROC;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "avH")) != EOF)
	{
		switch (opt)
		{
		    case 'a':			/* All */
			aflag++;
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

	my_pid = getpid ();

	/*
	 *	x
	 */
	read_utmp_file ();

#ifdef	IGNORE_DEV
	getignore (argc, argv);
#endif	IGNORE_DEV

	/*
	 *	Obtem o SCB
	 */
	if ((int)getscb (&scb) == -1)	
		error ("$*Não consegui obter o SCB");

	/*
	 *	Prepara o acesso à tabela de processos
	 */
	if ((int)(uvp = phys (scb.y_uproc, sizeof (UPROCV) * scb.y_nproc, O_KERNEL|O_RDONLY)) == -1)
		error ("$*Não consegui mapear a tabela de processos");

	procv_delta = (int)uvp - (int)scb.y_uproc;
	last_procv  = (UPROCV *)((int)scb.y_lastuproc + procv_delta);

	/*
	 *	Malha Principal pela Tabela de Processos
	 */
	for (/* acima */; uvp < last_procv; uvp++)
	{ 
		/*
		 *	Mapeia a UPROC
		 */
		if (uvp->u_uproc == NOUPROC)
			continue;

		if (up != NOUPROC)
			phys (up, 0, 0);

		if ((int)(up = phys (uvp->u_uproc, UPROCSZ, O_KERNEL|O_RDONLY)) == -1)
			error ("$*Não consegui mapear a UPROC");

		/*
		 *	Examina o estado
		 */
		switch (up->u_state)
		{
		    case SNULL:
		    case SZOMBIE:
		    case SSWAPRDY:
		    case SSWAPSLP:
			continue;

		    default:
			break;
		}

		if (up->u_pid == my_pid)
			continue;

		/*
		 *	x
		 */
		getdir (up);

		phys (imem, 0, 0);

	}	/* fim da malha de processos */

	/*
	 *	Finalmente, imprime a tabela
	 */
	print_cwd_table ();

	return (exit_code);

}	/* end pallwd */

/*
 ****************************************************************
 *	Le a Tabela de Usuários					*
 ****************************************************************
 */
void
read_utmp_file (void)
{
	int		fd;
	WHOTB		*wp;
	UTMP		utmp;
	STAT		s;

	if ((fd = open ("/var/adm/utmp", 0)) < 0)
		error ("$*Não consegui abrir o arquivo UTMP");

	if (chdir ("/dev") < 0)
		error ("$*Não consegui alterar o diretório corrente para \"/dev\"");

	wp = whotb;

	while (read (fd, &utmp, sizeof (utmp)) == sizeof (utmp))
	{
		if (utmp.ut_name[0] == '\0')
			continue;

		nwho++;

		strcpy (wp->w_ttyn, (char *)utmp.ut_line);
		strcpy (wp->w_name, (char *)utmp.ut_name);

		if (stat ((char *)wp->w_ttyn, &s) < 0)
			error ("$*Não consegui obter o estado de \"/dev/%s\"", wp->w_ttyn);

		wp->w_ttydev = s.st_rdev;

#ifdef	DEBUG
	 	printf
		(	"%-14.14s %-7.7s %2d %2d\n",
			wp->w_name, wp->w_ttyn,
			MAJOR (wp->w_ttydev), MINOR (wp->w_ttydev)
		);
#endif	DEBUG
		wp++;
	}

	othertty = wp++; strcpy (othertty->w_name, "<other tty>");
	notty	 = wp;   strcpy (notty->w_name,    "<notty>");

	close (fd);

}	/* end read_utmp_file */

#ifdef	IGNORE_DEV
/*
 ****************************************************************
 *	Examina os dispositivos dos arquivos a ignorar		*
 ****************************************************************
 */
void
getignore (int argc, const char *argv[])
{
	int		i;
	const char	**igp;
	STAT		s;

	if (aflag)
		return;

	if (argc > 0)
		igp = argv;
	else
		igp = igfilelist;

	for (i = 0; *igp != NULL; igp++)
	{
		if (stat (*igp, &s) < 0)
			error ("$*Não consegui obter o estado de \"%s\"", *igp);

		ignoredev[i] = s.st_rdev;

#ifdef	DEBUG
		printf ("%s ", *igp);
		printf
		(	"%2d %2d\n",
			MAJOR (ignoredev[i]),
			MINOR (ignoredev[i])
		);
#endif	DEBUG
		i++;
	}

}	/* end getignore */

/*
 ****************************************************************
 *	Retorna 1 se deve ignorar				*
 ****************************************************************
 */
int
ignore (dev_t dev)
{
	const dev_t	*p;

	if (aflag)
		return (0);

	for (p = ignoredev; *p; p++)
	{
		if (*p == dev)
			return (1);
	}

	return (0);

}	/* end ignore */
#endif	IGNORE_DEV

/*
 ****************************************************************
 *	Processa o Diretório Corrente				*
 ****************************************************************
 */
void
getdir (const UPROC *up)
{
	WHOTB		*wp;
	DIRCELL		*p;
	char		*wdp;
	char		cwdbuf[128];

	/*
	 *	Procura a entrada da tty dada
	 */
	lastwho->w_ttydev = up->u_ttydev;

	for (wp = whotb; wp->w_ttydev != up->u_ttydev; wp++)
		/* procura a entrada */;

	/*
	 *	Se for tty fora da tabela, usa "othertty",
	 *	Se for sem tty, usa "notty"
	 */
	if (wp == lastwho)	/* othertty == lastwho */
	{
#ifdef	DEBUG
		printf ("Processo com tty fora da tabela who\n");
		printf
		(	"MAJOR = %2d, MINOR = %2d\n",
			MAJOR (up->u_ttydev), MINOR (up->u_ttydev)
		);
#endif	DEBUG
		if (up->u_ttydev == NULL)
			wp++;
	}

	/*
	 *	verifica se não tem diretório corrente
	 */
	if (up->u_cdir == 0)
	{
#ifdef	DEBUG
		printf ("Processo sem diretório corrente:\n");
		printf
		(	"Nr. = %d * Terminal = %s * Usuário = %s\n\n",
			pmem->u_pid, wp->w_ttyn, wp->w_name
		);
#endif	DEBUG
		return;
	}

	/*
	 *	Verifica os privilégios
	 */
	if (up->u_euid == 0)		/* Puper-usuário */
		wp->w_suser++;

	if (up->u_egid == 0)		/* Grupo sys */
		wp->w_gsys++;

	if (up->u_nice > 0)		/* Nice positivo */
		wp->w_nice++;

	/*
	 *	Verifica se o inode já está
	 */
	for (p = wp->w; p->w_pdir; p++)
	{
		if (p->w_inop == up->u_cdir)
			return;
	}

	/*
	 *	le o inode do diretório
	 */
	imem = phys (up->u_cdir, sizeof (INODE), O_KERNEL|O_RDONLY);

	if ((int)imem == -1)
		error ("$*Não consegui mapear a tabela de INODEs");

#ifdef	IGNORE_DEV
	if (ignore (imem->i_dev))
		return;
#endif	IGNORE_DEV

#ifdef	DEBUG
		printf ("value of cdir = %o\n", up->u_cdir);
		printf
		(	"inumber = %6d, MAJOR = %6d, MINOR = %6d\n",
			imem->i_ino, MAJOR (imem->i_dev), MINOR (imem->i_dev)
		);
#endif	DEBUG

	/*
	 *	Coloca o diretório na entrada
	 */
	if (chdirip (up->u_cdir) < 0)
		error ("$*Não consegui trocar o diretório corrente");

	if ((wdp = getcwd (cwdbuf, sizeof (cwdbuf))) == NOSTR)
		error ("$*Não consegui obter o diretório corrente");

#if (0)	/*******************************************************/
		perror ("Não conseguiu o nome do diretório\npallwd");
		printf ("inumber = %6d, MAJOR = %6d, MINOR = %6d\n",
		imem->i_ino, MAJOR (imem->i_dev), MINOR (imem->i_dev) );
		wdp = "???";
#endif	/*******************************************************/

	for (p = wp->w; p->w_pdir; p++)
		/* procura o final */;

	p->w_inop = up->u_cdir;
	p->w_pdir = strdup (wdp);

}	/* end getdir */

/*
 ****************************************************************
 *	Imprime a Tabela Gerada					*
 ****************************************************************
 */
void
print_cwd_table (void)
{
	const WHOTB	*wp;
	const DIRCELL	*p;
	int		i;

	for (wp = whotb; wp->w_name[0]; wp++)
	{
		if
		(	!aflag && !wp->w_suser && !wp->w_gsys &&
			!wp->w_nice && !wp->w[0].w_pdir
		)
			continue;

	 	printf ("%-14.14s %-7.7s", wp->w_name, wp->w_ttyn);
		printf (wp->w_suser ? " <root>" : "       ");
		printf (wp->w_gsys  ? " <sys>"  : "      " );
		printf (wp->w_nice  ? " <n>" : "    ");

		for (p = wp->w, i = 0; p->w_pdir; p++, i++)
		{
			if (i > 0)
				printf ("\n\t\t\t\t");

			printf ("\t%s", p->w_pdir);
		}

		printf ("\n");
	}

}	/* end print_cwd_table */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime Centésimos					*
 ****************************************************************
 */
void
printcent (int arg)
{
	printf ("%d.%02d", arg / 100, arg % 100);

}	/* end printcent */
#endif	DEBUG

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
		"%s - imprime todos os diretórios correntes\n"
		"\n%s\n"
		"\nSintaxe:\n"
#ifdef	IGNORE_DEV
		"\t%s [<dev> ...]\n",
#else
		"\t%s\n",
#endif	IGNORE_DEV
		pgname, pgversion, pgname
	);

#ifdef	IGNORE_DEV
	fprintf
	(	stderr,
		"\nObs.:\tNão são considerados os dispositivos <dev>s na "
		"busca de diretórios\n\tcorrentes\n"
	);
#endif	IGNORE_DEV

	exit (2);

}	/* end help */
