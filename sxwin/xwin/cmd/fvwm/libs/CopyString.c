/*
 ****************************************************************
 *								*
 *			CopyString.c				*
 *								*
 *	Copia uma cadeia, removendo brancos			*
 *								*
 *	Versão	1.0.0, de 27.08.97				*
 *		4.4.0, de 20.03.03				*
 *								*
 *	Módulo: cmd/fvwm/libs					*
 *		X Windows - Clientes				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 2003 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <string.h>
#include "fvwmlib.h"

void
CopyString (char **dest, char *source)
{
	int	len;
	char	c, *start;

	for
	(	/* vazio */;
		((c = *source) == ' ' || c == '\t') &&
		c != '\n' && c != '\0';
		source++
	)
		/* procurando primeiro caractere válido */;

	start = source;		/* guarda o início de verdade */

	for
	(	len = 0;
		(c = *source) != '\n' && c != '\0';
		len++, source++
	)
		/* andando até o fim da cadeia */;
  
	for
	(	source--;
		len > 0 && (c = *source) != '\0' &&
		(c == ' ' || c == '\t');
		source--, len--
	)
		/* voltando para trás, pulando os brancos do final */;

	*dest = safemalloc (len + 1);
	strncpy (*dest, start, len);
	(*dest)[len] = 0;	  

}	/* end CopyString */
