/*
 ****************************************************************
 *								*
 *			t1.c					*
 *								*
 *	Programa de teste do módulo "ctype"			*
 *								*
 *	Versão	2.3.0, de 08.02.91				*
 *								*
 *	Rotinas:						*
 *		main						*
 *								*
 *	Módulo: ctype						*
 *		libc/etc					*
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

void		print_table (void);

/*
 ****************************************************************
 *  Imprime uma tabela de caracteres aceitos por um atributo	*
 ****************************************************************
 */
main (int argc, register char *argv[])
{
	register int	i, c, p;

	/*
	 *	Escreve o nome do atributo
	 */
	switch (p = atol (argv[1]))
	{
	    case 1:
		printf ("isalpha:\n\n");
		break;

	    case 2:
		printf ("isupper:\n\n");
		break;

	    case 3:
		printf ("islower:\n\n");
		break;

	    case 4:
		printf ("isdigit:\n\n");
		break;

	    case 5:
		printf ("isxdigit:\n\n");
		break;

	    case 6:
		printf ("isalnum:\n\n");
		break;

	    case 7:
		printf ("isspace:\n\n");
		break;

	    case 8:
		printf ("ispunct:\n\n");
		break;

	    case 9:
		printf ("isprint:\n\n");
		break;

	    case 10:
		printf ("isgraph:\n\n");
		break;

	    case 11:
		printf ("iscntrl:\n\n");
		break;

	    default:
		printf ("Atributo inválido: \"%s\"\n", argv[1]);
		exit (1);

	}	/* end switch */

	/*
	 *	Malha pelos caracteres
	 */
	for (i = 0, c = 0; c < 256; c++)
	{
		switch (p)
		{
		    case 1:
			if (!isalpha (c))
				continue;
			break;

		    case 2:
			if (!isupper (c))
				continue;
			break;

		    case 3:
			if (!islower (c))
				continue;
			break;

		    case 4:
			if (!isdigit (c))
				continue;
			break;

		    case 5:
			if (!isxdigit (c))
				continue;
			break;

		    case 6:
			if (!isalnum (c))
				continue;
			break;

		    case 7:
			if (!isspace (c))
				continue;
			break;

		    case 8:
			if (!ispunct (c))
				continue;
			break;

		    case 9:
			if (!isprint (c))
				continue;
			break;

		    case 10:
			if (!isgraph (c))
				continue;
			break;

		    case 11:
			if (!iscntrl (c))
				continue;
			break;

		    default:
			printf ("Atributo inválido: \"%s\"\n", argv[1]);
			exit (1);

		}	/* end switch */

		if (i >= 5)
			{ printf ("\n"); i = 0; }
	
		if (isprint (c))
			printf ("%02X \"%c\"        ", c, c);
		else
			printf ("%02X ...        ", c, c);

		i++;
	}

	if (i)
		printf ("\n");

	exit (0);

}	/* end print_table */
