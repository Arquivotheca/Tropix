/*
 ****************************************************************
 *								*
 *			fsname.c				*
 *								*
 *	Consulta/atualiza nomes/volumes de sistemas de arquivos	*
 *								*
 *	Versão	1.0.0, de 10.07.86				*
 *		4.3.0, de 10.09.02				*
 *								*
 *	Módulo: fsname						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/t1.h>
#include <sys/v7.h>
#include <sys/syscall.h>
#include <sys/kcntl.h>
#include <sys/ioctl.h>
#include <sys/disktb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stat.h>
#include <ustat.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.3.0, de 10.09.02";

entry int	tflag;		/* Tabela */
entry int	sflag;		/* Atualiza */
entry int	fflag;		/* Force */
entry int	vflag;		/* Verbose */

entry int	v7sbmagic;	/* Mágica do superbloco V7 */

const char	*fs_names[] = FS_NAMES;

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	SB_ENDIAN_CV
#define	SB_ENDIAN_LONG(x)	long_endian_cv (x)
#define	SB_ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	SB_ENDIAN_LONG(x)	(x)
#define	SB_ENDIAN_SHORT(x)	(x)
#endif	SB_ENDIAN_CV

/*
 ******	Protótipos de funções ***********************************
 */
void 		table (const char *[]);
void		help (void);

/*
 ****************************************************************
 *	Programa de consultar/atualizar nomes do SB		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	int		fd;
	char		altered = 0;
	DISKTB		d;
	T1SB		t1sb;
	V7SB		v7sb;
	USTAT		u;
	STAT		s;
	char		fs_type = 0;
	char		*fname = NOSTR, *fpack = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "tsfvH")) != EOF)
	{
		switch (opt)
		{
		    case 't':			/* Tabela */
			if (sflag)
				help ();
			tflag++;
			break;

		    case 's':			/* Atualiza */
			if (tflag)
				help ();
			sflag++;
			break;

		    case 'f':			/* Force */
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

	/*
	 *	Obtém a MÁGICA do superbloco
	 */
	v7sbmagic = SB_ENDIAN_LONG (V7_SBMAGIC);

	/*
	 *	Analisa a função a realizar
	 */
	if (!sflag)
		table (argv);

	/*
	 *	Atualização
	 */
	if (argc < 1 || argc > 3)
		help ();

	if (stat (*argv, &s) < 0)
		error ("$*Não consegui obter o estado de \"%s\"", *argv);

	/*
	 *	Prepara o terminal para consultas
	 */
	if ((askyesno_stdin = fopen ("/dev/tty", "r")) == NOFILE)
		error ("$*Não consegui abrir \"/dev/tty\"");

	if (ustat (s.st_rdev, &u) >= 0)
	{
		fprintf
		(	stderr,
			"O dispositivo \"%s\" está montado. "
			"Continua? (n): ",
			argv[0]
		);

		if (askyesno () <= 0)
			exit (1);
	}

	if ((fd = open (argv[0], O_RDWR)) < 0)
		error ("$*Não consegui abrir \"%s\"", *argv);

	if (ioctl (fd, DKISADISK) < 0)
		error ("$O dispositivo \"%s\" NÃO é um disco", *argv);

	/*
	 *	Verifica se é T1
	 */
   /***	if (fs_type == 0) ***/
	{
		lseek (fd, BL4TOBY (T1_SBNO), SEEK_SET);

		if (read (fd, &t1sb, sizeof (t1sb)) != sizeof (t1sb))
			error ("$*Não consegui ler o superbloco de \"%s\"", *argv);

		if (t1sb.s_magic == T1_SBMAGIC)
		{
			fs_type = 'T';

			fname = t1sb.s_fname;
			fpack = t1sb.s_fpack;
		}
	}

	if (fs_type == 0)
	{
		lseek (fd, BLTOBY (V7_SBNO), SEEK_SET);

		if (read (fd, &v7sb, sizeof (V7SB)) != sizeof (v7sb))
			error ("$*Não consegui ler o superbloco de \"%s\"", *argv);

		if (v7sb.s_magic == v7sbmagic)
		{
			fs_type = 'V';

			fname = v7sb.s_fname;
			fpack = v7sb.s_fpack;
		}
	}

	if (fs_type == 0)
		error ("$O Dipositivo \"%s\" NÃO contém um sistema de arquivos T1 ou V7", *argv);

	/*
	 *	Obtém o <nome>
	 */
	if (argc > 1)
	{
		if   (streq (argv[1], "-"))
			{ memset (fname, '\0', 16); altered++; }
		elif (!streq (argv[1], "."))
			{ strscpy (fname, argv[1], sizeof (t1sb.s_fname)); altered++; }
	}

	/*
	 *	Obtém o <volume>
	 */
	if (argc > 2)
	{
		if (kcntl (K_GET_DEV_TB, s.st_rdev, &d) < 0)
			error ("$*Não consegui obter a tabela de \"%s\"", argv[0]);

		if   (streq (argv[2], "-"))
			{ strscpy (fpack, d.p_name, sizeof (t1sb.s_fpack)); altered++; }
		elif (!streq (argv[2], "."))
			{ strscpy (fpack, argv[2], sizeof (t1sb.s_fpack)); altered++; }
	}

	/*
	 *	Se não alterou, ...
	 */
	if (!altered)
	{
		printf
		(	"NÃO alterado: \"%s\", \"%s\", \"%s\"\n",
			argv[0],  fname, fpack
		);
		exit (0);
	}

	/*
	 *	Consulta para ver se deseja atualizar
	 */
	if (!fflag)
	{
		fprintf
		(	stderr,
			"Atualiza \"%s\", \"%s\", \"%s\"? (n): ",
			argv[0],  fname, fpack
		);

		if (askyesno () <= 0)
			exit (1);
	}

	/*
	 *	Grava o novo SB
	 */
	switch (fs_type)
	{
	    case 'T':
		lseek (fd, BL4TOBY (T1_SBNO), SEEK_SET);

		if (write (fd, &t1sb, sizeof (t1sb)) != sizeof (t1sb))
			error ("$*Não consegui escrever o superbloco de \"%s\"", argv[0]);
		break;

	    case 'V':
		lseek (fd, BLTOBY (V7_SBNO), SEEK_SET);

		if (write (fd, &v7sb, sizeof (V7SB)) != sizeof (V7SB))
			error ("$*Não consegui escrever o superbloco de \"%s\"", argv[0]);
		break;
	}


	return (0);

}	/* end fsname */

/*
 ****************************************************************
 *	Imprime uma tabela dos SBs				*
 ****************************************************************
 */
void
table (const char *argv[])
{
	int		fd;
	T1SB		t1sb;
	V7SB		v7sb;
	USTAT		u;
	STAT		s;

	if (*argv == NOSTR)
		help ();

	printf ("--- DISPOSITIVO --- TIPO ---- NOME ---- --- VOLUME ---\n");

	for (/* vazio */; *argv; argv++)
	{
		if (stat (*argv, &s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", *argv);
			continue;
		}

		if (ustat (s.st_rdev, &u) >= 0)
		{
			printf ("%-20.20s", *argv);
			printf ("%-4.4s %-14.14s %.14s\n", fs_names[u.f_type], u.f_fname, u.f_fpack);
			continue;
		}

		if ((fd = open (*argv, O_RDONLY)) < 0)
			{ error ("*Não consegui abrir \"%s\"", *argv); continue; }

		if (ioctl (fd, DKISADISK) < 0)
			{ error ("O dispositivo \"%s\" NÃO é um disco", *argv); close (fd); continue; }

		printf ("%-20.20s", *argv);

		/*
		 *	Verifica se é T1
		 */
		lseek (fd, BL4TOBY (T1_SBNO), SEEK_SET);

		if (read (fd, &t1sb, sizeof (t1sb)) != sizeof (t1sb))
			{ error ("*Não consegui ler o superbloco de \"%s\"", *argv); close (fd); continue; }

		if (t1sb.s_magic == T1_SBMAGIC)
		{
			printf ("T1   %-14.14s %.14s\n", t1sb.s_fname, t1sb.s_fpack);
			close (fd);
			continue;
		}

		/*
		 *	Verifica se é V7
		 */
		lseek (fd, BLTOBY (V7_SBNO), SEEK_SET);

		if (read (fd, &v7sb, sizeof (V7SB)) < 0)
			{ error ("*Não consegui ler o superbloco de \"%s\"", *argv); close (fd); continue; }

		if (v7sb.s_magic == v7sbmagic)
			printf ("V7   %-14.14s %.14s\n", v7sb.s_fname, v7sb.s_fpack);
		else
			printf (".\n");

		close (fd);

	}	/* end for (argv) */

	exit (0);

}	/* end table */

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
		"%s - consulta/atualiza nomes/volumes de sistemas de arquivos T1 e V7\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-t] <dispositivo> ...\n"
		"\t%s -s [-f] <dispositivo> <nome> [<volume>]\n",
		pgname, pgversion, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-t: Imprime uma tabela com o nome e volume "
			"dos <dispositivo>s dados\n"
		"\t-s: Atualiza o <nome> e <volume> do <dispositivo>\n"
		"\t-f: A atualização é realizada sem confirmação do usuário\n"
		"\nObs.:\tSe o <nome> ou <volume> forem dados como \"-\" recebem "
			"um valor \"padrão\"."
		"\n\tSe não forem dados, ou dados como \".\", não são modificados.\n"
	);

	exit (2);

}	/* end help */
