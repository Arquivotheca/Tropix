/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Definições universais					*
 *								*
 *	Versão	1.1.0, de 11.01.88				*
 *		4.2.0, de 27.04.02				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definições universais					*
 ****************************************************************
 */
#define	EVER	;;
#define	elif	else if
#define NOVOID	(void *)NULL
#define	NOPTR	(void *)NULL

#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	STDIN	0
#define STDOUT	1

#define	MIN(a,b)	((a) < (b) ? (a) : (b))

typedef unsigned short	ushort;	/* 16 bites sem sinal */

/*
 ******	Indicadores *********************************************
 */
extern int		Tflag;		/* Gera o formato TAR */
extern int		Dflag;		/* NÃO Comprime */
extern int		nflag;		/* Interativo */
extern int		uflag;		/* Comparação com atualização */
extern int		rflag;		/* Sempre cheque os CRCs */
extern int		vflag;		/* Verbose */
extern int		dflag;		/* Diretorio Corrente */
extern int		fflag;		/* Copia mesmo que já exista */
extern int		mflag;		/* Restaura dono, grupo e datas */
extern int		gflag;		/* Processa o "garfield" */
extern int		dotflag;	/* Não ignora ".old" etc ... */
extern int		crcflag;	/* Não gera CRC na compressão */
extern int		Gflag;		/* Debug */

/*
 ******	Declarações da deflação *********************************
 */
#define DEF_LEVEL	5	/* Nível de compactação normal (ZIP) */
#define MIN_LEVEL	1	/* Nível de compactação mínimo (ZIP) */
#define MAX_LEVEL	9	/* Nível de compactação máximo (ZIP) */

extern int		ZIP_LEVEL;	/* Os níveis podem ser de 1..9 */

/*
 ****** Variáveis externas **************************************
 */
extern const char	headerversion[]; /* Versão do programa */

extern int		exit_code;	/* Código de retorno */

extern int		file_block;	/* No. do bloco do arquivo na coleção */ 

/*
 ******	Buffer de Entrada/Saida *********************************
 */
extern int		col_area_sz;		/* Tamanho do buffer em vigor (blocagem) */

extern int		col_area_avail_sz;	/* No. de bytes disponiveis no buffer */

extern int		col_is_a_tape;		/* A coleção é uma fita */

/*
 ****** Variáveis diversas **************************************
 */
extern const char	*g_filenm;	/* Nome do Arquivo em Processamento */
extern int		g_filenmput;	/* O Nome do Arquivo ja foi impresso */
extern int		g_nlput;	/* O <nl> já foi escrito */

extern time_t		date;		/* Data de criação da Coleção GAR */

extern int		vol;		/* No. do Volume GAR */

extern FILE		*ttyfp;		/* Para ler do terminal */

extern const char	**color_vector;	/* Para as cores dos modos */

/*
 ****** Garfield ************************************************
 */
extern const char	garfield[];
extern FILE		*gfp;

/*
 ****** Garstamp ************************************************
 */
extern const char	garstamp[];
extern time_t		garstamp_time;

/*
 ****** Coleção *************************************************
 */
extern int		device_code;	/* Código do dispositivo */
extern const char	*device_nm; 	/* Nome do Dispositivo */

extern int		colfd;		/* File Descriptor da Coleção */
extern dev_t		coldev;		/* Dispositivo da Coleção */
extern ino_t		colino;		/* Inode da Coleção */

extern char		*reduce_nm0,	/* Nomes do arquivo temporário */
			*reduce_nm1,
			*reduce_nm2;

extern long		totalbytes;	/* No. Total de Bytes lidos/escritos */

extern off_t		col_offset;	/* Tamanho da coleção */

extern off_t		col_max_offset;	/* Tamanho máximo da coleção */

/*
 ******	Protótipos de funções ***********************************
 */
extern void		(*function) (void *, int);

extern off_t		deflate (int, FILE *);
extern off_t		inflate (void (*) (), int);
extern void		msg (char *, ...);
extern off_t		reduce (int, FILE *);
extern off_t		unreduce (void (*) ());
extern char		*editpercent (int);
extern int		getpercent (long, long);
extern void		bread (void *, int);
extern void		bwrite (const void *, int);
extern void		on_signal (int, ...);
extern const char	*edit_BY_to_KB (ulong bytes);

#ifdef	STAT_H
extern void		get_dev_size (int, STAT *sp);
extern void		print_file_nm (const char *, const STAT *, const char *, FILE *);
#endif	STAT_H

