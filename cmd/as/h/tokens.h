/*
 ****************************************************************
 *								*
 *			tokens.h				*
 *								*
 *	Tokens reconhecidos pelo parser				*
 *								*
 *	Versão	1.0.0, de 21.01.86				*
 *		3.0.0, de 26.02.94				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Os tipos de tokens **************************************
 */

/*** typedef	enum major	MAJOR; ***/

enum major
{
	BLANK,		/* Separador */
	EOL,		/* Final da Linha */
	ID,		/* Identificador */
	LID,		/* Simbolo local */
	LOCCNT,		/* Locate Counter (.) */
	CTE,		/* Constante */
	STRING,		/* Cadeia */
	OP,		/* Operador */
	NUMBER,		/* # */
	LP,		/* ( */
	RP,		/* ) */
	CM,		/* , */
	CL,		/* : */
	SM,		/* ; */
	EQ,		/* = */
	AT,		/* @ */
	LB,		/* [ */ 
	RB,		/* ] */
	LC,		/* { */
	RC,		/* } */
	SIZE,		/* :b, :w ou :l */
	SCALE,		/* @b, @w, @l ou @q */
	RCTE		/* Constante real */

};
