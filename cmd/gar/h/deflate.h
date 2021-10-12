/*
 ****************************************************************
 *								*
 *			deflate.h				*
 *								*
 *	Cabeçalho com definições globais			*
 *								*
 *	Versão	3.0.00, de 05.06.93				*
 *		3.1.06, de 04.05.97				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários de compressão/descompressão		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado no "GZIP" do GNU			*
 * 								*
 ****************************************************************
 */

#define DEF_LEVEL	5	/* Nível de compactação normal */

#define MIN_LEVEL	1	/* Nível de compactação mínimo */
#define MAX_LEVEL	9	/* Nível de compactação máximo */

#define local static

/*
 * To save memory for 16 bit systems, some arrays are overlaid between
 * the various modules:
 *
 * deflate:  prev+head   window	   d_buf  l_buf  outbuf
 * inflate:		 window	   inbuf
 *
 * For compression, input is done in window[]. For decompression, output
 * is done in window.
 */
#define INBUFSIZ	(32 * 1024)	/* input buffer size */
#define INBUF_EXTRA	64		/* required by unlzw() */

#define OUTBUFSIZ	(16 * 1024)	/* output buffer size */
#define OUTBUF_EXTRA	2048 		/* required by unlzw() */

#define DIST_BUFSIZE	(32 * 1024)	/* buffer for distances, see trees.c */

#define WSIZE		(32 * 1024)

#define PREV_SZ		(64 * 1024)

extern char		*inbuf;		/* input buffer */
extern char		*outbuf;	/* output buffer */
extern ushort		*d_buf;		/* buffer for distances, see trees.c */
extern char		*window;	/* Sliding window and suffix table (unlzw) */

#define tab_suffix window

#define tab_prefix prev		/* hash link (see deflate.c) */
#define head (prev+WSIZE)	/* hash head (see deflate.c) */

extern ushort		*tab_prefix;	/* prefix code (see unlzw.c) */

extern unsigned int	insize;		/* valid bytes in inbuf */
extern unsigned int	inptr;  	/* index of next byte to be processed in inbuf */
extern unsigned int	outcnt;		/* bytes in output buffer */

extern long		bytes_in; 	/* number of input bytes */
extern long		bytes_out; 	/* number of output bytes */

extern int 		ifd;		/* input file descriptor */
extern int 		ofd;		/* output file descriptor */

extern ulong		crc_32;		/* CRC usado pelo deflate/inflate */

extern off_t		col_size;

extern long		block_start;	/* window offset of current block */
extern unsigned int	strstart;	/* window offset of current string */

/* The minimum and maximum match lengths */

#define MIN_MATCH  3
#define MAX_MATCH  258

/*
 * Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */
#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)

/*
 * In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */
#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)

/*
 ******	Entrada/saída do arquivo comprimido *********************
 */
typedef struct
{
	char	*z_ptr;		/* Ponteiro para o proximo caractere */
	char	*z_base;	/* Ponteiro para o inicio do buffer */
	char	*z_bend;	/* Fim do buffer */

}	ZIPFILE;

#define	GETZIP() 	(zp0->z_ptr < zp0->z_bend ? \
				 *zp0->z_ptr++ : read_zip (zp0))

#define	PUTZIP(c)  	{ if (zp1->z_ptr < zp1->z_bend)	\
				*zp1->z_ptr++ = (c);	\
			else				\
				write_zip ((c), zp1); }

extern ZIPFILE		zipfile;	/* Estrutura para a saída ZIP */

/*
 ******	Protótipos de funções ***********************************
 */
extern int		read_zip (ZIPFILE *);
extern void		write_zip (int, ZIPFILE *);
extern void		putzip_long (unsigned int);

extern int		file_read (char *, unsigned);
extern void		ct_init (void);
extern int		ct_tally (int, int);
extern ulong		flush_block (char *, ulong, int);
extern void		bi_init (void);
extern void		fill_window (void);
extern int		longest_match (ulong);
extern void		bi_windup (void);
extern unsigned int	bi_reverse (unsigned int, int);
extern void		send_bits (unsigned int, int);
extern void		copy_block (char *, unsigned int, int);
extern void		write_buf (int, void *, unsigned int);
extern int 		fill_inbuf (void);
extern void		flush_window (void);
extern ulong		updcrc (char *, unsigned int);
