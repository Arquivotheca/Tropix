/*
 ****************************************************************
 *								*
 *			mail.c					*
 *								*
 *	Recebe ou envia cartas do correio eletrônico		*
 *								*
 *	Versão	3.0.0, de 03.12.97				*
 *		4.9.0, de 08.09.07				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <errno.h>
#include <xti.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão:	4.9.0, de 08.09.07";

#define MAIL_PORT	25

/*
 ****** Variáveis globais ***************************************
 */
entry int		vflag;		/* Verbose */
entry int		eflag;		/* Edita o arquivo */
entry int		fflag;		/* Foi dado o nome do arquivo */
entry int		dflag;		/* Dá o diretório */
entry int		rflag;		/* Não modifique a caixa-postal */
entry int		sflag;		/* Tamanho da carta */
entry int		uflag;		/* Só considera carta não lidas */
entry int		iflag;		/* Atualiza o índice de cartas */
entry int		aflag;		/* Envia a todos os servidores */

entry int		exit_code = 0;	/* Código de retorno */

entry int		global_tcp_fd;	/* Para o TCP */

entry const char	*local_user_nm; /* Nome do usuário local */

entry char		mailbox_nm[120] = "/var/mail/";

entry const char	tcp_dev_nm[] = "/dev/itntcp";

const char		send_mail_nm[] = "/usr/lib/send_mail";
const char		*const send_mail_arg0 = send_mail_nm + 9;

const char		*send_mail_file_nm;

entry char		my_name[256];	/* Nome do computador */

entry const char	*my_domain;	/* Nome do domínio */

entry char		*mailrc_domain;	/* Nome do domínio dado em ".mailrc" */

entry char		*user_complete_name; /* Nome do usuário */

entry char		*snd_nm;	/* Nome do remetente */

entry const char	mailrc[] = "/.mailrc";
entry char		*mailrc_nm;	/* Nome do arquivo ".mailrc" */

entry const char	dead_letter[] = "/dead.letter";
entry char		*dead_letter_nm; /* Nome da cópia da carta */

entry const char	global_mailrc_nm[] = "/etc/mailrc";

entry char		from_with_host;	/* Inclui o nome do computador */

entry char		subject[128];	/* Assunto da carta */

entry const char	*text_file_nm;	/* Nome do arquivo do texto */

/*
 ******	Lista de alias ******************************************
 */
typedef struct alias	ALIAS;

struct alias
{
	const char	*a_nm;	/* Nome simbólico ou do domínio */
	ALIAS		*a_domain;	/* Domínio seguinte */
	ALIAS		*a_alias;	/* Alias seguinte */
};

#define	NOALIAS	(ALIAS *)NULL

entry ALIAS		*alias_head;	/* Lista de alias */

/*
 ******	Lista de destinatários **********************************
 */
typedef struct dst	DST;

struct dst
{
	const char	*d_nm;		/* Nome do destinatário */
	char		d_cc;		/* Se For do Cc */
	DST		*d_forw;	/* Destinatário seguinte */
	DST		*d_back;	/* Destinatário anterior */
};

#define	NODST	(DST *)NULL

/*
 ******	Protótipos de funções ***********************************
 */
void		open_and_read_mailrc (void);
void		read_mailrc (FILE *);
void		insert_alias (DST *, const char *, int);
const char	*can_dst_nm (const char *);
const char	*create_header_file (const DST *, const char *);
const char	*create_text_file (void);
const char	*edit_text_file (const char *);
#if (0)	/*******************************************************/
const char	*copy_text_file (const char *);
#endif	/*******************************************************/
void		snd_on_signal (int, ...);
void		help (void);

int		forward_mail (const char *argv[], const char *);

/*
 ****************************************************************
 *	Recebe ou envia cartas do correio eletrônico		*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt, count;
	const char	*cp;
	const PASSWD	*pwd;
	const char	*file_nm = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vef:d:rsuiaH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'e':			/* Edita o arquivo */
			eflag++;
			break;

		    case 'f':			/* O texto da carta foi dado em um arquivo */
			file_nm = optarg;
			fflag++;
			break;

		    case 'd':			/* Dá o diretório */
			dflag++;
			strncpy (mailbox_nm, optarg, sizeof (mailbox_nm));
			break;

		    case 'r':			/* Somente leitura */
			rflag++;
			break;

		    case 's':			/* Tamanho */
			sflag++;
			break;

		    case 'u':			/* Não lidas */
			uflag++;
			break;

		    case 'i':			/* Atualiza o índice de cartas */
			iflag++;
			break;

		    case 'a':			/* Envia a todos os servidores */
			aflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Reserva um espaço para as áreas de entrada/saída e "readdir"
	 *	(Não muito elegante)
	 */
	free (malloc (4 * 1024 + sizeof (DIR)));

	/*
	 *	Obtém o nome do usuário local
	 */
	local_user_nm = alloc_and_copy (getenv ("USER"));

	if (local_user_nm == NOSTR)
		local_user_nm = getlogin ();

	if (local_user_nm == NOSTR)
		error ("$Não consegui obter o nome do usuário local");

	if ((pwd = getpwnam (local_user_nm)) == NOPASSWD)
		error ("$Não consegui obter a entrada de contas/senhas de \"%s\"", local_user_nm);

	/*
	 *	Prepara algumas cadeias
	 */
	user_complete_name = alloca (strlen (pwd->pw_comment) + 1);
	strcpy (user_complete_name, pwd->pw_comment);

	mailrc_nm = alloca (strlen (pwd->pw_dir) + strlen (mailrc) + 1);
	strcpy (mailrc_nm, pwd->pw_dir);
	strcat (mailrc_nm, mailrc);

	dead_letter_nm = alloca (strlen (pwd->pw_dir) + strlen (dead_letter) + 1);
	strcpy (dead_letter_nm, pwd->pw_dir);
	strcat (dead_letter_nm, dead_letter);

	endpwent ();

	/*
	 *	Lê o arquivo ".mailrc" (resultado em "alias_head")
	 */
	open_and_read_mailrc ();

	/*
	 *	Obtém o nome do próprio computador
	 */
	if ((global_tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	cp = t_addr_to_node (global_tcp_fd, 0);

	if (cp[0] >= '0' && cp[0] <= '9')
		error ("$NÃO consegui obter o nome do próprio computador");

	strcpy (my_name, cp); cp = strchr (my_name, '.');

	if (cp == NOSTR)
		error ("$NÃO consegui obter o nome do próprio domínio");

	my_domain = cp + 1;

#if (0)	/*******************************************************/
	close (global_tcp_fd);
#endif	/*******************************************************/

#ifdef	DEBUG
	if (vflag)
		printf ("Meu nome/domínio: %s/%s\n", my_name, my_domain);
#endif	DEBUG
	/*
	 *	Prepara o tempo em INGLÊS
	 */
	if (setlocale (LC_TIME, "E") == NOSTR)
		error ("Não consegui alterar o tempo para inglês");

	/*
	 *	Prepara o nome do remetente
	 */
	if   (mailrc_domain != NOSTR)
		count = strlen (mailrc_domain);
	elif (from_with_host)
		count = strlen (my_name);
	else
		count = strlen (my_domain);

	snd_nm = alloca (strlen (local_user_nm) + count + 2);

	if   (mailrc_domain != NOSTR)
		sprintf (snd_nm, "%s@%s", local_user_nm, mailrc_domain);
	elif (from_with_host)
		sprintf (snd_nm, "%s@%s", local_user_nm, my_name);
	else
		sprintf (snd_nm, "%s@%s", local_user_nm, my_domain);

	/*
	 *	Verifica se é para ler a caixa postal ou atualizar o arquivo índice
	 */
	if (argc == 0)
	{
		if (iflag)
			update_status_file ();
		else
			mbox ();

		exit (exit_code);
	}

#if (0)	/*******************************************************/
	/*
	 *	Obtém o nome do próprio computador
	 */
	if ((global_tcp_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", tcp_dev_nm);

	cp = t_addr_to_node (global_tcp_fd, 0);

	if (cp[0] >= '0' && cp[0] <= '9')
		error ("$NÃO consegui obter o nome do próprio computador");

	strcpy (my_name, cp); cp = strchr (my_name, '.');

	if (cp == NOSTR)
		error ("$NÃO consegui obter o nome do próprio domínio");

	my_domain = cp + 1;

	close (global_tcp_fd);

#ifdef	DEBUG
	if (vflag)
		printf ("Meu nome/domínio: %s/%s\n", my_name, my_domain);
#endif	DEBUG
#endif	/*******************************************************/

	/*
	 *	Em caso de sinal, ...
	 */
	signal (SIGINT,  snd_on_signal);
	signal (SIGTERM, snd_on_signal);

	/*
	 *	Envia as cartas
	 */
	forward_mail (argv, file_nm);

}	/* end mail */

/*
 ****************************************************************
 *	Envia cartas						*
 ****************************************************************
 */
int
forward_mail (const char *argv[], const char *file_nm)
{
	const char	**cpp;
	const char	*arg;
	const char	*boundary = NOSTR;
	const DST	*dp;
	const char	**vv, **vp;
	int		pid;
	int		count;
	time_t		data;
	const TM	*tm;
	int		c;
#if (0)	/*******************************************************/
	char		*snd_nm;
#endif	/*******************************************************/
	FILE		*text_fp, *send_mail_fp;
	DST		dst_head;
	char		area[256];

	/*
	 *	Esvazia a fila
	 */
	dst_head.d_forw = &dst_head;
	dst_head.d_back = &dst_head;

	/*
	 *	Insere os destinatários de "argv"
	 */
	for (cpp = argv; (arg = *cpp) != NOSTR; cpp++)
		insert_alias (&dst_head, arg, 0 /* Não cc */);

	/*
	 *	Lê o texto
	 */
	if   (file_nm == NOSTR)
		text_file_nm = create_text_file ();
	elif (eflag)
		text_file_nm = edit_text_file (file_nm);
	else
		text_file_nm = copy_text_file (file_nm);

#undef	DEBUG
#ifdef	DEBUG
	if (vflag)
	{
		int	c;

		printf ("*********************\n");

		rewind (text_file_fp);

		while ((c = getc (text_file_fp)) >= 0)
			putchar (c);

		printf ("*********************\n");
	}
#endif	DEBUG

	/*
	 *	Verifica se deseja enviar anexos
	 */
	if (!fflag || eflag)
	{
		fprintf (stderr, "Anexos: "); fnrgets (area, sizeof (area), stdin);

		if (area[0] != '\0')
			boundary = insert_attachments (area, &text_file_nm);
	}

	/*
	 *	Verifica se deseja enviar cópias
	 */
	fprintf (stderr, "Cc: "); fnrgets (area, sizeof (area), stdin);

	for (arg = strtok (area, " ,\t"); arg != NOSTR; arg = strtok (NOSTR, " ,\t"))
		insert_alias (&dst_head, alloc_and_copy (arg), 1 /* cc */);

	/*
	 *	Começa a criar o arquivo da carta
	 *
	 *	Escreve a parte de remetente/destinatários para o "send_mail"
	 */
	if ((send_mail_file_nm = mktemp ("/var/tmp/send_mail_file_XXXXXX")) == NOSTR)
		error ("$NÃO consegui obter o nome de um arquivo temporário");

	if ((send_mail_fp = fopen (send_mail_file_nm, "w+")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo temporário");

	/*
	 *	Compõe o cabeçalho da carta
	 */
	time (&data); tm = localtime (&data);
	strftime (area, BLSZ, "%a, %d %b %Y %X %Z", tm);
	fprintf (send_mail_fp, "Date: %s\r\n", area);

	if   (mailrc_domain != NOSTR)
		fprintf (send_mail_fp, "From: %s <%s@%s>\r\n", user_complete_name, local_user_nm, mailrc_domain);
	elif (from_with_host)
		fprintf (send_mail_fp, "From: %s <%s@%s>\r\n", user_complete_name, local_user_nm, my_name);
	else
		fprintf (send_mail_fp, "From: %s <%s@%s>\r\n", user_complete_name, local_user_nm, my_domain);

	/* Põe o "To" */

	for (count = 0, dp = dst_head.d_forw; dp != &dst_head; dp = dp->d_forw)
	{
		if (dp->d_cc)
			continue;

		if (count++ == 0)
			fprintf (send_mail_fp, "To: ");
		else
			fprintf (send_mail_fp, ", ");

		fprintf (send_mail_fp, "%s", dp->d_nm);
	}

	if (count)
		fprintf (send_mail_fp, "\r\n");

	/* Põe o "Cc" */

	for (count = 0, dp = dst_head.d_forw; dp != &dst_head; dp = dp->d_forw)
	{
		if (!dp->d_cc)
			continue;

		if (count++ == 0)
			fprintf (send_mail_fp, "Cc: ");
		else
			fprintf (send_mail_fp, ", ");

		fprintf (send_mail_fp, "%s", dp->d_nm);
	}

	if (count)
		fprintf (send_mail_fp, "\r\n");

	fprintf (send_mail_fp, "Subject: %s\r\n", subject);

	if (boundary != NOSTR)
	{
		fprintf
		(	send_mail_fp,
			"Mime-Version: 1.0\r\n"
			"Content-Type: multipart/mixed; charset=iso-8859-1;\r\n"
			"	boundary= \"%s\"\r\n"
			"Content-Transfer-Encoding: 8bit\r\n",
			boundary
		);
	}
	else
	{
		fprintf
		(	send_mail_fp,
			"Content-Type: text/plain; charset=ISO-8859-1\r\n"
			"Content-Transfer-Encoding: 8bit\r\n"
		);
	}

	fprintf (send_mail_fp, "\r\n");		/* Separa o cabeçalho do corpo da carta */

	/*
	 *	Escreve o texto da carta
	 */
	if ((text_fp = fopen (text_file_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"\n", text_file_nm);

#if (0)	/*******************************************************/
	char		iso = 1;
	if (iso)
	{
		while ((c = getc (text_fp)) >= 0)
		{
			if   (c == '\r')
				continue;
			elif (c == '\n')
				putc ('\r', send_mail_fp);

			if (c < 0x80 && c != '=')
			{
				putc (c, send_mail_fp);
			}
			else
			{
				putc ('=', send_mail_fp);
				putc (quote_vec[c >> 4], send_mail_fp);
				putc (quote_vec[c & 0x0F], send_mail_fp);
			}
		}
	}
	else
#endif	/*******************************************************/
	{
		while ((c = getc (text_fp)) >= 0)
		{
			if   (c == '\r')
				continue;
			elif (c == '\n')
				putc ('\r', send_mail_fp);

			putc (c, send_mail_fp);
		}
	}

	fclose (text_fp);

	unlink (text_file_nm);

	fclose (send_mail_fp);

	/*
	 *	Invoca o programa "send_mail"
	 */
	for (count = 0, dp = dst_head.d_forw; dp != &dst_head; dp = dp->d_forw)
		count++;

	vv = vp = alloca ((6 + 1 + count) * sizeof (char *));

	*vp++ = send_mail_arg0;		/* 0: send_mail */

	if (vflag)
		*vp++ = "-v";		/* 1: -v */

	if (fflag && !eflag)		/* 2: -f */
		*vp++ = "-f";

	if (aflag)
		*vp++ = "-a";		/* 3: -a */

	*vp++ = send_mail_file_nm;	/* 4: /var/tmp/... */

	*vp++ = snd_nm;

	for (dp = dst_head.d_forw; dp != &dst_head; dp = dp->d_forw)
		*vp++ = dp->d_nm;

	*vp = NOSTR;			/* 5: NOSTR */

#undef	SEND_MAIL_DEBUG
#ifdef	SEND_MAIL_DEBUG
	fprintf (stderr, "SEND_MAIL: ");

	for (vp = vv; *vp != NOSTR; vp++)
		fprintf (stderr, "%s ", *vp);

	fprintf (stderr, "\n");
#else
	if   ((pid = fork ()) < 0)
		error ("$*Não consegui criar um processo");
	elif (pid != 0)
		return (0);

	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	execv (send_mail_nm, vv);
	error ("$*Não consegui executar \"%s\"", send_mail_nm);
#endif	SEND_MAIL_DEBUG

	return (0);	/* Nunca executado */

}	/* end forward_mail */

/*
 ****************************************************************
 *	Abre e processa os arquivos ".mailrc"			*
 ****************************************************************
 */
void
open_and_read_mailrc (void)
{
	FILE		*fp;

	/*
	 *	Abre o arquivo "/etc/mailrc"
	 */
	if   ((fp = fopen (global_mailrc_nm, "r")) != NOFILE)
	{
		read_mailrc (fp); fclose (fp);
	}
	elif (errno != ENOENT)
	{
		error ("Não consegui ler o arquivo \"%s\"", global_mailrc_nm);

	}

	/*
	 *	Abre o arquivo ".mailrc"
	 */
	if   ((fp = fopen (mailrc_nm, "r")) != NOFILE)
	{
		read_mailrc (fp); fclose (fp);
	}
	elif (errno != ENOENT)
	{
		error ("Não consegui ler o arquivo \"%s\"", mailrc_nm);

	}

#undef	ALIAS_DEBUG
#ifdef	ALIAS_DEBUG
	/*
	 *	Imprime a lista
	 */
	{
		ALIAS	*ap, *dp;

		for (ap = alias_head; ap != NOALIAS; ap = ap->a_alias)
		{
			printf ("%s:", ap->a_nm);

			for (dp = ap->a_domain; dp != NOALIAS; dp = dp->a_domain)
				printf (" %s", dp->a_nm);

			printf ("\n");
		}
	}
#endif	ALIAS_DEBUG

}	/* end open_and_read_mailrc */

/*
 ****************************************************************
 *	Processa o arquivo ".mailrc"				*
 ****************************************************************
 */
void
read_mailrc (FILE *fp)
{
	ALIAS		*ap, *dp;
	ALIAS		*alias_tail  = NOALIAS;
	ALIAS		*domain_tail = NOALIAS;
	const char	*key, *sym, *domain;
	char		area[BLSZ];

	/*
	 *	Por enquanto, processa apenas linhas "alias", "[un]set from_with_host" e "set domain"
	 */
	while (fnrgets (area, BLSZ, fp) != NOSTR)
	{
		if ((key = strtok (area, " ,\t")) == NOSTR)
			continue;

		if   (streq (key, "set"))
		{
			if ((sym = strtok (NOSTR, " ,\t")) == NOSTR)
				continue;

			if   (streq (sym, "from_with_host"))
			{
				from_with_host = 1;
			}
			elif (!strncmp (sym, "domain=", 7))
			{
				mailrc_domain = mem_alloc (strlen (sym + 7) + 1);

				strcpy (mailrc_domain, sym + 7);
			}

			continue;
		}
		elif (streq (key, "unset"))
		{
			if ((sym = strtok (NOSTR, " ,\t")) == NOSTR)
				continue;

			if (streq (sym, "from_with_host"))
				from_with_host = 0;

			continue;
		}

		if (!streq (key, "alias"))
			continue;

		if ((sym = strtok (NOSTR, " ,\t")) == NOSTR)
			continue;

		ap = mem_alloc (sizeof (ALIAS));

		ap->a_nm     = alloc_and_copy (sym);
		ap->a_domain = NOALIAS;
		ap->a_alias  = NOALIAS;

		if (alias_head == NOALIAS)
			{ alias_head = ap; alias_tail = ap; }
		else
			{ alias_tail->a_alias = ap; alias_tail = ap; }

		if (ap->a_nm == NOSTR)
			error ("$Memória esgotada");

		while ((domain = strtok (NOSTR, " ,\t")) != NOSTR)
		{
		    next_line:
			if (domain[0] == '\\' && domain[1] == '\0')
			{
				if (fnrgets (area, BLSZ, fp) == NOSTR)
					return;

				if ((domain = strtok (area, " ,\t")) == NOSTR)
					break;

				goto next_line;
			}

			dp = mem_alloc (sizeof (ALIAS));

			dp->a_nm     = alloc_and_copy (domain);
			dp->a_domain = NOALIAS;
			dp->a_alias  = NOALIAS;

			if (ap->a_domain == NOALIAS)
				{ ap->a_domain = dp; domain_tail = dp; }
			else
				{ domain_tail->a_domain = dp; domain_tail = dp; }

			if (dp->a_nm == NOSTR)
				error ("$Memória esgotada");
		}
	}

}	/* end read_mailrc */

/*
 ****************************************************************
 *	Insere um nome simbólico da lista de destinatários	*
 ****************************************************************
 */
void
insert_alias (DST *dst_head, const char *arg, int cc)
{
	const ALIAS		*ap;
	DST			*dp, *bp;
	char			*cp;
	ALIAS			arg_head;

	if (streq (arg, "."))
		return;

	cp = strend (arg) - 1;

	if (cp[0] == ',')		/* Retira uma possível vírgula do final */
		cp[0] = '\0';

	for (ap = alias_head; /* abaixo */; ap = ap->a_alias)
	{
		if (ap == NOALIAS)
		{
			arg_head.a_nm     = arg;
			arg_head.a_domain = NOALIAS;
			arg_head.a_alias  = NOALIAS;

			ap = &arg_head;
			break;
		}

		if (streq (ap->a_nm, arg))
		{
			ap = ap->a_domain;
			break;
		}
	}

	for (/* acima */; ap != NOALIAS; ap = ap->a_domain)
	{
		dp = mem_alloc (sizeof (DST));

		dp->d_nm = can_dst_nm (ap->a_nm);
		dp->d_cc = cc;

		bp = dst_head->d_back;

		dp->d_forw = dst_head;
		dst_head->d_back = dp;

		dp->d_back = bp;
		bp->d_forw = dp;
	}

}	/* end insert_alias */

/*
 ****************************************************************
 *	Canoniza o nome do destinatário				*
 ****************************************************************
 */
const char *
can_dst_nm (const char *dst_nm)
{
	char		*new_nm;
	const char	*add_sufix_str;

	if   (strchr (dst_nm, '@') == NOSTR)
		add_sufix_str = "@";
	elif (strchr (dst_nm, '.') == NOSTR)
		add_sufix_str = ".";
	else
		return (dst_nm);

	new_nm = mem_alloc (strlen (dst_nm) + strlen (my_domain) + 2);

	strcpy (new_nm, dst_nm);
	strcat (new_nm, add_sufix_str);
	strcat (new_nm, my_domain);

	return (new_nm);

}	/* end can_dst_nm */

/*
 ****************************************************************
 *	Lê o texto da mensagem					*
 ****************************************************************
 */
const char *
create_text_file (void)
{
	FILE		*fp;
	int		c, last_c = 0;
	const char	*text_nm;
	char		area[80];
	char		local_subject[80];

	enum { NORMAL, NL_SEEN, TIL_SEEN, DOT_SEEN }	state = NL_SEEN;

	/*
	 *	Obtém o arquivo
	 */
	if ((text_nm = mktemp ("/tmp/mail_text_XXXXXX")) == NOSTR)
		error ("$NÃO consegui obter o nome de um arquivo temporário");

	if ((fp = fopen (text_nm, "w+")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo temporário");

	/*
	 *	Lê o assunto
	 */
	fprintf (stderr, "Assunto: ");
	fnrgets (local_subject, sizeof (local_subject), stdin);

	convert_subject_to_ISO (subject, local_subject, sizeof (subject));

	/*
	 *	Lê o conteúdo
	 */
	for (EVER)
	{
		if ((c = getchar ()) < 0)
			break;

		if   (c == '\n')
		{
			if (state == DOT_SEEN)
				break;

			state = NL_SEEN;
		}
		elif (c == '~')
		{
			if (state == NL_SEEN)
			{
				state = TIL_SEEN;
				last_c = c;
				continue;
			}
			else
			{
				state = NORMAL;
			}
		}
		elif (c == 'r')
		{
			if (state == TIL_SEEN)
			{
				FILE		*file_fp;
				int		n = 0;
				const char	*file_nm;

				state = NORMAL; last_c = 0;
				fnrgets (area, sizeof (area), stdin);

				if ((file_nm = strtok (area, " ,\t")) == NOSTR)
					continue;

				if ((file_fp = fopen (file_nm, "r")) == NOFILE)
				{
					error ("*Não consegui abrir \"%s\"", file_nm);
					continue;
				}

				while ((c = getc (file_fp)) >= 0)
					{ putc (c, fp); n++; }

				printf ("%s: %d bytes\n", file_nm, n);
				fclose (file_fp);
				state = NL_SEEN;
				continue;
			}
			else
			{
				state = NORMAL;
			}
		}
		elif (c == 'v')
		{
			if (state == TIL_SEEN)
			{
				state = NORMAL; last_c = 0;

				getchar ();	/* Pula o <nl> */
				fflush (fp);
				sprintf (area, "vi %s", text_nm);

				if (system (area) < 0)
					error ("*Não consegui editar \"%s\"", text_nm);

				printf ("(continue)\n");
				fseek (fp, 0, SEEK_END);
				state = NL_SEEN;
				continue;
			}
			else
			{
				state = NORMAL;
			}
		}
		elif (c == '.')
		{
			if (state == NL_SEEN)
			{
				state = DOT_SEEN;
				last_c = c;
				continue;
			}
			else
			{
				state = NORMAL;
			}
		}
		else
		{
			state = NORMAL;
		}

		if (last_c != 0)
			{ putc (last_c, fp); last_c = 0; }

		putc (c, fp);

	}	/* end for (EVER) */

	fclose (fp);

	return (text_nm);

}	/* end create_text_file */

/*
 ****************************************************************
 *	Edita um arquivo de texto dado				*
 ****************************************************************
 */
const char *
edit_text_file (const char *file_nm)
{
	FILE		*fp_in, *fp_tmp;
	const char	*text_nm;
	int		c;
	char		NL_SEEN;
	char		line[120];

	/*
	 *	Abre o arquivo
	 */
	if ((fp_in = fopen (file_nm, "r")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo \"%s\" de texto", file_nm);

	/*
	 *	Obtém o arquivo temporário
	 */
	if ((text_nm = mktemp ("/tmp/mail_text_XXXXXX")) == NOSTR)
		error ("$NÃO consegui obter o nome de um arquivo temporário");

	if ((fp_tmp = fopen (text_nm, "w+")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo temporário");

	/*
	 *	Obtém o assunto
	 */
	subject[0] = '\0';

	if (fngets (line, sizeof (line), fp_in) == line)
	{
		if (strncmp (line, "Assunto: ", 9) == 0)
			strncpy (subject, line + 9, sizeof (subject));
	}

	if (subject[0] == '\0')
	{
		fprintf (stderr, "Assunto: ");
		fnrgets (subject, sizeof (subject), stdin);

		fseek (fp_in, 0, SEEK_SET);
	}

	/*
	 *	Converte o conteúdo
	 */
	fputs
	(	"\n\n"
		"------------ Carta Recebida ------------\n",
		fp_tmp
	);

	for (NL_SEEN = 1; /* abaixo */; /* abaixo */)
	{
		if ((c = getc (fp_in)) < 0)
			break;

		if (NL_SEEN)
			{ fputs ("> ", fp_tmp); NL_SEEN = 0; }

		putc (c, fp_tmp);

		if (c == '\n')
			NL_SEEN = 1;
	}

	fputs (">\n", fp_tmp);

	fclose (fp_in); fclose (fp_tmp);

	/*
	 *	Edita o arquivo através do "vi"
	 */
	sprintf (line, "vi %s", text_nm);

	if (system (line) < 0)
		error ("*Não consegui editar \"%s\"", text_nm);

	return (text_nm);

}	/* end edit_text_file */

/*
 ****************************************************************
 *	Copia um arquivo de texto dado				*
 ****************************************************************
 */
const char *
copy_text_file (const char *file_nm)
{
	FILE		*fp_in, *fp_tmp;
	const char	*text_nm;
	const char	*lp;
	char		*key_word;
	const TM	*tm;
	time_t		data;
	int		c;
	char		area[DATASZ], token[DATASZ];

	/*
	 *	Abre o arquivo
	 */
	if ((fp_in = fopen (file_nm, "r")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo \"%s\" de texto", file_nm);

	/*
	 *	Obtém o arquivo temporário
	 */
	if ((text_nm = mktemp ("/var/tmp/mail_text_XXXXXX")) == NOSTR)
		error ("$NÃO consegui obter o nome de um arquivo temporário");

	if ((fp_tmp = fopen (text_nm, "w+")) == NOFILE)
		error ("$*NÃO consegui abrir o arquivo temporário");

	/*
	 *	O Assunto é herdado ...
	 */

#if (0)	/*******************************************************/
	/*
	 *	Coloca a indicação de quem enviou a carta
	 */
	fprintf
	(	fp_tmp,
		">\r\n"
		"> Carta enviada por: %s <%s@%s>\r\n"
		">\r\n"
		"\r\n",
		user_complete_name, local_user_nm, my_name
	);
#endif	/*******************************************************/

	/*
	 *	Copia o cabeçalho, removendo o "Status" e alterando a data
	 */
	for (EVER)
	{
		if (fnrgets (area, sizeof (area), fp_in) != area)
			break;

		if (area[0] == '\0')
			{ fprintf (fp_tmp, "\r\n"); break; }

		/*
		 *	Procura a palavra chave
		 */
		lp = get_keyword (area, token, DATASZ);

		if (token[0] == '\0')
			goto output;

		lp = skip_blanks (lp);

		if (*lp++ != ':')
			goto output;

		key_word = strtoupper (token);

		lp = skip_blanks (lp);

		/*
		 *	Identifica e processa a palavra chave
		 */
		if   (streq (key_word, "STATUS"))
		{
			continue;		/* Ignora */
		}
		elif (streq (key_word, "DATE"))
		{
			time (&data); tm = localtime (&data);
			strftime (area, BLSZ, "%a, %d %b %Y %X %Z", tm);
			fprintf (fp_tmp, "Date: %s\r\n", area);
		}
		else
		{
		    output:
			fprintf (fp_tmp, "%s\r\n", area);
		}

	}	/* end for (EVER) */

	/*
	 *	Coloca a indicação de quem enviou a carta
	 */
	fprintf
	(	fp_tmp,
		">\r\n"
		">	Carta reenviada por: %s <%s>\r\n"
		">\r\n"
		"\r\n",
		user_complete_name, snd_nm
	);

	/*
	 *	Copia o corpo da carta
	 */
	while ((c = getc (fp_in)) >= 0)
		putc (c, fp_tmp);

	fclose (fp_in); fclose (fp_tmp);

	return (text_nm);

}	/* end copy_text_file */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	va_list			args;
	const char		*fmt = format;
	char			err, abrt;
	int			event, save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$Memória esgotada";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (global_tcp_fd);

			fprintf (stderr, " (%s)", t_strevent (global_tcp_fd, event));
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	fprintf (stderr, "\r\n");

	if (abrt == '$')
		quit (1);

	va_end (args);

}	/* end error */

/*
 ****************************************************************
 *	Se receber um sinal, ...				*
 ****************************************************************
 */
void
snd_on_signal (int signo, ...)
{
	quit (1);

}	/* end snd_on_signal */

/*
 ****************************************************************
 *	Restaura o estado do terminal e termina			*
 ****************************************************************
 */
void
quit (int code)
{
	if (text_file_nm != NOSTR)
		unlink (text_file_nm);

	exit (code);

}	/* end quit */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - recebe ou envia cartas do correio eletrônico\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-vea] [-f <arquivo>] <destinatário> ...\n"
		"\t%s [-rsui] [-d <caixa postal>]\n",
		pgname, pgversion, pgname, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\t-e: Oferece o <arquivo> para edição através de \"vi\"\n"
		"\t-f: O texto da carta está no <arquivo>\n"
		"\t-r: Não modifica a caixa postal\n"
		"\t-s: Lista o tamanho das cartas ao invés da data\n"
		"\t-u: Mostre apenas as cartas não lidas\n"
		"\t-d: Abre a <caixa postal> dada, ao invés da caixa postal do sistema\n"
		"\t-i: Atualiza o arquivo índice de cartas\n"
		"\t-a: Envia a todos os servidores disponíveis\n"
		"\nObs.:\tSe não for dado nenhum <destinatário>, abre a caixa postal\n"
	);

	exit (2);

}	/* end help */
