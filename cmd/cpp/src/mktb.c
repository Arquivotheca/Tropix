/*
 ****************************************************************
 *								*
 *			mktb.c					*
 *								*
 *	Gera as tabelas do preprocessador			*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 23.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <mkasmtb.h>

#include "../h/scan.h"
#include "../h/error.h"
#include "../h/symtb.h"

/*
 ****************************************************************
 *	Função de HASH						*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL

entry int	ncolision;

entry TOKEN	token;

#define	MK

#include "../h/hash.h"

/*
 ****** Tabela de diretivas *************************************
 */
typedef struct
{
	char	t_name[12];
	DIRECT	t_dir;

}	TBL0;

const TBL0	tbl0[] =
{
	"if",		IF,		/* Diretiva if */
	"elif",		ELIF,		/* Diretiva elif */
	"endif",	ENDIF,		/* Diretiva endif */
	"ifdef",	IFDEF,		/* Diretiva ifdef */
	"ifndef",	IFNDEF,		/* Diretiva ifndef */
	"else",		ELSE,		/* Diretiva else */
	"include",	INCLUDE,	/* Diretiva include */
	"define",	DEFINE,		/* Diretiva define */
	"undef",	UNDEF,		/* Diretiva undef */
	"line",		LINE,		/* Diretiva line */
	"error",	ERROR,		/* Diretiva error */
	"pragma",	PRAGMA,		/* Diretiva pragma */
	""
};

#define	TBL0SZ	((sizeof (tbl0) / sizeof (TBL0)) - 1)

/*
 ****** Tabela de macros intrínsecas ****************************
 */
typedef	struct
{
	char	t_name[12];
	BUILTIN	t_builtin;

}	TBL1;

const TBL1 tbl1[] =
{
	"__LINE__",	_LINE,
	"__FILE__",	_FILE,
	"__DATE__",	_DATE,
	"__TIME__",	_TIME,
	"defined",	_DEFINED,
	"__STDC__",	_STDC,
	""
};

#define	TBL1SZ	((sizeof (tbl1) / sizeof (TBL1)) - 1)

/*
 ****** Tabela de símbolos e hash a construir *******************
 */
entry SYMTB	symtb[TBL0SZ + TBL1SZ];

entry SYMTB	*hashtb[HASHSZ];

entry int	macdef[256];

const OBJENT	objhash[]	=
{
	sizeof (SYMTB *), 0,				1, "symtb", symtb,
	0
};

entry OBJENT	objsymtb[]	=
{
	sizeof (char *),  offsetof (SYMTB, s_name),	1, "nametb", NOSTR /* abaixo */,
	sizeof (int),	  offsetof (SYMTB, s_flags),	1, NOSTR,    NOVOID,
	sizeof (int),	  offsetof (SYMTB, s_index),	1, NOSTR,    NOVOID,
	sizeof (int),	  offsetof (SYMTB, s_nargs),	1, NOSTR,    NOVOID,
	sizeof (int),	  offsetof (SYMTB, s_expand),	1, NOSTR,    NOVOID,
	sizeof (char *),  offsetof (SYMTB, s_macro),	1, NOSTR,    NOVOID,
	sizeof (SYMTB *), offsetof (SYMTB, s_link),	1, "symtb",  symtb,
	0
};

/*
 ****************************************************************
 *	Gera as tabelas do preprocessador			*
 ****************************************************************
 */
void
main (void)
{
	const TBL0	*tp0;
	const TBL1	*tp1;
	SYMTB		*sp, *lastsp, **link_place;
	int		len, nametb_sz = 0;
	char		*nametb, *nametb_ptr;

	/*
	 *	Obtém o tamanho necessário para a tabela de nomes
	 */
	for (tp0 = tbl0; tp0->t_name[0]; tp0++)
		nametb_sz += strlen (tp0->t_name) + 1;

	for (tp1 = tbl1; tp1->t_name[0]; tp1++)
		nametb_sz += strlen (tp1->t_name) + 1;

	nametb_ptr = nametb = alloca (nametb_sz);

	objsymtb[0].o_rel_addr = nametb;

	/*
	 *	Prepara as palavras chaves
	 */
	for (sp = symtb, tp0 = tbl0; tp0->t_name[0]; tp0++, sp++)
	{
		len = strlen (tp0->t_name);

		sp->s_name   = nametb_ptr;
		sp->s_flags  = S_KEYWORD;
		sp->s_index  = tp0->t_dir;
	   /***	sp->s_nargs  = 0; ***/
	   /***	sp->s_expand = 0; ***/
	   /***	sp->s_macro  = NOSTR; ***/
	   /***	sp->s_link = NOSYMTB; ***/

		strcpy (nametb_ptr, tp0->t_name);
		nametb_ptr += len + 1;

		if (hash (sp->s_name, &link_place) != NOSYMTB)
			printf ("Símbolo duplicado: \"%s\"\n", sp->s_name);

		*link_place = sp;
	   /***	sp->s_link = NOSYMTB; ***/
	}

	/*
	 *	Prepara as macros intrínsecas
	 */
	for (tp1 = tbl1; tp1->t_name[0]; tp1++, sp++)
	{
		len = strlen (tp1->t_name);

		sp->s_name   = nametb_ptr;
		sp->s_flags = S_MACRO|S_BUILTIN;
		sp->s_index = tp1->t_builtin;
	   /***	sp->s_nargs  = 0; ***/
	   /***	sp->s_expand = 0; ***/
	   /***	sp->s_macro  = NOSTR; ***/
	   /***	sp->s_link = NOSYMTB; ***/

		strcpy (nametb_ptr, tp1->t_name);
		nametb_ptr += len + 1;

		if (hash (sp->s_name, &link_place) != NOSYMTB)
			printf ("Símbolo duplicado: \"%s\"\n", sp->s_name);

		*link_place = sp;
	   /***	sp->s_link = NOSYMTB; ***/

		macdef[tp1->t_name[0]]++;
	}

	lastsp = sp;

	printf
	(	"symtbsz = (%d == %d), colisões = %d\n",
		TBL0SZ + TBL1SZ, sp - symtb, ncolision
	);

	/*
	 *	Gera os módulos
	 */
	mkasmtb
	(
		"nametb.s",
		"nametb",
		nametb,
		nametb_sz,
		RO,
		1,
		(OBJENT *)NULL
	);

	mkasmtb
	(
		"symtb.s",
		"symtb",
		symtb,
		(lastsp - symtb) * sizeof (SYMTB),
		RW,
		sizeof (SYMTB),
		objsymtb
	);

	mkasmtb
	(
		"hashtb.s",
		"hashtb",
		hashtb,
		(long)sizeof (hashtb),
		RW,
		sizeof (SYMTB *),
		objhash
	);

	mkasmtb
	(
		"macdef.s",
		"macdef",
		macdef,
		sizeof (macdef),
		RW,
		8 * sizeof (int),
		(OBJENT *)NULL
	);

}	/* end main */
