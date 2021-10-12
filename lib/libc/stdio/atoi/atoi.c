/*
 ****************************************************************
 *								*
 *			atoi.c					*
 *								*
 *	Converte cadeias decimais para números inteiros 	*
 *								*
 *	Versão	1.0.0, de 29.09.86				*
 *		3.0.0, de 08.07.95				*
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
 ****************************************************************
 *	Ponto de Entrada da Rotina				*
 ****************************************************************
 */
int
atoi (register const char *str)
{
	register char	c;
	register int	n;
	register int	sign;

	/*
	 *	Ignora os brancos iniciais
	 */
	while ((c = *str) == ' ' || c == '\t')
		str++;

	/*
	 *	Verifica se foi dado sinal
	 */
	if (c == '+' || c == '-')
		{ sign = (c == '-'); str++; }
	else
		sign = 0;

	/*
	 *	Ignora zeros iniciais
	 */
	while ((c = *str) == '0')
		str++;

	/*
	 *	Realiza a conversão
	 */
	n = 0;

	while ((c = *str++) >= '0' && c <= '9')
	{
		n = (n << 3) + (n << 1) + (c & 0xF);
	}

	return (sign ? -n : n);

}	/* end atoi */
