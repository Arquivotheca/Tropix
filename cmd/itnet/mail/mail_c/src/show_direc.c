/*
 ****************************************************************
 *								*
 *			show_direc.c				*
 *								*
 *	Mostra uma carta/lista de cartas na tela do vídeo	*
 *								*
 *	Versão	3.0.0, de 19.12.97				*
 *		4.0.0, de 28.03.01				*
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
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminfo.h>
#include <termio.h>
#include <time.h>
#include <signal.h>

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry int	new_letters;	/* Indica recebimento de novas cartas */

extern TERMIO	new_termio, old_termio; /* Estado do vídeo */

/*
 ****** Protótipos de funções ***********************************
 */
void		write_one_line (MDIREC *);
int		rm_letter (MBOX *cb);
char		*convert_quoted (const char *, char *);
int		convert_and_show (MDIREC *, int);
int		move_or_copy_letter (int, char *, const MDIREC *);
int		get_part_info_and_show (MDIREC *);
int		get_text_sz (const MDIREC *, const MDIREC *);

/*
 ****************************************************************
 *	Mostra um arquivo					*
 ****************************************************************
 */
int
show_direc (MDIREC *list_head, int mode)
{
	int		i, n = 0;
	MDIREC		*lp, *cursor_mp;
	MBOX		cb;
	char		c, *cp;
	int		cursor, new_cursor;
	char		read_area[80], pattern_area[80];

	/*
	 *	Lista uma tela
	 */
	cb.b_screen_last_line = NOMDIREC;
	cb.b_msg = 0;
	cb.b_next_cmd = 0;

	pattern_area[0] = '\0';

	list_head->m_text = "*** Cabeça da Lista ***";
	list_head->m_len  = strlen (list_head->m_text);

	fputs (c_clear, fp_out);

	cb.b_cursor_mp = cb.b_screen_first_line = list_head->m_forw;

	/*
	 *	Procura a primeira carta com estado "U"
	 */
	for (EVER)
	{
		if (cb.b_screen_first_line->m_head)
			{ cb.b_screen_first_line = NOMDIREC; break; }

		if (cb.b_screen_first_line->m_status == 'U')
		{
			cb.b_cursor_mp = cb.b_screen_first_line;

			for (i = TEXTLINE/2; i > 0; i--)
			{
				if (cb.b_screen_first_line->m_back->m_head)
					break;

				cb.b_screen_first_line = cb.b_screen_first_line->m_back;
			}

			break;
		}

		cb.b_screen_first_line = cb.b_screen_first_line->m_forw;
	}

	/* Se NÃO tem carta com estado "U", vai para o final */

	if (cb.b_screen_first_line == NOMDIREC)
	{
		cb.b_cursor_mp = list_head->m_back;

		cb.b_screen_first_line = list_head->m_back;

		for (i = TEXTLINE - 1; i > 0; i--)
		{
			if (cb.b_screen_first_line->m_back->m_head)
				break;

			cb.b_screen_first_line = cb.b_screen_first_line->m_back;
		}
	}

	/*
	 *	Escreve uma tela
	 */
	i = TEXTLINE;

	for (lp = cb.b_screen_first_line; lp->m_head == 0; lp = lp->m_forw)
	{
		write_one_line (lp); cb.b_screen_last_line = lp;

		if (--i <= 0)
			break;
	}

	/*
	 *	Malha de leitura e execução dos comandos
	 */
	for (EVER)
	{
		/*
		 *	Escreve o texto ainda pendente
		 */
		fflush (fp_out);

		/*
		 *	Reescreve a última linha, se for o caso
		 */
		if (cb.b_msg)
		{
			fprintf
			(	fp_out, "%s%s",
				parmexec (c_cup, LASTLINE, 0), c_el
			);

			if (cb.b_next_cmd == 0)
				cb.b_next_cmd = getc (fp_tty);

			for (i = 0, lp = cb.b_screen_first_line; lp->m_head == 0; lp = lp->m_forw, i++)
			{
				if (i == LASTLINE - 1)
				{
					fprintf (fp_out, "%s", parmexec (c_cup, LASTLINE - 1, 0));
					write_one_line (lp);
					fputs (c_ed, fp_out);
					break;
				}
			}

			cb.b_msg = 0;
		}

		/*
		 *	Posiciona o cursor
		 */
		for (cursor = 0, lp = cb.b_screen_first_line; /* abaixo */; lp = lp->m_forw, cursor++)
		{
			if (lp == cb.b_cursor_mp)
				break;

			if (lp == cb.b_screen_last_line)
			{
				if (cb.b_cursor_mp->m_index < cb.b_screen_first_line->m_index)
					{ cb.b_cursor_mp = cb.b_screen_first_line; cursor = 0; }
				else
					  cb.b_cursor_mp = cb.b_screen_last_line;

				break;
			}
		}

		fprintf (fp_out, "%s%s", parmexec (c_cup, cursor, 0), c_rev);
		write_one_line (lp);
		fprintf (fp_out, "%s", c_sgr0);

		fputs (parmexec (c_cup, cursor, LINE_SZ), fp_out); fflush (fp_out);

		/*
		 *	Analisa o comando
		 */
		if ((cb.b_cmd = cb.b_next_cmd) == 0)
			cb.b_cmd = getc (fp_tty);

		cb.b_next_cmd = 0;

		switch  (cb.b_cmd)
		{
			/*
			 ******	Encerra ******************************************************
			 */
		    case CTLC:
		    case 'q':
		    case_q:
		    case 'Q':
			fprintf (fp_out, "%s%s", parmexec (c_cup, LASTLINE, 0), c_ed); fflush (fp_out);
			return (0);

			/*
			 ******	Branco: Nada faz *********************************************
			 */
		    case ' ':
			n = 0;
			break;

			/*
			 ******	Reescreve a tela *********************************************
			 */
		    case CTLR:
		    clear_and_write_page:
			if (cb.b_screen_first_line->m_head)
				goto case_q;
#undef	CLEAR
#ifdef	CLEAR
			fputs (c_clear, fp_out);
#else
			fputs (parmexec (c_cup, 0, 0), fp_out);
#endif	CLEAR
			i = TEXTLINE;

			for (lp = cb.b_screen_first_line; lp->m_head == 0; lp = lp->m_forw)
			{
				write_one_line (lp); cb.b_screen_last_line = lp;

				if (--i <= 0)
					break;
			}
#ifndef	CLEAR
			fputs (c_ed, fp_out);
#endif	CLEAR
			n = 0; break;

			/*
			 ******	Avança <n> linhas ********************************************
			 */
		    case '+':
		    case '=':
		    case 'j':
		    case CTLE:
		    case_plus_line:
			if (n == 0)
				n = 1;

			cursor_mp = cb.b_cursor_mp;

			for (i = 0; i < n; i++)
			{
				if ((lp = cb.b_cursor_mp->m_forw)->m_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor + i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			write_one_line (cursor_mp);

			if   (new_cursor > TEXTLINE + TEXTLINE/2)
			{
				cb.b_screen_first_line = cb.b_cursor_mp;

				goto clear_and_write_page;
			}
			elif (new_cursor < TEXTLINE)
			{
				break;
			}

			/* Cursor novo logo abaixo da janela */

			for (i = new_cursor - TEXTLINE + 1; i > 0; i--)
			{
				cb.b_screen_first_line = cb.b_screen_first_line->m_forw;
				cb.b_screen_last_line  = cb.b_screen_last_line->m_forw;

				fprintf (fp_out, "%s\n", parmexec (c_cup, LASTLINE, 0));
				fputs (parmexec (c_cup, TEXTLINE - 1, 0), fp_out);
				write_one_line (cb.b_screen_last_line);
			}

			break;

			/*
			 ******	Retrocede <n> linhas *****************************************
			 */
		    case '-':
		    case '_':
		    case 0x1D:
		    case 'k':
		    case CTLY:
		    case_minus_line:
			if (n == 0)
				n = 1;

			cursor_mp = cb.b_cursor_mp;

			for (i = 0; i < n; i++)
			{
				if ((lp = cb.b_cursor_mp->m_back)->m_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor - i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			write_one_line (cursor_mp);

			if   (new_cursor < -TEXTLINE/2)
			{
				cb.b_screen_first_line = cb.b_cursor_mp;

				goto clear_and_write_page;
			}
			elif (new_cursor >= 0)
			{
				break;
			}

			/* Cursor novo logo acima da janela */

			for (i = -new_cursor; i > 0; i--)
			{
				cb.b_screen_first_line = cb.b_screen_first_line->m_back;
				cb.b_screen_last_line  = cb.b_screen_last_line->m_back;

				fprintf (fp_out, "%s%s", parmexec (c_cup, 0, 0), c_ri);
				write_one_line (cb.b_screen_first_line);
			}

			break;

			/*
			 ******	Avança meia página *******************************************
			 */
		    case CTLD:
			n = (TEXTLINE + 1)/2;
			goto case_plus_line;

			/*
			 ******	Retrocede meia página ****************************************
			 */
		    case CTLU:
			n = (TEXTLINE + 1)/2;
			goto case_minus_line;

			/*
			 ******	Avança <n> telas *********************************************
			 */
		    case CTLF:
			i = TEXTLINE;

			if (n == 0)
				n = i;
			else
				n *= i;

			for (i = 0; i < n; i++)
			{
				if ((lp = cb.b_cursor_mp->m_forw)->m_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			cb.b_screen_first_line = cb.b_cursor_mp;
			goto clear_and_write_page;

			/*
			 ******	Retrocede <n> telas ******************************************
			 */
		    case CTLB:
			i = TEXTLINE;

			if (n == 0)
				n = i;
			else
				n *= i;

			for (i = 0; i < n; i++)
			{
				if ((lp = cb.b_cursor_mp->m_back)->m_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			cb.b_screen_first_line = cb.b_cursor_mp;
			goto clear_and_write_page;

			/*
			 ******	Posiciona dada a linha ***************************************
			 */
		    case 'g':
		    case 'G':
		    case_g:
			if   (n == 0)			/* Última página */
			{
			    last_page:
				cb.b_cursor_mp = cb.b_screen_first_line = list_head->m_back;

				for (i = TEXTLINE - 1; i > 0; i--)
				{
					if (cb.b_screen_first_line->m_head)
						break;

					cb.b_screen_first_line = cb.b_screen_first_line->m_back;
				}

				if (cb.b_screen_first_line->m_head)
					cb.b_screen_first_line = list_head->m_forw;

				goto clear_and_write_page;
			}
			elif (n == 1)			/* Primeira página */
			{
				cb.b_cursor_mp = cb.b_screen_first_line = list_head->m_forw;

				goto clear_and_write_page;
			}
			elif ((unsigned)n > list_head->m_back->m_index) /* Linha inválida */
			{
				goto last_page;
			}

			/* Procura a linha de índice dado */

			for (lp = list_head->m_forw; lp->m_index != n; lp = lp->m_forw)
			{
				if (lp->m_head)	/* Linha inexistente */
					{ putc ('\a', fp_out); lp = cb.b_cursor_mp; break; }
			}

			if   (lp == cb.b_cursor_mp)
				{ n = 0; break; }

			if (lp->m_index > cb.b_cursor_mp->m_index)
			{
				for (n = 0; /* abaixo */; n++, lp = lp->m_back)
				{
					if (lp->m_back->m_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_plus_line;
			}
			else	/* lp->m_index < cb.b_cursor_mp->m_index */
			{
				for (n = 0; /* abaixo */; n++, lp = lp->m_forw)
				{
					if (lp->m_forw->m_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_minus_line;
			}

			/*
			 ******	Mostra uma carta *********************************************
			 */
		    case '\n':
		    case '\r':
		    case 'o':
		    case 'u':
		    case_n:
			n = 0;

			if (cb.b_cursor_mp->m_head)
				goto case_q;

			for (EVER) switch (read_letter (cb.b_cursor_mp))
			{
			    case NODE_BACK:
				if (cb.b_cursor_mp->m_back->m_head == 0)
					cb.b_cursor_mp = cb.b_cursor_mp->m_back;
				else
					putc ('\a', fp_out);

				continue;

			    case NODE_FORW:
				if (cb.b_cursor_mp->m_forw->m_head == 0)
					cb.b_cursor_mp = cb.b_cursor_mp->m_forw;
				else
					putc ('\a', fp_out);

				continue;

			    case NODE_UP:
			    default:
				goto clear_and_write_page;

			    case FORWARD:
				goto case_f;

			/** case REPLY: **/

			    case DELETE:
				if (rm_letter (&cb) >= 0)
					goto case_n;

				continue;

			    case QUIT:
				goto case_q;
			}

			break;

			/*
			 ****** Envia uma carta para outro destinatário **********************
			 */
		    case 'f':
		    case_f:
		    {
			const char	**vv, **vp;
			int		pid;

			n = 0;

			fprintf (fp_out, "%s%s: f ", parmexec (c_cup, LASTLINE, 0), c_ed); fflush (fp_out);

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			fnrgets (read_area, sizeof (read_area), fp_tty);

			if (read_area[0] == '\0')
			{
				ioctl (fileno (fp_tty), TCSETNR, &new_termio);
				fprintf
				(	fp_out, "%s%s\n",
					parmexec (c_cup, LASTLINE, 0), c_ed
				);
				break;
			}

			strcpy (mailbox_ptr, cb.b_cursor_mp->m_nm);

			/*
			 *	Invoca o programa "send_mail"
			 */
			vv = vp = alloca (6 * sizeof (char *));

			*vp++ = send_mail_arg0;			/* 0: send_mail */

			if (vflag)
				*vp++ = "-v";			/* 1: [-v] */

			*vp++ = copy_text_file (mailbox_nm);	/* 2: Nome do arquivo da carta */

			*vp++ = snd_nm;				/* 3: Remetente */

			*vp++ = read_area;			/* 4: Destinatário */

			*vp = NOSTR;				/* 5: NOSTR */
#undef	SEND_MAIL_DEBUG
#ifdef	SEND_MAIL_DEBUG
			fprintf (stderr, "SEND_MAIL: ");

			for (vp = vv; *vp != NOSTR; vp++)
				fprintf (stderr, "%s ", *vp);

			fprintf (stderr, "\n");
#else
			if ((pid = fork ()) < 0)
				error ("$*Não consegui criar um processo");

			if (pid == 0)
			{
				signal (SIGINT,  SIG_IGN);
				signal (SIGQUIT, SIG_IGN);

				execv (send_mail_nm, vv);
				error ("$*Não consegui executar \"%s\"", send_mail_nm);

				return (0);	/* Nunca executado */
			}
#endif	SEND_MAIL_DEBUG

			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

			fprintf (fp_out, "\r\nTecle para continuar"); fflush (fp_out);

			while ((c = getc (fp_tty)) == CTLS || c == CTLQ)
				/* vazio */;

			goto clear_and_write_page;
		    }

#if (0)	/*******************************************************/
	char		read_area[80], pattern_area[80], edit_area[120];
			sprintf
			(	edit_area,
				"mail -f %s %s",
				mailbox_nm, read_area
			);

			fprintf (fp_out, "%s\n", edit_area);

			system (edit_area);
#endif	/*******************************************************/

			/*
			 ******	Remove uma carta *********************************************
			 */
		    case 'd':
			n = 0;

			if (rm_letter (&cb) >= 0)
				goto clear_and_write_page;

			break;

			/*
			 ******	Um número ****************************************************
			 */
		    case '0':
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			n = (n * 10) + (cb.b_cmd - '0');
			break;

			/*
			 ******	Escape *******************************************************
			 */
		    case '\e':
			if (getc (fp_tty) != '[')
				goto case_default;

			switch (getc (fp_tty))
			{
			    case 'A':	/* Seta para cima */
				goto case_minus_line;

			    case 'B':	/* Seta para baixo */
				goto case_plus_line;

			    case '6':	/* Page down */
				goto case_n;

			    case '7':	/* Home */
				n = 1; goto case_g;

			    case '8':	/* End */
				n = 0; goto case_g;
			}

			goto case_default;

			/*
			 ******	Til (ignora, final de seqüências de escape) ******************
			 */
		    case '~':
			continue;

			/*
			 ******	Invoca o "sh" ************************************************
			 */
		    case '!':
		    case_sh:
			n = 0;
			fprintf (fp_out, "%s%s: !", parmexec (c_cup, LASTLINE, 0), c_ed);
			fflush (fp_out); /*** cb.b_msg++; ***/

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			fnrgets (read_area, sizeof (read_area), fp_tty);

			if (read_area[0] == '\0')
			{
				ioctl (fileno (fp_tty), TCSETNR, &new_termio);
				fprintf
				(	fp_out, "%s%s\n",
					parmexec (c_cup, LASTLINE, 0), c_ed
				);
				break;
			}

			system (read_area);

			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

			fprintf (fp_out, "\r\nTecle para continuar"); fflush (fp_out);

			while ((c = getc (fp_tty)) == CTLS || c == CTLQ)
				/* vazio */;

			if (c == '!')
			{
				fprintf
				(	fp_out, "%s%s\n",
					parmexec (c_cup, LASTLINE, 0), c_ed
				);
				goto case_sh;
			}

			goto clear_and_write_page;

			/*
			 ******	Cadeia de busca **********************************************
			 */
		    case '/':
			n = 0;
			fprintf (fp_out, "%s%s/", parmexec (c_cup, LASTLINE - 1, 0), c_ed);
			fflush (fp_out); cb.b_msg++;

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			fnrgets (pattern_area, sizeof (pattern_area), fp_tty);
			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

#ifdef	SEARCH_FROM_BEGIN
			for (cp = NOSTR, lp = list_head->m_forw; lp->m_head == 0; lp = lp->m_forw)
#else
			for (cp = NOSTR, lp = cb.b_screen_first_line; lp->m_head == 0; lp = lp->m_forw)
#endif	SEARCH_FROM_BEGIN
			{
				if (lp->m_status == '\0')
					get_letter_info (lp);

				if (cp = strstr (lp->m_text, pattern_area))
					break;
			}

		    after_pattern_search:
			if (cp == NOSTR)
			{
				fprintf
				(	fp_out,
					"\a%s%s%sCadeia não encontrada: \"%s\"%s\r",
					parmexec (c_cup, LASTLINE - 1, 0), c_ed,
					c_rev, pattern_area, c_sgr0
				);
				cb.b_msg++; break;
			}

			n = lp->m_index; cb.b_next_cmd = ' '; goto case_g;

			/*
			 ******	Repetição da cadeia de busca (adiante) ***********************
			 */
		    case 'n':
			n = 0;

			if (pattern_area[0] == '\0')
			{
			    missing_pattern:
				fprintf
				(	fp_out,
					"\a%s%s%sAinda não foi dada uma cadeia de busca%s\r",
					parmexec (c_cup, LASTLINE - 1, 0), c_ed, c_rev, c_sgr0
				);
				cb.b_msg++; break;
			}

			for (cp = NOSTR, lp = cb.b_cursor_mp->m_forw; lp->m_head == 0; lp = lp->m_forw)
			{
				if (lp->m_status == '\0')
					get_letter_info (lp);

				if (cp = strstr (lp->m_text, pattern_area))
					break;
			}

			goto after_pattern_search;

			/*
			 ******	Repetição da cadeia de busca (volta) *************************
			 */
		    case 'N':
			n = 0;

			if (pattern_area[0] == '\0')
				goto missing_pattern;

			for (cp = NOSTR, lp = cb.b_cursor_mp->m_back; lp->m_head == 0; lp = lp->m_back)
			{
				if (lp->m_status == '\0')
					get_letter_info (lp);

				if (cp = strstr (lp->m_text, pattern_area))
					break;
			}

			goto after_pattern_search;

			/*
			 ******	CSTOP & CSTART ***********************************************
			 */
		    case CTLQ:
		    case CTLS:
			continue;

			/*
			 ******	Caracter inválido ********************************************
			 */
		    default:
		    case_default:
			putc ('\a', fp_out);
			n = 0;
			break;

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end show_direc */

/*
 ****************************************************************
 *	Imprime uma linha					*
 ****************************************************************
 */
void
write_one_line (MDIREC *lp)
{
	if (lp->m_status == '\0')
		get_letter_info (lp);

	fputs (lp->m_text, fp_out);

	if (lp->m_len != LINE_SZ)
		fprintf (fp_out, "%s\r\n", c_el);

}	/* end write_one_line */
