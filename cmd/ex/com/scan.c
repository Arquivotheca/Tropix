/*
 ****************************************************************
 *								*
 *			scan.c					*
 *								*
 *	Leitura dos comandos					*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 23.03.95				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <curses.h>
#include <ctype.h>

#include "../h/global.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Opções de compilação					*
 ****************************************************************
 */
#define	NO_TERMINATOR	/* Nao é necessário terminador para cadeias */

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry char	Line[LSIZE];
entry char	*nextp;
entry TOKEN	token;

/*
 ****************************************************************
 *	Leitura de uma linha da entrada padrão			*
 ****************************************************************
 */
bool	
scn_command (void)
{
	int		c;
	char		*lp;
	bool		too_long;

	lp = Line; too_long = false;

	while ((c = wgetch (Win)) != ERR)
	{
		if (c == EOT || c == EOL)
			break;

		if (c == '\n')
		{
			if (lp > Line && lp[-1] == '\\')
				lp[-1] = c;
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

	lp[0] = '\0';

	if (too_long)
	{
		scn_error ("Linha longa demais", NOMARK);
		return (false);
	}

	if   (c == EOT)
	{
		scn_error ("Fim inesperado do arquivo de entrada", NOMARK);
		return (false);
	}
	elif (c == ERR)
	{
		return (false);
	}

	nextp = Line;
	return (true);

}	/* end scn_command */

/*
 ****************************************************************
 *	Leitura de um Nome (separado por brancos)		*
 ****************************************************************
 */
char *
scn_name (void)
{
	char		*lp, *bp;

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

	return (NOSTR);

}	/* end scn_name */

/*
 ****************************************************************
 *	Leitura de uma cadeia (terminada por term)		*
 ****************************************************************
 */
char *
scn_string (int term)
{
	char		*lp;
	char		*bp;
	bool		special;

	token.t_begin = nextp;
	special = false;

	for (lp = nextp, bp = temp_buffer; /* sem teste */; lp++, bp++)
	{
		/*
		 *	Deteção de terminação
		 */
		if (*lp == term)
		{
			if (special)
			{
				*(--bp) = term;
				continue;
			}
			else
			{
				break;
			}
		}

		/*
		 *	Terminação com '\n'
		 */
#ifdef	NO_TERMINATOR
		if ((*bp = *lp) == '\0')
			break;
#else
		if ((*bp = *lp) == '\0')
		{
			token.t_end = lp - 1;
			nextp = lp;
			error_msg = "Cadeia não terminada";
			return (NULL);
		}
#endif	NO_TERMINATOR

		/*
		 *	Tratamento Especial
		 */
		if (*lp == '\\')
			special = !special;
		else
			special = false;
	}

	*bp = '\0';

	token.t_end = lp - 1;

#if (0)	/*************************************/
	nextp = (*lp != '\0') ? lp + 1 : lp;
#endif	/*************************************/

	nextp = lp;

	if (*lp)
		nextp++;

	return (temp_buffer);

}	/* end scn_string */

/*
 ****************************************************************
 *	Pega letra minúscula					*
 ****************************************************************
 */
int
scn_lower (void)
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
		error_msg = "Espero letra minúscula";

	return (ch);

}	/* end scn_lower */
