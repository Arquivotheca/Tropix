/*
 ****************************************************************
 *								*
 *			strftime.c				*
 *								*
 *	Formatação de tempos					*
 *								*
 *	Versão	2.3.0, de 07.03.90				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: strftime					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	EVER	;;

static char	*endstr;	/* Final da área permitida */

static char	present_locale[LC_COL] = "?";

/*
 ****** Estrutura unificada *************************************
 */
typedef char	MEMBER[16];

static MEMBER	*universal;

#define	NOMEMBER (MEMBER *)NULL

#define weekdayname	(universal)
#define fullweekdayname (universal + 7)
#define monthname	(universal + 7 + 7)
#define fullmonthname	(universal + 7 + 7 + 12)
#define universal_size	(7 + 7 + 12 + 12)

/*
 ******	Protótipos **********************************************
 */
char	*strappend (char *, const char *);
int	time_locale (void);

/*
 ****************************************************************
 *	Formatação de tempos					*
 ****************************************************************
 */
size_t
strftime (char *str, size_t maxsize, register const char *fmt, const TM *tm)
{
	register char	*cp;
	register int	f, i;
	char		sinal;
	char		buf[16];

	/*
	 *	Verifica se a localização está correta
	 */
	if (!streq (present_locale, _lc_vector[LC_TIME]))
	{
		if (time_locale () < 0)
			return (0);
	}

	/*
	 *	Executa a formatação
	 */
	cp = str; endstr = cp + maxsize;

	for (EVER) switch (f = *fmt++)
	{
	    case '%':
		switch (f = *fmt++)
		{
			/*
			 *	Dia da semana abreviado
			 */
		    case 'a':
			cp = strappend (cp, weekdayname[tm->tm_wday]);
			break;
	
			/*
			 *	Dia da semana completo
			 */
		    case 'A':
			cp = strappend (cp, fullweekdayname[tm->tm_wday]);
			break;
	
			/*
			 *	Nome do mês abreviado
			 */
		    case 'b':
			cp = strappend (cp, monthname[tm->tm_mon]);
			break;
	
			/*
			 *	Nome do mês completo
			 */
		    case 'B':
			cp = strappend (cp, fullmonthname[tm->tm_mon]);
			break;
	
			/*
			 *	Representação da data + tempo:
			 *	"22:16:45 QUI  8 mar 1990"
			 */
		    case 'c':
			sprintf
			(	buf, "%02d:%02d:%02d ",
				tm->tm_hour, tm->tm_min, tm->tm_sec
			);
			if ((cp = strappend (cp, buf)) == NOSTR)
				return (0);
			goto case_x;
	
			/*
			 *	Dia do mês (01- 31)
			 */
		    case 'd':
			sprintf (buf, "%02d", tm->tm_mday);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Hora do dia (00-23)
			 */
		    case 'H':
			sprintf (buf, "%02d", tm->tm_hour);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Hora do dia (01-12)
			 */
		    case 'I':
			i = tm->tm_hour;
			sprintf (buf, "%02d", i > 12 ? i-12 : (i == 0 ? 12 : i));
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Dia do ano (001- 366)
			 */
		    case 'j':
			sprintf (buf, "%03d", tm->tm_yday + 1);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Mes do ano (01-12)
			 */
		    case 'm':
			sprintf (buf, "%02d", tm->tm_mon + 1);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Minuto da hora (00-59)
			 */
		    case 'M':
			sprintf (buf, "%02d", tm->tm_min);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	AM / PM
			 */
		    case 'p':
			if (tm->tm_hour > 12)
				cp = strappend (cp, "PM");
			else
				cp = strappend (cp, "AM");
			break;
	
			/*
			 *	Segundo do minuto (00-60)
			 */
		    case 'S':
			sprintf (buf, "%02d", tm->tm_sec);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Semana do ano (domingo) (00-53)
			 */
		    case 'U':
			i = tm->tm_yday - tm->tm_wday + 7;
			sprintf (buf, "%02d", (i - i % 7) / 7);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Dia da semana (0-6)
			 */
		    case 'w':
			sprintf (buf, "%01d", tm->tm_wday);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Semana do ano (segunda) (00-53)
			 */
		    case 'W':
			i = tm->tm_yday - tm->tm_wday + 8;
			sprintf (buf, "%02d", (i - i % 7) / 7);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Representação de tempo: "QUI  8 mar 1990"
			 */
		    case 'x':
		    case_x:
			sprintf
			(	buf, "%3s %2d %3s %4d",
				weekdayname[tm->tm_wday],
				tm->tm_mday,
				monthname[tm->tm_mon],
				tm->tm_year + 1900
			);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Representação de tempo: "22:16:45"
			 */
		    case 'X':
			sprintf
			(	buf, "%02d:%02d:%02d",
				tm->tm_hour, tm->tm_min, tm->tm_sec
			);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Ano sem século (00-99)
			 */
		    case 'y':
			i = tm->tm_year;
			sprintf (buf, "%02d", i >= 100 ? i - 100 : i);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Ano com século (1970-2037)
			 */
		    case 'Y':
			sprintf (buf, "%04d", tm->tm_year + 1900);
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Timezone
			 */
		    case 'Z':
			if ((i = gettzmin ()) < 0)
				{ sinal = '-'; i = -i; }
			else
				{ sinal = '+'; }

			sprintf (buf, "%c%02d%02d", sinal, i / 60, i % 60); 
			cp = strappend (cp, buf);
			break;
	
			/*
			 *	Inválidos e '%%'
			 */
		    case '%':
		    default:
			if (cp >= endstr)
				return (0);
			*cp++ = f;
			break;
	
		}	/* end switch */

		if (cp == NOSTR)
			return (0);

		break;

		/*
		 *	Final do formato
		 */
	    case '\0':
		if (cp >= endstr)
			return (0);
		*cp = '\0';
		return (cp - str);

		/*
		 *	Outros caracteres
		 */
	    default:
		if (cp >= endstr)
			return (0);
		*cp++ = f;
		break;

	}	/* end for (EVER) switch */

}	/* end strftime */

/*
 ****************************************************************
 *	Adiciona um campo					*
 ****************************************************************
 */
static char *
strappend (register char *cp, register const char *info)
{
	register char	c;

	while (c = *info++)
	{
		if (cp >= endstr)
			return (NOSTR);

		*cp++ = c;
	}

	return (cp);

}	/* end strappend */

/*
 ****************************************************************
 *	Verifica a localização					*
 ****************************************************************
 */
static int
time_locale (void)
{
	register FILE	*fp;
	const char	*token;
	char		file_nm[32];
	char		area[128];
	int		i = 0;

	/*
	 *	Aloca a tabela, se necessário
	 */
	if (universal == NOMEMBER)
	{
		if ((universal = malloc (universal_size * sizeof (MEMBER))) == NOMEMBER)
			return (-1);
	}

	/*
	 *	Compõe o nome do arquivo
	 */
	strcpy (file_nm, "/etc/locale/time_");
	strcat (file_nm, _lc_vector[LC_TIME]);

	/*
	 *	Abre o arquivo
	 */
	if ((fp = fopen (file_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"strftime: NÃO consegui abrir \"%s\" (%s)\n",
			file_nm, strerror (errno)
		);
		return (-1);
	}

	/*
	 *	Lê os tokens do arquivos
	 */
	while (fngets (area, sizeof (area), fp) != NOSTR)
	{
		if (area[0] == '#')
			continue;

		for (token = strtok (area, " ,\t"); token != NOSTR; token = strtok (NOSTR, " ,\t"))
			strcpy (universal[i++], token);
	}

	fclose (fp);

	if (i != universal_size)
	{
		fprintf
		(	stderr,
			"strftime: O arquivo \"%s\" contém %d definições ao invés de %d\n",
			file_nm, i, universal_size
		);
		return (-1);
	}

	/*
	 *	Atualiza a localização atual
	 */
	strcpy (present_locale, _lc_vector[LC_TIME]);
	return (0);

}	/* end time_locale */
