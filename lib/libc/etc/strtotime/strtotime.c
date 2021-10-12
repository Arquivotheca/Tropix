/*
 ****************************************************************
 *								*
 *			strtotime.c				*
 *								*
 *	Conversão de uma cadeia em uma data ("time_t")		*
 *								*
 *	Versão	2.3.0, de 09.03.90				*
 *		4.8.0, de 07.09.05				*
 *								*
 *	Módulo: strtotime					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <time.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	elif	else if

/*
 ****************************************************************
 *	Conversão de uma cadeia em uma data ("time_t")		*
 ****************************************************************
 */
time_t
strtotime (const char *str, TM *tp)
{
	int		i, j;
	char		*cp;
	const char	*ccp;
	char		buf[4];

	/*
	 *	Analisa os segundos (no final)
	 */
	tp->tm_sec = 0;

	for (i = 0, ccp = str; *ccp != '\0'; i++, ccp++)
	{
		if (*ccp >= '0' && *ccp <= '9')
			continue;

		if (*ccp == ':' || *ccp == '.')
		{
			tp->tm_sec = atoi (++ccp);

			if (tp->tm_sec > 59)
				return (-1);
			ccp--;
			break;
		}

		return (-1);
	}

	if (i & 1)
		return (-1);

	/*
	 *	Vem do final para o início analisando o resto
	 */
	buf[2] = '\0';

	for (j = 0; ccp > str; j++)
	{
		cp = &buf[2];

		*--cp = *--ccp;
		*--cp = *--ccp;

		switch (j)
		{
		    case 0:
			if ((tp->tm_min = atoi (cp)) > 59)
				return (-1);
			break;

		    case 1:
			if ((tp->tm_hour = atoi (cp)) > 23)
				return (-1);
			break;

		    case 2:
			if ((tp->tm_mday = atoi (cp)) > 31)
				return (-1);
			break;

		    case 3:
			if ((tp->tm_mon = atoi (cp) - 1) > 11)
				return (-1);
			break;

		    case 4:
			if   ((tp->tm_year = atoi (cp)) <= 37)
				tp->tm_year += 100;
			elif (tp->tm_year < 70)
				return (-1);
			break;

		    default:
			return (-1);

		}	/* end switch (j) */

	}	/* end for */

	/*
	 *	Agora, completa o resto de TM
	 */
	return (mktime (tp));

}	/* end strtotime */
