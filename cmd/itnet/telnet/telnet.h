/*
 ****************************************************************
 *								*
 *			telnet.h				*
 *								*
 *	Comunicação com estação remota (definições)		*
 *								*
 *	Versão 	3.0.0, de 02.04.93				*
 *	 	3.0.2, de 19.08.97				*
 *								*
 *	Módulo: ITNET						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Códigos dos comandos TELNET				*
 ****************************************************************
 */
typedef enum
{
	SE = 240,	/* End of subnegotiation parameters */
	NOP,		/* No operation */
	DM,		/* Data mark */
	BRK,		/* Break */
	IP,		/* Interrupt Process */
	AO,		/* Abort Output */
	AYT,		/* Are you there? */
	EC,		/* Erase character */
	EL,		/* Erase line */
	GA,		/* Go ahead */
	SUB,		/* Subnegotiation of option */
	WILL,		/* Desire to begin or confirmation of performance */
	WONT,		/* Refusal to begin or continuation of performance */
	DO,		/* Request to perform or confirmation */
	DONT,		/* Request to stop or confirmation expecting */
	IAC		/* Escape character */

}	NVT_CODE;

/*
 ******	Códigos das opções **************************************
 */
typedef enum
{
	NVT_ECHO		=  1,
	NVT_SUPRESS_GO_AHEAD	=  3,
	NVT_NAOL		=  8,
	NVT_NAOP		=  9,
	NVT_TERMINAL_TYPE	= 24,
	NVT_WINDOW_SIZE		= 31

}	NVT_OPTION;

/*
 ******	Estados do automata *************************************
 */
typedef enum
{
	STATE_NO = 0,
	STATE_YES,
	STATE_WANTNO,
	STATE_WANTYES,

	STATE_ACCEPT,		/* O parceiro enviou um WILL */
	STATE_RECUSE,		/* O parceiro enviou um WONT */
	STATE_INFO		/* O parceiro enviou a informação desejada */

}	STATE_ENUM;
