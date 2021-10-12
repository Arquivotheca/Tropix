/*
 ****************************************************************
 *								*
 *			mbox.c					*
 *								*
 *	Lê as cartas da caixa postal				*
 *								*
 *	Versão	3.0.0, de 16.12.97				*
 *		4.0.0, de 12.08.00				*
 *								*
 *	Módulo: MAIL						*
 *		Utilitários de comunicação			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <termio.h>
#include <terminfo.h>
#include <signal.h>
#include <errno.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define REG_MODE 0600

entry char		*mailbox_ptr;

const char		status_nm[] = "status";
const char		status_new_nm[] = "new_status";

entry int		mailbox_dev;	/* Dispositivo da caixa de correio */
entry int		mailbox_ino;	/* Nó-índice da caixa de correio */
entry time_t		mailbox_time;	/* Última modificação da caixa de correio */

/*
 ****** Dados de "terminfo" *************************************
 */
entry TERMIO		new_termio, old_termio; /* Estado do vídeo */

entry INFO		info;		/* Informações sobre o vídeo */

entry const char	*c_clear,	/* Limpa a tela */
			*c_cup,		/* Endereça o cursor */
			*c_ed,		/* Limpa até o resto da tela */
			*c_el,		/* Limpa até o resto da linha */
			*c_ri,		/* Scroll reverso */
			*c_rev,		/* Reverse video */
			*c_sgr0;	/* Termina um campo */

/*
 ****** Variáveis referentes à tela *****************************
 */
const char		dev_tty_nm[] =  "/dev/tty";

entry FILE		*fp_tty;	/* Teclado */
entry FILE		*fp_out;	/* Tela do vídeo */

entry int		TEXTLINE;	/* No. de linhas de texto */
entry int		LASTLINE;	/* Última linha da tela (começando de 0) */
entry int		LINE_SZ;	/* Número máximo de caracteres por linha */

/*
 ******	Estrutura usada para o arquivo de índice ****************
 */
typedef struct mini	MINI; 

struct mini
{
	char		*m_file_nm;	/* Nome do arquivo */
	char		*m_uidl;	/* UIDL */
	off_t		m_size;		/* Tamanho */
	ino_t		m_ino;		/* o no. do INODE */

	char		m_found;	/* Para conferir */

	MINI		*m_back;	/* Ponteiro para o antecessor da lista */
	MINI		*m_forw;	/* Ponteiro para o sucessor da lista */
};

/*
 ****** Protótipos de funções ***********************************
 */
void		remove_deleted_letters_from_status_file (void);
DIR		*mailbox_data_init (void);
void		mbox_on_signal (int, ...);
void		reset_termio (void);

/*
 ****************************************************************
 *	Lê as cartas da caixa postal				*
 ****************************************************************
 */
void
mbox (void)
{
	TERMIO		*tp;
	DIR		*mailbox_fp;
	const DIRENT	*dp;
	STAT		mailbox_s;
	MDIREC		letter_head;
	MDIREC		*mp, *fp, *bp;
	int		n, index, fd;
	int		bad_letters = 0;
	const char	*str;

	/*
	 *	Abre o vídeo para leituras dos comandos
	 */
	if ((fp_tty = fopen (dev_tty_nm, "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", dev_tty_nm);

	/*
	 *	Abre o vídeo para a saída na tela
	 */
	if ((fp_out = fopen (dev_tty_nm, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", dev_tty_nm);

	/*
	 *	Obtém o TERMIO do vídeo
	 */
	if (ioctl (fileno (fp_tty), TCGETS, tp = &old_termio) < 0)
		error ("$*Não consegui obter o \"termio\" de \"%s\"", dev_tty_nm);

	/*
	 *	Calcula o número de linhas e colunas
	 */
	if (tp->t_nline == 0)
		tp->t_nline = 24;

	if (tp->t_ncol == 0)
		tp->t_ncol = 80;

	TEXTLINE = tp->t_nline - 1;
	LASTLINE = tp->t_nline - 1;
	LINE_SZ  = tp->t_ncol;

	/*
	 *	Obtem as características do vídeo
	 */
	if (getinfo (NOSTR, &info) == 0)
		exit (1);

	/*
	 *	Obtém ponteiros para as cadeias relevantes de "info"
	 */
	if ((c_clear = info.i_strings[s_clear]) == NOSTR)
		error ("$O vídeo não pode limpar a tela?");

	if ((c_cup = info.i_strings[s_cup]) == NOSTR)
		error ("$O vídeo não pode endereçar o cursor?");

	if ((c_ed = info.i_strings[s_ed]) == NOSTR)
		error ("$O vídeo não pode limpar até o resto da janela?");

	if ((c_el = info.i_strings[s_el]) == NOSTR)
		error ("$O vídeo não pode limpar até o resto da linha?");

	if ((c_ri = info.i_strings[s_ri]) == NOSTR)
		error ("$O vídeo não pode executar um \"scroll\" reverso?");

	if   ((c_rev = info.i_strings[s_rev]) == NOSTR)
		{ c_rev = ""; c_sgr0 = "";	}
	elif ((c_sgr0 = info.i_strings[s_sgr0]) == NOSTR)
		{ c_rev = ""; c_sgr0 = "";	}

	/*
	 *	Põe a entrada do vídeo em RAW
	 */
	tp = &new_termio; *tp = old_termio;

	tp->c_oflag &= ~OPOST;
	tp->c_lflag &= ~(ISIG|ICANON|ECHO);
	tp->t_min = 1;

	if (ioctl (fileno (fp_tty), TCSETNR, tp) < 0)
		error ("$*Não consegui modificar o \"termio\" de \"%s\"", dev_tty_nm);

	signal (SIGTERM, mbox_on_signal);
	signal (SIGINT,  mbox_on_signal);
	atexit (reset_termio);

	/*
	 *	Abre a caixa postal
	 */
	if ((mailbox_fp = mailbox_data_init ()) == NODIR)
		return;

	/*
	 *	Inicializa a lista
	 */
	letter_head.m_forw = &letter_head;
	letter_head.m_back = &letter_head;
	letter_head.m_head = 1;

	/*
	 *	Lê o diretório
	 */
	while ((dp = readdir (mailbox_fp)) != NODIRENT)
	{
		if (dp->d_name[0] == '.')
			continue;

		if (streq (dp->d_name, status_nm))
			continue;

		mp = mem_alloc (sizeof (MDIREC));

		/*
		 *	Preenche alguns dos campos
		 */
	   /***	mp->m_back	= ...;	/* Ponteiro para o antecessor da lista */
	   /***	mp->m_forw	= ...;	/* Ponteiro para o sucessor da lista */

		mp->m_head	= 0;	/* Não é a cabeça da lista */
		mp->m_status	= '\0';	/* Por enquanto sem nenhuma informação */

		mp->m_nm	= mem_alloc (strlen (dp->d_name) + 1); strcpy (mp->m_nm, dp->d_name);
		mp->m_ino	= dp->d_ino;	/* No. do inode do arquivo */

		mp->m_index	= 0;	/* No. da carta */

		mp->m_from_text	= "";	/* Remetente (Texto original) */
		mp->m_from_box	= "";	/* Remetente (somente a caixa postal) */
		mp->m_to_text	= "";	/* Destinatário (Texto original) */
		mp->m_to_box	= "";	/* Destinatário (somente a caixa postal) */
		mp->m_cc_text	= "";	/* Outros destinatários (Texto original) */
		mp->m_date	= "";	/* Data */
		mp->m_subject	= "";	/* Asssunto */

	   /***	mp->m_text	= ...;	/* Texto do cabeçalho */
	   /***	mp->m_len	= ...;	/* Tamanho do texto */

		mp->m_time	= -1;	/* Data convertida */
		mp->m_offset	= 0;	/* Deslocamento do texto */

		memsetl (&mp->m_part_head, 0, sizeof (PART) / sizeof (long));

		/*
		 *	Insere na lista: Ordem ascendente pelo nome
		 */
		for (fp = letter_head.m_back; fp->m_head == 0; fp = fp->m_back)
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

	}	/* end lendo o diretório */

	closedir (mailbox_fp);

	/*
	 *	Verifica se leu alguma carta
	 */
	if (letter_head.m_forw->m_head)
		{ error ("Não há cartas para \"%s\"", local_user_nm); return; }

	/*
	 *	Numera as cartas
	 */
	for (index = 1, mp = letter_head.m_forw; mp->m_head == 0; mp = mp->m_forw)
		mp->m_index = index++;

#ifdef	DEBUG
	/*
	 *	Imprime a lista de arquivos
	 */
	printf ("Foram obtidas %d cartas\r\n", index - 1);

	for (n = 5, mp = letter_head.m_forw; mp->m_head == 0; mp = mp->m_forw)
	{
		if (n-- <= 0)
			break;

		printf ("%6d:  %s\r\n", mp->m_index, mp->m_nm);
	}

	printf ("...........................\r\n");

	for (n = 5, mp = letter_head.m_back; mp->m_head == 0; mp = mp->m_back)
	{
		if (n-- <= 0)
			break;

		printf ("%6d:  %s\r\n", mp->m_index, mp->m_nm);
	}
#endif	DEBUG

#undef	LIST
#ifdef	LIST
	/*
	 *	Gera uma lista de endereços
	 */
	for (mp = letter_head.m_forw; mp->m_head == 0; mp = mp->m_forw)
	{
		get_letter_info (mp);
	}

	exit (0);
#endif	LIST

	/*
	 *	Procura o estado de algumas ou todas as cartas, ...
	 */
	if (uflag)
	{
		/* Procura cartas não lidas */

		for (mp = letter_head.m_forw; mp->m_head == 0; mp = mp->m_forw)
		{
			if   ((str = get_letter_info (mp)) != NOSTR)
			{
				error (str, mp->m_nm);
				bad_letters++;
			}
			elif (mp->m_status != 'U')
			{
				/* Retira da lista */

				mp->m_back->m_forw = mp->m_forw;
				mp->m_forw->m_back = mp->m_back;
			}
		}

		if (letter_head.m_forw == &letter_head)
			{ error ("Todas as cartas para \"%s\" já foram lidas", local_user_nm); return; }
	}
	else
	{
		/* Lê no máximo TEXLINE cartas, coletando informações */

		for (n = TEXTLINE, mp = letter_head.m_back; mp->m_head == 0; mp = mp->m_back)
		{
			if (n-- <= 0)
				break;

			if ((str = get_letter_info (mp)) != NOSTR)
			{
				error (str, mp->m_nm);
				bad_letters++;
			}
		}
	}

	if (bad_letters)
		getchar ();

#ifdef	DEBUG
	/*
	 *	Imprime a lista de arquivos
	 */
	for (n = TEXTLINE, mp = letter_head.m_back; mp->m_head == 0; mp = mp->m_back)
	{
		if (n-- <= 0)
			break;

		printf ("%s\r\n", mp->m_text);
	}
#endif	DEBUG
#undef	DEBUG

	/*
	 *	Finalmente, mostra a lista
	 */
	show_direc (&letter_head, 0);

	/*
	 *	As partes abaixo não são executadas se foi dado o nome da caixa postal
	 */
	if (dflag)
		return;

	/*
	 *	Verifica se a caixa postal foi modificada
	 */
	if   (instat (mailbox_dev, mailbox_ino, &mailbox_s) < 0)
		error ("*Não consegui obter o estado da caixa postal \"%s\"", mailbox_nm);
	elif (new_letters || mailbox_time < mailbox_s.st_mtime)
		printf ("Chegou correspondência nova para você\r\n");

	/*
	 *	Remove as linhas de arquivos removidos, se for o caso
	 */
	if (rflag)
		return;

	if (delete_head.m_forw != &delete_head)
	{
		remove_deleted_letters_from_status_file (); return;
	}

	/*
	 *	Muda o estado do arquivo "status"
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((fd = open (mailbox_nm, O_RDWR)) < 0)
	{
		if (errno != EROFS)
			error ("*Não consegui abrir o arquivo \"%s\"", mailbox_nm);

		return;
	}

	if (write (fd, "R", 1) != 1)
		error ("*Não consegui escrever no arquivo \"%s\"", mailbox_nm);

	close (fd);

}	/* end mbox */

/*
 ****************************************************************
 *	Coleta informações sobre uma carta			*
 ****************************************************************
 */
const char *
get_letter_info (MDIREC *mp)
{
	int		fd;
	FILE		*letter_fp;
	STAT		s;
	const char	*lp;
	char		area[DATASZ], token[DATASZ];
	char		*key_word;
	const char	*msg = NOSTR;
	int		n, color_chars = 0, from_len, date_len;
	char		*dst, *src;
	int		local_user_nm_len = strlen (local_user_nm);

	/*
	 *	Lê as cartas, coletando informações
	 */
	mp->m_status = '!';	/* Indica que já tentou ler a carta */

	if
	(	(fd = inopen (mailbox_dev, mp->m_ino)) < 0 ||
		(letter_fp = fdopen (fd, "r")) == NOFILE
	)
	{
		return ("*Não consegui abrir a carta \"%s\"");
	}

	if (sflag && fstat (fd, &s) < 0)
		return ("*Não consegui obter o estado da carta \"%s\"");

	/*
	 ******	Le e analisa as linhas do cabeçalho da carta ****
	 */
	for (EVER)
	{
		if (funrgets (area, DATASZ, letter_fp) == NOSTR)
			break;

		if (area[0] == '\0')
			{ mp->m_offset = ftell (letter_fp); break; }

		/*
		 *	Procura a palavra chave
		 */
		lp = get_keyword (area, token, DATASZ);

		if (token[0] == '\0')
			continue;

		lp = skip_blanks (lp);

		if (*lp++ != ':')
			continue;

		key_word = strtoupper (token);

		lp = skip_blanks (lp);

		/*
		 *	Identifica e processa a palavra chave
		 */
		if   (streq (key_word, "STATUS"))
		{
			if ((mp->m_status = lp[0]) != 'U' && mp->m_status != 'R')
				mp->m_status = '?';

			if (ftell (letter_fp) != 10)	/* sizeof ("status: U\n") */
				msg = "Carta \"%s\" com \"Status\" em posição inválida";
		}
		elif (streq (key_word, "FROM"))
		{
			mp->m_from_text = alloc_and_copy (lp);
			get_mailbox (lp, token, DATASZ);
			mp->m_from_box = alloc_and_copy (token);
		}
		elif (streq (key_word, "TO"))
		{
			mp->m_to_text = alloc_and_copy (lp);
			get_mailbox (lp, token, DATASZ);
			mp->m_to_box = alloc_and_copy (token);
		}
		elif (streq (key_word, "CC"))
		{
			mp->m_cc_text = alloc_and_copy (lp);
		}
		elif (streq (key_word, "DATE"))
		{
			mp->m_date = alloc_and_copy (lp);
			mp->m_time = get_date (lp);
		}
		elif (streq (key_word, "SUBJECT"))
		{
			mp->m_subject = alloc_and_copy (lp);
		}
		elif (streq (key_word, "CONTENT-TYPE"))
		{
			for (EVER)
			{
				lp = get_keyword (lp, token, sizeof (token));

				if (token[0] == '\0')
					break;

				strtoupper (token);

				if   (streq (token, "BOUNDARY") && *lp == '=')
				{
					lp = get_value (++lp, token, sizeof (token));

					mp->m_part_head.p_boundary = alloc_and_copy (strtoupper (token));
				}
#if (0)	/*******************************************************/
				elif (streq (token, "CHARSET") && *lp == '=')
				{
					lp = get_value (++lp, token, sizeof (token));

					if (strncmp (strtoupper (token), "ISO", 3))
						mp->m_part_head.p_quoted = 1;
				}
#endif	/*******************************************************/

				lp = skip_to_sm (lp);

				if (*lp++ != ';')
					break;
			}
		}
		elif (streq (key_word, "CONTENT-TRANSFER-ENCODING"))
		{
			lp = get_keyword (lp, token, DATASZ);

			if (streq (strtoupper (token), "QUOTED-PRINTABLE"))
				mp->m_part_head.p_quoted = 1;
		}

	}	/* end for (EVER) */

	fclose (letter_fp);

	/*
	 ******	Calcula os tamanhos dos campos ******************
	 */
	from_len = LINE_SZ / 2 - 20;

	if (from_len > 30)
		from_len = 30;

	if (LINE_SZ > 100)
		date_len = 22;
	else
		date_len = 16;

	/* Cria o texto da linha completo, para "show" */

	area[STATUS_COL] = mp->m_status;
	area[1] = ' ';
	sprintf (area + INDEX_COL, "%4d ", mp->m_index);

	/* Coloca o remetente ou destinatário */

	dst = mp->m_from_box;

	if
	(	strncmp (dst, local_user_nm, local_user_nm_len) ||
		*mp->m_to_box == '\0'
	)
	{
		sprintf (area + BEGIN_COL, "From: ");
	}
	else
	{
		dst = mp->m_to_box;
		sprintf (area + BEGIN_COL, "To:   ");
	}

#ifdef	LIST
	printf ("%s\r\n", dst);
#endif	LIST

	src = area + BEGIN_COL + 6;
	sprintf (src, "%-*.*s    ", from_len, from_len, dst);

	src += from_len + 2;

	/*
	 *	Coloca a data ou tamanho (com as cores)
	 */
	if   (!sflag)
	{
		sprintf (src, "%-*.*s    ", date_len, date_len, scan_date (mp));
	}
	elif (s.st_size < 100 * 1024)
	{
		sprintf (src, "%*d    ", date_len, s.st_size);
	}
	else /* (sflag && s.st_size >= 100 * 1024) */
	{
		sprintf (src, "\e[34m%*d\e[0m    ", date_len, s.st_size); color_chars = 9; src += 9;
	}

	src += date_len + 2;

	sprintf (src, "%s", mp->m_subject);

	/* Cria a linha */

	n = strlen (area);

	if (n > LINE_SZ + color_chars)
		n = LINE_SZ + color_chars;

	mp->m_text = mem_alloc (n + 1);

	mp->m_len  = n - color_chars;		/* Os "color_chars" não ocupam espaço */

	strncpy (mp->m_text, area, n); mp->m_text[n] = '\0';

	return (msg);

}	/* end get_letter_info */

/*
 ****************************************************************
 *	Atualiza o arquivo de índice "status"			*
 ****************************************************************
 */
void
update_status_file (void)
{
	DIR		*mailbox_fp;
	STAT		letter_s;
	int		letter_fd, status_fd;
	int		bad = 0, status_count = 0, dir_count = 0;
	FILE		*letter_fp, *status_fp;
	char		status_char = 'R', *cp;
	const char	*str;
	const DIRENT	*dp;
	MINI		mini_head, *mp, *bp;
	char		area[120], uidl[64];

	/*
	 *	Abre a caixa postal
	 */
	if ((mailbox_fp = mailbox_data_init ()) == NODIR)
		return;

	mini_head.m_forw = mini_head.m_back = &mini_head; 

	/*
	 *	Lê o diretório
	 */
	while ((dp = readdir (mailbox_fp)) != NODIRENT)
	{
		if (dp->d_name[0] ==  '.')
			continue;

		if (streq (dp->d_name, status_nm))
			continue;

		strcpy (uidl, dp->d_name);	/* Em princípio */

		letter_fd = -1;

		if
		(	(letter_fd = inopen (mailbox_dev, dp->d_ino)) < 0 ||
			fstat (letter_fd, &letter_s) < 0 ||
			(letter_fp = fdopen (letter_fd, "r")) == NOFILE
		)
		{
			error ("*Não consegui abrir a carta \"%s\"", dp->d_name);
			close (letter_fd);
			continue;
		}

		if (letter_s.st_size == 0)
		{
			fprintf (stderr, "Arquivo da carta \"%s\" vazio, remove? (n): ", dp->d_name);

			if (askyesno () > 0)
			{
				strcpy (mailbox_ptr, dp->d_name);

				if (unlink (mailbox_nm) < 0)
					error ("*Não consegui remover \"%s\"", mailbox_nm);
			}

			fclose (letter_fp);
			continue;
		}

		for (EVER)
		{
			if (fnrgets (area, sizeof (area), letter_fp) == NOSTR)
			{
				error ("*Erro na leitura da carta local \"%s\"", dp->d_name);
				break;
			}

			cp = area;

			/* Cuida das linhas começando por "." */

			if   (area[0] == '\0')
			{
				break;
			}
			elif (area[0] == '.')
			{
				if (area[1] == '\0')
					break;

				if (area[1] == '.')
					cp++;
			}

			if (cp[4] != ':')
				continue;

			cp[4] = '\0'; strtoupper (area);

			if (!streq (cp, "UIDL"))
				continue;

			cp += 5;	/* Pula o ":" */

			while (*cp == ' ' || *cp == '\t')
				cp++;

			strcpy (uidl, cp);

			break;
		}

		/*
		 *	Preenche os campos
		 */
		mp = alloca (sizeof (MINI));

		mp->m_file_nm = alloca (strlen (dp->d_name) + 1); strcpy (mp->m_file_nm, dp->d_name);
		mp->m_uidl    = alloca (strlen (uidl) + 1); strcpy (mp->m_uidl, uidl);
		mp->m_size    = letter_s.st_size;
		mp->m_ino     = dp->d_ino;

		/*
		 *	Coloca esta carta na lista
		 */
		bp = mini_head.m_back;

		mini_head.m_back = mp;  mp->m_forw = &mini_head;
		bp->m_forw = mp;	mp->m_back = bp;

		fclose (letter_fp);

		dir_count++;

	}	/* Lendo a caixa postal */

	closedir (mailbox_fp);

	/*
	 *	Le o arquivo de índices "status" e confere
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((status_fp = fopen (mailbox_nm, "r")) == NOFILE)
		{ error ("*Não consegui abrir o arquivo \"%s\"", mailbox_nm); goto creat; }

	status_char = fgetc (status_fp); fgetc (status_fp); 

	for (mp = mini_head.m_forw; mp != &mini_head; mp = mp->m_forw)
		mp->m_found = 0;

	while (fngets (area, sizeof (area), status_fp) != NOSTR)
	{
		if ((str = strfield (area, ':')) == NOSTR)
			{ bad = 1; break; }

		for (mp = mini_head.m_forw; mp != &mini_head; mp = mp->m_forw)
		{
			if (mp == &mini_head)
				{ bad = 2; break; }

			if (streq (mp->m_file_nm, str))
				break;
		}

		if (bad)
			break;

		if ((str = strfield (NOSTR, ':')) == NOSTR)
			{ bad = 3; break; }

		if (!streq (mp->m_uidl, str))
			{ bad = 4; break; }

		if ((str = strfield (NOSTR, ':')) == NOSTR)
			{ bad = 5; break; }

		if (mp->m_size != atol (str))
			{ bad = 6; break; }

		if ((str = strfield (NOSTR, ':')) == NOSTR)
			{ bad = 7; break; }

		if (mp->m_ino != atol (str))
			{ bad = 8; break; }

		status_count++; mp->m_found++;

	}	/* lendo as linhas do arquivo */

	fclose (status_fp);

	/*
	 *	Verifica se todos foram referenciados
	 */
	if (status_count != dir_count)
		bad = 9;

	if (!bad) for (mp = mini_head.m_forw; mp != &mini_head; mp = mp->m_forw)
	{
		if (!mp->m_found)
			{ bad = 10; break; }
	}

	if (!bad)
		{ printf ("Arquivo de índices correto\n"); return; }

	/*
	 *	Escreve o arquivo de índices "status"
	 */
    creat:
	fprintf (stderr, "Arquivo de índices incorreto, atualiza? (n): ");

	if (askyesno () <= 0)
		return;

	printf ("Atualizando, ...\n");

   /***	strcpy (mailbox_ptr, status_nm); ***/

	if
	(	(status_fd = creat (mailbox_nm, REG_MODE)) < 0 ||
		(status_fp = fdopen (status_fd, "w")) == NOFILE
	)
		error ("$*Não consegui abrir o arquivo \"%s\"", status_nm);

	fprintf (status_fp, "%c\n", status_char);

	for (mp = mini_head.m_forw; mp != &mini_head; mp = mp->m_forw)
	{
		fprintf (status_fp, "%s:%s:%d:%d\n", mp->m_file_nm, mp->m_uidl, mp->m_size, mp->m_ino);
	}

	fclose (status_fp);

}	/* end update_status_file */

/*
 ****************************************************************
 *   Retira as cartas removidas do arquivo de índice "status"	*
 ****************************************************************
 */
void
remove_deleted_letters_from_status_file (void)
{
	int		new_status_fd;
	char		*new_nm;
	const char	*file_nm, *uidl, *size_str, *ino_str;
	FILE		*status_fp, *new_status_fp;
	const MDIREC	*mp;
	int		bad = 0;
	char		area[120];

	/*
	 *	Obtém o nome do novo arquivo de "status"
	 */
	strcpy (mailbox_ptr, status_new_nm);

	new_nm = alloca (strlen (mailbox_nm) + 1); strcpy (new_nm, mailbox_nm);

	/*
	 *	Abre o arquivo de "status"
	 */
	strcpy (mailbox_ptr, status_nm);

	if ((status_fp = fopen (mailbox_nm, "r")) == NOFILE)
		{ error ("*Não consegui abrir o arquivo \"%s\"", mailbox_nm); return; }

	fseek (status_fp, 2, SEEK_SET);

	/*
	 *	Cria o novo arquivo de estado
	 */
	if
	(	(new_status_fd = creat (new_nm, REG_MODE)) < 0 ||
		(new_status_fp = fdopen (new_status_fd, "w")) == NOFILE
	)
	{
		error ("*Não consegui criar o arquivo \"%s\"", mailbox_nm); return;
	}

	fputc ('R', new_status_fp); fputc ('\n', new_status_fp);

	/*
	 *	Lê e escreve os arquivos de estado
	 */
	while (fngets (area, sizeof (area), status_fp) != NOSTR)
	{
		if ((file_nm = strfield (area, ':')) == NOSTR)
			{ bad++; continue; }

		if ((uidl = strfield (NOSTR, ':')) == NOSTR)
			{ bad++; continue; }

		if ((size_str = strfield (NOSTR, ':')) == NOSTR)
			{ bad++; continue; }

		if ((ino_str = strfield (NOSTR, ':')) == NOSTR)
			{ bad++; continue; }

		for (mp = delete_head.m_forw; /* abaixo */; mp = mp->m_forw)
		{
			if (mp->m_head)
			{
				fprintf (new_status_fp, "%s:%s:%s:%s\n", file_nm, uidl, size_str, ino_str);
				break; 
			}

			if (streq (mp->m_nm, file_nm))
				break;
		}

	}	/* lendo e escrevendo as linhas dos arquivos */

	fclose (status_fp); fclose (new_status_fp);

	if (bad)
		error ("Erro no formato do arquivo \"%s\"", mailbox_nm);

	/*
	 *	Acerta os nomes
	 */
	if (unlink (mailbox_nm) < 0)
		error ("*Não consegui remover o arquivo \"%s\"", mailbox_nm);

	if (rename (new_nm, mailbox_nm) < 0)
		error ("*Não consegui trocar de nome o arquivo \"%s\"", mailbox_nm);

}	/* end remove_deleted_letters_from_status_file */

/*
 ****************************************************************
 *	Prepara os dados da caixa postal			*
 ****************************************************************
 */
DIR *
mailbox_data_init (void)
{
	STAT		mailbox_s;
	DIR		*mailbox_fp;
	static int	cat_done;

	/*
	 *	Prepara o nome
	 */
	if (mailbox_ino == 0)
	{
		if (!dflag && !cat_done)
			{ strcat (mailbox_nm, local_user_nm); cat_done++; }

		if (stat (mailbox_nm, &mailbox_s) < 0)
		{
			error ("*Não consegui obter o estado da caixa postal \"%s\"", mailbox_nm);
			return (NODIR);
		}

		strcat (mailbox_nm, "/");
		mailbox_ptr = strend (mailbox_nm);

		mailbox_dev  = mailbox_s.st_dev;
		mailbox_ino  = mailbox_s.st_ino;
		mailbox_time = mailbox_s.st_mtime;

#undef	DEBUG
#ifdef	DEBUG
		printf ("mailbox_nm = \"%s\"\r\n", mailbox_nm);
#endif	DEBUG
	}

	/*
	 *	Abre a caixa
	 */
	if ((mailbox_fp = opendir (mailbox_nm)) == NODIR)
	{
		if (!dflag && errno == ENOENT)
			error ("Não há cartas para \"%s\"", local_user_nm);
		else
			error ("*Não consegui abrir a caixa postal \"%s\"", mailbox_nm);

		return (NODIR);
	}

	return (mailbox_fp);

}	/* end mailbox_data_init */

/*
 ****************************************************************
 *	Restaura o estado do vídeo				*
 ****************************************************************
 */
void
mbox_on_signal (int signo, ...)
{
	fputs (parmexec (c_cup, LASTLINE, 0), fp_out); fflush (fp_out);
	error ("$Recebi o sinal \"%s\", terminando ...", sigtostr (signo));

}	/* end mbox_on_signal */

/*
 ****************************************************************
 *	Restaura o termio					*
 ****************************************************************
 */
void
reset_termio (void)
{
	ioctl (fileno (fp_tty), TCSETNR, &old_termio);

}	/* end reset_termio */
