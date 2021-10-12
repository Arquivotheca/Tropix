/*
 ****************************************************************
 *								*
 *			login.c					*
 *								*
 *	Post Office Protocol - Version 3 (cliente)		*
 *								*
 *	Versão	3.1.0, de 09.10.98				*
 *		4.0.0, de 14.12.00				*
 *								*
 *	Módulo: POP3_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
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

#include "../h/pop3_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry	char		first_reply_line[128];

entry int		total_msg_no;		/* No. de mensagens */
entry int		total_msg_bytes;	/* Tamanho total das mensagens */

entry int		new_msg_no;		/* No. de mensagens */
entry int		new_msg_bytes;		/* Tamanho total das mensagens */

entry MBOX		pop3_list_head;		/* Cabeça da lista de mensagens */

entry FILE		*uidl_fp;		/* Arquivo dos UIDLs lidos */

#define REG_MODE	0600

/*
 ****************************************************************
 *	Processa o "login"					*
 ****************************************************************
 */
void
do_login (void)
{
	const char	*pass;

	/*
	 *	Executa o comando "USER"
	 */
    user:
	if (comando (1, "USER %s", remote_user_nm) != OK)
	{
		error
		(	"Usuário \"%s\" desconhecido pelo servidor POP3 \"%s\"",
			remote_user_nm, remote_node_nm
		);
		do_quit_and_exit ();
	}

#if (0)	/*******************************************************/
	/*
	 *	Tenta dar um STAT mesmo sem ter dado a senha
	 */
	if (comando (1, "STAT") == OK)
		{ decode_stat (); return; }
#endif	/*******************************************************/

	/*
	 *	É necessária a senha
	 */
	if ((pass = getpass ("Senha: ")) == NOSTR || pass[0] == '\0' || received_intr)
		do_quit_and_exit ();

	if (comando (1, "PASS %s", pass) != OK)
	{
		printf ("Senha inválida\n");
		goto user;
	}

	/*
	 *	Dá outro STAT
	 */
	if (comando (1, "STAT") == OK)
		{ decode_stat (); return; }

	/*
	 *	Não respondeu corretamente o STAT
	 */
	error ("O Servidor POP3 NÃO aceitou o comando STAT");

	do_quit_and_exit ();

}	/* end do_login */

/*
 ****************************************************************
 *	Interpreta a linha do STAT				*
 ****************************************************************
 */
void
decode_stat (void)
{
	const char	*str;

	/*
	 *	Resposta de STAT: +OK <messages_no> <messages_bytes>
	 */
	str = first_reply_line + OKSZ;	/* Pula o "+OK " */

	total_msg_no    = strtol (str, &str, 0); 
	total_msg_bytes = strtol (str, &str, 0); 

	printf
	(	"A caixa postal <%s@%s> contém %d mensagens (%s)\n",
		remote_user_nm, remote_node_nm, total_msg_no, edit_size (total_msg_bytes)
	);

}	/* end decode_stat */

/*
 ****************************************************************
 *	Le e armazena as linhas do UIDL/LIST			*
 ****************************************************************
 */
int
read_and_store_uidl_or_list (void)
{
	char		line[128];
	long		pid;
	int		uidl;

	/*
	 *	Cria um arquivo temporário
	 */
	if ((uidl_fp = tmpfile ()) == NOFILE)
		error ("$*Não consegui criar um arquivo temporário para os UIDLs");

	/*
	 *	Cria um filho para ler os UIDLs/LISTs
	 */
	if ((pid = fork ()) < 0)
		error ("$*Não consegui criar um processo filho");

	if (pid > 0)
	{
		tcp_fp->_bend = tcp_fp->_base;	/* Esvazia a fila do pai */
		return (pid);
	}

	/*
	 *	Realiza o "login"
	 */
	do_login ();

	/*
	 *	Tenta dar o comando UIDL, se não conseguiu, usa o LIST
	 */
	if   (comando (1, "UIDL") == OK)
	{
		uidl = 1;
	}
	elif (comando (1, "LIST") == OK)
	{
		uidl = 0;
		error ("O servidor POP3 de \"%s\" NÃO aceitou o comando UIDL", remote_node_nm);
	}
	else
	{
		uidl = -1;
		error ("O servidor POP3 de \"%s\" NÃO aceitou o comando UIDL nem LIST", remote_node_nm);
	}

	fprintf (uidl_fp, "%d\n", uidl);

	if (uidl < 0)
		exit (0);

	/*
	 *	Resposta de UIDL: +OK <messages_no> <messages_bytes>
	 *
	 *	Resposta de LIST: +OK <messages_no> <messages_bytes>
	 *
	 *		+OK
	 *		1 ...........
	 *		2 ...........
	 *		.
	 */
	for (EVER)
	{
		if (fnrgets (line, sizeof (line), tcp_fp) == NOSTR)
			error ("$*Erro na leitura de uma mensagem do servidor");

		if (vflag)
			puts (line);

		if (streq (line, "."))
			break;

		fnputs (line, uidl_fp);
	}

	exit (0);

	return (0);	/* To make LINT happy */

}	/* end read_and_store_uidl_or_list */

/*
 ****************************************************************
 *	Interpreta as linhas do UIDL				*
 ****************************************************************
 */
void
decode_uidl (void)
{
	char		line[128];
	const char	*str;
	int		/* n = 0, */ index;
	MBOX		*mp, *ap;

	/*
	 *	Inicializa a lista de mensagens do pop3
	 */
	pop3_list_head.m_forw = &pop3_list_head;
	pop3_list_head.m_back = &pop3_list_head;

	/*
	 *	Resposta de UIDL: +OK <messages_no> <messages_bytes>
	 *
	 *		+OK
	 *		1 ...........
	 *		2 ...........
	 *		.
	 */
	for (EVER)
	{
		if (fngets (line, sizeof (line), uidl_fp) == NOSTR)
			break;

		if (vflag)
			puts (line);

		if (streq (line, "."))
			break;

		index = strtol (line, &str, 0);

		while (*str == ' ' || *str == '\t')
				str++;

		if (vflag)
			printf ("Mensagem %3d: %s\n", index, str);

		/*
		 *	Adiciona a mensagem na lista
		 */
		if ((mp = malloc (sizeof (MBOX))) == NOMBOX)
			error ("$Memória esgotada");

		/*
		 *	Preenche alguns dos campos
		 */
		mp->m_nm      = "";
#if (0)	/*******************************************************/
		mp->m_nm[0]   = '\0';
#endif	/*******************************************************/
		mp->m_ino     = 0;

	   	mp->m_index   = index;

	   	mp->m_status  = 'N';	/* Em princípio é nova */
	   	mp->m_quoted  = 0;

	   	mp->m_uidl    = strdup (str);
	   	mp->m_from    = "";
	   	mp->m_to      = "";
	   	mp->m_date    = "";
	   	mp->m_time    = -1;
	   	mp->m_subject = "";
	   	mp->m_size    = 0;	/* Não sei ainda ***/

	   	mp->m_text    = "";
	   	mp->m_len     = 0;

	   	mp->m_local   = NOMBOX;
	   /***	mp->m_back    = ...;
	   /***	mp->m_forw    = ...;

		/*
		 *	Insere no final da lista
		 */
		ap = pop3_list_head.m_back;

		ap->m_forw = mp;
		mp->m_back = ap;

		ap = &pop3_list_head;

		mp->m_forw = ap;
		ap->m_back = mp;

#if (0)	/*******************************************************/
		n++;
#endif	/*******************************************************/
	}

	fclose (uidl_fp);

#ifdef	DEBUG
	/*
	 *	Imprime as cartas remotas
	 */
	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
		printf (" %d, \"%s\"\r\n", mp->m_index, mp->m_uidl);

	/*
	 *	Conta cada uma das filas
	 */
	{
		int	remote = 0, local = 0;

		for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
			remote++;

		for (ap = mbox_list_head.m_forw; ap != &mbox_list_head; ap = ap->m_forw)
			local++;

		printf ("Fila remota = %d, fila local = %d\n", remote, local);
	}
#endif	DEBUG

	/*
	 *	Verifica quais são cartas novas
	 */
	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		for (ap = mbox_list_head.m_forw; /* abaixo */; ap = ap->m_forw)
		{
			if (ap == &mbox_list_head)
				break;

			if (streq (ap->m_uidl, mp->m_uidl) || streq (ap->m_nm, mp->m_uidl))
			{
			   	mp->m_status  = 'O';	/* É antiga */
			   	mp->m_local   = ap;
				break;
			}
		}
	}

	/*
	 *	Dá um comando LIST para cada carta nova
	 */
	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
	   	if (mp->m_status != 'N')
			continue;

		if (comando (1, "LIST %d", mp->m_index) != OK)
		{
			error ("Não consegui executar LIST para a carta %d\n", mp->m_index);
			continue;
		}

		str = first_reply_line + OKSZ;	/* Pula o "+OK " */

		if ((index = strtol (str, &str, 0)) != mp->m_index) 
			error ("Índice não confere: %d :: %d\n", index, mp->m_index);

		mp->m_size = strtol (str, &str, 0); 

		new_msg_no++;
		new_msg_bytes += mp->m_size;
	}

}	/* end decode_uidl */

/*
 ****************************************************************
 *	Interpreta as linhas do LIST				*
 ****************************************************************
 */
void
decode_list (void)
{
	char		line[128];
	const char	*str;
	int		n = 0, index, size;
	MBOX		*mp, *ap;

	/*
	 *	Inicializa a lista de mensagens do pop3
	 */
	pop3_list_head.m_forw = &pop3_list_head;
	pop3_list_head.m_back = &pop3_list_head;

	/*
	 *	Resposta de LIST: +OK <messages_no> <messages_bytes>
	 *
	 *		+OK
	 *		1 ...........
	 *		2 ...........
	 *		.
	 */
	for (EVER)
	{
		if (fngets (line, sizeof (line), uidl_fp) == NOSTR)
			break;

		if (vflag)
			puts (line);

		if (streq (line, "."))
			break;

		index = strtol (line, &str, 0);
		size  = strtol (str, &str, 0);

		if (vflag)
			printf ("Mensagem %3d: %d\n", index, size);

		new_msg_no++;
		new_msg_bytes += size;

		/*
		 *	Adiciona a mensagem na lista
		 */
		if ((mp = malloc (sizeof (MBOX))) == NOMBOX)
			error ("$Memória esgotada");

		/*
		 *	Preenche alguns dos campos
		 */
		mp->m_nm      = "";
#if (0)	/*******************************************************/
		mp->m_nm[0]   = '\0';
#endif	/*******************************************************/
		mp->m_ino     = 0;

	   	mp->m_index   = index;

	   	mp->m_status  = 'N';	/* Em princípio é nova */
	   	mp->m_quoted  = 0;

	   	mp->m_uidl    = "";
	   	mp->m_from    = "";
	   	mp->m_to      = "";
	   	mp->m_date    = "";
	   	mp->m_time    = -1;
	   	mp->m_subject = "";
	   	mp->m_size    = size;

	   	mp->m_text    = "";
	   	mp->m_len     = 0;

	   	mp->m_local   = NOMBOX;
	   /***	mp->m_back    = ...;
	   /***	mp->m_forw    = ...;

		/*
		 *	Insere no final da lista
		 */
		ap = pop3_list_head.m_back;

		ap->m_forw = mp;
		mp->m_back = ap;

		ap = &pop3_list_head;

		mp->m_forw = ap;
		ap->m_back = mp;

		n++;
	}

	fclose (uidl_fp);

	error ("Todas as %d mensagens serão consideradas novas", new_msg_no);

}	/* end decode_list */

/*
 ****************************************************************
 *	Atualiza a caixa postal local				*
 ****************************************************************
 */
void
update_local_mbox (void)
{
	MBOX		*mp;
	int		fd;

	/*
	 *	Pergunta se deseja atualizar
	 */
	if   (new_msg_no == 0)
	{
		printf ("NÃO há mensagens novas\n");
		return;
	}
	elif (new_msg_no == 1)
	{
		printf ("Há uma mensagem nova (%s)\n", edit_size (new_msg_bytes));
	}
	else
	{
		printf ("Há %d mensagens novas (%s)\n", new_msg_no, edit_size (new_msg_bytes));
	}

	fprintf (stderr, "Deseja atualizar a caixa postal local? (n): ");

	if (cp_askyesno () <= 0)
		return;

	/*
	 *	Executa o comando "RETR" para cada carta nova
	 */
	cp_askyesno_force	= 1;
   	cp_askyesno_force_seen  = 0;

	rm_askyesno_force	= 1;
	rm_askyesno_force_seen  = 0;

	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
	   	if (mp->m_status != 'N')
			continue;

		get_top (mp);

		get_msg_info (mp);

		printf ("%s\n", mp->m_text);

		fprintf (stderr, "Recebe a carta %d? (n): ", mp->m_index);

		if (cp_askyesno () > 0)
		{
			signal (SIGINT, SIG_IGN);

			get_message (mp);

			signal (SIGINT, SIG_DFL);
		}

		fprintf (stderr, "Remove a carta %d? (n): ", mp->m_index);

		if (rm_askyesno () > 0)
		{
			if (comando (1, "DELE %d", mp->m_index) != OK)
			{
				error
				(	"Não consegui remover a carta %d\n",
					mp->m_index
				);
			}
		}

	   	if (cp_askyesno_force_seen < 0 && rm_askyesno_force_seen < 0)
			break;

	}	/* end for (cartas novas) */

	/*
	 *	Cria/atualiza o arquivo de "status"
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((fd = open (mailbox_nm, O_RDWR|O_CREAT, REG_MODE)) < 0)
	{
		error ("*Não consegui criar o arquivo de estado \"%s\"", mailbox_nm);
		return;
	}

	if (chown (mailbox_nm, user_uid, user_gid) < 0)
		error ("*Não consegui mudar o dono/grupo do arquivo de estado \"%s\"", mailbox_nm);

	if (write (fd, "N\n", 2) != 2)
		error ("*Não consegui escrever no arquivo de estado \"%s\"", mailbox_nm);

	close (fd);


}	/* end update_local_mbox */

/*
 ****************************************************************
 *	Remove as cartas da caixa posta remota			*
 ****************************************************************
 */
void
delete_msg_in_remote_mbox (void)
{
	MBOX		*mp;
	const char	*text;

	rm_askyesno_force	= 1;
   	rm_askyesno_force_seen  = 0;

	if (total_msg_no > 0)
	{
		printf
		(	"Removendo %d carta(s) da caixa postal <%s@%s>\n",
			total_msg_no, remote_user_nm, remote_node_nm
		);
	}

	for (mp = pop3_list_head.m_forw; mp != &pop3_list_head; mp = mp->m_forw)
	{
		text = mp->m_text;

		if (text[0] == '\0' && mp->m_local != NOMBOX)
			text = mp->m_local->m_text;

		if (text[0] != '\0')
			printf ("%s\n", text + 6);

		fprintf (stderr, "Remove a carta %d? (n): ", mp->m_index);

		if (rm_askyesno () <= 0)
			continue;

	   	if (rm_askyesno_force_seen < 0)
			break;

		if (comando (1, "DELE %d", mp->m_index) != OK)
		{
			error
			(	"Não consegui remover a carta %d\n",
				mp->m_index
			);
		}
	}

}	/* delete_msg_in_remote_mbox  */

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

	if (comando (1, "RETR %d", mp->m_index) != OK)
	{
		error
		(	"Não consegui obter a mensagem %d (%s)\n",
			mp->m_index, mp->m_uidl
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

	if ((s.st_uid != user_uid || s.st_gid != user_gid) && chown (mailbox_nm, user_uid, user_gid))
		error ("*Não consegui modificar o dono/grupo do arquivo da carta \"%s\"", mailbox_nm);

	/*
	 *	Obtém um FILE para o arquivo
	 */
	if ((fp = fdopen (fd, "w")) == NOFILE)
	{
	    bad_with_close:
		close (fd);
	    bad_without_close:
		error ("*Não consegui criar o arquivo para a carta \"%s\"", mailbox_nm);
		return;
	}

	/*
	 *	Finalmente, escreve o conteúdo da carta
	 */
	fprintf (fp, "Status: U\n");
	fprintf (fp, "Uidl: %s\n", mp->m_uidl);

	for (EVER)
	{
		if (fnrgets (line, sizeof (line), tcp_fp) == NOSTR)
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

#if (0)	/*******************************************************/
			if (link (old_nm, mailbox_nm) < 0)
				continue;

			unlink (old_nm);
#endif	/*******************************************************/

			break;
		}
	}

	/*
	 *	Atualiza "status"
	 */
	fseek (status_fp, 0, SEEK_END);

	fprintf (status_fp, "%s:%s:%d:%d\n", mailbox_ptr, mp->m_uidl, s.st_size, s.st_ino);

}	/* get_message */

#if (0)	/*******************************************************/
	fclose (fp);

	/*
	 *	Verifica se pode trocar o nome do arquivo
	 */
	if (mp->m_time == -1)
		return;

	old_nm = alloca (strlen (mailbox_nm) + 1); strcpy (old_nm, mailbox_nm);

	for (letra = 't'; letra < 'z'; letra++)
	{
		sprintf (mailbox_ptr, "l.%u.%c", mp->m_time, letra);

		if (link (old_nm, mailbox_nm) < 0)
			continue;

		unlink (old_nm);
		break;
	}
#endif	/*******************************************************/

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
	if (comando (1, "TOP %d 0", mp->m_index) != OK)
	{
		error
		(	"Não consegui obter o cabeçalho da mensagem %d (%s)\n",
			mp->m_index, mp->m_uidl
		);
		return;
	}

	/*
	 *	Le o cabeçalho
	 */
	for (EVER)
	{
		if (fnrgets (line, sizeof (line), tcp_fp) == NOSTR)
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

		if (!end_of_header_seen)
			get_line_info (line_ptr, mp);
	}

}	/* get_top */

/*
 ****************************************************************
 *	 Envia comandos	para o servidor				*
 ****************************************************************
 */
int
comando (int do_wait, const char *format, ...)
{
	va_list 	args;
	int		r = OK;
	char		com[BLSZ];

	va_start (args, format);

	vsprintf (com, format, args);

	if (vflag)
		printf ("Enviando o comando \"%s\"\n", com);

	strcat (com, "\r\n");

	if (t_snd (tcp_fd, com, strlen (com), T_PUSH) < 0)
		error (NOSTR);

	if (do_wait)
		r = get_reply ();
	
	va_end (args);

	return (r);

}	/* end comando */

/*
 ****************************************************************
 *	Recebe uma mensagem do servidor				*
 ****************************************************************
 */
int
get_reply (void)
{
	int		code = OK;

	/*
	 *	Lê a primeira linha
	 */
	if (fnrgets (first_reply_line, sizeof (first_reply_line), tcp_fp) == NOSTR)
		error ("$*Erro na leitura de uma mensagem do servidor");

	if (vflag)
		puts (first_reply_line);

	if (strncmp (first_reply_line, "+OK", 3))
		code = ERR;

	return (code);

}	/* end get_reply */

/*
 ****************************************************************
 *	Le uma linha de um arquivo (terminando por "\r\n")	*
 ****************************************************************
 */
char *
fnrgets (char *str, int n, FILE *fp)
{
	char		*cp = str;
	int		c = 0;

	while ((c = getc (fp)) >= 0)
	{
		if (c == '\n')
		{
			if (cp > str && cp[-1] == '\r')
				cp--;

			break;
		}

		if (--n > 0)
			*cp++ = c;
	}

	if (cp == str && c < 0)
		return (NOSTR);

	*cp = '\0';

	return (str);

}	/* end fnrgets */
