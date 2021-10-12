/*
 ****************************************************************
 *								*
 *			stdio.c					*
 *								*
 *	Diversas Rotinas de E/S para a Console			*
 *								*
 *	Versão	3.0.0, de 28.06.94				*
 *		3.0.9, de 17.02.98				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1998 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <stdarg.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ****** Protótipos de funções ***********************************
 */
void	strprintf (char *, int, const char *, int *);
void	printn (char **, int *, unsigned int, unsigned int, int, int, int);

/*
 ****************************************************************
 *	Impressão na console ("printf") simplificada		*
 ****************************************************************
 */
void
printf (const char *fmt, ...)
{
	const char	*cp;
	int		c;
	char		area[4096];

	strprintf (area, sizeof (area), fmt, va_first (fmt));

	for (cp = area; (c = *cp) != '\0'; cp++)
		putchar (c);

}	/* end printf */

/*
 ****************************************************************
 *	Edição de textos 					*
 ****************************************************************
 */
void
snprintf (char *area, int size, const char *fmt, ...)
{
	strprintf (area, size, fmt, va_first (fmt));

}	/* end snprintf */

/*
 ****************************************************************
 *	Edição de textos 					*
 ****************************************************************
 */
void
strprintf (char *str, int size, const char *fmt, int *args)
{
	int		c, w, n, sinal, fill;
	const char	*cp;
	ulong		arg;

	/*
	 *	Inicialização; reserva um byte para o '\0'
	 */
	if (size > 0)
		size--;

	/*
	 *	Malha principal
	 */
	for (EVER)
	{
		/*
		 *	Texto no formato
		 */
		while ((c = *fmt++) != '%')
		{
			if (c == '\0')
				{ *str++ = c; return; }

			if (size > 0)
				{ *str++ = c; size--; }
		}

		if (*fmt == '%')
		{
			fmt++;

			if (size > 0)
				{ *str++ = '%'; size--; }

			continue;
		}

		/*
		 *	Retira a largura do campo e o operando
		 */
		for (fill = ' ', w = 0; (c = *fmt++) >= '0' && c <= '9'; /* vazio */)
		{
			if (w == 0 && c == '0')
				fill = '0';
			else
				w = w * 10 + c - '0';
		}

		arg = *args++; sinal = 0;

		/*
		 *	Analisa a conversão
		 */
		switch (c)
		{
			/*
			 *	Decimal
			 */
		    case 'd':
		    case 'D':
			if ((int)arg < 0)
				{ arg = -arg; sinal++; }
		    case 'u':
		    case 'U':
			printn (&str, &size, arg, 10, w, sinal, fill);
			break;

			/*
			 *	Hexadecimal
			 */
		    case 'x':
			arg &= 0xFFFF;
		    case 'X':
			printn (&str, &size, arg, 16, w, 0, '0');
			break;

			/*
			 *	Endereço (ponteiro)
			 */
		    case 'p':
		    case 'P':
			printn (&str, &size, arg >> 16,	16, 4, 0, '0');

			if (size > 0)
				{ *str++ = ' '; size--; }

			printn (&str, &size, arg & 0xFFFF, 16, 4, 0, '0');
			break;

			/*
			 *	Octal
			 */
		    case 'o':
			arg &= 0xFFFF;
		    case 'O':
			printn (&str, &size, arg, 8, w, 0, '0');
			break;

			/*
			 *	Cadeia de caracteres
			 */
		    case 's':
			for (cp = (const char *)arg; (c = *cp); cp++, w--)
			{
				if (size > 0)
					{ *str++ = c; size--; }
			}

			while (w-- > 0)
			{
				if (size > 0)
					{ *str++ = ' '; size--; }
			}

			break;

			/*
			 *	Um caractere
			 */
		    case  'c':
			if (size > 0)
				{ *str++ = arg; size--; }
			break;

			/*
			 *	Seqüência de bits
			 */
		    case 'b':
		    case 'B':
			cp = (char *)*args++;

			printn (&str, &size, arg, 16, w, 0, '0');

			if (!arg)
				break;

			for (c = 0; n = *cp++; /* vazio */)
			{
				if (arg & (1 << (n - 1)))
				{
					if (c)
					{
						if (size > 0)
							{ *str++ = ','; size--; }
					}
					else
					{
						if (size > 0)
							{ *str++ = ' '; size--; }

						if (size > 0)
							{ *str++ = '<'; size--; }
					}

					for (/* vazio */; (n = *cp) > ' '; cp++)
					{
						if (size > 0)
							{ *str++ = n; size--; }
					}

					c = 1;
				}
				else
				{
					for (/* vazio */; *cp > ' '; cp++)
						/* vazio */;
				}
			}

			if (c)
			{
				if (size > 0)
					{ *str++ = '>'; size--; }
			}

			break;

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end strprintf */

/*
 ****************************************************************
 *	Imprime um Inteiro 					*
 ****************************************************************
 */
void
printn (char **str_ptr, int *size_ptr, unsigned n, unsigned base, int w, int sinal, int fill)
{
	char		*str, *cp, *end_area;
	int		size;
	char		area[32];
	int		i;
	char		c;

	/*
	 *	Inicializa os valores
	 */
	str = *str_ptr; size = *size_ptr;

	end_area = area + sizeof (area);

	/*
	 *	Converte o valor para a base dada
	 */
	for (cp = end_area; cp > area; /* abaixo */)
	{
		*--cp = "0123456789ABCDEF"[n%base];

		if ((n /= base) == 0)
			break;
	}

	/*
	 *	Coloca os brancos/zeros no início
	 */
	if (w > 0)
	{
#if (0)	/*******************************************************/
		if (base == 10)
			c = ' ';
		else
			c = '0';
#endif	/*******************************************************/
		c = fill;

		i = w - (end_area - cp);

		if (sinal)
			i--;

		for (/* acima */; i > 0; i--)
		{
			if (size > 0)
				{ *str++ = c; size--; }
		}
	}

	/*
	 *	Põe o sinal
	 */
	if (sinal)
	{
		if (size > 0)
			{ *str++ = '-'; size--; }
	}

	/*
	 *	Coloca os dígitos
	 */
	while (cp < end_area)
	{
		if (size > 0)
			{ *str++ = *cp++; size--; }
	}

	/*
	 *	Atualiza os novos valores
	 */
	*str_ptr = str; *size_ptr = size;

}	/* end printn */
