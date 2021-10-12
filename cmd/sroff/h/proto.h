/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Protótipos de funções					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
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

FILE		*abre (char *);
void		altvar (int, char *);
void		atlarg (void);
void		atlinflut (void);
void		atnbrancos (void);
void		attamlin (void);
void		attampag (void);
int		calcnumcol (void);
void		centra (void);
BOOL		codiftit (char *, TITULO *, TITULO *);
void		compnegr (TPCHAR, char, int, char *);
void		compul (int, char *);
int		contacarac (char *);
BLOCO		*criablc (void);
char		*crialbuf (char *, char *, char *);
void		errcmd (void);
void		fimcit (void);
void		fimflut (void);
void		fimimpr (void);
void		fimpag (void);
void		fimrod (void);
void		help (void);
int		identcarac (char **);
void		imprdiv (void);
void		imprfixo (void);
void		imprflut (void);
void		imprrod (void);
void		inicdiv (char *);
void		inicia (void);
void		inicimpr (void);
void		iniciocit (void);
void		inicpag (void);
void		inictbl (void);
void		iparag (int, char *);
char		*lelinha (FILE *);
int		lenum (char **, char **);
char		*lepalvr (char *, char **);
void		libtbl (void);
void		lista (void);
void		literal (void);
void		mostra (void);
void		mtsub (void);
void		mttbl (void);
void		parag (int);
char		*poebr (int, char *);
void		poelin (void);
void		poenl (int);
char		*poepalvr (char *, int);
void		poetit (TITULO *);
char		*preenche (char *);
void		procarq (FILE *);
void		secnum (char *);
void		titpag (void);
void		trtblc (char *);
void		trtcarac (int, char *);
void		trtcmd (void);
void		trtdata (void);
void		trtfmt (void);
void		trtlin (int);
void		trtopcao (void);
void		trtpag (int);
void		trtrod (void);
void		trttit (int, char *);
void		trttxt (void);
void		trtwo (char *);

#ifdef	TBL_H
int		alelemto (DADOS *, char *);
int		calclarg (FORMATO *, int, int);
int		formcmp (int, DADOS *, FORMATO *, int, int);
char		*identatr (char *, FORMATO *, int);
void		incr (int, FORMATO *);
void		inicdat (DADOS *);
#endif	TBL_H
