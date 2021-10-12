/*
 ****************************************************************
 *								*
 *			include.c				*
 *								*
 *	Processamento da diretiva "#include"			*
 *								*
 *	Versão	1.0.0, de 02.09.88				*
 *		3.0.1, de 13.07.97				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*	
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Estrutura da Pilha					*
 ****************************************************************
 */
#define	STACKSZ		20

typedef	struct
{
	const char	*t_program;	/* Texto */
	const char	*t_endprogram;	/* Fim do Texto */
	const char	*t_srcdir;	/* Diretório do programa */
	const char	*t_nextp;	/* Próximo Caractere */
	const char	*t_name;	/* Nome do Arquivo */
	int		t_lineno;	/* Número da Linha */

}	STACK;

/*
 ****************************************************************
 *	A Pilha de Includes					*
 ****************************************************************
 */
static STACK	stack[STACKSZ],	/* Pilha para Includes */
		*stackp = stack;

/*
 ****************************************************************
 *	Processa a Diretiva "#include"				*
 ****************************************************************
 */
void
doinclude (void)
{
	const char	*lp;
	char		c;
	const char	*name, *filenm;
	char		area[MACSZ];

	/*
	 *	Pula os brancos iniciais
	 */
	for (lp = nextp; (c = *lp) == ' ' || c == '\t'; lp++)
		/* vazio */;

	/*
	 *	Verifica qual é a forma
	 */
	if (c == '<' || c == '"')
	{
		/*
		 *	Forma 	#include "stdio.h"
		 *	ou 	#include <stdio.h>
		 */
		name = lp;

		skipline ();
	}
	else
	{
		/*
		 *	Forma 	#include stdio.h
		 */
		c = '"';
		name = expand (lp, area);
		lineno++;
	}

	if ((filenm = mkname (name)) == NOSTR)
	{
		lineno--;
		*(strchr (name, '\n')) = '\0';
		err_msg
		(	ERRO, NOMARK,
			"Nome inválido de arquivo: \"%s\"\n",
			name
		);
		lineno++;
		return;
	}

	if ((name = searchfile (filenm, c)) == NOSTR)
	{
		lineno--;
		err_msg (ERRO, NOMARK, "Arquivo \"%s\" não encontrado", filenm);
		lineno++;
		return;
	}

	if (push () < 0)
	{
		err_msg
		(
			ERRO,
			NOMARK,
			"Excesso de inclusões aninhadas"
		);
	}
	else
	{
		srcname = name;

		if (readfile (srcname) < 0)
			pop ();
		else
			putlineno (1);
	}

}	/* end doinclude */

/*
 ****************************************************************
 *	Empilha Informações do Arquivo Atual			*
 ****************************************************************
 */
int
push (void)
{
	STACK		*sp;

	if ((sp = stackp++) >= &stack[STACKSZ])
		return (-1);

	sp->t_program    = program;
	sp->t_endprogram = endprogram;
	sp->t_srcdir	 = incld[0];
	sp->t_lineno     = lineno;
	sp->t_nextp      = nextp;
	sp->t_name       = srcname;

	return (0);

}	/* end push */

/*
 ****************************************************************
 *	Desempilha Informações do Arquivo Atual			*
 ****************************************************************
 */
int
pop (void)
{
	STACK		*sp;

	/*
	 *	Libera o buffer anteriormente alocado.
	 */
	free ((char *)program - 4);	/* Cuidado: 4 definido em ../scan.c  */

	if ((sp = --stackp) < stack)
		return (-1);

	program    = sp->t_program;
	endprogram = sp->t_endprogram;
	incld[0]   = sp->t_srcdir;
	lineno     = sp->t_lineno;
	nextp      = sp->t_nextp;
	srcname    = sp->t_name;

	/*
	 *	Emite o número da linha. Repare que ele foi
	 *	empilhado já incrementado.
	 */
	putlineno (lineno);

	return (0);

}	/* end pop */

/*
 ****************************************************************
 *	Procura o Arquivo a ser Incluído			*
 ****************************************************************
 */
const char *
searchfile (const char *name, char del)
{
	const char	**ip;
	char		*ep;
	const char	*endp;

	/*
	 *	A ordem de busca é:
	 *
	 *		- Diretório do programa fonte (só com '"')
	 *		- Diretório dados nos diversos "-I"
	 *		- "/usr/include"
	 */
	if (dflag)
	{
		fprintf
		(	stderr,
			"searchfile: Procurando por \"%s\" em:\n",
			name
		);

		for (ip = (del == '"' ? incld : incld + 1); *ip; ip++)
			fprintf	(stderr, "\t%s\n", *ip);
	}

	/*
	 *	Se começa por '/' ...
	 */
	if (name[0] == '/')
		return (name);

	/*
	 *	Busca nos diversos diretórios
	 */
	if ((ep = malloc (MACSZ)) == NOSTR)
		error (NOSTR);

	for (ip = (del == '"' ? incld : incld + 1); *ip; ip++)
	{
		endp = mstrcat (ep, *ip, "/", name, NOSTR);

		if (access (ep, R_OK) >= 0)
		{
			ep = realloc (ep, endp - ep);
			return (ep);
		}
	}

	free (ep);

	return (NOSTR);

}	/* end searchfile */

/*
 ****************************************************************
 *	Concatenador genérico					*
 ****************************************************************
 */
const char *
mstrcat (char *dst, ...)
{
	va_list		 ap;
	char		*lp, c;
	const char	*cp;

	va_start (ap, dst);

	lp = dst;

	while ((cp = va_arg (ap, char *)) != NOSTR)
	{
		while (c = *cp++)
			*lp++ = c;
	}

	*lp++ = '\0';

	va_end (ap);

	return (lp);

}	/* end mstrcat */

/*
 ****************************************************************
 *	Procura Algo semelhante a um Nome			*
 ****************************************************************
 */
const char *
mkname (const char *name)
{
	const char	*cp;
	char		*ep, c;

	for (cp = name; (c = *cp) == ' ' || c == '\t'; cp++)
		/* vazio */;

	if (c == '\0' || c == '\n')
		return (NOSTR);

	if   (c == '"')
	{
		cp++;
		if ((ep = strchr (cp, '"')) == NOSTR)
			return (NOSTR);
		*ep = '\0';
	}
	elif (c == '<')
	{
		cp++;
		if ((ep = strchr (cp, '>')) == NOSTR)
			return (NOSTR);
		*ep = '\0';
	}
	else
	{
		if ((ep = strchr (cp + 1, '\n')) == NOSTR)
			return (NOSTR);

		while (ep > cp && ((c = *--ep) == ' ' || c == '\t'))
			/* vazio */;
		*++ep = '\0';
	}

	return (cp);
	
}	/* end mkname */
