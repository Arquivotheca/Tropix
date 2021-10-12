/*
 ****************************************************************
 *								*
 *			trtblc.c				*
 *								*
 *	Trata blocos.						*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtblc		criablc				*
 *		imprfixo	fimflut				*
 *		imprflut					*
 *								*
 *	Modulo: SROFF						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

#include "../h/sroff.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Trata os comandos relativos a blocos.			*
 ****************************************************************
 */
void
trtblc (char *arg)
{
	register char	*cp;

	cp = arg;
	if (cp == NOSTR)
	{
		/*
		 *	Bloco e' fixo.
		 */
		blfixo = blccorr = criablc ();
		return;
	}

	if (*cp++ != 'f')
	{
		errcmd ();
		return;
	}

	while (ESPACO (*cp))
		cp++;

	if (*cp != NOCHAR)
	{
		errcmd ();
		return;
	}

	/*
	 *	Bloco e' flutuante.
	 */
	if (linflut == 0)
	{
		/*
		 *	Imprime os blocos flutuantes do inicio da pagina.
		 */
		while (blflut != (BLOCO *)NULL)
		{
			if (linhaescrita == 0)
				inicpag ();
			else
			{
				trtpag (-1);
				inicpag ();
			}

			imprflut ();

			if (linhaescrita == tampag)
				fimpag ();
		}
	}

	if (blflut == (BLOCO *)NULL)
	{
		/*
		 *	O bloco e' o primeiro da fila.
		 */
		blflut = blinic = blccorr = criablc ();
		temflut = TRUE;
		return;
	}

	blccorr = blflut;
	while (blccorr->b_blcprox != (BLOCO *)NULL)
		blccorr = blccorr->b_blcprox;

	/*
	 *	Aloca o bloco no fim da fila.
	 */
	if ((blccorr->b_blcprox = criablc ()) != (BLOCO *)NULL)
	{
		temflut = TRUE;
		blinic = blccorr = blccorr->b_blcprox;
	}

}	/* end trtblc */

/*
 ****************************************************************
 *	Organiza a abertura de um novo bloco.			*
 ****************************************************************
 */
BLOCO *
criablc (void)
{
	register BLOCO	*blc;

	/*
	 *	Cria bloco.
	 */
	if ((blc = (BLOCO *)malloc (sizeof (BLOCO))) == (BLOCO *)NULL)
	{
		fprintf (stderr, no_mem_text, pgname);
		return ((BLOCO *)NULL);
	}
	blc->b_nlinhas = 0;
	blc->b_blcprox = (BLOCO *)NULL;

	/*
	 *	Cria linha.
	 */
	if ((blc->b_plinha = (LINHA *)malloc (sizeof (LINHA))) == (LINHA *)NULL)
	{
		fprintf (stderr, no_mem_text, pgname);
		free (blc);
		return ((BLOCO *)NULL);
	}

	lincorr = blc->b_plinha;
	lincorr->l_lin = NOSTR;
	lincorr->l_linprox = (LINHA *)NULL;
	tembloco = TRUE;
	return (blc);

}	/* end criablc */

/*
 ****************************************************************
 *	Imprime os blocos fixos.				*
 ****************************************************************
 */
void
imprfixo (void)
{
	BLOCO	*blp,
		*blpaux;
	LINHA	*lp,
		*lpaux;

	tembloco = FALSE;

	if (linflut != 0)
		attampag ();

	blp = blfixo;

	if (blp->b_nlinhas > (tampag - linhaescrita))
	{
		if (linflut != 0)
			linflut = 0;

		trtpag (-1);
	}

	while (blp != (BLOCO *)NULL)
	{
		if (linhaescrita == 0)
			inicpag ();

		blpaux = blp->b_blcprox;

		lp = blp->b_plinha;
		while (lp->l_linprox != (LINHA *)NULL)
		{
			lpaux = lp->l_linprox;
			fprintf (stdout, "%s", lp->l_lin);
			free (lp->l_lin);
			free (lp);
			lp = lpaux;
		}
		free (lp);

		if ((linhaescrita += blp->b_nlinhas) == tampag)
		{
			if (linflut != 0)
				linflut = 0;

			fimpag ();
		}

		free (blp);

		blp = blpaux;
	}

	blfixo = (BLOCO *)NULL;

	if (linflut != 0)
		atlinflut ();
	else
	{
		/*
		 *	Imprime os blocos flutuantes de inicio de pagina.
		 */
		if (blflut != (BLOCO *)NULL)
		{
			if (linhaescrita == 0)
				inicpag ();

			imprflut ();

			if (linhaescrita == tampag)
				fimpag ();
		}
	}

}	/* end imprfixo */

/*
 ****************************************************************
 *	Trata o bloco flutuante lido.				*
 ****************************************************************
 */
void
fimflut (void)
{
	tembloco = FALSE;
	temflut = FALSE;

	if (blinic->b_nlinhas < (tampag - linhaescrita))
	{
		/*
		 *	O bloco e' menor que o fim da pagina corrente.
		 */
		tampag -= blinic->b_nlinhas;
		linflut += blinic->b_nlinhas;
	}
	else
		if (blinic->b_nlinhas == (tampag - linhaescrita))
		{
			/*
			 *	O bloco cabe exatamente
			 *	no fim da pagina corrente.
			 */
			linflut += blinic->b_nlinhas;
			if (linhaescrita == 0)
				inicpag ();
			fimpag ();
		}

}	/* end fimflut */

/*
 ****************************************************************
 *	Imprime os blocos flutuantes que cabem na pagina.	*
 ****************************************************************
 */
void
imprflut (void)
{
	BLOCO	*blp,
		*blpant,
		*blpaux;
	LINHA	*lp,
		*lpaux;


	blp = blflut;
	blflut = (BLOCO *)NULL;
	blpant = (BLOCO *)NULL;

	while (linhaescrita < tampag  &&  blp != (BLOCO *)NULL)
	{
		blpaux = blp->b_blcprox;

		if (blp->b_nlinhas <= (tampag - linhaescrita))
		{
			if (blpant != (BLOCO *)NULL)
				blpant->b_blcprox = blpaux;

			lp = blp->b_plinha;
			while (lp->l_linprox != (LINHA *)NULL)
			{
				lpaux = lp->l_linprox;
				fprintf (stdout, "%s", lp->l_lin);
				free (lp->l_lin);
				free (lp);
				lp = lpaux;
			}
			free (lp);

			linhaescrita += blp->b_nlinhas;

			free (blp);

			blp = blpaux;
		}
		else
		{
			if (blflut == (BLOCO *)NULL)
				blflut = blp;
			blpant = blp;
			blp = blp->b_blcprox;
		}
	}

	if (blflut == (BLOCO *)NULL)
		blflut = blp;

}	/* end imprflut */
