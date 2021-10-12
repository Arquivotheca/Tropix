/*
 ****************************************************************
 *								*
 *			show.c					*
 *								*
 *	Mostra uma carta/lista de cartas na tela do vídeo	*
 *								*
 *	Versão	3.0.0, de 19.12.97				*
 *		3.1.6, de 07.02.99				*
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

#include "../h/mail.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern TERMIO	new_termio, old_termio; /* Estado do vídeo */

/*
 ******	Estrutura de contrôle usada por "show" ******************
 */
typedef struct
{
	LINE	*b_screen_first_line;
	LINE	*b_screen_last_line;
	LINE	*b_cursor_mp;
	char	b_cmd;
	char	b_next_cmd;
	char	b_msg;

}	CBOX;

/*
 ****** Protótipos de funções ***********************************
 */
void		line_write_one_line (LINE *);
int		rm_letter (CBOX *cb);
char		*convert_quoted (const char *, char *);
int		convert_and_show (LINE *, int);
int		get_part_info_and_show (LINE *);
int		get_base_64_sz (const LINE *);
int		get_text_sz (const LINE *, const LINE *);

/*
 ****************************************************************
 *	Mostra um arquivo					*
 ****************************************************************
 */
int
show_line (const MDIREC *letter_mp, LINE *list_head)
{
	int		i, n = 0;
	LINE		*lp, *cursor_mp;
	CBOX		cb;
	char		c, *cp;
	int		cursor, new_cursor;
	char		read_area[80], pattern_area[80];

	/*
	 *	Lista uma tela
	 */
	cb.b_screen_last_line = NOLINE;
	cb.b_msg = 0;
	cb.b_next_cmd = 0;

	pattern_area[0] = '\0';

	list_head->l_text = "*** Cabeça da Lista ***";
	list_head->l_len  = strlen (list_head->l_text);

	fputs (c_clear, fp_out);

	cb.b_cursor_mp = cb.b_screen_first_line = list_head->l_forw;

	/*
	 *	Escreve uma tela
	 */
	i = TEXTLINE;

	for (lp = cb.b_screen_first_line; lp->l_head == 0; lp = lp->l_forw)
	{
		line_write_one_line (lp); cb.b_screen_last_line = lp;

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

			for (i = 0, lp = cb.b_screen_first_line; lp->l_head == 0; lp = lp->l_forw, i++)
			{
				if (i == LASTLINE - 1)
				{
					fprintf (fp_out, "%s", parmexec (c_cup, LASTLINE - 1, 0));
					line_write_one_line (lp);
					fputs (c_ed, fp_out);
					break;
				}
			}

			cb.b_msg = 0;
		}

		/*
		 *	Posiciona o cursor
		 */
		for (cursor = 0, lp = cb.b_screen_first_line; /* abaixo */; lp = lp->l_forw, cursor++)
		{
			if (lp == cb.b_cursor_mp)
				break;

			if (lp == cb.b_screen_last_line)
			{
				if (cb.b_cursor_mp->l_index < cb.b_screen_first_line->l_index)
					{ cb.b_cursor_mp = cb.b_screen_first_line; cursor = 0; }
				else
					  cb.b_cursor_mp = cb.b_screen_last_line;

				break;
			}
		}

		fprintf (fp_out, "%s%s", parmexec (c_cup, cursor, 0), c_rev);
		line_write_one_line (lp);
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
			return (QUIT);

			/*
			 ******	Branco: Nada faz *********************************************
			 */
		    case ' ':
			n = 0;
			break;

			/*
			 ****** Parte seguinte ***********************************************
			 */
		    case 'l':
			return (NODE_FORW);

			/*
			 ****** Parte anterior ***********************************************
			 */
		    case 'h':
			return (NODE_BACK);

			/*
			 ****** Sobe de nível na árvore **************************************
			 */
		    case 'i':
			return (NODE_UP);

			/*
			 ******	Reescreve a tela *********************************************
			 */
		    case CTLR:
		    clear_and_write_page:
			if (cb.b_screen_first_line->l_head)
				goto case_q;
#undef	CLEAR
#ifdef	CLEAR
			fputs (c_clear, fp_out);
#else
			fputs (parmexec (c_cup, 0, 0), fp_out);
#endif	CLEAR
			i = TEXTLINE;

			for (lp = cb.b_screen_first_line; lp->l_head == 0; lp = lp->l_forw)
			{
				line_write_one_line (lp); cb.b_screen_last_line = lp;

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
				if ((lp = cb.b_cursor_mp->l_forw)->l_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor + i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			line_write_one_line (cursor_mp);

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
				cb.b_screen_first_line = cb.b_screen_first_line->l_forw;
				cb.b_screen_last_line  = cb.b_screen_last_line->l_forw;

				fprintf (fp_out, "%s\n", parmexec (c_cup, LASTLINE, 0));
				fputs (parmexec (c_cup, TEXTLINE - 1, 0), fp_out);
				line_write_one_line (cb.b_screen_last_line);
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
				if ((lp = cb.b_cursor_mp->l_back)->l_head)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor - i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			line_write_one_line (cursor_mp);

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
				cb.b_screen_first_line = cb.b_screen_first_line->l_back;
				cb.b_screen_last_line  = cb.b_screen_last_line->l_back;

				fprintf (fp_out, "%s%s", parmexec (c_cup, 0, 0), c_ri);
				line_write_one_line (cb.b_screen_first_line);
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
				if ((lp = cb.b_cursor_mp->l_forw)->l_head)
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
				if ((lp = cb.b_cursor_mp->l_back)->l_head)
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
				cb.b_cursor_mp = cb.b_screen_first_line = list_head->l_back;

				for (i = TEXTLINE - 1; i > 0; i--)
				{
					if (cb.b_screen_first_line->l_head)
						break;

					cb.b_screen_first_line = cb.b_screen_first_line->l_back;
				}

				if (cb.b_screen_first_line->l_head)
					cb.b_screen_first_line = list_head->l_forw;

				goto clear_and_write_page;
			}
			elif (n == 1)			/* Primeira página */
			{
				cb.b_cursor_mp = cb.b_screen_first_line = list_head->l_forw;

				goto clear_and_write_page;
			}
			elif ((unsigned)n > list_head->l_back->l_index) /* Linha inválida */
			{
				goto last_page;
			}

			/* Procura a linha de índice dado */

			for (lp = list_head->l_forw; lp->l_index != n; lp = lp->l_forw)
			{
				if (lp->l_head)	/* Linha inexistente */
					{ putc ('\a', fp_out); lp = cb.b_cursor_mp; break; }
			}

			if   (lp == cb.b_cursor_mp)
				{ n = 0; break; }

			if (lp->l_index > cb.b_cursor_mp->l_index)
			{
				for (n = 0; /* abaixo */; n++, lp = lp->l_back)
				{
					if (lp->l_back->l_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_plus_line;
			}
			else	/* lp->l_index < cb.b_cursor_mp->l_index */
			{
				for (n = 0; /* abaixo */; n++, lp = lp->l_forw)
				{
					if (lp->l_forw->l_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_minus_line;
			}

			/*
			 ****** Adiciona o conteúdo a um arquivo *****************************
			 */
		    case 's':
			n = 0;

			fprintf
			(	fp_out, "%s: %c %s",
				parmexec (c_cup, LASTLINE - 1, 0), cb.b_cmd, c_ed
			);
			fflush (fp_out); cb.b_msg++;

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);
			fnrgets (read_area, sizeof (read_area), fp_tty);
			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

			cp = read_area;

			while (*cp == ' ' || *cp == '\t')
				cp++;

			if (*cp == '\0')
				{ cb.b_next_cmd = ' '; break; }

			append_lines (cp, list_head, NOSTR);

			break;

			/*
			 ****** Envia uma carta para outro destinatário **********************
			 */
		    case 'f':
			return (FORWARD);

			/*
			 ****** Responde uma carta *******************************************
			 */
		    case 'r':
		    {
			const char	*file_nm;

			n = 0;

			if ((file_nm = mktemp ("/var/tmp/mail_file_XXXXXX")) == NOSTR)
				error ("$NÃO consegui obter o nome de um arquivo temporário");

			if (append_lines (file_nm, list_head, letter_mp->m_subject) < 0)
				break;

			fprintf (fp_out, "%s%s: ", parmexec (c_cup, LASTLINE, 0), c_ed); fflush (fp_out);

			ioctl (fileno (fp_tty), TCSETNR, &old_termio);

			sprintf (read_area, "mail -e -f %s %s", file_nm, letter_mp->m_from_box);

			fprintf (fp_out, "%s\n", read_area);

			system (read_area);

			ioctl (fileno (fp_tty), TCSETNR, &new_termio);

			fprintf (fp_out, "\r\nTecle para continuar"); fflush (fp_out);

			unlink (file_nm);

			while ((c = getc (fp_tty)) == CTLS || c == CTLQ)
				/* vazio */;

			goto clear_and_write_page;
		    }

			/*
			 ******	Remove uma carta *********************************************
			 */
		    case 'd':
			return (DELETE);

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

			    case 'C':	/* Seta para direita */
				return (NODE_FORW);

			    case 'D':	/* Seta para esquerda */
				return (NODE_BACK);

			    case '5':	/* Page up */
				return (NODE_UP);

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
			for (cp = NOSTR, lp = list_head->l_forw; lp->l_head == 0; lp = lp->l_forw)
#else
			for (cp = NOSTR, lp = cb.b_screen_first_line; lp->l_head == 0; lp = lp->l_forw)
#endif	SEARCH_FROM_BEGIN
			{
				if (cp = strstr (lp->l_text, pattern_area))
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

			n = lp->l_index; cb.b_next_cmd = ' '; goto case_g;

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

			for (cp = NOSTR, lp = cb.b_cursor_mp->l_forw; lp->l_head == 0; lp = lp->l_forw)
			{
				if (cp = strstr (lp->l_text, pattern_area))
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

			for (cp = NOSTR, lp = cb.b_cursor_mp->l_back; lp->l_head == 0; lp = lp->l_back)
			{
				if (cp = strstr (lp->l_text, pattern_area))
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

}	/* end show */

/*
 ****************************************************************
 *	Imprime uma linha					*
 ****************************************************************
 */
void
line_write_one_line (LINE *lp)
{
	fputs (lp->l_text, fp_out);

	if (lp->l_len != LINE_SZ)
		fprintf (fp_out, "%s\r\n", c_el);

}	/* end write_one_line */
