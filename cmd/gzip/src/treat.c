/*
 ****************************************************************
 *								*
 *			treat.c					*
 *								*
 *	Processa os arquivos					*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.0.0, de 05.06.93				*
 *								*
 *	Módulo: GZIP						*
 *		Utilitários de compressão/descompressão		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/times.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <ftw.h>
#include <signal.h>
#include <errno.h>

#include "../h/gzip.h"
#include "../h/lzw.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define RW_USER 0600    /* creation mode for open() */

#define MAX_PATH_LEN	1024 /* max pathname length */

/*
 ******	Variáveis locais ****************************************
 */
extern int	exit_code;	/* program exit code */
extern int	save_orig_name; /* set if original name must be saved */
extern int	last_member;	/* set for .zip and .Z files */
extern int	part_nb;	/* number of parts in .z file */
extern ulong	time_stamp;	/* original time stamp (modification time) */
extern long	ifile_size;	/* input file size, -1 for devices (debug only) */

extern long	bytes_in;	/* number of input bytes */
extern long	bytes_out;	/* number of output bytes */
extern char	ifname[];	/* input filename */
extern char	ofname[];	/* output filename */
extern int	remove_ofname;	/* remove output file on error */
extern STAT	istat;		/* status for input file */
extern int	ifd;		/* input file descriptor */
extern int	ofd;		/* output file descriptor */
extern unsigned	insize;		/* valid bytes in inbuf */
extern unsigned	inptr;		/* index of next byte to be processed in inbuf */
extern void	(*work) (int infile, int outfile); /* function to call */

/*
 ******	Protótipos de funções ***********************************
 */
int	treat_file (const char *, STAT *, int);
int	create_outfile (void);
char	*get_suffix (char *);
int	get_istat (const char *, STAT *);
int	make_ofname (void);
int	name_too_long(char *, STAT *);
int	get_method (int);
int	check_ofname(void);
void	copy_stat (STAT *);
char	*get_compress_method (int);

/*
 ****************************************************************
 *	Trata a entrada padrão					*
 ****************************************************************
 */
void
treat_stdin (void)
{
	strcpy (ifname, "stdin");
	strcpy (ofname, "stdout");

	/*
	 *	Obtém o tempo do arquivo de entrada
	 */
	if (fstat (fileno (stdin), &istat) < 0)
		gzip_error ("fstat (stdin)");

	time_stamp = istat.st_mtime;

	/*
	 *	Tamanho ainda desconhecido
	 */
	ifile_size = -1;

	clear_bufs ();
	cflag = 1;
	part_nb = 0;

	if (dflag)
	{
		if ((method = get_method (ifd)) < 0)
			exit (exit_code);
	}

	/*
	 *	Actually do the compression/decompression.
	 *	Loop over zipped members.
	 */
	for (EVER)
	{
		(*work) (fileno (stdin), fileno (stdout));

		if (!dflag || last_member || inptr == insize)
			break;
		/* end of file */

		if ((method = get_method (ifd)) == -1)
			return;

		bytes_out = 0;		/* required for length check */
	}

	if (vflag)
	{
		if   (tflag)
		{
			fprintf (stderr, " OK");

		}
		elif (!dflag)
		{
			fprintf (stderr, "Compressão: ");
			display_ratio (bytes_in-bytes_out-overhead, bytes_in);
		}

		fprintf (stderr, "\n");
	}

}	/* end treat_stdin */

/*
 ****************************************************************
 *	Processa um nome					*
 ****************************************************************
 */
void
treat_name (const char *name)
{
 	if (Rflag)
		ftw (name, treat_file);
	else
		treat_file (name, (STAT *)NULL, 0);	 /* PROVISÓRIO */

}	/* end treat_name */

/*
 ****************************************************************
 *	Comprime ou descomprime o arquivo dado			*
 ****************************************************************
 */
int
treat_file (const char *iname, STAT *sp, int type)
{
	sp = sp; type = type; 	 /* PROVISÓRIO */

	/*
	 *	Check if the input file is present, set ifname and istat
	 */
	if (get_istat (iname, &istat) != 0)
		return (0);

	/* If the input name is that of a directory, recurse or ignore: */

	if (S_ISDIR (istat.st_mode))
	{
		if (!Rflag)
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" é um diretório -- ignorado\n",
				pgname, ifname
			);

		}

		return (0);
	}

	if (!S_ISREG (istat.st_mode))
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" não é regular nem um diretório -- ignorado\n",
			pgname, ifname
		);
		return (0);
	}

	if (istat.st_nlink > 1 && !cflag && !fflag)
	{
		fprintf
		(	stderr, "%s: O arquivo \"%s\" tem %d outros \"link\"%c -- não modificado\n",
			pgname, ifname,
			istat.st_nlink - 1, istat.st_nlink > 2 ? 's' : ' '
		);
		return (0);
	}

	ifile_size = istat.st_size;
	time_stamp = istat.st_mtime;

	/*
	 *	Generate output file name
	 */
	if   (cflag)
		strcpy (ofname, "stdout");
	elif (make_ofname () != 0)
		return (0);

	/*
	 *	Open the input file and determine compression method.
	 *	The mode parameter is ignored but required by some
	 *	systems (VMS).
	 */
	if ((ifd = open (ifname, O_RDONLY, RW_USER)) < 0)
	{
		perror (ifname);
		exit_code = 1;
		return (0);
	}

	clear_bufs ();
	part_nb = 0;

	/*
	 *	"method" updates ofname if original given
	 */
	if (dflag)
	{
		if ((method = get_method (ifd)) < 0)
		{
			close (ifd);
			return (0);
		}
	}

	/*
	 *	If compressing to a file, check if ofname is not ambiguous
	 *	because the operating system truncates names. Otherwise,
	 *	generate a new ofname and save the original name in the
	 *	compressed file.
	 */
	if (cflag)
	{
		ofd = fileno (stdout);
			/* keep remove_ofname as zero */
	}
	else
	{
		if (create_outfile () == -1)
			return (0);

#if (0)	/*******************************************************/
		if (save_orig_name && !vflag)
		{
			fprintf
			(	stderr, "%s: %s compressed to %s\n",
				pgname, ifname, ofname
			);
		}
#endif	/*******************************************************/
	}

	if (vflag)
	{
		fprintf
		(	stderr,
			"%s:\t%s", ifname,
			strlen (ifname) >= 15 ? "" : (strlen(ifname) >= 7 ? "\t" : "\t\t")
		);
	}

	/*
	 *	Actually do the compression/decompression.
	 *	Loop over zipped members.
	 */
	for (EVER)
	{
		(*work) (ifd, ofd);

		if (!dflag || last_member || inptr == insize)
			break;
		/* end of file */

		if ((method = get_method (ifd)) < 0)
			break;

		bytes_out = 0;		/* required for length check */
	}

	close (ifd);

	if (!cflag && close (ofd))
		write_error ();

	if (method == -1)
		return (0);	 /* error, don't display success msg */

	/* Display statistics */

	if (vflag)
	{
		if (!dflag)
			display_ratio (bytes_in-bytes_out-overhead, bytes_in);

		if   (tflag)
			fprintf (stderr, " OK");
		elif (!cflag)
			fprintf (stderr, " -- substituído por \"%s\"", ofname);

		if (dflag)
			fprintf (stderr, " (%s)\n", get_compress_method (method));
		else
			fprintf (stderr, "\n");
	}

	/* Copy modes, times, ownership */

	if (!cflag)
		copy_stat (&istat);

	return (0);

}	/* end treat_file */

/*
 ****************************************************************
 *	Cria o arquivo de saída					*
 ****************************************************************
 */
/* ========================================================================
 * Create the output file. Return 0 for success, -1 for error.
 * Try twice if ofname is exactly one beyond the name limit, to avoid
 * creating a compressed file of name "1234567890123."
 * We could actually loop more than once if the user gives an extra long
 * name, but I prefer generating an error then. (Posix forbids the system
 * to truncate names.) The error message is generated by check_ofname()
 * in this case.
 * IN assertions: the input file has already been open (ifd is set) and
 *   ofname has already been updated if there was an original name.
 * OUT assertions: ifd and ofd are closed in case of error.
 */
int
create_outfile (void)
{
	STAT		ostat;	/* stat for ofname */
	int		n;	/* loop counter */

	for (n = 1; n <= 2; n++)
	{
		if (check_ofname () == -1)
		{
			close (ifd);
			return -1;
		}

		/*
		 *	Create the output file
		 */
		remove_ofname = 1;

		if ((ofd = open (ofname, O_WRONLY|O_CREAT|O_EXCL, RW_USER)) < 0)
		{
			perror (ofname);
			close (ifd);
			exit_code = 1;
			return -1;
		}

		/*
		 *	Check for name truncation on new file (1234567890123.z)
		 */
		if (fstat (ofd, &ostat) < 0)
		{
			fprintf (stderr, "%s: ", pgname);
			perror (ofname);
			close (ifd); close (ofd);
			unlink (ofname);
			exit_code = 1;
			return -1;
		}

		if (!name_too_long (ofname, &ostat))
			return 0;

		/*
		 *	Name might be too long if an original name was saved
		 */
		if (dflag)
		{
			fprintf
			(	stderr,
				"%s: %s: Advertência -- nome truncado\n",
				pgname, ofname
			);
			return 0;
		}
		else
		{
			close (ofd);
			unlink (ofname);
			save_orig_name = 1;
			strcpy (ofname + strlen (ofname) - 3 - 1, ".gz");
				/* 1234567890123.z -> 123456789012.z */
#if (0)	/*******************************************************/
			strcpy (ofname + strlen (ofname) - 2 - 1, ".z");
				/* 1234567890123.z -> 123456789012.z */
#endif	/*******************************************************/

		}	/* end if (dflag) ? */

	}	/* end for (n = ...) */

	close (ifd);
	fprintf (stderr, "%s: %s: Nome muito longo\n", pgname, ofname);
	exit_code = 1;
	return -1;

}	/* end create_outfile */

/*
 ****************************************************************
 *	Obtém o sufixo do nome de um arquivo			*
 ****************************************************************
 */
/* ========================================================================
 * Return a pointer to the 'z' suffix of a file name, or NULL.
 * For all systems, ".z", ".Z", ".taz", ".tgz", "-z" are accepted suffixes.
 * ".tgz" is a useful convention for tar.z files on systems limited
 * to 3 characters extensions. On such systems, ".?z" and ".??z" are
 * also accepted suffixes. For Unix, we do not want to accept any
 * .??z suffix as indicating a compressed file; some people use .xyz
 * to denote volume data.
 */
char *
get_suffix (char *name)
{
	int		 len;
	register char	 *p = strrchr (name,  '.');
	char		 suffix[10];		/* last few chars of name, forced to lower case */

	if (p == NOSTR || p == name || strchr (p - 1, '/') != NOSTR)
		return NOSTR;

	strncpy (suffix, p, sizeof (suffix));

	suffix[sizeof (suffix) - 1] = '\0';	/* Force null termination */

	strlwr (suffix);

	if
	(	streq (suffix, ".z")   || streq (suffix, ".zip") ||
		streq (suffix, ".tgz") || streq (suffix, ".taz") ||
		streq (suffix, ".gz")
	)
		return p;

	len = strlen (suffix);

	if (len <= 2)
		return NOSTR;

	if (streq (suffix + len - 2, "-z"))
		return (p + len - 2);

	return NOSTR;

}	/* end get_suffix */


/*
 ****************************************************************
 *	Obtém o nome do arquivo e o seu estado			*
 ****************************************************************
 */
/* ========================================================================
 * Set ifname to the input file name (with .z appended if necessary)
 * and istat to its stats. Return 0 if ok, -1 if error.
 */
int
get_istat (const char *iname, STAT *sbuf)
{
	int		 iexists;	/* set if iname exists */
	int		 ilen = strlen (iname);
	char		 *suff;

	strcpy (ifname, iname);
	errno = 0;

	/* If input file exists, return OK. */

	if (stat (ifname, sbuf) == 0)
		return 0;

	if (!dflag || errno != ENOENT)
	{
		perror (ifname);
		exit_code = 1;
		return -1;
	}

	/*
	 *	"file.ext" doesn't exist, try file.ext.z and file.ext.Z.
	 *	For MSDOS try file.exz, for VMS try file.ext-z.
	 */
	if ((suff = get_suffix (ifname)) != NOSTR)
	{
		perror (ifname); /* ifname already has z suffix and does not exist */
		exit_code = 1;
		return -1;
	}

	strcat (ifname, ".gz");
	ilen += 3;
#if (0)	/*******************************************************/
	if (strrchr (ifname, '.') != NOSTR)
	{
		strcat (ifname, ".z");
		ilen += 2;
	}
	else
	{
		strcat (ifname, ".z");
		ilen += 2;
	}
#endif	/*******************************************************/

	errno = 0;

	iexists = (stat (ifname, sbuf) >= 0);

	if (!iexists)
	{
		errno = 0;
		ifname[ilen-1] = 'Z';
		iexists = !stat (ifname, sbuf);
	}

	if (!iexists)
	{
		ifname[ilen-1] = 'z';
		perror(ifname);
		exit_code = 1;
		return -1;
	}

	if (!S_ISREG (sbuf->st_mode))
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" não é regular -- ignorado\n",
			pgname, ifname
		);
		return -1;
	}

	return 0; /* ok */

}	/* end get_istat */

/*
 ****************************************************************
 *	Obtém o nome do método de compressão			*
 ****************************************************************
 */
char *
get_compress_method (int i)
{
	switch (i)
	{
	    case STORED:
	   /***	return ("store"); ***/
		return ("armazenamento");

	    case COMPRESSED:
	   /***	return ("compress"); ***/
		return ("compressão");

	    case PACKED:
	   /***	return ("pack"); ***/
		return ("empacotamento");

	    case DEFLATED:
	   /***	return ("deflate"); ***/
		return ("deflação");

	    default:
		return ("desconhecido");
	}

}	/* enb get_compress_method */
