/*
 ****************************************************************
 *								*
 *			getopt.c				*
 *								*
 *	Fornece as opções de um comando				*
 *								*
 *	Versão	1.0.0, de 17.02.86				*
 *		3.0.0, de 22.06.95				*
 *								*
 *	Módulo: getopt						*
 *		lib/etc 					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry const char	*optarg;   	/* Argumento de uma opção */
entry int		optind = 1;	/* Indice do argumento apos as opções */
entry int		opterr = 1;	/* Escreve mensagem de erro */

/*
 ****************************************************************
 *	Variáveis locais estaticas				*
 ****************************************************************
 */
static const char *optp;	/* Ponteiro para a Opção corrente */
static const char **argvv;	/* Ponteiro para o Argumento corrente */

/*
 ****************************************************************
 *	Fornece as opções de um comando				*
 ****************************************************************
 */
int
getopt (int argc, const char *argv[], const char *optstring)
{
	register int		c;
	register const char	*cp;

	/*
	 *	Primeira chamada
	 */
	if (argvv == (const char **)NULL)
		{ argvv = argv; optp = "" - 1; }

	optarg = (const char *)NULL; optp++;

	/*
	 *	Verifica se este argumento tem mais opções
	 */
	if (optp[0] == '\0')
	{
		argvv++; optp = argvv[0];

		if (optp == (const char *)NULL || optp[0] != '-')
		{
			optind = argvv - argv;
			argvv = (const char **)NULL;
			return (EOF);
		}

		if (optp[1] == '\0')
		{
			optind = argvv - argv;
			argvv = (const char **)NULL;
			return (EOF);
		}

		if (optp[1] == '-')
		{
			optind = argvv - argv + 1;
			argvv = (const char **)NULL;
			return (EOF);
		}
		optp++;

	}

	c = optp[0];

	/*
	 *	Procura a opção na lista
	 */
	for (cp = optstring; cp[0] != '\0'; cp++)
	{
		if (cp[0] == c)
			goto found;
	}

	if (opterr)
		fprintf (stderr, "%s: Opção inválida: '%c'\n", argv[0], c);

	optarg = optp; return ('?');

	/*
	 *	A opção foi encontrada na lista
	 */
    found:
	if (cp[1] != ':')
		return (c);

	/*
	 *	A Opção tem Argumento
	 */
	argvv++;

	if (optp[1] == '\0' && optp[-1] == '-')
	{
		/*
		 *	Retorna o argumento
		 */
		if ((optarg = argvv[0]) == (const char *)NULL)
		{
			if (opterr)
			{
				fprintf
				(	stderr,
					"%s: Faltando o argumento da opção '%c'\n",
					argv[0],
					c
				);
			}

			optarg = optp; return ('?');
		}

		return (c);
	}
	else
	{
		/*
		 *	Foram encontrados Caracteres Estranhos
		 *	antes ou depois desta opção
		 */
		if (opterr)
		{
			fprintf
			(	stderr,
				"%s: Caracteres estranhos antes ou "
				"após a opção '%c'\n",
				argv[0],
				c
			);
		}

		optarg = optp; return ('?');
	}

}	/* end getopt */
