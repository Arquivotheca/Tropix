/*
 ****************************************************************
 *								*
 *			parm.h					*
 *								*
 *	Definições para a compilação e execução das		*
 *	capacidades de cadeias de caracteres com parâmetros	*
 *								*
 *	Versão	1.0.0, de 07.05.87				*
 *		3.0.0, de 22.03.95				*
 *								*
 *	Modulo: parm						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Linguagem Fonte *****************************************
 */
#define	S_SPECIAL	'%'

/*
 *	Instruções de saída formatada
 */
#define	SO_CHAR		'c'
#define	SO_DEC		'd'
#define	SO_OCTAL	'o'
#define	SO_HEX		'x'
#define	SO_CHEX		'X'

/*
 *	Instruções de "push"
 */
#define	SP_PARM		'p'
#define	SP_CHAR1	'\''
#define	SP_CHAR2	'\''
#define	SP_DEC1		'{'
#define	SP_DEC2		'}'

/*
 *	Instruções aritméticas
 */
#define	SA_ADD		'+'
#define	SA_MUL		'*'
#define	SA_SUB		'-'
#define	SA_DIV		'/'
#define	SA_MOD		'm'

/*
 *	Outras instruções
 */
#define	S_INC		'i'

/*
 ******	Linguagem objeto ****************************************
 */
#define	T_END		0
#define	T_NULL		1
#define	T_NORMAL	2
#define	T_SPECIAL	3

/*
 *	Instruções de saída formatada
 */
#define	TO_CHAR		4
#define	TO_DEC		5
#define	TO_OCTAL	6
#define	TO_HEX		7
#define	TO_CHEX		8

/*
 *	Instruções de "push"
 */
#define	TP_PARM		9
#define	TP_CHAR		10
#define	TP_DEC		11

/*
 *	Instruções aritméticas
 */
#define	TA_ADD		12
#define	TA_MUL		13
#define	TA_SUB		14
#define	TA_DIV		15
#define	TA_MOD		16

/*
 *	Outras instruções
 */
#define	T_INC		17

/*
 ******	Outras definições ***************************************
 */
#define	PARMSZ	255

extern char	*_parmerror;	/* Mensagem de Erro */
extern char	*_parmptr;	/* Local do Erro */
extern char	*_parmrtn;	/* Área[PARMSZ] para retorno das Funções */
