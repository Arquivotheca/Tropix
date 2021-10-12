/*
 ****************************************************************
 *								*
 *			expand.c				*
 *								*
 *	Macro-expansão						*
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
#include <setjmp.h>
#include <string.h>
#include <time.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Variáveis						*
 ****************************************************************
 */
#define	STDC	'1'		/* Já é ANSI */

#define INCLINENO(lp)	if (lp >= program && lp < endprogram)	lineno++

typedef enum {	NO_RECUR, RECUR }	RECURSION;

entry int	definedstate;	/* Estado do defined: 1 => processa */

const char	unbekannt[] = "?\n"; /* Argumento faltando */

extern jmp_buf	eof_env;	/* Ponto de EOF */

extern jmp_buf	fatal_env;	/* Ponto de erro fatal */

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern void	lineexpand (const char *, char **, RECURSION);

/*
 ****************************************************************
 *	Examina a linha e expande as macros 			*
 ****************************************************************
 */
const char *
expand (const char *buf, char *area)
{
	char		*areap;

	areap = area; *areap++ = '\n';

	lineexpand (buf, &areap, RECUR);

	*areap++ = '\n'; *areap++ = '\0';

	return (area + 1);

}	/* end expand */

/*
 ****************************************************************
 *	Expande a linha						*
 ****************************************************************
 */
void
lineexpand (const char *buf, char **areapp, RECURSION recursion)
{
	const char	*lp;
	SYMTB		*sp;
	char		c;
	char		*namep;
	char		del;
	SYMTB		**link_place;

	/*
	 *	Analisa a linha	- esta linha pode ser a linha original,
	 *	o corpo de uma macro, ou ainda, o argumento da chamada
	 *	de uma macro - em qualquer caso, terminado por '\n'.
	 */
	for (lp = buf; (c = *lp++) != '\n'; /* vazio */)
	{
		if   (c == '\\')
		{
			if (*lp == '\n')
				{ lp++; INCLINENO (lp); }
			else
				{ *(*areapp)++ = c; }
			continue;
		}
		elif (c == '\'' || c == '"')
		{
			del = c;
			*(*areapp)++ = c;

			while ((c = *lp++) != del && c != '\n')
			{
				if (c == '\\')
				{
					if ((c = *lp++) == '\n')
						{ INCLINENO (lp); continue; }
					*(*areapp)++ = '\\';
				}

				*(*areapp)++ = c;
			}
			if (c == '\n')
				break;
			*(*areapp)++ = c;
			continue;
		}
		elif (c == '/' && lp[0] == '*')
		{
			nextp = lp; skipcomment ();
			if (!Cflag)
				{ lp = nextp; continue; }
			lp--;
			while (lp < nextp)
				*(*areapp)++ = *lp++;
			continue;
		}
		elif (categ[c] != LETTER)
		{
			*(*areapp)++ = c; continue;
		}

		/*
		 *	Trata-se de uma letra 
		 */
		namep = (*areapp); lp--;

		do
		{
			*(*areapp)++ = *lp++;
		}
		while ((c = CATEG (lp)) == LETTER || c == DIGIT);

		if (macdef[namep[0]] == 0)
			continue;

		*(*areapp) = '\0';

		if (!TYPE (sp, namep, S_MACRO))
			continue;

		if (sp->s_expand && recursion == NO_RECUR)
			continue;

		sp->s_expand++;

		lp = macexpand (sp, lp, namep, areapp);

		sp->s_expand--;

	}	/* end for */

	nextp = lp;

}	/* end lineexpand */

/*
 ****************************************************************
 *	Macro-Expansão						*
 ****************************************************************
 */
const char *
macexpand (SYMTB *sp, const char *lp, char *namep, char **areapp)
{
	char		*cp;
	char		c;
	int		n;
	int		plevel, inquote, inplick, lines;
	const char	*mb;
	const char	*ap;
	const char	*macargb[ARGSZ];
	char		corpo[2*MACSZ];
	char		*corpop;
	char		argum[MACSZ];

	/*
	 *	Trata as Macros Intrínsecas.
	 */
	if (sp->s_flags & S_BUILTIN)
		return (builtin (sp, lp, namep, areapp));

	n = 0;

	/*
	 *	Processa os argumentos
	 *
	 *	Convenção:	nargs == -1 -> Macro do tipo OBJETO
	 *			nargs >=  0 -> Macro do tipo FUNÇÃO
	 */
	if (sp->s_nargs >= 0)
	{
		/*
		 *	Pula o espaço entre o nome da macro
		 *	e o começo dos argumentos
		 */
		for (lines = 0, cp = (char *)lp; /* vazio */; /* vazio */)
		{
			if   (categ[(c = *cp++)] == SEPAR)
				continue;
			elif (c == '\0')
				longjmp (eof_env, 1);
			elif (c == '\n')
			{
				if (cp >= program && cp < endprogram)
					lines++;
				continue;
			}
			else
				{ cp--; break; }

		}	/* end for (EVER) */

		/*
		 *	Se o nome da macro não for seguido de '(', não expande
		 *	Examina o caso "(nome) ()"
		 */
		if (*cp != '(')
		{
			if (*cp != ')')
				return (lp);

			for (ap = cp, cp = namep - 1; *cp != '\n'; cp--)
			{
				if   (*cp == '(')
					{ lp = ap + 1; *cp = ' '; break; }
				elif (categ[*cp] != SEPAR)
					break;
			}

			return (lp);
		}

		lineno += lines;
		mb = cp;
		lp = cp + 1;
		cp = argum;

		/*
		 *	Identifica o início e final de cada argumento
		 *	Atenção com nível de parênteses.
		 */
		do
		{
			/*
			 *	Fora de um argumento
			 */
			for (EVER)
			{
				if   (categ[(c = *lp++)] == SEPAR)
					continue;
				elif (c == '\0')
					longjmp (eof_env, 1);
				elif (c == '\n')
					{ INCLINENO (lp); continue; }
				else
					{ lp--; break; }

			}	/* end for (EVER) */

			/*
			 *	Achou o começo de um argumento
			 */
			macargb[n] = cp;
			plevel = 1;
			inquote = inplick = 0;

			if (cp > argum + sizeof (argum) - 100) 
				goto argum_err;

			/*
			 *	Dentro de um argumento
			 */
			for (EVER)
			{
				switch (c = *lp++)
				{
				    case '\n':
					if (cp > argum + sizeof (argum) - 100)
					{
					    argum_err:
						err_msg
						(
							ERRO, NOMARK,
							"Lista de argumentos "
							"da macro \"%s\" "
							"com tamanho excessivo",
							sp->s_name
						);
						longjmp (fatal_env, 1);
					}

					INCLINENO (lp);
					c = ' ';
					break;
	
				    case '"':
					if (!inplick)
						inquote = !inquote;
					break;
	
				    case '\'':
					if (!inquote)
						inplick = !inplick;
					break;
	
				    case '(':
					if (!inquote && !inplick)
						plevel++;
					break;
	
				    case ')':
					if (!inquote && !inplick)
					{
						if (--plevel <= 0)
							goto found;
					}
					break;
	
				    case ',':
					if (!inquote && !inplick && plevel <= 1)
						goto found;
					break;
	
				    case '\\':
					if ((c = *lp++) == '\n')
						{ INCLINENO (lp); continue; }
					else
						{ *cp++ = '\\'; }
					break;
	
				    case '/':
					if (*lp != '*' || inquote || inplick)
						break;

					nextp = lp;
					skipcomment ();
					lp = nextp;
					c = ' '; /* Comentário => 1 <sp> */
					break;

				    case '\0':
					longjmp (eof_env, 1);

				}	/* end switch */

				*cp++ = c;

			}	/* end for (EVER) */

		    found:
			cp--;
			while (cp >= macargb[n] && CATEG (cp) ==  SEPAR)
				cp--;
			cp++;
			*cp++ = '\n';

			n++;

		}	while (c != ')');

		/*
		 *	Pequeno ajuste necessário para "()".
		 */
		if (n == 1 && macargb[0][0] == '\n')
			n = 0;

		/*
		 *	Confere o número de argumentos.
		 */
		if (n != sp->s_nargs)
		{
			err_msg
			(
				ERRO, MYMARK,
				"O número de argumentos para \"%s\" não confere",
				sp->s_name, mb, lp - 1
			);

			for (ap = unbekannt; n < sp->s_nargs; n++)
				macargb[n] = ap;
		}

	}	/* end 	if (nargs >= 0) */

#define	DEBUG
#ifdef	DEBUG
	if (dflag)
		printargs (sp, macargb, n);
#endif	DEBUG

	/*
	 *	Retira o nome da macro
	 */
	(*areapp) = namep;

	/*
	 *	Prepara a expansão do corpo
	 */
	corpop = corpo; *corpop++ = '\n';

	/*
	 *	Realiza a substituição
	 */
	for (cp = (char *)sp->s_macro; (c = *cp++) != '\n'; /* sem incr. */)
	{
		if   (c == CARG)
		{
			/*
			 *	Argumento normal.
			 */
			lineexpand (macargb[*cp++], &corpop, RECUR);
		}
		elif (c == CSTR)
		{
			/*
			 *	Argumento a ser transformado em cadeia.
			 */
			ap = macargb[*cp++];
			inquote = inplick = 0;

			*corpop++ = '"';

			for (EVER)
			{
				switch (c = *ap++)
				{
				    case '\n':
					goto str_out;
	
				    case '\'':
					if (!inquote)
						inplick = !inplick;
					goto normal;
	
				    case '"':
					*corpop++ = '\\';
					if (!inplick)
						inquote = !inquote;
					goto normal;
	
				    case '\\':
					if (inplick || inquote)
						*corpop++ = '\\';
				    default:
				    normal:
					*corpop++ = c;

				}	/* end switch */

			}	/* end while */

		     str_out:
			*corpop++ = '"';
		}
		elif (c == CGLUE)
		{
			/*
			 *	Argumento a não ser modificado
			 */
			for (ap = macargb[*cp++]; *ap != '\n'; /* vazio */)
				*corpop++ = *ap++;
		}
		else
		{
			if (corpop > corpo + sizeof (corpo) - 100) 
			{
				err_msg
				(
					ERRO, NOMARK,
					"A expansão do corpo da macro \"%s\" "
					"ultrapassou o tamanho máximo",
					sp->s_name
				);
				longjmp (fatal_env, 1);
			}

			*corpop++ = c;
		}

	}	/* end for pelo corpo da macro */

	*corpop++ = '\n';

	/*
	 *	Realiza a nova expansão no corpo
	 */
	lineexpand (corpo + 1, areapp, NO_RECUR);

	return (lp);

}	/* end macexpand */

/*
 ****************************************************************
 *	Trata as Macros Instrínsecas				*
 ****************************************************************
 */
const char *
builtin (SYMTB *sp, const char *lp, char *bp, char **areapp)
{
	const char	*cp;
	time_t		time_clock;
	MAJOR		M;
	int		lparen = 0;
	SYMTB		**link_place;

	switch (sp->s_index)
	{
	    case _LINE:
		sprintf (bp, "%d", lineno);
		bp = strend (bp);
		break;

	    case _FILE:
		*bp++ = '"';
		strcpy (bp, srcname);
		bp = strend (bp);
		*bp++ = '"';
		break;

	    case _DATE:
		time (&time_clock);
		cp = btime (&time_clock);
		memcpy (bp, cp + 13, 11);
		bp += 11;
		break;

	    case _TIME:
		time (&time_clock);
		cp = btime (&time_clock);
		memcpy (bp, cp, 8);
		bp += 8;
		break;

	    case _DEFINED:
		if (definedstate == 0)
		{
			strcpy (bp, "defined");
			bp += 7;
			break;
		}

		/*
		 *	Processo o "defined"
		 */
		nextp = lp;

		if ((M = scan ()) == LP)
			{ lparen++; M = scan (); }

		lp = nextp;

		if (M != ID)
		{
			err_msg (ERRO, TOKMARK, "Esperava um identificador");
			*bp++ = '0';
			break;
		}

		if (lparen)
		{
			if (scan () == RP)
				lp = nextp;
			else
				err_msg (ADVR, TOKMARK, "Esperava um ')'");
		}
		if (TYPE (sp, token.l_name, S_MACRO))
			*bp++ = '1';
		else
			*bp++ = '0';

		break;

	    case _STDC:
		*bp++ = STDC;
		break;

	    default:
		err_msg (COMP, NOMARK, "Macro intrínseca inválida");
	}

	(*areapp) = bp;

	return (lp);

}	/* end builtin */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime os Argumentos					*
 ****************************************************************
 */
void
printargs (SYMTB *sp, const char *macargb[], int n)
{
	const char	*cp;
	int		i, c;
	int		virg = 0;

	fprintf (stderr, "DEBUG => %s (", sp->s_name);

	for (i = 0; i < n; i++)
	{
		if (virg++)
			fprintf (stderr, ",");

		fprintf (stderr, "\ed8\ee");

		for (cp = macargb[i]; (c = *cp++) != '\n'; /* vazio */)
			putc (c, stderr);

		fprintf (stderr, "\ed8\ee");
	}

	fprintf (stderr, ")\n");

}	/* end printargs */

/*
 ****************************************************************
 *	Imprime uma linha expandida				*
 ****************************************************************
 */
void
printexpline (const char *msg, const char *line)
{
	int		c;

	fprintf (stderr, "DEBUG => %s", msg);
	fprintf (stderr, "\ed8\ee");

	while ((c = *line++) != '\n')
		putc (c, stderr);

	fprintf (stderr, "\ed8\ee\n");

}	/* end printexpline */

#else

/*
 ****************************************************************
 *	Imprime uma linha expandida				*
 ****************************************************************
 */
void
printexpline (const char *msg, const char *line)
{
	fprintf (stderr, "DEBUG => %s \"%s\"\n", msg, line);

}	/* end printexpline */
#endif	DEBUG
