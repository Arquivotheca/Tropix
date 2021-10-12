/*
 ****************************************************************
 *								*
 *			files.h					*
 *								*
 *	File names and variables for bison			*
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

/* These two should be pathnames for opening the sample parser files.
   When bison is installed, they should be absolute pathnames.
   XPFILE1 and XPFILE2 normally come from the Makefile.  */

#define PFILE	XSTR (XPFILE)		/* Simple parser */
#define PFILE1	XSTR (XPFILE1)		/* Semantic parser */

extern FILE *finput;   /* read grammar specifications */
extern FILE *foutput;  /* optionally output messages describing the actions taken */
extern FILE *fdefines; /* optionally output #define's for token numbers. */
extern FILE *ftable;   /* output the tables and the parser */
extern FILE *fattrs;   /* if semantic parser, output a .h file that defines YYSTYPE */
		       /* and also contains all the %{ ... %} definitions.  */
extern FILE *fguard;   /* if semantic parser, output yyguard, containing all the guard code */
extern FILE *faction;  /* output all the action code; precise form depends on which parser */
extern FILE *fparser;  /* read the parser to copy into ftable */

/* File name specified with -o for the output file, or 0 if no -o.  */
extern const char *spec_outfile;

extern const char *spec_name_prefix; /* for -a, from getargs.c */

/* File name pfx specified with -b, or 0 if no -b.  */
extern char const *spec_file_prefix;

extern const char *infile;
extern char *outfile;
extern char *defsfile;
extern char *tabfile;
extern char *attrsfile;
extern char *guardfile;
extern char *actfile;
