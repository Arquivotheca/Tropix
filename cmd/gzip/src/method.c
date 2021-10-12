/*
 ****************************************************************
 *								*
 *			method.c				*
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
#include <signal.h>
#include <errno.h>

#include "../h/gzip.h"
#include "../h/lzw.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define MAX_PATH_LEN   1024 /* max pathname length */

#if (0)	/*******************************************************/
/*
 ******	Global buffers ******************************************
 */
extern char	*inbuf;
#endif	/*******************************************************/

/*
 ******	Variáveis locais ****************************************
 */
extern int	exit_code;   /* program exit code */
extern int	save_orig_name;   /* set if original name must be saved */
extern int	last_member;	  /* set for .zip and .Z files */
extern int	part_nb;		  /* number of parts in .z file */
extern ulong	time_stamp;	   /* original time stamp (modification time) */
extern long	ifile_size;	  /* input file size, -1 for devices (debug only) */
extern int	foreground;	   /* set if program run in foreground */

extern long	bytes_in;			 /* number of input bytes */
extern long	bytes_out;			/* number of output bytes */
extern char	ifname[]; 		/* input filename */
extern  char	ofname[MAX_PATH_LEN];	/* output filename */
extern int	remove_ofname;	   /* remove output file on error */
extern STAT	istat;		 /* status for input file */
extern int	ifd;				  /* input file descriptor */
extern int	ofd;				  /* output file descriptor */
extern unsigned	insize;		   /* valid bytes in inbuf */
extern unsigned	inptr;			/* index of next byte to be processed in inbuf */
extern unsigned	outcnt;		   /* bytes in output buffer */
extern void	(*work) (int infile, int outfile); /* function to call */

/*
 ******	Protótipos de funções ***********************************
 */
char	*get_suffix (char *);
int	name_too_long(char *, STAT *);
int	get_method (int);
int	check_ofname (void);
void	reset_times (char *, STAT *);
void	copy_stat (STAT *);

extern int	isatty (int);

/*
 ****************************************************************
 *	Gera o nome de saída, dado o de entrada			*
 ****************************************************************
 */
/* ========================================================================
 * Generate ofname given ifname. Return 0 if ok, -1 if file must be skipped.
 * Initializes save_orig_name.
 * IN assertion: this function is not called if cflag is true.
 */
int
make_ofname (void)
{
	char		 *suff;		/* ofname z suffix */

	strcpy (ofname, ifname);
	suff = get_suffix (ofname);

	if   (dflag)	/* Descompressão */
	{
		if (suff == NOSTR)
		{
			fprintf
			(	stderr,
				"%s: %s: faltando o sufixo \"z\" -- ignorado\n",
				pgname, ifname
			);
			return -1;
		}

		/* Make a special case for .tgz and .taz: */

		strlwr (suff);

		if (streq (suff, ".tgz") || streq (suff, ".taz"))
		{
			strcpy (suff, ".tar");
		}
		else
		{
			/* strip z suffix and optional version number */
			*suff = '\0';
		}
		/* ofname might be changed later if infile contains an original name */

	}
	elif (suff != NOSTR)
	{
		/* Avoid annoying messages with -r (see treat_dir()) */

		if (vflag || !Rflag)
		{
			fprintf	
			(	stderr,
				"%s: %s já possui o sufixo \"%s\" -- inalterado\n",
				pgname, ifname, suff
			);
		}

		if (exit_code == 0)
			exit_code = WARNING;
		return -1;
	}
	else
	{
		save_orig_name = 1;	/* Sempre guarda o nome original */

		strcat (ofname, ".gz");
#if (0) /*******************************************************/
		save_orig_name = 0;

		strcat (ofname, ".z");
#endif  /*******************************************************/

	}	/* dflag ? */

	return 0;

}	/* end make_ofname */


/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
/* ========================================================================
 * Check the magic number of the input file and update ofname if an
 * original name was given and cflag is not set.
 * Return the compression method, -1 for error, -2 for warning.
 * Set inptr to the offset of the next byte to be processed.
 * This function may be called repeatedly for an input file consisting
 * of several contiguous gzip'ed members.
 * IN assertions: there is at least one remaining compressed member.
 *   If the member is a zip file, it must be the only one.
 */
int
get_method (int in /* input file descriptor */)
{
	char		 flags;
	char		 magic[2];	/* magic header */

	magic[0] = get_byte ();
	magic[1] = get_byte ();

	time_stamp = istat.st_mtime; /* may be modified later for some methods */
	method = -1;		/* unknown yet */
	part_nb++;		/* number of parts in gzip file */
	last_member = RECORD_IO;

	/* assume multiple members in gzip file except for record oriented I/O */

	if (!memcmp (magic, GZIP_MAGIC, 2) || !memcmp (magic, OLD_GZIP_MAGIC, 2))
	{
		work = unzip;
		method = get_byte ();
		flags  = get_byte ();

		if ((flags & ENCRYPTED) != 0)
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" está encriptado"
				" -- obtenha uma versão mais nova de %s\n",
				pgname, ifname, pgname
			);
			exit_code = 1;
			return -1;
		}

		if ((flags & CONTINUATION) != 0)
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" contém mais de uma parte"
				" -- obtenha uma versão mais nova de %s\n",
				pgname, ifname, pgname
			);
			exit_code = 1;

			if (fflag <= 1)
				return -1;
		}

		if ((flags & RESERVED) != 0)
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" contém os indicadores 0x%X"
				" -- obtenha a versão mais nova de %s\n",
				pgname, ifname, flags, pgname
			);
			exit_code = 1;
			if (fflag <= 1)
				return -1;
		}

		time_stamp  =  get_byte ();
		time_stamp |= (get_byte ()) << 8;
		time_stamp |= (get_byte ()) << 16;
		time_stamp |= (get_byte ()) << 24;

		(void)get_byte ();  /* Ignore extra flags for the moment */
		(void)get_byte ();  /* Ignore OS type for the moment */

		if ((flags & CONTINUATION) != 0)
		{
			unsigned	part = get_byte ();
		
			part |= (get_byte ()) << 8;
	
			if (vflag)
			{
				fprintf
				(	stderr,"%s: %s: Parte número %u\n",
					pgname, ifname, part
				);
			}
		}

		if ((flags & EXTRA_FIELD) != 0)
		{
			unsigned	len = get_byte ();
	
			len |= (get_byte ()) << 8;
	
			if (vflag)
			{
				fprintf
				(	stderr,
					"%s: %s: Campo adicional de %u bytes ignorado\n",
					pgname, ifname, len
				);
			}
	
			while (len--)
				get_byte ();
		}

		/* Get original file name if it was truncated */

		if ((flags & ORIG_NAME) != 0)
		{
			if (cflag || part_nb > 1)
			{
				/* Discard the old name */
	
				while (get_byte () != 0)
					/* null */ ;
			}
			else
			{
				/* Copy the base name. Keep a directory prefix intact. */
	
				char		*p = basename (ofname);
	
				for (EVER)
				{
					*p = get_byte ();
	
					if (*p++ == '\0')
						break;
	
					if (p >= ofname + sizeof (ofname))
					{
						gzip_error ("corrupted input -- file name too large");
					}
				}
	
			}	/* end cflag */
	
		}	/* end orig_name */

		/* Discard file comment if any */

		if ((flags & COMMENT) != 0)
		{
			while (get_byte () != 0)
				/* null */ ;
		}

	}
	elif (!memcmp (magic, PKZIP_MAGIC, 2) && inptr == 2 &&
			!memcmp(inbuf, PKZIP_MAGIC, 4))
	{
		/*
		 * To simplify the code, we support a zip file when alone only.
		 * We are thus guaranteed that the entire local header fits in inbuf.
		 */
		inptr = 0;
		work = unzip;
		if (check_zipfile (in) == -1)
			return -1;

		/* check_zipfile may get ofname from the local header */

		last_member = 1;

	}
	elif (!memcmp(magic, PACK_MAGIC, 2))
	{
		work = unpack;
		method = PACKED;
	}
	elif (!memcmp(magic, LZW_MAGIC, 2))
	{
		work = unlzw;
		method = COMPRESSED;
		last_member = 1;
	}

	if (method >= 0)
		return method;

	if (part_nb == 1)
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" não está no formato GZIP\n",
			pgname, ifname
		);

		exit_code = 1;
		return -1;
	}
	else
	{
		fprintf
		(	stderr,
			"%s: %s: Ignorando caracteres não identicados ao final\n",
			pgname, ifname
		);

		return -2;

	}

}	/* end get_method */

/*
 ****************************************************************
 *	Compara duas estruturas STAT				*
 ****************************************************************
 */
int
same_file (register STAT *stat1, register STAT *stat2)
{
	return stat1->st_mode  == stat2->st_mode
	&& stat1->st_ino   == stat2->st_ino
	&& stat1->st_dev   == stat2->st_dev
	&& stat1->st_uid   == stat2->st_uid
	&& stat1->st_gid   == stat2->st_gid
	&& stat1->st_size  == stat2->st_size
	&& stat1->st_atime == stat2->st_atime
	&& stat1->st_mtime == stat2->st_mtime
	&& stat1->st_ctime == stat2->st_ctime;

}	/* end same_file */

/*
 ****************************************************************
 *	Verifica se o SO truncou o nome				*
 ****************************************************************
 */
int
name_too_long (char *name, STAT *statb)
{
	int		s = strlen (name);
	char		c = name[s-1];
	STAT		tstat;		/* stat for truncated name */
	int		res;

	tstat = *statb;	  /* Just in case OS does not fill all fields */
	name[s-1] = '\0';
	res = stat (name, &tstat) >= 0 && same_file (statb, &tstat);
	name[s-1] = c;
	return (res);

}	/* end name_too_long */

/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
/* ========================================================================
 * If compressing to a file, check if ofname is not ambigous
 * because the operating system truncates names. Otherwise, generate
 * a new ofname and save the original name in the compressed file.
 * If the compressed file already exists, ask for confirmation.
 *	The check for name truncation is made dynamically, because different
 * file systems on the same OS might use different truncation rules (on SVR4
 * s5 truncates to 14 chars and ufs does not truncate).
 *	This function returns -1 if the file must be skipped, and
 * updates save_orig_name if necessary.
 * IN assertions: save_orig_name is already set if ofname has been
 * already truncated because of NO_MULTIPLE_DOTS. The input file has
 * already been open and istat is set.
 */
int
check_ofname (void)
{
	int		s = strlen (ofname);
	STAT		ostat;		 /* stat for ofname */

	if (stat (ofname, &ostat) < 0)
		return 0;

	/* Check for name truncation on existing file: */

	if (!dflag && s > 8 && name_too_long (ofname, &ostat))
	{
		save_orig_name = 1;

		strcpy (ofname + s - 5, ".gz"); /* 12345678901234.gz -> 12345678901.gz */
#if (0)	/*******************************************************/
		strcpy (ofname + s - 4, ".z"); /* 12345678901234.z -> 123456789012.z */
#endif	/*******************************************************/

		if (stat (ofname, &ostat) < 0)
			return 0;

	}	/* end !dflag && name_too_long */

	/*
	 *	Check that the input and output files are different
	 *	(could be the same by name truncation or links).
	 */
	if (same_file (&istat, &ostat))
	{
		fprintf
		(	stderr, "%s: \"%s\" e \"%s\" são o mesmo arquivo\n",
			pgname, ifname, ofname
		);
		exit_code = 1;
		return -1;
	}

	/* Ask permission to overwrite the existing file */

	if (!fflag) 
	{
		char response[80];

		strcpy (response, "n");

		fprintf
		(	stderr,
			"%s: \"%s\" já existe.",
			pgname, ofname
		);

		if (foreground && isatty (fileno (stdin)))
		{
			fprintf
			(	stderr,
				" Apaga/reescreve? (n): "
			);

			if (askyesno () <= 0)
			{
				if (exit_code == 0)
					exit_code = WARNING;

				return (-1);
			}
		}
		else
		{
			fprintf (stderr, "\n");
		}
	}

	chmod (ofname, 0777);

	if (unlink (ofname))
	{
		fprintf (stderr, "%s: ", pgname);
		perror (ofname);
		exit_code = 1;
		return -1;
	}

	return 0;

}	/* end check_ofname */

/*
 ****************************************************************
 *	Atribui tempos						*
 ****************************************************************
 */
void
reset_times (char *name, STAT *statb)
{
	time_t timep[3];

	timep[0] = statb->st_atime;
	timep[1] = statb->st_mtime;
	timep[2] = statb->st_ctime;

	if (utime (name, timep) < 0)
	{
		fprintf (stderr, "%s: ", pgname);
		perror(ofname);
	}

}	/* end reset_times */

/*
 ****************************************************************
 *	Atribui donos e modo					*
 ****************************************************************
 */
void
copy_stat (STAT *ifstat)
{
	time_t		 diff = ifstat->st_mtime - time_stamp;

	if (diff < 0)
		diff = -diff;

	if (dflag && diff > 60 && time_stamp != 0)
	{
		ifstat->st_mtime = time_stamp;

		if (vflag)
			fprintf (stderr, "%s: Restaurada a data do arquivo\n", ofname);
	}

	reset_times (ofname, ifstat);

	/* Copy the protection modes */

	if (chmod (ofname, ifstat->st_mode & 07777))
	{
		fprintf (stderr, "%s: ", pgname);
		perror(ofname);
	}

	chown (ofname, ifstat->st_uid, ifstat->st_gid);

	remove_ofname = 0;

	/* It's now safe to remove the input file: */

	chmod (ifname, 0777);

	if (unlink(ifname))
	{
		fprintf (stderr, "%s: ", pgname);
		perror(ifname);
	}

}	/* end copy_stat */
