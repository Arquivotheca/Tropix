/*
 ****************************************************************
 *								*
 *			mkttytb.c				*
 *								*
 *	Gera a tabela de terminais				*
 *								*
 *	Versão	1.1.0, de 26.06.86				*
 *		4.6.0, de 06.08.04				*
 *								*
 *	Módulo: ps						*
 *		Utilitários Especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <mkasmtb.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.6.0, de 06.08.04";

#define	TTYSZ	100

#define	elif	else if

entry int	vflag;		/* Verbose */

/*
 ****** Tabela de terminais *************************************
 */
typedef struct
{
	char	t_ttyid[16];	/* Nome da linha */
	dev_t	t_dev;		/* Dispositivo */

}	TTYTB;

entry TTYTB	ttytb[TTYSZ];

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);
int		getdev (void);
int		tty_nm_cmp (const void *, const void *);

/*
 ****************************************************************
 *	Gera a tabela de terminais				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	char		*cwd;
	int		ntty, r;

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

	/*
	 *	Gera a tabela
	 */
	cwd = getcwd (NOSTR, 256);

	ntty = getdev ();

	qsort (ttytb, ntty, sizeof (TTYTB), tty_nm_cmp);

	if (chdir (cwd) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui voltar para o diretório \"%s\" (%s)\n",
			cwd, strerror (errno)
		);
		exit (1);
	}

	r = mkasmtb
	(	"ttytb.s",
		"ttytb",
		ttytb,
		(ntty + 1) * sizeof (TTYTB),
		RO,
		sizeof (TTYTB),
		(OBJENT *)NULL
	);

	if (r < 0)
	{
		fprintf (stderr, "Erro na execução de \"mkasmtb\"\n");
		exit (1);
	}

	return (0);

}	/* end mkttytb */

/*
 ****************************************************************
 *	Prepara uma Tabela de TTYs				*
 ****************************************************************
 */
int
getdev (void)
{
	int		ntty = 0;
	DIR		*dir_fp;
	char		*cp;
	int		n_con = 0, n_tty = 0, n_pty = 0, n_video = 0;
	STAT		s;
	DIRENT		*dp;

	if (chdir ("/dev") < 0)
	{
		fprintf
		(	stderr,
			"Não consegui ir para diretório \"/dev\" (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	if ((dir_fp = opendir (".")) == NODIR)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"/dev\" (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Lê o diretório
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (streq (dp->d_name, "tty"))
			continue;

		if (stat (dp->d_name, &s) < 0)
			continue;

		if (!S_ISCHR (s.st_mode))
			continue;

		/*
		 *	Já prepara supondo 3 caracteres significativos
		 */
		cp = dp->d_name;

		if   (strncmp (dp->d_name, "tty", 3) == 0)
		{
			cp += 3; n_tty++;
		}
		elif (strncmp (dp->d_name, "ptyc", 4) == 0)
		{
			/* Transforma "ptyc00" em "p00" (3 caracteres) */

			n_pty++;
			cp[1] = cp[4]; cp[2] = cp[5]; cp[3] = '\0';
		}
		elif (strncmp (dp->d_name, "co", 2) == 0)
		{
			n_con++;
		}
		elif (strncmp (dp->d_name, "video", 5) == 0)
		{
			/* Transforma "video0" em "vd0" (3 caracteres) */

			n_video++;
			cp[1] = 'd'; cp[2] = cp[5]; cp[3] = '\0';
		}
		else
		{
			continue;
		}

		strncpy (ttytb[ntty].t_ttyid, cp, sizeof (ttytb[ntty].t_ttyid) - 1);

		ttytb[ntty++].t_dev = s.st_rdev;

	}	/* end while (fread) */

	closedir (dir_fp);

	if (!vflag)
		return (ntty);

	/*
	 *	Imprime a estatística
	 */
	printf
	(	"%s: Número total = %d"
		" (con = %d, tty = %d, pty = %d, video = %d)\n",
		pgname, ntty, n_con, n_tty, n_pty, n_video
	);

	return (ntty);

}	/* end getdev */

/*
 ****************************************************************
 *	Compara nomes de terminais				*
 ****************************************************************
 */
int
tty_nm_cmp (const void *v_1, const void *v_2) 
{
	const TTYTB *tty_1 = v_1, *tty_2 = v_2;

	return (strcmp (tty_1->t_ttyid, tty_2->t_ttyid));

}	/* end tty_nm_cmp */

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
		"%s - gera uma tabela de terminais para PS\n"
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
