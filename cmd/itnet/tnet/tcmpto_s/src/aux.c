/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Comparação remota de árvores (servidor) 		*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_S/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <a.out.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <xti.h>

#include "../h/tcmpto_s.h"
#include "../../tnet.h"

/*
 ****************************************************************
 *	Envia um segmento de mensagem (com T_PUSH)		*
 ****************************************************************
 */
void
t_snd_msg (int code, const char *snd_msg)
{
	t_snd_msg_no_push (code, snd_msg);

	t_push (tcp_fd);

}	/* end t_snd_msg */

/*
 ****************************************************************
 *	Envia um segmento de mensagem (sem T_PUSH)		*
 ****************************************************************
 */
void
t_snd_msg_no_push (int code, const char *snd_msg)
{
	MSG		m;
	int		len;

	m.m_type = ENDIAN_LONG (FMSG);
	m.m_code = ENDIAN_LONG (code);
	m.m_len	 = 0;

	if (snd_msg == NOSTR)
	{
		if (t_snd (tcp_fd, &m, sizeof (m), 0) != sizeof (m))
			error (NOSTR);
	}
	else
	{
		len = strlen (snd_msg) + 1;
		m.m_len = ENDIAN_LONG (len);

		if (t_snd (tcp_fd, &m, sizeof (m), 0) != sizeof (m))
			error (NOSTR);

		if (t_snd (tcp_fd, snd_msg, len, 0) != len)
			error (NOSTR);
	}

}	/* end t_snd_msg_no_push */

/*
 ****************************************************************
 *	Recebe um segmento de mensagem				*
 ****************************************************************
 */
int
t_rcv_msg (char **rcv_msg)
{
	MSG		m;
	int		flags, len;
	static char	rmsg[BLSZ];

	if (t_rcv (tcp_fd, &m, sizeof (m), &flags) != sizeof (m))
		error (NOSTR);

	if (ENDIAN_LONG (m.m_type) != FMSG)
		error ("$Esperava um segmento FMSG");

	len = ENDIAN_LONG (m.m_len);

	if (len != 0)
	{
		if ((unsigned)len > sizeof (rmsg))
			error ("$Mensagem com tamanho inválido: %d", len);

		if (t_rcv (tcp_fd, rmsg, len, &flags) != len)
			error (NOSTR);
	}

	if (rcv_msg != (char **)NULL)
	{
		if (len > 0)
			*rcv_msg = rmsg;
		else
			*rcv_msg = NOSTR;
	}

	return (ENDIAN_LONG (m.m_code));

}	/* end t_rcv_msg */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Testa em primeiro lugar os pontos
	 */
	if (!dotflag)
	{
		if (patmatch (file_nm, ".!.*") || patmatch (file_nm, "*/.!.*"))
			return (0);
	}

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	/*
	 *	Finalmente testa os padrões de exclusão
	 */
	if ((pp = exc_pat)[0] != NOSTR)
	{
		for (/* vazio */; *pp != NOSTR; pp++)
		{
			if (patmatch (file_nm, *pp))
				return (0);
		}
	}

	return (1);

}	/* end pattern_accept */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (servidor)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	FILE		*log_fp;
	char		err, abrt;
	int		event = 0, save_errno = errno;
	time_t		time_loc;

	va_start (args, format);

	log_fp = global_log_fp;

	/*
	 *	Processa os argumentos da cadeia
	 */
	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
	{
		fmt++;

		if (save_errno == TLOOK)
			event = t_look (tcp_fd);
	}

	/*
	 *	Escreve as mensagens
	 */
	if (log_fp != NOFILE)
	{
		/* Escreve a mensagem com o tempo */

		fprintf  (log_fp, pgname);

		time (&time_loc);
		fprintf (log_fp, " (%.24s)", btime (&time_loc));

		fprintf  (log_fp, ": ", pgname);
		vfprintf (log_fp, fmt, args);

		/* Escreve o tipo do erro */

		if (err == '*')
		{
			if (event != 0)
				fprintf (log_fp, " (%s)", t_strevent (tcp_fd, event));
			elif (save_errno != 0)
				fprintf (log_fp, " (%s)", strerror (save_errno));
		}

		putc ('\n', log_fp); fflush (log_fp);
	}

	if (abrt == '$' || event ==  T_DISCONNECT)
		tcp_abort (1);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Emite uma mensagem					*
 ****************************************************************
 */
void
msg (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	int		save_errno = errno;
	char		err;

	va_start (args, format);

	if ((err = *fmt) == '*')
		fmt++;

	printf ("\t**** ");
	vprintf (fmt, args);
	putchar ('\n');

	if (err == '*'  &&  save_errno > 0)
		{ msg (strerror (save_errno)); errno = 0; }

	va_end (args);

}	/* end msg */

/*
 ****************************************************************
 *	Em caso dos outros sinais				*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	tcp_abort (1);

}	/* end on_signal */

/*
 ****************************************************************
 *	Encerra as atividades (desconectando o TCP)		*
 ****************************************************************
 */
void
tcp_abort (int n)
{
	if (snd_dis)
		t_snddis (tcp_fd, (T_CALL *)NULL);

	quit (n);

}	/* end tcp_abort */

/*
 ****************************************************************
 *	Encerra as atividades					*
 ****************************************************************
 */
void
quit (int n)
{
	exit (n);

}	/* end quit */

#ifdef	LITTLE_ENDIAN
/*
 ****************************************************************
 *	Conversão ENDIAN da estrutura STAT			*
 ****************************************************************
 */
void
stat_endian_cv (STAT *dsp, const STAT *ssp)
{
	dsp->st_dev   =	ENDIAN_SHORT (ssp->st_dev);
	dsp->st_ino   =	ENDIAN_SHORT (ssp->st_ino);
	dsp->st_mode  =	ENDIAN_LONG  (ssp->st_mode);
	dsp->st_nlink =	ENDIAN_SHORT (ssp->st_nlink);
	dsp->st_uid   =	ENDIAN_SHORT (ssp->st_uid);
	dsp->st_gid   =	ENDIAN_SHORT (ssp->st_gid);
	dsp->st_rdev  =	ENDIAN_SHORT (ssp->st_rdev);
	dsp->st_size  =	ENDIAN_LONG  (ssp->st_size);
	dsp->st_atime =	ENDIAN_LONG  (ssp->st_atime);
	dsp->st_mtime =	ENDIAN_LONG  (ssp->st_mtime);
	dsp->st_ctime =	ENDIAN_LONG  (ssp->st_ctime);

}	/* end stat_endian_cv */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Conversão ENDIAN da estrutura HEADER			*
 ****************************************************************
 */
void
header_endian_cv (HEADER *hp)
{
   	hp->h_machine	= ENDIAN_SHORT (hp->h_machine);	
	hp->h_magic	= ENDIAN_SHORT (hp->h_magic);
   	hp->h_version	= ENDIAN_SHORT (hp->h_version);
   	hp->h_flags	= ENDIAN_SHORT (hp->h_flags);
	hp->h_time	= ENDIAN_LONG  (hp->h_time);	
   	hp->h_serial	= ENDIAN_LONG  (hp->h_serial);
   	hp->h_tstart	= ENDIAN_LONG  (hp->h_tstart);
   	hp->h_dstart	= ENDIAN_LONG  (hp->h_dstart);
   	hp->h_entry	= ENDIAN_LONG  (hp->h_entry);
	hp->h_tsize	= ENDIAN_LONG  (hp->h_tsize);
	hp->h_dsize	= ENDIAN_LONG  (hp->h_dsize);
	hp->h_bsize	= ENDIAN_LONG  (hp->h_bsize);
	hp->h_ssize	= ENDIAN_LONG  (hp->h_ssize);
	hp->h_rtsize	= ENDIAN_LONG  (hp->h_rtsize);
	hp->h_rdsize	= ENDIAN_LONG  (hp->h_rdsize);
   	hp->h_lnosize	= ENDIAN_LONG  (hp->h_lnosize);
   	hp->h_dbsize	= ENDIAN_LONG  (hp->h_dbsize);

}	/* end header_endian_conversion */
#endif	/*******************************************************/
#endif	LITTLE_ENDIAN
