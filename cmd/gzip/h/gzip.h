/*
 ****************************************************************
 *								*
 *			gzip.h					*
 *								*
 *	Cabeçalho com definições globais			*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.0.0, de 18.03.96				*
 *								*
 *	Módulo: GZIP						*
 *		Utilitários de compressão/descompressão		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

#define	elif	else if
#define	EVER	;;

#define OF(args)  args

typedef void *voidp;

#define memzero(s, n)	 memset ((s), 0, (n))

#define local static

/*
 *	Sistema Operacional
 *
 *	UNIX = 3, MACOS = 7, WIN32 = 11, TOPS20 = 10
 */
#define OS_CODE  3	/* assume UNIX */

#define RECORD_IO 0

/* Return codes from gzip */
#define OK  	0
#define ERROR   1
#define WARNING 2

/* Compression methods (see algorithm.doc) */
#define STORED	 	0
#define COMPRESSED	1
#define PACKED		2
/* methods 3 to 7 reserved */
#define DEFLATED  	8

extern int	method;		 /* compression method */

/* To save memory for 16 bit systems, some arrays are overlaid between
 * the various modules:
 *
 * deflate:  prev+head   window	    d_buf  l_buf  outbuf
 * unlzw:   tab_prefix  tab_suffix  stack  inbuf  outbuf
 * inflate:		  window	   inbuf
 * unpack:		  window	   inbuf
 *
 * For compression, input is done in window[]. For decompression, output
 * is done in window except for unlzw.
 */

#define INBUFSIZ  0x8000  /* input buffer size */
#define INBUF_EXTRA  64	 /* required by unlzw() */

#  define OUTBUFSIZ  16384  /* output buffer size */
#define OUTBUF_EXTRA 2048   /* required by unlzw() */

#define DIST_BUFSIZE 0x8000 /* buffer for distances, see trees.c */

extern char	inbuf[];	/* input buffer */
extern char	outbuf[];	/* output buffer */
extern ushort	d_buf[];	/* buffer for distances, see trees.c */
extern char	window[];	/* Sliding window and suffix table (unlzw) */

#define tab_suffix window

#define tab_prefix prev		/* hash link (see deflate.c) */
#define head (prev+WSIZE)	/* hash head (see deflate.c) */

extern ushort	tab_prefix[];	/* prefix code (see unlzw.c) */

extern unsigned insize; /* valid bytes in inbuf */
extern unsigned inptr;  /* index of next byte to be processed in inbuf */
extern unsigned outcnt; /* bytes in output buffer */

extern long	bytes_in;   /* number of input bytes */
extern long	bytes_out;  /* number of output bytes */
extern long	overhead;   /* number of bytes in gzip header */

#define isize bytes_in
/* for compatibility with old zip sources (to be cleaned) */

extern int 	ifd;	/* input file descriptor */
extern int 	ofd;	/* output file descriptor */
extern char	ifname[];   /* input filename or "stdin" */
extern char	ofname[];   /* output filename or "stdout" */

extern ulong	time_stamp;  /* original time stamp (modification time) */
extern long	ifile_size; /* input file size, -1 for devices (debug only) */

typedef int file_t;	 /* Do not use stdio */

#define NO_FILE  (-1)   /* in memory compression */

#define	GZIP_MAGIC	"\037\213" /* Magic header for gzip files, 1F 8B */
#define	OLD_GZIP_MAGIC	"\037\236" /* Magic header for gzip 0.5 = freeze 1.x */
#define	PKZIP_MAGIC   "PK\003\004" /* Magic header for pkzip files */
#define	PACK_MAGIC	"\037\036" /* Magic header for packed files */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT	     0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

/* internal file attribute */
#define UNKNOWN 0xffff
#define BINARY  0
#define ASCII   1

#define WSIZE 0x8000	/* window size--must be a power of two, and */
			/*  at least 32K for zip's deflate method */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

extern int	cflag;		/* output to stdout (-c) */
extern int	dflag;		/* Descomprime */
extern int	fflag;		/* Force */
extern int	Rflag;		/* Recursivo */
extern int	tflag;		/* check .z file integrity */
extern int	vflag;		/* be verbose (-v) */

extern int	level;		/* compression level */

extern const char *pgname;	/* program name */
extern int	save_orig_name;	/* set if original name must be saved */
extern int	decrypt;	/* flag to turn on decryption */
extern int	exit_code;	/* program exit code */

/*
 ******	Entrada do arquivo comprimido ***************************
 */
#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf ())

/*
 ******	Saída do arquivo comprimido *****************************
 */
typedef struct
{
	char	*z_ptr;		/* Ponteiro para o proximo caractere */
	char	*z_base;	/* Ponteiro para o inicio do buffer */
	char	*z_bend;	/* Fim do buffer */

}	ZIPFILE;

#if (0)	/*************************************/
#define	GETZIP() 	(fp0->z_ptr < fp0->z_bend ? \
				 *fp0->z_ptr++ : readzip (fp0))
#endif	/*************************************/

#define	PUTZIP(c)  	{ if (zp1->z_ptr < zp1->z_bend)	\
				*zp1->z_ptr++ = (c);	\
			else				\
				write_zip ((c), zp1); }

extern void	write_zip (int, ZIPFILE *);
extern void	putzip_long (unsigned long);

/*
 *	put_byte is used for the compressed output, put_char for the
 *	uncompressed output. However unlzw() uses window for its
 *	suffix table instead of its output buffer, so it does not use put_char.
 *	(to be cleaned up).
 */
#define put_char(c) {window[outcnt++]=(char)(c); if (outcnt==WSIZE)\
	flush_window();}

/*
 ******	x *******************************************************
 */
#define tolow(c)  (isupper(c) ? (c)-'A'+'a' : (c))	/* force to lower case */

#if (0)	/*************************************/
/* Macros for getting two-byte and four-byte header values */
#define SH(p) ((ushort)(char)((p)[0]) | ((ushort)(char)((p)[1]) << 8))
#define LG(p) ((ulong)(SH(p)) | ((ulong)(SH((p)+2)) << 16))
#endif	/*************************************/

/* Diagnostic functions */
#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

/*
 ******	Protótipos de funções ***********************************
 */
	/* in zip.c: */
extern void	zip (int in, int out);
extern int	file_read (char *buf, unsigned size);

	/* in unzip.c */
extern void	unzip (int in, int out);
extern int	check_zipfile (int in);

	/* in unpack.c */
extern void	unpack (int in, int out);

	/* in gzip.c */
extern void	abort_gzip (int, ...);

		/* in deflate.c */
extern void	lm_init (int pack_level, ushort *flags);
extern ulong 	deflate (void);

		/* in trees.c */
extern void	ct_init (ushort *attr, int *);
extern int	ct_tally (int dist, int lc);
extern ulong	flush_block (char *buf, ulong stored_len, int eof);

		/* in bits.c */
extern void	bi_init (file_t zipfile);
extern void	send_bits (unsigned value, int length);
extern unsigned	bi_reverse (unsigned value, int length);
extern void	bi_windup (void);
extern void	copy_block (char *buf, unsigned len, int header);

	/* in util.c: */
extern ulong	updcrc (char *s, unsigned n);
extern void	clear_bufs (void);
extern int 	fill_inbuf (void);
extern void	flush_outbuf (void);
extern void	flush_window (void);
extern void	write_buf (int fd, voidp buf, unsigned cnt);
extern char	*strlwr	(char *s);
extern char	*basename (const char *fname);
extern char	*add_envopt (int *argcp, char ***argvp, char *env);
extern void	gzip_error (const char *m);
extern void	warn (char *a, char *b);
extern void	read_error (void);
extern void	write_error (void);
extern void	display_ratio (long num, long den);

	/* in inflate.c */
extern int	inflate (void);
