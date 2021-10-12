/*
 ****************************************************************
 *								*
 *			global.h				*
 *								*
 *	Definições globais					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 23.03.95				*
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

#define	H_GLOBAL	/* Para declarar as funções */

/*
 ******	Tipos fundamentais **************************************
 */
typedef	enum	{ false, true } bool;

#define	EVER	;;
#define	elif	else if

/*
 ******	Limites máximos de caracteres para: *********************
 */
#define  LSIZE	512		/* Linha do texto */
#define  NSIZE	 64		/* Nome de arquivo */
#define  RSIZE	128		/* Cadeia para Substituição */

/*
 ******	Manipulação de Indicadores ******************************
 */
#define	check(f,m)	(((f)&(m)) == (m))	/* Verificação */
#define	set(f,m)	((f) |=  (m))		/* Ligando */
#define	reset(f,m)	((f) &= ~(m))		/* Desligando */
