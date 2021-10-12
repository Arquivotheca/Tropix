/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Comparação remota de árvores (cliente) 			*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <a.out.h>
#include <stat.h>
#include <string.h>
#include <errno.h>
#include <termio.h>
#include <xti.h>

#include "../../tnet.h"
#include "../h/tcmpto_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry int	sigint_received;	/* Veio o sinal SIGINT */

#ifdef	REPORTs
extern TERMIO	old_video, video;	/* modos do terminal */
#endif	REPORTs

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
 *	Imprime o nome do arquivo				*
 ****************************************************************
 */
void
print_file_nm (const char *file_nm, const STAT *sp, const char *str, char *file_printed)
{
	int		type, save_errno;

	save_errno = errno;

	if (*file_printed)
		return;

	if (sp == (STAT *)NULL)
		type = 0;
	else
		type = (sp->st_mode & S_IFMT) >> 12;

	fprintf (stderr, "%s%s%s", color_vector[type], file_nm, color_vector[MC_DEF]);

	if (str)
		fprintf (stderr, "%s", str);

	*file_printed = 1;

	errno = save_errno;

}	/* end printf_file_nm */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list		args;
	const char	*fmt = format;
	char		err, abrt;
	int		event, save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (tcp_fd);
	
			fprintf (stderr, " (%s)", t_strevent (tcp_fd, event));
	
			if (event == T_DISCONNECT)
				{ putc ('\n', stderr); quit (1); }
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	putc ('\n', stderr);

	if (abrt == '$')
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

	fprintf (stderr, "\t**** ");
	vfprintf (stderr, fmt, args);

#if (0)	/*******************************************************/
	putc ('\n', stderr);

	if (err == '*'  &&  save_errno > 0)
		{ msg (strerror (save_errno)); errno = 0; }
#endif	/*******************************************************/

	if (err == '*'  &&  save_errno > 0)
		{ fprintf (stderr, " (%s)", strerror (save_errno)); errno = 0; }

	putc ('\n', stderr);

	va_end (args);

}	/* end msg */

/*
 ****************************************************************
 *	Executa a opção "-s" (padrões de "C")			*
 ****************************************************************
 */
void
put_s_option (void)
{
	put_p_option ("*.[cshryv]", inc_pat, &inc_pati);
	put_p_option ("*[Mm]akefile", inc_pat, &inc_pati);

}	/* end put_s_option */

/*
 ****************************************************************
 *	Executa a opção "-p" (outros padrões)			*
 ****************************************************************
 */
int
put_p_option (const char *cp, const char *pat_vec[], int *pati_ptr)
{
	char		c;

	for (EVER)
	{
		while ((c = *cp) == ' ' || c == '\t' )
			cp++;

		if (c == '\0')
			return (0);

		if (*pati_ptr >= NPAT)
		{
			fprintf
			(	stderr,
				"%s: No momento somente aceitamos "
				XSTR (NPAT) " padrões\n\n",
				pgname
			);
			help ();
		}

		pat_vec[(*pati_ptr)++] = cp++;

		while ((c = *cp) != '\0' && c != ' ' && c != '\t')
			cp++;

		if (c == '\0')
			return (0);

		*(char *)cp++ = '\0';

	}	/* end for (EVER) */

}	/* end put_p_option */

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
 *	Imprime o tempo de transferência			*
 ****************************************************************
 */
void
print_time_interval (long byte_cnt, const MUTM *begin_time)
{
	MUTM		end_time;
	time_t		Time, Utime;
	long		mili_time, i;

	/*
	 *	Faz a diferença de tempos - cuidado com o "carry"
	 */
	mutime (&end_time);

	Time = end_time.mu_time - begin_time->mu_time;

	if ((Utime = end_time.mu_utime - begin_time->mu_utime) < 0)
	{
		Time  -= 1;		/* Tira um segundo daqui */
		Utime += 1000000;	/* E compensa aqui */
	}

	mili_time = (Utime + 500) / 1000;	/* Milésimos de seg. */

	/*
	 *	Agora imprime
	 */
	fprintf
	(	stderr,
		"\tTempo = %d:%02d.%03d",
		Time / 60, Time % 60, mili_time
	);

	if ((i = 1000 * Time + mili_time) > 0)
		fprintf (stderr, " (%d bytes/segundo)", 1000 * byte_cnt / i);

#if (0)	/*******************************************************/
	long	centitime, i;

	centitime = (Utime + 5000) / 10000;	/* Centésimos de seg. */

	/*
	 *	Agora imprime
	 */
	printf
	(	"\tTempo = %d:%02d.%02d",
		Time / 60, Time % 60, centitime
	);

	if ((i = 100 * Time + centitime) > 0)
		printf (" (%d bytes/segundo)", 100 * byte_cnt / i);
#endif	/*******************************************************/

}	/* end print_time_interval */

/*
 ****************************************************************
 *	Imprime o plural ou singular				*
 ****************************************************************
 */
char *
plural (int n)
{
	if (n == 1)
		return ("");
	else
		return ("s");

}	/* end plural */

/*
 ****************************************************************
 *	Em caso do sinal SIGINT					*
 ****************************************************************
 */
void
on_sigint (int signo, ...)
{
	tcp_abort (1);
#if (0)	/*******************************************************/
	if (fromflag)
		tcp_abort (1);
	else
		sigint_received++;
#endif	/*******************************************************/

}	/* end on_signal */

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
#ifdef	REPORTs
	if (old_video.c_name[0])
		ioctl (1, TCSETNR, &old_video);
#endif	REPORTs

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
#endif	LITTLE_ENDIAN
