/*
 ****************************************************************
 *								*
 *			getcomp.c				*
 *								*
 *	Preparação das chaves e função de comparação		*
 *								*
 *	Versão	1.0.0, de 06.10.86				*
 *		3.0.0, de 05.07.97				*
 *								*
 *	Módulo: sort						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/sort.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
#define NKEYS	10

entry KEY	key_vec[NKEYS];

entry int	(*compar) (const void *, const void *);

/*
 ****************************************************************
 *	Rotina que prepara as chaves para comparação		*
 ****************************************************************
 */
void
get_keys (const char *p)
{
	register int	i;
	register KEY	*q;

	numkeys = 0;

	q = &key_vec[0];

	/*
	 *	Percorre a cadeia dada, retirando as informações das chaves 
	 */
	while (*p != '\0')
	{
		q->k_index = strtol (p, &p, 10);

	   /***	q->k_begin = 0; ***/
	   /***	q->k_size = 0; ***/
		q->k_up_down = 'a';	/* UP */
		q->k_code = 'a';	/* LEXICOGRÁFICO */

		if (*p == ':')
			q->k_begin = strtol (++p, &p, 10);

		if (*p == '.')
			q->k_size = strtol (++p, &p, 10);

		if   (*p == 'd')
			{ q->k_up_down = 'd'; p++; }
		elif (*p == 'a')
			p++;

		if (*p != ' ' && *p != '\0')
			q->k_code = *p++;

		q++;

		if (++numkeys > NKEYS)
		{
			fprintf
			(	stderr,
				"%s: Foram dadas mais do que " XSTR (NKEYS)
				" chaves\n",
				pgname
			);
			exit (1);
		}

	}	/* end while */

	/*
	 *	Imprime a informação coletada
	 */
	if (vflag)
	{
		fprintf (stderr, "As %d chaves são:\n", numkeys);

		for (i = 0; i < numkeys; i++)
		{
			q = &key_vec[i];

			fprintf (stderr, "campo = %d, ",	q->k_index);
			fprintf (stderr, "início = %d, ",	q->k_begin);
			fprintf (stderr, "comprimento = %d, ",	q->k_size);
			fprintf (stderr, "ordem = \"%c\", ",	q->k_up_down);
			fprintf (stderr, "tipo = \"%c\"\n",	q->k_code);
		}
	}

}	/* end get_keys */

/*
 ****************************************************************
 *	Escolhe a rotina de comparação				*
 ****************************************************************
 */
void
set_cmp_function (void)
{
	int	(*char_compar) (const char *, const char *) = strcmp;

	if (numkeys != 0)
	{
		char_compar = cmp_key;
	}
	else switch (tflag)
	{
	    case 'a':
		char_compar = rflag ? strrcmp : strcmp;	
		break;

	    case 'i':
		char_compar = rflag ? strircmp : stricmp;	
		break;

	    case 'f':
		char_compar = rflag ? cmp_fold_down : cmp_fold_up;	
		break;

	    case 'd':
		char_compar = rflag ? cmp_dic_down : cmp_dic_up;	
		break;
	}

	compar = (int (*) (const void *, const void *))char_compar;

}	/* end set_cmp_function */
