/*
 ****************************************************************
 *								*
 *			to.c					*
 *								*
 *	Comparação remota de árvores (servidor)			*
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
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <termio.h>
#include <xti.h>

#include "../../tnet.h"
#include "../../reduce.h"
#include "../h/tcmpto_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry char	snd_rcv_buf[BLSZ];		/* Área dos blocos a transmitir */

/*
 ****************************************************************
 *	Copia um arquivo para o cliente				*
 ****************************************************************
 */
void
copy_to (const char *file_nm)
{
	off_t		n;
	int		fd = -1;
	STAT		s;
#ifdef	LITTLE_ENDIAN
	STAT		big_s;
#endif	LITTLE_ENDIAN
	char		*rmsg;

	/*
	 *	Obtém o estado do arquivo
	 */
	if (lstat (file_nm, &s) < 0)
	{
		t_snd_msg (-1, strerror (errno));
		return;
	}

	if (S_ISREG (s.st_mode) && (fd = inopen (s.st_dev, s.st_ino)) < 0)
	{
		t_snd_msg (-1, strerror (errno));
		return;
	}

	/*
	 *	Transmite o nome e estado do arquivo
	 */
	t_snd_msg_no_push (0, NOSTR);

#ifdef	LITTLE_ENDIAN
	stat_endian_cv (&big_s, &s);

	if (t_snd (tcp_fd, &big_s, sizeof (STAT), T_PUSH) < 0)
		error (NOSTR);
#else
	if (t_snd (tcp_fd, &s, sizeof (STAT), T_PUSH) < 0)
		error (NOSTR);
#endif	LITTLE_ENDIAN

	/*
	 *	Se for um elo simbólico, envia também o conteúdo
	 */
	if (S_ISLNK (s.st_mode))
	{
		char		*local_symlink_nm;

		n = s.st_size + 1; local_symlink_nm  = alloca (n);

		if (readlink (file_nm, local_symlink_nm, n) < 0)
			error ("$*Não consegui obter o conteúdo do elo \"%s\"", file_nm);

		if (t_snd (tcp_fd, local_symlink_nm, n, 0) < 0)
			error (NOSTR);

		t_push (tcp_fd);
	}

	/*
	 *	Espera confirmação de criação do arquivo na estação remota
	 */
	if (t_rcv_msg (&rmsg) < 0)
	{
		if (fd > 0)
			close (fd);
		return;
	}

	/*
	 *	Se não for regular, não precisa de enviar o conteúdo
	 */
	if (!S_ISREG (s.st_mode))
		return;

	/*
	 *	Transmite os segmentos
	 */
	if (Cflag && s.st_size >= MIN_COMPRESS_SZ)
	{
		reduce (fd, snd_rcv_buf);
	}
	else
	{
		for
		(	/* vazio */;
			(n = read (fd, snd_rcv_buf, BLSZ)) > 0;
			/* vazio */
		)
		{
			if (t_snd (tcp_fd, snd_rcv_buf, n, 0) < 0)
				error (NOSTR);

		}	/* loop pelo segmentos */
	
		if (n < 0)
			error ("$*Erro de leitura do arquivo \"%s\"", file_nm);
	}

	t_push (tcp_fd);

	close (fd);

}	/* end file_copy */

/*
 ****************************************************************
 *	Transmite um segmento compactado			*
 ****************************************************************
 */
void
send_seg (char c, COLFILE *fp)
{
	if (t_snd (tcp_fd, snd_rcv_buf, sizeof (snd_rcv_buf), 0) < 0)
		error (NOSTR);

	fp->i_ptr = fp->i_base;

	*fp->i_ptr++ = c;

}	/* end send_seg */
