/*
 ****************************************************************
 *								*
 *			textmap.c				*
 *								*
 *	Imprime uma tabela de programas reentrantes em uso	*
 *								*
 *	Versão	1.0.0, de 06.07.87				*
 *		4.6.0, de 14.08.04				*
 *								*
 *	Modulo: textmap						*
 *		Utilitário Especial				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/region.h>

#include <sys/inode.h>
#include <sys/kfile.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 14.08.04";

#define	elif	else if
#define	EVER	;;

/*
 ****** Variáveis globais ***************************************
 */
entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Variáveis da tabela *************************************
 */
typedef struct text	TEXT;

struct text
{
	dev_t	t_dev;		/* Dispositivo */
	DIRENT	t_dirent;	/* Tabela de Textos */
	TEXT	*t_next;	/* Ponteiro para o próximo */

};

#define	NOTEXT	(TEXT *)NULL

entry TEXT	*text1;		/* Ponteiro para a 1a. entrada */
entry TEXT	*text2;		/* Ponteiro para a última entrada */

/*
 ****** Lista de diretórios *************************************
 *
 *	Estas cadeias são modificadas por "strfield"
 */
entry char	*dirlist[] =
{
	"/bin",
	"/usr/bin",
	"/etc",
	"/usr/etc",
	"/tmp",
	"/lib",
	"/usr/lib",
	"/usr/lib/itnet",
	"/usr/pub/bin",
	"/usr/pub/lib",
	"/usr/xwin/bin",
	".",
	NOSTR
};

/*
 ****** Protótipos de funções ***********************************
 */
void		getdir (const char *);
const char	*gettext (ino_t, dev_t);
char		*env_var_scan (char *, const char *);
void		help (void);

/*
 ****************************************************************
 *	Imprime uma tabela de programas reentrantes em uso	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	INODE		*ip;
	SCB		*sp;
	const REGIONG	*rgp;
	char		**dirpp;
	int		i;
	char		path[512];
	SCB		scb;

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
	 *	Pequena consistência
	 */
	if (argc > 0)
		help ();

	/*
	 *	Obtem o System Control Block
	 */
	if ((int)getscb (sp = &scb) == -1)
		error ("$*Não foi possível obter o SCB");

	/*
	 *	Le os diretórios
	 */
	for (dirpp = dirlist; *dirpp != NOSTR; dirpp++)
	{
		if (vflag)
			printf ("\"%s\" => ", *dirpp);

		path[0] = '\0';
		env_var_scan (path, *dirpp);
		getdir (path);

		if (vflag)
			printf ("\"%s\"\n", path);
	}

	/*
	 *	Mapeia os INODEs
	 */
	ip = phys (sp->y_inode, sp->y_ninode * sizeof (INODE), O_KERNEL|O_READ);

	if ((int)ip == -1)
		error ("$*Não foi possível mapear os INODEs");

	printf
	(	"TEXT            SIZE   M T   COUNT  "
		"CORE  MADDR  SWAP  SADDR\n"
	);

	for (i = sp->y_ninode;  i > 0;  i--, ip++)
	{
		if ((ip->i_flags & ITEXT) == 0)
			continue;

		rgp = NOREGIONG;

		if (ip->i_xregiong != NOREGIONG)
		{
			rgp = phys (ip->i_xregiong, sizeof (REGIONG), O_KERNEL|O_READ);

			if ((int)rgp == -1)
			{
				error ("*Não foi possível mapear a tabela REGIONG");
				rgp = NOREGIONG;
			}
		}

		printf
		(	"%-14.14s %5d   %1s %1s (%2d",
			gettext (ip->i_ino, ip->i_dev),
			PGTOKB (ip->i_xsize),
			ip->i_mode  & IMETX ?  "m" : "",
			ip->i_mode  & ISVTX ?  "t" : "",
			ip->i_xcount
		);

		if (rgp != NOREGIONG)
			printf (", %2d)   ", rgp->rgg_count);
		else
			printf (",  .)   ");

		printf ("%1s ", ip->i_flags & IXCORE ? "c" : "");

		if (rgp != NOREGIONG && rgp->rgg_paddr)
			printf ("%7d    ", PGTOKB (rgp->rgg_paddr - (scb.y_SYS_ADDR >> PGSHIFT)));
		else
			printf ("      .    ");

		printf ("%1s ", ip->i_flags & IXSWAP ? "s" : "");

		if (ip->i_xdaddr)
			printf ("%7d\n", BLTOKB (ip->i_xdaddr - SWAP_OFFSET));
		else
			printf ("      .\n");

		if (rgp != NOREGIONG)
			phys (rgp, 0, 0);
	}

	return (exit_code);

}	/* end textmap */

/*
 ****************************************************************
 *	Le um diretório 					*
 ****************************************************************
 */
void
getdir (const char *path)
{
	DIR		*dir_fd;
	const DIRENT	*dp;
	STAT		s;
	TEXT		*tp;

	/*
	 *	Abre o diretório
	 */
	if ((dir_fd = opendir (path)) == NODIR || fstat (dir_fd->d_fd, &s) < 0)
	{
		error ("*Não consegui abrir \"%s\"", path);
		return;
	}

	/*
	 *	Le as entradas do diretório
	 */
	for (EVER)
	{
		if ((tp = malloc (sizeof (TEXT))) == NOTEXT)
			error (NOSTR);

		if ((dp = readdir (dir_fd)) == NODIRENT)
			{ free (tp); break; }

		memmove (&tp->t_dirent, dp, sizeof (DIRENT));

		if (text1 == NOTEXT)
			text1 = tp;
		else
			text2->t_next = tp;

		text2 = tp;

		tp->t_dev  = s.st_dev;
	   /***	tp->t_dir  = ... acima; ***/
		tp->t_next = NOTEXT;

	}	/* end for (EVER) */
		
	closedir (dir_fd);

}	/* end getdir */

/*
 ****************************************************************
 *	Procura o nome na Tabela				*
 ****************************************************************
 */
const char *
gettext (ino_t ino, dev_t dev)
{
	TEXT		*tp;

	for (tp = text1; tp != NOTEXT; tp = tp->t_next)
	{
		if (tp->t_dirent.d_ino == ino && tp->t_dev == dev)
			return (tp->t_dirent.d_name);
	}

	return ("????");

}	/* end gettext */

/*
 ****************************************************************
 *	Substitui variáveis "$..." do ambiente em um caminho	*
 ****************************************************************
 */
char *
env_var_scan (char *output_path, const char *input_path)
{
	const char	*path, *env;
	int		slash_missing = 0;
	char		*str;

	str = alloca (strlen (input_path) + 1); strcpy (str, input_path);

	for
	(	path = strfield (str, '/');
		path != NOSTR;
		path = strfield (NOSTR, '/')
	)
	{
		if (slash_missing)
			strcat (output_path, "/");
		else
			slash_missing++;

		if (path[0] == '$' && (env = getenv (path + 1)) != NOSTR)
			strcat (output_path, env);
		else
			strcat (output_path, path);
	}

	return (output_path);

}	/* end env_var_scan */

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
		"%s - imprime uma tabela de programas reentrantes em uso\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
