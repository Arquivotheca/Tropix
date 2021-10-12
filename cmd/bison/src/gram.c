/*
 ****************************************************************
 *								*
 *			gram.c					*
 *								*
 *	Allocate input grammar variables for bison		*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

/* comments for these variables are in gram.h  */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
int nitems;
int nrules;
int nsyms;
int ntokens;
int nvars;

short *ritem;
short *rlhs;
short *rrhs;
short *rprec;
short *rprecsym;
short *sprec;
short *rassoc;
short *sassoc;
short *token_translations;
short *rline;

int start_symbol;

int translations;

int max_user_token_number;

int semantic_parser;

int pure_parser;

int error_token_number;
