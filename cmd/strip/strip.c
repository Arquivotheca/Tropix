/*
 ****************************************************************
 *								*
 *			strip.c					*
 *								*
 *  Remove as tabelas de símbolos e depuração de módulos-objeto	*
 *								*
 *	Versão	1.0.0, de 11.06.86				*
 *		4.3.0, de 06.06.02				*
 *								*
 *	Módulo: strip						*
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
#include <a.out.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.3.0, de 06.06.02";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	wflag;		/* Remova apenas WSDB */
entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

/*
 ****** Protótipos de funções ***********************************
 */
int		strip_file (int, const char *);
void		header_endian_conversion (HEADER *);
void		help (void);

/*
 ****************************************************************
 *  Remove as tabelas de símbolos e depuração de módulos-objeto	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "wvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'w':			/* Remova apenas WSDB */
			wflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
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
	 *	Analisa o Nflag
	 */
	if   (*argv == NOSTR)
	{
		if (!Nflag)
		{
			exit_code += strip_file (0, "a.out");

			exit (exit_code);
		}
	}
	elif (Nflag)
	{
		error ("Os argumentos supérfluos serão ignorados");
		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR) 		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		while (fngets (area, 1024, stdin) != NOSTR)
			exit_code += strip_file (2, area);
	}
	else				/* Foram dados argumentos */
	{
		for (/* vazio */; *argv; argv++)
			exit_code += strip_file (argc, *argv);
	}

	return (exit_code);

}	/* end strip */

/*
 ****************************************************************
 *	Processa um arquivo					*
 ****************************************************************
 */
int
strip_file (int argc, const char *file_nm)
{
	int		fd;
	off_t		sym_sz, db_sz, sz;
	void		*file_addr;
	HEADER		h;
	STAT		s;

	/*
	 *	Escreve o nome do arquivo
	 */
	if (argc > 1)
		printf ("%s:\n", file_nm);

	/*
	 *	Abre o arquivo
	 */
	if ((fd = open (file_nm, O_RDONLY)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", file_nm);
		return (1);
	}

	/*
	 *	Obtém o estado do arquivo
	 */
	if (fstat (fd, &s) < 0)
	{
		error ("*Não consegui obter o estado de \"%s\"", file_nm);
		close (fd);
		return (1);
	}

	/*
	 *	Analisa o tipo do arquivo
	 */
	if (!S_ISREG (s.st_mode))
	{
		error ("\"%s\" não é regular", file_nm);
		close (fd);
		return (1);
	}

	/*
	 *	Le o cabeçalho do arquivo
	 */
	if ((sz = read (fd, &h, sizeof (HEADER))) < 0)
	{
		error ("*Erro na leitura do cabeçalho de \"%s\"", file_nm);
		close (fd);
		return (1);
	}

#ifdef	HEADER_ENDIAN_CV
	header_endian_conversion (&h);
#endif	HEADER_ENDIAN_CV

	/*
	 *	Verifica se o arquivo é um módulo-objeto
	 */
	if (sz != sizeof (HEADER) || h.h_magic != FMAGIC && h.h_magic != NMAGIC && h.h_magic != SMAGIC)
	{
		error ("\"%s\" não é um módulo objeto", file_nm);
		close (fd);
		return (1);
	}

	/*
	 *	Verifica se o arquivo tem as tabelas em questão
	 */
	sym_sz	= h.h_ssize   + h.h_rtsize + h.h_rdsize;
	db_sz	= h.h_lnosize + h.h_dbsize;
	sz	= h.h_tsize   + h.h_dsize  + h.h_refsize;

	if   (wflag)
	{
		if (db_sz == 0)
		{
			error ("\"%s\" não tem tabelas de depuração", file_nm);
			close (fd);
			return (0);
		}

		sz += sym_sz;
	}
	elif (sym_sz + db_sz == 0)
	{
		error ("\"%s\" não tem tabela de símbolos nem de depuração", file_nm);
		close (fd);
		return (0);
	}

	/*
	 *	Aloca memória para o TEXT + DATA + SYM (talvez)
	 */
	if ((file_addr = malloc (sz)) == NOVOID)
	{
		error ("*Não consegui memória para ler \"%s\"", file_nm);
		close (fd);
		return (0);
	}

	/*
	 *	Lê o TEXT + DATA + SYM (talvez)
	 */
	if (read (fd, file_addr, sz - h.h_refsize) != sz - h.h_refsize)
	{
		error ("*Erro na leitura do texto/data/symtb de \"%s\"", file_nm);
		free (file_addr);
		close (fd);
		return (1);
	}

	/*
	 *	Lê o REFTB
	 */
	if (h.h_refsize)
	{
		lseek
		(	fd,
			sizeof (h) + h.h_tsize + h.h_dsize + sym_sz + db_sz + h.h_modsize,
			SEEK_SET
		);

		if (read (fd, file_addr + sz - h.h_refsize, h.h_refsize) != h.h_refsize)
		{
			error ("*Erro na leitura da tabela de referências externas de \"%s\"", file_nm);
			free (file_addr);
			close (fd);
			return (1);
		}
	}

	/*
	 *	Reabre o arquivo
	 */
	close (fd);

	if ((fd = open (file_nm, O_WRONLY|O_TRUNC)) < 0)
	{
		error ("*Não consegui reabrir \"%s\"", file_nm);
		free (file_addr);
		return (1);
	}

	/*
	 *	Zera os campos em questão
	 */
	h.h_lnosize = 0; h.h_dbsize = 0;

	if (!wflag)
		{ h.h_ssize = 0; h.h_rtsize = 0; h.h_rdsize = 0; }

	/*
	 *	Escreve o cabeçalho do arquivo
	 */
#ifdef	HEADER_ENDIAN_CV
	header_endian_conversion (&h);
#endif	HEADER_ENDIAN_CV

	if (write (fd, &h, sizeof (HEADER)) != sizeof (HEADER))
	{
		error ("*Erro na escrita do cabeçalho de \"%s\"", file_nm);
		free (file_addr);
		close (fd);
		return (1);
	}

	/*
	 *	Escreve o TEXT + DATA + SYM (talvez)
	 */
	if (write (fd, file_addr, sz) != sz)
	{
		error ("*Erro na escrita do texto/data/symtb de \"%s\"", file_nm);
		free (file_addr);
		close (fd);
		return (1);
	}

	/*
	 *	Terminou com sucesso
	 */
	free (file_addr);
	close (fd);

	return (0);

}	/* end strip_file */

#ifdef	HEADER_ENDIAN_CV
/*
 ****************************************************************
 *	Converte um cabeçalho de programa de ordem de bytes 	*
 ****************************************************************
 */
void
header_endian_conversion (HEADER *hp)
{
   	hp->h_machine	= big_to_little_short (hp->h_machine);	
	hp->h_magic	= big_to_little_short (hp->h_magic);
   	hp->h_version	= big_to_little_short (hp->h_version);
   	hp->h_flags	= big_to_little_short (hp->h_flags);
	hp->h_time	= big_to_little_long  (hp->h_time);	
   	hp->h_serial	= big_to_little_long  (hp->h_serial);
   	hp->h_tstart	= big_to_little_long  (hp->h_tstart);
   	hp->h_dstart	= big_to_little_long  (hp->h_dstart);
   	hp->h_entry	= big_to_little_long  (hp->h_entry);
	hp->h_tsize	= big_to_little_long  (hp->h_tsize);
	hp->h_dsize	= big_to_little_long  (hp->h_dsize);
	hp->h_bsize	= big_to_little_long  (hp->h_bsize);
	hp->h_ssize	= big_to_little_long  (hp->h_ssize);
	hp->h_rtsize	= big_to_little_long  (hp->h_rtsize);
	hp->h_rdsize	= big_to_little_long  (hp->h_rdsize);
   	hp->h_lnosize	= big_to_little_long  (hp->h_lnosize);
   	hp->h_dbsize	= big_to_little_long  (hp->h_dbsize);

}	/* end header_endian_conversion */

#endif	HEADER_ENDIAN_CV
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
		"%s - remove as tabelas de símbolos e depuração de módulos-objeto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-wN] [<módulo> ...] \n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-w: Remove apenas as tabelas de depuração\n"
		"\t-N: Lê os nomes dos módulos de \"stdin\"\n"
		"\nObs.:\tSe não for dado nenhum <módulo> será considerado \"a.out\"\n"
	);

	exit (2);

}	/* end help */
