/*
 ****************************************************************
 *								*
 *			error.h					*
 *								*
 *	Definições acerca de mensagens de erro			*
 *								*
 *	Versão	3.2.3, de 03.01.00				*
 *		4.4.0, de 10.12.02				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Tipo da marca de indicação do erro			*
 ****************************************************************
 */
typedef	enum
{
	SHORTMARK,	/* Indicador de 1 Caractere ('^') */
	LONGMARK	/* Indicador de vários caracteres ('|/////|') */

}	MARKTYPE;

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void	sherror (MARKTYPE, const char *, ...);
