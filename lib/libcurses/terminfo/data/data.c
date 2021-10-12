/*
 ****************************************************************
 *								*
 *			data.c					*
 *								*
 *	Dados do interpretador de cadeias de parâmetros		*
 *								*
 *	Versão	2.0.0, de 07.05.87				*
 *		3.0.0, de 24.02.95				*
 *								*
 *	Modulo: data						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Variáveis exportadas					*
 ****************************************************************
 */
#define	PARMSZ	255

entry char	*_parmerror;		/* Mensagem de Erro */
entry char	*_parmptr;		/* Local do Erro */
entry char	*_parmrtn;		/* Área[PARMSZ] para retorno das Funções */
