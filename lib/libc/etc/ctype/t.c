/*
 ****************************************************************
 *								*
 *			t.c					*
 *								*
 *	Programa de teste do módulo ctype.			*
 *								*
 *	Versão	1.0, de 18.11.86				*
 *								*
 *	Rotinas:						*
 *		main						*
 *								*
 *	Módulo: LIBC						*
 *		Bibliotecas					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<stdio.h>
#include	"ctype.h"

#define		EVER	;;

/*
 ****************************************************************
 *	Converte um valor inteiro para uma cadeia com seu	*
 *	valor booleano por extenso.				*
 ****************************************************************
 */
char *
tobool (bool)
int	bool;
{
	if (bool)
		return ("****");
	else
		return ("");
}

/*
 ****************************************************************
 *	Programa Principal					*
 ****************************************************************
 */
main ()
{
	int		c;
	char		buf[80];

	puts ("Programa de testes do ctype\n");

	for (EVER)
	{
		printf ("Entre o codigo de um caracter: ");

		if (gets (buf) != buf)
			exit (0);
		if (buf[0] == '\'')
			c = buf[1];
		else
			c = strtol (buf, 0, 0);

		printf ("Caractere lido: \"%c\" : 0x%02X\n\n", c, c);

		printf ("isalpha?\t%s\n", tobool (isalpha (c)));
		printf ("isupper?\t%s\t\t", tobool (isupper (c)));
		printf ("islower?\t%s\n\n", tobool (islower (c)));

		printf ("isdigit?\t%s\t\t", tobool (isdigit (c)));
		printf ("isxdigit?\t%s\n", tobool (isxdigit (c)));
		printf ("isalnum?\t%s\n\n", tobool (isalnum (c)));

		printf ("isspace?\t%s\t\t", tobool (isspace (c)));
		printf ("ispunct?\t%s\n", tobool (ispunct (c)));
		printf ("isprint?\t%s\t\t", tobool (isprint (c)));
		printf ("isgraph?\t%s\n", tobool (isgraph (c)));
		printf ("iscntrl?\t%s\n\n", tobool (iscntrl (c)));

		printf ("isascii?\t%s\t\t", tobool (isascii (c)));
		printf ("toascii?\t%x\n", toascii (c));
		printf ("isiso?\t\t%s\t\t", tobool (isiso (c)));
		printf ("toiso?\t\t%x\n\n", toiso (c));

		printf ("_tolower?\t\"%c\", 0x%02X\t",
					_tolower (c), _tolower (c));
		printf ("tolower?\t\"%c\", 0x%02X\n",
					tolower (c), tolower (c));
		printf ("_toupper?\t\"%c\", 0x%02X\t",
					_toupper (c), _toupper (c));
		printf ("toupper?\t\"%c\", 0x%02X\n\n",
					toupper (c), toupper (c));

	}	/* end for (EVER) */

}	/* end main () */
