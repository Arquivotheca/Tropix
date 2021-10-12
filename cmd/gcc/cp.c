




typedef	unsigned char	uchar;		
typedef	signed char	schar;		
typedef	unsigned short	ushort;		
typedef	unsigned long	ulong;		
typedef	long       	daddr_t;  	
typedef	char *     	caddr_t;  	
typedef	int		pg_t;     	
typedef	ushort		ino_t;     	
typedef	long       	time_t;   	
typedef	short        	dev_t;    	
typedef	long       	off_t;    	
typedef	long		sid_t[2];	
typedef	long		id_t[4];	
typedef	long		lid_t[8];	
typedef	long		*idp_t;		



typedef	struct	stat	STAT;

struct	stat
{
	dev_t	st_dev;		
	ino_t	st_ino;		
	long	st_mode;	
	ushort	st_nlink;	
	ushort 	st_uid;		
	ushort 	st_gid;		
	dev_t	st_rdev;	
	off_t	st_size;	
	time_t	st_atime;	
	time_t	st_mtime;	
	time_t	st_ctime;	
	long	st_res[2];	
};










extern int	stat (char *, STAT *); 
extern int	fstat (int, STAT *); 



extern void	_exit (int);
extern int	access (char *, int);
extern unsigned	alarm (unsigned);
extern int	brk (void *);
extern int	chdir (char *);
extern int	chmod (char *, int);
extern int	chown (char *, int, int);
extern int	chroot (char *);
extern int	close (int);
extern int	clrtgrp (void);
extern int	creat (char *, int);
extern int	dup (int);
extern int	execl (char *, char *, ...);
extern int	execle (char *, char *, ...);
extern int	execv (char *, char **);
extern int	execve (char *, char **, char **);
extern void	exit (int);
extern int	fcntl (int, int, int);
extern int	fork (void);
extern int	getegid (void);
extern int	geteuid (void);
extern int	getgid (void);
extern int	getpgrp (void);
extern long	getpid (void);
extern long	getppid (void);
extern int	getsn (void);
extern int	getuid (void);
extern int	ioctl (int, int, ...);
extern int	kcntl (int, ...);
extern int	kill (int, int);
extern int	link (char *, char *);
extern int	mknod (char *, int, int);
extern int	mount (char *, char *, int);
extern int	nice (int);
extern int	open (char *, int, ...);
extern int	inopen (int, int);
extern int	pause (void);
extern void	*phys (void *, int, int);
extern int	pipe (int []);
extern int	plock (int);
extern int	ptrace (int, int, int, int);
extern int	read (int, void *, int);
extern void	*sbrk (int);
extern int	setgid (int);
extern int	setpgrp (void);
extern int	setuid (int);
extern int	sigchild (long);
extern int	spy (void);
extern int	sync (void);
extern int	umask (int);
extern int	umount (char *);
extern int	unlink (char *);
extern int	untext (char *);
extern int	wait (int *);
extern int	write (int, void *, int);



typedef long	fpos_t;		
typedef int	size_t;		
typedef char	*va_list;	


typedef struct _iobuf
{
	unsigned char	*_ptr;		
	unsigned char	*_base;		
	unsigned char	*_bend;		
	unsigned char	*_lend;		
	short		_bsize;		
	short		_flag;		
	short		_file;		
	short		_reser;		

}	FILE;

extern FILE	_ioblk[];













extern int	_readbuf (FILE *);
extern int	_writebuf (int, FILE *);
extern int	remove (const char *);
extern int	rename (const char *, const char *);
extern FILE	*tmpfile (void);
extern char	*tmpnam (char *);
extern FILE	*popen (const char *, const char *);
extern int	pclose (FILE *);
extern int	fclose (FILE *);
extern int	fflush (FILE *);
extern FILE	*fopen (const char *, const char *);
extern FILE	*fdopen (int, const char *);
extern FILE	*freopen (const char *, const char *, FILE *);
extern int	setbuf (FILE *, char *);
extern int	setvbuf (FILE *, char *, int, size_t);
extern int	fprintf (FILE *, const char *, ...);
extern int	fscanf (FILE *, const char *, ...);
extern int	printf (const char *, ...);
extern int	scanf (const char *, ...);
extern int	sprintf (char *, const char *, ...);
extern int	sscanf (const char *, const char *, ...);
extern int 	vfprintf (FILE *, const char *, va_list);
extern int	vprintf (const char *, va_list);
extern int 	vsprintf (char *, const char *, va_list);
extern int	fgetc (FILE *);
extern char	*fgets (char *, int, FILE *);
extern char	*fngets (char *, int, FILE *);
extern int	fputc (int, FILE *);
extern int	fputs (const char *, FILE *);
extern int	fnputs (const char *, FILE *);
extern char	*gets (char *);
extern int	puts (const char *);
extern int	ungetc (int, FILE *);
extern size_t	fread (void *, size_t, size_t, FILE *);
extern size_t	fwrite (void *, size_t, size_t, FILE *);
extern int	fgetpos (FILE *, fpos_t *);
extern int	fsetpos (FILE *, const fpos_t *);
extern int	fseek (FILE *, long, int);
extern long	ftell (FILE *);
extern void	rewind (FILE *);
extern void	perror (const char *);





typedef	int	size_t;		


extern char	*strcpy (char *, const char *);
extern char	*strncpy (char *, const char *, size_t);
extern char	*strmove (char *, const char *);
extern char	*strgcpy (char *, const char *);
extern char	*strcat (char *, const char *);
extern char	*strncat (char *, const char *, size_t);
extern char	*strdup (const char *);

extern int	strcmp (const char *, const char *);
extern int	streq (const char *, const char *);
extern int	strncmp (const char *, const char *, size_t);

extern char	*strchr (const char *, int);
extern char	*strrchr (const char *, int);
extern char	*strend (const char *);
extern size_t	strcspn (const char *, const char *);
extern char	*strpbrk (const char *, const char *);
extern size_t	strspn (const char *, const char *);
extern char	*strstr (const char *, const char *);
extern char	*strtok (char *, const char *);
extern char	*strfield (char *, int);
extern size_t	strlen (const char *);
extern int	strcoll (const char *, const char *);
extern size_t	strxfrm (char *, const char *, size_t);
extern char	*strerror (int);

extern int	strrcmp (const char *, const char *);
extern int	stricmp (const char *, const char *);
extern int	strircmp (const char *, const char *);
extern int	strtcmp (const char *, const char *, char []);
extern int	strntcmp (const char *, const char *, size_t, char []);
extern int	strttcmp (const char *, const char *, char [], char []);
extern int	strnttcmp (const char *, const char *, size_t, char [], char []);

extern void	*memcpy (void *, const void *, size_t);
extern void	*memocpy (void *, const void *, size_t);
extern void	*memccpy (void *, const void *, size_t);
extern void	*memmove (void *, const void *, size_t);

extern int	memcmp (const void *, const void *, size_t);
extern int	memeq (const void *, const void *, size_t);
extern int	memtcmp (const void *, const void *, size_t, char []);
extern int	memttcmp (const void *, const void *, size_t, char [], char []);

extern void	*memchr (const void *, int, size_t);
extern void	*memset (void *, int, size_t);
extern short	*memsetw (short *, int, int);
extern long	*memsetl (long *, int, int);
















typedef struct { int  quot, rem; }	div_t;	
typedef struct { long quot, rem; }	ldiv_t;	
typedef int				size_t;	
typedef char				wchar_t;


extern long double	atof (const char *);
extern int	atoi (const char *);
extern long	atol (const char *);
extern long double	strtod (const char *, char **);
extern long	strtol (const char *, char **, int);
extern unsigned strtoul (const char *, char **, int);
extern long	rand (void);
extern void 	srand (long);
extern void 	*calloc (size_t, size_t);
extern void 	free (void *);
extern void 	*malloc (size_t);
extern void 	*salloc (size_t);
extern void 	*realloc (void *, size_t);
extern void 	abort (void);
extern int 	atexit (void (*) (void));
extern void 	exit (int);
extern int	getopt (int, char **, const char[]);
extern int	optind;
extern char	*optarg;
extern char	*getenv (const char *);
extern int	putenv (const char *);
extern int	delenv (const char *);
extern int	system (const char *);
extern void	*bsearch (const void *, const void *, size_t, size_t, int (*) ());
extern void	qsort (void *, size_t, size_t, int (*) ());
extern void	qtsort (void **, size_t, int (*) ());
extern int	abs (int);
extern div_t	div (int, int);
extern long	labs (long);
extern ldiv_t	ldiv (long, long);
extern int	mblen (const char *, size_t);
extern int	mbtowc (wchar_t *, const char *, size_t);
extern int	wctomb (char *, wchar_t);
extern size_t	mbstowcs (wchar_t *, const char *, size_t);
extern size_t	wcstombs (char *, const wchar_t *, size_t);


const char	pgversion[] =  "Versão:	2.3.1, de 26.11.89";



char	*pgname;	

int	iflag;		
int	fflag;		
int	vflag;		
int	mflag;		
int	xflag;		
int	Nflag;		
int	dflag;		

FILE	*ttyfp;		

extern int	optind;		

extern char	*optarg;	

void		help (void);
int		copia (char *, char *);
int		copiablocos (char *, char *, STAT *, STAT *);
int		stat (char *, STAT *);

extern int	askyesno (FILE *, int);
extern int	utime (char *, time_t []);


main (int argc, char **argv)
{
	register int	opt, ret = 0;
	register char	*cp;
	char		buf[512];
	char		pathname[512];
	char		*dirend = (char *)0;
	char		*dirnm;
	STAT		s;

	
	pgname = argv[0];

	
	while ((opt = getopt (argc, argv, "ifvmxNdH")) != (-1))
	{
		switch (opt)
		{
		    case 'i':			
			iflag++;
			break;

		    case 'f':			
			fflag++;
			break;

		    case 'v':			
			vflag++;
			break;

		    case 'm':			
			mflag++;
			break;

		    case 'x':			
			xflag++;
			break;

		    case 'N':			
			Nflag++;
			break;

		    case 'd':			
			dflag++;
			break;

		    case 'H':			
			help ();

		    default:			
			fputc ('\n', (&_ioblk[2]));
			help ();

		}	

	}	

	argv += optind;
	argc -= optind;

	
	if (iflag || !fflag)
	{
		if ((ttyfp = fopen ("/dev/tty", "r")) == (FILE *)0)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui abrir \"/dev/tty\"\n",
				pgname
			);
			perror (pgname);
			exit (1);
		}
	}

	
	if (Nflag)
	{
		if (argc != 1)
			help ();
	}
	else
	{
		if (argc < 2)
			help ();
	}

	
	if (stat ((dirnm = argv[argc-1]), &s) >= 0 && (((s.st_mode) & 0x00F000) == 0x004000))
	{
		strcpy (pathname, dirnm);
		strcat (pathname, "/");
		dirend = strrchr (pathname, '/') + 1;
	}

	
	if ((Nflag || argc > 2)  &&  dirend == (char *)0)
	{
		fprintf
		(	(&_ioblk[2]),
			"%s: O arquivo \"%s\" não é um diretório\n\n",
			pgname,	dirnm
		);
		help ();
	}

	
	if   (dirend == (char *)0)
	{
		
		ret += copia (argv[0], argv[1]);
	}
	else if (!Nflag)
	{
		
		argv[argc-1] = (char *)0;

		for (; *argv; argv++)
		{
			if ((cp = strrchr (*argv, '/')) == (char *)0)
				cp = *argv;
			else
				cp++;

			strcpy (dirend, cp);

			ret += copia (*argv, pathname);
		}
	}
	else
	{
		
		while (gets (buf) != (char *)0)
		{
			if ((cp = strrchr (buf, '/')) == (char *)0)
				cp = buf;
			else
				cp++;

			strcpy (dirend, cp);

			ret += copia (buf, pathname);
		}
	}

	exit (ret);

}	


int
copia (oldnm, newnm)
char	*oldnm;
char	*newnm;
{
	STAT		olds, news;

	if (dflag)
		fprintf ((&_ioblk[2]), "=> copia (%s, %s)\n", oldnm, newnm);

	
	if (iflag)
	{
		fprintf ((&_ioblk[2]), "%s: ", oldnm);
		fflush ((&_ioblk[2]));
		if (askyesno (ttyfp, 0) <= 0)
			return (0);
	}

	if (stat (oldnm, &olds) < 0)
	{
		fprintf
		(	(&_ioblk[2]),
			"Não consegui obter o estado de \"%s\"\n",
			oldnm
		);
		perror (pgname);
		return (1);
	}

	if ((((olds.st_mode) & 0x00F000) == 0x004000))
	{
		fprintf
		(	(&_ioblk[2]),
			"%s: \"%s\" é um diretório\n",
			pgname, oldnm
		);
		return (1);
	}

	
	if (stat (newnm, &news) >= 0)
	{
		if ((((news.st_mode) & 0x00F000) == 0x004000))
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: \"%s\" já existe "
				"e é um diretório\n",
				pgname, newnm
			);
			return (1);
		}

		if
		(	olds.st_dev == news.st_dev  &&
			olds.st_ino == news.st_ino
		)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: \"%s\" já existe "
				"e é o mesmo arquivo do que \"%s\"\n",
				pgname, newnm, oldnm
			);
			return (1);
		}

		if (!fflag)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: \"%s\" já existe. Apaga/reescreve? (n): ",
				pgname, newnm
			);
			fflush ((&_ioblk[2]));
			if (askyesno (ttyfp, 0) <= 0)
				return (1);

		}

		if
		(	(olds.st_mode & 0x00F000) != (news.st_mode & 0x00F000) ||
			!(((olds.st_mode) & 0x00F000) == 0x008000)
		)
		{
			if (unlink (newnm) < 0)
			{
				fprintf
				(	(&_ioblk[2]),
					"%s: Não consigo remover \"%s\"\n",
					pgname, newnm
				);
				perror (pgname);
				return (1);
			}
		}

	}	 

	
	if (copiablocos (oldnm, newnm, &olds, &news) != 0)
		return (1);

	if (vflag  &&  !iflag)
		fprintf ((&_ioblk[1]), "%s:\n", oldnm);

	return (0);

}	
 

int
copiablocos (oldnm, newnm, olds, news)
char	*oldnm, *newnm;
STAT	*olds, *news;
{
	register int	fdin, fdout;
	register int	n;
	char		buf[512];

	if (dflag)
		fprintf ((&_ioblk[2]), "=> copiablocos (%s, %s)\n", oldnm, newnm);

	
	switch (olds->st_mode & 0x00F000)
	{
		
	    case 0x008000:
	    copia_conteudo:
		if ((fdout = creat (newnm, olds->st_mode)) < 0)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui criar o arquivo \"%s\"\n",
				pgname, newnm
			);
			perror (pgname);
			return (1);
		}

		if ((fdin = open (oldnm, 0x0000)) < 0)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui abrir o arquivo \"%s\"\n",
				pgname, oldnm
			);
			perror (pgname);
			close (fdout);
			return (1);
		}

		while ((n = read (fdin, buf, sizeof (buf))) > 0)
		{
			if (write (fdout, buf, n) != n)
			{
				fprintf
				(	(&_ioblk[2]),
					"%s: Erro de escrita em \"%s\"\n",
					pgname, newnm
				);
				perror (pgname);
				close (fdin);
				close (fdout);
				return (1);
			}
		}

		close (fdin);
		close (fdout);

		if (n < 0)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Erro de leitura em \"%s\"\n",
				pgname, oldnm
			);
			perror (pgname);
			return (1);
		}

		break;

		
	    case 0x006000:
	    case 0x002000:
	    case 0x001000:
		if (!xflag)
			goto copia_conteudo;

		if (mknod (newnm, olds->st_mode, olds->st_rdev) < 0)
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui criar \"%s\"\n",
				pgname, newnm
			);
			perror (pgname);
			return (1);
		}

		break;

	    default:
		fprintf
		(	(&_ioblk[2]),
			"%s: Tipo de arquivo inválido\n",
			pgname
		);
		return (1);

	}	

	
	if (stat (newnm, news) < 0)
	{
		fprintf
		(	(&_ioblk[2]),
			"%s: Não consegui obter o estado de \"%s\"\n",
			pgname, newnm
		);
		perror (pgname);
		return (1);
	}

	
	if (!mflag)
		return (0);

	if ((olds->st_mode & ~0x00F000) != (news->st_mode & ~0x00F000))
	{
		if (chmod (newnm, olds->st_mode & ~0x00F000) < 0) 
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui restaurar a "
				"proteção de \"%s\"\n",
				pgname, newnm
			);
			perror (pgname);
			return (1);
		}
	}

	if (utime (newnm, &olds->st_atime) < 0)
	{
		fprintf
		(	(&_ioblk[2]),
			"%s: Não consegui restaurar os tempos de \"%s\"\n",
			pgname, newnm
		);
		perror (pgname);
		return (1);
	}

	if (olds->st_uid != news->st_uid || olds->st_gid != news->st_gid)
	{
		if (chown (newnm, olds->st_uid, olds->st_gid) < 0) 
		{
			fprintf
			(	(&_ioblk[2]),
				"%s: Não consegui restaurar o "
				"dono/grupo de \"%s\"\n",
				pgname, newnm
			);
			perror (pgname);
			return (1);
		}
	}

	return (0);

}	


void
help ()
{
	fprintf ((&_ioblk[2]), "%s - copia arquivos\n", pgname);
	fprintf ((&_ioblk[2]), "\n%s\n", pgversion);
	fprintf
	(	(&_ioblk[2]),
		"\nSintaxe:\n"
		"\t%s [-ifvmx] <arquivo1> <arquivo2>\n"
		"\t%s [-ifvmx] <arquivo1> [<arquivo2> ...] <diretório>\n"
		"\t%s [-ifvmx] -N <diretório>\n",
		pgname,	pgname,	pgname
	);
	fprintf
	(	(&_ioblk[2]),
		"\nOpções:"
		"\t-i: Pede a confirmaçãoªpara cada arquivo a copiar\n"
		"\t-f: Os arquivos destino existentes são removidos "
			"silenciosamente\n"
		"\t-v: Lista o nome dos arquivos copiados\n"
		"\t-m: Copia o dono, grupo, proteção e datas do arquivo\n"
		"\t-x: Para arquivos especiais e FIFOs, copia o nó-índice\n"
		"\t-N: Lê os nomes dos arquivos de \"stdin\"\n"
	);
	exit (2);

}	
