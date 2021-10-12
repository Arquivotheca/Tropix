/*
 ****************************************************************
 *								*
 *			mbox.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente)		*
 *								*
 *	Versão	3.1.0, de 09.10.98				*
 *		3.1.0, de 09.10.98				*
 *								*
 *	Módulo: POP3_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <xti.h>

#include "../h/pop3_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define DIR_MODE 0700

#define REG_MODE 0600 

#define	NOCSTR	(const char **)NULL

#define	MAIL_PREFIX	"/var/mail/"

entry char		mailbox_nm[80] = MAIL_PREFIX;
entry char		*mailbox_ptr;

const char		status_nm[] = "status";

entry FILE		*status_fp;

entry int		mailbox_dev;	/* Dispositivo da caixa de correio */
entry int		mailbox_ino;	/* Nó-índice da caixa de correio */
entry time_t		mailbox_time;	/* Última modificação da caixa de correio */

entry MBOX		mbox_list_head;	/* Cabeça da lista de mensagens */

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
 *	Lê as cartas da caixa postal local			*
 ****************************************************************
 */
void
get_mbox (void)
{
	MBOX		*mp, *bp, *fp;
	const char	*file_nm, *uidl, *size_str, *ino_str;
	int		index = 0;
	ino_t		ino = 0;
	off_t		size = 0;
	int		bad;
	STAT		s;
	char		area[DATASZ];

	/*
	 *	Inicializa a lista de mensagens
	 */
	mbox_list_head.m_forw = &mbox_list_head;
	mbox_list_head.m_back = &mbox_list_head;

	/*
	 *	Abre/cria a caixa postal
	 */
	strcat (mailbox_nm, local_user_nm);

	if (stat (mailbox_nm, &s) < 0)
	{
		if (errno != ENOENT)
			error ("$*Não consegui abrir a caixa postal \"%s\"", mailbox_nm);

		if (mkdir (mailbox_nm, DIR_MODE) < 0)
			error ("$*Não consegui criar a caixa postal \"%s\"", mailbox_nm);

		if (stat (mailbox_nm, &s) < 0)
			error ("$*Não consegui abrir a caixa postal \"%s\"", mailbox_nm);

		if ((s.st_uid != user_uid || s.st_gid != user_gid) && chown (mailbox_nm, user_uid, user_gid))
			error ("*Não consegui modificar o dono/grupo do arquivo da carta \"%s\"", mailbox_nm);
	}

	if (!S_ISDIR (s.st_mode))
		error ("$A caixa postal \"%s\" NÃO é um diretório", mailbox_nm);

#if (0)	/*******************************************************/
	if ((mailbox_fp = inopendir (s.st_dev, s.st_ino)) == NODIR)
		error ("$*Não consegui abrir a caixa postal \"%s\"", mailbox_nm);
#endif	/*******************************************************/

	strcat (mailbox_nm, "/"); mailbox_ptr = strend (mailbox_nm);

	mailbox_dev  = s.st_dev;
	mailbox_ino  = s.st_ino;
	mailbox_time = s.st_mtime;

	/*
	 *	Abre o arquivo de índices "status"
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((status_fp = fopen (mailbox_nm, "r+")) == NOFILE)
		{ creat (mailbox_nm, REG_MODE); chown (mailbox_nm, user_uid, user_gid); return; }

#if (0)	/*******************************************************/
		error ("*Não consegui abrir o arquivo \"%s\"", mailbox_nm);
#endif	/*******************************************************/

	/*
	 *	Lê o arquivo de índices "status"
	 */
	fseek (status_fp, 2, SEEK_SET);

	while (fngets (area, sizeof (area), status_fp) != NOSTR)
	{
		bad = 0;

		if ((file_nm = strfield (area, ':')) == NOSTR)
			bad = 1;

		if ((uidl = strfield (NOSTR, ':')) == NOSTR)
			 bad = 2;

		if
		(	(size_str = strfield (NOSTR, ':')) == NOSTR ||
			(size = atol (size_str)) == 0
		)
			bad = 3;

		if
		(	(ino_str = strfield (NOSTR, ':')) == NOSTR ||
			(ino = atol (ino_str)) == 0
		)
			bad = 4;

		if (bad)
			{ error ("Arquivo \"%s\" com erro %d", mailbox_nm, bad); continue; }

		/*
		 *	Insere na lista
		 */
		if ((mp = malloc (sizeof (MBOX))) == NOMBOX)
			error ("$Memória esgotada");

		if ((mp->m_nm = malloc (strlen (file_nm) + 1)) == NOSTR)
			error ("$Memória esgotada");

		if ((mp->m_uidl = malloc (strlen (uidl) + 1)) == NOSTR)
			error ("$Memória esgotada");

		strcpy (mp->m_nm, file_nm);
		mp->m_ino  = ino;

	   /***	mp->m_index   = ...; ***/

	   	mp->m_status  = ' ';	/* Ainda não tem informações sobre a carta */
	   	mp->m_quoted  = 0;

		strcpy (mp->m_uidl, uidl);
	   	mp->m_from    = "";
	   	mp->m_to      = "";
	   	mp->m_date    = "";
	   	mp->m_time    = -1;
	   	mp->m_subject = "";
		mp->m_size    = size;

	   	mp->m_text    = "";
	   	mp->m_len     = 0;

	   	mp->m_local   = NOMBOX;
	   /***	mp->m_back    = ...;
	   /***	mp->m_forw    = ...;

		/*
		 *	Insere na lista
		 */
		for (fp = mbox_list_head.m_back; fp != &mbox_list_head; fp = fp->m_back)
		{
			ulong		mp_no, fp_no;

			mp_no = atol (mp->m_nm + 2);
			fp_no = atol (fp->m_nm + 2);

			if (mp_no != 0 && fp_no != 0)
			{
				if (mp_no >= fp_no)
					break;
			}
			else
			{
				if (strcmp (mp->m_nm, fp->m_nm) >= 0)
					break;
			}
		}

		bp = fp->m_forw;

		bp->m_back = mp; mp->m_forw = bp;
		mp->m_back = fp; fp->m_forw = mp;

	}	/* lendo e escrevendo as linhas dos arquivos */

   /***	fclose (status_fp); ***/

	/*
	 *	Verifica se leu alguma carta
	 */
	if (mbox_list_head.m_forw == &mbox_list_head)
		return;

	/*
	 *	Numera as cartas
	 */
	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		mp->m_index = ++index;

#undef	DEBUG
#ifdef	DEBUG
	/*
	 *	Imprime a lista de arquivos
	 */
	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		printf ("%s\r\n", mp->m_text);
#endif	DEBUG

}	/* end get_mbox */

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

#if (0)	/*******************************************************/
	printf ("%s => (%u) ", date_str, tempo);
	printf ("%s\r", ctime (&tempo));
	getchar ();
#endif	/*******************************************************/

	return (tempo);

}	/* end get_date */

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
		dst = "";
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
		if (mp->m_uidl[0] == '\0')
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
	int		n;

	/*
	 *	Calcula os tamanhos dos campos
	 */
	memsetl (area, ' ', sizeof (area) / sizeof (long));

	from_len = LINE_SZ / 2 - 20;

	if (from_len > 30)
		from_len = 30;

	size_len = 7;

	date_len = 16;

	/*
	 *	Cria o texto da linha completo
	 */
	src = area; dst = scan_address (mp->m_from);

	if
	(	strncmp (dst, local_user_nm, local_user_nm_len) ||
		*mp->m_to == '\0'
	)
	{
		sprintf (src, "From: ");
	}
	else
	{
		dst = scan_address (mp->m_to);
		sprintf (src, "To:   ");
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

	/* Cria a linha */

	n = strlen (area);

	if (n > LINE_SZ)
		n = LINE_SZ;

	if ((mp->m_text = malloc (n + 1)) == NOSTR)
		error ("$Memória esgotada");

	mp->m_len  = n; strncpy (mp->m_text, area, n); mp->m_text[n] = '\0';

	return (ret);

}	/* end get_msg_info */

/*
 ****************************************************************
 *	Edita um tamanho de arquivo				*
 ****************************************************************
 */
char *
edit_size (int size)
{
	static char	size_str[16];
	char		*cp = size_str;

	if   (size < 1 * KBSZ)
		sprintf (cp, "%4d  B", size);
	elif (size < 1024 * KBSZ)
		sprintf (cp, "%4d KB", (size + KBSZ  - 1) / KBSZ);
	else
		sprintf (cp, "%4d MB", (size + MBSZ  - 1) / MBSZ);

	while (*cp == ' ')
		cp++;

	return (cp);

}	/* end edit_size */
