/*
 ****************************************************************
 *								*
 *			top/top-main.c				*
 *								*
 *	This is the top level of cc1.				*
 * 	It parses command args, opens files, invokes the	*
 *	various passes in the proper order, and counts the	*
 *	time used by each. Error messages and low-level		*
 *	interface to malloc also handled here.			*
 *								*
 *	Versão	3.0.0, de 26.08.93				*
 *		3.0.0, de 09.01.94				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

/*
 ******	Inclusões ***********************************************
 */
#pragma	getenv (GEN)

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include ../$GEN.gen/config.h

#include "../h/tree.h"
#include "../h/flags.h"

#include "top-data.h"
#include "top.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
const char	**color_vector;	/* Para as cores dos modos */ 

/*
 ****************************************************************
 *	Entry point of cc1					*
 ****************************************************************
 */
/*
 *	Entry point of cc1. Decode command args, then call
 *	compile_file. Exit code is 35 if can't open files,
 *	34 if fatal error, 33 if had nonfatal errors, else success
 */
int
main (int argc, char *argv[])
{
	register int	i;
	char		*filename = NOSTR;
	int		print_mem_flag = 0;
	char		*p;

	/*
	 *	save in case md file wants to emit args as a comment
	 */
	save_argc = argc;
	save_argv = argv;

	p = argv[0] + strlen (argv[0]);

	while (p != argv[0] && p[-1] != '/')
		--p;

	progname = p;

#ifdef RLIMIT_STACK
	/*
	 *	Get rid of any avoidable limit on stack size
	 */
	{
		struct rlimit	rlim;

		/*
		 *	Set the stack limit huge so that alloca does not fail
		 */
		getrlimit (RLIMIT_STACK, &rlim);
		rlim.rlim_cur = rlim.rlim_max;
		setrlimit (RLIMIT_STACK, &rlim);
	}
#endif /* RLIMIT_STACK */

	signal (SIGFPE,  float_signal);
	signal (SIGPIPE, pipe_closed);

	/*
	 *	Initialize whether `char' is signed
	 */
	flag_signed_char = DEFAULT_SIGNED_CHAR;

#ifdef DEFAULT_SHORT_ENUMS
	/*
	 *	Initialize how much space enums occupy, by default
	 */
	flag_short_enums = DEFAULT_SHORT_ENUMS;
#endif

	/*
	 *	This is zeroed by -O
	 */
	obey_regdecls = 1;

	/*
	 *	Initialize register usage now so switches may override
	 */
	init_reg_sets ();

	target_flags = 0;
	set_target_switch ("");

	for (i = 1; i < argc; i++)
	{
	    if (argv[i][0] == '-' && argv[i][1] != 0)
	    {
		register char	*str = argv[i] + 1;

		if (str[0] == 'M')	/* Opção "-M" */
			exit (0);

		if (str[0] == 'Y')
			str++;

		if (str[0] == 'm')
		{
			set_target_switch (&str[1]);
		}
		elif (!strcmp (str, "dumpbase"))
		{
			dump_base_name = argv[++i];
		}
		elif (str[0] == 'd')
		{
			register char *p = &str[1];

			while (*p) switch (*p++)
			{
			    case 'c':
				combine_dump = 1;
				break;

			    case 'd':
				dbr_sched_dump = 1;
				break;

			    case 'f':
				flow_dump = 1;
				break;

			    case 'g':
				global_reg_dump = 1;
				break;

			    case 'j':
				jump_opt_dump = 1;
				break;

			    case 'J':
				jump2_opt_dump = 1;
				break;

			    case 'l':
				local_reg_dump = 1;
				break;

			    case 'L':
				loop_dump = 1;
				break;

			    case 'm':
				print_mem_flag = 1;
				break;

			    case 'r':
				rtl_dump = 1;
				break;

			    case 's':
				cse_dump = 1;
				break;

			    case 'y':
				yydebug = 1;
				break;
			}
		}
		elif (str[0] == 'f')
		{
			int		j;
			register char	*p = &str[1];
			int		found = 0;

			/*
			 *	Some kind of -f option.
			 *	P's value is the option sans `-f'.
			 *	Search for it in the table of options
			 */
			for (j = 0; !found && f_options[j].string != NOSTR; j++)
#if (0)	/*************************************/
			(j = 0;
			 !found && j < sizeof (f_options) / sizeof (f_options[0]);
			 j++)
#endif	/*************************************/
			{
				if (!strcmp (p, f_options[j].string))
				{
					*f_options[j].variable = f_options[j].on_value;

					/*
					 *	A goto here would be cleaner,
					 *	but breaks the vax pcc
					 */
					found = 1;
				}

				if
				(	p[0] == 'n' && p[1] == 'o' &&
					p[2] == '-' &&
					! strcmp (p+3, f_options[j].string)
				)
				{
					*f_options[j].variable = ! f_options[j].on_value;
					found = 1;
				}
			}

			if   (found)
				/* vazio */;
			elif (!strncmp (p, "fixed-", 6))
				fix_register (&p[6], 1, 1);
			elif (!strncmp (p, "call-used-", 10))
				fix_register (&p[10], 0, 1);
			elif (!strncmp (p, "call-saved-", 11))
				fix_register (&p[11], 0, 0);
			elif (! lang_decode_option (argv[i]))
				error ("Invalid option `%s'", argv[i]);
		}
		elif (!strcmp (str, "noreg"))
		{
			/* vazio */;
		}
		elif (!strcmp (str, "opt"))
		{
			optimize = 1, obey_regdecls = 0;
		}
		elif (!strcmp (str, "O"))
		{
			optimize = 1, obey_regdecls = 0;
		}
		elif (!strcmp (str, "O1") || !strcmp (str, "O2"))
		{
			/*
			 *	Accept -O1 and -O2 for compatibility
			 *	with version 2
			 */
			optimize = 1, obey_regdecls = 0;
		}
		elif (!strcmp (str, "O0"))
		{
			optimize = 0, obey_regdecls = 1;
		}
		elif (!strcmp (str, "pedantic"))
		{
			pedantic = 1;
		}
		elif (lang_decode_option (argv[i]))
		{
			/* vazio */;
		}
		elif (!strcmp (str, "quiet"))
		{
			quiet_flag = 1;
		}
		elif (!strcmp (str, "version"))
		{
			extern char	*version_string, *language_string;

			fprintf (stderr, "%s version %s", language_string, version_string);
#ifdef TARGET_VERSION
			TARGET_VERSION;
#endif
#ifdef __GNUC__
#ifndef __VERSION__
#define __VERSION__ "[unknown]"
#endif
			fprintf (stderr, " compiled by GNU C version %s.\n", __VERSION__);
#else
			fprintf (stderr, " compiled by CC.\n");
#endif
			print_target_switch_defaults ();
		}
		elif (!strcmp (str, "w"))
		{
			inhibit_warnings = 1;
		}
		elif (!strcmp (str, "W"))
		{
			extra_warnings = 1;
		}
		elif (!strcmp (str, "Wunused"))
		{
			warn_unused = 1;
		}
		elif (!strcmp (str, "Wshadow"))
		{
			warn_shadow = 1;
		}
		elif (!strcmp (str, "Wswitch"))
		{
			warn_switch = 1;
		}
		elif (!strncmp (str, "Wid-clash-", 10))
		{
			char	*endp = str + 10;

			while (*endp)
			{
				if (*endp >= '0' && *endp <= '9')
					endp++;
				else
					error ("Invalid option `%s'", argv[i]);
			}

			warn_id_clash = 1;
			id_clash_len = atoi (str + 10);
		}
		elif (!strcmp (str, "p"))
		{
			profile_flag = 1;
		}
		elif (!strcmp (str, "a"))
		{
#if !defined (BLOCK_PROFILER) || !defined (FUNCTION_BLOCK_PROFILER)
			warning ("`-a' option (basic block profile) not supported");
#else
			profile_block_flag = 1;
#endif
		}
		elif (!strcmp (str, "gg"))
		{
			write_symbols = GDB_DEBUG;
		}
#ifdef DBX_DEBUGGING_INFO
		elif (!strcmp (str, "g0"))
		{
			write_symbols = DBX_DEBUG;
		}
		elif (!strcmp (str, "G0"))
		{
			write_symbols = DBX_DEBUG;
		}
		elif (!strcmp (str, "g"))
		{
			write_symbols = DBX_DEBUG;
			use_gdb_dbx_extensions = 1;
		}
		elif (!strcmp (str, "G"))
		{
			write_symbols = DBX_DEBUG;
			use_gdb_dbx_extensions = 1;
		}
#endif
#ifdef SDB_DEBUGGING_INFO
		elif (!strcmp (str, "g"))
		{
			write_symbols = SDB_DEBUG;
		}
		elif (!strcmp (str, "G"))
		{
			write_symbols = SDB_DEBUG;
		}
		elif (!strcmp (str, "g0"))
		{
			write_symbols = SDB_DEBUG;
		}
		elif (!strcmp (str, "G0"))
		{
			write_symbols = SDB_DEBUG;
		}
#endif
		elif
		(	!strcmp (str, "g") || !strcmp (str, "G") ||
			!strcmp (str, "g0") || !strcmp (str, "G0")
		)
		{
			warning ("`%s' not supported on this system", str);
		}
		elif (!strcmp (str, "symout"))
		{
			if (write_symbols == NO_DEBUG)
				write_symbols = GDB_DEBUG;

			sym_file_name = argv[++i];
		}
		elif (!strcmp (str, "o"))
		{
			asm_file_name = argv[++i];
		}
		else
			error ("Invalid option `%s'", argv[i]);
	    }
	    else
	    {
			filename = argv[i];
	    }
	}

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Some machines may reject certain combinations of options
	 */
#ifdef OVERRIDE_OPTIONS
	OVERRIDE_OPTIONS;
#endif

	/*
	 *	Now that register usage is specified,
	 *	convert it to HARD_REG_SETs
	 */
	init_reg_sets_1 ();

	compile_file (filename);

#ifndef USG
#ifndef VMS
	if (print_mem_flag)
	{
		extern char	**environ;
		char		*lim = (char *) sbrk (0);

		fprintf (stderr, "Data size %d.\n", (int) lim - (int) &environ);
		fflush (stderr);

		system ("ps v");
	}
#endif /* not VMS */
#endif /* not USG */

	if (errorcount)
		exit (FATAL_EXIT_CODE);

	if (sorrycount)
		exit (FATAL_EXIT_CODE);

	exit (SUCCESS_EXIT_CODE);

	return (34);

}	/* end main */
