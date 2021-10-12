/*
 ****************************************************************
 *								*
 *			cmd.c					*
 *								*
 *	Post Office Protocol - Version 3 (servidor)		*
 *								*
 *	Versão	3.0.0, de 15.08.98				*
 *		3.1.0, de 29.10.98				*
 *								*
 *	Módulo: POP3_S						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <errno.h>
#include <xti.h>

#include "../h/pop3_s.h"

/*
 ******	Variáveis globais ***************************************
 */
entry int	erased_total;	/* Total de cartas removidas */

/*
 ****************************************************************
 *	Comando HELP						*
 ****************************************************************
 */
void
do_help (int argc, const char *argv[])
{
	const CMDTB	*tp;

	reply
	(	"+Este servidor POP3 (%s) processa os seguintes comandos:\r\n",
		pgversion
	);

	for (tp = cmdtab; tp->c_nome != NOSTR; tp++)
	{
		if (tp->c_funcao == NOFUNC)
			continue;

		fprintf
		(	tcp_fp,
			"    %s %s\t%s\r\n",
			tp->c_nome, tp->c_args, tp->c_help
		);
	}

	fprintf (tcp_fp, ".\r\n"); fflush (tcp_fp); t_push (tcp_fd);

}	/* end do_help */

/*
 ****************************************************************
 *	Comando QUIT						*
 ****************************************************************
 */
void
do_quit (int argc, const char *argv[])
{
	const PASSWD	*pw = &user_pw;

	if (server_state == S_READY)
		delete_messages ();

	if (pw->pw_name != NOSTR)
	{
		error
		(	"Logout para \"%s@%s\"",
			pw->pw_name, client_node_nm
		);
	}

	reply ("+*Tchau");

	if (t_sndrel (tcp_fd) < 0)
		error ("*Erro no SNDREL");

	if (t_rcvrel (tcp_fd) < 0)
		error ("*Erro no RCVREL");

	t_close (tcp_fd);

	exit (0);

}	/* end do_quit */

/*
 ****************************************************************
 *	Comando STAT						*
 ****************************************************************
 */
void
do_stat (int argc, const char *argv[])
{
	const MBOX	*mp;
	long		total_size = 0;
	int		list_size = 0;

	if (check_ready_state () < 0)
		return;

	if (argc > 0)
	{
		reply ("-*Argumentos demais");
		return;
	}

	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
	{
		if (!mp->m_erased)
			{ list_size++; total_size += mp->m_size; }
	}

	reply ("+*%d %d", list_size, total_size);

}	/* end do_stat */

/*
 ****************************************************************
 *	Comando LIST						*
 ****************************************************************
 */
void
do_list (int argc, const char *argv[])
{
	const MBOX	*mp;
	const char	*str;
	int		index;
	long		total_size = 0;
	int		list_size = 0;

	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply ("-*Argumentos demais");
		return;
	}

	if (argc == 0)	  /* mostra informações de todas as mensagens */
	{
		for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		{
			if (!mp->m_erased)
				{ list_size++; total_size += mp->m_size; }
		}

		reply ("+%d %d", list_size, total_size);

		for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		{
			if (!mp->m_erased)
				fprintf (tcp_fp, "%d %d\r\n", mp->m_index, mp->m_size);
		}

		fprintf (tcp_fp, ".\r\n"); fflush (tcp_fp); t_push (tcp_fd);
	}
	else		/* Mostra informações de apenas uma mensagem */
	{
		index = strtol (argv[0], &str, 0);

		if (index < 1 || index > mbox_list_size || *str != '\0')
		{
			reply ("-*Mensagem \"%s\" inexistente", argv[0]);
			return;
		}

		mp = mbox_list_vector[index];

		if (mp->m_erased)
		{
			reply ("-*A mensagem %d foi anteriormente removida", index);
			return;
		}

		reply ("+*%d %d", mp->m_index, mp->m_size);
	}

}	/* end do_list */

/*
 ****************************************************************
 *	Comando RETR						*
 ****************************************************************
 */
void
do_retr (int argc, const char *argv[])
{
	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply ("-*Faltando o número da mensagem");
		return;
	}
	elif (argc > 1)
	{
		reply ("-*Argumentos demais");
		return;
	}

	send_letter (argv[0], -1);

}	/* end do_retr */

/*
 ****************************************************************
 *	Comando DELE						*
 ****************************************************************
 */
void
do_dele (int argc, const char *argv[])
{
	MBOX		*mp;
	int		index;
	const char	*str;

	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply ("-*Faltando o número da mensagem");
		return;
	}
	elif (argc > 1)
	{
		reply ("-*Argumentos demais");
		return;
	}

	/*
	 *	Procura a carta na lista
	 */
	index = strtol (argv[0], &str, 0);

	if (index < 1 || index > mbox_list_size || *str != '\0')
	{
		reply ("-*Mensagem \"%s\" inexistente", argv[0]);
		return;
	}

	mp = mbox_list_vector[index];

	if (mp->m_erased)
	{
		reply ("-*A mensagem %d foi anteriormente removida", index);
		return;
	}

	mp->m_erased = 1; erased_total++;

	reply ("+*Mensagem %d removida", index);

}	/* end do_dele */

/*
 ****************************************************************
 *	Comando NOOP						*
 ****************************************************************
 */
void
do_noop (int argc, const char *argv[])
{
	reply
	(	"+*Comando NOOP completado com sucesso após intensos cálculos"
	);

}	/* end do_noop */

/*
 ****************************************************************
 *	Comando RSET						*
 ****************************************************************
 */
void
do_rset (int argc, const char *argv[])
{
	MBOX		*mp;

	if (check_ready_state () < 0)
		return;

	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		mp->m_erased = 0;

	erased_total = 0;

	reply ("+*Todas as mensagens apagadas foram recuperadas");

}	/* end do_rset */

/*
 ****************************************************************
 *	Comando TOP						*
 ****************************************************************
 */
void
do_top  (int argc, const char *argv[])
{
	long		lines;
	const char	*str = "";

	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply ("-*Faltando o número da mensagem");
		return;
	}
	elif (argc > 2)
	{
		reply ("-*Argumentos demais");
		return;
	}

	if (argc == 2)
		lines = strtol (argv[1], &str, 0);
	else
		lines = 0;

	if (lines < 0 || *str != '\0')
	{
		reply ("-*Numero de linhas \"%s\" inválido", argv[1]);
		return;
	}

	send_letter (argv[0], lines);

}	/* end do_top */

/*
 ****************************************************************
 *	Comando UIDL						*
 ****************************************************************
 */
void
do_uidl (int argc, const char *argv[])
{
	const MBOX	*mp;
	const char	*str;
	int		index;

	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply ("-*Argumentos demais");
		return;
	}

	if (argc == 0)	  /* Mostra informações de todas as mensagens */
	{
		reply ("+");

		for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		{
			if (!mp->m_erased)
				fprintf (tcp_fp, "%d %s\r\n", mp->m_index, mp->m_uidl);
		}

		fprintf (tcp_fp, ".\r\n"); fflush (tcp_fp); t_push (tcp_fd);
	}
	else		/* Mostra informações de apenas uma mensagem */
	{
		index = strtol (argv[0], &str, 0);

		if (index < 1 || index > mbox_list_size || *str != '\0')
		{
			reply ("-*Mensagem \"%s\" inexistente", argv[0]);
			return;
		}

		mp = mbox_list_vector[index];

		if (mp->m_erased)
		{
			reply ("-*Mensagem %d foi anteriormente removida", index);
			return;
		}

		reply ("+*%d %s", mp->m_index, mp->m_uidl);
	}

}	/* end do_uidl */

/*
 ****************************************************************
 *	Comando LAST						*
 ****************************************************************
 */
void
do_last (int argc, const char *argv[])
{
	if (check_ready_state () < 0)
		return;

	reply ("+*%d", last_read_index);

}	/* end do_last */

/*
 ****************************************************************
 *	Apaga mensagens ao fim da sessao			*
 ****************************************************************
 */
void
delete_messages (void)
{
	FILE		*status_fp;
	const MBOX	*mp;

	/*
	 *	Verifica se foram removidas cartas
	 */
	if (erased_total == 0)
		return;

	/*
	 *	Abre o arquivo de "status"
	 */
	if ((status_fp = fopen (status_nm, "w")) == NOFILE)
		{ error ("*Não consegui abrir o arquivo \"%s\"", status_nm); return; }

	fprintf (status_fp, "R\n");

	/*
	 *	Remove os arquivos e escreve "status"
	 */
	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
	{
		if (mp->m_erased)
		{
			if (unlink (mp->m_nm) < 0)
				error ("*Não consegui remover a carta \"%s\"", mp->m_nm);
		}
		else
		{
			fprintf (status_fp, "%s:%s:%d:%d\n", mp->m_nm, mp->m_uidl, mp->m_size, mp->m_ino);
		}
	}

	fclose (status_fp);

}	/* end delete_messages */

/*
 ****************************************************************
 *	Envia um arquivo					*
 ****************************************************************
 */
void
send_letter (const char *index_str, int max_lines)
{
	const MBOX	*mp;
	FILE		*fp;
	int   		index, fd, lines = 0;
	char		area[DATASZ+2];
	const char	*str;
	STAT		s;
	char		blank_line_seen = 0;
	char		msg_begin_seen  = 0;

	/*
	 *	Procura a carta na lista
	 */
	index = strtol (index_str, &str, 0);

	if (index < 1 || index > mbox_list_size || *str != '\0')
	{
		reply ("-*Mensagem \"%s\" inexistente", index_str);
		return;
	}

	mp = mbox_list_vector[index];

	if (mp->m_erased)
	{
		reply ("-*Mensagem %d foi anteriormente removida", index);
		return;
	}

	/*
	 *	Abre o arquivo da carta e envia o tamanho
	 */
	if
	(	(fd = inopen (mailbox_dev, mp->m_ino)) < 0 ||
		(fp = fdopen (fd, "r")) == NOFILE ||
		fstat (fd, &s) < 0
	)
	{
		reply
		(	"-*Não consegui abrir o arquivo \"%s\" (%s)",
			mp->m_nm, strerror (errno)
		);
		return;
	}

	reply ("+%d bytes", s.st_size);

	/*
	 *	Envia o número de linhas pedido
	 */
	for (EVER)
	{
		if (fngets (area, DATASZ, fp) != area)
			break;

		if   (!blank_line_seen)
		{
			if (area[0] == '\0')
				blank_line_seen = 1;
		}
		elif (!msg_begin_seen)
		{
			if (area[0] == '\0')
				continue;
			else
				{ msg_begin_seen = 1; goto tst; }
		}
		else	/* No meio da carta */
		{
		    tst:
			if (max_lines >= 0 && lines++ >= max_lines)
				break;
		}

		if (area[0] == '.')
			putc ('.', tcp_fp);

		frnputs (area, tcp_fp);

	}	/* end for (EVER) */

	/*
	 *	Epílogo
	 */
	if (ferror (fp))
		error ("*Erro na leitura do arquivo \"%s\"", mp->m_nm);

	fclose (fp);

	fprintf (tcp_fp, ".\r\n"); fflush (tcp_fp); t_push (tcp_fd);

	last_read_index = index;

}	/* end send_letter */

/*
 ****************************************************************
 *	Escreve uma cadeia em um arquivo (com <cr><nl>)		*
 ****************************************************************
 */
void
frnputs (const char *str, FILE *fp)
{
	int		c;

	while ((c = *str++) != '\0')
		putc (c, fp);

	putc ('\r', fp);
	putc ('\n', fp);

}	/* end frnputs */

/*
 ****************************************************************
 *	Tabela de comandos					*
 ****************************************************************
 */
entry CMDTB	cmdtab[] =
{
    "HELP",	do_help,
		"",
		"\t\tFornece informações sobre os comandos",

    "USER",	do_user,
		"<nome do usuário>",
		"Define o nome do usuário",

    "PASS", 	do_pass,
		"<senha>",
		"\tDefine a senha do usuário",

    "QUIT",	do_quit,
		"",
		"\t\tEncerra o serviço do servidor",

    "STAT",	do_stat,
		"",
		"\t\tFornece o número de mensagens e o tamanho total",

    "LIST",	do_list,
		"[<no. da mensagem>]",
		"Fornece o tamanho de uma mensagem (ou de todas)",

    "RETR",	do_retr,
		"<no. da mensagem>",
		"Fornece o conteúdo de uma mensagem",

    "DELE",	do_dele,
		"<no. da mensagem>",
		"Remove uma mensagem",

    "NOOP",	do_noop,
		"",
		"\t\tApenas responde +OK",

    "RSET",	do_rset,
		"",
		"\t\t\"Desapaga\" todas as mensagens",

    "TOP",	do_top,
		"<no. da mensagem> [<no. de linhas>]",
		"\r\n\t\t\t\tFornece o cabeçalho e as primeiras linhas de uma mensagem",

    "UIDL",	do_uidl,
		"[<no. da mensagem>]",
		"Fornece a identificação única de uma mensagem (ou de todas)",

    "LAST",	do_last,
		"",
		"\t\tFornece o número da última mensagem processada",

    NOSTR

}	/* end cmdtab */;
