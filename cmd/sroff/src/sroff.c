/*
 ****************************************************************
 *								*
 *			sroff.c					*
 *								*
 *	Formatador de textos					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		4.7.0, de 28.27.04				*
 *								*
 *	Rotinas:						*
 *		main	abre					*
 *		inicia	help					*
 *								*
 *	Modulo: SROFF						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include  <a.out.h>
#include  <errno.h>

#include "../h/sroff.h"
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis globais.					*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.7.0, de 28.12.04";

const char	no_mem_text[] =  "%s: Memória esgotada";

entry char	*fn;		/* nome do arquivo corrente */
entry BOOL	debug;		/* opcão "-d" */
entry BOOL	laser;		/* indica impressora a laser */

entry char	bufin[BLSZ];	/* buffer de entrada */
entry char	buftrab[BLSZ]; /* palavras a serem formatadas */
entry char	*cptrab;	/* proxima posicão livre de buftrab */
entry PALVR	buflin[100];	/* ponteiros para buftrab */
entry char	bfout1[BLSZ];	/* buffer de saida (linha formatada) */
entry char	bfout2[BLSZ];	/* buffer de saida (negrito) */
entry char	bfout3[BLSZ];	/* buffer de saida (sublinhadas) */
entry char	*out1;		/* ponteiro para bfout1 */
entry char	*out2;		/* ponteiro para bfout2 */
entry char	*out3;		/* ponteiro para bfout3 */
entry TITULO	buftit[5];	/* titulos */

entry int	contpalvr;	/* numero de palavras em buftrab */
entry int	contcarac;	/* numero de caracteres em buftrab */
entry int	linhalida;	/* numero da linha corrente */
entry int	linhaescrita;	/* numero da linha na pagina corrente */
entry TPDISTB	distb;		/* colocacão de brancos excedentes */
entry int	tampag;		/* tamanho da pagina (texto) */
entry int	tamlin;		/* tamanho da linha (sem indentacão) */
entry int	nbrancos;	/* indentacão total da linha */
entry int	pagcorr;	/* numero da pagina corrente */

entry BOOL	lit;		/* comando 'literal' em execucão */
entry BOOL	lst;		/* comando 'lista' em execucão */
entry int	cit;		/* indentacão no comando 'citacão' */
entry int	pl;		/* tamanho total da pagina */
entry int	ln;		/* linhas por polegada (laser) */
entry int	m1;		/* margem 1 */
entry int	m2;		/* margem 2 */
entry int	m3;		/* margem 3 */
entry int	m4;		/* margem 4 */
entry int	ls;		/* espaco entre as linhas de texto */
entry int	lsblc;		/* espaco antes de bloco ou rodape */
entry int	lsrod;		/* espaco considerado no inicio do rodape */
entry int	lscit;		/* espaco antes da citacão */
entry int	ll;		/* tamanho total da linha */
entry int	po;		/* 'page offset' */
entry int	in;		/* indentacão */
entry int	ce;		/* numero de linhas a centrar */
entry int	ul;		/* numero de linhas a sublinhar */
entry int	pp;		/* paragrafo padrão */
entry int	ip;		/* paragrafo indentado */
entry BOOL	iptit;		/* linha com titulo no comado '.ip' */
entry TPCHAR	tpchar;		/* tipo do caracter corrente */
entry int	rpt2;		/* palavras em negrito na linha */
entry int	rpt3;		/* palavras sublinhadas na linha */
entry BOOL	tp;		/* 'title page' */
entry BOOL	he;		/* 'header' */
entry TITULO	*eh;		/* 'even header' */
entry TITULO	*oh;		/* 'odd header' */
entry BOOL	fo;		/* 'footer' */
entry TITULO	*ef;		/* 'even footer' */
entry TITULO	*of;		/* 'odd footer' */
entry TITULO	*tl;		/* 'title' */

entry int	nivel[4];	/* nivel de titulacão */
entry int	ultnivel;	/* ultimo nivel colocado */
entry int	indentauto;	/* indentacão automatica dos titulos */

entry BLOCO	*blccorr;	/* bloco corrente */
entry LINHA	*lincorr;	/* linha corrente */
entry BOOL	tembloco;	/* bloco em execucão */
entry BOOL	temflut;	/* bloco flutuante em execucão */
entry int	linflut;	/* numero de linhas de bloco flutuante no fim da
				   pagina corrente */
entry BOOL	temrod;		/* rodape em execucão */
entry int	linrod;		/* numero de linhas de rodape no fim da pagina
				   corrente */
entry BOOL	rodinic;	/* inicio do rodape na pagina seguinte */
entry BOOL	fimbloco;	/* bloco do tamanho da pagina */
entry BLOCO	*blfixo;	/* fila de blocos fixos */
entry BLOCO	*blflut;	/* fila de blocos flutuantes */
entry BLOCO	*blrod;		/* fila de rodapes */
entry BLOCO	*rodprox;	/* primeiro rodape da pagina seguinte */
entry BLOCO	*blinic;	/* primeiro subbloco flutuante do ultimo
				   bloco */
entry char	*cpblc;		/* linha do bloco */

entry COLUNA	*crtcol;	/* caracteristicas das colunas */
entry int	numcol;		/* numero de colunas */
entry int	tbl;		/* estado da tabela */
entry FORMATO	**fmtpont;	/* ponteiros para vetor de formatos */
entry int	contfmt;	/* numero de formato atual */
entry int	maxfmt;		/* numero total de formatos */
entry DADOS	**dtpont;	/* ponteiros para vetor de dados */
entry int	contdata;	/* numero de dados atual */
entry int	maxdata;	/* numero total de dados */
entry int	inicfmt;	/* inicio dos formatos da ultima subtabela */
entry BOOL	center;		/* centra a tabela */
entry BOOL	expand;		/* expande a tabela */
entry BOOL	box;		/* a tabela fica em uma moldura */
entry BOOL	doublebox;	/* a tabela fica em uma moldura dupla */
entry BOOL	allbox;		/* cada elemento da tabela fica em moldura */
entry char	tab;		/* separador na entrada de dados */
entry long	lintit;		/* linha do título para a quebra de página */
entry BOOL	temtit;		/* há título na quebra de página */
entry BOOL	titul;		/* o título da quebra de página é sublinhado */
entry BOOL	titneg;		/* o título da quebra de página é negrito */

/*
 ****************************************************************
 *	Variaveis locais.					*
 ****************************************************************
 */
entry BOOL	Nflag;
entry char	nome[BLSZ];

/*
 ****************************************************************
 *	Formatador de textos					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	register FILE	*fp;

	laser = FALSE;
	debug = FALSE;

	/*
	 *	Identifica opcoes.
	 */
	while ((opt = getopt (argc, argv, "ldNH")) != EOF)
	{
		switch (opt)
		{
		    case 'l':
			laser = TRUE;
			break;

		    case 'd':
			debug = TRUE;
			break;

		    case 'N':
			Nflag = TRUE;
			break;

		    case 'H':
			help ();

		    default:
			putc ('\n', stderr);
			help ();
		}
	}
	argv += optind;

	/*
	 *	Verifica uso correto do programa.
	 */
	if (Nflag  &&  *argv != NOSTR)
		help ();

	inicia ();

	if (laser)
	{
		fprintf (stdout, "\eE");	/* Reset */
		fprintf (stdout, "\e&a0L");	/* Margem Esquerda */
		fprintf (stdout, "\e(0N");	/* ISO */
	}

	if (Nflag)
	{
		fn = nome;
		while (fngets (fn, BLSZ, stdin) != NOSTR)
		{
			if ((fp = abre (fn)) != NOFILE)
			{
				procarq (fp);
				fclose (fp);
			}
		}
	}
	else
	{
		if (*argv == NOSTR)
		{
			fn = "stdin";
			procarq (stdin);
		}
		else
		{
			while (*argv != NOSTR)
			{
				if ((fp = abre ((char *)*argv)) != NOFILE)
				{
					fn =  (char *)*argv;
					procarq (fp);
					fclose (fp);
				}
				argv++;
			}
		}
	}

	if (tembloco  ||  temflut  ||  temrod  ||
			lit  ||  lst  ||  cit != 0  ||  tbl != 0)
	{
		if (laser)
			fprintf (stdout, "\eE");

		fprintf
		(	stderr,
			"%s: Final inesperado do arquivo \"%s\"\n",
			pgname, fn
		);

		exit (1);
	}
		
/*****
	if (linflut == 0)
	{
******/
		/*
		 *	Imprime os blocos flutuantes do fim da pagina.
		 */
		while (blflut != (BLOCO *)NULL)
		{
			if (blflut->b_nlinhas > (tampag - linhaescrita))
				trtpag (-1);

			if (linhaescrita == 0)
				inicpag ();

			imprflut ();

			if (linhaescrita == tampag)
				fimpag ();
		}
/*****
	}
******/

	if (linhaescrita != 0)
		/*
		 *	Quebra a pagina.
		 */
		trtpag (-1);

	if (laser)
		fprintf (stdout, "\eE");

	return (0);

}	/* end main */	

/*
 ****************************************************************
 *	Abre e verifica as condicoes do arquivo.		*
 ****************************************************************
 */
FILE *
abre (char *file_nm)
{
	register FILE	*fp;
	long		magic;
	STAT		s;
	HEADER		h;

	/*
	 *	Abre o arquivo.
	 */
	if ((fp = fopen (file_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		return (NOFILE);
	}

	/*
	 *	Le o status do arquivo.
 	 */
	if (fstat (fileno (fp), &s) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, file_nm, strerror (errno)
		);
		return (NOFILE);
	}

	/*
	 *	Verifica se o arquivo é regular.
	 */
	if ((s.st_mode & S_IFMT) != S_IFREG)
	{
		fprintf
		(	stderr, 
			"%s: \"%s\" não é um arquivo regular\n",
			pgname, file_nm
		);
		return (NOFILE);
	}

	/*
	 *	Testa se é um programa objeto
	 *	Apenas se não for "stdin" nem FIFO,
	 *	pois não podemos dar "rewind" em um pipe
	 */
	if (fp != stdin && !S_ISFIFO (s.st_mode))
	{
		if ((fread (&h, sizeof (HEADER), 1, fp)) == 1)
		{
			magic = ENDIAN_SHORT (h.h_magic);

			if (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC)
			{
				fprintf
				(	stderr,
					"%s: \"%s\" é um programa objeto\n",
					pgname, file_nm
				);
				return (NOFILE);
			}
		}

		rewind (fp);

	}	/* end 	if (fp != stdin) */

	return (fp);

}	/* end abre */

/*
 ****************************************************************
 *	Inicia as variaveis do sistema.				*
 ****************************************************************
 */
void
inicia (void)
{
	register int	i;

	cptrab = buftrab;
	out1 = bfout1;
	out2 = NOSTR;
	out3 = NOSTR;
	rpt2 = 0;
	rpt3 = 0;

	contpalvr = contcarac = 0;
	linhalida = linhaescrita = 0;
	distb = INICIO;
	pagcorr = 1;
	lit = lst = FALSE;
	ls = 1;

	he = fo = FALSE;
	eh = &(buftit[0]);
	oh = &(buftit[1]);
	ef = &(buftit[2]);
	of = &(buftit[3]);
	tl = &(buftit[4]);
	tl->t_temtit = TRUE;

	pl = 75;
	ln = 6;
	m1 = 4;
	m2 = m3 = 2;
	m4 = 1;
	tp = FALSE;

	ll = 67;
	po = in = pp = ip = cit = 0;
	attamlin ();
	atnbrancos ();

	ce = ul = 0; 
	iptit = FALSE;
	tpchar = ROMANO;


	for (i = 0; i < 4; i++)
		nivel[i] = 0;
	ultnivel = 0;
	indentauto = 0;

	blccorr = blfixo = blflut = (BLOCO *)NULL;
	blinic = blrod = rodprox = (BLOCO *)NULL;
	lincorr = (LINHA *)NULL;
	temrod = temflut = tembloco = fimbloco = FALSE;
	linrod = linflut = 0;
	attampag ();

}	/* end inicia */

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
		"%s - formatador de textos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-lN] [<texto> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Gera o texto formatado para impressora \"laser\"\n"
		"\t-N: Lê os nomes dos <texto>s de \"stdin\"\n"
		"\nObs.:\tSe não forem dados <texto>s nem a opção \"-N\",\n"
		"\té editado o texto lido de \"stdin\"\n"
	);

	exit (2);

}	/* end help */
