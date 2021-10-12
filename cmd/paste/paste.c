/*
 ****************************************************************
 *								*
 *			paste.c					*
 *								*
 *	Une linhas de vários arquivos				*
 *								*
 *	Versão	3.2.2, de 20.07.99				*
 *		3.2.3, de 20.11.99				*
 *								*
 *	Módulo:	paste						*
 *		Utilitários Básicos				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definicões globais				*
 ****************************************************************
 */
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	MAXSZ	1024

entry const char	pgversion[] = "Versão: 3.2.3, de 20.11.99";

/*
 ****** Protótipos de funcões *******************************
 */
void		error (const char *format, ...);
void		help (void);

/*
 ****************************************************************
 *	Une linhas de vários arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	STAT		s;
	int		i, n, opt, len;
	int		eof = 0;
	FILE		*fp, **fp_vec, **fp_ptr;
	FILE		*stdin_copy_fp = NOFILE;
	int		stdin_max_len = 0, file_max_len;
	int		max_len = 0;
	int		*max_len_vec, *max_len_ptr;
	char		*cp, bad_file = 0;
	const char	*delimiter_str = "\t", *file_nm;
	int		delimiter_len;
	char		area[MAXSZ];
	char		line[MAXSZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "d:H")) != EOF)
	{
		switch (opt)
		{
		    case 'd':
			delimiter_str = optarg;
			break;

		    case 'H':
			help ();

		    default:
			putc('\n', stderr);
			help ();

		}	/* end switch */
	}

	/*
	 *	Pequena consistência
	 */
	argv += optind;
	argc -= optind;

	if (argc <= 0)
		return (0);

	delimiter_len = strlen (delimiter_str);

	/*
	 *	Reserva memória para os ponteiros de arquivos e tamanhos máximos das áreas
	 */
	if ((fp_ptr = fp_vec = malloc (argc * sizeof (FILE *))) == (FILE **)NULL)
		error ("$Memória esgotada");

	if ((max_len_ptr = max_len_vec = malloc (argc * sizeof (int))) == (int *)NULL)
		error ("$Memória esgotada");

	memsetl (max_len_vec, 0, argc);

	/*
	 *	Examina os arquivos
	 */
	for (/* vazio */; file_nm = *argv; argv++, fp_ptr++, max_len_ptr++)
	{
		if   (streq (file_nm, "-"))
		{
			if (stdin_copy_fp != NOFILE)
			{
				*fp_ptr = stdin_copy_fp;
				*max_len_ptr = stdin_max_len;
				max_len += (stdin_max_len + delimiter_len);
				continue;
			}

			if ((stdin_copy_fp = *fp_ptr = tmpfile ()) == NOFILE)
				error ("$*Não consegui criar um arquivo temporário");

			/*
			 *	Copia a entrada padrão
			 */
			while (fngets (area, sizeof (area), stdin) != NOSTR)
			{
				fnputs (area, stdin_copy_fp);

				len = strlen (area);

				if (stdin_max_len < len)
					stdin_max_len = len;
			}

			*max_len_ptr = stdin_max_len;
			max_len += (stdin_max_len + delimiter_len);

			rewind (stdin_copy_fp);
		}
		elif (stat (file_nm, &s) < 0)
		{
			bad_file++; error ("*Não consegui obter o estado de \"%s\"", file_nm);
		}
		elif (!S_ISREG (s.st_mode))
		{
			bad_file++; error ("*O arquivo \"%s\" não é regular", file_nm);
		}
		elif ((fp = *fp_ptr = fopen (file_nm, "r")) == NOFILE)
		{
			bad_file++; error ("*Não consegui abrir \"%s\"", file_nm);
		}
		else	/* Lê o arquivo para encontrar a linha mais longa */
		{
			file_max_len = 0;

			while (fngets (area, sizeof (area), fp) != NOSTR)
			{
				len = strlen (area);

				if (file_max_len < len)
					file_max_len = len;
			}

			*max_len_ptr = file_max_len;
			max_len += (file_max_len + delimiter_len);

			rewind (fp);
		}

	}	/* end for de verificação e leitura dos arquivos */

	/*
	 *	Verifica se houve erros
	 */
	if (bad_file)
		exit (1);

	/*
	 *	Verifica se o tamanho da linha não ultrapassa a área
	 *	Repare que precisa de um byte para o '\0'
	 */
#undef	DEBUG
#ifdef	DEBUG
	fprintf (stderr, "max_len = %d\n", max_len - delimiter_len);
#endif	DEBUG

	if (max_len - delimiter_len >= MAXSZ)
		error ("$A linha gerada tem mais de " XSTR (MAXSZ) " bytes");

	/*
	 *	Une as diversas linhas
	 */
	for (EVER)
	{
		fp_ptr = fp_vec; max_len_ptr = max_len_vec;

		cp = &line[0];

		for (i = argc; /* abaixo */; fp_ptr++, max_len_ptr++)
		{
			if   ((fp = *fp_ptr) == NOFILE)
			{
				len = 0;
			}
			elif (fngets (area, sizeof (area), fp) == NOSTR)
			{
				len = 0; *fp_ptr = NOFILE; eof++;
			}
			else
			{
				len = strlen (area); memmove (cp, area, len);
			}

			cp += len;

			/* Verifica se foi o último */

			if (--i <= 0)
				break;

			/*
			 *	Coloca brancos para terminar o campo
			 */
			if ((n = *max_len_ptr - len) > 0)
				{ memset (cp, ' ', n); cp += n; }

			/* Insere o delimitador */

			memmove (cp, delimiter_str, delimiter_len); cp += delimiter_len;
		}

		if (eof >= argc)
			break;

		*cp = '\0';

		fnputs (line, stdout);
	}

	return (0);

}	/* end main */ 

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Imprime uma mensagem de erro				*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	args;
	register const char	*fmt = format;
	char			err, abrt;
	int			save_errno = errno;

	va_start (args, format);

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*' && save_errno != 0)
		fprintf (stderr, " (%s)", strerror (save_errno));

	putc ('\n', stderr);

	if (abrt == '$')
		exit (1);

	va_end (args);

}	/* end error */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Ajuda do programa					*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - une linhas de vários arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-d <cadeia>] <arquivo> ...\n",
		pgname, pgversion, pgname, pgname
	);

	fprintf
	(	stderr,
		"\nOpcões:"
		"\t-d: Separa os campos com <cadeia> ao invés de <ht>\n"
		"\nObs.:\tPode ser dado \"-\" uma ou mais vezes, para representar <stdin>\n"
	);
		
	exit (1);

}	/* end help */
