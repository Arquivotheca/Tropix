/*
 ****************************************************************
 *								*
 *			sinh.c					*
 *								*
 *	Rotina para o cálculo das funções hiperbólicas		*
 *								*
 *	Versão	3.0.0, de 09.02.96				*
 *		3.0.0, de 11.03.96				*
 *								*
 *	Rotinas: 						*
 *		sinh,	cosh,	tanh				*
 *								*
 *	Módulo: sinh						*
 *		libm486/math					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <math.h>

/*
 ****************************************************************
 *								*
 *	Rotina que calcula o seno hiperbólico			*
 *								*
 *	sinh = ((e ** x) - (e ** -x)) / 2 =			*
 *	sinh = ((e ** x) - (1 / e ** x)) / 2 =			*
 *								*
 ****************************************************************
 */
double
sinh (double arg)
{
	double	valexp;

	valexp = exp (arg);

	return ((valexp - (1.0 / valexp)) / 2.0);

}	/* end sinh */

/*
 ****************************************************************
 *								*
 *	Rotina que calcula o cosseno hiperbólico		*
 *								*
 *	cosh = ((e ** x) + (e ** -x)) / 2 =			*
 *	cosh = ((e ** x) + (1 / e ** x)) / 2 =			*
 *								*
 ****************************************************************
 */
double
cosh (double arg)
{
	double	valexp;

	valexp = exp (arg);

	return ((valexp + (1.0 / valexp)) / 2.0);

}	/* end cosh */

/*
 ****************************************************************
 *								*
 *	Rotina que calcula a tangente hiperbólica		*
 *								*
 *	tanh = sinh/cosh = 1 - (2 / (e ** 2 * x) + 1)		*
 *								*
 ****************************************************************
 */
double
tanh (double arg)
{
	return (1.0 - (2.0 / (exp (2.0 * arg) + 1.0)));

}	/* end tanh */
