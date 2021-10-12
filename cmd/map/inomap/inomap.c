/*
 ****************************************************************
 *								*
 *			inomap.c				*
 *								*
 *	Desenha no Terminal um Mapa de INODEs			*
 *								*
 *	Versão	1.0.0, de 25.02.87				*
 *		2.3.0, de 04.09.89				*
 *								*
 *	Modulo: inomap						*
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
#include <sys/inode.h>
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

#define	BEGLN	3		/* No. da linha do comeco da tabela */
#define	BEGCOL	10		/* No. da coluna de comeco do mapa */
#define LARG	6		/* No. de linhas da informacao */

#define COLFOLD(c)	(c % (NCOL-BEGCOL) + BEGCOL)	
#define LNFOLD(l, c)	(c / (NCOL-BEGCOL) * LARG + BEGLN + l)	

#define		PERCENT(a, b)	((100*(a))/(b))

char		tela[NLIN][NCOL]; /* O grafico na tela */

int		lin, col;	/* linha e coluna correntes */

INODE		*inode;		/* comeco da tabela */

INODE		**iclock;	/* iclock */

int		oiclock = -1;	/* coluna do iclock anterior */

SYM		sym;		/* Entrada de tabela de simbolos */

char		*pgname;	/* Nome do Programa */

int		tempo;		/* Intervalo entre cada Mostra */

SCB		scb;		/* O bloco de controle */

TERMIO		oldtty,
		newtty;		/* Controle de Paginacao do Terminal */

char		buf[BUFSIZ];	/* Para o Terminal */

INFO		info;		/* Informações sobre o Terminal */

entry int	vflag;		/* Verbose */

extern int	optind;		/* Incremento do argv (em getopt) */

extern char	*optarg;	/* Argumento (de getopt) */

extern char	*parmexec ();

char		*btime ();
int		quit ();
int		bus ();
void		help ();

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
	 *	Mapeia a tabela de INODEs
	 */
	inode = (INODE *)phys ((char *)scb.y_inode,
			scb.y_ninode * sizeof (INODE), O_KERNEL|O_RDONLY);

	if ((int)inode < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear os INODEs\n",
					pgname);
		quit ();
	}

	/*
	 *	Mapeia o CLOCK
	 */
	if (kcntl (K_GETSYMGN, lidcan (lid, "_iclock"), &sym) < 0)
	{
		fprintf (stderr, "%s: ", pgname);
		fprintf (stderr,
			"Nao obtive o endereco de \"iclock\"\n");
		quit ();
	}

	iclock = (INODE **)phys ((char *)sym.s_value,
			sizeof (sym.s_value), O_KERNEL|O_RDONLY);

	if ((int)iclock < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear iclock\n",
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
	strput ("INODE MAP", 0, 35);

	strput ("USED",  BEGLN+0, 0);   
	strput ("LOCK",  BEGLN+1, 0);   
	strput ("TEXT",  BEGLN+2, 0);   
	strput ("CLOCK", BEGLN+3, 0);   

	strput ("USED",  BEGLN+LARG+0, 0);   
	strput ("LOCK",  BEGLN+LARG+1, 0);   
	strput ("TEXT",  BEGLN+LARG+2, 0);   
	strput ("CLOCK", BEGLN+LARG+3, 0);   

	strput ("USED",  BEGLN+2*LARG+0, 0);   
	strput ("LOCK",  BEGLN+2*LARG+1, 0);   
	strput ("TEXT",  BEGLN+2*LARG+2, 0);   
	strput ("CLOCK", BEGLN+2*LARG+3, 0);   

	fflush (stdout);

	/*
	 *	Inicia o Desenho.
	 */

	for (EVER)
	{
		/*
		 *	INODEs
		 */ 
		inodemap ();

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

}	/* end inomap */

/*
 ****************************************************************
 *	Obtem o Mapa dos BUFFERs 				*
 ****************************************************************
 */
int
inodemap ()
{
	register INODE	*ip;
	register	i, ninode;

	ninode =  scb.y_ninode;

	for (ip = inode, i = 0; i < ninode; i++, ip++)
	{
		if (ip->i_count)
			charput ('*', LNFOLD (0, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (0, i), COLFOLD (i));
	}

	for (ip = inode, i = 0; i < ninode; i++, ip++)
	{
		if (ip->i_lock)
			charput ('*', LNFOLD (1, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (1, i), COLFOLD (i));
	}

	for (ip = inode, i = 0; i < ninode; i++, ip++)
	{
		if (ip->i_flags & ITEXT)
			charput ('*', LNFOLD (2, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (2, i), COLFOLD (i));
	}

	for (ip = inode, i = 0; i < ninode; i++, ip++)
	{
		if (ip->i_flags & ICLOCK)
			charput ('*', LNFOLD (3, i), COLFOLD (i));
		else
			charput ('.', LNFOLD (3, i), COLFOLD (i));

	}

	i = (*iclock - scb.y_inode);

	if (oiclock != i)
	{
		charput (' ', LNFOLD (-2, oiclock), COLFOLD (oiclock));
		charput (' ', LNFOLD (-1, oiclock), COLFOLD (oiclock));

		charput ('|', LNFOLD (-2, i), COLFOLD (i));
		charput ('V', LNFOLD (-1, i), COLFOLD (i));

		oiclock = i;
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
	fprintf (stderr, "%s - Desenha um mapa de \"inodes\"\n", pgname);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-v] <período>\n", pgname);
	fprintf (stderr, "\nOpções:\t-v: verboso\n");
	fprintf (stderr, "\nArgs:\t<período>: Período entre atualizações ");
	fprintf (stderr, "(normalmente 1 segundo)\n");
	exit (2);

}	/* end help */
