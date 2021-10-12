/*
 ****************************************************************
 *								*
 *			msg.h					*
 *								*
 *	Definições acerca de mensagens de erro			*
 *								*
 *	Versão	1.0.0, de 21.01.86				*
 *		4.1.0, de 25.08.01				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****** Tipo de erro ********************************************
 */
/*** typedef	enum errtype	ERRTYPE; ***/

enum errtype
{
	ADVR,		/* Advertência */
	ERRO,		/* Erro */
	COMP		/* Erro do Montador */
};

/*
 ****** Tipo de ponteiro ****************************************
 */
/*** typedef	enum marktype	MARKTYPE; ***/

enum marktype
{
	NOMARK,		/* Sem indicação do local do erro */
	SHORTMARK,	/* Indicador de 1 Caractere ('A') */
	LONGMARK,	/* Indicador de Varios Caracteres ('|/////|') */
	LASHORTMARK,	/* Indicador de 1 Caractere ('A') do LA */
	LALONGMARK,	/* Indicador de Varios Caracteres ('|/////|') do LA */
	MYMARK,		/* Local Indicado por quem chama */
	EXPMARK,	/* Local Indicado pela expressão */
	OPNDMARK	/* Local Indicado pelo operando */
};

/*
 ****** Protótipos e variáveis externas *************************
 */
extern	short	lineprinted;
