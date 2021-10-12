/*
 ****************************************************************
 *								*
 *			strtoul.c				*
 *								*
 *   Extrai valores numericos inteiros sem sinal de uma cadeia	*
 *								*
 *	Versão	1.0.0, de 16.09.86				*
 *		3.0.0, de 11.07.95				*
 *								*
 *	Módulo: libc/etc					*
 *		Biblioteca Basica				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ******	Definições globais **************************************
 */
#define	NIL	(-1)
#define	EVER	;;

/*
 ****************************************************************
 *	Tabela para a conversão de caracteres			*
 ****************************************************************
 */
extern const char	_strtol_base_tb[];

/*
 ****************************************************************
 *   Extrai valores numericos inteiros sem sinal de uma cadeia	*
 ****************************************************************
 */
unsigned long
strtoul (const char *str, const char **ptr, int base)
{
	register const char	*cp = str;
	register int		c, sign = 0;
	register unsigned long	n = 0, val;

	/*
	 *	Ignora Brancos ou TABs iniciais
	 */
	while (*cp == ' ' || *cp == '\t')
		cp++;

	/*
	 *	Analisa o sinal
	 */
	if ((c = *cp) == '+' || c == '-')
		{ sign = (c == '-'); cp++; }

	/*
	 *	Se não foi especificada uma base, descobre de acordo
	 *	com o inicio da cadeia
	 */
	if (base < 2 || base > 36)
	{
		if (*cp == '0')
		{
			if ((c = *++cp) == 'x' || c == 'X')
				{ base = 16; cp++; }
			else
				base = 8;
		}
		else
		{
			base = 10;
		}
	}
	else
	{
		/*
		 *	Ignora os zeros iniciais
		 */
		while (*cp == '0')
			cp++;

		if (base == 16 && ((c = *cp) == 'x' || c == 'X'))
			cp++;
	}

	/*
	 *	Realiza a conversão
	 */
	for (EVER)
	{
		if ((val = _strtol_base_tb[*cp]) == NIL || val >= base)
			break;

		n = n * base + val; cp++;
	}

	if (ptr != (const char **)NULL)
		*ptr = cp;

	return (sign ? -n : n);

}	/* end strtoul */
