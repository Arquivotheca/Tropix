/*
 ****************************************************************
 *								*
 *			uidl.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente gráfico)	*
 *								*
 *	Versão	4.3.0, de 07.09.02				*
 *		4.4.0, de 21.12.02				*
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
#define DIR_MODE 0700

entry int		new_msg_bytes;

entry int		mail_vec_sz;
entry MBOX		**mail_vec_tb;

entry MBOX		pop3_list_head =	/* Cabeça da lista de cartas remotas */
{
	&pop3_list_head, &pop3_list_head
};

/*
 ****************************************************************
 *	Le e armazena as linhas do UIDL/LIST			*
 ****************************************************************
 */
int
get_remote_UIDLs (void)
{
	const char	*str;
	char		line[128];
	const char	*file_nm, *uidl;
	int		index, no, bad;
	MBOX		*mp, *ap;
	int		msg_no = 0, total_msg_no = 0, total_msg_bytes = 0;
	int		count, all_size_count, good_size_count;

	/*
	 ******	Dá o comando STAT *******************************
	 */
	if (comando (1, "STAT") != OK)
	{
		xerror ("O Servidor POP3 NÃO aceitou o comando STAT");
		return (-1);
	}

	/* Resposta de STAT: +OK <messages_no> <messages_bytes> */

	str = first_reply_line + OKSZ;	/* Pula o "+OK " */

	total_msg_no    = strtol (str, &str, 0); 
	total_msg_bytes = strtol (str, &str, 0); 

	xerror
	(	"A caixa postal <%s@%s> contém %d mensage%s (%s)",
		remote_user_nm, remote_node_nm, total_msg_no,
		total_msg_no == 1 ? "m": "ns", edit_size (total_msg_bytes)
	);

	/*
	 ******	Dá o comando UIDL *******************************
	 */
	if (comando (1, "UIDL") != OK)
	{
		xerror ("O servidor POP3 de \"%s\" NÃO aceitou o comando UIDL", remote_node_nm);
		return (-1);
	}

	draw_percent_percent (0, 100);

   /***	pop3_list_head.m_forw = &pop3_list_head; ***/
   /***	pop3_list_head.m_back = &pop3_list_head; ***/

	for (EVER)
	{
		/* Linha de UIDL: 1 <uidl1> */

		if (fnrgets_tcp (line, sizeof (line)) == NOSTR)
			{ xerror ("$*Erro na leitura de uma mensagem do servidor"); return (-1); }

		if (streq (line, "."))
			break;

		no = strtol (line, &str, 0);

		while (*str == ' ' || *str == '\t')
				str++;

		/*
		 *	Adiciona a mensagem à lista
		 */
		if ((mp = malloc (sizeof (MBOX))) == NOMBOX)
			error ("$Memória esgotada");

	   /***	mp->m_back	= ...;	***/
	   /***	mp->m_forw	= ...;	***/

	   /***	mp->m_index	= ...;	/* Abaixo ***/

		mp->m_no	= no;
		mp->m_uidl	= strdup (str);

		mp->m_status	= 'N';		/* Em princípio é nova */
		mp->m_rcv	= 0;		/* Trás ou não */
		mp->m_del	= 0;		/* Apaga ou não */

		mp->m_from	= NOSTR;
		mp->m_to	= NOSTR;
		mp->m_date	= NOSTR;
		mp->m_time	= -1;
		mp->m_subject	= NOSTR;
		mp->m_size	= 0;

		mp->m_text	= NOSTR;
	   /***	mp->m_len	= ...;	***/

		/*
		 *	Insere no final da lista
		 */
		ap = pop3_list_head.m_back;

		ap->m_forw = mp;
		mp->m_back = ap;

		ap = &pop3_list_head;

		mp->m_forw = ap;
		ap->m_back = mp;

		/*
		 *	Atualiza a barra de percentagem
		 */
		draw_percent_percent (++msg_no, total_msg_no);

		do_XFlush ();
	}

#ifdef	DEBUG
	/*
	 *	Imprime as cartas remotas
	 */
	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
		printf (" %d, \"%s\"\r\n", mp->m_no, mp->m_uidl);
#endif	DEBUG

	/*
	 ****** Verifica quais das cartas são novas *************
	 */
	fseek (local_status_fp, 2, SEEK_SET);

	while (fngets (line, sizeof (line), local_status_fp) != NOSTR)
	{
		bad = 0;

		if ((file_nm = strfield (line, ':')) == NOSTR)
			bad = 1;

		if ((uidl = strfield (NOSTR, ':')) == NOSTR)
			 bad = 2;

		if (bad)
			{ xerror ("Arquivo \"%s\" com erro %d", status_nm, bad); continue; }

		for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
		{
			if (streq (mp->m_uidl, uidl))	/* É antiga */
				mp->m_status  = 'O';
		}
	}

   /***	fclose (local_status_fp); ***/

#if (0)	/*******************************************************/
	int		index, no, status, bad;
	static int	wait_made;

	if (!wait_made)
	{
		if (wait (&status) < 0)
			xerror ("Erro no Processo de leitura dos UIDLs locais");

		wait_made++;
	}

	rewind (local_uidl_fp);

	while (fngets (line, sizeof (line), local_uidl_fp) == line)
	{
		for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
		{
			if (streq (mp->m_uidl, line))	/* É antiga */
				mp->m_status  = 'O';
		}
	}

   /***	fclose (local_uidl_fp); ***/

#endif	/*******************************************************/

	/*
	 ******	Conta as cartas novas (de qualquer tamanho) *****
	 */
	for (all_size_count = 0, mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		if (mp->m_status == 'N')
			all_size_count++;
	}

	if (all_size_count == 0)
		{ xerror ("NÃO há cartas novas"); return (0); }

	/*
	 ******	Dá um comando LIST para cada carta nova ***********
	 */
	draw_percent_percent (0, 100); count = 0;

	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		if (mp->m_status != 'N')
			continue;

		count++;

		if (comando (1, "LIST %d", mp->m_no) != OK)
		{
			xerror ("Não consegui executar LIST para a carta %d", mp->m_no);
			continue;
		}

		str = first_reply_line + OKSZ;	/* Pula o "+OK " */

		if ((no = strtol (str, &str, 0)) != mp->m_no) 
			xerror ("Índice não confere: %d :: %d", no, mp->m_no);

		mp->m_size = strtol (str, &str, 0); 

		/*
		 *	Verifica se o tamanho é aceitável
		 */
		if (sizem_size_in_use != 0 && mp->m_size > sizem_size_in_use)
			mp->m_status = 'B';

		/*
		 *	Atualiza a barra de percentagem
		 */
		draw_percent_percent (count, all_size_count);

		do_XFlush ();

	}	/* for (cartas novas) */

	/*
	 ******	Conta as cartas novas (de bom tamanho) **********
	 */
	for (good_size_count = 0, mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		if (mp->m_status == 'N')
			mp->m_index = good_size_count++;
	}

	if (sizem_size_in_use == 0)
	{
		if   (good_size_count == 0)
			{ xerror ("NÃO há cartas novas"); return (0); }
		elif (good_size_count == 1)
			xerror ("Há uma carta nova");
		else
			xerror ("Há %d cartas novas", good_size_count);
	}
	else
	{
		int		KB_size = sizem_size_in_use / KBSZ;

		if   (good_size_count == 0)
			{ xerror ("NÃO há cartas novas (de até %d KB)", KB_size); return (0); }
		elif (good_size_count == 1)
			xerror ("Há uma carta nova (de até %d KB)", KB_size);
		else
			xerror ("Há %d cartas novas (de até %d KB)", good_size_count, KB_size);
	}

	/*
	 ******	Cria o vetor somente com as cartas novas ********
	 */
	NMAIL = mail_vec_sz = good_size_count;

	if ((mail_vec_tb = realloc (mail_vec_tb, mail_vec_sz * sizeof (MBOX *))) == NULL)
		error ("$Memória esgotada");

	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		if (mp->m_status == 'N')
			mail_vec_tb[mp->m_index] = mp;
	}

	/*
	 ******	Dá um comando TOP para cada carta nova **********
	 */
	FIRST_LINE = 0;

	draw_percent_percent (0, 100);

	for (index = 0; index < mail_vec_sz; index++)
	{
		mp = mail_vec_tb[index];

		new_msg_bytes += mp->m_size;

		get_top (mp);		/* Dá o comando TOP */

		get_msg_info (mp);	/* Prepara a linha para a janela */

		/*
		 *	Escreve a carta e atualiza a barra de percentagem
		 */
		draw_percent_percent (index + 1, good_size_count);

		if (index < NLINE)
		{
			draw_mail_line (index);
		}
		else
		{
			update_scroll_bar (0, FIRST_LINE + 1);
		}

		do_XFlush ();

	}	/* for (cartas novas) */

	reset_scroll_button ();

	if (NMAIL == 1)
		xerror ("A carta nova contém %s", edit_size  (new_msg_bytes));
	else
		xerror ("As cartas novas contém %s", edit_size (new_msg_bytes));

	return (0);

}	/* end receive_uidl */

/*
 ****************************************************************
 *	Remove a lista de cartas remotas			*
 ****************************************************************
 */
void
free_remote_uidl_list (void)
{
	MBOX		*mp, *forw;

	chkalloc ();

	if (mail_vec_tb)
		{ free (mail_vec_tb); mail_vec_tb = NULL; }

	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = forw)
	{
		if (mp->m_uidl != NOSTR)
			free (mp->m_uidl);
		
		if (mp->m_from != NOSTR)
			free (mp->m_from);
		
		if (mp->m_to != NOSTR)
			free (mp->m_to);
		
		if (mp->m_date != NOSTR)
			free (mp->m_date);
		
		if (mp->m_subject != NOSTR)
			free (mp->m_subject);
		
		if (mp->m_text != NOSTR)
			free (mp->m_text);
		
		forw = mp->m_forw; free (mp);
	}

	pop3_list_head.m_forw = &pop3_list_head;
	pop3_list_head.m_back = &pop3_list_head;

	chkalloc ();

}	/* end free_remote_uidl_list */

/*
 ****************************************************************
 *	Obtém o cabeçalho da mensagem				*
 ****************************************************************
 */
void
get_top (MBOX *mp)
{
	char		line[DATASZ];
	char		*line_ptr;
	char		end_of_header_seen = 0;

	/*
	 *	Envia o comando TOP
	 */
	if (comando (1, "TOP %d 0", mp->m_no) != OK)
	{
		xerror
		(	"Não consegui obter o cabeçalho da mensagem %d (%s)",
			mp->m_no, mp->m_uidl
		);
		return;
	}

	/*
	 *	Le o cabeçalho
	 */
	for (EVER)
	{
		if (fnrgets_tcp (line, sizeof (line)) == NOSTR)
			error (NOSTR);

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

if (vflag)
	printf ("%s\n", line_ptr);

		if (!end_of_header_seen)
			get_line_info (line_ptr, mp);
	}

}	/* get_top */

/*
 ****************************************************************
 *	Edita um tamanho de arquivo				*
 ****************************************************************
 */
const char *
edit_size (int size)
{
	static char	size_str[16];
	char		*cp = size_str;

	if   (size < 1 * KBSZ)
		sprintf (cp, "%4d  B", size);
	elif (size < 1024 * KBSZ)
		sprintf (cp, "%4d KB", (size + KBSZ  - 1) / KBSZ);
	else
		sprintf (cp, "%4d MB", (size + MBSZ  - 1) / MBSZ);

	while (*cp == ' ')
		cp++;

	return (cp);

}	/* end edit_size */
