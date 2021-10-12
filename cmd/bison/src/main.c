/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Top level entry point of bison				*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/machine.h"	/* JF for MAXSHORT */
#include "../h/files.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0 (1.21), de 06.07.93";

char *version_string = "GNU Bison version 1.21\n";

extern	int lineno;

int verboseflag;
int definesflag;
int debugflag;
int nolinesflag;

const char	*spec_name_prefix; /* for -p.  */
const char	*spec_file_prefix; /* for -b. */
extern int	fixed_outfiles;/* for -y */


/* Nonzero means failure has been detected; don't write a parser file.  */
int failure;

void		help (void);

/*
 ****************************************************************
 *	Programa principal					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
  int opt;

  failure = 0;
  lineno = 0;

  verboseflag = 0;
  definesflag = 0;
  debugflag = 0;
  fixed_outfiles = 0;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "b:dlo:p:tvVyH")) != EOF)
	{
		switch (opt)
		{
		    case 'b':			/* File prefix */
			spec_file_prefix = optarg;
			break;

		    case 'd':			/* Defines */
			definesflag = 1;
			break;

		    case 'l':			/* No lines */
			nolinesflag = 1;
			break;

		    case 'o':			/* Out file */
			spec_outfile = optarg;
			break;

		    case 'p':			/* Prefix */
			spec_name_prefix = optarg;
			break;

		    case 't':			/* Debug */
			debugflag = 1;
			break;

		    case 'v':			/* Verbose */
			verboseflag = 1;
			break;

		    case 'V':			/* Version */
			printf ("%s", version_string);
			break;

		    case 'y':			/* Fixed output files */
			fixed_outfiles = 1;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

  if (optind == argc)
    {
      fprintf(stderr, "%s: no grammar file given\n", pgname);
      exit(1);
    }
  if (optind < argc - 1)
    fprintf(stderr, "%s: warning: extra arguments ignored\n", pgname);

  infile = argv[optind];

  openfiles ();

  /* read the input.  Copy some parts of it to fguard, faction, ftable and fattrs.
     In file reader.c.
     The other parts are recorded in the grammar; see gram.h.  */
  reader();

  /* find useless nonterminals and productions and reduce the grammar.  In
     file reduce.c */
  reduce_grammar();

  /* record other info about the grammar.  In files derives and nullable.  */
  set_derives();
  set_nullable();

  /* convert to nondeterministic finite state machine.  In file LR0.
     See state.h for more info.  */
  generate_states();

  /* make it deterministic.  In file lalr.  */
  lalr();

  /* Find and record any conflicts: places where one token of lookahead is not
     enough to disambiguate the parsing.  In file conflicts.
     Currently this does not do anything to resolve them;
     the trivial form of conflict resolution that exists is done in output.  */
  initialize_conflicts();

  /* print information about results, if requested.  In file print. */
  if (verboseflag)
    verbose();
  else
    terse();

  /* output the tables and the parser to ftable.  In file output. */
  output();
  done(failure);

}	/* end main */

/* functions to report errors which prevent a parser from being generated */

void
fatal (char *s)
{
  if (infile == NOSTR)
    fprintf(stderr, "fatal error: %s\n", s);
  else
    fprintf(stderr, "\"%s\", line %d: %s\n", infile, lineno, s);
  done(1);
}


/* JF changed to accept/deal with variable args.
   DO NOT change this to use varargs.  It will appear to work
   but will break on systems that don't have the necessary library
   functions.  This is the ONLY safe way to write such a function.  */
/*VARARGS1*/

void
fatals (char *fmt, int x1, int x2, int x3, int x4, int x5, int x6, int x7, int x8)
{
  char buffer[200];

  sprintf(buffer, fmt, x1,x2,x3,x4,x5,x6,x7,x8);
  fatal(buffer);
}


void
toomany (char *s)
{
  char buffer[200];

	/* JF new msg */
  sprintf(buffer, "limit of %d exceeded, too many %s", MAXSHORT, s);
  fatal(buffer);
}


void
berror (char *s)
{
  fprintf(stderr, "internal error, %s\n", s);
  abort();
}

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
		"%s - gerador de analisadores sintáticos do GNU\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-dltvVy] [-b <file_prefix>] [-o <out_file>]\n"
			"\t\t\t\t\t[-p <name_prefix>] <grammar>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-d: Gera o arquivo de macro-definições\n"
		"\t-l: Não gera os comandos \"#line\"\n"
		"\t-t: Gera o analisador com \"debug\"\n"
		"\t-v: Gera o arquivo \"*.output\"\n"
		"\t-V: Imprime a versão do gerador\n"
		"\t-y: Utiliza os nomes dos arquivos no estilo do \"yacc\"\n"
		"\t-b: Cria os arquivos com o prefixo <file_prefix>\n"
		"\t-o: Cria o analisador com o nome <out_file>\n"
		"\t-p: Cria os símbolos externos com o prefixo <name_prefix>\n"
	);

	exit (2);

}	/* end help */
