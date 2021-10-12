/*
 ****************************************************************
 *								*
 *			<setjmp.h>				*
 *								*
 *	Desvios não locais					*
 *								*
 *	Versão	1.0.0, de 20.11.86				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: setjmp.h					*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1995 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definição de tipos					*
 ****************************************************************
 */
typedef int	jmp_buf[13];

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern int	setjmp (jmp_buf);		/* Prepara o desvio */
extern void	longjmp (jmp_buf, int);		/* Desvia */
