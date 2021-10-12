/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
 *								*
 *      Versão  3.0.0, de 16.12.97				*
 *              4.0.0, de 09.08.00				*
 *								*
 *	Módulo: MAIL						*
 *              Utilitários de comunicação			*
 *              Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "../h/mail.h"

/*
 ******	Tabela de nomes de meses ********************************
 */
typedef char	QUATUOR[4];

const QUATUOR	get_date_month_table[] =
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
	"UT",	0,	"GMT",	0,
	"EST",	-5,	"EDT",	-4,
	"CST",	-6,	"CDT",	-5,
	"MST",	-7,	"MDT",	-6,
	"PST",	-8,	"PDT",	-7,
	""
};

/*
 ******	Protótipos de funções ***********************************
 */
void		do_mime_2 (const char *, char *);

/*
 ****************************************************************
 *	Le linha(s) de um arquivo (com linhas de continuação)	*
 ****************************************************************
 */
char *
funrgets (char *area, int count, FILE *fp)
{
	char		*cp = area;
	const char	*end_area = area + count - 1;
	int		c;
	char		mime2 = 0;

	/*
	 *	Le linha
	 */
	for (EVER)
	{
		if   ((c = getc (fp)) < 0)
		{
			if (cp == area)
				return (NOSTR);
			else
				{ *cp = '\0'; break; }
		}
		elif (c == '\r')
		{
			/* ignora */
		}
		elif (c == '?')
		{
			if (cp > area && cp[-1] == '=')
				mime2++;

			if (cp < end_area)
				*cp++ = c;
		}
		elif (c != '\n')				/* Caractere regular */
		{
			if (cp < end_area)
				*cp++ = c;
		}
		elif (cp == area)				/* '\n' */
		{
			*cp = '\0'; break;
		}
		elif ((c = getc (fp)) != ' ' && c != '\t')	/* '\n' */
		{
			ungetc (c, fp); *cp = '\0'; break;	/* Não é continuação */
		}
		else	/*  <sp> ou <ht> */			/* Continuação */
		{
			if (cp < end_area)
				*cp++ = ' ';
		}

	}	/* end for (EVER) */

	/*
	 *	Processa o MIME 2
	 */
	if (mime2)
		do_mime_2 (area, area);

	return (area);

}	/* end funrgets */

/*
 ****************************************************************
 *	Le uma linha de um arquivo (ignorando "\r")		*
 ****************************************************************
 */
char *
fnrgets (char *str, int n, FILE *fp)
{
	char	*cp = str;
	int	c = 0;

	while ((c = getc (fp)) >= 0)
	{
		if (c == '\r')
			continue;

		if (c == '\n')
			break;

		if (--n > 0)
			*cp++ = c;
	}

	if (cp == str && c < 0)
		return (NOSTR);

	*cp = '\0';

	return (str);

}	/* end fnrgets */

/*
 ****************************************************************
 *	Processa o MIME 2					*
 ****************************************************************
 */
void
do_mime_2 (const char *in_area, char *out_area)
{
	const char	*rp = in_area;
	char		*cp = out_area;
	int		c;
	char		questions = 0, q_field = 0;

	/*
	 *	Processa o MIME 2
	 */
	for (EVER)
	{
		if   ((c = *rp++) == '\0')
		{
			break;
		}
		elif (c == '=')
		{
			if   (questions == 0)
			{
				if (*rp == '?')
					{ questions++; rp++; }
				else
					*cp++ = c;
			}
			elif (q_field)
			{
				char	d, high = 0, low = 0;

				d = rp[0];

				if   (d >= '0' && d <= '9')
					high = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					high = d - 'A' + 10 + 1;

				d = rp[1];

				if   (d >= '0' && d <= '9')
					low = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					low = d - 'A' + 10 + 1;

				if (high && low)
				{
					*cp++ = (high - 1) << 4 | (low - 1);
					rp += 2;
				}
			}
		}
		elif (c == '?')
		{
			if (questions == 0)
				{ *cp++ = c; continue; }

			if   (++questions == 2)
			{
				if   (*rp == 'Q' || *rp == 'q')
				{
					q_field++; rp++;
				}
				elif (*rp == 'B' || *rp == 'b')
				{
					char		t;
					int		total_bits = 0;
					long		word = 0;

					rp++;	/* Pula o "B" */

					while ((c = *rp++) != 0)
					{
						if   (c == '=')
						{
							continue;
						}
						elif (c == '?')
						{
							if (++questions == 3)
								continue;

							questions--; rp--;

							break;
						}

						if ((t = base_64_tb[c]) == 0xFF)
							break;

						word <<= 6; word |= t;

						if ((total_bits += 6) >= 24)
						{
							*cp++ = (word >> 16);
							*cp++ = (word >> 8);
							*cp++ =  word;

							total_bits -= 24;
							word = 0;
						}
					}

					/* Emite os bytes residuais */

					if   (total_bits >= 18)
					{
						*cp++ = (word >> 10);
						*cp++ = (word >> 2);
					}
					elif (total_bits >= 12)
					{
						*cp++ = (word >> 4);
					}
				}
			}
		   /***	elif (questions == 3)	/* "?" após "Q" ou "B" ***/
		   /***	{	***/
		
		   /***	}	***/
			elif (questions >= 4)
			{
				if (*rp == '=')
				{
					rp++;
					questions = q_field = 0;
				}
				else
				{
					questions--;
					*cp++ = c;
				}
			}
		}
		elif (questions == 1)
		{
			/* Pula o "ISO-8859-1" */
		}
		elif (c == '_' && q_field)
		{
			*cp++ = ' ';
		}
		else	/* Caractere sem transformação */
		{
			*cp++ = c;
		}

	}	/* end for (EVER) */

	*cp = '\0';

}	/* end do_mime_2 */

/*
 ****************************************************************
 *	Analisa Palavras-chave					*
 ****************************************************************
 */
const char *
get_keyword (const char *lp, char *area, int count)
{
	const char	*end_area = area + count - 1;

	lp = skip_blanks (lp);

	while (CATEG (lp) & KEYWORD)
	{
		if (area < end_area)
			*area++ = *lp;

		lp++;
	}

	*area = '\0';

	return (lp);

}	/* end get_keyword */

/*
 ****************************************************************
 *	Analisa Valores						*
 ****************************************************************
 */
const char *
get_value (const char *lp, char *area, int count)
{
	const char	*end_area = area + count - 1;

	lp = skip_blanks (lp);

	if (*lp == '"')
	{
		lp++;

		while (*lp != '"' && *lp != '\0')
		{
			if (area < end_area)
				*area++ = *lp;

			lp++;
		}

		if (*lp == '"')
			lp++;
	}
	else
	{
		while (CATEG (lp) & VALUE)
		{
			if (area < end_area)
				*area++ = *lp;

			lp++;
		}
	}

	*area = '\0';

	return (lp);

}	/* end get_value */

/*
 ****************************************************************
 *	Analisa Átomos						*
 ****************************************************************
 */
const char *
get_atom (const char *lp, char *area, int count)
{
	const char	*end_area = area + count - 1;

	lp = skip_blanks (lp);

	while (CATEG (lp) & ATOM)
	{
		if (area < end_area)
			*area++ = *lp;

		lp++;
	}

	*area = '\0';

	return (lp);

}	/* end get_atom */

/*
 ****************************************************************
 *	Pula brancos						*
 ****************************************************************
 */
const char *
skip_blanks (const char *lp)
{
	int		level = 0;

	for (EVER)
	{
		while (CATEG (lp) & SPACE)
			lp++;

		switch (*lp++)
		{
		    case '(':
			level++;
			break;

		    case ')':
			level--;
			break;

		    case '\0':
			return (lp - 1);

		    default:
			if (level <= 0)
				return (lp - 1);

			break;

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end skip_blanks */

/*
 ****************************************************************
 *	Pula até o próximo ";"					*
 ****************************************************************
 */
const char *
skip_to_sm (const char *lp)
{
	char		in_quote = 0;
	int		level = 0;

	for (EVER) switch (*lp++)
	{
	    case '\0':
		return (lp - 1);

	    case ';':
		if (!in_quote && level <= 0)
			return (lp - 1);
		break;

	    case '"':
		if (level <= 0)
			in_quote ^= 1;
		break;

	    case '(':
		if (!in_quote)
			level++;
		break;

	    case ')':
		if (!in_quote)
			level--;
		break;

	    default:
		break;
	}

}	/* end skip_to_sm */

/*
 ****************************************************************
 *	Converte uma cadeia para maiúsculas			*
 ****************************************************************
 */
char *
strtoupper (char *str)
{
	char	*cp = str;

	if (cp == NOSTR)
		return (NOSTR);

	for (/* vazio */; *cp != '\0'; cp++)
		*cp = toupper (*cp);

	return (str);

}	/* end strtoupper */

/*
 ****************************************************************
 *	Aloca memória e copia					*
 ****************************************************************
 */
char *
alloc_and_copy (const char *str)
{
	char		*dst;

	if (str == NOSTR)
		return (NOSTR);

	dst = mem_alloc (strlen (str) + 1);

	strcpy (dst, str); return (dst);

}	/* end alloc_and_copy */

/*
 ****************************************************************
 *	Analisa "mailbox"					*
 ****************************************************************
 */
const char *
get_mailbox (const char *lp, char *area, int count)
{
	char		*begin_area = area;
	const char	*end_area = area + count - 1;
	char		less_seen = 0;
	char		c;

	/*
	 *	Prólogo
	 */
	for (EVER)
	{
		lp = skip_blanks (lp);

		switch (*lp)
		{
		    case '<':
			area = begin_area;
			less_seen++;
			lp++;
			break;

		    case '>':
			{ *area = '\0'; return (lp); }

		    case '"':
			lp++;

			for (EVER)
			{
				if   ((c = *lp++) == '"')
					break;
				elif (c == '\0')
					{ *area = '\0'; return (lp); }
			}

			break;

		    case '@':
		    case '.':
			if (area < end_area)
				*area++ = *lp;
			lp++;
			break;

		    case '\0':
			{ *area = '\0'; return (lp); }

		    default:
			if ((CATEG (lp) & ATOM) == 0)
				{ *area = '\0'; return (lp); }

			do
			{
				if (area < end_area)
					*area++ = *lp;

				lp++;
			}
			while (CATEG (lp) & ATOM);

			break;

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end get_mailbox */

/*
 ****************************************************************
 *	Converte o tempo para o formato interno			*
 ****************************************************************
 */
time_t
get_date (const char *date_str)
{
	const QUATUOR	*mp;
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
 *	Retira uma data						*
 ****************************************************************
 */
const char *
scan_date (const MDIREC *mp)
{
	TM			*tp;
	const char		*str;
	char			*cp;
	int			c, i = 0;
	static char		area[40];

	/*
	 *	Primeiro caso
	 */
	if (mp->m_time != -1)
	{
		tp = localtime (&mp->m_time);
		strftime (area, sizeof (area), "%x", tp);
		area[15] = ' ';
		strftime (area + 16, sizeof (area) - 16, "%X", tp);
		area[21] = '\0';

		return (area);
	}

	/*
	 *	Segundo caso
	 */
	str = mp->m_date;

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
 *	Prepara uma linha MIME (com linhas de continuação)	*
 ****************************************************************
 */
LINE *
get_mime_line (char *area, int count, LINE *lp)
{
	char		*cp = area;
	const char	*end_area = area + count - 1;
	const char	*rp = lp->l_text;
	int		c;
	char		mime2 = 0;

	/*
	 *	Le linha
	 */
	for (EVER)
	{
		if   ((c = *rp++) == '?')
		{
			if (cp > area && cp[-1] == '=')
				mime2++;

			if (cp < end_area)
				*cp++ = c;
		}
		elif (c != '\0')				/* Caractere regular */
		{
			if (cp < end_area)
				*cp++ = c;
		}
		elif ((lp = lp->l_forw)->l_head != 0)		/* Final de linha */
		{
			*cp = '\0'; break;
		}
		elif (cp == area)
		{
			*cp = '\0'; break;
		}
		elif ((c = lp->l_text[0]) == ' ' || c == '\t')	/* Continuação */
		{
			rp = lp->l_text + 1;

			if (cp < end_area)
				*cp++ = ' ';
		}
		else
		{
			*cp = '\0'; break;
		}

	}	/* end for (EVER) */

	/*
	 *	Processa o MIME 2
	 */
	if (mime2)
		do_mime_2 (area, area);

	return (lp);

}	/* end get_mime_line */
