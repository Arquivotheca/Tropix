/*
 ****************************************************************
 *								*
 *			atof.c					*
 *								*
 *	Converte uma cadeia de caracteres representando		*
 *	um número de ponto flutuante no formato interno		*
 *								*
 *	Versão	2.0.0, de 11.08.86				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: strtod						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ****************************************************************
 *	Converte números de ponto flutuante			*
 ****************************************************************
 */
double
atof (const char *cp)
{
	return (strtod (cp, (const char **)0));

}	/* end atof */
