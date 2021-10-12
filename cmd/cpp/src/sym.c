/*
 ****************************************************************
 *								*
 *			   sym.c				*
 *								*
 *	Gerência da tabela de símbolos				*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 22.08.01				*
 *								*
 *	Módulo:	CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>

#include "../h/scan.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Função de Hash						*
 ****************************************************************
 */
#include "../h/hash.h"

/*
 ****************************************************************
 *	Imprime a Tabela de Símbolos				*
 ****************************************************************
 */
void
printsymtb (void)
{
	SYMTB		**hp;
	const SYMTB	*sp;
	const char	*cp;
	char		c;

	fprintf (stderr, "\n******** TABELA DE SÍMBOLOS ********\n\n");

	for (hp = &hashtb[0]; hp < &hashtb[HASHSZ]; hp++)
	{
		for (sp = *hp; sp != NOSYMTB; sp = sp->s_link)
		{
			if ((sp->s_flags & (S_MACRO|S_PREUNDEF)) == 0)
				continue;
	
			fprintf (stderr, "%-14.14s ", sp->s_name);
	
			fprintf (stderr, "%P ", sp->s_macro);
	
			fprintf (stderr, "%3d  ", macdef[sp->s_name[0]]);
	
			fprintf (stderr, "%-14.14s ", flagedit (sp->s_flags));
	
			if (sp->s_macro == NOSTR)
			{
				putc ('\n', stderr);
				continue;
			}
	
			for (cp = sp->s_macro; *cp != '\n'; /* sem incr. */)
			{
				if   ((c = *cp++) == CARG)
					fprintf	(stderr, "<%d>", *cp++);
				elif (c == CSTR)
					fprintf	(stderr, "[%d]", *cp++);
				elif (c == CGLUE)
					fprintf	(stderr, "{%d}", *cp++);
				else
					putc (c, stderr);
			}
	
			putc ('\n', stderr);

		}	/* end lista de colisão */

	}	/* end hashtb */

	fprintf (stderr, "\n******** FIM ********\n");

}	/* end printsymtb */

/*
 ****************************************************************
 *	Tabela de flags 					*
 ****************************************************************
 */
typedef struct
{
	char	r_flagnm[12];	/* Nome do flag */
	int	r_flag;		/* Máscara do flag */

}	RFLAG;

const RFLAG flagset[] =
{
	"KEYWORD",	S_KEYWORD,
	"MACRO",	S_MACRO,
	"PREUNDEF",	S_PREUNDEF,

	"PREDEF",	S_PREDEF,
	"ENV",		S_ENV,
	"BUILTIN",	S_BUILTIN,
	""
};

/*
 ****************************************************************
 *	Edita um conjunto de flags				*
 ****************************************************************
 */
const char *
flagedit (int flags)
{
	const RFLAG	*rp;
	int		ou;
	static char	buf[32];

	if (flags == 0)
		return ("");

	ou = 0; buf[0] = '\0';

	for (rp = flagset; rp->r_flagnm[0]; rp++)
	{
		if (flags & rp->r_flag)
		{
			if (ou++)
				strcat (buf, "|");

			strcat (buf, rp->r_flagnm);
		}
	} 

	return (buf);

}	/* end flagedit */
