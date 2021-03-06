/*
 ****************************************************************
 *								*
 *			aicasm.c				*
 *								*
 *	Montador para a linguagem simb?lica da placa 2940	*
 *								*
 *	Vers?o	4.0.0, de 15.03.01				*
 *		4.0.0, de 15.03.01				*
 *								*
 *	M?dulo: N?cleo						*
 *		N?CLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright ? 2000 NCE/UFRJ - tecle "man licen?a"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

#include "../h/types.h"
#include "../h/syscall.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../h/queue.h"
#include "../aic/aicasm.h"
#include "../aic/asm_symbol.h"
#include "../aic/asm_format.h"
#include "../aic/sysexits.h"

/*
 ****************************************************************
 *	Defini??es globais					*
 ****************************************************************
 */
typedef struct patch
{
	STAILQ_ENTRY (patch)	links;
	int			patch_func;
	unsigned		begin;
	unsigned		skip_instr;
	unsigned		skip_patch;

}	patch_t;

STAILQ_HEAD (patch_list, patch)	patches;

/*
 ******	Vari?veis globais ***************************************
 */
entry struct path_list			search_path;
entry int				includes_search_curdir;
entry const char			*appname;
entry FILE				*ofile;
entry const char			*ofilename;
entry const char			*regfilename;
entry FILE				*regfile;
entry const char			*listfilename;
entry FILE				*listfile;

entry STAILQ_HEAD (, instruction)	seq_program;
entry struct cs_tailq			cs_tailq;
entry struct scope_list			scope_stack;
entry symlist_t				patch_functions;

extern FILE				*yyin;

/*
 ******	Prot?tipos de fun??es ***********************************
 */
void		usage (void);
void		back_patch (void);
void		output_code (void);
void		output_listing (const char *ifilename);
void		dump_scope (scope_t * scope);
void		emit_patch (scope_t * scope, int patch);
int		check_patch (patch_t ** start_patch, int start_instr, int *skip_addr, int *func_vals);

extern void	symtable_open (void);
extern void	symtable_dump (FILE *);
extern void	symlist_free (symlist_t *symlist);
extern void	symtable_close (void);

extern int	yyparse (void);

/*
 ****************************************************************
 *	Montador para a linguagem simb?lica da placa 2940	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		ch;
	int		retval;
	const char	*inputfilename;
	scope_t		*sentinal;

	STAILQ_INIT (&patches);
	SLIST_INIT (&search_path);
	STAILQ_INIT (&seq_program);
	TAILQ_INIT (&cs_tailq);
	SLIST_INIT (&scope_stack);

	/*
	 *	Set Sentinal scope node
	 */
	sentinal = scope_alloc ();
	sentinal->type = SCOPE_ROOT;

	includes_search_curdir = 1;
	appname = *argv;
	regfile = NULL;
	listfile = NULL;

	/*
	 *	Examina os argumentos
	 */
	while ((ch = getopt (argc, argv, "d:l:n:o:r:I:O:")) != -1)
	{
		switch (ch)
		{
		    case 'd':
			stop ("-d: Assembler not built with debugging information", EX_SOFTWARE);
			break;

		    case 'l': 			/* Create a program listing */
			if ((listfile = fopen (optarg, "w")) == NULL)
			{
				perror (optarg);
				stop (NULL, EX_CANTCREAT);
			}
			listfilename = optarg;
			break;

		    case 'n': 			/* Don't complain about the -nostdinc directrive */
			if (strcmp (optarg, "ostdinc"))
			{
				fprintf
				(	stderr,
					"%s: Unknown option -%c%s\n",
					 appname, ch, optarg
				);
				usage ();
			}
			break;

		    case 'o':
			if ((ofile = fopen (optarg, "w")) == NULL)
			{
				perror (optarg);
				stop (NULL, EX_CANTCREAT);
			}
			ofilename = optarg;
			break;

		    case 'r':
			if ((regfile = fopen (optarg, "w")) == NULL)
			{
				perror (optarg);
				stop (NULL, EX_CANTCREAT);
			}
			regfilename = optarg;
			break;

		    case 'I':
		    {
			path_entry_t	include_dir;

			if (strcmp (optarg, "-") == 0)
			{
				if (includes_search_curdir == 0)
				{
					fprintf
					(	stderr,
						"%s: Warning - '-I-' specified multiple times\n",
						appname
					);
				}

				includes_search_curdir = 0;

				for
				(	include_dir = search_path.slh_first;
					include_dir != NULL;
					include_dir = include_dir->links.sle_next
				)
				{
					/*
					 * All entries before a '-I-'
					 * only apply to includes
					 * specified with quotes
					 * instead of "<>".
					 */
					include_dir->quoted_includes_only = 1;
				}
			}
			else
			{
				include_dir = (path_entry_t) malloc (sizeof (*include_dir));

				if (include_dir == NULL)
				{
					perror (optarg);
					stop (NULL, EX_OSERR);
				}

				include_dir->directory = strdup (optarg);

				if (include_dir->directory == NULL)
				{
					perror (optarg);
					stop (NULL, EX_OSERR);
				}

				include_dir->quoted_includes_only = 0;

				SLIST_INSERT_HEAD (&search_path, include_dir, links);
			}
			break;
		    }

		    case '?':
		    default:
			usage ();

		}	/* end switch */

	}	/* end while */

	argc -= optind;
	argv += optind;

	if (argc != 1)
	{
		fprintf (stderr, "%s: No input file specifiled\n", appname);
		usage ();
	}

	/*
	 *	Compila o programa fonte
	 */
	symtable_open ();
	inputfilename = *argv;
	include_file (*argv, SOURCE_FILE);
	retval = yyparse ();

	/*
	 *	Ep?logo
	 */
	if (retval == 0)
	{
		if
		(	SLIST_FIRST (&scope_stack) == NULL
			|| SLIST_FIRST (&scope_stack)->type != SCOPE_ROOT
		)
		{
			stop ("Unterminated conditional expression", EX_DATAERR);
		}

		/* Process outmost scope */

		process_scope (SLIST_FIRST (&scope_stack));

		/*
		 * Decend the tree of scopes and insert/emit patches as
		 * appropriate.  We perform a depth first tranversal,
		 * recursively handling each scope.
		 */
		/* start at the root scope */

		dump_scope (SLIST_FIRST (&scope_stack));

		/* Patch up forward jump addresses */

		back_patch ();

		if (ofile != NULL)
			output_code ();

		if (regfile != NULL)
			symtable_dump (regfile);

		if (listfile != NULL)
			output_listing (inputfilename);
	}

	stop (NOSTR, 0);

	return (0);

}	/* end aicasm */

/*
 ****************************************************************
 *	Modo de usar o programa					*
 ****************************************************************
 */
void
usage (void)
{

	fprintf
	(	stderr,
		 "usage: %-16s [-nostdinc] [-I-] [-I directory] [-o output_file]\n"
		 "[-r register_output_file] [-l program_list_file] input_file\n",
		 appname
	);

	exit (1);

}	/* end usage */

/*
 ****************************************************************
 *	Atualiza os r?tulos					*
 ****************************************************************
 */
void
back_patch (void)
{
	struct instruction *cur_instr;

	for
	(	cur_instr = seq_program.stqh_first;
		cur_instr != NULL;
		cur_instr = cur_instr->links.stqe_next
	)
	{
		if (cur_instr->patch_label != NULL)
		{
			struct ins_format3	*f3_instr;
			unsigned		address;

			if (cur_instr->patch_label->type != LABEL)
			{
				char		buf[255];

				snprintf
				(	buf,
					sizeof (buf),
					"Undefined label %s",
					cur_instr->patch_label->name
				);
				stop (buf, EX_DATAERR);
			}

			f3_instr = &cur_instr->format.format3;
			address = f3_instr->address;
			address += cur_instr->patch_label->info.linfo->address;
			f3_instr->address = address;
		}

	}	/* end for */

}	/* end back_patch */

/*
 ****************************************************************
 *	Escreve o c?digo de sa?da				*
 ****************************************************************
 */
void
output_code (void)
{
	struct instruction	*cur_instr;
	patch_t			*cur_patch;
	critical_section_t	*cs;
	symbol_node_t		*cur_node;
	int			instrcount;

	instrcount = 0;

	fprintf
	(	ofile,
		"/*\n"
		" * DO NOT EDIT - This file is automatically generated.\n"
		" */\n"
	);

	fprintf (ofile, "entry uchar	seqprog[] = {\n");

	for
	(	cur_instr = seq_program.stqh_first;
		cur_instr != NULL;
		cur_instr = cur_instr->links.stqe_next
	)
	{

		fprintf
		(	ofile,
			"%s\t0x%02X, 0x%02X, 0x%02X, 0x%02X",
			cur_instr == seq_program.stqh_first ? "" : ",\n",
			cur_instr->format.bytes[0],
			cur_instr->format.bytes[1],
			cur_instr->format.bytes[2],
			cur_instr->format.bytes[3]
		);
		instrcount++;
	}

	fprintf (ofile, "\n};\n\n");

	/*
	 * Output patch information.  Patch functions first.
	 */
	for
	(	cur_node = SLIST_FIRST (&patch_functions);
		cur_node != NULL;
		cur_node = SLIST_NEXT (cur_node, links)
	)
	{
		fprintf
		(	ofile,
			"entry int ahc_patch%d_func(struct ahc_softc *ahc);\n"
			"\n"
			"entry int\n"
			"ahc_patch%d_func (struct ahc_softc *ahc)\n"
			"{\n"
			"	return (%s);\n"
			"}\n\n",
			cur_node->symbol->info.condinfo->func_num,
			cur_node->symbol->info.condinfo->func_num,
			cur_node->symbol->name
		);
	}

	fprintf
	(	ofile,
		"typedef int patch_func_t (struct ahc_softc *);\n"
		"struct patch {\n"
		"	patch_func_t	*patch_func;\n"
		"	ulong		begin		:10,\n"
		"			skip_instr :10,\n"
		"			skip_patch :12;\n"
		"} patches[] = {\n"
	);

	for
	(	cur_patch = STAILQ_FIRST (&patches);
		cur_patch != NULL;
		cur_patch = STAILQ_NEXT (cur_patch, links)
	)
	{
		fprintf
		(	ofile, "%s\t{ ahc_patch%d_func, %d, %d, %d }",
			cur_patch == STAILQ_FIRST (&patches) ? "" : ",\n",
			cur_patch->patch_func, cur_patch->begin,
			cur_patch->skip_instr, cur_patch->skip_patch
		);
	}

	fprintf (ofile, "\n};\n");

	fprintf
	(	ofile,
		"struct cs {\n"
		"	ushort begin;\n"
		"	ushort end;\n"
		"} critical_sections[] = {\n"
	);

	for (cs = TAILQ_FIRST (&cs_tailq); cs != NULL; cs = TAILQ_NEXT (cs, links))
	{
		fprintf
		(	ofile, "%s\t{ %d, %d }",
			cs == TAILQ_FIRST (&cs_tailq) ? "" : ",\n",
			cs->begin_addr, cs->end_addr
		);
	}

	fprintf (ofile, "\n};\n");

	fprintf
	(	ofile,
		"const int num_critical_sections = sizeof(critical_sections)\n"
		"/ sizeof(*critical_sections);\n"
	);

	fprintf (stderr, "%s: %d instructions used\n", appname, instrcount);

}	/* end output_code */

/*
 ****************************************************************
 *	Emite o escopo						*
 ****************************************************************
 */
void
dump_scope (scope_t * scope)
{
	scope_t	*cur_scope;

	/*
	 *	Emit the first patch for this scope
	 */
	emit_patch (scope, 0);

	/*
	 *	Dump each scope within this one.
	 */
	cur_scope = TAILQ_FIRST (&scope->inner_scope);

	while (cur_scope != NULL)
	{

		dump_scope (cur_scope);

		cur_scope = TAILQ_NEXT (cur_scope, scope_links);
	}

	/*
	 *	Emit the second, closing, patch for this scope
	 */
	emit_patch (scope, 1);

}	/* end dump_scope */

/*
 ****************************************************************
 *	Emite as corre??es					*
 ****************************************************************
 */
void
emit_patch (scope_t * scope, int patch)
{
	patch_info_t	*pinfo;
	patch_t		*new_patch;

	pinfo = &scope->patches[patch];

	if (pinfo->skip_instr == 0) 		/* No-Op patch */
		return;

	new_patch = (patch_t *)malloc (sizeof (*new_patch));

	if (new_patch == NULL)
		stop ("Could not malloc patch structure", EX_OSERR);

	memset (new_patch, 0, sizeof (*new_patch));

	if (patch == 0)
	{
		new_patch->patch_func = scope->func_num;
		new_patch->begin = scope->begin_addr;
	}
	else
	{
		new_patch->patch_func = 0;
		new_patch->begin = scope->end_addr;
	}

	new_patch->skip_instr = pinfo->skip_instr;
	new_patch->skip_patch = pinfo->skip_patch;
	STAILQ_INSERT_TAIL (&patches, new_patch, links);

}	/* end emit_patch */

/*
 ****************************************************************
 *	Gera a listagem						*
 ****************************************************************
 */
void
output_listing (const char *ifilename)
{
	char			buf[1024];
	FILE			*ifile;
	struct instruction	*cur_instr;
	patch_t			*cur_patch;
	symbol_node_t		*cur_func;
	int			*func_values;
	int			instrcount;
	int			instrptr;
	int			line;
	int			func_count;
	int			skip_addr;

	instrcount = 0;
	instrptr = 0;
	line = 1;
	skip_addr = 0;

	if ((ifile = fopen (ifilename, "r")) == NULL)
	{
		perror (ifilename);
		stop (NULL, EX_DATAERR);
	}

	/*
	 *	Determine which options to apply to this listing.
	 */
	for
	(	func_count = 0, cur_func = SLIST_FIRST (&patch_functions);
		cur_func != NULL;
		cur_func = SLIST_NEXT (cur_func, links)
	)
		func_count++;

	func_values = NULL;

	if (func_count != 0)
	{
		func_values = (int *)malloc (func_count * sizeof (int));

		if (func_values == NULL)
			stop ("Could not malloc", EX_OSERR);

		func_values[0] = 0;	/* FALSE func */
		func_count--;

		/*
		 *	Ask the user to fill in the return values for the rest of the functions.
		 */
		for
		(	cur_func = SLIST_FIRST (&patch_functions);
			cur_func != NULL && SLIST_NEXT (cur_func, links) != NULL;
			cur_func = SLIST_NEXT (cur_func, links), func_count--
		)
		{
			int		input;

			fprintf (stdout, "\n(%s)\n", cur_func->symbol->name);

			fprintf (stdout, "Enter the return value for this expression[T/F]:");

			while (1)
			{

				input = getchar ();
				input = toupper (input);

				if (input == 'T')
				{
					func_values[func_count] = 1;
					break;
				}
				else if (input == 'F')
				{
					func_values[func_count] = 0;
					break;
				}
			}

			if (isatty (fileno (stdin)) == 0)
				putchar (input);
		}

		fprintf (stdout, "\nThanks!\n");
	}

	/* Now output the listing */

	cur_patch = STAILQ_FIRST (&patches);

	for
	(	cur_instr = STAILQ_FIRST (&seq_program);
		cur_instr != NULL;
		cur_instr = STAILQ_NEXT (cur_instr, links), instrcount++
	)
	{
		if (check_patch (&cur_patch, instrcount, &skip_addr, func_values) == 0)
		{
			/*
			 * Don't count this instruction as it is in a patch
			 * that was removed.
			 */
			continue;
		}

		while (line < cur_instr->srcline)
		{
			fgets (buf, sizeof (buf), ifile);
			fprintf (listfile, "\t\t%s", buf);
			line++;
		}

		fprintf
		(	listfile,
			"%03X %02X%02X%02X%02X", instrptr,
			cur_instr->format.bytes[0],
			cur_instr->format.bytes[1],
			cur_instr->format.bytes[2],
			cur_instr->format.bytes[3]
		);

		fgets (buf, sizeof (buf), ifile);
		fprintf (listfile, "\t%s", buf);
		line++;
		instrptr++;
	}

	/* Dump the remainder of the file */

	while (fgets (buf, sizeof (buf), ifile) != NULL)
		fprintf (listfile, "\t\t%s", buf);

	fclose (ifile);

}	/* end output_listing */

/*
 ****************************************************************
 *	Verifica as corre??es					*
 ****************************************************************
 */
int
check_patch (patch_t ** start_patch, int start_instr, int *skip_addr, int *func_vals)
{
	patch_t	*cur_patch;

	cur_patch = *start_patch;

	while (cur_patch != NULL && start_instr == cur_patch->begin)
	{
		if (func_vals[cur_patch->patch_func] == 0)
		{
			int		skip;

			/* Start rejecting code */

			*skip_addr = start_instr + cur_patch->skip_instr;

			for
			(	skip = cur_patch->skip_patch;
				skip > 0 && cur_patch != NULL;
				skip--
			)
			{
				cur_patch = STAILQ_NEXT (cur_patch, links);
			}
		}
		else
		{
			/*
			 * Accepted this patch.  Advance to the next one and
			 * wait for our intruction pointer to hit this point.
			 */
			cur_patch = STAILQ_NEXT (cur_patch, links);
		}
	}

	*start_patch = cur_patch;

	if (start_instr < *skip_addr) 		/* Still skipping */
		return (0);

	return (1);

}	/* end check_patch */

/*
 ****************************************************************
 *	Emite mensagens de erro e encessar o programa		*
 ****************************************************************
 */
void
stop (const char *string, int err_code)
{
	if (string != NULL)
	{
		fprintf (stderr, "%s: ", appname);

		if (yyfilename != NULL)
		{
			fprintf
			(	stderr,
				"Stopped at file %s, line %d - ",
				 yyfilename, yylineno
			);
		}

		fprintf (stderr, "%s\n", string);
	}

	if (ofile != NULL)
	{
		fclose (ofile);

		if (err_code != 0)
		{
			fprintf
			(	stderr,
				"%s: Removing %s due to error\n",
				 appname, ofilename
			);
			unlink (ofilename);
		}
	}

	if (regfile != NULL)
	{
		fclose (regfile);

		if (err_code != 0)
		{
			fprintf
			(	stderr,
				"%s: Removing %s due to error\n",
				 appname, regfilename
			);
			unlink (regfilename);
		}
	}

	if (listfile != NULL)
	{
		fclose (listfile);

		if (err_code != 0)
		{
			fprintf
			(	stderr,
				"%s: Removing %s due to error\n",
				 appname, listfilename
			);
			unlink (listfilename);
		}
	}

	symlist_free (&patch_functions);
	symtable_close ();

	exit (err_code);

}	/* end stop */

/*
 ****************************************************************
 *	Aloca espa?o para uma instru??o				*
 ****************************************************************
 */
struct instruction *
seq_alloc (void)
{
	struct instruction *new_instr;

	new_instr = (struct instruction *) malloc (sizeof (struct instruction));

	if (new_instr == NULL)
		stop ("Unable to malloc instruction object", EX_SOFTWARE);

	memset (new_instr, 0, sizeof (*new_instr));
	STAILQ_INSERT_TAIL (&seq_program, new_instr, links);
	new_instr->srcline = yylineno;
	return new_instr;

}	/* end seq_alloc */

/*
 ****************************************************************
 *	Aloca uma se??o cr?tica					*
 ****************************************************************
 */
critical_section_t *
cs_alloc (void)
{
	critical_section_t *new_cs;

	new_cs = (critical_section_t *) malloc (sizeof (critical_section_t));

	if (new_cs == NULL)
		stop ("Unable to malloc instruction object", EX_SOFTWARE);

	memset (new_cs, 0, sizeof (*new_cs));

	TAILQ_INSERT_TAIL (&cs_tailq, new_cs, links);
	return new_cs;

}	/* end cs_alloc */

/*
 ****************************************************************
 *	Aloca um escopo						*
 ****************************************************************
 */
scope_t	*
scope_alloc (void)
{
	scope_t	*new_scope;

	new_scope = (scope_t *)malloc (sizeof (scope_t));

	if (new_scope == NULL)
		stop ("Unable to malloc instruction object", EX_SOFTWARE);

	memset (new_scope, 0, sizeof (*new_scope));

	TAILQ_INIT (&new_scope->inner_scope);

	if (SLIST_FIRST (&scope_stack) != NULL)
	{
		TAILQ_INSERT_TAIL (&SLIST_FIRST (&scope_stack)->inner_scope,
					new_scope, scope_links);
	}

	/* This patch is now the current scope */

	SLIST_INSERT_HEAD (&scope_stack, new_scope, scope_stack_links);
	return new_scope;

}	/* end scope_alloc */

/*
 ****************************************************************
 *	Processa o escopo					*
 ****************************************************************
 */
void
process_scope (scope_t * scope)
{
	/*
	 *	We are "leaving" this scope.  We should now have enough
	 *	information to process the lists of scopes we encapsulate.
	 */
	scope_t		*cur_scope;
	unsigned	skip_patch_count;
	unsigned	skip_instr_count;

	cur_scope = TAILQ_LAST (&scope->inner_scope, scope_tailq);
	skip_patch_count = 0;
	skip_instr_count = 0;

	while (cur_scope != NULL)
	{
		unsigned		patch0_patch_skip;

		patch0_patch_skip = 0;

		switch (cur_scope->type)
		{
		    case SCOPE_IF:
		    case SCOPE_ELSE_IF:
			if (skip_instr_count != 0)
			{
				/* Create a tail patch */
				patch0_patch_skip++;
				cur_scope->patches[1].skip_patch = skip_patch_count + 1;
				cur_scope->patches[1].skip_instr = skip_instr_count;
			}

			/* Count Head patch */

			patch0_patch_skip++;

			/* Count any patches contained in our inner scope */

			patch0_patch_skip += cur_scope->inner_scope_patches;

			cur_scope->patches[0].skip_patch = patch0_patch_skip;
			cur_scope->patches[0].skip_instr = cur_scope->end_addr - cur_scope->begin_addr;

			skip_instr_count += cur_scope->patches[0].skip_instr;

			skip_patch_count += patch0_patch_skip;

			if (cur_scope->type == SCOPE_IF)
			{
				scope->inner_scope_patches += skip_patch_count;
				skip_patch_count = 0;
				skip_instr_count = 0;
			}
			break;

		    case SCOPE_ELSE:
			/* Count any patches contained in our innter scope */

			skip_patch_count += cur_scope->inner_scope_patches;

			skip_instr_count += cur_scope->end_addr - cur_scope->begin_addr;
			break;

		    case SCOPE_ROOT:
			stop ("Unexpected scope type encountered", EX_SOFTWARE);

		}	/* end switch */

		cur_scope = TAILQ_PREV (cur_scope, scope_tailq, scope_links);

	}	/* end while */

}	/* end process_scope */
