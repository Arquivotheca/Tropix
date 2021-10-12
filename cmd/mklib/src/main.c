/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Módulo principal de "mklib"				*	
 *								*
 *	Versão	1.0.0, de 23.10.86				*
 *		4.2.0, de 01.10.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stat.h>
#include <fcntl.h>
#include <errno.h>
#include <a.out.h>

#include "../h/mklib.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 01.10.01";

entry int		exit_code = 0;	/* Código de retorno */

entry int		vflag;		/* Verbose */
entry int		fflag;		/* Atualiza, sem consultar */

entry int		(*command) () = NOFUNC;	/* Função a executar */

entry MOD		*mod_first, *mod_last;	/* Lista de módulos */

/*
 ****** Nomes das bibliotecas ***********************************
 */
entry const char	*lib_dir_nm;	/* Nome da biblioteca */

entry int		lib_dir_nm_len; /* Comprimento do nome da biblioteca */

entry char		*lib_sinop_nm;	/* Nome do arquivo de sinopse */

entry const char	lib_sym_nm[] = ".LIBSYM";

entry const char	libc_nm[] = "/lib/libc";
entry const char	libm_nm[] = "/lib/libm";

/*
 ****** Dados das bibliotecas ***********************************
 */
entry int		lib_dev;	/* Dispositivo da biblioteca */
entry int		lib_ino;	/* No. do nó-índice da biblioteca */
entry char		lib_dev_nm[16];	/* Nome do dispositivo da biblioteca */

/*
 ****************************************************************
 *	Módulo principal de "mklib"				*	
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*lib_nm_str = NOSTR;
	int		modif;
	STAT		s;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "rudtkfvcmH")) != EOF)
	{
		switch (opt)
		{
		    case 'r':				/* Replace */
			if (command != NOFUNC)
				help ();
			command = do_replace;
			break;

		    case 'u':				/* Update */
			if (command != NOFUNC)
				help ();
			command = do_update;
			break;

		    case 'd':				/* Delete */
			if (command != NOFUNC)
				help ();
			command = do_delete;
			break;

		    case 't':				/* Table */
			if (command != NOFUNC)
				help ();
			command = do_table;
			break;

		    case 'k':				/* Check */
			if (command != NOFUNC)
				help ();
			command = do_consistency;
			break;

		    case 'f':				/* Força */
			fflag++;
			break;

		    case 'v':				/* Verbose */
			vflag++;
			break;

		    case 'c':				/* Libc */
			lib_nm_str = libc_nm;
			break;

		    case 'm':				/* Libm */
			lib_nm_str = libm_nm;
			break;

		    case 'H':				/* Help */
			help ();

		    default:				/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
   /***	argc -= optind; ***/

	/*
	 *	Prepara o nome da biblioteca
	 */
	if (lib_nm_str == NOSTR)
		lib_nm_str = *argv++;

	if (lib_nm_str == NOSTR)
	{
		fprintf (stderr, "Não foi dado o nome da biblioteca?\n\n");
		help ();
	}

	lib_dir_nm	= path_can (lib_nm_str);

	lib_dir_nm_len	= strlen (lib_dir_nm);

	/*
	 *	Verifica a função
	 */
	if  (command == NOFUNC)
		command = do_create;

	/*
	 *	Prepara o nome da biblioteca e do arquivo de sinopse
	 */
	if ((lib_sinop_nm = malloc (strlen (lib_dir_nm) + 1 + strlen (lib_sym_nm) + 1)) == NOSTR)
		error (NOSTR);

	strcpy (lib_sinop_nm, lib_dir_nm);
	strcat (lib_sinop_nm, "/");
	strcat (lib_sinop_nm, lib_sym_nm);

#ifdef	DEBUG
	if (vflag > 1)
	{
		printf
		(	"lib_dir_nm = \"%s\", lib_sinop_nm = \"%s\"\n",
			lib_dir_nm, lib_sinop_nm
		);
	}
#endif	DEBUG

	/*
	 *	O nome da biblioteca deve ser um diretório
	 */
	if (stat (lib_dir_nm, &s) < 0)
		error ("$*Não consegui obter o estado de \"%s\"", lib_dir_nm);

	if (!S_ISDIR (s.st_mode))
		error ("$\"%s\" não é um diretório", lib_dir_nm);

	lib_dev = s.st_dev; 	lib_ino = s.st_ino;

	if (kcntl (K_GETDEVNM, lib_dev, lib_dev_nm) < 0)
		error ("*Não consegui obter o nome do dispositivo da biblioteca");

	if (vflag)
	{
		printf
		(	"Biblioteca: \"%s\" (dev = \"%s\")\n",
			lib_dir_nm, lib_dev_nm
		);
	}

	/*
	 *	Executa a funcao desejada
	 */
	modif = (*command) (argv);

	/*
	 *	Verifica se deve ordenar a LIBSYM
	 */
	if (modif < 0 && exit_code == 0)
		sort_all_sym_lists ();

	/*
	 *	Havendo alterações, reescreve LIBSYM
	 */
	if   (modif != 0 && exit_code == 0)
	{
		write_sinop_file ();

		if (vflag)
			printf ("Arquivo de sinopse ATUALIZADO\n");
	}
	elif (vflag)
	{
		printf ("Arquivo de sinopse NÃO ALTERADO\n");
	}

	return (exit_code);

}	/* end mklib */

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
		"%s - cria/atualiza bibliotecas de módulos objeto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-rudtkfv] [-cm] [<biblioteca>] [<módulo> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Sem opções, cria o arquivo de sinopse\n"
		"\t-r: Substitui (inclui) os <módulo>s dados\n"
		"\t-u: Atualiza <módulo>s a partir do diretório corrente\n"
		"\t-d: Remove os <módulo>s dados\n"
		"\t-t: Lista o arquivo de sinopse\n"
		"\t-k: Verifica a consistência do arquivo de sinopse\n\n"

		"\t-f: Não consulta o usuário para as atualizações/remoções\n"
		"\t-v: Verboso\n"
		"\t-c: Especifica a \"libc\"\n"
		"\t-m: Especifica a \"libm\"\n"

		"\nObs.:\tA biblioteca pode ser dada com o nome completo "
		"(tal como \"/lib/libc\"),\n"
		"\tou por meio de uma opção (\"-c\" ou \"-m\")\n"
	);

	exit (2);

}	/* end help */
