/*
 ****************************************************************
 *								*
 *			trtrod.c				*
 *								*
 *	Trata rodapes.						*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtrod	fimrod					*
 *		imprrod						*
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
 *	Trata o comando de inicio de rodape (".(f").		*
 ****************************************************************
 */
void
trtrod (void)
{
	if (linflut != 0)
	{
		linflut = 0;
		attampag ();
	}

	if (blrod == (BLOCO *)NULL)
	{
		/*
		 *	Primeiro rodape.
		 */
		if (linhaescrita < (tampag - lsblc + 1))
		{
			/*
			 *	Pelo menos uma linha cabe na pagina.
			 */
			lsrod = lsblc;
			linrod += (lsblc + 1);
			attampag ();
			rodinic = FALSE;
		}
		else
		{
			/*
			 *	O rodape nao cabe na pagina.
			 */
			trtpag (-1);
			rodinic = TRUE;
		}

		if ((blrod = blccorr = criablc ()) == (BLOCO *)NULL)
			return;

		temrod = TRUE;

		return;
	}

	if (linhaescrita < tampag)
		/*
		 *	Pelo menos uma linha cabe na pagina.
		 */
		rodinic = FALSE;
	else
	{
		/*
		 *	O rodape nao cabe na pagina.
		 */
		trtpag (-1);
		rodinic = TRUE;
	}

	blccorr = blrod;
	while (blccorr->b_blcprox != (BLOCO *)NULL)
		blccorr = blccorr->b_blcprox;

	/*
	 *	Cria rodape no fim da fila.
	 */
	if ((blccorr->b_blcprox = criablc ()) != (BLOCO *)NULL)
	{
		blccorr = blccorr->b_blcprox;
		temrod = TRUE;
	}

}	/* end trtrod */

/*
 ****************************************************************
 *	Trata o ultimo rodape lido.				*
 ****************************************************************
 */
void
fimrod (void)
{
	temrod = FALSE;
	tembloco = FALSE;

	if (rodinic)
	{
		/*
		 *	Tem rodape no inicio da pagina.
		 */
		linrod = 0;
		attampag ();
		blfixo = blrod;
		imprfixo ();
		rodprox = blrod = (BLOCO *)NULL;
	}
	else
	{
		if (rodprox == (BLOCO *)NULL)
		{
			if (linrod > 0)
			{
				/*
				 *	O rodape cabe na pagina.
				 */
				attampag ();
				atlinflut ();
			}
			else
			{
				/*
				 *	O rodape termina a pagina.
				 */
				if (linhaescrita == 0)
					inicpag ();
				fimpag ();
			}
		}
		else
		{
			/*
			 *	Tem rodape na pagina seguinte.
			 */
			if (linhaescrita == 0)
				inicpag ();
			fimpag ();
			blfixo = blrod;
			imprfixo ();
			rodprox = blrod = (BLOCO *)NULL;
		}
	}

}	/* end fimrod */

/*
 ****************************************************************
 *	Imprime os rodapes que couberem no fim da pagina.	*
 ****************************************************************
 */
void
imprrod (void)
{
	BLOCO	*blp,
		*blpaux;
	LINHA	*lp,
		*lpaux;


	blp = blrod;

	while (blp != rodprox)
	{
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

		free (blp);

		blp = blpaux;
	}
	blrod = blp;

}	/* end imprrod */
