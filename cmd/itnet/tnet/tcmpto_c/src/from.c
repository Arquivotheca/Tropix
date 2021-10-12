/*
 ****************************************************************
 *								*
 *			from.c					*
 *								*
 *	Comparação remota de árvores (cliente)	 		*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.5.0, de 03.06.03				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
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
#include <errno.h>
#include <unistd.h>
#include <termio.h>
#include <xti.h>

#include "../../tnet.h"
#include "../../reduce.h"
#include "../h/tcmpto_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern int	MSPEED;			/* Velocidade da linha */

extern char	snd_rcv_buf[BLSZ];	/* Área dos blocos a transmitir */

extern const char *info_file_nm;	/* Especial para info */
extern int	info_cnt_total;

extern int	MAX_BIT_USED;		/* O no. de bits realmente usados */ 

extern long	in_cnt, out_cnt;	/* Total de caracteres de entrada/saída */

/*
 ****** Protótipos de funções ***********************************
 */
int		verdir (const char *);
void		make_local_not_regular_file (const char *, STAT *, STAT *);
void		set_mode_and_time_of_file (const char *, STAT *);

extern void	print_time_interval (long, MUTM *);

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
	STAT		new_s, old_s;
	off_t		n;
	char		*rmsg;
	int		seg;
	MUTM		begin_time;

	/*
	 *	Transmite o nome do arquivo
	 */
	t_snd_msg (CPFROM, file_nm);

	/*
	 *	Verifica se o arquivo existe no nó remoto
	 */
	if (t_rcv_msg (&rmsg) < 0)
	{
		msg
		(	"Não consegui obter o estado do arquivo em \"%s\"",
			remote_node_nm
		);
		msg (rmsg);
		return;
	}

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
			{
				t_snd_msg (-1, NOSTR);		/* OK, EOF do arquivo */
				return;
			}

			if (rmdir (file_nm) < 0)
			{
				msg ("*Não consegui remover o diretório antigo");
				t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
				return;
			}

			old_s.st_mode = 0;
		}
		elif ((old_s.st_mode & S_IFMT) != (new_s.st_mode & S_IFMT))
		{
			if (unlink (file_nm) < 0)
			{
				msg ("*Não consegui remover o arquivo antigo");
				t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
				return;
			}

			old_s.st_mode = 0;
		}
	}

	/*
	 *	Cria os diretórios intermediários
	 */
	errno = 0;

	if (!pipeflag && verdir (file_nm) < 0)
	{
		t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
		return;
	}

	/*
	 *	Verifica se o arquivo é regular
	 */
	if (!S_ISREG (new_s.st_mode))
	{
		if (pipeflag)
			msg ("O arquivo \"%s\" não é regular", file_nm);
		else
			make_local_not_regular_file (file_nm, &old_s, &new_s);

		t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
		return;
	}

	/*
	 *	Verifica se consegue criar o arquivo
	 */
	if   (pipeflag)
	{
		fd = 1;		/* Saída padrão */
	}
	elif ((fd = creat (file_nm, new_s.st_mode)) < 0)
	{
		msg ("*Não consegui criar o arquivo em \"%s\"\n", local_node_nm);

		t_snd_msg (-1, NOSTR);		/* EOF do arquivo */
		return;
	}

	t_snd_msg (0, NOSTR);	/* Envie os blocos */

	/*
	 *	Avalia o tempo
	 */
	n = new_s.st_size;

	if (vflag)
	{
		seg = n / MSPEED;

		fprintf
		(	stderr,
			"\t%d bytes, tempo previsto = %d:%02d\n",
			n, seg / 60, seg % 60
		);
	}

	/*
	 *	Prepara os dados para "info"
	 */
	info_file_nm = file_nm;
	in_cnt = 0;	out_cnt = 0;
	info_cnt_total = n;

	mutime (&begin_time);

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
	
			in_cnt += n;
#ifdef	REPORTs
			if (!pipeflag && ioctl (fileno (tty_fp), TCNREAD, 0) > 0)
				info ();
#endif	REPORTs
			n -= p;
	
		}	/* loop pelos blocos do arquivo */
	}

	if (!pipeflag)
		close (fd);

	/*
	 *	Mensagem final do arquivo
	 */
	if (vflag)
	{
		print_time_interval (new_s.st_size, &begin_time);

		if (Cflag && new_s.st_size >= MIN_COMPRESS_SZ) fprintf
		(	stderr,
			" \t(%s %%, %d)",
			editpercent (getpercent (out_cnt - in_cnt, out_cnt)),
			MAX_BIT_USED
		);

		fprintf (stderr, "\n");
	}

	/*
	 *	Acerta as características do arquivo
	 */
	set_mode_and_time_of_file (file_nm, &new_s);

}	/* end copy_from */

/*
 ****************************************************************
 *	Cria o arquivo não regular				*
 ****************************************************************
 */
void
make_local_not_regular_file (const char *file_nm, STAT *old_sp, STAT *new_sp)
{
	LITEM		*lp;
	int		status;

	/*
	 *	Em seguida, remove o arquivo, caso ainda exista
	 */
	if (old_sp->st_mode != 0)
	{
		if (S_ISDIR (old_sp->st_mode))
			status = rmdir (file_nm);
		else
			status = unlink (file_nm);

		if (status < 0)
		{
			msg ("*Não consegui remover o arquivo (antigo) \"%s\"", file_nm);
			return;
		}
	}

   /***	old_sp->st_mode = 0; ***/

	/*
	 *	Agora, verifica qual o tipo de arquivo
	 */
	switch (new_sp->st_mode & S_IFMT)
	{
	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFIFO:
		if (mknod (file_nm, new_sp->st_mode, new_sp->st_rdev) < 0)
		{
			msg ("*Não consegui criar o arquivo \"%s\"", file_nm);
			return;
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
			msg ("*Não consegui criar o elo simbólico \"%s\"", file_nm);
			return;
		}

		break;
	    }

	    case S_IFDIR:
		if (access (file_nm, F_OK) < 0 && mkdir (file_nm, new_sp->st_mode) < 0)
		{
			msg ("*Não consegui criar o diretório \"%s\"", file_nm);
			return;
		}

		break;

	    default:
		msg ("Tipo %d inválido de arquivo", (new_sp->st_mode & S_IFMT) >> 12);
		return;
	}

	/*
	 *	Acerta as características do arquivo
	 */
	if (!S_ISLNK (new_sp->st_mode))
		set_mode_and_time_of_file (file_nm, new_sp);

	/*
	 *	Agora, cuida dos links
	 */
	if (old_sp->st_nlink == 1)
		return;

	if (lstat (file_nm, new_sp) < 0)
	{
		msg ("*Não consegui obter o novo estado do arquivo \"%s\"", file_nm);
			return;
	}

	for (lp = local_linklist_first; lp != (LITEM *)NULL; lp = lp->l_next)
	{
		if (lp->l_dev == old_sp->st_dev && lp->l_ino == old_sp->st_ino)
		{
			lp->l_dev = new_sp->st_dev; lp->l_ino = new_sp->st_ino;

			if (streq (file_nm, lp->l_name))
				continue;

			if (unlink (lp->l_name) < 0)
			{
				msg ("*Não consegui remover o elo físico \"%s\"", lp->l_name);
				return;
			}

#if (0)	/*******************************************************/
			if (verdir (lp->l_name) < 0)
			{
				msg ("Não consegui criar os diretórios intermediários de arquivo \"%s\"", lp->l_name);
				return;
			}
#endif	/*******************************************************/

			if (link (file_nm, lp->l_name) < 0)
			{
				msg ("*Não consegui criar o elo físico \"%s\"", lp->l_name);
				return;
			}
		}
	}

}	/* end make_local_not_regular_file */

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

#ifdef	REPORTs
	if (!pipeflag && ioctl (fileno (tty_fp), TCNREAD, 0) > 0)
		info ();
#endif	REPORTs

	/*
	 *	Prepara o ponteiro
	 */
	fp->i_ptr = fp->i_base;

	return (*fp->i_ptr++);

}	/* end receive_seg */

/*
 ****************************************************************
 *	Cria os diretórios faltando				*
 ****************************************************************
 */
int
verdir (const char *file_nm)
{
	char		*cp;

	cp = alloca (strlen (file_nm) + 1); strcpy (cp, file_nm);

	for (file_nm = cp, cp++; *cp != '\0'; cp++)
	{
	    if (*cp == '/')
	    {
		*cp = '\0';

		if (access (file_nm, F_OK) < 0 && mkdir (file_nm, 0777) < 0)
		{
			msg
			(	"*Não consegui criar o diretório \"%s\" em \"%s\"",
				file_nm, local_node_nm
			);

			*cp = '/'; return (-1);
		}

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
	if (!mflag)
		return;

	if (chmod (file_nm, sp->st_mode & ~S_IFMT) < 0)
		msg ("*Não consegui restaurar a proteção de \"%s\"", file_nm);

	if (utime (file_nm, &sp->st_atime) < 0)
		msg ("*Não consegui restaurar os tempos de \"%s\"", file_nm);

	if (chown (file_nm, sp->st_uid, sp->st_gid) < 0)
		msg ("*Não consegui restaurar o dono de \"%s\"", file_nm);

}	/* end set_mode_and_time_of_file */
