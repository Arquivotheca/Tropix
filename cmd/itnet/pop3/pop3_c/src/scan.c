/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Analisador léxico					*
 *								*
 *	Versão	1.0.0, de 23.08.88				*
 *		3.2.0, de 30.09.01				*
 *								*
 *	Módulo: POP3_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xti.h>

#include "../h/pop3_c.h"

/*
 ****************************************************************
 *	Processa o MIME 2					*
 ****************************************************************
 */
void
do_mime_2 (const char *in_area, char *out_area)
{
	const char	*rp = in_area;
	char		*cp = out_area;
	int		c;
	char		questions = 0, q_field = 0;

	/*
	 *	Processa o MIME 2
	 */
	for (EVER)
	{
		if   ((c = *rp++) == '\0')
		{
			break;
		}
		elif (c == '=')
		{
			if   (questions == 0)
			{
				if (*rp == '?')
					{ questions++; rp++; }
				else
					*cp++ = c;
			}
			elif (q_field)
			{
				char	d, high = 0, low = 0;

				d = rp[0];

				if   (d >= '0' && d <= '9')
					high = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					high = d - 'A' + 10 + 1;

				d = rp[1];

				if   (d >= '0' && d <= '9')
					low = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					low = d - 'A' + 10 + 1;

				if (high && low)
				{
					*cp++ = (high - 1) << 4 | (low - 1);
					rp += 2;
				}
			}
		}
		elif (c == '?')
		{
			if (questions == 0)
				{ *cp++ = c; continue; }

			if   (++questions == 2)
			{
				if   (*rp == 'Q' || *rp == 'q')
				{
					q_field++; rp++;
				}
				elif (*rp == 'B' || *rp == 'b')
				{
					char		t;
					int		total_bits = 0;
					long		word = 0;

					rp++;	/* Pula o "B" */

					while ((c = *rp++) != 0)
					{
						if   (c == '=')
						{
							continue;
						}
						elif (c == '?')
						{
							if (++questions == 3)
								continue;

							questions--; rp--;

							break;
						}

						if ((t = base_64_tb[c]) == 0xFF)
							break;

						word <<= 6; word |= t;

						if ((total_bits += 6) >= 24)
						{
							*cp++ = (word >> 16);
							*cp++ = (word >> 8);
							*cp++ =  word;

							total_bits -= 24;
							word = 0;
						}
					}

					/* Emite os bytes residuais */

					if   (total_bits >= 18)
					{
						*cp++ = (word >> 10);
						*cp++ = (word >> 2);
					}
					elif (total_bits >= 12)
					{
						*cp++ = (word >> 4);
					}
				}
			}
		   /***	elif (questions == 3)	/* "?" após "Q" ou "B" ***/
		   /***	{	***/
		
		   /***	}	***/
			elif (questions >= 4)
			{
				if (*rp == '=')
				{
					rp++;
					questions = q_field = 0;
				}
				else
				{
					questions--;
					*cp++ = c;
				}
			}
		}
		elif (questions == 1)
		{
			/* Pula o "ISO-8859-1" */
		}
		elif (c == '_' && q_field)
		{
			*cp++ = ' ';
		}
		else	/* Caractere sem transformação */
		{
			*cp++ = c;
		}

	}	/* end for (EVER) */

	*cp = '\0';

}	/* end do_mime_2 */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Processa o MIME 2					*
 ****************************************************************
 */
void
do_mime_2 (const char *in_area, char *out_area)
{
	const char	*rp = in_area;
	char		*cp = out_area;
	int		c;
	char		in_mime, questions;
	char		transformation;

	/*
	 *	Processa o MIME 2
	 */
	transformation = 0;
	in_mime = questions = 0;

	for (EVER)
	{
		if   ((c = *rp++) == '\0')
		{
			break;
		}
		elif (c == '=')
		{
			if (!in_mime)
			{
				if (*rp == '?')
					in_mime++;
				else
					*cp++ = c;
			}
			elif (transformation == 'q')
			{
				char	d, high = 0, low = 0;

				d = rp[0];

				if   (d >= '0' && d <= '9')
					high = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					high = d - 'A' + 10 + 1;

				d = rp[1];

				if   (d >= '0' && d <= '9')
					low = d - '0' + 1;
				elif (d >= 'A' && d <= 'F')
					low = d - 'A' + 10 + 1;

				if (high && low)
				{
					*cp++ = (high - 1) << 4 | (low - 1);
					rp += 2;
				}
			}
		}
		elif (c == '?')
		{
			if   (in_mime)
			{
				questions++;

				if   (questions == 2)
				{
					if   (*rp == 'Q' || *rp == 'q')
					{
						transformation = 'q'; rp++;
					}
					elif (*rp == 'B' || *rp == 'b')
					{
						char		first_question = 1;
						char		t;
						int		total_bits = 0;
						long		word = 0;

						rp++;

						while ((c = *rp++) != 0)
						{
							if   (c == '=')
							{
								continue;
							}
							elif (c == '?')
							{
								questions++;

								if (first_question)
								{
									first_question = 0;
									continue;
								}

								break;
							}

							if ((t = base_64_tb[c]) == 0xFF)
								break;

							word <<= 6; word |= t;

							if ((total_bits += 6) >= 24)
							{
								*cp++ = (word >> 16);
								*cp++ = (word >> 8);
								*cp++ =  word;

								total_bits -= 24;
								word = 0;
							}
						}

						/* Emite os bytes residuais */

						if   (total_bits >= 18)
						{
							*cp++ = (word >> 10);
							*cp++ = (word >> 2);
						}
						elif (total_bits >= 12)
						{
							*cp++ = (word >> 4);
						}
					}
				}
				elif (questions >= 4)
				{
					if (*rp == '=')
					{
						rp++;
						in_mime = questions = 0;
					}
					else
					{
						questions--;
						*cp++ = c;
					}
				}
#if (0)	/*******************************************************/
				elif (questions == 3)
				{
					*cp++ = c;
				}
#endif	/*******************************************************/
			}
			else
			{
				*cp++ = c;
			}
		}
		elif (questions == 1)
		{
			/* Pula */
		}
		elif (c == '_' && transformation == 'q')
		{
			*cp++ = ' ';
		}
		else
		{
			*cp++ = c;
		}

	}	/* end for (EVER) */

	*cp = '\0';

}	/* end do_mime_2 */
#endif	/*******************************************************/
