/*
 ****************************************************************
 *								*
 *			gets.c					*
 *								*
 *	Lê (e edita) uma linha do teclado			*
 *								*
 *	Versão	3.0.0, de 05.07.94				*
 *		3.0.7, de 14.03.97				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1994 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	CTL_Q	0x11

/*
 ******	Tabela de Acentos ***************************************
 */
typedef struct
{
	ushort	a_fonte;	/* Fonte: pares */
	ushort	a_destino;	/* Destino: resultado da conversao */

}	ACENTO;

#define	OR(a, b)	((a << 8) | b)

#define IS_ACCENT(c)	(c == '\'' || c == ',' || c == '~' ||	\
				c == '^' || c == '`' || c == ':')

entry int	esc_char = 0;	/* Primeira parte de possível acento */

extern const ACENTO acento[];	/* Tabela de acentos */

/*
 ****************************************************************
 *	Lê (e edita) uma linha do teclado			*
 ****************************************************************
 */
char *
gets (char *cp)
{
	int		c;
	char		*old_cp = cp;
	ushort		or_c;
	const ACENTO	*ap;

	/*
	 *	Lê os caracteres
	 */
	for (EVER)
	{
		/*
		 *	Obtém um caractere
		 */
	    again:
		c = getchar ();

		if (c == CTL_Q)
			goto again;

		/*
		 *	Tratamento dos ACENTOS
		 */
		if (esc_char)
		{
			or_c = OR (esc_char, c);	esc_char = 0;

			if   (c == '\\')
				continue;

			/*
			 *	Procura o acento dado
			 */
			for (ap = acento; ap->a_fonte; ap++)
			{
				if (ap->a_fonte == or_c)
				{
					c = ap->a_destino; 	cp[-1] = c;
					putchar ('\b');		putchar (c);
					goto again;
				}
			}

			/*
			 *	Se não achou na tabela, continua normalmente
			 */
		}

		/*
		 *	Verifica se terminou a linha
		 */
		if (c == '\n' || c == '\r')
		{
			*cp++ = '\0'; 	putchar ('\n');
			return (old_cp);
		}

		/*
		 *	Processa erase
		 */
		if (c == '\b' || c == 0x7F /* DEL */)
		{
			if (cp <= old_cp)
				continue;

			cp--;
			putchar ('\b');	putchar (' ');	putchar ('\b');
			continue;
		}

		/*
		 *	Processa os acentos
		 */
		if (IS_ACCENT (c))
			esc_char = c;

		/*
		 *	Guarda e ecoa o caracter
		 */
		*cp++ = c;

		putchar (c);

	}	/* end for EVER */

}	/* end gets */

/*
 ****************************************************************
 *	Tabela de Acentos					*
 ****************************************************************
 */
const ACENTO	acento[] =
{
	OR ('~', 'A'),	0xC3,	/*  Ã */
	OR ('~', 'a'),	0xE3,	/*  ã */
	OR ('~', 'O'),	0xD5,	/*  Õ */
	OR ('~', 'o'),	0xF5,	/*  õ */
	OR (',', 'C'),	0xC7,	/*  Ç */
	OR (',', 'c'),	0xE7,	/*  ç */
	OR (',', 'z'),	0xDF,	/* sz */
	OR ('\'','A'),	0xC1,	/*  Á */
	OR ('\'','a'),	0xE1,	/*  á */
	OR ('\'','E'),	0xC9,	/*  É */
	OR ('\'','e'),	0xE9,	/*  é */
	OR ('\'','I'),	0xCD,	/*  Í */
	OR ('\'','i'),	0xED,	/*  í */
	OR ('\'','O'),	0xD3,	/*  Ó */
	OR ('\'','o'),	0xF3,	/*  ó */
	OR ('\'','U'),	0xDA,	/*  Ú */
	OR ('\'','u'),	0xFA,	/*  ú */
	OR ('^', 'A'),	0xC2,	/*  Â */
	OR ('^', 'a'),	0xE2,	/*  â */
	OR ('^', 'E'),	0xCA,	/*  Ê */
	OR ('^', 'e'),	0xEA,	/*  ê */
	OR ('^', 'I'),	0xCE,	/* ^I */
	OR ('^', 'i'),	0xEE,	/* ^i */
	OR ('^', 'O'),	0xD4,	/*  Ô */
	OR ('^', 'o'),	0xF4,	/*  ô */
	OR ('^', 'U'),	0xDB,	/* ^U */
	OR ('^', 'u'),	0xFB,	/* ^u */
	OR ('`', 'A'),	0xC0,	/*  À */
	OR ('`', 'a'),	0xE0,	/*  à */
	OR ('`', 'E'),	0xC8,	/* `E */
	OR ('`', 'e'),	0xE8,	/* `e */
	OR ('`', 'I'),	0xCC,	/* `I */
	OR ('`', 'i'),	0xEC,	/* `i */
	OR ('`', 'O'),	0xD2,	/* `O */
	OR ('`', 'o'),	0xF2,	/* `o */
	OR ('`', 'U'),	0xD9,	/* `U */
	OR ('`', 'u'),	0xF9,	/* `u */
	OR (':', 'A'),	0xC4,	/* :A */
	OR (':', 'a'),	0xE4,	/* :a */
	OR (':', 'E'),	0xCB,	/* :E */
	OR (':', 'e'),	0xEB,	/* :e */
	OR (':', 'I'),	0xCF,	/* :I */
	OR (':', 'i'),	0xEF,	/* :i */
	OR (':', 'O'),	0xD6,	/* :O */
	OR (':', 'o'),	0xF6,	/* :o */
	OR (':', 'U'),	0xDC,	/*  Ü */
	OR (':', 'u'),	0xFC,	/*  ü */
	0,		0
};
