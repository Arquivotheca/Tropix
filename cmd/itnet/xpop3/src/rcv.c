/*
 ****************************************************************
 *								*
 *			rcv.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 14.09.02				*
 *		4.4.0, de 25.12.02				*
 *								*
 *	Módulo: xpop3						*
 *		Utilitários especiais				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stat.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

#include "../h/xpop3.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define REG_MODE	0600

/*
 ****************************************************************
 *	Atualiza a caixa postal local				*
 ****************************************************************
 */
void
do_receive (int unused)
{
	MBOX		*mp;
	int		index;
	int		total_size = 0, size = 0;

	/*
	 *	Calcula quantos bytes temos de trazer
	 */
	for (index = 0; index < mail_vec_sz; index++)
	{
		mp = mail_vec_tb[index];

		if (mp->m_status != 'N')
			continue;

		if (mp->m_rcv != DO)
			continue;

		total_size += mp->m_size;
	}

	draw_percent_percent (0, 100);

	do_XFlush ();

	/*
	 *	x
	 */
	for (index = 0; index < mail_vec_sz; index++)
	{
		mp = mail_vec_tb[index];

		if (mp->m_status != 'N')
			continue;

		if (mp->m_rcv != DO)
			continue;

		signal (SIGINT, SIG_IGN);

		get_message (mp);

		signal (SIGINT, SIG_DFL);

		/*
		 *	Atualiza a barra de percentagem
		 */
		size += mp->m_size;

		draw_percent_percent (size, total_size);

		do_XFlush ();

	}

	/*
	 *	Atualiza o arquivo de "status"
	 */
	rewind (local_status_fp);

	fprintf (local_status_fp, "R\n");

}	/* end update_local_mbox */

/*
 ****************************************************************
 *	Processa o resto da resposta				*
 ****************************************************************
 */
void
get_message (MBOX *mp)
{
	FILE		*fp;
	int		fd = -1;
	time_t		tempo;
	int		letra = 'a';
	char		*old_nm;
	char		line[DATASZ];
	char		*line_ptr;
	char		end_of_header_seen = 0;
	STAT		s;

#if (0)	/*******************************************************/
printf ("Trazendo carta %d, %d bytes\n", mp->m_no, mp->m_size);
#endif	/*******************************************************/

	if (comando (1, "RETR %d", mp->m_no) != OK)
	{
		xerror
		(	"Não consegui obter a mensagem %d (%s)\n",
			mp->m_no, mp->m_uidl
		);
		return;
	}

	/*
	 *	Obtém um nome único e cria o arquivo para a carta
	 */
	for (letra = 'p'; /* abaixo */; letra++)
	{
		if (letra > 'z')
			letra = 'a';

		if (letra == 'p' - 1)
			goto bad_without_close;

		time (&tempo);

		sprintf (mailbox_ptr, "l.%u.%c", tempo, letra);

		if ((fd = open (mailbox_nm, O_RDWR|O_CREAT|O_EXCL, REG_MODE)) >= 0)
			break;
	}

	/*
	 *	Acerta o UID do arquivo
	 */
	if (fstat (fd, &s) < 0)
	{
		error ("*Não consegui obter o estado do arquivo da carta \"%s\"", mailbox_nm);
		goto bad_with_close;
	}

	if
	(	(s.st_uid != local_user_uid || s.st_gid != local_user_gid) &&
		chown (mailbox_nm, local_user_uid, local_user_gid)
	)
		error ("*Não consegui modificar o dono/grupo do arquivo da carta \"%s\"", mailbox_nm);

	/*
	 *	Obtém um FILE para o arquivo
	 */
	if ((fp = fdopen (fd, "w")) == NOFILE)
	{
	    bad_with_close:
		close (fd);
	    bad_without_close:
		xerror ("*Não consegui criar o arquivo para a carta \"%s\"", mailbox_nm);
		return;
	}

	/*
	 *	Finalmente, escreve o conteúdo da carta
	 */
	fprintf (fp, "Status: U\n");
	fprintf (fp, "Uidl: %s\n", mp->m_uidl);

	for (EVER)
	{
#if (0)	/*******************************************************/
		if (fnrgets (line, sizeof (line), tcp_fp) == NOSTR)
#endif	/*******************************************************/
		if (fnrgets_tcp (line, sizeof (line)) == NOSTR)
			error ("$*Erro na leitura de uma mensagem do servidor");

		line_ptr = line;

		/* Cuida das linhas começando por "." */

		if   (line[0] == '\0')
		{
			end_of_header_seen = 1;
		}
		elif (line[0] == '.')
		{
			if (line[1] == '\0')
				break;

			if (line[1] == '.')
				line_ptr++;
		}

		if   (end_of_header_seen)
			fnputs (line_ptr, fp);
		elif (get_line_info (line_ptr, mp) >= 0)
			fnputs (line_ptr, fp);
	}

	fflush (fp); fstat (fileno (fp), &s); fclose (fp);

	/*
	 *	Verifica se pode trocar o nome do arquivo
	 */
	if (mp->m_time != -1)
	{
		old_nm = alloca (strlen (mailbox_nm) + 1); strcpy (old_nm, mailbox_nm);

		for (letra = 't'; letra < 'z'; letra++)
		{
			sprintf (mailbox_ptr, "l.%u.%c", mp->m_time, letra);

			rename (old_nm, mailbox_nm);

			break;
		}
	}

	/*
	 *	Atualiza "status"
	 */
	fseek (local_status_fp, 0, SEEK_END);

	fprintf (local_status_fp, "%s:%s:%d:%d\n", mailbox_ptr, mp->m_uidl, s.st_size, s.st_ino);

	/*
	 *	Atualiza o botão
	 */
	mp->m_rcv = DONE;

	draw_mail_rcv_button (mp, mp->m_index - FIRST_LINE);

	do_XFlush ();

}	/* get_message */
