/*
 ****************************************************************
 *								*
 *			from.c					*
 *								*
 *	Comparação remota de árvores (servidor) 		*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.5.0, de 03.06.03				*
 *								*
 *	Módulo: TCMPTO_S/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <xti.h>

#include "../../tnet.h"
#include "../../reduce.h"
#include "../h/tcmpto_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern char	snd_rcv_buf[BLSZ];	/* Para receber os blocos dos arquivos */

int		make_remote_not_regular_file (const char *, STAT *, STAT *);
void		set_mode_and_time_of_file (const char *, STAT *);

/*
 ****************************************************************
 *	Copia um Arquivo					*
 ****************************************************************
 */
void
copy_from (const char *file_nm)
{
	int		fd;
	int		p;
	STAT		old_s, new_s;
	off_t		n;

	/*
	 *	Recebe o estado do arquivo
	 */
	if (t_rcv (tcp_fd, &new_s, sizeof (STAT), (int *)NULL) < 0)
		error (NOSTR);

#ifdef	LITTLE_ENDIAN
	stat_endian_cv (&new_s, &new_s);
#endif	LITTLE_ENDIAN

	/*
	 *	Verifica se o arquivo já existe e precisa ser removido
	 */
	old_s.st_mode = 0; old_s.st_nlink = 1;

	if (lstat (file_nm, &old_s) >= 0)
	{
		if   (S_ISDIR (old_s.st_mode))
		{
			if (S_ISDIR (new_s.st_mode))
				{ t_snd_msg (0, NOSTR); return; }  	/* OK */

			if (rmdir (file_nm) < 0)
			{
				t_snd_msg_no_push (-1, "Não consegui remover o diretório antigo");
				t_snd_msg	  (-1, strerror (errno));
				return;
			}

			old_s.st_mode = 0;
		}
		elif ((old_s.st_mode & S_IFMT) != (new_s.st_mode & S_IFMT))
		{
			if (unlink (file_nm) < 0)
			{
				t_snd_msg_no_push (-1, "Não consegui remover o arquivo antigo");
				t_snd_msg	  (-1, strerror (errno));
				return;
			}

			old_s.st_mode = 0;
		}
	}

	/*
	 *	Cria os diretórios intermediários
	 */
	errno = 0;

	if (verdir (file_nm) < 0)
	{
		t_snd_msg_no_push (-1, "Não consegui criar os diretórios intermediários");
		t_snd_msg	  (-1, strerror (errno));
		return;
	}

	/*
	 *	Verifica se o arquivo é regular
	 */
	if (!S_ISREG (new_s.st_mode))
	{
		if (make_remote_not_regular_file (file_nm, &old_s, &new_s) < 0)
			t_snd_msg (-1, strerror (errno));
		else
			t_snd_msg (0, NOSTR);	/* OK */
		return;
	}

	/*
	 *	Verifica se consegue criar o arquivo
	 */
	if ((fd = creat (file_nm, new_s.st_mode)) < 0)
	{
		t_snd_msg_no_push (-1, "Não consegui criar o arquivo");
		t_snd_msg	  (-1, strerror (errno));
		return;
	}

	t_snd_msg (0, NOSTR);	/* OK */

	/*
	 *	Recebe os segmentos de dados
	 */
	n = new_s.st_size;

	if (Cflag && n >= MIN_COMPRESS_SZ)
	{
		if (unreduce (fd) != n)
			error ("Erro de tamanho no arquivo \"%s\"", file_nm);
	}
	else
	{
		while (n > 0)
		{
			p = MIN (n, BLSZ);

			if (t_rcv (tcp_fd, snd_rcv_buf, p, (int *)NULL) != p)
				error (NOSTR);

			if (write (fd, snd_rcv_buf, p) != p)
				error ("$*Erro de escrita no disco");

			n -= p;

		}	/* loop pelos blocos do arquivo */
	}

	close (fd);

	/*
	 *	Acerta as características do arquivo
	 */
	set_mode_and_time_of_file (file_nm, &new_s);

}	/* end file_copy */

/*
 ****************************************************************
 *	Cria o arquivo não regular				*
 ****************************************************************
 */
int
make_remote_not_regular_file (const char *file_nm, STAT *old_sp, STAT *new_sp)
{
	LITEM		*lp;
	char		buf[BLSZ];
	int		status;

	/*
	 *	Remove o arquivo, caso ainda exista
	 */
	if (old_sp->st_mode != 0)
	{
		if (S_ISDIR (old_sp->st_mode))
			status = rmdir (file_nm);
		else
			status = unlink (file_nm);

		if (status < 0)
		{
			sprintf (buf, "Não consegui remover o arquivo (antigo) \"%s\"", file_nm);
			t_snd_msg_no_push (-1, buf);
			return (-1);
		}
	}

   /***	old_sp->st_mode = 0; ***/

	/*
	 *	Agora, verifica qual o tipo de arquivo a criar
	 */
	switch (new_sp->st_mode & S_IFMT)
	{
	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFIFO:
		if (mknod (file_nm, new_sp->st_mode, new_sp->st_rdev) < 0)
		{
			sprintf (buf, "Não consegui criar o arquivo \"%s\"", file_nm);
			t_snd_msg_no_push (-1, buf);
			return (-1);
		}

		break;

	    case S_IFLNK:
	    {
		int		n = new_sp->st_size + 1;
		char		*symlink_nm = alloca (n);

		if (t_rcv (tcp_fd, symlink_nm, n, (int *)NULL) != n)
			error (NOSTR);

		if (symlink (symlink_nm, file_nm) < 0)
		{
			sprintf (buf, "Não consegui criar o elo simbólico \"%s\"", file_nm);
			t_snd_msg_no_push (-1, buf);
			return (-1);
		}

		break;
	    }

	    case S_IFDIR:
		if (access (file_nm, F_OK) < 0 && mkdir (file_nm, new_sp->st_mode) < 0)
		{
			sprintf (buf, "Não consegui criar o diretório \"%s\"", file_nm);
			t_snd_msg_no_push (-1, buf);
			return (-1);
		}

		break;

	    default:
		sprintf (buf, "Tipo %d inválido de arquivo", (new_sp->st_mode & S_IFMT) >> 12);
		t_snd_msg_no_push (-1, buf);
		return (-1);
	}

	/*
	 *	Acerta as características do arquivo
	 */
	if (!S_ISLNK (new_sp->st_mode))
		set_mode_and_time_of_file (file_nm, new_sp);

	/*
	 *	Agora, cuida dos elos físicos
	 */
	if (old_sp->st_nlink == 1)
		return (0);

	if (lstat (file_nm, new_sp) < 0)
	{
		sprintf (buf, "Não consegui obter o novo estado do arquivo \"%s\"", file_nm);
		t_snd_msg_no_push (-1, buf);
		return (-1);
	}

	for (lp = remote_linklist_first; lp != (LITEM *)NULL; lp = lp->l_next)
	{
		if (lp->l_dev == old_sp->st_dev && lp->l_ino == old_sp->st_ino)
		{
			lp->l_dev = new_sp->st_dev; lp->l_ino = new_sp->st_ino;

			if (streq (file_nm, lp->l_name))
				continue;

			if (unlink (lp->l_name) < 0)
			{
				sprintf (buf, "Não consegui remover o elo físico \"%s\"", lp->l_name);
				t_snd_msg_no_push (-1, buf);
				return (-1);
			}

#if (0)	/*******************************************************/
			if (verdir (lp->l_name) < 0)
			{
				sprintf (buf, "Não consegui criar os diretórios intermediários de \"%s\"", lp->l_name);
				t_snd_msg_no_push (-1, buf);
				return (-1);
			}
#endif	/*******************************************************/

			if (link (file_nm, lp->l_name) < 0)
			{
				sprintf (buf, "Não consegui criar o elo físico \"%s\"", lp->l_name);
				t_snd_msg_no_push (-1, buf);
				return (-1);
			}
		}
	}

	return (0);

}	/* end make_remote_not_regular_file */

/*
 ****************************************************************
 *	Le um bloco da coleção e confere o CRC			*
 ****************************************************************
 */
int
receive_seg (COLFILE *fp)
{
	/*
	 *	Verifica se é a primeira utilização da GETC
	 */
	if (fp->i_base == NOSTR)
	{
		fp->i_base = snd_rcv_buf;
		fp->i_bend = fp->i_base + BLSZ;
	}

	/*
	 *	Obtém um segmento
	 */
	if (t_rcv (tcp_fd, snd_rcv_buf, BLSZ, (int *)NULL) != BLSZ)
		error (NOSTR);

   /***	in_cnt += BLSZ; ***/

	/*
	 *	Prepara o ponteiro
	 */
	fp->i_ptr = fp->i_base;

	return (*fp->i_ptr++);

}	/* end receive_seg */

/*
 ****************************************************************
 *	Cria os Diretorios Faltando				*
 ****************************************************************
 */
int
verdir (const char *file_nm)
{
	char	*cp;

	cp = alloca (strlen (file_nm) + 1); strcpy (cp, file_nm);
	
	for (file_nm = cp, cp++; *cp != '\0'; cp++)
	{
	    if (*cp == '/')
	    {
		*cp = '\0';

		if (access (file_nm, F_OK) < 0 && mkdir (file_nm, 0777) < 0)
			{ *cp = '/'; return (-1); }

		*cp = '/';
	    }
	}

	return (0);

}	/* end verdir */

/*
 ****************************************************************
 *	Atribui o dono/modo					*
 ****************************************************************
 */
void
set_mode_and_time_of_file (const char *file_nm, STAT *sp)
{
#if (0)	/*************************************/
	char		buf[BLSZ];
#endif	/*************************************/

	if (!mflag)
		return;

	if (chmod (file_nm, sp->st_mode & ~S_IFMT) < 0)
	{
#if (0)	/*************************************/
		sprintf (buf, "Não consegui restaurar a proteção de \"%s\"", file_nm);
		t_snd_msg_no_push (-1, buf);
		return (-1);
#endif	/*************************************/
	}

	if (utime (file_nm, &sp->st_atime) < 0)
	{
#if (0)	/*************************************/
		sprintf ("Não consegui restaurar os tempos de \"%s\"", file_nm);
		t_snd_msg_no_push (-1, buf);
		return (-1);
#endif	/*************************************/
	}

	if (chown (file_nm, sp->st_uid, sp->st_gid) < 0)
	{
#if (0)	/*************************************/
		sprintf ("Não consegui restaurar o dono de \"%s\"", file_nm);
		t_snd_msg_no_push (-1, buf);
		return (-1);
#endif	/*************************************/
	}

}	/* end set_mode_and_time_of_file */
