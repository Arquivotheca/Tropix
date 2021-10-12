/*
 ****************************************************************
 *								*
 *			define.c				*
 *								*
 *	Processamento das diretivas "#define" e "#undef"	*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
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
#include <setjmp.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Definições Locais					*
 ****************************************************************
 */
static const char	macropdef[] = "1\n";	/* Para Macros Pré-Definidas */

static char		*arg[ARGSZ];		/* Para Coletar os Argumentos */
static int		nargs;

extern jmp_buf		fatal_env;		/* Ponto de erro fatal */

/*
 ****************************************************************
 *	Analisa a Diretiva #define				*
 ****************************************************************
 */
void
dodefine (void)
{
	SYMTB		*sp, **link_place;
	int		n;
	char		*cp, c;
	const char	*lp;
	char		*macro_begin;
	const char	*space_end;
	const char	*id_begin, *id_end;
	int		id_lno;

	eof_state = 0;

	if (scan () != ID)
	{
		err_msg
		(	ERRO, TOKMARK,
			"Esperava um identificador. Linha ignorada"
		);
		putoutnl ();
		return;
	}

#ifdef	DEBUG
	error ("define: nome = \"%s\"", token.l_name);
#endif	DEBUG

	id_begin = token.l_begin; id_end = token.l_end; id_lno = token.l_line;

	/*
	 *	Analisa o conteúdo da SYMTB para este nome
	 */
	if ((sp = hash (token.l_name, &link_place)) == NOSYMTB)
	{
		/*
		 *	É um nome novo
		 */
		if ((sp = malloc (sizeof (SYMTB))) == NOSYMTB)
			error (NOSTR);

		if ((sp->s_name = strdup (token.l_name)) == NOSTR)
			error (NOSTR);

		sp->s_flags = S_NULL;

		*link_place = sp; sp->s_link = NOSYMTB;
	}
	else switch (sp->s_flags & (S_MACRO|S_PREUNDEF|S_BUILTIN))
	{
		/*
		 *	É uma entrada vazia que pode ser reaproveitada
		 */
	    case S_NULL:
		break;

		/*
		 *	É um nome para não definir como macro
		 */
	    case S_PREUNDEF:
		break;

		/*
		 *	Já é uma macro intrínseca
		 */
	    case (S_MACRO|S_BUILTIN):
		err_msg
		(	ADVR, TOKMARK,
			"Macros intrínsecas não podem ser redefinidas"
		);
		break;

		/*
		 *	É a redefinição de uma macro antiga
		 */
	    case S_MACRO:
		break;

		/*
		 *	É algo incompreensível
		 */
	    default:
		err_msg
		(	COMP, TOKMARK,
			"Tipo incompreensível de símbolo: %s",
			flagedit (sp->s_flags)
		);
		break;

	}	/* end switch */

	/*
	 *	Convenção:	nargs == -1 -> Macro do tipo OBJETO
	 *			nargs >=  0 -> Macro do tipo FUNÇÃO
	 */
	n = 0; nargs = -1;

	/*
	 *	O parêntese deve vir grudado no nome da macro.
	 */
	if (*nextp == '(')
	{
		eof_state = 1; nargs = 0; nextp++;

		if (scan () != RP) for (EVER)
		{
			if (token.l_major != ID)
			{
				err_msg
				(	ERRO, TOKMARK,
					"Esperava um identificador ou ')'"
				);
				goto skip;
			}

			if (n >= ARGSZ)
			{
				err_msg
				(
					ADVR, TOKMARK,
					"Número excessivo de argumentos. Definição ignorada"
				);

				goto skip;
			}

			if ((arg[n++] = strdup (token.l_name)) == NOSTR)
				error (NOSTR);
#if (0)	/*******************************************************/
			if ((arg[n] = malloc (token.l_end - token.l_begin + 1)) == NOSTR)
				error (NOSTR);

			strcpy (arg[n++], token.l_name);
#endif	/*******************************************************/

			if (scan () == RP)
			{
				nargs = n;
				break;
			}

			if (token.l_major != CM)
			{
				err_msg
				(	ERRO, TOKMARK,
					"Esperava uma vírgula ou ')'"
				);
				goto skip;
			}

			scan ();

		}	/* end if != ')' for (EVER) */

	}	/* end  if (*nextp == '(') */

	/*
	 *	Pula o espaço entre o final dos parâmetros e início do corpo
	 */
	for (lp = nextp; /* vazio */; /* vazio */)
	{
		if   ((c = *lp) == '\\')
		{
			if ((c = lp[1]) == '\n')
				{ lp += 2; lineno++; putoutnl (); }
			else
				{ break; }
		}
		elif (c == '/' && lp[1] == '*')
		{
			nextp = ++lp; skipcomment (); lp = nextp;
		}
		elif (categ[c] != SEPAR)
		{
			break;
		}
		else
		{
			lp++;
		}

	}	/* end for */

	/*
	 *	Aloca memória para a MACRO
	 */
	if ((macro_begin = cp = malloc (2 * MACSZ)) == NOSTR)
		error (NOSTR);

	space_end = cp + (2 * MACSZ);

	eof_state = 0;

	/*
	 *	Armazena o corpo da MACRO
	 */
	while ((c = *lp) != '\n')
	{
		if (categ[c] == LETTER)
		{
			nextp = lp;
			scan ();

			if ((n = findarg ()) >= 0)
			{
				*cp++ = CARG; *cp++ = n; lp = nextp;
			}
			else
			{
				while (lp < nextp)
					*cp++ = *lp++;
			}
		}
		elif (c == '\\')
		{
			if ((c = lp[1]) == '\n')
			{
				if (cp >= space_end - 100)
				{
					err_msg
					(	ERRO, NOMARK,
						"Corpo da macro \"%s\" com "
						"tamanho excessivo",
						sp->s_name
					);
					longjmp (fatal_env, 1);
				}

				lp += 2; lineno++; putoutnl ();
			}
			elif (c == '#')
			{
				*cp++ = c; lp += 2;
			}
			else
			{
				*cp++ = '\\'; lp++;
			}
		}
		elif (c == '#')
		{
			if (lp[1] == '#')
			{
				/*
				 *	Operador ##
				 */
				if (cp >= macro_begin + 2)
				{
					if
					(	CATEG (cp - 1) == SEPAR &&
						cp[-2] != CARG &&
						cp[-2] != CGLUE
					)
						cp--;

					if (cp[-2] == CARG)
						cp[-2] = CGLUE;
				}

				lp += 2; nextp = lp;

				if (scan () != ID || (n = findarg ()) < 0)
				{
					while (CATEG (lp) == SEPAR)
						lp++;
					while (lp < nextp)
						*cp++ = *lp++;
					continue;
				}

				*cp++ = CGLUE; *cp++ = n; lp = nextp;

				continue;
			}

			/*
			 *	Operador #
			 */
			nextp = lp + 1;

			if (scan () != ID || (n = findarg ()) < 0)
			{
				while (lp < nextp)
					*cp++ = *lp++;
				continue;
			}

			*cp++ = CSTR; *cp++ = n; lp = nextp;
		}
		elif (c == '/' && lp[1] == '*')
		{
			nextp = ++lp; skipcomment (); lp = nextp;

			if (cp > macro_begin && cp[-1] != ' ')
				*cp++ = ' ';
		}
		elif (categ[c] == SEPAR)
		{
			if (cp > macro_begin && cp[-1] != ' ')
				*cp++ = ' ';
			lp++;
		}
		elif (c == '\'' || c == '"')
		{
			nextp = lp; scan ();

			while (lp < nextp)
			{
				if (lp[0] == '\\' && lp[1] == '\n')
					lp += 2;

				*cp++ = *lp++;
			}
		}
		else
		{
			*cp++ = c;
			lp++;
		}

	}	/* end while */

	/*
	 *	Retira o possível branco do final
	 */
	if (cp > macro_begin && cp[-1] == ' ')
		cp--;

	*cp++ = '\n';

	nextp = lp;
	putoutnl ();

	/*
	 *	Verifica se é um nome que não deve ser definido ou BUILTIN
	 *	(BUILTIN já recebeu a mensagem de erro)
	 */
	if (sp->s_flags & (S_BUILTIN|S_PREUNDEF))
		{ free (macro_begin); free_args_mem (); return; }

	/*
	 *	Macro nova
	 */
	if ((sp->s_flags & S_MACRO) == 0)
	{
		macro_begin = realloc (macro_begin, cp - macro_begin);

		sp->s_flags  |= S_MACRO;
		sp->s_nargs  = nargs;
		sp->s_expand = 0;
		sp->s_macro  = macro_begin;
#ifdef	DEBUG
		error ("define: macro = \"%s\"", macro_begin);
#endif	DEBUG
		macdef[sp->s_name[0]]++;

		free_args_mem ();

		return;
	}

	/*
	 *	É a redefinição de uma macro antiga
	 */
	token.l_begin = id_begin;
	token.l_end   = id_end;
	token.l_line  = id_lno;

	if (sp->s_nargs != nargs)
	{
		err_msg
		(	ERRO, TOKMARK,
			"Macro \"%s\" sendo redefinida com um "
			"número diferente de argumentos",
			sp->s_name
		);

		free (macro_begin);
		free_args_mem ();

		return;
	}

	for (lp = sp->s_macro, cp = macro_begin; *lp == *cp++; /* abaixo */)
	{
		if (*lp++ == '\n')
			{ free (macro_begin); free_args_mem (); return; }
	}

	err_msg
	(	ERRO, TOKMARK,
		"Macro \"%s\" sendo redefinida com um texto diferente",
		sp->s_name
	);

	free (macro_begin);
	free_args_mem ();

	return;

	/*
	 *	Houve erro na definição da Macro.
	 */
    skip:
	eof_state = 0;

	putoutnl ();

}	/* end dodefine */

/*
 ****************************************************************
 *	Verifica se é um Argumento				*
 ****************************************************************
 */
int
findarg (void)
{
	char		**ip;

	/*
	 *	Lembrar que "nargs" pode ser == -1
	 */
	for (ip = &arg[nargs - 1]; ip >= &arg[0]; ip--)
	{
		if (streq (*ip, token.l_name))
			return (ip - arg);
	}

	return (-1);

}	/* end findarg */

/*
 ****************************************************************
 *	Libera o espaço dos argumentos				*
 ****************************************************************
 */
void
free_args_mem (void)
{
	char		**ip;

	for (ip = &arg[nargs - 1]; ip >= &arg[0]; ip--)
	{
		free (*ip);
	}

}	/* end free_args_mem */

/*
 ****************************************************************
 *	Processa a Diretiva "#undef"				*
 ****************************************************************
 */
void
doundef (void)
{
	SYMTB		*sp, **link_place;

	eof_state = 0;

	if   (scan () != ID)
	{
		err_msg
		(	ADVR, TOKMARK,
			"Esperava um identificador. Linha ignorada"
		);
	}
	elif (TYPE (sp, token.l_name, S_MACRO))
	{
		if (sp->s_flags & S_BUILTIN)
		{
			err_msg
			(	ADVR, TOKMARK,
				"Macros intrínsecas "
				"não podem ser indefinidas"
			);
		}
		else
		{
			sp->s_flags &= ~(S_MACRO|S_PREDEF|S_ENV);
			macdef[sp->s_name[0]]--;
		}
	}

	putoutnl ();

}	/* end doundef */

/*
 ****************************************************************
 *	Trata os símbolos predefinidos				*
 ****************************************************************
 */
void
mkpredef (const char *def, int flag)
{
	char		*cp;
	SYMTB		*sp, **link_place;

	/*
	 *	"flag" pode ser: S_PREDEF ou S_ENV
	 */
	if ((cp = strchr (def, '=')) != NOSTR)
		*cp = '\0';

	/*
	 *	Cria uma entrada, se ainda não existe
	 */
	if ((sp = hash (def, &link_place)) == NOSYMTB)
	{
		if ((sp = malloc (sizeof (SYMTB))) == NOSYMTB)
			error (NOSTR);

#if (0)	/*******************************************************/
		if ((sp->s_name = strdup (def)) == NOSTR)
			error (NOSTR);
#endif	/*******************************************************/

		sp->s_name  = def;
		sp->s_flags = S_NULL;

		*link_place = sp;
	   	sp->s_link = NOSYMTB;
	}

	/*
	 *	Se este identificador foi alvo de um "-U", não define a macro
	 */
	if (sp->s_flags & S_PREUNDEF)
	{
		if (flag == S_ENV)
			return;

		fprintf
		(	stderr,
			"A macro \"%s\" já foi indefinida através "
			"de \"-U\"\n\n", def
		);

		help ();
	}

	/*
	 *	Providências a tomar se a macro já existe
	 *	Prioridade decrescente: "-D", "env", interno
	 */
	if (sp->s_flags & S_MACRO)
	{
		if (flag == S_PREDEF)
		{
			if (sp->s_flags & S_BUILTIN)
			{
				fprintf
				(	stderr,
					"A macro intrínseca \"%s\" "
					"não pode ser redefinida\n\n",
					def
				);
			}
			else	/* Macro de "-D" */
			{
				fprintf
				(	stderr,
					"A macro \"%s\" já está definida\n\n",
					def
				);
			}

			help ();
		}
		else	/* flag == S_ENV */
		{
			if   (sp->s_flags & S_PREDEF)
			{
				/*** return ***/;
			}
			elif (sp->s_flags & S_BUILTIN)
			{
				err_msg
				(	ERRO, NOMARK,
					"A macro intrínseca \"%s\" "
					"não pode ser redefinida",
					def
				);
			}
			else	/* Macro de #define ou #pragma */
			{
				err_msg
				(	ERRO, NOMARK,
					"A macro \"%s\" já está definida",
					def
				);
			}

			return;
		}

	}	/* if S_MACRO */

	/*
	 *	Verifica se foi dado o "="
	 *
	 *	Neste ponto, ((sp->s_flags & S_MACRO) == 0)
	 */
	if (cp == NOSTR)
		sp->s_macro = macropdef;
	else
		sp->s_macro = ++cp;

	sp->s_nargs = -1;
	sp->s_expand = 0;

	/*
	 *	Cuidado com redefinições ao trocar o '\0' por '\n'
	 */
	if (strchr (sp->s_macro, '\n') == NOSTR)
		*(strend (sp->s_macro)) = '\n';
	sp->s_flags |= (S_MACRO|flag);
	macdef[def[0]]++;

}	/* end mkpredef */

/*
 ****************************************************************
 *	Processa a Opção "-U"					*
 ****************************************************************
 */
void
mkpreundef (const char *def)
{
	SYMTB		*sp, **link_place;

	if   ((sp = hash (def, &link_place)) == NOSYMTB)
	{
		if ((sp = malloc (sizeof (SYMTB))) == NOSYMTB)
			error (NOSTR);

#if (0)	/*******************************************************/
		if ((sp->s_name = strdup (def)) == NOSTR)
			error (NOSTR);
#endif	/*******************************************************/

		sp->s_name  = def;
		sp->s_flags = S_NULL;

		*link_place = sp;
	   	sp->s_link = NOSYMTB;
	}
	elif (sp->s_flags & S_MACRO)
	{
		if   (sp->s_flags & S_BUILTIN)
		{
			fprintf
			(	stderr,
				"A macro intrínseca \"%s\" "
				"não pode ser apagada\n\n",
				def
			);

			help ();
		}
		elif (sp->s_flags & S_PREDEF)
		{
			fprintf
			(	stderr,
				"A macro \"%s\" já foi definida através "
				"de \"-D\"\n\n", def
			);

			help ();
		}
		else
		{
			sp->s_flags &= ~(S_MACRO|S_PREDEF|S_ENV);
			macdef[def[0]]--;
		}
	}

	sp->s_flags |= S_PREUNDEF;

}	/* end mkpreundef */
