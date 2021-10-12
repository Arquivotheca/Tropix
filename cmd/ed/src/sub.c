/*
 ****************************************************************
 *								*
 *			src/sub.c				*
 *								*
 *	Substituições						*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
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
#include <stdlib.h>

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Compilação de uma substituição.				*
 ****************************************************************
 */
void
sub_compile (const char *string, REPL *repl)
{
	const char	*cp;	/* Character pointer */
	char		*tp;	/* Text pointer */
	char		**sp;	/* Specials pointer */

	sp = repl->r_specials;
	tp = repl->r_text;

	for (cp = string; *cp; cp++)
	{
		switch (*cp)
		{
			/*
			 *	Comandos Especiais:
			 */
		    case '&':
		    case '\n':
			*tp++ = '\0';
			*sp++ = tp;
			*tp++ = *cp;
			break;

		    case '\\':
			if (*(cp+1))
				cp++;
			/* continua */

		    default:
			*tp++ = *cp;
			break;
		}
	}

	*sp = tp;
	*tp++ = '\0';
	*tp = '\0';

}	/* end sub_compile */

/*
 ****************************************************************
 *	Executa substituição.					*
 ****************************************************************
 */
char *
sub_execute (BASE *base, char *start, char *End, REPL *repl)
{
	char		*lp;	/* Line Pointer */
	char		*cp;	/* Character Pointer */
	char		*tp;	/* Replacement Text Pointer */
	char		**sp;	/* Special Commands Pointer */
	char		*next;	/* Proximo caractere após a substituição */
	int		room;	/* Espaco disponivel */

	/*
	 *	Copia início da linha.
	 */
	lp = temp_buffer;
	cp = base->ch;

	if ((room = LSIZE - (start - cp)) < 0)
		return (NULL);

	while (cp < start)
		*lp++ = *cp++;

	/*
	 *	Insere texto da Substituição.
	 */
	sp = repl->r_specials;
	tp = repl->r_text;

	for (EVER)	/*  Percorre repl->r_text */
	{
		/*
		 *	Copia até um comando especial.
		 */
		if ((room -= *sp - tp) < 0)
			return (NULL);

		while (*lp++ = *tp++)
			/* vazio */;
		lp--;

		/*
		 *	Verifica se repl->r_text já foi percorrido.
		 */
		if (*tp == '\0')
			break;

		/*
		 *	Executa comando especial.
		 */
		switch (*tp)
		{
			/*
			 *	Copia a parte removida na substituição.
			 */
		    case '&':
			if ((room -= End - start) < 0)
				return (NULL);

			cp = start;

			while (cp < End)
				*lp++ = *cp++;
			break;

			/*
			 *	Divide a linha em duas.
			 */
		    case '\n':
			*lp = '\0';

			if (!txt_insert ((base != Root) ? base->ant : NOBASE, temp_buffer))
				return (false);

			lp = temp_buffer;
			room = LSIZE;
			break;

		    default:
			printf ("Special pointer inesperado: '%c' \n", *tp);
			break;

		}	/* switch **sp */

		tp++; sp++;
	}

	/*
	 *	Copia final da linha após a parte substituida.
	 */
	next = lp;
	cp = End;

	while (room-- > 0 && (*lp++ = *cp++))
		/* vazio */;

	if (room == 0)
		return (NULL);

	/*
	 *	Aloca uma nova base->ch.
	 */
	if ((cp = malloc (lp - temp_buffer)) == NULL)
		return (NULL);

	free (base->ch); base->ch = cp;

	/*
	 *	Copia a nova linha para a nova base->ch.
	 */
	lp = temp_buffer;

	while (*cp++ = *lp++)
		/* vazio */;

	next = base->ch + (next - temp_buffer);

	return (next);

}	/* end sub_execute */
