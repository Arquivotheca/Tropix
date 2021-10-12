/*
 ****************************************************************
 *								*
 *			test.c					*
 *								*
 *	Testa propriedades de arquivos (para o "sh")		*
 *								*
 *	Versão	2.0.0, de 01.08.88				*
 *		4.2.0, de 24.04.02				*
 *								*
 *	Módulo: test						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stat.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 24.04.02";

#define elif	else if
#define IPSHIFT	3

entry int	vflag;		/* Verbose */
entry int	oflag;		/* Outório */

/*
 ****** Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Testa propriedades de arquivos (para o "sh")		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, m, fd, fatores = 0, sucesso = 1;
	int		EUID = geteuid ();
	int		EGID = getegid ();
	long		mode;
	STAT		s;
	const char	*str;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "or:w:x:f:d:c:b:l:p:u:g:k:m:s:t:vH")) != EOF)
	{
		m = 0;

		switch (opt)
		{
		    case 'o':			/* Outório */
			if (fatores)
				{ error ("A opção \"-o\" deve ser a primeira\n"); help (); }

			oflag++;
			sucesso = 0;
			continue;

		    case 'r':			/* Leitura */
			if ((fd = open (optarg, O_RDONLY)) >= 0)
				{ m = 1; close (fd); }
			break;

		    case 'w':			/* Escrita */
			if ((fd = open (optarg, O_WRONLY)) >= 0)
				{ m = 1; close (fd); }
			break;

		    case 'x':			/* Execução */
			if (stat (optarg, &s) >= 0)
			{
				mode = S_IXUSR;

				if   (EUID == 0)
				{
					mode = (S_IXUSR|S_IXGRP|S_IXOTH);
				}
				elif (EUID != s.st_uid)
				{
					mode >>= IPSHIFT;

					if (EGID != s.st_gid)
						mode >>= IPSHIFT;
				}

				if (mode & s.st_mode)
					m = 1;
			}
			break;

		    case 'f':			/* Regular */
			if (stat (optarg, &s) >= 0 && S_ISREG (s.st_mode))
				m = 1;
			break;

		    case 'd':			/* Diretório */
			if (stat (optarg, &s) >= 0 && S_ISDIR (s.st_mode))
				m = 1;
			break;

		    case 'c':			/* Caracteres */
			if (stat (optarg, &s) >= 0 && S_ISCHR (s.st_mode))
				m = 1;
			break;

		    case 'b':			/* Blocos */
			if (stat (optarg, &s) >= 0 && S_ISBLK (s.st_mode))
				m = 1;
			break;

		    case 'l':			/* Elo simbólico */
			if (lstat (optarg, &s) >= 0 && S_ISLNK (s.st_mode))
				m = 1;
			break;

		    case 'p':			/* Fifo */
			if (stat (optarg, &s) >= 0 && S_ISFIFO (s.st_mode))
				m = 1;
			break;

		    case 'u':			/* User-set-ID */
			if (stat (optarg, &s) >= 0 && s.st_mode & S_ISUID)
				m = 1;
			break;

		    case 'g':			/* Group-set-ID */
			if (stat (optarg, &s) >= 0 && s.st_mode & S_ISGID)
				m = 1;
			break;

		    case 'k':			/* Swap */
			if (stat (optarg, &s) >= 0 && s.st_mode & S_ISVTX)
				m = 1;
			break;

		    case 'm':			/* Memory */
			if (stat (optarg, &s) >= 0 && s.st_mode & S_IMETX)
				m = 1;
			break;

		    case 's':			/* Size > 0 */
			if (stat (optarg, &s) >= 0 && s.st_size > 0)
				m = 1;
			break;

		    case 't':			/* Terminal */
			if ((fd = strtol (optarg, &str, 0)) < 0 || *str != '\0')
				{ error ("Descritor de arquivo inválido: \"%s\"\n", optarg); help (); }

			if (isatty (fd))
				m = 1;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			continue;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

		fatores++;

		/*
		 *	Acumula este resultado
		 */
		if (oflag)
			sucesso |= m;		/* Ou-tório */
		else
			sucesso &= m;		/* E-ório */

	}	/* end while */

	argv += optind;

	if (*argv != NOSTR)
		help ();

	/*
	 *	Calcula o valor de retorno
	 */
	m = (fatores == 0) || (sucesso == 0);

	if (vflag)
		printf ("%s: retornando %d\n", pgname, m);

	return (m);

}	/* end test */

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
		"%s - testa propriedades de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-o] [-{rwxfdcblpugkms} <arquivo> ...] [-t <fd> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-o: Efetua o \"ou-tório\" das condições abaixo ao invés do \"e-ório\"\n"
		"\t-r: Permissão de leitura\n"
		"\t-w: Permissão de escrita\n"
		"\t-x: Permissão de execução/pesquisa\n"
		"\t-f: O <arquivo> é regular\n"
		"\t-d: O <arquivo> é um diretório\n"
		"\t-c: O <arquivo> é especial de caracteres\n"
		"\t-b: O <arquivo> é especial de blocos\n"
		"\t-l: O <arquivo> é um elo simbólico\n"
	);
	fprintf
	(	stderr,
		"\t-p: O <arquivo> é um \"fifo\"\n"
		"\t-u: Indicador \"set-user-ID\" ligado\n"
		"\t-g: Indicador \"set-group-ID\" ligado\n"
		"\t-k: Indicador para manter o texto no \"swap\"\n"
		"\t-m: Indicador para manter o texto na memória\n"
		"\t-s: Tamanho do <arquivo> não NULO\n"
		"\t-t: O descritor <fd> é de um terminal/vídeo\n"
		"\nObs.:\tA opção \"-o\" deve ser a primeira\n"
	);

	exit (2);

}	/* end help */
