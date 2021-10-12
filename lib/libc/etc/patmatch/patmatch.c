/*
 ****************************************************************
 *								*
 *			patmatch.c				*
 *								*
 *	Comparador de padrões					*
 *								*
 *	Versão	1.1.0, de 14.08.86				*
 *		4.5.0, de 10.10.03				*
 *								*
 *	Módulo: patmatch					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>

/*
 ****************************************************************
 *	Comparador de padrões					*
 ****************************************************************
 */
int
patmatch (register const char *sp /* Cadeia */, register const char *pp /* Padrão */)
{
	register char	sc,	/* Caracter da Cadeia */
			pc;	/* Caracter do Padrão */
	int		found,	/* Encontrou o Char. na lista */
			not = 0; /* Foi visto um '!' */
#undef	DEBUG
#ifdef	DEBUG
	printf ("DBG: str = \"%s\", pdr = \"%s\"\n", sp, pp);
#endif	DEBUG

	/*
	 *	O Padrão pode conter '?', '*', '[....]', '!c', '![....]'.
	 *	O Caracter '\' desfaz a ação especial dos caracteres
	 *	acima. A Comparação é do estilo do "sh", mas
	 *	o caracter '/' é reconhecido por '?' e '*'
	 *
	 *	Devolve:
	 *	    1 => A Cadeia foi Reconhecida.
	 *	    0 => Não foi Reconhecida, ou
	 *		 o Padrão contem um erro de Sintaxe
	 */

	/*
	 *	Malha Principal de Comparação
	 */
	for (/* sem inicialização */; /* sem teste */; sp++, pp++)
	{
		switch (pc = pp[0])
		{
			/*
			 *	O Padrão contem um caracter Normal,
			 *	ou outras palavras, 
			 *	diferente de '?', '*', '[', '!' e '\0'
			 */
		    default:
			if (pc == sp[0])
				continue;
			else
				return (0);

			/*
			 *	O Padrão Acabou
			 */
		    case '\0':
			if (sp[0] == '\0')
				return (1);
			else
				return (0);

			/*
			 *	Simula como se o "coringa" '?' fosse
			 *	igual ao caracter da cadeia
			 */
		    case '?':
			if (sp[0] == '\0')
				return (0);
			continue;

			/*
			 *	Procura Sufixos em comum
			 */
		    case '*':
			if ((++pp)[0] == '\0')
				return (1);

			while (sp[0] != '\0')
			{
				if (patmatch (sp++, pp))
					return (1);
			}

			return (0);

			/*
			 *	Trata o caracter seguinte como normal
			 */
		    case '\\':
			pp++;

			if ((pc = pp[0]) == '\0')
				return (0);

			if ((sc = sp[0]) == '\0' || sc != pc)
				return (0);

			continue;

		    case '!':
			pp++;
			if ((pc = pp[0]) == '\0')
				return (0);

			if (pc != '[')
			{
				/*
				 *	Comparação inversa
				 */
				if ((sc = sp[0]) == '\0' || sc == pc)
					return (0);

				continue;
			}

			/*
			 *	pp[0] == '['. Prepara para [....] inverso
			 */
			not = 1;

			/* Cai atraves */

		    case '[':
			found = 0;

			if ((sc = sp[0]) == '\0')
				return (0);

			/*
			 *	Analisa a Sequencia '[....]'
			 */
			while ((pc = (++pp)[0]) != '\0')
			{
				switch (pc)
				{
				    default:
					if (pc == sc)
						found = 1;

					continue;

				    case '-':
					if (pp[-1] == '[' || pp[1] == ']')
					{
						if (sc == '-')
							found = 1;
					}
					else
					{
						if (pp[-1] <= sc && sc <= pp[1])
							found = 1;
					}

					continue;

				    case ']':
					if (found != not)
						goto nextchar;
					else
						return (0);

				}	/* end switch [......] */

			}	/* end while [......] */

		}	/* end switch */

		/*
		 *	A seqüência [....] terminou inesperadamente,
		 *	ou em todos os casos
		 */
		return (0);

	    nextchar:
		not = 0;

	}	/* end malha principal de comparação */

}	/* end patmatch */
