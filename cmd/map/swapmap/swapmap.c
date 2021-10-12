/*
 ****************************************************************
 *								*
 *			swapmap.c				*
 *								*
 *	Desenha no Terminal um Mapa do Swap			*
 *								*
 *	Versão	1.0.0, de 04.07.86				*
 *		2.3.0, de 04.09.89				*
 *								*
 *	Modulo: Coremap.c					*
 *		Utilitario Especial				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <sys/common.h>
#include <sys/scb.h>
#include <sys/file.h>
#include <sys/map.h>
#include <sys/tty.h>
#include <curses/info.h>
#include <curses/capaccess.h>


#if (0)	/*************************************/

	/*
	 *	Põe o cabeçalho
	 */
	fputs (parmexec (info.i_strings[s_cup], 21, 24), stdout);

	printf ("COREMAP (* %d bytes)\n", DELTA);

	
		/*
		 *	Coloca a Mensagem no final da tela
		 */
		fputs (parmexec (info.i_strings[s_cup], 23, 0), stdout);

	if (oldad != addr - 1 || addr % MAXCOL == 0)
	{
		fputs
		(	parmexec
			(	info.i_strings[s_cup],
				1 + addr / MAXCOL,
				5 + addr % MAXCOL
			),
			stdout
		);
	}
#endif

/*
 ****************************************************************
 *	Variaveis e Definicoes Globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	2.3.0, de 04.09.89";

#define	NOMAP	(MAP *)NULL

#define	MAXCHAR	(19*64)		/* Nr. Max. de caracteres da tela */

#define	MAXCOL	64		/* Nr. de Caracteres por linha */

#define	PERCENT(a, b)	(int)((100*(long)(a))/(b))

pg_t		MXSWA;		/* Tamanho do Swap em Blocos */

short		KMXSWA;		/* Tamanho do Swap em Kb */

short		BETA;		/* Nr. de Blocos por Caractere */

short		ENDSCREEN;	/* Nr. de caracteres usados da tela */

MAP		*map;		/* "Instantaneo" da Swapmap */

MAP		*swapmap;	/* Ponteiro para a Swapmap */

int		tempo;		/* Intervalo entre cada Mostra */

short		oldad = -2;	/* Endereco Atual da Tela */

TERMIO		oldtty,
		newtty;		/* Controle de Paginacao do Terminal */

char		buf[BLSZ];	/* Para o Terminal */

char		tela[MAXCHAR];	/* O grafico na tela */

INFO		info;		/* Informações sobre o Terminal */

SCB		scbbuf;

entry char	*pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

extern int	optind;		/* Incremento do argv (em getopt) */

extern char	*optarg;	/* Argumento (de getopt) */

extern char	*parmexec ();
void		help ();
void		print ();
void		copia();
void		quit ();
long		phys ();

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
	register SCB	*scb;
	register MAP	*swappt;
	register short	ndmap;
	register short	addr;
	register short	sz;
	register short	unused, used;
	register short	minhole, maxhole, nhole;
	register short	svad;
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

	/*
	 *	Obtem as características do terminal
	 */
	if (getinfo (NOSTR, &info) == 0)
		exit (1);

	/*
	 *	Obtem o System Control Block.
	 */
	if (getscb (scb = &scbbuf) < 0)
	{
		fprintf (stderr, "%s: Nao foi possivel copiar o SCB\n", pgname);
		quit ();
	}

	/*
	 *	Verifica se o Intervalo de tempo foi especificado.
	 */
	if (argc > 0)
		tempo = atoi (argv[0]);
	if (tempo <= 0)
		tempo = 1;

	setbuf (stdout, buf);

	/*
	 *	Mapeia as Tabelas
	 */
	MXSWA	= scb->y_nswap;
	ndmap   = scb->y_ndmap;
	swapmap = (MAP *)phys ((long)scb->y_diskmap,
				ndmap * sizeof (MAP), O_KERNEL|O_READ);
	if ((int)swapmap == NIL)
	{
		fprintf (stderr, "%s: Nao foi possivel mapear a swapmap\n",
					pgname);
		quit ();
	}

	/*
	 *	Aloca o Instantaneo da Tabela.
	 */
	if ((map = (MAP *)malloc (ndmap * sizeof (MAP))) == NOMAP)
	{
		fprintf (stderr, "%s: Memoria excedida\n", pgname);
		quit ();
	}

	/*
	 *	Calcula os Parametros globais da Memoria
	 */
	BETA	  = (MXSWA + MAXCHAR - 1) / MAXCHAR;
	KMXSWA	  = BLTOKB (MXSWA);
	ENDSCREEN = MXSWA / BETA;

	fputs (info.i_strings[s_clear], stdout);
	printf ("\t\t\tSWAPMAP (* %d bytes)\n", BLTOBY (BETA));

	for (addr = 0; addr < KMXSWA; addr += BLTOKB (MAXCOL * BETA))
	{
		printf ("\n%4d", addr);
	}

	/*
	 *	Desfaz o Mapeamento do SCB.
	 */
	phys ((long)scb, 0, 0);

	/*
	 *	Loop fazendo o Grafico na tela
	 */
	for (EVER)
	{
		unused = 0;
		maxhole = minhole = nhole = 0;
		oldad = -2;

		/*
		 *	Tira o instantaneo da Tabela.
		 */
		copia ((int *)map, (int *)swapmap, ndmap * sizeof (MAP));

		/*
		 *	Varre a tabela, desenhando na tela.
		 */
		swappt = map;
		for (addr = 0; swappt->a_size != 0; swappt++)
		{
			nhole++;

			/*
			 *	Determina o maior e menor buracos
			 */
			if (swappt->a_size > maxhole)
				maxhole = swappt->a_size;
			if (swappt->a_size < minhole || minhole == 0)
				minhole = swappt->a_size;

			unused += swappt->a_size;
			svad   = swappt->a_addr / BETA;
			sz     = swappt->a_size;

			/*
			 *	Imprime a Regiao Cheia.
			 */
			for (/* vazio */; addr < svad; addr++)
			{
				print (addr, '*');
			}

			/*
			 *	Se nao termina em vizinhanca de Kb,
			 *	coloca um 'x' no final.
			 */
			if ((svad *= BETA) != swappt->a_addr)
			{
				print (addr, 'x');
				if ((sz -= BETA - swappt->a_addr + svad) < 0)
				sz = 0;
				addr++;
			}

			/*
			 *	Imprime o Buraco.
			 */
			for (/* vazio */; sz >= BETA; sz -= BETA, addr++)
			{
				print (addr, '.');
			}

			if (sz && swappt[1].a_size != 0)
			{
				print (addr, 'x');
				addr++;
			}
		}
	
		/*
		 *	Completa as porcoes vazias com 'o'.
		 */
		for (/* vazio */; addr < ENDSCREEN; addr++)
		{
			print (addr, 'o');
		}
	
		/*
		 *	Coloca a Mensagem no final da tela
		 */
		fputs (parmexec (info.i_strings[s_cup], 22, 0), stdout);
#if (0)	/*************************************/
		printf ("\033Y");
		putchar (' ' + 22);
		putchar (' ');
#endif	/*************************************/

		used = MXSWA - unused;
		printf ("Total = %d Kb * Used = %d Kb * Percent = %d %%  \n",
			BLTOKB (MXSWA), BLTOKB (used),
			PERCENT (used, MXSWA));

/*****************************
		printf ("Nr. holes = %u *", nhole);
		printf (" Max. Hole = %u Kb * Min. Hole = %u b  ",
			BLTOKB (maxhole), BLTOBY (minhole));
******************************/

		fflush (stdout);
		sleep (tempo);

	}	/* end for (EVER) */

}	/* end swapmap */

/*
 ****************************************************************
 *	Imprime um Caractere					*
 ****************************************************************
 */
void
print (addr, ch)
register short addr;
register char ch;
{
	if (tela[addr] == ch)
		return;

	tela[addr] = ch;

	if (oldad != addr - 1 || addr % MAXCOL == 0)
	{
		fputs
		(	parmexec
			(	info.i_strings[s_cup],
				2 + addr / MAXCOL,
				5 + addr % MAXCOL
			),
			stdout
		);
	}

#if (0)	/*************************************/
		putchar ('\033');
		putchar ('Y');
		putchar (' ' + 2 + addr / MAXCOL);
		putchar (' ' + 5 + addr % MAXCOL);
#endif	/*************************************/

	oldad = addr;
	putchar (ch);
}

/*
 ****************************************************************
 *	Tira um "Instantaneo" da Tabela				*
 ****************************************************************
 */
void
copia (dest, fonte, nbytes)
register int *dest, *fonte;
{
	register short	npal;

	npal = nbytes / sizeof (int);

	while (npal-- > 0)
		*dest++ = *fonte++;

}	/* end copia */

/*
 ****************************************************************
 *	Encerramento do Programa				*
 ****************************************************************
 */
void
quit ()
{
	fflush (stdout);
	ioctl (1, TCSETAW, &oldtty);
	putchar ('\n');
	exit (0);

}	/* end quit */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help ()
{
	fprintf (stderr, "%s - Desenha um mapa do \"swap\"\n", pgname);
	fprintf (stderr, "\n%s\n", pgversion);
	fprintf (stderr, "\nSintaxe:\n");
	fprintf (stderr, "\t%s [-v] <período>\n", pgname);
	fprintf (stderr, "\nOpções:\t-v: verboso\n");
	fprintf (stderr, "\nArgs:\t<período>: Período entre atualizações ");
	fprintf (stderr, "(normalmente 1 segundo)\n");
	exit (2);

}	/* end help */
