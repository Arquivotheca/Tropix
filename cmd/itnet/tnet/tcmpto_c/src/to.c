/*
 ****************************************************************
 *								*
 *			to.c					*
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
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
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
entry int	MSPEED = 800;		/* Velocidade de 9600 */

entry char	snd_rcv_buf[BLSZ];	/* Área dos blocos a transmitir */

entry const char *info_file_nm;		/* Especial para info */
entry int	info_cnt_total;

extern int	MAX_BIT_USED;		/* Número de bits realmente usados */ 

extern long	in_cnt, out_cnt;	/* Total de caracteres de entrada/saída */

/*
 ******	Protótipos de funções ***********************************
 */
extern void	print_time_interval (long, MUTM *);

/*
 ****************************************************************
 *	Copia um arquivo para o nó remoto			*
 ****************************************************************
 */
void
copy_to (const char *file_nm)
{
	off_t		n;
	int		fd = -1;
	int		seg;
	STAT		s;
#ifdef	LITTLE_ENDIAN
	STAT		big_s;
#endif	LITTLE_ENDIAN
	char		*rmsg;
	MUTM		begin_time;

	/*
	 *	Obtém o estado do arquivo
	 */
	if (lstat (file_nm, &s) < 0)
	{
		msg ("*Não consegui obter o estado do arquivo em \"%s\"", local_node_nm);
		return;
	}

	if (S_ISREG (s.st_mode) && (fd = inopen (s.st_dev, s.st_ino)) < 0)
	{
		msg ("*Não consegui abrir o arquivo em \"%s\"", local_node_nm);
		return;
	}

	/*
	 *	Transmite o nome e estado do arquivo
	 */
	t_snd_msg_no_push (CPTO, file_nm);

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
	 *	Repare o protocolo de erro de DUAS mensagens
	 */
	if (t_rcv_msg (&rmsg) < 0)
	{
		msg ("Mensagem de \"%s\":", remote_node_nm);
		msg (rmsg);
		if (t_rcv_msg (&rmsg) < 0)
			msg (rmsg);

		if (fd > 0)
			close (fd);
		return;
	}

	/*
	 *	Se for regular, envia o conteúdo
	 */
	if (!S_ISREG (s.st_mode))
		return;

	/* Avalia o tempo */

	n = s.st_size;

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
	 *	Transmite os segmentos
	 */
	if (Cflag && n >= MIN_COMPRESS_SZ)
	{
		reduce (fd, snd_rcv_buf);
	}
	else
	{
		for
		(	/* vazio */;
			(n = read (fd, snd_rcv_buf, BLSZ)) > 0;
			in_cnt += n
		)
		{
			if (t_snd (tcp_fd, snd_rcv_buf, n, 0) < 0)
				error (NOSTR);
#ifdef	REPORTs
			if (ioctl (fileno (tty_fp), TCNREAD, 0) > 0)
				info ();
#endif	REPORTs
		}	/* loop pelo segmentos */
	
		if (n < 0)
			error ("$*Erro de leitura do arquivo \"%s\"", file_nm);
	}

	t_push (tcp_fd);

	close (fd);

	/*
	 *	Mensagem final do arquivo
	 */
	if (vflag)
	{
		print_time_interval (s.st_size, &begin_time);

#if (0)	/*************************************/
		fprintf
		(	stderr,
			"\tTempo = %d:%02d, %d bytes/segundo",
			seg / 60, seg % 60,
			seg == 0 ? 0 : s.st_size / seg
		);
#endif	/*************************************/

		if (Cflag && s.st_size >= MIN_COMPRESS_SZ) fprintf
		(	stderr,
			" \t(%s %%, %d)",
			editpercent (getpercent (in_cnt - out_cnt, in_cnt)),
			MAX_BIT_USED
		);

		fprintf (stderr, "\n");
	}

}	/* end copy_to */

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
#ifdef	REPORTs
	if (ioctl (fileno (tty_fp), TCNREAD, 0) > 0)
		info ();
#endif	REPORTs

	fp->i_ptr = fp->i_base;

	*fp->i_ptr++ = c;

}	/* end send_seg */

#ifdef	REPORTs
/*
 ****************************************************************
 *	Rotina de Informação					*
 ****************************************************************
 */
void
info (void)
{
	/*
	 *	Lê um caractere do terminal e analisa 
	 */
	if   (getc (tty_fp) == ' ')
	{
		fprintf
		(	stderr,
			"Transmitindo o arquivo \"%s\", "
			"byte %d de %d (%s %%)\n",
			info_file_nm, in_cnt, info_cnt_total,
			editpercent (getpercent (in_cnt, info_cnt_total))
		);
	}

	/*
	 *	Pula os caracteres restantes
	 */
	while (ioctl (fileno (tty_fp), TCNREAD, 0) > 0)
		getc (tty_fp);

}	/* end info */
#endif	REPORTs
