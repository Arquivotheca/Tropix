/*
 ****************************************************************
 *								*
 *			mbox.c					*
 *								*
 *	Post Office Protocol - Version 3 (servidor)		*
 *								*
 *	Versão	3.0.0, de 15.08.98				*
 *		4.0.0, de 11.01.01				*
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
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/pop3_s.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	REG_MODE	0600

#define	MAIL_PREFIX	"/var/mail/"

entry char		mailbox_nm[80] = MAIL_PREFIX;

entry const char	status_nm[] = "status";

entry int		mailbox_dev;		/* Dispositivo da caixa de correio */

entry MBOX		mbox_list_head =	/* Cabeça da lista de mensagens */
{
	&mbox_list_head, &mbox_list_head
};
	
entry int		mbox_list_size;		/* No. de cartas da caixa postal */

entry MBOX		**mbox_list_vector;	/* Vetor para acesso direto */

entry int		last_read_index;	/* mensagem atual */

/*
 ******	Protótipos de funções ***********************************
 */
int		get_line_info (const char *line, MBOX *mp);
char		*fnrgets (char *str, int n, FILE *fp);

/*
 ****************************************************************
 *	Lê as cartas da caixa postal local			*
 ****************************************************************
 */
void
get_mbox (void)
{
	const PASSWD		*pw = &user_pw;
	MBOX			*mp, *bp, *fp;
	int			fd, index = 0;
	const char		*file_nm, *uidl, *size_str, *ino_str;
	ino_t			ino = 0;
	off_t			size = 0;
	int			bad;
	FILE			*status_fp;
	STAT			s;
	char			area[DATASZ];

	/*
	 *	Prepara a caixa postal
	 */
	setgid  (pw->pw_gid);
	seteuid (pw->pw_uid);

	strcat (mailbox_nm, pw->pw_name);

	if (chdir (mailbox_nm) < 0)
	{
		error
		(	"*Não consegui dar \"chdir\" para o diretório \"%s\"",
			mailbox_nm
		);
	    no_mbox:
		reply
		(	"-*Usuário \"%s\" recusado "
			"(diretório \"%s\" não disponível)",
			pw->pw_name, mailbox_nm
		);

		server_state = S_RESET;
		return;
	}

	/*
	 *	Abre a caixa postal
	 */
	if
	(	(status_fp = fopen (status_nm, "r")) == NOFILE ||
		fstat (fileno (status_fp), &s) < 0
	)
	{
		error ("*Não consegui abrir a caixa postal \"%s\"", mailbox_nm);
		goto no_mbox;
	}

	mailbox_dev  = s.st_dev;

	reply ("+*Usuário \"%s\" em sessão", pw->pw_name);

	error ("Login para \"%s@%s\"", pw->pw_name, client_node_nm);

	server_state = S_READY;

	/*
	 *	Lê o arquivo de índices "status"
	 */
	fseek (status_fp, 2, SEEK_SET);

	while (fngets (area, sizeof (area), status_fp) != NOSTR)
	{
		bad = 0;

		if ((file_nm = strfield (area, ':')) == NOSTR)
			bad = 1;

		if ((uidl = strfield (NOSTR, ':')) == NOSTR)
			 bad = 2;

		if
		(	(size_str = strfield (NOSTR, ':')) == NOSTR ||
			(size = atol (size_str)) == 0
		)
			bad = 3;

		if
		(	(ino_str = strfield (NOSTR, ':')) == NOSTR ||
			(ino = atol (ino_str)) == 0
		)
			bad = 4;

		if (bad)
			{ error ("Arquivo \"%s\" com erro %d", status_nm, bad); continue; }

		/*
		 *	Insere na lista
		 */
		if ((mp = malloc (sizeof (MBOX))) == NOMBOX)
			error ("$Memória esgotada");

		if ((mp->m_nm = malloc (strlen (file_nm) + 1)) == NOSTR)
			error ("$Memória esgotada");

		if ((mp->m_uidl = malloc (strlen (uidl) + 1)) == NOSTR)
			error ("$Memória esgotada");

		strcpy (mp->m_nm, file_nm);
		strcpy (mp->m_uidl, uidl);
		mp->m_size = size;
		mp->m_ino  = ino;

	   /***	mp->m_index   = ...; ***/

	   	mp->m_status  = ' ';	/* Ainda não tem informações sobre a carta */
	   	mp->m_erased  = 0;	/* Ainda não está apagada */

		/*
		 *	Insere na lista
		 */
		for (fp = mbox_list_head.m_back; fp != &mbox_list_head; fp = fp->m_back)
		{
			ulong		mp_no, fp_no;

			mp_no = atol (mp->m_nm + 2);
			fp_no = atol (fp->m_nm + 2);

			if (mp_no != 0 && fp_no != 0)
			{
				if (mp_no >= fp_no)
					break;
			}
			else
			{
				if (strcmp (mp->m_nm, fp->m_nm) >= 0)
					break;
			}
		}

		bp = fp->m_forw;

		bp->m_back = mp; mp->m_forw = bp;
		mp->m_back = fp; fp->m_forw = mp;

		mbox_list_size++;

	}	/* lendo e escrevendo as linhas dos arquivos */

	fclose (status_fp);

	/*
	 *	Numera as cartas e cria o vetor de acesso direto
	 */
	if ((mbox_list_vector = malloc ((mbox_list_size + 1) * sizeof (MBOX *))) == (MBOX **)NULL)
		error ("$Memória esgotada");

	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
	{
		mp->m_index = ++index; mbox_list_vector[index] = mp;
	}

	last_read_index = 0;

#undef	DEBUG
#ifdef	DEBUG
	/*
	 *	Imprime a lista de arquivos
	 */
	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = mp->m_forw)
		printf ("%s\r\n", mp->m_text);
#endif	DEBUG

	/*
	 *	A caixa postal foi lida
	 */
	if ((fd = open (status_nm, O_RDWR|O_CREAT), REG_MODE) < 0)
	{
		error ("*Não consegui criar o arquivo de estado \"%s\"", status_nm);
		return;
	}

	write (fd, "R\n", 2);
	close (fd); 

}	/* end get_mbox */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Armazena informação de uma linha de cabeçalho		*
 ****************************************************************
 */
int
get_line_info (const char *line, MBOX *mp)
{
	const char	*src;
	char		*dst;
	int		ret = 0, n;
	char		key_word[32];

	/*
	 *	Procura a palavra-chave
	 */
	if ((src = strchr (line, ':')) == NOSTR)
		return (0);

	if ((n = src - line) >= sizeof (key_word) - 1)
		return (0);

	strncpy (key_word, line, n); key_word[n] = '\0';

	src++;	/* Pula o ":" */

	while (*src == ' ' || *src == '\t')
		src++;

	/*
	 *	Procura o valor
	 */
	if (*src != '\0')
	{
		if ((dst = malloc (strlen (src) + 1)) == NOSTR)
			error ("$Memória esgotada");

		strcpy (dst, src);
	}
	else
	{
		dst = "";
	}

	if   (streq (strtoupper (key_word), "STATUS"))
	{
		ret = -1;	/* Ignora o STATUS */
	}
#if (0)	/*******************************************************/
	elif (streq (key_word, "FROM"))
	{
		mp->m_from = dst;
	}
	elif (streq (key_word, "TO"))
	{
		mp->m_to = dst;
	}
	elif (streq (key_word, "DATE"))
	{
		mp->m_date = dst;
		mp->m_time = get_date (dst);
	}
	elif (streq (key_word, "SUBJECT"))
	{
		mp->m_subject = dst;
	}
#endif	/*******************************************************/
	elif (streq (key_word, "UIDL"))
	{
		if (mp->m_uidl[0] == '\0')
			mp->m_uidl = dst;

		ret = -1;	/* Ignora o UIDL */
	}

	return (ret);

}	/* end get_line_info */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Libera os recursos da caixa postal local		*
 ****************************************************************
 */
void
put_mbox (void)
{
	MBOX		*mp, *forw;

	if (mbox_list_head.m_forw == NOMBOX)
		return;

	free (mbox_list_vector); mbox_list_vector = (MBOX **)NULL;

	for (mp = mbox_list_head.m_forw; mp != &mbox_list_head; mp = forw)
	{
		forw = mp->m_forw; free (mp);
	}

}	/* end put_mbox */

#if (0)	/*******************************************************/
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
#endif	/*******************************************************/
