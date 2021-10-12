/*
 ****************************************************************
 *								*
 *			bufmap.c				*
 *								*
 *	Desenha no Terminal um Mapa de BUFFERS			*
 *								*
 *	Versão	1.0.0, de 17.03.87				*
 *		2.3.0, de 04.09.89				*
 *								*
 *	Modulo: bufmap						*
 *		Utilitario Especial				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/kcntl.h>

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/sync.h>

#include <sys/tty.h>
#include <sys/buf.h>
#include <a.out.h>

#include <curses/info.h>
#include <curses/capaccess.h>

/*
 ****************************************************************
 *	Variaveis e Definicoes Globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	2.3.0, de 04.09.89";

#define	ESC	033
#define	BELL	007

#define	NLIN	24		/* Nr. de linhas */
#define	NCOL	80		/* Nr. de colunas */

#define	BEGLN	2		/* No. da linha do comeco da tabela */
#define	ENDLN	(NLIN - 2)	/* No. da linha do final da tabela */
#define	BEGCOL	10		/* No. da coluna de comeco do mapa */
#define LARG	3		/* No. de linhas da informacao */

#define COLFOLD(c)	(c % (NCOL-BEGCOL) + BEGCOL)	
#define LNFOLD(l, c)	(c / (NCOL-BEGCOL) * LARG + BEGLN + l)	

#define	NPOINTS		(((ENDLN-BEGLN)/LARG) * (NCOL-BEGCOL))

#define	PERCENT(a, b)	((100*(a))/(b))

char		*pgname;	/* Nome do programa */

char		tela[NLIN][NCOL];	/* O grafico na tela */

int		vflag;		/* Verbose */

extern int	optind;		/* Incremento do argv (em getopt) */

extern char	*optarg;	/* Argumento (de getopt) */

void		help ();

unsigned 	lin, col;		/* linha e coluna correntes */

BHEAD		*bufs;			/* comeco da tabela */

BHEAD		**bclock;		/* bclock */
int		obclock = -1;		/* coluna do bclock anterior */

BHEAD		**bflushclock;		/* bflushclock */
int		obflushclock = -1;	/* coluna do bflushclock anterior */

SYM		sym;			/* Entrada de tabela de simbolos */

int		tempo;			/* Intervalo entre cada Mostra */

TERMIO		oldtty,
		newtty;			/* Controle de Paginacao do Terminal */

SCB		scb;

int		BLKperPOINT;

char		buf[BUFSIZ];		/* Para o Terminal */

INFO		info;			/* Informações sobre o Terminal */

extern char	*parmexec ();

char		*btime ();
int		quit ();
int		bus ();

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (argc, argv)
int	argc;
char	*argv[];
{
	register char	*cp;
	time_t		t;
	lid_t		lid;
	register int	opt;

	pgname = argv[0];

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fputc ('\n', stderr);
			help ();
		}

	}

	argv += optind;
	argc -= optind;


	/*
	 *	Tira o "PAGING" do Terminal.
	 */
	ioctl (1, TCGETS, &oldtty);
	newtty = oldtty;
	newtty.c_cc[VPAGE] = 0;
	ioctl (1, TCSETAW, &newtty);

	signal (SIGINT, quit);
	signal (SIGBUS, bus);

	/*
	 *	Obtem as características do terminal
	 */
	if (getinfo (NOSTR, &info) == 0)
		exit (1);

	/*
	 *	Obtem o System Control Block.
	 */
	if (getscb (&scb) < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel copiar o SCB\n", pgname);
		quit ();
	}

	/*
	 *	Mapeia a tabela de BUFFERs
	 */
	bufs  = (BHEAD *)phys ((char *)scb.y_bhead,
			scb.y_nbuf * sizeof (BHEAD), O_KERNEL|O_RDONLY);

	if ((int)bufs < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear os BUFFERs\n",
					pgname);
		quit ();
	}

	/*
	 *	Mapeia o CLOCK
	 */
	if (kcntl (K_GETSYMGN, lidcan (lid, "_bclock"), &sym) < 0)
	{
		fprintf (stderr, "%s: ", pgname);
		fprintf (stderr,
			"Nao obtive o endereco de \"bclock\"\n");
		quit ();
	}

	bclock = (BHEAD **)phys ((char *)sym.s_value,
			sizeof (sym.s_value), O_KERNEL|O_RDONLY);

	if ((int)bclock < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear bclock\n",
					pgname);
		quit ();
	}

	/*
	 *	Mapeia o FLUSHCLOCK
	 */
	if (kcntl (K_GETSYMGN, lidcan (lid, "_bflushclock"), &sym) < 0)
	{
		fprintf (stderr, "%s: ", pgname);
		fprintf (stderr,
			"Nao obtive o endereco de \"bflushclock\"\n");
		quit ();
	}

	bflushclock = (BHEAD **)phys ((char *)sym.s_value,
			sizeof (sym.s_value), O_KERNEL|O_RDONLY);

	if ((int)bflushclock < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear bflushclock\n",
					pgname);
		quit ();
	}

	/*
	 *	Verifica se o Intervalo de tempo foi especificado.
	 */
	if (argc > 0)
		tempo = atoi (argv[0]);
	if (tempo <= 0 && argv[0][0] != '0')
		tempo = 1;
	if (tempo == 0)
		nice (20);

	setbuf (stdout, buf);

	/*
	 *	Limpa a Tela.
	 */
	memset ((char *)tela, ' ', NLIN * NCOL);
	fputs (info.i_strings[s_clear], stdout);

	/*
	 *	Coloca os Titulos
	 */
	strput ("BUF MAP", 0, 35);

	strput ("DIRTY", BEGLN+0, 0);   
	strput ("CLOCK", BEGLN+1, 0);   

	strput ("DIRTY", BEGLN+LARG+0, 0);   
	strput ("CLOCK", BEGLN+LARG+1, 0);   

	strput ("DIRTY", BEGLN+2*LARG+0, 0);   
	strput ("CLOCK", BEGLN+2*LARG+1, 0);   

	strput ("DIRTY", BEGLN+3*LARG+0, 0);   
	strput ("CLOCK", BEGLN+3*LARG+1, 0);   

	strput ("DIRTY", BEGLN+4*LARG+0, 0);   
	strput ("CLOCK", BEGLN+4*LARG+1, 0);   

	strput ("DIRTY", BEGLN+5*LARG+0, 0);   
	strput ("CLOCK", BEGLN+5*LARG+1, 0);   

	fflush (stdout);

	BLKperPOINT = (scb.y_nbuf + NPOINTS-1) / NPOINTS;

	/*
	 *	Inicia o Desenho.
	 */
	for (EVER)
	{
		/*
		 *	Buffers
		 */ 
		bufmap ();

		/*
		 *	Coloca a Mensagem no final da tela
		 */
		time (&t);
		cp = btime (&t);
		cp[24] = '\0';
		strput (cp, 23, 0);
		fflush (stdout);
		sleep (tempo);

	}	/* end for (EVER) */

}	/* end coremap */

/*
 ****************************************************************
 *	Obtem o Mapa dos BUFFERs 				*
 ****************************************************************
 */
int
bufmap ()
{
	register BHEAD	*bp;
	register int	i, j, k, nbuf;

	nbuf =  scb.y_nbuf / BLKperPOINT;

#if (0)	/*************************************/
	for (bp = bufs, i = 0; i < nbuf; i++, bp++)
	{
		if (bp->b_lock)
			charput ('*', LNFOLD (0, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (0, i), COLFOLD (i));
	}
#endif	/*************************************/

	for (bp = bufs, i = 0; i < nbuf; i++)
	{
		for (k = 0, j = 0; j < BLKperPOINT; j++, bp++)
		{
			if (bp->b_flags & B_DIRTY)
				k++;
		}

		if (k == 0)
			charput ('.',     LNFOLD (0, i), COLFOLD (i));
		else
			charput ('0' + k, LNFOLD (0, i), COLFOLD (i));
	}

	for (bp = bufs, i = 0; i < nbuf; i++)
	{
		for (k = 0, j = 0; j < BLKperPOINT; j++, bp++)
		{
			if (bp->b_flags & B_CLOCK)
				k++;
		}

		if (k == 0)
			charput ('.',     LNFOLD (1, i), COLFOLD (i));
		else
			charput ('0' + k, LNFOLD (1, i), COLFOLD (i));
	}

#if (0)	/*************************************/
	for (bp = bufs, i = 0; i < nbuf; i++, bp++)
	{
		if (bp->b_flags & B_DIRTY)
			charput ('*', LNFOLD (0, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (0, i), COLFOLD (i));
	}

	for (bp = bufs, i = 0; i < nbuf; i++, bp++)
	{
		if (bp->b_flags & B_CLOCK)
			charput ('*', LNFOLD (1, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (1, i), COLFOLD (i));
	}

	i = (*bclock - scb.y_bhead);
#endif	/*************************************/

	/*
	 *	Processa "bclock"
	 */
	i = (*bclock - scb.y_bhead) / BLKperPOINT;

	if (obclock != i)
	{
		charput (' ', LNFOLD (-1, obclock), COLFOLD (obclock));

		charput ('V', LNFOLD (-1, i), COLFOLD (i));

		obclock = i;
	}

	/*
	 *	Processa "bflushclock"
	 */
	i = (*bflushclock - scb.y_bhead) / BLKperPOINT;

	if (obflushclock != i)
	{
		charput (' ', LNFOLD (-1, obflushclock), COLFOLD (obflushclock));

		charput ('|', LNFOLD (-1, i), COLFOLD (i));

		obflushclock = i;
	}

}	/* end bufget */

/*
 ****************************************************************
 *	Coloca uma Cadeia na Tela				*
 ****************************************************************
 */
strput (cp, l, c)
register char		*cp;
register int		l, c;
{
	register ch;

	while ((ch = *cp++) != '\0')
		charput (ch, l, c++);

}	/* end strput */

/*
 ****************************************************************
 *	Coloca um Caracter na Tela				*
 ****************************************************************
 */
charput (ch, l, c)
int ch, l, c;
{
	if (l >= NLIN || c >= NCOL)
	{
		putchar (BELL);
		return;
	}

	if (tela[l][c] == ch)
		return;

	if (l != lin || c != col)
	{
		fputs (parmexec (info.i_strings[s_cup], l, c), stdout);
		lin = l;
		col = c;
	}

	tela[l][c] = ch;
	putchar (ch);
	col++;

}	/* end charput */

/*
 ****************************************************************
 *	Encerramento do Programa				*
 ****************************************************************
 */
quit ()
{
	fflush (stdout);
	ioctl (1, TCSETAW, &oldtty);
	exit (0);

}	/* end quit */

/*
 ****************************************************************
 *	Deu Bus Error						*
 ****************************************************************
 */
bus ()
{
	fflush (stdout);
	ioctl (1, TCSETAW, &oldtty);
	putchar ('\n');
	exit (0);

}	/* end bus */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help ()
{
	fprintf (stderr, "%s - Desenha um mapa de \"buffers\"\n", pgname);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-v] <período>\n", pgname);
	fprintf (stderr, "\nOpções:\t-v: verboso\n");
	fprintf (stderr, "\nArgs:\t<período>: Período entre atualizações ");
	fprintf (stderr, "(normalmente 1 segundo)\n");
	exit (2);

}	/* end help */
