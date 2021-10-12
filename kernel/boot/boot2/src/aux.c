/*
 ****************************************************************
 *								*
 *			str.c					*
 *								*
 *	Cópia e comparação de cadeias 				*
 *								*
 *	Versão	3.0.0, de 26.07.94				*
 *		3.0.0, de 26.07.94				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1997 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/common.h"

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Compara cadeias de caracteres				*
 ****************************************************************
 */
int
streq (char *pa, char *pb)
{
	for (EVER)
	{
		if (*pa++ != *pb)
			return (0);

		if (*pb++ == '\0')
			return (1);
	}

}	/* end streq */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Compara IDs (14 caracteres)				*
 ****************************************************************
 */
int
ideq (char *pa, char *pb)
{
	int		i;

	for (i = 14; i > 0; i--)
	{
		if (*pa++ != *pb)
			return (0);

		if (*pb++ == '\0')
			return (1);
	}

	return (1);

}	/* end ideq */

/*
 ****************************************************************
 *	Converte Decimal para Binario				*
 ****************************************************************
 */
long
atol (char *cp)
{
	long		n = 0, c;
	int		menos = 0;

	if (*cp == '-')
		{ menos++; cp++; }

	while ((c = *cp++) >= '0' && c <= '9')
		n = 10 * n + c - '0';

	if (menos)
		return (-n);
	else
		return (n);

}	/* end atol */

/*
 ****************************************************************
 *	Converte Hexadecimal para Binario			*
 ****************************************************************
 */
long
xtol (char *cp)
{
	long		n;
	int		c;

	n = 0;

	for (EVER)
	{
		if   ((c = *cp++) >= '0' && c <= '9')
			n = (n << 4) | (c - '0');
		elif (c >= 'A' && c <= 'F')
			n = (n << 4) | (c - 'A' + 10);
		elif (c >= 'a' && c <= 'f')
			n = (n << 4) | (c - 'a' + 10);
		else
			break;
	}

	return (n);

}	/* end xtol */
