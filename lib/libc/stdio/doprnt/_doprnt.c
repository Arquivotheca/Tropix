/*
 ****************************************************************
 *								*
 *			doprnt.c				*
 *								*
 *	Formata a saída de dados 				*
 *								*
 *	Versão	1.0.0, de 26.03.87				*
 *		4.4.0, de 31.10.02				*
 *								*
 *	Módulo: doprnt 						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>

/*
 ****************************************************************
 *	Definições externas e globais				*
 ****************************************************************
 */
#define elif	else if
#define EVER	;;

#ifdef	PC

#define N_SIGNIF  15		/* Num. max. dig. signif. num double */

#else	ICA

#define	double	long double
#define N_SIGNIF  18		/* Num. max. dig. signif. num extd */

#endif	PC

/*
 ******	Protótipos de funções ***********************************
 */
static char	*int_conversion (unsigned long, char *, int, int, int);
static char	*quad_conversion (unsigned long long q, char *str, int);
static void	strout (const char *, int, int, FILE *, int);

extern int	_fcvt (double, int *, char *);
extern int	divmoduq (unsigned long long *, int);

/*
 ****************************************************************
 *	Formata a saída de dados 				*
 ****************************************************************
 */
int
_doprnt (const char *fmt, int *args, FILE *fp)
{
	char			*str, *cp, *bp;
	int			fc, i;
	long			n;
	unsigned long		u;
	long long		qn;
	unsigned long long	qu;
	double		 	d;
	int			count = 0, field, precision, free_field;
	int			hexa_base, float_char, exp, float_minus;
	int			n_signif;
	char			minus_flag, plus_flag, space_flag, number_flag;
	char			pad_char = ' ', size;
	char			float_area[32], area[640];

	/*
	 *	Percorre a cadeia do formato
	 */
	for (str = area; (fc = *fmt++) != '\0'; /* abaixo*/)
	{
		/*
		 *	Se não for o começo de uma conversão, ...
		 */
		if (fc != '%')
			{ *str++ = fc; continue; }

		/*
		 *	Se for "%%", ...
		 */
		if ((fc = *fmt++) == '%')
			{ *str++ = fc; continue; }

		/*
		 *	Estamos iniciando uma conversão: emite a área
		 */
		if (str > area)
		{
			strout (area, str - area, 0, fp, ' ');
			count += str - area; str = area;
		}

		/*
		 *	Inicializa os diversos parâmetros da conversão
		 */
		minus_flag = 0;	 plus_flag = 0; space_flag = 0;
		number_flag = 0; pad_char = ' '; hexa_base = 'a' - 10;
		float_char = 'E'; field = 0; precision = -1; size = 0;

		/*
		 *	Analisa os possíveis indicadores
		 */
		for (EVER)
		{
			switch (fc)
			{
			    case '-':
				minus_flag++;
				fc = *fmt++;
				continue;

			    case '+':
				plus_flag++;
				fc = *fmt++;
				continue;

			    case ' ':
				space_flag++;
				fc = *fmt++;
				continue;

			    case '#':
				number_flag++;
				fc = *fmt++;
				continue;

			    case '0':
				pad_char = '0';
				fc = *fmt++;
				continue;

			    default:
				break;

			}	/* end switch */

			break;

		}	/* end for (EVER) */

		/*
		 *	Pequenos ajustes nos indicadores
		 */
		if (minus_flag)
			pad_char = ' ';

		if (plus_flag)
			space_flag = 0;

		/*
		 *	Retira o tamanho do campo
		 */
		if (fc == '*')
		{
			if ((field = *args++) < 0)
				{ minus_flag++; field = -field; }

			fc = *fmt++;
		}
		else for (/* vazio */; fc >= '0' && fc <= '9'; fc = *fmt++)
		{
			field = 10 * field + (fc - '0');
		}

		/*
		 *	Retira a precisão
		 */
		if (fc == '.')
		{
			fc = *fmt++; precision = 0;

			if (fc == '*')
			{
				/* Valor negativo == não dado */

				precision = *args++; fc = *fmt++;
			}
			else for (/* vazio */; fc >= '0' && fc <= '9'; fc = *fmt++)
			{
				precision = 10 * precision + (fc - '0');
			}
		}

		/*
		 *	Retira o tamanho ("short", "long", "long long")
		 */
		switch (fc)
		{
		    case 'h':
		    case 'L':
		    case 'q':
			size = fc; fc = *fmt++;
			break;

		    case 'l':
			size = fc; fc = *fmt++;

			if (fc == 'l')
				{ size = 'q'; fc = *fmt++; }

			break;
		}

		/*
		 *	Finalmente, examina qual é a conversão
		 */
		switch (fc)
		{
			/*
			 ******	Conversão decimal com sinal ***************
			 */
		    case 'd':
		    case 'i':
			if (precision < 0)
				precision = 1;
			else
				pad_char = ' ';

			if (size == 'q')
				goto case_qd;

			n = *args++;

			if (size == 'h')
				n = (short)n;

			if   (n < 0)
				{ *str++ = '-'; n = -n; }
			elif (plus_flag)
				{ *str++ = '+'; }
			elif (space_flag)
				{ *str++ = ' '; }

			str = int_conversion (n, str, 10, precision, 0);
			break;

			/*
			 ******	Conversão decimal quad com sinal **********
			 */
		    case_qd:
			qn = *(unsigned long long *)args;
			args += sizeof (unsigned long long) / sizeof (int);

			if   (qn < 0)
				{ *str++ = '-'; qn = -qn; }
			elif (plus_flag)
				{ *str++ = '+'; }
			elif (space_flag)
				{ *str++ = ' '; }

			str = quad_conversion (qn, str, precision);
			break;

			/*
			 ******	Conversão decimal sem sinal ***************
			 */
		    case 'u':
			if (precision < 0)
				precision = 1;
			else
				pad_char = ' ';

			if (size == 'q')
				goto case_qu;

			u = *args++;

			if (size == 'h')
				u = (unsigned short)u;

			str = int_conversion (u, str, 10, precision, 0);
			break;

			/*
			 ******	Conversão decimal quad sem sinal **********
			 */
		    case_qu:
			qu = *(unsigned long long *)args;
			args += sizeof (unsigned long long) / sizeof (int);

			str = quad_conversion (qu, str, precision);

			break;

			/*
			 ******	Conversão octal ***************************
			 */
		    case 'o':
			u = *args++;

			if (size == 'h')
				u = (unsigned short)u;

			if (precision < 0)
				precision = 1;
			else
				pad_char = ' ';

			if (number_flag && u != 0)
				*str++ = '0';

			str = int_conversion (u, str, 8, precision, 0);
			break;

			/*
			 ******	Conversão hexadecimal *********************
			 */
		    case 'X':
			hexa_base = 'A' - 10;
		    case 'x':
			u = *args++;

			if (size == 'h')
				u = (unsigned short)u;

			if (precision < 0)
				precision = 1;
			else
				pad_char = ' ';

			if (number_flag)
				{ *str++ = '0'; *str++ = 'x'; }

			str = int_conversion (u, str, 16, precision, hexa_base);
			break;

			/*
			 ******	Ponteiro **********************************
			 */
		    case 'P':
			hexa_base = 'A' - 10;
		    case 'p':
			u = *args++;

			pad_char = ' ';

			if (number_flag)
				{ *str++ = '0'; *str++ = 'x'; }

			str = int_conversion (u >> 16, str, 16, 4, hexa_base);

			if (!number_flag)
				*str++ = ' ';

			str = int_conversion ((unsigned short)u, str, 16, 4, hexa_base);
			break;

			/*
			 ******	Lista de indicadores **********************
			 */
		    case 'B':
			hexa_base = 'A' - 10;
		    case 'b':
			u = *args++; cp = (char *)*args++;

			if (size == 'h')
				u = (unsigned short)u;

			if (precision < 0)
				precision = 1;
			else
				pad_char = ' ';

			if (number_flag)
				{ *str++ = '0'; *str++ = 'x'; }

			str = int_conversion (u, str, 16, precision, hexa_base);

			if (u == 0)
				break;

			for (i = 0; n = *cp++; /* abaixo */)
			{
				if (u & (1 << (n - 1)))
				{
					if (i)
						*str++ = '|';
					else
						{ *str++ = ' '; *str++ = '<'; }

					for (/* vazio */; (n = *cp) >= ' '; cp++)
						*str++ = n;

					i = 1;
				}
				else
				{
					for (/* vazio */; *cp >= ' '; cp++)
						/* vazio */;
				}
			}

			if (i)
				*str++ = '>';

			break;

			/*
			 ******	Conversão caractere ***********************
			 */
		    case 'c':
			*str++ = *args++; pad_char = ' ';
			break;

			/*
			 ******	Conversão "%s" - não utiliza o epílogo ****
			 */
		   case 's':
			str = (char *)*args++;

			if (str == NOSTR)
				str = "(NULO)";

			n = strlen (str);

			if (precision < 0 || precision > n)
				precision = n;

			free_field = field - precision;

			if (free_field < 0)
				free_field = 0;

			count += free_field + precision;

			if (!minus_flag)
				free_field = -free_field;

			strout (str, precision, free_field, fp, ' ');

			str = area;

			continue;

			/*
			 ******	Pseudo-conversão "n" **********************
			 */
		    case 'n':
			if (size == 'h')
				**(short int **)args = count;
			else
				**(int **)args = count;

			args++;
			continue;

			/*
			 ******	Conversão flutuante "%E" ******************
			 */
		    case 'e':
			float_char = 'e';
		    case 'E':
			d = *(double *)args;
			args += sizeof (double) / sizeof (int);

			if (precision < 0)
				precision = 6;

			cp = float_area;

			exp = _fcvt (d, &float_minus, cp);
		    case_E:
			if   (float_minus)
				*str++ = '-';
			elif (plus_flag)
				*str++ = '+';
			elif (space_flag)
				*str++ = ' ';

			*str++ = *cp++;		/* Copia o primeiro digito */

			if (precision >= 1)
			{
				*str++ = '.'; n_signif = 1;

				while (precision > 0 && n_signif < N_SIGNIF)
				{
					*str++ = *cp++;
					precision--;
					n_signif++;
				}

				while (precision-- > 0)
					*str++ = '0';
			}

			/*
			 *	Acerta o formato "G"
			 */
			if (fc == 'G') for (bp = NOSTR, cp = str - 1; cp >= area; cp--)
			{
				if (*cp == '0')
					continue;

				if (*cp == '.')
				{
					if (bp != NOSTR)
						str = bp + 1;
					else
						str = cp;
					break;
				}

				if (bp == NOSTR)
					bp = cp;
			}

			/*
			 *	Coloca o expoente
			 */
			*str++ = float_char;

			if (exp < 0)
				{ exp = -exp; *str++ = '-'; }
			else
				{ *str++ = '+'; }
#ifdef	ICA
			if (exp > 999)
				{ *str++ = (exp / 1000) + '0'; exp %= 1000; }
#endif	ICA
			*str++ = (exp / 100) + '0'; exp %= 100;
			*str++ = (exp / 10)  + '0';
			*str++ = (exp % 10)  + '0';

			break;

			/*
			 ******	Conversão flutuante "%F" ******************
			 */
		    case 'f':
			d = *(double *)args;
			args += sizeof (double) / sizeof (int);

			if (precision < 0)
				precision = 6;

			cp = float_area;

			exp = _fcvt (d, &float_minus, cp);
		    case_F:
			if   (float_minus)
				*str++ = '-';
			elif (plus_flag)
				*str++ = '+';
			elif (space_flag)
				*str++ = ' ';

			n_signif = 0;

			while (exp > 0 && n_signif < N_SIGNIF)
				{ *str++ = *cp++; exp--; n_signif++; }

			while (exp > 0)
				{ *str++ = '0'; exp--; }

			if (exp == 0)
			{
				if (n_signif < N_SIGNIF)
					{ *str++ = *cp++; n_signif++; }
				else
					*str++ = '0';
			}

			if (exp < 0)
				*str++ = '0';

			if (exp < 0 || precision != 0)
				*str++ = '.';

			while (++exp < 0)
			{
				if (--precision < 0)
					break;

				*str++ = '0';
			}

			while (precision > 0 && n_signif < N_SIGNIF)
				{ *str++ = *cp++; precision--; n_signif++; }

			while (precision-- > 0)
				*str++ = '0';

			/*
			 *	Acerta o formato "G"
			 */
			if (fc == 'G') for (bp = NOSTR, cp = str - 1; cp >= area; cp--)
			{
				if (*cp == '0')
					continue;

				if (*cp == '.')
				{
					if (bp != NOSTR)
						str = bp + 1;
					else
						str = cp;
					break;
				}

				if (bp == NOSTR)
					bp = cp;
			}

			break;

			/*
			 ******	Conversão flutuante "%G" ******************
			 */
		    case 'g':
			float_char = 'e'; fc = 'G';
		    case 'G':
			d = *(double *)args;
			args += sizeof (double) / sizeof (int);

			if   (precision < 0)
				precision = 6;
			elif (precision == 0)
				precision = 1;

			cp = float_area;

			exp = _fcvt (d, &float_minus, cp);

			if (exp < -4 || exp > precision)
				goto case_E;
			else
				goto case_F;

			/*
			 ******	Conversão não identificada ****************
			 */
		    default:
			args++;
			*str++ = '%'; *str++ = fc;
			continue;

		}	/* end switch (o tipo da conversão) */

		/*
		 *	Epílogo: emite a conversão realizada
		 */
		n = str - area; free_field = field - n;

		if (free_field < 0)
			free_field = 0;

		count += free_field + n;

		if (!minus_flag)
			free_field = -free_field;

		strout (area, n, free_field, fp, pad_char);

		str = area;

	}	/* end for (percorrendo o formato) */

	/*
	 *	Emite o possível campo residual
	 */
	if (str > area)
	{
		strout (area, str - area, 0, fp, ' ');
		count += str - area; /*** str = area; ***/
	}

	return (count);

}	/* end _doprnt */

/*
 ****************************************************************
 *	Converte números inteiros				*
 ****************************************************************
 */
static char *
int_conversion (unsigned long n, char *str, int base, int precision, int hexa_base)
{
	int		i;
	char		area[16];
	char		*cp = area;

	/*
	 *	Converte o número em "area"
	 *	Repare que os dígitos ficam na ordem inversa!
	 */
	do
	{
		i = n % base;

		if (i < 10)
			*cp++ = i + '0';
		else
			*cp++ = i + hexa_base;
	}
	while (n /= base);

	/*
	 *	Coloca "0"s no começo do número (se necessário)
	 */
	for (i = precision - (cp - area); i > 0; i--)
		*str++ = '0';

	/*
	 *	Copia o número convertido (invertendo a ordem)
	 */
	while (cp > area)
		*str++ = *--cp;

	return (str);

}	  /* end  int_conversion */

/*
 ****************************************************************
 *	Converte números inteiros de 64 bits			*
 ****************************************************************
 */
static char *
quad_conversion (unsigned long long q, char *str, int precision)
{
	int		i;
	char		area[32];
	char		*cp = area;

	/*
	 *	Converte o número em "area"
	 *	Repare que os dígitos ficam na ordem inversa!
	 */
	do
	{
		*cp++ = divmoduq (&q, 10) + '0';
	}
	while (q);

	/*
	 *	Coloca "0"s no começo do número (se necessário)
	 */
	for (i = precision - (cp - area); i > 0; i--)
		*str++ = '0';

	/*
	 *	Copia o número convertido (invertendo a ordem)
	 */
	while (cp > area)
		*str++ = *--cp;

	return (str);

}	  /* end  int_conversion */

/*
 ****************************************************************
 *	Imprime uma cadeia editada				*
 ****************************************************************
 */
static void
strout (const char *str, int str_cnt, int pad_cnt, FILE *fp, int pad_char)
{
	int		c;

	/*
	 *	Toma cuidado para o sinal não ficar depois dos "00000..."
	 */
	if (pad_char == '0' && (*str == '-' || *str == '+' || *str == ' ') && str_cnt > 0)
		{ c = *str++; putc (c, fp); str_cnt--; }

	/*
	 *	Coloca o prólogo
	 */
	while (pad_cnt < 0)
		{ putc (pad_char, fp); pad_cnt++; }

	/*
	 *	Coloca a cadeia propriamente dita
	 */
	while (str_cnt > 0)
		{ c = *str++; putc (c, fp); str_cnt--; }

	/*
	 *	Coloca o epílogo
	 */
	while (pad_cnt > 0)
		{ putc (' ', fp); pad_cnt--; }

}	/* end _strout */
