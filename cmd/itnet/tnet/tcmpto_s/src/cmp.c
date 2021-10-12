/*
 ****************************************************************
 *								*
 *			tcmpto_s.c				*
 *								*
 *	Comparação remota de árvores (servidor)			*
 *								*
 *	Versão	2.3.0, de 29.10.91				*
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
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ftw.h>
#include <a.out.h>
#include <xti.h>

#include "../h/tcmpto_s.h"
#include "../../tnet.h"

/*
 ****************************************************************
 *	Compara o arquivo 					*
 ****************************************************************
 */
int
file_cmp (const char *file_nm, STAT *sp, int status)
{
	int		fd = 0;
	int		i, n, mod, code;
	char	 	*rmsg;
	STAT		s;
#ifdef	LITTLE_ENDIAN
	STAT		big_s;
#endif	LITTLE_ENDIAN
	char		buf[BLSZ];
	short		crc_buf[SEGCRCSZ];

	/*
	 *	Obtém o estado, se ainda não tem
	 */
	if   (status == FTW_NS || status == FTW_DNR)
		sp = (STAT *)NULL;
	elif (sp == (STAT *)NULL &&  lstat (file_nm, &s) >= 0)
		sp = &s;

	/*
	 *	Abre o arquivo, se for regular
	 */
	if (sp != (STAT *)NULL  &&  S_ISREG (sp->st_mode))
		fd = inopen (sp->st_dev, sp->st_ino);

	/*
	 *	Verifica se teve algum erro
	 *	Repare que não deixa o arquivo aberto 
	 */
	if (sp == (STAT *)NULL  ||  fd < 0)
	{
		t_snd_msg (-1, strerror (errno));

		return (t_rcv_msg (&rmsg));
	}

	/*
	 *	Não teve erros: envia o STAT
	 */
	t_snd_msg_no_push (0, NOSTR);

#ifdef	LITTLE_ENDIAN
	stat_endian_cv (&big_s, sp);

	if (t_snd (tcp_fd, &big_s, sizeof (STAT), 0) < 0)
		error (NOSTR);
#else
	if (t_snd (tcp_fd, sp, sizeof (STAT), 0) < 0)
		error (NOSTR);
#endif	LITTLE_ENDIAN

	/*
	 *	Verifica se tem alguma LINK no nó remoto
	 */
	if ((rmsg = remote_link_proc (file_nm, sp)) != NOSTR)
		t_snd_msg_no_push (1, rmsg);
	else
		t_snd_msg_no_push (0, NOSTR);

	/*
	 *	Verifica se deve mandar o cabeçalho de um módulo objeto
	 */
	if (qflag && S_ISREG (sp->st_mode))
	{
		HEADER		h;

		if (read (fd, &h, sizeof (HEADER)) < 0)
			error ("$Erro de leitura do arquivo \"%s\"", file_nm);

		if (h.h_magic == NMAGIC || h.h_magic == FMAGIC || h.h_magic == SMAGIC)
		{
			t_snd_msg_no_push (1, NOSTR);

			if (t_snd (tcp_fd, &h, sizeof (HEADER), 0) < 0)
				error (NOSTR);
		}
		else
		{
			t_snd_msg_no_push (0, NOSTR);
		}

		lseek (fd, 0, SEEK_SET);
	}

	t_push (tcp_fd);

	/*
	 *	Verifica se é para mandar os CRCs ou o conteúdo do elo simbólico
	 */
	if ((code = t_rcv_msg (&rmsg)) != FCMP)
		{ close (fd); return (code); }

	/*
	 *	Envia o conteúdo do elo simbólico
	 */
	if (S_ISLNK (sp->st_mode))
	{
		char		*remote_symlink = alloca (sp->st_size + 1);

		if (readlink (file_nm, remote_symlink, sp->st_size + 1) < 0)
			error ("$Erro de leitura do conteúdo do elo simbólico \"%s\"", file_nm);

		if (t_snd (tcp_fd, remote_symlink, sp->st_size + 1, 0) < 0)
			error (NOSTR);

		t_push (tcp_fd);

		return (t_rcv_msg (&rmsg));
	}

	/*
	 *	Envia os CRCs dos blocos
	 */
	for
	(	i = 0, mod = 0;
		(n = read (fd, buf, BLSZ)) > 0;
		i++
	)
	{
		/*
		 *	Verifica se é um módulo objeto
		 */
		if (i == 0)
		{
			HEADER		*hp; 

			hp = (HEADER *)buf;

			if (hp->h_magic == NMAGIC || hp->h_magic == FMAGIC || hp->h_magic == SMAGIC)
			{
				hp->h_time = 0;
			}
		}

		crc_buf[mod] = ENDIAN_SHORT (crc16 (buf, n));

		if (++mod >= SEGCRCSZ)
		{
			if (t_snd (tcp_fd, crc_buf, sizeof (crc_buf), 0) < 0)
				error (NOSTR);

			mod = 0;
		}

	}	/* loop pelo blocos */

	if (mod > 0)
	{
		if (t_snd (tcp_fd, crc_buf, mod * sizeof (short), 0) < 0)
			error (NOSTR);
	}

	t_push (tcp_fd);

	if (n < 0)
		error ("$Erro de leitura do arquivo \"%s\"", file_nm);

	close (fd);

	return (t_rcv_msg (&rmsg));	/* EOF do arquivo */

}	/* end file_cmp */
