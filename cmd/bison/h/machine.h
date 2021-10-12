/*
 ****************************************************************
 *								*
 *			machine.h				*
 *								*
 *	Define machine-dependencies for bison			*
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


/*
 ****************************************************************
 *	Define machine-dependencies for bison			*
 ****************************************************************
 */
#define	MAXSHORT	32767
#define	MINSHORT	-32768

#define	BITS_PER_WORD	32
#define MAXTABLE	32767

#define	WORDSIZE(n)	(((n) + BITS_PER_WORD - 1) / BITS_PER_WORD)
#define	SETBIT(x, i)	((x)[(i)/BITS_PER_WORD] |= (1<<((i) % BITS_PER_WORD)))
#define RESETBIT(x, i)	((x)[(i)/BITS_PER_WORD] &= ~(1<<((i) % BITS_PER_WORD)))
#define BITISSET(x, i)	(((x)[(i)/BITS_PER_WORD] & (1<<((i) % BITS_PER_WORD))) != 0)
