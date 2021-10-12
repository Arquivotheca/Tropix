/*
 ****************************************************************
 *								*
 *			mbox.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 07.09.02				*
 *		4.4.0, de 03.05.03				*
 *								*
 *	Módulo: xpop3						*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>

#include "../h/xpop3.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOCSTR	(const char **)NULL

/*
 ******	Tabela de nomes de meses ********************************
 */
typedef char	LINE[4];

const LINE	get_date_month_table[] =
{
	"jan", "feb", "mar", "apr", "may", "jun",
 	"jul", "aug", "sep", "oct", "nov", "dec", ""
};

/*
 ******	Tabela de nomes de zonas de tempo ***********************
 */
typedef struct
{
	char	z_nm[4];	/* Nome da zona */
	int	z_zone;		/* Diferencial */

}	ZONE;

const ZONE	get_date_zone_table[] =
{
	"UT",	0, 	"GMT",	0,
	"EST",	-5, 	"EDT",	-4,
	"CST",	-6, 	"CDT",	-5,
	"MST",	-7, 	"MDT",	-6,
	"PST",	-8, 	"PDT",	-7,
	""
};

/*
 ****************************************************************
 *	Obtém o usuário local "default"				*
 ****************************************************************
 */
void
get_default_local_user_nm (void)
{
	const char	*str;

	str = getenv ("USER");

	if (str == NOSTR)
		str = getlogin ();

	if (str == NOSTR)
		str = (getpwuid (getuid ()))->pw_name;

	if (str == NOSTR)
		return;

	strcpy (local_user_nm, str); strcpy (old_local_user_nm, str);

}	/* end get_default_local_user_nm */

/*
 ****************************************************************
 *	Retira um endereço					*
 ****************************************************************
 */
char *
scan_address (const char *str)
{
	char		*less_point, *more_point;
	static char	area[80];

	/*
	 *	Verifica se não há cadeia
	 */
	if (str == NOSTR)
		{ area[0] = '\0'; return (area); }

	/*
	 *	Processa o endereço
	 */
	while (*str == ' ' || *str == '\t')
		str++;

	strncpy (area, str, sizeof (area));

	less_point = strchr (area, '<');

	if (less_point != NOSTR)
	{
		more_point = strchr (area, '>');

		if (more_point != NOSTR)
			*more_point = '\0';

		return (less_point + 1);
	}

	return (area);

}	/* end scan_address */

/*
 ****************************************************************
 *	Retira uma data						*
 ****************************************************************
 */
char *
scan_date (const MBOX *mp)
{
	const char		*str;
	char			*cp;
	int			c, i = 0;
	static char		area[40];

	/*
	 *	Primeiro caso
	 */
	if (mp->m_time != -1)
	{
		strftime (area, sizeof (area), "%x", localtime (&mp->m_time));

		return (area);
	}

	/*
	 *	Segundo caso
	 */
	if ((str = mp->m_date) == NOSTR)
		{ area[0] = '\0'; return (area); }

	while (*str == ' ' || *str == '\t')
		str++;

	cp = strncpy (area, str, sizeof (area));

	/*
	 *	Agora, procura o 4o. branco (após o ano)
	 */
	for (EVER)
	{
		if   ((c = *cp) == ' ' || c == '\t')
		{
			if (++i >= 4)
				{ *cp = '\0'; break; }
		}
		elif (c == '\0')
		{
			break;
		}

		cp++;
	}

	return (area);

}	/* end scan_date */

/*
 ****************************************************************
 *	Converte uma cadeia para maiúsculas			*
 ****************************************************************
 */
char *
strtoupper (char *str)
{
	char *cp = str;

	if (cp == NOSTR)
		return (NOSTR);

	for (/* vazio */; *cp != '\0'; cp++)
		*cp = toupper (*cp);

	return (str);

}	/* end strtoupper */

/*
 ****************************************************************
 *	Converte o tempo para o formato interno			*
 ****************************************************************
 */
time_t
get_date (const char *date_str)
{
	const LINE	*mp;
	const ZONE	*zp;
	char		*str, *cp;
	int		fuso = 0;
	char		sinal = 0;
	time_t		tempo;
	TM		tm;

	/*
	 *	Em primeiro lugar, copia a cadeia original
	 */
	if (date_str == NOSTR || date_str[0] == '\0')
		return (-1);

	str = alloca (strlen (date_str) + 1); strcpy (str, date_str);

	/*
	 *	Pula o dia da semana
	 */
	if ((cp = strchr (str, ',')) != NOSTR)
		str = cp + 1;

	/*
	 *	Retira o dia do mês
	 */
	if ((str = strtok (str, " \t")) == NOSTR)
		return (-1);

	tm.tm_mday = strtol (str, NOCSTR, 10);

	if (tm.tm_mday <= 0 || tm.tm_mday > 31)
		return (-1);

	/*
	 *	Retira o mês
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	for (cp = str; *cp != '\0'; cp++)
		*cp = _tolower (*cp);

	for (tm.tm_mon = 0, mp = get_date_month_table; /* abaixo */; tm.tm_mon++, mp++) 
	{
		if (mp[0][0] == '\0')
			return (-1);

		if (streq (str, mp[0]))
			break;
	}

	/*
	 *	Retira o ano
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	if ((tm.tm_year = strtol (str, NOCSTR, 10)) >= 1900)
		tm.tm_year -= 1900;

	if (tm.tm_year <= 37)
		tm.tm_year += 100;

	if (tm.tm_year < 70 || tm.tm_year > 137)
		return (-1);

	/*
	 *	Retira a hora
	 */
	if ((str = strtok (NOSTR, ":")) == NOSTR)
		return (-1);

	tm.tm_hour = strtol (str, NOCSTR, 10);

	if ((unsigned)tm.tm_hour > 23)
		return (-1);

	/*
	 *	Retira o minuto
	 */
	if ((str = strtok (NOSTR, ": \t")) == NOSTR)
		return (-1);

	tm.tm_min = strtol (str, NOCSTR, 10);

	if ((unsigned)tm.tm_min > 59)
		return (-1);

	/*
	 *	Retira o segundo
	 */
	if ((str = strtok (NOSTR, " \t")) == NOSTR)
		return (-1);

	if (*str >= '0' && *str <= '9')
	{
		tm.tm_sec = strtol (str, NOCSTR, 10);

		if ((unsigned)tm.tm_sec > 59)
			return (-1);

		if ((str = strtok (NOSTR, " \t")) == NOSTR)
			return (-1);
	}
	else
	{
		tm.tm_sec = 0;
	}

	/*
	 *	Retira a zona em horas/minutos
	 */
	if   (*str == '+' || *str == '-')
	{
		if (*str++ == '-')
			sinal++;

		fuso = strtol (str, NOCSTR, 10);

		/* Converte de horas/minutos */

		fuso = (fuso / 100) * 60 + (fuso % 100);

		if (sinal)
			fuso = -fuso;
	}
	elif (*str >= 'A' && *str <= 'Z')
	{
		for (zp = get_date_zone_table; /* abaixo */; zp++)
		{
			if (zp->z_nm[0] == '\0')
				{ /*** fuso = 0; ***/ break; }

			if (streq (zp->z_nm, str))
				{ fuso = zp->z_zone * 60; break; }
		}
	}

	/*
	 *	Epílogo
	 */
	tm.tm_isdst = 0;	/* Horário de verão = 1 */

	tempo = mktime (&tm) - fuso * 60;	/* UT */

	return (tempo);

}	/* end get_date */

/*
 ****************************************************************
 *	Armazena informação de uma linha de cabeçalho		*
 ****************************************************************
 */
int
get_line_info (const char *line, MBOX *mp)
{
	const char	*src;
	char		*dst;
	int		ret = 0, n;
	char		key_word[32];

	/*
	 *	Procura a palavra-chave
	 */
	if ((src = strchr (line, ':')) == NOSTR)
		return (0);

	if ((n = src - line) >= sizeof (key_word) - 1)
		return (0);

	strncpy (key_word, line, n); key_word[n] = '\0';

	src++;	/* Pula o ":" */

	while (*src == ' ' || *src == '\t')
		src++;

	/*
	 *	Procura o valor
	 */
	if (*src != '\0')
	{
		if ((dst = malloc (strlen (src) + 1)) == NOSTR)
			error ("$Memória esgotada");

		strcpy (dst, src);
	}
	else
	{
		dst = NOSTR;
	}

	if   (streq (strtoupper (key_word), "STATUS"))
	{
		ret = -1;	/* Ignora o STATUS */
	}
	elif (streq (key_word, "FROM"))
	{
		mp->m_from = dst;
	}
	elif (streq (key_word, "TO"))
	{
		mp->m_to = dst;
	}
	elif (streq (key_word, "DATE"))
	{
		mp->m_date = dst;
		mp->m_time = get_date (dst);
	}
	elif (streq (key_word, "SUBJECT"))
	{
		mp->m_subject = dst;
	}
	elif (streq (key_word, "UIDL"))
	{
		if (mp->m_uidl == NOSTR)
			mp->m_uidl = dst;

		ret = -1;	/* Ignora o UIDL */
	}

	return (ret);

}	/* end get_line_info */

/*
 ****************************************************************
 *	Prepara a linha de cabeçalho de uma mensagem		*
 ****************************************************************
 */
int
get_msg_info (MBOX *mp)
{
	char		*dst, *src;
	int		ret = 0;
	int		local_user_nm_len = strlen (local_user_nm);
	int		from_len, date_len, size_len;
	char		area[DATASZ];

	/*
	 *	Calcula os tamanhos dos campos
	 */
	memset (area, ' ', sizeof (area));

	from_len = NCOL / 2 - 20;

	if (from_len > 30)
		from_len = 30;

	size_len = 7;

	date_len = 16;

	/*
	 *	Cria o texto da linha completo
	 */
	sprintf (area, "%4d ", mp->m_index + 1); src = area + 5;

	dst = scan_address (mp->m_from);

	if (strncmp (dst, local_user_nm, local_user_nm_len) || mp->m_to == NOSTR)
	{
		sprintf (src, "De:   ");
	}
	else
	{
		dst = scan_address (mp->m_to);
		sprintf (src, "Para: ");
	}

	if ((dst = strtok (dst, " \t,;")) == NOSTR)
		dst = "";
	else
		do_mime_2 (dst, dst);

	src += 6;
	sprintf (src, "%-*.*s    ", from_len, from_len, dst);

	src += from_len + 2;
	sprintf (src, "%-*.*s    ", date_len, date_len, scan_date (mp));

	src += date_len + 2;
	sprintf (src, "%*.*s    ", size_len, size_len, edit_size (mp->m_size));

	src += size_len + 2;
	do_mime_2 (mp->m_subject, src);

	(strend (src))[0] = ' ';

	/* Cria a linha */

	if ((mp->m_text = malloc (NCOL + 1)) == NOSTR)
		error ("$Memória esgotada");

	mp->m_len = NCOL; memmove (mp->m_text, area, NCOL);

	return (ret);

}	/* end get_msg_info */

