/*
 ****************************************************************
 *								*
 *			stdio.c					*
 *								*
 *	Diversas Rotinas auxiliares de E/S			*
 *								*
 *	Versão	3.0.0, de 28.06.94				*
 *		4.8.0, de 13.09.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdarg.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/disktb.h"
#include "../h/devmajor.h"
#include "../h/tty.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Responde SIM ou NÃO					*
 ****************************************************************
 */
int
askyesno (int std)
{
	char		*cp;
	char		area[32];
	
	for (EVER)
	{
		cp = gets (area); putchar ('\n');

		switch (*cp)
		{
		    case 's':
		    case 'S':
		    case 'y':
		    case 'Y':
			return (1);

		    case 'n':
		    case 'N':
			return (0);

		    case '\0':
			return (std);

		}	/* end switch */

		printf ("Sim ou não? (%c): ", std > 0 ? 's' : 'n');
	}

}	/* end askyesno */

/*
 ****************************************************************
 *	Função de conversão					*
 ****************************************************************
 */
long
strtol (const char *str, const char **ptr, int base)
{
	const char	*cp = str;
	char		c, sign = 0;
	long		n = 0, val;

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
	if (base < 2 || base > 16)
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
		if ((c = *cp) >= '0' && c <= '9')
			val = c - '0';
		elif (c >= 'a' && c <= 'f')
			val = c - 'a' + 10;
		elif (c >= 'A' && c <= 'F')
			val = c - 'A' + 10;
		else
			break;

		if (val >= base)
			break;

		n = n * base + val; cp++;
	}

	if (ptr != (const char **)NULL)
		*ptr = cp;

	return (sign ? -n : n);

}	/* end strtol */

/*
 ****************************************************************
 *	Função de PANIC						*
 ****************************************************************
 */
void
panic (const char *fmt, ...)
{
	const TTY	*tp;

	printf  ("\n***** PANIC: ");
	vprintf (fmt, va_first (fmt));
	printf  ("\n\n");

	print_calls ();

#if (0)	/*******************************************************/
	update (NODEV, 1 /* blocks_also */);
#endif	/*******************************************************/

	if ((tp = kernel_pty_tp) == NOTTY || (tp->t_state & ISOPEN) == 0)
		spyexcep ();

if (u.u_pid != scb.y_initpid)		/* PROVISÓRIO */
	kexit (-1, NULL, NULL);

	for (EVER)
		idle ();

   /***	va_end (ap); ***/

}	/* end panic */

/*
 ****************************************************************
 *	Obtem o MAJOR+MINOR do Dispositivo dado o nome		*
 ****************************************************************
 */
dev_t
strtodev (const char *cp)
{
	const DISKTB 	*dp;

	for (dp = disktb; dp->p_name[0] != '\0'; dp++)
	{
		if (streq (dp->p_name, cp))
			return (dp->p_dev);
	}

	/*
	 *	Se não estiver na tabela, ...
	 */
	return (NODEV);

}	/* end strtodev */

/*
 ****************************************************************
 *	Comparador de padrões					*
 ****************************************************************
 */
int
patmatch (const char *cp /* Cadeia */, const char *pp /* Padrão */)
{
	/*
	 *	Malha Principal de Comparação
	 *
	 *	Cuidado: Esta função é recursiva!
	 */
	for (/* sem inicialização */; /* sem teste */; cp++, pp++)
	{
		switch (*pp)
		{
			/*
			 *	O Padrão contem um caracter Normal,
			 *	ou outras palavras, 
			 *	diferente de '?', '*' e '\0'
			 */
		    default:
			if (*pp == *cp)
				continue;
			else
				return (0);

			/*
			 *	O Padrão Acabou
			 */
		    case '\0':
			if (*cp == '\0')
				return (1);
			else
				return (0);

			/*
			 *	Simula como se o "coringa" '?' fosse
			 *	igual ao caracter da cadeia
			 */
		    case '?':
			if (*cp == '\0')
				return (0);
			continue;

			/*
			 *	Procura Sufixos em comum
			 */
		    case '*':
			if (*++pp == '\0')
				return (1);

			while (*cp != '\0')
			{
				if (patmatch (cp++, pp))
					return (1);
			}

			return (0);

		}	/* end switch */

		return (0);

	}	/* end malha principal de comparação */

}	/* end patmatch */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
void
editpercent (char buf[], int size, int quoc)
{
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";

	snprintf
	(	buf, size,
		"%s%d.%02d%c",
		sinal, quoc / 100, quoc % 100, '%'
	);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (int numer, int denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */
#endif	/*******************************************************/
