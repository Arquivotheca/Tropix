/*
 ****************************************************************
 *								*
 *			mksym.c					*
 *								*
 *	Gera a tabela hash e os símbolos pre-definidos		*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.2.3, de 14.12.99				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <mkasmtb.h>
#include <string.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Nome e versão do programa				*
 ****************************************************************
 */
entry	const char	pgversion[]	= "Versão 3.2.3, de 14.12.99";

/*
 ****************************************************************
 *	Estruturas a serem geradas				*
 ****************************************************************
 */
entry	char	nullstr[] = "";		/* Cadeia vazia */
entry	char	*symtb;			/* Tabela de símbolos */
entry	HASH	hashtb[HASHSZ];		/* Tabela hash */
entry	MACRO	*at_macro,		/* Entrada da macro $@ */
		*star_macro,		/* Entrada da macro $* */
		*ask_macro,		/* Entrada da macro $? */
		*minor_macro;		/* Entrada da macro $< */

/*
 ****************************************************************
 *	Macros para alinhamento					*
 ****************************************************************
 */
#define	ALIGNSZ		(sizeof (long))
#define	ROUND(x)	((((x) + (ALIGNSZ - 1)) / ALIGNSZ) * ALIGNSZ)

/*
 ****************************************************************
 *	Símbolos pre-definidos					*
 ****************************************************************
 */
typedef	struct
{
	char	*p_name;	/* Nome */
	STYPE	p_type;		/* Tipo */
	MACRO	**p_macro;	/* Ponteiro para macro */

}	PREDEF;

entry	PREDEF	predef_symbols[] =
{
	{ "@",		S_NAME,		&at_macro	},
	{ "*",		S_NAME,		&star_macro	},
	{ "?",		S_NAME,		&ask_macro	},
	{ "<",		S_NAME,		&minor_macro	},
	{ ".putenv",	S_PUTENV,	(MACRO **)NULL	},
	{ ".exist",	S_EXIST,	(MACRO **)NULL	},
	{ ".virtual",	S_VIRTUAL,	(MACRO **)NULL	},
	{ NOSTR,	S_NAME,		(MACRO **)NULL	}
};

#define	NPREDEF		(sizeof (predef_symbols) / sizeof (PREDEF) - 1)

/*
 ****************************************************************
 *	Tabelas de relocação para "mkasmtb"			*
 ****************************************************************
 */
entry	OBJENT	hashtb_rel[] =
{
	{ sizeof (HASH *), offsetof (HASH, h_sym),
		1, "symtb", NOVOID },	/* NOVOID será substituído por symtb */
	{ 0 }
};

entry	OBJENT	symtb_rel[] =
{
	{ sizeof (char *), offsetof (SYM, s_macro.m_begin),
		1, "nullstr", nullstr },
	{ sizeof (char *), offsetof (SYM, s_macro.m_line),
		1, "nullstr", nullstr },
	{ 0 }
};

entry	OBJENT	entry_rel[] =
{
	{ sizeof (MACRO *), 0,
		1, "symtb", NOVOID },	/* NOVOID será substituído por symtb */
	{ 0 }
};

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	PREDEF	*pd;
	int	len, entrysz, symtbsz;
	char	*cp;
	HASH	*hp;
	SYM	*sp;
	MACRO	*mp;

	/*
	 *	Calcula o tamanho da entrada, em função dos comprimentos
	 *	dos símbolos a serem instalados.
	 */
	for (entrysz = 0, pd = predef_symbols; pd->p_name != NOSTR; pd++)
	{
		if ((len = strlen (pd->p_name)) > entrysz)
			entrysz = len;
	}

	/*
	 *	"entrysz" deve ser múltiplo de sizeof (long).
	 */
	entrysz = ROUND (SYM_FIXED_SZ + entrysz + 1);
	symtbsz = NPREDEF * entrysz;

	/*
	 *	Aloca memória para a "symtb".
	 */
	if ((symtb = malloc (symtbsz)) == NOSTR)
	{
		fprintf (stderr, "%s: Memória insuficiente\n", pgname);
		exit (2);
	}

	/*
	 *	Instala os símbolos, com seus tipos.
	 */
	for
	(	pd = predef_symbols, cp = symtb;
		pd->p_name != NOSTR;
		pd++, cp += entrysz
	)
	{
		hp = hash (pd->p_name);
		hp->h_sym  = sp = (SYM *)cp;

		(void)memset (&sp->s_node, 0, sizeof (DEPEN));

	/***	sp->s_node.d_sibling = NOSYM;	***/
	/***	sp->s_node.d_flags   = 0;	***/
	/***	sp->s_node.d_nline   = 0;	***/
	/***	sp->s_node.d_next    = NOSYM;	***/

		(void)strcpy (sp->s_name, pd->p_name);
		sp->s_namelen = strlen (sp->s_name);
		sp->s_isid    = FALSE;

		if ((sp->s_type = pd->p_type) == S_NAME)
		{
			mp = &sp->s_macro;
			mp->m_begin = mp->m_line = nullstr;
			*pd->p_macro = mp;
		}
	}

	/*
	 *	Gera as estruturas.
	 */
	mkasmtb
	(	"nullstr.s",
		"nullstr",
		nullstr,
		sizeof (nullstr),
		RO,
		sizeof (nullstr),
		NOOBJENT
	);

	mkasmtb
	(	"symtb.s",
		"symtb",
		symtb,
		symtbsz,
		RW,
		entrysz,
		symtb_rel
	);

	hashtb_rel[0].o_rel_addr = symtb;

	mkasmtb
	(	"hashtb.s",
		"hashtb",
		hashtb,
		sizeof (hashtb),
		RW,
		sizeof (HASH),
		hashtb_rel
	);


	entry_rel[0].o_rel_addr  = symtb;

	mkasmtb
	(	"at_macro.s",
		"at_macro",
		&at_macro,
		sizeof (at_macro),
		RW,
		sizeof (at_macro),
		entry_rel
	);

	mkasmtb
	(	"star_macro.s",
		"star_macro",
		&star_macro,
		sizeof (star_macro),
		RW,
		sizeof (star_macro),
		entry_rel
	);

	mkasmtb
	(	"ask_macro.s",
		"ask_macro",
		&ask_macro,
		sizeof (ask_macro),
		RW,
		sizeof (ask_macro),
		entry_rel
	);

	mkasmtb
	(	"minor_macro.s",
		"minor_macro",
		&minor_macro,
		sizeof (minor_macro),
		RW,
		sizeof (minor_macro),
		entry_rel
	);

	exit (0);

}	/* end main */
