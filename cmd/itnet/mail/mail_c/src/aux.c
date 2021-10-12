/*
 ****************************************************************
 *								*
 *			aux.c					*
 *								*
 *	Funções auxiliares de cópia, remoção, ... de cartas	*
 *								*
 *	Versão	4.0.0, de 31.08.00				*
 *		4.9.0, de 23.01.07				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ustat.h>
#include <terminfo.h>
#include <time.h>
#include <errno.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry const char	quote_vec[] = "0123456789ABCDEF";

entry MDIREC		delete_head =	/* para as cartas removidas */
{
	&delete_head, &delete_head, 1 /* Cabeça */
};
 
/*
 ******	Tabela de "Content-type" ********************************
 */
typedef struct content
{
	char	t_suffix[6];	/* Sufixo */
	char	*t_type;	/* O "Content-type" */

}	CONTENT;

const CONTENT	content[] =
{
	"aif",		"audio/x-aiff",
	"aiff",		"audio/x-aiff",
	"art",		"image/x-jg",
	"au",		"audio/basic",
	"z",		"application/x-compress",
	"gif",		"image/gif",
	"gz",		"application/x-gzip",
	"htm",		"text/html",
	"html",		"text/html",
	"pcx",		"image/pcx",
	"jpg",		"image/jpeg",
	"jpeg",		"image/jpeg",
	"jpe",		"image/jpeg",
	"jfif",		"image/jpeg",
	"pjpeg",	"image/jpeg",
	"pjp",		"image/jpeg",
	"hqx",		"application/mac-binhex40",
	"xls",		"application/excel",
	"ppt",		"application/powerpoint",
	"doc",		"application/msword",
	"dot",		"application/word",
#if (0)	/*******************************************************/
	"xls",		"application/vnd.ms-excel",
	"ppt",		"application/vnd.ms-powerpoint",
	"doc",		"application/vnd.ms-msword",
	"dot",		"application/vnd.ms-word",
#endif	/*******************************************************/
	"txt",		"text/plain",
	"text",		"text/plain",
	"pdf",		"application/pdf",
	"ai",		"application/postscript",
	"eps",		"application/postscript",
	"ps",		"application/postscript",
	"mov",		"video/quicktime",
	"rtf",		"application/rtf",
	"rmi",		"audio/midi",
	"mid",		"audio/midi",
	"tar",		"application/x-tar",
	"avi",		"video/avi",
	"wrl",		"x-worlds/x-vrml",
	"wrz",		"x-worlds/x-vrml",
	"wav",		"audio/x-wav",
	"mp2",		"audio/mpeg",
	"mp3",		"audio/mpeg",
	"smi",		"application/smil",
	"rm",		"application/rn-realmedia",
	"ra",		"audio/rn-realaudio",
#if (0)	/*******************************************************/
	"rm",		"application/vnd.rn-realmedia",
	"ra",		"audio/vnd.rn-realaudio",
#endif	/*******************************************************/
	"rmm",		"audio/x-pn-realaudio",
	"ram",		"audio/x-pn-realaudio",
	"bmp",		"image/x-MS-bmp",
	"rf",		"image/rn-realflash",
	"rv",		"video/rn-realvideo",
#if (0)	/*******************************************************/
	"rf",		"image/vnd.rn-realflash",
	"rv",		"video/vnd.rn-realvideo",
#endif	/*******************************************************/
	"zip",		"application/x-zip-compressed",
	"lha",		"application/zip",
	"arj",		"application/zip",
	"arc",		"application/zip",
	"tar",		"application/zip",
	"tgz",		"application/zip",
	"gz",		"application/zip",
	"xbm",		"application/x-bitmap",
	"exe",		"application/octet-stream",
	"com",		"application/octet-stream",
	""
};

/*
 ******	Protótipos de funções ***********************************
 */
int	insert_one_attachment (const char *part_nm, FILE *fp, const char *boundary);

/*
 ****************************************************************
 *    Adiciona o conteúdo de uma parte ao final de um arquivo	*
 ****************************************************************
 */
int
append_lines (const char *file_nm, const LINE *line_head, const char *subject)
{
	FILE		*fp;
	const LINE	*lp;

	fprintf
	(	fp_out, "%s%s",
		parmexec (c_cup, LASTLINE, 0), c_rev
	);

	if ((fp = fopen (file_nm, "a")) == NOFILE)
	{
		fprintf
		(	fp_out, "\a%s: "
			"Não conseguir criar o arquivo \"%s\" (%s)",
			pgname, file_nm, strerror (errno)
		);
		goto out;
	}

	/*
	 *	Escreve o assunto
	 */
	if (subject != NOSTR)
		fprintf (fp, "Assunto: Re: %s\n", subject);

	/*
	 *	Escreve o conteúdo da parte
	 */
	for (lp = line_head->l_forw; lp->l_head == 0; lp = lp->l_forw)
		fnputs (lp->l_text, fp);

	if (ferror (fp))
	{
		fprintf
		(	fp_out, "\a%s: "
			"Erro de escrita em \"%s\" (%s)",
			pgname, file_nm, strerror (errno)
		);
		fclose (fp);
		goto out;
	}

	fclose (fp);

	/*
	 *	Confirma
	 */
	fprintf
	(	fp_out, "Texto adicionado ao arquivo \"%s\"%s",
		file_nm, c_sgr0
	);

	return (0);

	/*
	 *	Termina o campo e retorna
	 */
    out:
	fputs (c_sgr0, fp_out);
	return (-1);

}	/* end append_lines */

/*
 ****************************************************************
 *	Insere anexos após o texto de uma carta			*
 ****************************************************************
 */
const char *
insert_attachments (char *attach_list, const char **text_file_nm)
{
	static char		boundary[80];
	const char	*new_text_file_nm;
	const char	*part_nm;
	FILE		*fp_part, *fp;
	char		bad = 0;
	int		c;

	/*
	 *	Em primeiro lugar, cria uma fronteira
	 */
	sprintf (boundary, "*.*.*.*.*.* TROPIX MAIL Boundary #%02d *.*.*.*.*.*", 1);

	/*
	 *	Cria e abre o arquivo de saída
	 */
	if ((new_text_file_nm = mktemp ("/var/tmp/mail_new_text_XXXXXX")) == NOSTR)
		error ("$NÃO consegui obter o nome de um arquivo temporário");

	if ((fp = fopen (new_text_file_nm, "w+")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo temporário");

	/*
	 *	Escreve o prólogo
	 */
	fprintf
	(	fp,
		"Esta é uma carta com várias partes no formato MIME\n"
		"\n"
		"--%s\n"
		"Content-type: text/plain; charset=iso-8859-1\n"
		"Content-Transfer-Encoding: quoted-printable\n"
		"\n",
		boundary
	);

	/*
	 *	Copia o texto codificando os caracteres acima de 0x7F
	 */
	if ((fp_part = fopen (*text_file_nm, "r")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo \"%s\"", *text_file_nm);

	while ((c = getc (fp_part)) >= 0)
	{
		if (c < 0x80 && c != '=')
		{
			putc (c, fp);
		}
		else
		{
			putc ('=', fp);
			putc (quote_vec[c >> 4], fp);
			putc (quote_vec[c & 0x0F], fp);
		}
	}

	fclose (fp_part); unlink (*text_file_nm); *text_file_nm = new_text_file_nm;

	/*
	 *	Insere os anexos
	 */
	for (part_nm = strtok (attach_list, " ,\t"); part_nm != NOSTR; part_nm = strtok (NOSTR, " ,\t"))
	{
		if (insert_one_attachment (part_nm, fp, boundary) < 0)
			bad++;
	}

	/*
	 *	Coloca o epílogo
	 */
	fprintf
	(	fp,
		"--%s--\n",
		boundary
	);

	/*
	 *	Se não conseguiu anexar um arquivo, salva o texto todo
	 */
	if (bad)
		{ save_text_file (fp); quit (1); }

	fclose (fp);

	return (boundary);

}	/* end insert_attachments */

/*
 ****************************************************************
 *	Converte para ISO o assunto				*
 ****************************************************************
 */
void
convert_subject_to_ISO (char *subject, const char *local_subject, int size)
{
	const char	*bp, *end_cp;
	char		*cp;
	int		c;

	/*
	 *	Se já estiver convertido, ...
	 */
	if (strncmp (local_subject, "=?ISO", 5) == 0)
		{ strscpy (subject, local_subject, size); return; }

	/*
	 *	Realiza a conversão
	 */
	strcpy (subject, "=?ISO-8859-1?Q?");

	for (bp = local_subject, cp = subject + 15, end_cp = subject + size - 5; (c = *bp) != '\0'; bp++)
	{
		if (cp >= end_cp)
			break;

		if (c < 0x80 && c != '=')
		{
			*cp++ = *bp;
		}
		else
		{
			*cp++ = '=';
			*cp++ = quote_vec[c >> 4];
			*cp++ = quote_vec[c & 0x0F];
		}
	}

	*cp++ = '?';
	*cp++ = '=';
	*cp = '\0';

}	/* end convert_subject_to_ISO */

/*
 ****************************************************************
 *	Insere um anexo						*
 ****************************************************************
 */
int
insert_one_attachment (const char *part_nm, FILE *fp, const char *boundary)
{
	char		area[80];
	FILE		*fp_part;
	STAT		s_tmp, s_part;
	USTAT		us;
	char		eof = 0;
	const char	*content_type = "application";
	const char	*last_point;
	unsigned int	src;
	int		n, c, i, linesz;
	const char	*last_nm = last_id (part_nm);

	/*
	 *	Prólogo
	 */
	while ((fp_part = fopen (part_nm, "r")) == NOFILE)
	{
		error ("*NÃO consegui abrir o arquivo \"%s\"", part_nm);

		printf ("Tecle novamente o nome do arquivo: ");

		if (fnrgets (area, sizeof (area), stdin) == NOSTR)
			return (0);

		part_nm = area;
	}

	/*
	 *	Analisa o Tamanho
	 */
	if (fstat (fileno (fp), &s_tmp) < 0 || ustat (s_tmp.st_dev, &us) < 0)
		{ error ("*Não consegui obter o espaço temporário livre"); return (-1); }

	if (fstat (fileno (fp_part), &s_part) < 0)
		{ error ("*Não consegui obter o tamanho do arquivo \"%s\" a anexar", part_nm); return (-1); }

	if (3 * us.f_tfree < 4 * s_part.st_size / us.f_bsize)	/* Cada 3 bytes geram 4 */
	{
		error
		(	"Não consegui espaço temporário para codificar o arquivo \"%s\" de %d bytes",
			part_nm, s_part.st_size
		);
#ifdef	DEBUG
		error ("dev = %P, %d :: %d", s_tmp.st_dev, 3 * us.f_tfree, 4 * s_part.st_size / us.f_bsize);
#endif	DEBUG
		return (-1);
	}

	/*
	 *	Obtém o tipo do arquivo
	 */
	if ((last_point = strrchr (last_nm, '.')) != NOSTR)
	{
		const CONTENT	*tp;

		for (last_point++, tp = content; tp->t_suffix[0] != '\0'; tp++)
		{
			if (streq (tp->t_suffix, last_point))
				{ content_type = tp->t_type; break; }
		}
	}

	/*
	 *	Coloca o prólogo
	 */
	fprintf
	(
		fp,
		"--%s\n"
		"Content-Type: %s; name=\"%s\"\n"
		"Content-Transfer-Encoding: base64\n"
		"Content-Disposition: attachment;\n"
		"	filename=\"%s\"\n"
		"\n",
		boundary, content_type, last_nm, last_nm
	
	);

	/*
	 *	Codifica
	 */
	for (linesz = 0; /* abaixo */; /* abaixo */)
	{
		src = 0; n = 0;

		for (i = 3; i > 0; i--)
		{
			if ((c = getc (fp_part)) >= 0)
				n += 8;
			else
				{ c = 0; eof++; }

			src <<= 8; src |= c;
		}

		for (i = 24 - 6; i >= 0; i -= 6)
		{
			if (n >= 0)
				c = base_64_vec[(src >> i) & 0x3F];
			else
				c = '=';

			putc (c, fp);

			n -= 6;
		}

		if (eof)
			break;

		if (++linesz >= 72/4)
		{
			fprintf (fp, "\n");

			linesz = 0;
		}
	}

	/*
	 *	Epílogo
	 */
	fprintf (fp, "\n");

	fclose (fp_part);

	return (0);

}	/* end insert_one_attachment */

/*
 ****************************************************************
 *	Salva o arquivo em "dead.letter"			*
 ****************************************************************
 */
void
save_text_file (FILE *fp)
{
	FILE		*dead_fp;
	int		c;

	if ((dead_fp = fopen (dead_letter_nm, "a")) == NOFILE)
		{ error ("*NÃO consegui abrir o arquivo \"%s\"", dead_letter_nm); return; }

	rewind (fp);

	while ((c = getc (fp)) >= 0)
	{
		if (c == '\r')
			continue;

		putc (c, dead_fp);
	}

	fclose (dead_fp);

	error ("O texto foi salvo em \"%s\"", dead_letter_nm);

}	/* end save_text_file */

/*
 ****************************************************************
 *	Obtém o último identificador de um caminho		*
 ****************************************************************
 */
const char *
last_id (const char *path)
{
	const char	*cp;

	if ((cp = strrchr (path, '/')) != NOSTR)
		return (cp + 1);
	else
		return (path);

}	/* end last_id */

/*
 ****************************************************************
 *	Remove uma carta					*
 ****************************************************************
 */
int
rm_letter (MBOX	*cb)
{
	STAT		mailbox_s;
	MDIREC		*mp, *bp;

	mp = cb->b_cursor_mp;

	if (mp->m_nm[0] == '\0')
		{ fprintf (fp_out, "\a"); fflush (fp_out); return (-1); }

	if (rflag)
	{
		fprintf
		(	fp_out, "\a%s%sEstá no modo de leitura apenas! %s%s",
			parmexec (c_cup, LASTLINE, 0), c_rev, c_el, c_sgr0
		);

		fflush (fp_out); getc (fp_tty);
		cb->b_msg++; cb->b_next_cmd = ' '; return (-1);
	}

	fprintf
	(	fp_out, "%sRemove realmente a carta %d? (n): %s",
		parmexec (c_cup, LASTLINE, 0), mp->m_index, c_el
	);

	fflush (fp_out); /*** cb->b_msg++; ***/

	if ((cb->b_cmd = getc (fp_tty)) != 's' && cb->b_cmd != 'S' && cb->b_cmd != 'y' && cb->b_cmd != 'Y')
		{ cb->b_msg++; cb->b_next_cmd = ' '; return (-1); }

	if (instat (mailbox_dev, mailbox_ino, &mailbox_s) >= 0 && mailbox_time < mailbox_s.st_mtime)
		new_letters++;

	strcpy (mailbox_ptr, mp->m_nm);

	if (unlink (mailbox_nm) < 0)
	{
		fprintf
		(	fp_out, "%sNão consegui remover a carta %d (%s)%s",
			parmexec (c_cup, LASTLINE, 0), mp->m_index,
			strerror (errno), c_el
		);
		cb->b_msg++; return (-1);
	}

	if (instat (mailbox_dev, mailbox_ino, &mailbox_s) >= 0)
		mailbox_time = mailbox_s.st_mtime;

	mp->m_back->m_forw = mp->m_forw;
	mp->m_forw->m_back = mp->m_back;

	/*
	 *	Atualiza a tela
	 */
	if (cb->b_screen_first_line == mp)
		cb->b_screen_first_line = mp->m_forw;

	if (mp->m_forw->m_head)
		cb->b_cursor_mp = mp->m_back;
	else
		cb->b_cursor_mp = mp->m_forw;

	/*
	 *	Coloca esta carta na lista de cartas removidas
	 */
	bp = delete_head.m_back;

	delete_head.m_back = mp;  mp->m_forw = &delete_head;
	bp->m_forw = mp;	  mp->m_back = bp;

	return (0);

}	/* end rm_letter */

/*
 ****************************************************************
 *	Para consulta						*
 ****************************************************************
 */
#if (0)	/*******************************************************/
Content-Type: multipart/mixed;
	boundary="----=_NextPart_000_006C_01C01351.4D84C740"

This is a multi-part message in MIME format.

------=_NextPart_000_006C_01C01351.4D84C740
Content-Type: multipart/alternative;
	boundary="----=_NextPart_001_006D_01C01351.4D84C740"


------=_NextPart_001_006D_01C01351.4D84C740
Content-Type: text/plain;
	charset="iso-8859-1"
Content-Transfer-Encoding: quoted-printable



------=_NextPart_001_006D_01C01351.4D84C740
Content-Type: text/html;
	charset="iso-8859-1"
Content-Transfer-Encoding: quoted-printable


------=_NextPart_001_006D_01C01351.4D84C740--

------=_NextPart_000_006C_01C01351.4D84C740
Content-Type: application/msword;
	name="=?iso-8859-1?Q?-_Tabela_de_pre=E7os_31-08-2000.doc?="
Content-Transfer-Encoding: base64
Content-Disposition: attachment;
	filename="=?iso-8859-1?Q?-_Tabela_de_pre=E7os_31-08-2000.doc?="


------=_NextPart_000_006C_01C01351.4D84C740--

#endif	/*******************************************************/
