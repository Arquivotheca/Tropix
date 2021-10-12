/*
 ****************************************************************
 *								*
 *			directive.c				*
 *								*
 *	Processa as diretivas					*
 *								*
 *	Versão	1.0.0, de 23.08.88				*
 *		4.1.0, de 22.08.01				*
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
 *	Variáveis Utilizadas					*
 ****************************************************************
 */
static char	stack[100];	/* Stack de "ifdefs", etc ... */

static char	*stackp = stack; /* Ponteiro do Stack */

entry int	falselevel;	/* No. de Condicionais FALSE aninhadas */

/*
 ****************************************************************
 *	Analisa uma Linha de Controle				*
 ****************************************************************
 */
void
directive (void)
{
	SYMTB		*sp, **link_place;
	MAJOR		maj;
	char		*cp, c;

	/*
	 *	Convenção da STACK:
	 *		'U': é indefinido, e nunca foi definido
	 *		'N': é indefinido, mas já foi definido
	 *		'D': é definido
	 */
	if ((maj = scan ()) == EOL)
	{
		skipline ();
		return;
	}

	if (maj != ID || !TYPE (sp, token.l_name, S_KEYWORD))
	{
		err_msg
		(	ERRO, TOKMARK,
			"Esperava uma diretiva de préprocessamento"
		);
		skipline ();
		return;
	}

	switch (sp->s_index)
	{
	    case DEFINE:
		if (falselevel == 0)
			dodefine ();
		break;

	    case UNDEF:
		if (falselevel == 0)
			doundef ();
		break;

	    case INCLUDE:
		if (falselevel == 0)
		{
			doinclude ();
			return;
		}
		break;

	    case IF:
		if (falselevel == 0)
		{
			if (expression ())
			{
				*stackp++ = 'D';
				putoutnl ();
			}
			else
			{
				*stackp++ = 'U';
				falselevel++;
			}
	
			/*
			 *	A análise de expressões já processou
			 *	o fim de linha.	Não devemos avançar mais.
			 */
			return;
		}
		else
		{
			*stackp++ = 'U';
			falselevel++;
		}
		break;

	    case IFDEF:
		if (scan () != ID)
		{
			err_msg (ERRO, TOKMARK, "Esperava um identificador");
			*stackp++ = 'D';
			break;
		}

		if (TYPE (sp, token.l_name, S_MACRO))
		{
			*stackp++ = 'D';
			if (falselevel == 0)
				putoutnl ();
		}
		else
		{
			*stackp++ = 'U';
			falselevel++;
		}

		break;

	    case IFNDEF:
		if (scan () != ID)
		{
			err_msg (ERRO, TOKMARK, "Esperava um identificador");
			*stackp++ = 'D';
			break;
		}

		if (!TYPE (sp, token.l_name, S_MACRO))
		{
			*stackp++ = 'D';
			if (falselevel == 0)
				putoutnl ();
		}
		else
		{
			*stackp++ = 'U';
			falselevel++;
		}

		break;

	    case ELIF:
		if (stackp <= stack)
		{
			err_msg
			(	ERRO, TOKMARK,
				"\"elif\" sem prévio \"if\" ou \"ifdef\""
			);
			break;
		}

		switch (stackp[-1])
		{
		    case 'U':
			if (falselevel == 1)
			{
				if (expression ())
				{
					stackp[-1] = 'D';
					falselevel--;	/* == 0 */
					putlineno (lineno);
				}
				else
				{
					/* Nada faz */
				}

				return;
			}
			else
			{
				/* Nada faz */
			}
			break;

		    case 'N':
			/* Nada faz */
			break;

		    case 'D':
			stackp[-1] = 'N';
			falselevel++;
			putoutnl ();
			break;

		    default:
			err_msg
			(	ERRO, TOKMARK,
				"Estado inválido da STACK: '%c'",
				stackp[-1]
			);
		}

		break;

	    case ELSE:
		if (stackp <= stack)
		{
			err_msg
			(	ERRO, TOKMARK,
				"\"else\" sem prévio \"if\" ou \"ifdef\""
			);
			break;
		}

		switch (stackp[-1])
		{
		    case 'U':
			stackp[-1] = 'D';
			if (--falselevel == 0)
				putlineno (lineno + 1);
			break;

		    case 'N':
			/* Nada faz */
			break;

		    case 'D':
			stackp[-1] = 'U';
			falselevel++;
			break;

		    default:
			err_msg
			(	ERRO, TOKMARK,
				"Estado inválido da STACK: '%c'",
				stackp[-1]
			);
		}

		break;

	    case ENDIF:
		if (stackp <= stack)
		{
			err_msg
			(	ERRO, TOKMARK,
				"\"endif\" sem prévio \"if\" ou \"ifdef\""
			);
			break;
		}

		switch (*--stackp)
		{
		    case 'U':
		    case 'N':
			if (--falselevel == 0)
				putlineno (lineno + 1);
			break;

		    case 'D':
			putoutnl ();
			break;

		    default:
			err_msg
			(	ERRO, TOKMARK,
				"Estado inválido da STACK: '%c'",
				stackp[0]
			);
		}

		break;

	    case ERROR:
		if (falselevel == 0)
		{
			const char	*msg;

			for (cp = (char *)nextp; CATEG (cp) == SEPAR; cp++)
				/* vazio */;

			msg = cp;

			while (*cp != '\n' && (*cp != '/' || cp[1] != '*'))
				cp++;

			c = *cp; *cp = '\0';

			err_msg (ERRO, NOMARK, msg);

			*cp = c; nextp = msg; putoutnl ();
		}

		break;

	    case PRAGMA:
		if (falselevel == 0)
			dopragma ();
		break;

	    case LINE:
		if (falselevel == 0)
		{
			if (scan () != ICTE)
			{
				err_msg
				(
					ERRO,
					TOKMARK,
					"Esperava uma constante"
				);
			};

			lineno = token.l_ival - 1;

			if (scan () == STR)
			{
				srcname = token.l_begin + 1;
				((char *)token.l_end)[0] = '\0';
				scan ();
			}

			return;
		}
		break;

	    default:
		err_msg
		(	COMP, TOKMARK,
			"Erro no s_index para diretivas"
		);

	}	/* end switch (sp->s_index) */

	skipline ();

}	/* end directive */

/*
 ****************************************************************
 *	Verifica o estado final da STACK			*
 ****************************************************************
 */
void
stackcheck (void)
{
	if (stackp != stack)
	{
		err_msg (ERRO, NOMARK, "Faltaram \"endif's\"");
	}
	else
	{
		if (falselevel)
		{
			err_msg
			(	COMP, NOMARK,
				"parsefile: falselevel não zerou: %d",
				falselevel
			);
		}
	}

}	/* end stackcheck */

/*
 ****************************************************************
 *	Escreve um '\n' no arquivo de saída			*
 ****************************************************************
 */
void
putoutnl (void)
{
	if (!Pflag)
		{ lineout++; putc ('\n', fpout); }

}	/* end putoutnl */

/*
 ****************************************************************
 *	Diz a Linha do Arquivo de Saída				*
 ****************************************************************
 */
void
putlineno (int n)
{
	static int		lastline  = -1;
	static const char	*lastfile = NOSTR;

	if (!Pflag)
	{
		if (lastline == n && lastfile == srcname)
		{
			error ("Linha %d: %d repetida", lineno, n);
		}
		else
		{
			fprintf
			(
				fpout,
				"# %d",
				lastline = n
			);

			if (lastfile != srcname)
			{
				fprintf
				(
					fpout,
					" \"%s\"",
					lastfile = srcname
				);
			}

			putc ('\n', fpout);
		}
	}

	lineout = n;

}	/* end putlineno */
