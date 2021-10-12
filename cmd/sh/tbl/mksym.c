/*
 ****************************************************************
 *								*
 *			mksym.c					*
 *								*
 *	Gera a Tabela Hash já Inicializada			*
 *								*
 *	Versão	3.2.2, de 16.09.99				*
 *		4.5.0, de 21.06.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <mkasmtb.h>

#include "../h/sym.h"

/*
 ****************************************************************
 *	Tabela de identificadores pré-definidos			*
 ****************************************************************
 */
typedef	struct
{
	char		*n_name;
	int		n_id;
	STYPE		n_type;

}	NAME;

NAME	nametb [] =
{
/*
 ******	Funções Intrínsecas *************************************
 */
	".",		F_DOT,		S_FUNC,
	":",		F_COLON,	S_FUNC,
	"cd",		F_CD,		S_FUNC,
	"eh",		F_EDIT_HIST,	S_FUNC,
	"eval",		F_EVAL,		S_FUNC,
	"exec",		F_EXEC,		S_FUNC,
	"exit",		F_EXIT,		S_FUNC,
	"export",	F_EXPORT,	S_FUNC,
	"false",	F_FALSE,	S_FUNC,
	"gh",		F_GRAPHIC_HIST,	S_FUNC,
	"h",		F_SHOW_HIST,	S_FUNC,
	"login",	F_LOGIN,	S_FUNC,
	"logout",	F_LOGOUT,	S_FUNC,
	"read",		F_READ,		S_FUNC,
	"readonly",	F_READONLY,	S_FUNC,
	"rehash",	F_REHASH,	S_FUNC,
	"return",	F_RETURN,	S_FUNC,
	"set",		F_SET,		S_FUNC,
	"shid",		F_SHID,		S_FUNC,
	"shift",	F_SHIFT,	S_FUNC,
	"spy",		F_SPY,		S_FUNC,
	"times",	F_TIMES,	S_FUNC,
	"trap",		F_TRAP,		S_FUNC,
	"true",		F_TRUE,		S_FUNC,
	"umask",	F_UMASK,	S_FUNC,
	"unset",	F_UNSET,	S_FUNC,
	"wait",		F_WAIT,		S_FUNC,
	"xh",		F_EXEC_HIST,	S_FUNC,

/*
 ******	Comandos de Programação *********************************
 */
	"break",	P_BREAK,	S_PROG,
	"case",		P_CASE,		S_PROG,
	"continue",	P_CONTINUE,	S_PROG,
	"do",		P_DO,		S_PROG,
	"done",		P_DONE,		S_PROG,
	"elif",		P_ELIF,		S_PROG,
	"else",		P_ELSE,		S_PROG,
	"esac",		P_ESAC,		S_PROG,
	"fi",		P_FI,		S_PROG,
	"for",		P_FOR,		S_PROG,
	"if",		P_IF,		S_PROG,
	"in",		P_IN,		S_PROG,
	"then",		P_THEN,		S_PROG,
	"time",		P_TIME,		S_PROG,
	"until",	P_UNTIL,	S_PROG,
	"while",	P_WHILE,	S_PROG,
	";;",		P_ENDOPT,	S_PROG,

/*
 ******	Variáveis ***********************************************
 */
	"CPU",		V_PERMANENT,	S_UNDEF,
	"HIST",		V_PERMANENT,	S_UNDEF,
	"HOME",		V_PERMANENT,	S_UNDEF,
	"PATH",		V_PERMANENT,	S_UNDEF,
	"PS1",		V_PERMANENT,	S_UNDEF,
	"PS2",		V_PERMANENT,	S_UNDEF,
	"SHELL",	V_PERMANENT,	S_UNDEF,
	"SYS",		V_PERMANENT,	S_UNDEF,
	"TERM",		V_PERMANENT,	S_UNDEF,
	"USER",		V_PERMANENT,	S_UNDEF,

	NOSTR
};

/*
 ****************************************************************
 *	As tabelas						*
 ****************************************************************
 */
entry	SYM	*hashtb[1000];
entry	SYM	symtb[sizeof (nametb) / sizeof (NAME) - 1];

/*
 ****************************************************************
 *	Tabelas de relocação para "mkasmtb"			*
 ****************************************************************
 */
entry	OBJENT	hashtb_rel[] =
{
	{ sizeof (SYM *), 0, 1, "symtb", symtb },
	{ 0 }
};

entry	OBJENT	symtb_rel[] =
{
	{ sizeof (SYM *), offsetof (SYM, s_hnext), 1, "symtb", symtb },
	{ 0 }
};

/*
 ****************************************************************
 *	Insere os símbolos, enquanto não há colisão		*
 ****************************************************************
 */
int
insert_names (int sz)
{
	SYM		**hp;
	SYM		*sp;
	NAME		*np;

	/*
	 *	Inicialmente, zera as tabelas.
	 */
	memset (hashtb, 0, sz * sizeof (SYM *));
	memset (symtb,  0, sizeof (symtb));

	/*
	 *	Insere os nomes das funções.
	 */
	for (sp = symtb, np = nametb; np->n_name != NOSTR; np++, sp++)
	{
		strscpy (sp->s_name, np->n_name, sizeof (sp->s_name));

		hp = &hashtb[strhash (sp->s_name, strlen (sp->s_name), sz)];

		if (*hp != NOSYMP)
			return (-1);	/* colisão */

		sp->s_type   = np->n_type;
		sp->s_union  = np->n_id;
	/***	sp->s_hnext  = NOSYMP;	***/
		*hp          = sp;
	}

	return (0);

}	/* end insert_names */

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		sz;

	/*
	 *	Descobre o tamanho que não provoca colisões.
	 */
	for (sz = 300; insert_names (sz) < 0; sz++)
		/* vazio */;

	printf ("*** Tamanho calculado para tabela HASH: %d ***\n", sz);
	printf ("*** Tamanho da tabela de símbolos: %d entradas, %d bytes ***\n",
		sizeof (symtb) / sizeof (SYM), sizeof (symtb));

	/*
	 *	Gera 3 arquivos: "hashtb.s", "symtb.s" e "hashsz.s".
	 */
	mkasmtb
	(
		"hashtb.s",
		"hashtb",
		hashtb,
		sz * sizeof (SYM *),
		RW,
		sizeof (SYM *),
		hashtb_rel
	);

	mkasmtb
	(
		"symtb.s",
		"symtb",
		symtb,
		sizeof (symtb),
		RW,
		sizeof (SYM),
		symtb_rel	/* desnecessário: os ponteiros são nulos */
	);

	mkasmtb
	(
		"hashsz.s",
		"hashsz",
		&sz,
		sizeof (int),
		RO,
		sizeof (int),
		NOOBJENT
	);

	return (0);

}	/* end main */
