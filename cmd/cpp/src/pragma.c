/*
 ****************************************************************
 *								*
 *			pragma.c				*
 *								*
 *	Processamento da diretiva "#pragma"			*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.2.0, de 18.09.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*	
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Processa a Diretiva "#pragma"				*
 ****************************************************************
 */
void
dopragma (void)
{
	const char	**ep;
	char		*cp, *macro;
	int		len;
	static int	envbrought = 0;
	SYMTB		*sp, **link_place;
	const char	*p;
	char		*bp;
	char		buf[128], msgbuf[128];

	if (scan () != ID)
	{
		err_msg (ADVR, TOKMARK, "Esperava um identificador");
		return;
	}

	/*
	 *	Analisa o "pragma"
	 */
	if   (streq (token.l_name, "getenv"))
	{
		/* Pragma "getenv" */

		if   (scan () == EOL)
		{
			/*
			 *	Sem argumentos: traz o ambiente todo
			 */
			if (envbrought++)
				return;
	
			for (ep = environ; *ep != NOSTR; ep++)
			{
				if ((cp = malloc (strlen (*ep) + 2)) == NOSTR)
					error (NOSTR);

				cp[0] = '$'; strcpy (cp + 1, *ep);

				mkpredef (cp, S_ENV);
			}
		}
		elif (token.l_major != LP)
		{
			err_msg (ADVR, TOKMARK, "Esperava um '('");
		}
		else
		{
			/*
			 *	Foi dada a lista de variáveis do ambiente
			 */
			scan ();

			for (EVER)
			{
				if (token.l_major != ID)
				{
					err_msg
					(	ADVR, TOKMARK,
						"Esperava um identificador"
					);
					break;
				}

				/*
				 *	Procura a variável no ambiente
				 */
				cp = (char *)token.l_name;

				if (*cp == '$')
					cp++;

				len = strlen (cp);

				for (ep = environ; *ep != NOSTR; ep++)
				{
					if (!strncmp (*ep, cp, len))
					{
						if ((*ep)[len] != '=')
							continue;

						if ((cp = malloc (strlen (*ep) + 2)) == NOSTR)
							error (NOSTR);

						cp[0] = '$'; strcpy (cp + 1, *ep);

						mkpredef (cp, S_ENV);
	
						goto next;
					}
				}

				/*
				 *	Não encontrou: verifica se já tinha um valor anterior
				 */
				if ((macro = malloc (len + 2)) == NOSTR)
					error (NOSTR);

				macro[0] = '$'; strcpy (macro + 1, cp);

				if (TYPE (sp, macro, S_MACRO))
				{
					const char	*end_buf = buf + sizeof (buf) - 1;

					p = sp->s_macro; bp = buf;

					while (*p != '\n' && bp < end_buf)
						*bp++ = *p++;

					*bp++ = '\0';

					snprintf
					(	msgbuf,
						sizeof (msgbuf),
						"Não encontrei a variável "
						"\"%s\" no ambiente, "
						"usando \"%s\"",
						cp, buf
					);

					err_msg (ADVR, TOKMARK, msgbuf);
				}
				else
				{
					err_msg
					(	ADVR, TOKMARK,
						"Não encontrei a variável "
						"\"%s\" no ambiente",
						cp
					);
				}

				/*
				 *	Prepara o próximo
				 */
			    next:
				scan ();

				if (token.l_major == RP)
					break;

				if (token.l_major != CM)
				{
					err_msg
					(	ADVR, TOKMARK,
						"Esperava ',' ou ')'"
					);
					break;
				}

				scan ();

			}	/* end while */

		}	/* end (Foi dada a lista de variáveis do ambiente) */
	}
	elif (streq (token.l_name, "pack"))
	{
		/* Pragma "pack" */

		/* Por enquanto, ignorando */
	}
	else
	{
		err_msg
		(	ADVR, TOKMARK,
			"Diretiva \"pragma\" não reconhecida"
		);
	}

	putoutnl ();

}	/* end dopragma */
