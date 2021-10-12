/*
 ****************************************************************
 *								*
 *			sroff.h					*
 *								*
 *	Estruturas, constantes e variaveis do sistema.		*
 *								*
 *	Versão 2.0.0, de 11.05.86				*
 *	Versão 2.3.2, de 08.02.90				*
 *								*
 *	Modulo: SROFF						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definicoes de constantes.				*
 ****************************************************************
 */
#define		LNORMAL		"\e(s0B"
#define		LBOLD		"\e(s3B"
#define		LITON		"\e(s1S"
#define		LITOFF		"\e(s0S"
#define		LULON		"\e&dD"
#define		LULOFF		"\e&d@"

#define		NOSTR		(char *)NULL
#define		NOCHAR		(char)NULL

#define		ESPACO(x)	((x) == ' '  ||  (x) == '\t'  ||  \
						 (x) == '\f')

#define		CADEIA	0
#define		PALAVRA	1

#define		QUEBRA	0
#define		FORMATA	1

#define		BC	256 * 'b' + 'c'
#define		PL	256 * 'p' + 'l'
#define		LN	256 * 'l' + 'n'
#define		M1	256 * 'm' + '1'
#define		M2	256 * 'm' + '2'
#define		M3	256 * 'm' + '3'
#define		M4	256 * 'm' + '4'
#define		LS	256 * 'l' + 's'
#define		LL	256 * 'l' + 'l'
#define		PO	256 * 'p' + 'o'
#define		IN	256 * 'i' + 'n'
#define		HE	256 * 'h' + 'e'
#define		EH	256 * 'e' + 'h'
#define		OH	256 * 'o' + 'h'
#define		FO	256 * 'f' + 'o'
#define		EF	256 * 'e' + 'f'
#define		OF	256 * 'o' + 'f'
#define		BP	256 * 'b' + 'p'
#define		PN	256 * 'p' + 'n'
#define		TP	256 * 't' + 'p'
#define		BR	256 * 'b' + 'r'
#define		CE	256 * 'c' + 'e'
#define		TL	256 * 't' + 'l'
#define		SP	256 * 's' + 'p'
#define		UL	256 * 'u' + 'l'
#define		NF	256 * 'n' + 'f'
#define		LI	256 * 'l' + 'i'
#define		FI	256 * 'f' + 'i'
#define		PP	256 * 'p' + 'p'
#define		LP	256 * 'l' + 'p'
#define		IP	256 * 'i' + 'p'
#define		IT	256 * 'i' + 't'
#define		EP	256 * 'e' + 'p'
#define		AQ	256 * '(' + 'q'
#define		FQ	256 * ')' + 'q'
#define		AL	256 * '(' + 'l'
#define		FL	256 * ')' + 'l'
#define		AB	256 * '(' + 'b'
#define		FB	256 * ')' + 'b'
#define		AT	256 * '(' + 't'
#define		FT	256 * ')' + 't'
#define		AF	256 * '(' + 'f'
#define		FF	256 * ')' + 'f'
#define		R	256 * 'r'
#define		I	256 * 'i'
#define		B	256 * 'b'
#define		WO	256 * 'w' + 'o'
#define		SH	256 * 's' + 'h'
#define		NR	256 * 'n' + 'r'
#define		TS	256 * 't' + 's'
#define		TE	256 * 't' + 'e'
#define		TQ	256 * 't' + '&'

/*
 ****************************************************************
 *	Definicoes de tipos. 					*
 ****************************************************************
 */
typedef	enum
{
	FALSE,
	TRUE

}	BOOL;

typedef	enum
{
	ROMANO,
	ITALICO,
	NEGRITO

}	TPCHAR;

typedef	enum
{
	INICIO,
	FIM

}	TPDISTB;

typedef	struct	palvr	PALVR;

struct	palvr
{
	BOOL	p_negrito;
	BOOL	p_italico;
	BOOL	p_sublinhado;
	char	*p_palavra;

};

typedef	struct	titulo	TITULO;

struct	titulo
{
	char	*t_pont[3];
	BOOL	t_temtit;
	BOOL	t_negrito;
	BOOL	t_italico;
	BOOL	t_sublinhado;
};

typedef	struct	linha	LINHA;

struct	linha
{
	char	*l_lin;
	LINHA	*l_linprox;

};

typedef	struct	bloco	BLOCO;

struct	bloco
{
	int	b_nlinhas;
	LINHA	*b_plinha;
	BLOCO	*b_blcprox;

};

/*
 ****************************************************************
 *	Variaveis globais.					*
 ****************************************************************
 */
extern const char no_mem_text[];	/* Acabou a memória */
extern	char	*fn;			/* nome do arquivo corrente */
extern	BOOL	laser;			/* indica impressora a laser */
extern	BOOL	debug;			/* opcao "-d" */

extern	char	bufin[];		/* buffer de entrada */
extern	char	buftrab[];		/* palavras a serem formatadas */
extern	char	*cptrab;		/* proxima posicao livre de buftrab */
extern	PALVR	buflin[];		/* ponteiros para buftrab */
extern	char	bfout1[];		/* buffer de saida (linha formatada) */
extern	char	bfout2[];		/* buffer de saida (negrito) */
extern	char	bfout3[];		/* buffer de saida (sublinhado) */
extern	char	*out1;			/* ponteiro para bfout1 */
extern	char	*out2;			/* ponteiro para bfout2 */
extern	char	*out3;			/* ponteiro para bfout3 */
extern	TITULO	buftit[];		/* titulos */

extern	int	contpalvr;		/* numero de palavras em buftrab */
extern	int	contcarac;		/* numero de caracteres em buftrab */
extern	int	linhalida;		/* numero da linha corrente */
extern	int	linhaescrita;		/* numero da linha na pagina corrente */
extern	TPDISTB	distb;			/* colocacao de brancos excedentes */
extern	int	tampag;			/* tamanho da pagina (texto) */
extern	int	tamlin;			/* tamanho da linha (sem indentacao) */
extern	int	nbrancos;		/* indentacao total da linha */
extern	int	pagcorr;		/* numero da pagina corrente */

extern	BOOL	lit;			/* comando 'literal' em execucao */
extern	BOOL	lst;			/* comando 'lista' em execucao */
extern	int	cit;			/* indentacao no comando 'citacao' */
extern	int	pl;			/* tamanho total da pagina */
extern	int	ln;			/* linhas por polegada (laser) */
extern	int	m1;			/* margem 1 */
extern	int	m2;			/* margem 2 */
extern	int	m3;			/* margem 3 */
extern	int	m4;			/* margem 4 */
extern	int	ls;			/* espaco entre as linhas de texto */
extern	int	lsblc;			/* espaco antes de bloco ou rodape */
extern	int	lsrod;			/* espaco considerado no inicio
					   do rodape */
extern	int	lscit;			/* espaco antes da citacao */
extern	int	ll;			/* tamanho total da linha */
extern	int	po;			/* 'page offset' */
extern	int	in;			/* indentacao */
extern	int	ce;			/* numero de linhas a centrar */
extern	int	ul;			/* numero de linhas a sublinhar */
extern	int	pp;			/* paragrafo padrao */
extern	int	ip;			/* paragrafo indentado */
extern	BOOL	iptit;			/* linha com titulo no comado '.ip' */
extern	TPCHAR	tpchar;			/* tipo do caracter corrente */
extern	int	rpt2;			/* palavras em negrito na linha */
extern	int	rpt3;			/* palavras sublinhadas na linha */
extern	BOOL	tp;			/* 'title page' */
extern	BOOL	he;			/* 'header' */
extern	TITULO	*eh;			/* 'even header' */
extern	TITULO	*oh;			/* 'odd header' */
extern	BOOL	fo;			/* 'footer' */
extern	TITULO	*ef;			/* 'even footer' */
extern	TITULO	*of;			/* 'odd footer' */
extern	TITULO	*tl;			/* 'title' */

extern	int	nivel[];		/* nivel de titulacao */
extern	int	ultnivel;		/* ultimo nivel colocado */
extern	int	indentauto;		/* indentacao automatica dos titulos */

extern	BLOCO	*blccorr;		/* bloco corrente */
extern	LINHA	*lincorr;		/* linha corrente */
extern	BOOL	tembloco;		/* bloco em execucao */
extern	BOOL	temflut;		/* bloco flutuante em execucao */
extern	int	linflut;		/* numero de linhas de bloco flutuante
					   no fim da pagina corrente */
extern	BOOL	temrod;			/* rodape em execucao */
extern	int	linrod;			/* numero de linhas de rodape no fim
					   da pagina corrente */
extern	BOOL	rodinic;		/* inicio do rodape na pag. seguinte */
extern	BOOL	fimbloco;		/* bloco do tamanho da pagina */
extern	BLOCO	*blfixo;		/* fila de blocos fixos */
extern	BLOCO	*blflut;		/* fila de blocos flutuantes */
extern	BLOCO	*blrod;			/* fila de rodapes */
extern	BLOCO	*rodprox;		/* primeiro rodape da pagina seguinte */
extern	BLOCO	*blinic;		/* primeiro subbloco flutuante do ultimo
					   bloco */
extern	char	*cpblc;			/* linha do bloco */

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

