/*
 ****************************************************************
 *								*
 *			letter.c				*
 *								*
 *	Lê e edita o conteúdo de uma carta			*
 *								*
 *	Versão	3.2.2, de 12.08.99				*
 *		4.3.0, de 12.07.02				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
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
#include <fcntl.h>
#include <errno.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */

/*
 ****** Protótipos de funções ***********************************
 */
int		creat_part_tree (PART *, PART *);
int		get_part_info (PART *);

/*
 ****************************************************************
 *	Lê uma carta						*
 ****************************************************************
 */
int
read_letter (MDIREC *letter_mp)
{
	int		fd_read, fd_write;
	FILE		*letter_fp;
	PART		*part_head;
	LINE		*line_head;
	LINE		prolog_head;
	char		area[DATASZ];
	LINE		*lp, *bp;
	int		len, code;
	void		*mem_alloc_status;

	/*
	 *	Salva o estado do alocador de memória
	 */
	mem_alloc_status = mem_alloc (0);

	/*
	 *	Inicializa a lista de linhas
	 */
	part_head = &letter_mp->m_part_head;
	line_head = &letter_mp->m_part_head.p_line_head;

	line_head->l_forw = line_head;
	line_head->l_back = line_head;
	line_head->l_head = 1;

	/*
	 *	Abre o arquivo da carta
	 */
	if
	(	(fd_read = inopen (mailbox_dev, letter_mp->m_ino)) < 0 ||
		(letter_fp = fdopen (fd_read, "r")) == NOFILE
	)
	{
		error ("*NÃO consegui abrir a carta \"%s\"", letter_mp->m_nm);
		getc (fp_tty);

	   /***	mem_free (mem_alloc_status); ***/

		return (-1);
	}

	fseek (letter_fp, letter_mp->m_offset, SEEK_SET);

	/*
	 *	Lê o conteúdo da carta
	 */
	for (EVER)
	{
		if (fnrgets (area, DATASZ, letter_fp) == NOSTR)
			break;

		lp = mem_alloc (sizeof (LINE)); lp->l_head = 0;

		len = strlen (area);
		lp->l_text = mem_alloc (len + 1);
		lp->l_len  = len;
		strcpy (lp->l_text, area);

		/*
		 *	Atualiza a lista duplamente encadeada
		 */
		bp = line_head->l_back;

		bp->l_forw = lp;
		lp->l_back = bp;

		lp->l_forw = line_head;
		line_head->l_back = lp;

	}	/* end for (EVER) */

	fclose (letter_fp);

	/*
	 *	Atualiza o estado da carta
	 */
	if (!rflag && letter_mp->m_status == 'U')
	{
		strcpy (mailbox_ptr, letter_mp->m_nm);

		if ((fd_write = open (mailbox_nm, O_WRONLY)) < 0)
		{
			if (errno != EROFS)
			{
				error ("*Não consegui abrir \"%s\" para atualizar o STATUS", letter_mp->m_nm);
				getc (fp_tty);
			}
		}
		else
		{
			lseek (fd_write, 8, SEEK_SET);

			if (write (fd_write, "R", 1) != 1)
			{
				error ("*Não consegui atualizar o STATUS de \"%s\"", letter_mp->m_nm);
				getc (fp_tty);
			}

			close (fd_write);

			letter_mp->m_status  = 'R';
			letter_mp->m_text[0] = 'R';
		}
	}

	/*
	 *	Verifica se a carta contém partes
	 */
	if (letter_mp->m_part_head.p_boundary != NOSTR)
	{
		code = part_prolog (letter_mp);

		mem_free (mem_alloc_status);

		return (code);
	}

	/*
	 *	Cria o prólogo da carta
	 */
	prolog_head.l_forw  = &prolog_head;
	prolog_head.l_back  = &prolog_head;
	prolog_head.l_head  = 1;

	text_line_insert (&prolog_head, "Carta: %d     (%s)", letter_mp->m_index, letter_mp->m_nm);
	text_line_insert (&prolog_head, "From: %s", letter_mp->m_from_text);
	text_line_insert (&prolog_head, "To: %s", letter_mp->m_to_text);

	if (letter_mp->m_cc_text[0] != '\0')
		text_line_insert (&prolog_head, "Cc: %s", letter_mp->m_cc_text);

	text_line_insert (&prolog_head, "Date: %s", letter_mp->m_date);
	text_line_insert (&prolog_head, "Subject: %s", letter_mp->m_subject);
	text_line_insert (&prolog_head, "");

	/*
	 *	Insere o prólogo de linhas
	 */
	lp = prolog_head.l_forw;
	bp = letter_mp->m_part_head.p_line_head.l_forw;

	letter_mp->m_part_head.p_line_head.l_forw = lp;
	lp->l_back = &letter_mp->m_part_head.p_line_head;

	lp = prolog_head.l_back;

	lp->l_forw = bp;
	bp->l_back = lp;

	/*
	 *	Mostra a carta
	 */
	convert_line_text (&letter_mp->m_part_head);

	code = show_line (letter_mp, &letter_mp->m_part_head.p_line_head);

	mem_free (mem_alloc_status);

	return (code);

}	/* end read_letter */

/*
 ****************************************************************
 *	Inicia uma parte					*
 ****************************************************************
 */
int
part_prolog (MDIREC *letter_mp)
{
	PART		prolog_head;

	/*
	 *	Cria o prólogo da carta
	 */
	prolog_head.p_forw  = &prolog_head;
	prolog_head.p_back  = &prolog_head;
	prolog_head.p_head  = 1;

	text_part_insert (&prolog_head, "Carta: %d     (%s)", letter_mp->m_index, letter_mp->m_nm);
	text_part_insert (&prolog_head, "From: %s", letter_mp->m_from_text);
	text_part_insert (&prolog_head, "To: %s", letter_mp->m_to_text);

	if (letter_mp->m_cc_text[0] != '\0')
		text_part_insert (&prolog_head, "Cc: %s", letter_mp->m_cc_text);

	text_part_insert (&prolog_head, "Date: %s", letter_mp->m_date);
	text_part_insert (&prolog_head, "Subject: %s", letter_mp->m_subject);
	text_part_insert (&prolog_head, "");

	text_part_insert (&prolog_head, "Esta carta contém várias partes no formato MIME:");
	text_part_insert (&prolog_head, "");

	/*
	 *	Cria a árvore de partes e mostra
	 */
	creat_part_tree (&letter_mp->m_part_head, &prolog_head);

	return (show_part (letter_mp, &letter_mp->m_part_head));

}	/* end part_prolog */

/*
 ****************************************************************
 *	Decodifica uma parte					*
 ****************************************************************
 */
int
creat_part_tree (PART *part_pp, PART *prolog_head)
{
	char		new_part = 0;
	const char	*boundary_str;
	int		boundary_len, part_index = 0;
	LINE		*lp;
	PART		*part_head, *pp = NOPART, *bp;

	/*
	 *	Obtém a cabeça da lista de partes
	 */
	part_head = mem_alloc (sizeof (PART));
	memsetl (part_head, 0, sizeof (PART) / sizeof (long));

	part_pp->p_part_head = part_head;

	part_head->p_forw = part_head;
	part_head->p_back = part_head;
	part_head->p_head = 1;

	/*
	 *	Insere o prólogo
	 */
	if (prolog_head != NOPART)
	{
		bp = prolog_head->p_forw;
		part_head->p_forw = bp; bp->p_back = part_head;

		bp = prolog_head->p_back;
		part_head->p_back = bp; bp->p_forw = part_head;

		convert_part_text (part_head);
	}

	/*
	 *	Pequena inicialização
	 */
	boundary_str = part_pp->p_boundary;
	boundary_len = strlen (boundary_str);

	/*
	 *	Desmembra as partes deste nível
	 */
	for (lp = part_pp->p_line_head.l_forw; lp->l_head == 0; lp = lp->l_forw)
	{
		/*
		 *	Verifica se tem a cadeia "mágica"
		 */
		if
		(	lp->l_text[0] == '-' && lp->l_text[1] == '-' &&
			!strncmp (strtoupper (&lp->l_text[2]), boundary_str, boundary_len)
		)
		{
			if   (lp->l_text[boundary_len + 2] == '\0')
			{
				part_index++; new_part++;

				/* Retira a linha da lista do arquivo */

				lp->l_back->l_forw = lp->l_forw;
				lp->l_forw->l_back = lp->l_back;

				lp = lp->l_back;
				continue;
			}
			elif (streq (&lp->l_text[boundary_len + 2], "--"))
			{
				break;
			}
		}

		/*
		 *	Verifica se é o que está antes da 1a. parte
		 */
		if (part_index == 0)
		{
			/* Insere o texto no cabeçalho */

			if (strchr (lp->l_text, '%') == NOSTR)
				text_part_insert (part_head, lp->l_text);

			/* Retira a linha da lista do arquivo */

			lp->l_back->l_forw = lp->l_forw;
			lp->l_forw->l_back = lp->l_back;

			/*** ... = lp; ***/ lp = lp->l_back;

			continue;
		}

		/*
		 *	Verifica se é início de uma parte
		 */
		if (!new_part)
			continue;

		/*
		 *	Termina a parte anterior
		 */
		if (pp != NOPART)
		{
			pp->p_line_head.l_back = lp->l_back;
			lp->l_back->l_forw = &pp->p_line_head;
		}

		/*
		 *	Cria a cabeça da PARTE
		 */
		pp = mem_alloc (sizeof (PART));
		memsetl (pp, 0, sizeof (PART) / sizeof (long));

		pp->p_index = part_index;

		bp = part_head->p_back;

		bp->p_forw = pp;
		pp->p_back = bp;

		pp->p_forw = part_head;
		part_head->p_back = pp;

		/*
		 *	Prepara o início das lista de linhas
		 */
		pp->p_line_head.l_head = 1;

		pp->p_line_head.l_forw = lp;
		lp->l_back = &pp->p_line_head;

		new_part = 0;

	}	/* end for (linhas do arquivo da carta) */

	/*
	 *	Termina a última parte
	 */
	if (pp != NOPART)
	{
		pp->p_line_head.l_back = lp->l_back;
		lp->l_back->l_forw = &pp->p_line_head;
	}

	/*
	 *	Agora cria as partes dos filhos, recursivamente
	 */
	for (pp = part_head->p_forw; pp->p_head == 0; pp = pp->p_forw)
	{
		if (pp->p_line_head.l_forw == NOLINE)	/* PROVISÓRIO */
			continue;

		get_part_info (pp);

		if (pp->p_boundary != NOSTR)
			creat_part_tree (pp, NOPART);
		else
			convert_line_text (pp);
	}

	return (0);

}	/* end creat_part_tree */

/*
 ****************************************************************
 *	Decodifica uma parte					*
 ****************************************************************
 */
int
get_part_info (PART *part_pp)
{
	LINE		*lp;
	const char	*cp;
	char		*key_word;
	char		area[DATASZ];
	char		name[80];
	char		type[80];
	char		token[80];
	char		only_once = 0;

	type[0] = '\0';

	/*
	 *	Percorre as linhas iniciais da parte
	 */
	for (lp = part_pp->p_line_head.l_forw; lp->l_head == 0; /* abaixo */)
	{
		lp = get_mime_line (area, DATASZ, lp);

		if (area[0] == '\0')
		{
			if (!part_pp->p_rfc822)
				break;

			if (only_once++ > 0)
				break;
		}

                cp = get_keyword (cp = area, token, sizeof (token));

                if (token[0] == '0')
                        continue;

                cp = skip_blanks (cp);

                if (*cp++ != ':')
                        continue;

                strtoupper (key_word = token);

                cp = skip_blanks (cp);

                /* Identifica a palavra chave */

                if   (streq (key_word, "CONTENT-TYPE"))
                {
	                cp = get_atom (cp, token, sizeof (token));

	                if (token[0] != '\0' && type[0] == '\0')
				strcat (type, token);

#if (0)	/*******************************************************/
	                if (token[0] != '\0')
			{
				if (type[0] != '\0')
					strcat (type, ", ");

				strcat (type, token);
			}
#endif	/*******************************************************/

	                if   (!strncmp (strtoupper (token), "TEXT", 4))
				part_pp->p_is_text++;
	                elif (streq (token, "MESSAGE/RFC822"))
				part_pp->p_rfc822++;

			for (EVER)
			{
		                cp = skip_to_sm (cp);

				if (*cp++ != ';')
					break;

		                cp = get_keyword (cp, token, sizeof (token));

		                strtoupper (token);

		                if   (streq (token, "NAME") && *cp == '=')
				{
			                cp = get_value (++cp, name, sizeof (name));

					part_pp->p_name = alloc_and_copy (name);
				}
				elif (streq (token, "BOUNDARY") && *cp == '=')
				{
					cp = get_value (++cp, name, sizeof (name));

					part_pp->p_boundary = alloc_and_copy (strtoupper (name));
				}

			}
		}
		elif (streq (key_word, "CONTENT-TRANSFER-ENCODING"))
		{
	                cp = get_atom (cp, token, sizeof (token));

	                strtoupper (token);

	                if   (streq (token, "QUOTED-PRINTABLE"))
	                {
				part_pp->p_quoted++;
			}
	                elif (streq (token, "BASE64"))
	                {
				part_pp->p_base64++; part_pp->p_size = get_base_64_sz (lp);
			}
		}
		elif (streq (key_word, "CONTENT-DISPOSITION"))
		{
	                cp = get_atom (cp, token, sizeof (token));

			for (EVER)
			{
		                cp = skip_to_sm (cp);

				if (*cp++ != ';')
					break;

		                cp = get_keyword (cp, token, sizeof (token));

		                strtoupper (token);

		                if   (streq (token, "FILENAME") && *cp == '=')
				{
			                cp = get_value (++cp, name, sizeof (name));

					part_pp->p_name = alloc_and_copy (name);
				}
			}
		}

	}	/* end for (linhas da parte) */

	/*
	 *	Epílogo
	 */
	sprintf (area, "Parte No. %d", part_pp->p_index);

	if (type[0] != '\0')
	{
		strcat (area, ", ");
		strcat (area, type);
	}

	if (part_pp->p_name != NOSTR)
	{
		strcat (area, ", name=\"");
		strcat (area, part_pp->p_name);
		strcat (area, "\"");
	}

	if (part_pp->p_quoted)
		strcat (area, ", quoted");

	if (part_pp->p_base64)
	{
		sprintf (token, ", base64, size=%d", part_pp->p_size);
		strcat  (area, token);
	}

	part_pp->p_text = alloc_and_copy (area);
	part_pp->p_len = strlen (area);

	if (part_pp->p_len > LINE_SZ)
	{
		part_pp->p_text[LINE_SZ] = '\0';
		part_pp->p_len = LINE_SZ;
	}

	return (0);

}	/* get_part_info */

/*
 ****************************************************************
 *	Converte adequadamente as linhas			*
 ****************************************************************
 */
void
convert_line_text (PART *part_head)
{
	LINE		line_head;
	LINE		*ip, *lp, *bp;
	char		*area_ptr;
	char		*area = NOSTR;
	int		area_len = 0;
	int		len, index;

	/*
	 *	Verifica se deve converter as seqüências "=XX"
	 */
	if (part_head->p_quoted)
	{
		/* Prepara a nova lista "line" */

		line_head.l_forw  = &line_head;
		line_head.l_back  = &line_head;
		line_head.l_head  = 1;

		/* Converte as seqüências "=XX" */

		for (ip = part_head->p_line_head.l_forw; ip->l_head == 0; ip = ip->l_forw)
		{
			area_ptr = area;

			for (EVER)
			{
				if   (area_ptr == NOSTR)
				{
					area_len = ip->l_len + DATASZ;
					area_ptr = area = alloca (area_len);
				}
				elif (area_len < (len = area_ptr - area) + ip->l_len)
				{
					area_len += (ip->l_len + DATASZ);
					area_ptr = alloca (area_len);
					strcpy (area_ptr, area);
					area = area_ptr; area_ptr += len;
				}

				if ((area_ptr = convert_quoted (ip->l_text, area_ptr)) == NOSTR)
					break;

				if (ip->l_forw->l_head)
					break;

				ip = ip->l_forw;
			}

			len = strlen (area); area_ptr = mem_alloc (len + 1); strcpy (area_ptr, area);

			lp = mem_alloc (sizeof (LINE)); lp->l_head = 0;

			lp->l_text = area_ptr;
			lp->l_len  = len;

			/*
			 *	Atualiza a lista duplamente encadeada
			 */
			bp = line_head.l_back;

			bp->l_forw = lp;
			lp->l_back = bp;

			lp->l_forw = &line_head;
			line_head.l_back = lp;

		}	/* end for (percorrendo as linhas do arquivo) */

		/*
		 *	Substitui a lista pela nova
		 */
		part_head->p_line_head.l_forw = line_head.l_forw;
		part_head->p_line_head.l_back = line_head.l_back;

	}	/* end if (file_head->m_quoted) */

	/*
	 *	Verifica se há linhas muito compridas
	 */
	for (lp = part_head->p_line_head.l_forw; lp->l_head == 0; /* abaixo */)
	{
		char		*cp;

		if (lp->l_len <= LINE_SZ)
			{ lp = lp->l_forw; continue; }

		/* É uma linha comprida: tenta achar o final de uma palavra */

		for (cp = lp->l_text + LINE_SZ - 1; /* abaixo */; cp--)
		{
			if (cp < lp->l_text + 5)
				{ len = LINE_SZ; break; }

			if (*cp != ' ' && *cp != '\t')
				continue;

			len = cp - lp->l_text;

			break;
		}

		ip = mem_alloc (sizeof (LINE)); ip->l_head = 0;

		ip->l_text = mem_alloc (len + 1);
		ip->l_len  = len;

		strncpy (ip->l_text, lp->l_text, len); ip->l_text[len] = '\0';

		if (len <= LINE_SZ)
			len++;

		lp->l_text += len; lp->l_len -= len;

		/*
		 *	Insere a nova linha antes da examinada
		 */
		bp = lp->l_back;

		bp->l_forw = ip;
		ip->l_back = bp;

		ip->l_forw = lp;
		lp->l_back = ip;

		/* Examina "lp" novamente */

	}	/* end for (lista "line") */

	/*
	 *	Numera as linhas
	 */
	for (lp = part_head->p_line_head.l_forw, index = 1; lp->l_head == 0; lp = lp->l_forw, index++)
		lp->l_index = index;

}	/* end convert_line_text */

/*
 ****************************************************************
 *	Desmembra linhas compridas do cabeçalho das partes	*
 ****************************************************************
 */
void
convert_part_text (PART *part_head)
{
	PART		*pp, *ip, *bp;

	/*
	 *	Verifica se há linhas muito compridas
	 */
	for (pp = part_head->p_forw; pp->p_head == 0; /* abaixo */)
	{
		if (pp->p_len <= LINE_SZ)
			{ pp = pp->p_forw; continue; }

		/* É uma linha comprida! */

		ip = mem_alloc (sizeof (PART)); ip->p_head = 0;
		memsetl (ip, 0, sizeof (PART) / sizeof (long));

		ip->p_text = mem_alloc (LINE_SZ + 1);
		ip->p_len  = LINE_SZ;

		strncpy (ip->p_text, pp->p_text, LINE_SZ); ip->p_text[LINE_SZ] = '\0';

		pp->p_text += LINE_SZ; pp->p_len -= LINE_SZ;

		/*
		 *	Insere a nova linha antes da examinada
		 */
		bp = pp->p_back;

		bp->p_forw = ip;
		ip->p_back = bp;

		ip->p_forw = pp;
		pp->p_back = ip;

		/* Examina "pp" novamente */

	}	/* end for (lista "line") */

}	/* end convert_part_text */

/*
 ****************************************************************
 *  Converte caracteres estilo "mail" nos correspondentes "iso"	*
 ****************************************************************
 */
char *
convert_quoted (const char *in_area, char *out_area)
{
	const char	*sp = in_area;
	char		*dp = out_area;

	/*
	 *	Retorna NULO se acabou a linha, e um ponteiro
	 *	para onde deve continuar, se houver continuação
	 */
	for (EVER)
	{
		int		c, d, val;

		if ((c = *sp++) == '\0')
			{ *dp = '\0'; return (NOSTR); }

		if (c != '=')
			{ *dp++ = c; continue; }

		/* Achou um  "=..." */

		if ((c = *sp++) == '\0')
			{ *dp = '\0'; return (dp); }

		if   (c >= '0' && c <= '9')
			val = c - '0';
		elif (c >= 'A' && c <= 'F')
			val = c - 'A' + 10;
		else
			{ *dp++ = '='; *dp++ = c; continue; }

		val <<= 4;

		if ((d = *sp++) == '\0')
			{ *dp++ = '='; *dp++ = c; *dp = '\0'; return (NOSTR); }

		if   (d >= '0' && d <= '9')
			val += d - '0';
		elif (d >= 'A' && d <= 'F')
			val += d - 'A' + 10;
		else
			{ *dp++ = '='; *dp++ = c; *dp++ = d; continue; }

		if (val != '\r')	/* Cuidado com o <cr> */
			*dp++ = val;
	}

}	/* end convert_quoted */

/*
 ****************************************************************
 *	Insere uma linha de texto em uma lista de linhas	*
 ****************************************************************
 */
void
text_line_insert (LINE *line_head, const char *format, ...)
{
	va_list		args;
	LINE		*lp, *bp;
	char		*cp;
	int		len;
	char		area[DATASZ];

	/*
	 *	Edita e copia a linha
	 */
	va_start (args, format);

	vsprintf (area, format, args);

	len = strlen (area); cp = mem_alloc (len + 1); strcpy (cp, area);

	/*
	 *	Cria o novo nó
	 */
	lp = mem_alloc (sizeof (LINE));

	lp->l_head = 0;
	lp->l_text = cp;
	lp->l_len  = len;

	/*
	 *	Insere no final da lista duplamente encadeada
	 */
	bp = line_head->l_back;

	bp->l_forw = lp;
	lp->l_back = bp;

	lp->l_forw = line_head;
	line_head->l_back = lp;

	va_end (args);

}	/* end text_line_insert */

/*
 ****************************************************************
 *	Insere uma linha de texto em uma lista de partes	*
 ****************************************************************
 */
void
text_part_insert (PART *part_head, const char *format, ...)
{
	va_list		args;
	PART		*pp, *bp;
	char		*cp;
	int		len;
	char		area[DATASZ];

	/*
	 *	Edita e copia a linha
	 */
	va_start (args, format);

	vsprintf (area, format, args);

	len = strlen (area); cp = mem_alloc (len + 1); strcpy (cp, area);

	/*
	 *	Cria o novo nó
	 */
	pp = mem_alloc (sizeof (PART));
	memsetl (pp, 0, sizeof (PART) / sizeof (long));

	pp->p_head = 0;
	pp->p_text = cp;
	pp->p_len  = len;

	/*
	 *	Insere no final da lista duplamente encadeada
	 */
	bp = part_head->p_back;

	bp->p_forw = pp;
	pp->p_back = bp;

	pp->p_forw = part_head;
	part_head->p_back = pp;

	va_end (args);

}	/* end text_part_insert */
