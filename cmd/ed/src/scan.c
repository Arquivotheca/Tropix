/*
 ****************************************************************
 *								*
 *			src/scan.c				*
 *								*
 *	Leitura dos Comandos					*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */
#include <stdio.h>
#include <ctype.h>

#include "../h/ed.h"
#include "../h/scan.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições deste módulo					*
 ****************************************************************
 */
#define	NO_TERMINATOR	/* Não é necessario terminador para strings */

entry char		Line[LSIZE];
entry char		*nextp;
entry const char	*error_msg;	/* Mensagem de Erro */
entry TOKEN		token;

/*
 ****************************************************************
 *	Leitura de uma linha da entrada padrao.			*
 ****************************************************************
 */
bool	
read_command (void)
{
	int		c;
	char		*lp;
	FILE		*in;
	bool		too_long;

	in = stdin; lp = Line; too_long = false;

	while ((c = getc (in)) >= 0)
	{
		if (c == '\n')
		{
			if (lp > Line && *(lp-1) == '\\')
				*(lp-1) = c;
			else
				break;
		}
		else
		{
			if (lp < &Line[LSIZE-1])
				*lp++ = c;
			else
				too_long = true;
		}
	}

	*lp = '\0';

	if (too_long)
	{
		ed_error ("Linha longa demais", NOMARK);
		return (false);
	}

	if (c < 0)
	{
		if (!Interruption)
			ed_error ("Fim inesperado do arquivo de entrada", NOMARK);
		return (false);
	}

	nextp = Line;
	return (true);

}	/* end read_command */

/*
 ****************************************************************
 *	Leitura de um Nome (separado por brancos).		*
 ****************************************************************
 */
char *
get_name (void)
{
	char		*lp;
	char		*bp;

	for (lp = nextp; isspace (*lp); lp++)
		/* vazio */;

	for (bp = temp_buffer; *lp && !isspace (*lp); lp++)
		*bp++ = *lp;

	*bp = '\0';

	if (*lp && lp > nextp)
	{
		token.t_begin = nextp;
		token.t_end = lp;
		nextp = lp + 1;
		return (temp_buffer);
	}
	elif (lp > nextp)
	{
		token.t_begin = nextp;
		token.t_end = lp - 1;
		nextp = lp;
		return (temp_buffer);
	}

	return (NULL);

}	/* end get_name */

/*
 ****************************************************************
 *	Leitura de uma Cadeia (terminada por term).		*
 ****************************************************************
 */
char *
get_string (int term)
{
	char		*lp;
	char		*bp;

	token.t_begin = nextp;

	for (lp = nextp, bp = temp_buffer; /* sem teste */; lp++, bp++)
	{
		if (*lp == term)
		{
			if (*(lp-1) == '\\')
			{
				*(--bp) = term;
				continue;
			}
			else
			{
				break;
			}
		}

		if ((*bp = *lp) == '\0')
#ifdef	NO_TERMINATOR
			break;
#else
		{
			token.t_end = lp - 1;
			nextp = lp;
			error_msg = "Cadeia não terminada";
			return (NULL);
		}
#endif	NO_TERMINATOR
	}

	*bp = '\0';

	token.t_end = lp - 1;
	nextp = (*lp) ? lp + 1 : lp;

	return (temp_buffer);

}	/* end get_string */

/*
 ****************************************************************
 *	Pega letra minuscula.					*
 ****************************************************************
 */
int
get_lower (void)
{
	char		*lp;
	char		ch;

	for (lp = nextp; *lp && isspace (*lp); lp++)
		/* vazio */;

	if (*lp)
	{
		token.t_begin = lp;
		token.t_end = lp;
		nextp = lp + 1;
		ch = islower (*lp) ? *lp : '\0';
	}
	else
	{
		token.t_begin = nextp;
		token.t_end = nextp;
		nextp = lp;
		ch = '\0';
	}

	if (ch == '\0')
		error_msg = "Espero letra minuscula";

	return (ch);

}	/* end get_lower */
