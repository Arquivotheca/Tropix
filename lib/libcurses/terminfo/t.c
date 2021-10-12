/*
 ****************************************************************
 *								*
 *		t.c						*
 *								*
 *	Teste para Compilação e Des-Compilação			*
 *								*
 *	Versão	1.0 0, de 27.04.87				*
 *		2.1.0, de 03.10.88				*
 *								*
 *	Modulo: teste						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>
#include	<termio.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<curses/defs.h>

/*
 ****************************************************************
 *	Funções e Variáveis Externas				*
 ****************************************************************
 */
extern	char	*parmcomp ();
extern	char	*parmuncomp ();
extern	char	*parmexec ();
extern	char	*_parmerror;

/*
 ****************************************************************
 *	Funções e Variáveis Locais				*
 ****************************************************************
 */
static void	put_string ();

/*
 ****************************************************************
 *	Definicao de Constantes					*
 ****************************************************************
 */
#define	ESC	0x1B	/* Caractere ISO Escape */
#define	NL	0x0A	/* Caractere ISO New Line */
#define	CR	0x0D	/* Caractere ISO Carriage Return */
#define	HT	0x09	/* Caractere ISO Horizontal Tabulation */
#define	BS	0x08	/* Caractere ISO Back Space */
#define	NP	0x0C	/* Caractere ISO New Page */

/*
 ****************************************************************
 *	x
 ****************************************************************
 */
main ()
{
	char	line[80];
	char	code[80];
	char	source[80];
	char	exec[80];
	char	*p;

	printf ("Entre cadeia: ");
	while (gets (line) != NULL)
	{
		if ((p = parmcomp (line)) == NULL)
		{
			printf ("Não consegui compilar \"%s\"\n", line);
			if (_parmerror)
				printf ("--> %s\n", _parmerror);
			goto next;
		}
		strcpy (code, p);
		printf ("Compilei: \"%s\"\n", line);

		if ((p = parmuncomp (code)) == NULL)
		{
			printf ("Não consegui descompilar\n");
			goto next;
		}
		strcpy (source, p);
		printf ("Descompilado: \"");
		put_string (source);
		printf ("\"\n");

		if ((p = parmexec (code, '1', '2')) == NULL)
		{
			printf ("Não consegui executar\n");
			goto next;
		}
		strcpy (exec, p);
		printf ("Execução: \"%s\"\n\n", exec);

next:
		printf ("Entre outra cadeia: ");
	}

	printf ("FIM\n");
	exit (0);

}	/* end main */

/*
 ****************************************************************
 *	Escreve uma capacidade String por Extenso		*
 ****************************************************************
 */
static void	
put_string (string)
char	*string;
{
	register char	*s;
		FILE	*output = stdout;

	for (s = string ; *s ; s++)
	{
		/*
		 *	Caracteres Comuns (alguns tem de ser "escapados")
		 */
		if (isgraph (*s) || *s == ' ')
		{
			switch (*s)
			{
			    case '\\':
			    case '^':
			    case ',':
				putc ('\\', output);
				putc (*s, output);
				break;

			    default:
				putc (*s, output);
			}
		}
		/*
		 *	Caracteres de Controle ("escapados" ou na forma "^X")
		 */
		else
		{
			switch (*s)
			{
			    case ESC:
				putc ('\\', output);
				putc ('E', output);
				break;

			    case NL:
				putc ('\\', output);
				putc ('n', output);
				break;

			    case CR:
				putc ('\\', output);
				putc ('r', output);
				break;

			    case HT:
				putc ('\\', output);
				putc ('t', output);
				break;

			    case BS:
				putc ('\\', output);
				putc ('b', output);
				break;

			    case NP:
				putc ('\\', output);
				putc ('f', output);
				break;

			    case 040:
				putc ('\\', output);
				putc ('0', output);
				break;

			    default:
				if (isascii (*s))
				{
					putc ('^', output);
					putc (*s | 0x40, output);
				}
				else
					fprintf (output, "\\%o", *s);
			}
		}

	}	/* end for string */

}	/* end put_string */
