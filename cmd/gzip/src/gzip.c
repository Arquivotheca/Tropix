/*
 ****************************************************************
 *								*
 *			gzip.c					*
 *								*
 *	Módulo principal					*
 *								*
 *	Versão	3.0.0, de 05.06.93				*
 *		3.2.1, de 21.06.99				*
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

/*
 ******	Global buffers ******************************************
 */
entry char	inbuf[INBUFSIZ + INBUF_EXTRA];
entry char	outbuf[OUTBUFSIZ + OUTBUF_EXTRA];
entry ushort	d_buf[DIST_BUFSIZE];
entry char	window[2 * WSIZE];
entry ushort	tab_prefix[1 << BITS];


#if (0)	/*******************************************************/
entry char	*inbuf;
entry char	*outbuf;
entry ushort	*d_buf;
entry char	*window;
entry ushort	*tab_prefix;
#endif	/*******************************************************/

/*
 ******	Variáveis locais ****************************************
 */
const char	pgversion[] =  "Versão: 3.2.1, de 21.06.99";

entry const char *pgname;	/* Nome do programa */

entry int	cflag;		/* Output to stdout (-c) */
entry int	dflag;		/* Descomprime (-d) */
entry int	fflag;		/* Don't ask questions, compress links (-f) */
entry int	Rflag;		/* Recursivo (-r ou -R) */
entry int	tflag;		/* Check .z file integrity (-t) */
entry int	vflag;		/* Be verbose (-v) */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

entry int	foreground;	   /* set if program run in foreground */
entry int	maxbits = BITS;   /* max bits per code for LZW */
entry int	method = DEFLATED;/* compression method */
entry int	level = 5;		/* compression level */
entry int	exit_code = 0;   /* program exit code */
entry int	save_orig_name;   /* set if original name must be saved */
entry int	last_member;	  /* set for .zip and .Z files */
entry int	part_nb;		  /* number of parts in .z file */
entry ulong	time_stamp;	   /* original time stamp (modification time) */
entry long	ifile_size;	  /* input file size, -1 for devices (debug only) */

entry long	bytes_in;			 /* number of input bytes */
entry long	bytes_out;			/* number of output bytes */
entry char	ifname[MAX_PATH_LEN]; /* input filename */
entry char	ofname[MAX_PATH_LEN]; /* output filename */
entry int	remove_ofname = 0;	   /* remove output file on error */
entry STAT	istat;		 /* status for input file */
entry int	ifd;				  /* input file descriptor */
entry int	ofd;				  /* output file descriptor */
entry unsigned	insize;		   /* valid bytes in inbuf */
entry unsigned	inptr;			/* index of next byte to be processed in inbuf */
entry unsigned	outcnt;		   /* bytes in output buffer */
void		(*work) (int infile, int outfile) = zip; /* function to call */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

extern void	treat_stdin (void);
extern void	treat_name (const char *);

/*
 ****************************************************************
 *	Ponto de entrada de "gzip"				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register const char	*cp;
	char			buf[128];
	char			*str;

	pgname = argv[0];

	/*
	 *	Prepara sinais
	 */
	if (foreground = (signal (SIGINT, SIG_IGN) != SIG_IGN))
		signal (SIGINT, abort_gzip);

	signal (SIGTERM, abort_gzip);
	signal (SIGHUP,  abort_gzip);

	/*
	 *	A opção de compressão também pode ser dada no ambiente
	 */
	if ((str = (char *)getenv ("GZIP")) == NOSTR)
		str = "";

	for (cp = strtok (str, ","); cp != NOSTR; cp = strtok (NOSTR, ","))
	{
		if (cp[0] == '-')
			cp++;

		if (cp[0] >= '1'  &&  cp[0] <= '9' && cp[1] == '\0')
		{
			level = *cp - '0';
		}
	}

	/*
	 *	Muda a ação conforme o nome do programa
	 *
	 *	"ungzip", "uncompress", "gunzip" => descomprime
	 *	"zcat", "pcat", "gzcat" => descomprime e põe na saída padrão
	 */
	cp = basename (pgname);

	if   (!strncmp (cp, "un", 2) || !strncmp (cp, "gun", 3))
		dflag = 1;
	elif (streq (cp+1, "cat") || streq (cp, "gzcat"))
		{ dflag = cflag = 1; }

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "cdfrRt123456789vNH")) != EOF)
	{
		switch (opt)
		{
		    case 'c':
			cflag = 1;
			break;

		    case 'd':
			dflag = 1;
			break;

		    case 'f':
			fflag++;
			break;

		    case 'r':
		    case 'R':
			Rflag = 1;
			break;

		    case 't':
			tflag = dflag = cflag = 1;
			break;

		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			level = opt - '0';
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

#if (0)	/*******************************************************/
	/*
	 *	Aloca as áreas globais
	 */
	if ((inbuf = malloc (INBUFSIZ + INBUF_EXTRA)) == NOSTR)
		error ("Memória insuficiente");

	if ((outbuf = malloc (OUTBUFSIZ + OUTBUF_EXTRA)) == NOSTR)
		error ("Memória insuficiente");

	if ((d_buf = malloc (DIST_BUFSIZE * sizeof (ushort))) == NOVOID)
		error ("Memória insuficiente");

	if ((window = malloc (2 * WSIZE)) == NOSTR)
		error ("Memória insuficiente");

	if ((tab_prefix = malloc ((1 << BITS) * sizeof (ushort))) == NOVOID)
		error ("Memória insuficiente");
#endif	/*******************************************************/

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag && *argv != NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Os argumentos supérfluos serão ignorados\n",
			pgname
		);

		*argv = NOSTR;
	}

	/*
	 *	Procura os argumentos
	 */
	if   (Nflag)
	{
		/*
		 *	Lê os nomes dos arquivos de "stdin"
		 */
		while (gets (buf) != NOSTR)
		{
			treat_name (buf);
		}
	}
	elif (*argv != NOSTR)
	{
		/*
		 *	Foram dados argumentos
		 */
		for (/* vazio */; *argv; argv++)
		{
			treat_name (*argv);
		}
	}
	else	/* Entrada padrão */
	{
		treat_stdin ();
	}

	return (exit_code);

}	/* end gzip */

/*
 ****************************************************************
 *	Aborta a execução					*
 ****************************************************************
 */
void
abort_gzip (int signo, ...)
{
	if (remove_ofname)
	{
		close(ofd);
		unlink (ofname);
	}

	exit(1);

}	/* end abort_gzip */

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
		"%s - comprime ou expande arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\tgzip [-cdfrRt1..9vN] [<arquivo> ...]\n"
		"\tgunzip [-cfrRtvN] [<arquivo> ...]\n",
		pgname, pgversion
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-c: Escreve em <stdout>, não modifica os arquivos originais\n"
		"\t-d: Descomprime\n"
		"\t-f: Permite a reescrita de arquivos de saída e comprime \"links\"\n"
		"\t-r: Recursivo: processa árvores\n"
		"\t-R: Como \"-r\", acima\n"
		"\t-t: Verifica a integridade dos arquivos comprimidos\n"
		"\t-1: Comprime mais rápido\n"
		"\t-9: Comprime melhor\n"
		"\t-v: Verboso\n"
		"\t-N: Le os nomes dos arquivos de \"stdin\"\n"
	);

	exit (2);

}	/* end help */
