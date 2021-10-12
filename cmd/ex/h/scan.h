/*
 ****************************************************************
 *								*
 *			scan.h					*
 *								*
 *	Definições para o uso do "scanner"			*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	H_SCAN		/* Para declarar as funções */

/*
 ******	Tipo "token" ********************************************
 */
typedef	struct
{
	char	*t_begin;
	char	*t_end;

}	TOKEN;

/*
 ******	Tipo da marca de erro ***********************************
 */
typedef	enum { NOMARK, SHORTMARK, LONGMARK }	ERROR;
