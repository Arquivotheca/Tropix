/*
 ****************************************************************
 *								*
 *			show_part.c				*
 *								*
 *	Mostra uma a lista de partes na tela do vídeo		*
 *								*
 *	Versão	4.0.0, de 24.08.00				*
 *		4.0.0, de 24.08.00				*
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
extern int	new_letters;	/* Indica recebimento de novas cartas */

extern TERMIO	new_termio, old_termio; /* Estado do vídeo */

/*
 ****** Protótipos de funções ***********************************
 */
void		part_write_one_line (PART *);
int		rm_letter (PARTBOX *cb);
char		*convert_quoted (const char *, char *);
int		convert_and_show (PART *, int);
int		move_or_copy_letter (int, char *, const PART *);
int		get_part_info_and_show (PART *);
int		get_text_sz (const PART *, const PART *);

/*
 ****************************************************************
 *	Mostra um arquivo					*
 ****************************************************************
 */
int
show_part (const MDIREC *letter_mp, PART *part_head)
{
	int		i, n = 0;
	PART		*list_head = part_head->p_part_head;
	PART		*lp, *cursor_mp;
	PART		*first_part;
	PARTBOX		cb;
	char		c;
	int		cursor, new_cursor;
	char		read_area[80], pattern_area[80];

	/*
	 *	Lista uma tela
	 */
	cb.b_screen_last_line = NOPART;
	cb.b_msg = 0;
	cb.b_next_cmd = 0;

	pattern_area[0] = '\0';

	list_head->p_text = "*** Cabeça da Lista ***";
	list_head->p_len  = strlen (list_head->p_text);

	fputs (c_clear, fp_out);

	cb.b_cursor_mp = first_part = cb.b_screen_first_line = list_head->p_forw;

	/*
	 *	coloca o cursor após o cabeçalho
	 */
	for (lp = cb.b_screen_first_line; lp->p_head == 0; lp = lp->p_forw)
	{
		if (lp->p_line_head.l_forw != NOLINE)
		{
			cb.b_cursor_mp = first_part = lp;
			break;
		}
	}

	/*
	 *	Escreve uma tela
	 */
	i = TEXTLINE;

	for (lp = cb.b_screen_first_line; lp->p_head == 0; lp = lp->p_forw)
	{
		part_write_one_line (lp); cb.b_screen_last_line = lp;

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

			for (i = 0, lp = cb.b_screen_first_line; lp->p_head == 0; lp = lp->p_forw, i++)
			{
				if (i == LASTLINE - 1)
				{
					fprintf (fp_out, "%s", parmexec (c_cup, LASTLINE - 1, 0));
					part_write_one_line (lp);
					fputs (c_ed, fp_out);
					break;
				}
			}

			cb.b_msg = 0;
		}

		/*
		 *	Posiciona o cursor
		 */
		for (cursor = 0, lp = cb.b_screen_first_line; /* abaixo */; lp = lp->p_forw, cursor++)
		{
			if (lp == cb.b_cursor_mp)
				break;

			if (lp == cb.b_screen_last_line)
			{
				if (cb.b_cursor_mp->p_index < cb.b_screen_first_line->p_index)
					{ cb.b_cursor_mp = cb.b_screen_first_line; cursor = 0; }
				else
					  cb.b_cursor_mp = cb.b_screen_last_line;

				break;
			}
		}

		fprintf (fp_out, "%s%s", parmexec (c_cup, cursor, 0), c_rev);
		part_write_one_line (lp);
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
			if (cb.b_screen_first_line->p_head)
				goto case_q;
#undef	CLEAR
#ifdef	CLEAR
			fputs (c_clear, fp_out);
#else
			fputs (parmexec (c_cup, 0, 0), fp_out);
#endif	CLEAR
			i = TEXTLINE;

			for (lp = cb.b_screen_first_line; lp->p_head == 0; lp = lp->p_forw)
			{
				part_write_one_line (lp); cb.b_screen_last_line = lp;

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
				if ((lp = cb.b_cursor_mp->p_forw)->p_head)
					break;

				if (lp->p_line_head.l_forw == NOLINE)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor + i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			part_write_one_line (cursor_mp);

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
				cb.b_screen_first_line = cb.b_screen_first_line->p_forw;
				cb.b_screen_last_line  = cb.b_screen_last_line->p_forw;

				fprintf (fp_out, "%s\n", parmexec (c_cup, LASTLINE, 0));
				fputs (parmexec (c_cup, TEXTLINE - 1, 0), fp_out);
				part_write_one_line (cb.b_screen_last_line);
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
				if ((lp = cb.b_cursor_mp->p_back)->p_head)
					break;

				if (lp->p_line_head.l_forw == NOLINE)
					break;

				cb.b_cursor_mp = lp;
			}

			n = 0;

			if (i == 0)
				{ putc ('\a', fp_out); break; }

			new_cursor = cursor - i;

			fprintf (fp_out, "%s", parmexec (c_cup, cursor, 0));
			part_write_one_line (cursor_mp);

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
				cb.b_screen_first_line = cb.b_screen_first_line->p_back;
				cb.b_screen_last_line  = cb.b_screen_last_line->p_back;

				fprintf (fp_out, "%s%s", parmexec (c_cup, 0, 0), c_ri);
				part_write_one_line (cb.b_screen_first_line);
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
				if ((lp = cb.b_cursor_mp->p_forw)->p_head)
					break;

				if (lp->p_line_head.l_forw == NOLINE)
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
				if ((lp = cb.b_cursor_mp->p_back)->p_head)
					break;

				if (lp->p_line_head.l_forw == NOLINE)
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
				cb.b_cursor_mp = cb.b_screen_first_line = list_head->p_back;

				for (i = TEXTLINE - 1; i > 0; i--)
				{
					if (cb.b_screen_first_line->p_head)
						break;

					cb.b_screen_first_line = cb.b_screen_first_line->p_back;
				}

				if (cb.b_screen_first_line->p_head)
					cb.b_screen_first_line = list_head->p_forw;

				goto clear_and_write_page;
			}
			elif ((unsigned)n > list_head->p_back->p_index) /* Linha inválida */
			{
				goto last_page;
			}

			/* Procura a linha de índice dado */

			for (lp = first_part; lp->p_index != n; lp = lp->p_forw)
			{
				if (lp->p_head)	/* Linha inexistente */
					{ putc ('\a', fp_out); lp = cb.b_cursor_mp; break; }
			}

			if   (lp == cb.b_cursor_mp)
				{ n = 0; break; }

			if (lp->p_index > cb.b_cursor_mp->p_index)
			{
				for (n = 0; /* abaixo */; n++, lp = lp->p_back)
				{
					if (lp->p_back->p_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_plus_line;
			}
			else	/* lp->p_index < cb.b_cursor_mp->p_index */
			{
				for (n = 0; /* abaixo */; n++, lp = lp->p_forw)
				{
					if (lp->p_forw->p_head || lp == cb.b_cursor_mp)
						break;
				}

				goto case_minus_line;
			}

			/*
			 ******	Mostra uma parte ou carta ************************************
			 */
		    case '\n':
		    case '\r':
		    case 'o':
		    case 'u':
		    case_n:
			n = 0;

			for (EVER)
			{
				int		r;

				if (cb.b_cursor_mp->p_boundary != NOSTR)
				{
					switch (r = show_part (letter_mp, cb.b_cursor_mp))
					{
					    case NODE_BACK:
						lp = cb.b_cursor_mp->p_back;

						if (lp->p_head == 0 && lp->p_line_head.l_forw != NOLINE)
							cb.b_cursor_mp = lp;
						else
							putc ('\a', fp_out);

						continue;

					    case NODE_FORW:
						if (cb.b_cursor_mp->p_forw->p_head == 0)
							cb.b_cursor_mp = cb.b_cursor_mp->p_forw;
						else
							putc ('\a', fp_out);

						continue;

					    case NODE_UP:
						goto clear_and_write_page;

					/** case FORWARD: **/
					/** case REPLY: **/
					/** case DELETE: **/
					/** case QUIT: **/
					    default:
						return (r);
					}
				}
				else
				{
					if (cb.b_cursor_mp->p_base64)
					{
						if (cb.b_cursor_mp->p_is_text)
						{
							r = show_base_64_text (letter_mp, &cb);
						}
						else
						{
							write_base_64_file (&cb);
							goto clear_and_write_page;
						}
					}
					else
					{
						r = show_line (letter_mp, &cb.b_cursor_mp->p_line_head);
					}

					switch (r)
					{
					    case NODE_BACK:
						lp = cb.b_cursor_mp->p_back;

						if (lp->p_head == 0 && lp->p_line_head.l_forw != NOLINE)
							cb.b_cursor_mp = lp;
						else
							putc ('\a', fp_out);

						continue;

					    case NODE_FORW:
						if (cb.b_cursor_mp->p_forw->p_head == 0)
							cb.b_cursor_mp = cb.b_cursor_mp->p_forw;
						else
							putc ('\a', fp_out);

						continue;

					    case NODE_UP:
						goto clear_and_write_page;

					/** case FORWARD: **/
					/** case REPLY: **/
					/** case DELETE: **/
					/** case QUIT: **/
					    default:
						return (r);
					}
				}
			}

			break;

			/*
			 ****** Envia uma carta para outro destinatário **********************
			 */
		    case 'f':
			return (FORWARD);

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

}	/* end sho_part */

/*
 ****************************************************************
 *	Imprime uma linha					*
 ****************************************************************
 */
void
part_write_one_line (PART *lp)
{
	fputs (lp->p_text, fp_out);

	if (lp->p_len != LINE_SZ)
		fprintf (fp_out, "%s\r\n", c_el);

}	/* end part_write_one_line */
