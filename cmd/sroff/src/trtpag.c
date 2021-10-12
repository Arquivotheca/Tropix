/*
 ****************************************************************
 *								*
 *			trtpag.c				*
 *								*
 *	Arruma a pagina.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtpag		inicpag				*
 *		fimpag		poetit				*
 *		poelin		poenl				*
 *		crialbuf					*
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
#include <string.h>

#include "../h/sroff.h"
#include "../h/proto.h"

#define		LLAY		"\e&l%dd2e%dF"

/*
 ****************************************************************
 *	Variáveis estáticas.					*
 ****************************************************************
 */
static	BOOL	inictxt = TRUE;

/*
 ****************************************************************
 *	Arruma a pagina.					*
 ****************************************************************
 */
void
trtpag (int opt)
{
	register BLOCO	*blp;
	register long	tam,
			aux = 0;

	if (tembloco)
	{
		if (fimbloco)
		{
			/*
			 *	Cria novo bloco,
			 *	o anterior e' do tamanho da pagina.
			 */
			if (temflut)
			{
				/*
				 *	Transforma o flutuante corrente em fixo.
				 */
				temflut = FALSE;
				blfixo = blinic;
				if (blflut != blinic)
				{
					blp = blflut;
					while (blp->b_blcprox != blinic)
						blp = blp->b_blcprox;
					blp->b_blcprox = (BLOCO *)NULL;
				}
				else
					blflut = (BLOCO *)NULL;
			}
			if ((blccorr->b_blcprox = criablc ()) == (BLOCO *)NULL)
				tembloco = FALSE;
			else
			{
				blccorr = blccorr->b_blcprox;
				if (temrod  &&  rodprox == (BLOCO *)NULL)
					rodprox = blccorr;
			}

			fimbloco = FALSE;
		}

		/*
		 *	Calcula o tamanho da  area necessaria.
		 */
		if (opt != 0)
		{
			tam = opt + 1;
		}
		else
		{
			tam = strlen (out1) + nbrancos + ls + 1;	
			if (laser)
			{
				if (out2 != NOSTR  ||  out3 != NOSTR)
				{
					if (contpalvr != 0)
						tam += (contpalvr * 20);
					else
						tam += ((ll * 10) + 10);
				}
			}
			else
			{
				tam += (out2 == NOSTR ? 0 :
					(strlen (out2) + 1 + nbrancos));	
				tam += (out3 == NOSTR ? 0 : ((aux =
					strlen (out3)) + 1 + nbrancos));	
				if (out2 != NOSTR  &&  out3 != NOSTR)
					tam += (aux + 1 + nbrancos);
			}
		}

		/*
		 *	Aloca area.
		 */
		if ((lincorr->l_lin = malloc (tam)) == NOSTR)
		{
			fprintf (stderr, no_mem_text, pgname);
			return;
		}
		cpblc = lincorr->l_lin;

		/*
		 *	Aloca area para a proxima linha.
		 */
		if ((lincorr->l_linprox =
			(LINHA *)malloc (sizeof (LINHA))) == (LINHA *)NULL)
		{
			fprintf (stderr, no_mem_text, pgname);
			return;
		}

		lincorr = lincorr->l_linprox;
		lincorr->l_linprox = (LINHA *)NULL;
		lincorr->l_lin = NOSTR;
	}
	else
	{	
		if (linhaescrita == 0)
		{
			/*
			 *	Inicio de pagina.
			 */
			if (opt == 1  &&  blflut == blrod) 
				/*
				 *	Não inicia pag. com uma linha em branco.
				 */
				return;
			else
			{
				inicpag ();
				if (linflut == 0)
				{
					while (blflut != (BLOCO *)NULL)
					{
						imprflut ();

						if (linhaescrita != tampag)
							break;

						fimpag ();
						inicpag ();
					}
				}
			}
		}
	}

	if (opt == 0)
	{
		/*
		 *	Copia para o texto formatado a linha de bfout1.
		 */
		poelin ();
		poenl (1);
	}
	else
	{
		/*
		 *	Pula linhas no texto formatado.
		 */
		poenl ((opt > 0) ? opt : tampag);
	}

}	/* end trtpag */
 
/*
 ****************************************************************
 *	Inicia uma pagina.					*
 ****************************************************************
 */
void
inicpag (void)
{
	register int	i;

	if (inictxt)
	{
		/*
		 *	Início do texto.
		 *	Manda os parâmetros para a laser.
		 */ 
		inictxt = FALSE;
		if (laser)
			fprintf (stdout, LLAY, ln, pl);
	}

	/*
	 *	Coloca a margem 1.
	 */
	for (i = 0; i < m1; i++)
		putchar ('\n');

	if (he  &&  !tp)
	{
		/*
		 *	Coloca o 'page offset' antes do header.
		 */
		for (i = 0; i < po; i++)
			putchar (' ');

		/*
		 *	Coloca o 'header'.
		 */
		poetit ((pagcorr % 2 == 0) ? eh : oh);

		/*
		 *	Coloca a margem 2.
		 */
		for (i = 0; i < m2; i++)
			putchar ('\n');
	}

}	/* end inicpag */

/*
 ****************************************************************
 *	Termina uma pagina.					*
 ****************************************************************
 */
void
fimpag (void)
{
	register int	i;

	if (linflut != 0)
	{
		/*
		 *	Tem bloco flutuante no fim da pagina.
		 */
		attampag ();
		imprflut ();
		linflut = 0;
	}

	if (blrod != (BLOCO *)NULL)
	{
		/*
		 *	Tem rodape no fim da pagina.
		 */
		if (linrod != 0)
		{
			linrod = 0;
			attampag ();
		}

		/*
		 *	Imprime linha separada.
		 */
		for (i = lsrod; i > 0; i--)
			putchar ('\n');
		for (i = po; i > 0; i--)
			putchar (' ');
		for (i = 30; i > 0; i--)
			putchar ('-');
		putchar ('\n');

		imprrod ();
	}
	
	if (fo  &&  !tp)
	{
		/*
		 *	Coloca a margem 3.
		 */
		for (i = 0; i < m3; i++)
			putchar ('\n');

		/*
		 *	Coloca o 'page offset' antes do footer.
		 */
		for (i = 0; i < po; i++)
			putchar (' ');

		/*
		 *	Coloca o 'footer'.
		 */
		poetit ((pagcorr % 2 == 0) ? ef : of);
	}

	/*
	 *	Coloca a margem 4.
	 */
	for (i = 0; i < m4; i++)
		putchar ('\n');

	pagcorr++;
	linhaescrita = 0;

	if (tp)
	{
		/*
		 *	Fim de 'title page'.
		 */
		tp = FALSE;
		attampag ();
	}

}	/* end fimpag */

/*
 ****************************************************************
 *	Coloca o header/footer na pagina.			*
 ****************************************************************
 */
void
poetit (TITULO *tit)
{
	register int	cont,
			nb,
			nb1,
			nb2;
	char		*cp;
	int		tam[3];
	char		*pont[3];
	char		tittrb[BLSZ];
	char		titout[BLSZ];


	/*
	 *	Identifica titulo em branco.
	 */
	if (!tit->t_temtit)
	{
		putchar ('\n');
		return;
	}

	/*
	 *	Copia os elementos do titulo para tittrb.
	 */
	cont = 0;
	cptrab = tittrb;
	while (cont < 3)
	{
		cp = tit->t_pont[cont];

		tam[cont] = 0;
		pont[cont] = cptrab;

		if (tit->t_pont[cont] != NOSTR)
		{
			while (*cp != NOCHAR)
			{
				tam[cont] += identcarac (&cp);
			}
		}
		*cptrab++ = NOCHAR;

		cont++;
	}
	cptrab = buftrab;

	/*
	 *	Calcula o numero de brancos.
	 */
	nb1 = (ll / 2) + (ll % 2) - tam[0] - (tam[1] / 2) - (tam[1] % 2);
	nb2 = (ll / 2) - tam[2] - (tam[1] / 2);

	if ((nb1 <= 0)  &&  (nb2 > 0))
	{
		nb = ll - tam[0] - tam[1] - tam[2];
		nb2 = (nb / 2) + (nb % 2);
		nb1 = (nb / 2);
	}
	else
	{
		if ((nb2 <= 0)  &&  (nb1 > 0))
		{
			nb = ll - tam[0] - tam[1] - tam[2];
			nb1 = (nb / 2) + (nb % 2);
			nb2 = (nb / 2);
		}
	}

	/*
	 *	Seleciona o buffer a ser usado.
	 */
	if (tit == tl)
		cp = bfout1;
	else
		cp = titout;

	/*
	 *	Monta o titulo.
	 */
	if (laser  &&  tit->t_italico)
	{
		strcpy (cp, LITON);
		cp = strend (cp);
	}
	strcpy (cp, pont[0]);
	cp = strend (cp);
	cp = poebr (nb1, cp);	
	strcpy (cp, pont[1]);
	cp = strend (cp);
	cp = poebr (nb2, cp);	
	strcpy (cp, pont[2]);
	if (laser  &&  tit->t_italico)
	{
		strcat (cp, LITOFF);
	}

	/*
	 *	Coloca o titulo.
	 */
	if (tit == tl)
	{
		if (tit->t_negrito)
			out2 = bfout1;

		if (tit->t_sublinhado)
			mtsub ();

		nbrancos = po;
		lit = TRUE;

		trtpag (0);

		lit = FALSE;
		atnbrancos ();
	}
	else
	{
		if (laser)
		{
			if (tit->t_negrito)
				fprintf (stdout, "%s", LBOLD);

			if (tit->t_sublinhado)
				fprintf (stdout, "%s", LULON);

			fprintf (stdout, "%s\n", titout);

			if (tit->t_negrito)
				fprintf (stdout, "%s", LNORMAL);

			if (tit->t_sublinhado)
				fprintf (stdout, "%s", LULOFF);
		}
		else
		{
			if (tit->t_sublinhado)
			{
				out1 = titout;
				mtsub ();	
				out3 = NOSTR;
				out1 = bfout1;

				if (tit->t_negrito)
					fprintf (stdout, "%s\r", bfout3);

				fprintf (stdout, "%s\r", bfout3);
			}
	
			if (tit->t_negrito)
				fprintf (stdout, "%s\r", titout);

			fprintf (stdout, "%s\n", titout);
		}
	}

}	/* end poetit */

/*
 ****************************************************************
 *	Coloca a linha formatada na pagina.			*
 ****************************************************************
 */
void
poelin (void)
{
	register int	nb;
	register char	*ptb;

	if (debug)
	{
		if (out3 != NOSTR)
			fprintf (stderr, "linha3 = %s\n", out3);

		if (out2 != NOSTR)
			fprintf (stderr, "linha2 = %s\n", out2);

		fprintf (stderr, "linha1 = %s\n", out1);
	}

	if (laser)
	{
		/*
		 *	Monta a linha para a impressora laser.
		 */
		if (out2 != NOSTR  ||  out3 != NOSTR)
		{
			ptb = crialbuf (out1, out2, out3);
			rpt2 = 0;
			out2 = NOSTR;
			rpt3 = 0;
			out3 = NOSTR;
		}
		else
			ptb = out1;

	}
	else
	{
		if (out3 != NOSTR)
		{
			/*
			 *	Imprime os sublinhados.	
			 */
			if (tembloco)
			{
				cpblc = poebr (nbrancos, cpblc);
				if (out2 != NOSTR)
				{
					strcpy (cpblc, out3);
					cpblc = strend (cpblc);
					*cpblc++ = '\r';
					cpblc = poebr (nbrancos, cpblc);
					strcpy (cpblc, out3);
					cpblc = strend (cpblc);
					*cpblc++ = '\r';
				}
				else
				{
					strcpy (cpblc, out3);
					cpblc = strend (cpblc);
					*cpblc++ = '\r';
				}
			}
			else
			{
				nb = nbrancos;
				while (nb-- > 0)
					putchar (' ');

				if (out2 != NOSTR)
				{
					fprintf (stdout, "%s\r", out3);
					nb = nbrancos;
					while (nb-- > 0)
						putchar (' ');

					fprintf (stdout, "%s\r", out3);
				}
				else
				{
					fprintf (stdout, "%s\r", out3);
				}
			}

			rpt3 = 0;
			out3 = NOSTR;
		}

		if (out2 != NOSTR)
		{
			/*
			 *	Imprime as palavras em negrito.
			 */
			if (tembloco)
			{
				cpblc = poebr (nbrancos, cpblc);
				strcpy (cpblc, out2);
				cpblc = strend (cpblc);
				*cpblc++ = '\r';
			}
			else
			{
				nb = nbrancos;
				while (nb-- > 0)
					putchar (' ');
				fprintf (stdout, "%s\r", out2);
			}

			rpt2 = 0;
			out2 = NOSTR;
		}

		ptb = out1;
	}

	/*
	 *	Imprime a linha.
	 */
	if (tembloco)
	{
		cpblc = poebr (nbrancos, cpblc);
		strcpy (cpblc, ptb);
		cpblc = strend (cpblc);
	}
	else
	{
		nb = nbrancos;
		while (nb-- > 0)
			putchar (' ');
		fprintf (stdout, "%s", ptb);
	}

	if (!lst  &&  !lit)
	{
		if (iptit)
		{
			/*
			 *	Atualiza as variaveis referentes ao '.ip'.
			 */
			atnbrancos ();
			attamlin ();
			iptit = FALSE;
		}

		if (pp > 0)
		{
			/*
			 *	Fim da primeira linha do paragrafo padrao.
			 */
			pp = 0;
			atnbrancos ();
			attamlin ();
		}
	}

}	/* end poelin */

/*
 ****************************************************************
 *	Pula linhas no texto formatado.				*
 ****************************************************************
 */
void
poenl (int cnt)
{
	register int	i,
			nlin;

	if (tembloco)
	{
		if (temrod)
		{
			nlin = tampag -
				(rodprox == (BLOCO *)NULL ? linhaescrita : 0);

			for (i = cnt * ls; i > 0; i--)
			{
				*cpblc++ = '\n';
				blccorr->b_nlinhas++;
				/*
				 *	Verifica fim de rodape.
				 *	Ultima linha igual ao fim da pagina.
				 */
				if (--nlin <= 0)
				{
					linrod = 0;
					attampag ();
					fimbloco = TRUE;
					break;
				}
				linrod++;
				attampag ();
			}
		}
		else
		{
			nlin = tampag + linflut + linrod;

			for (i = cnt * ls; i > 0; i--)
			{
				*cpblc++ = '\n';
				/*
				 *	Verifica fim de bloco.
				 *	Bloco do tamanho da pagina.
				 */
				if (++(blccorr->b_nlinhas) == nlin)
				{
					fimbloco = TRUE;
					break;
				}
			}
		}	
		*cpblc = NOCHAR;
	}
	else
	{
		for (i = cnt * ls; i > 0; i--)
		{
			putchar ('\n');

			/*
			 *	Verifica o fim da pagina.
			 */
			if (++linhaescrita == tampag)
			{
				fimpag ();
				break;
			}
		}
	}

}	/* end poenl */

/*
 ****************************************************************
 *	Monta um buffer com bold e underline para a laser.	*
 ****************************************************************
 */
char *
crialbuf (char *cp1, char *cp2, char *cp3)
{
	static	 char	laserbuf[3 * BLSZ];
	register char	*cp;
	BOOL		onbold;
	BOOL		onul;

	cp = laserbuf;
	onbold = FALSE;
	onul = FALSE;

	while ((cp2 != NOSTR  &&  *cp2 != NOCHAR)
				||  (cp3 != NOSTR  &&  *cp3 != NOCHAR))
	{
		/*
		 *	Trata o negrito.
		 */
		if (cp2 != NOSTR  &&  *cp2 != NOCHAR)
		{
			if (*cp2 == *cp1  &&  *cp2 != ' '  &&  !onbold)
			{
				onbold = TRUE;
				strcpy (cp, LBOLD);
				cp = strend (cp);
			}
			else
			{
				if (*cp2 != *cp1  &&  onbold)
				{
					onbold = FALSE;
					strcpy (cp, LNORMAL);
					cp = strend (cp);
				}
			}

			cp2++;
		}

		/*
		 *	Trata o sublinhado.
		 */
		if (cp3 != NOSTR  &&  *cp3 != NOCHAR)
		{
			if (*cp3 == '_'  &&  !onul)
			{
				onul = TRUE;
				strcpy (cp, LULON);
				cp = strend (cp);
			}
			else
			{
				if (*cp3 == ' '  &&  *cp1 != ' '  &&  onul)
				{
					onul = FALSE;
					strcpy (cp, LULOFF);
					cp = strend (cp);
				}
			}

			cp3++;
		}

		*cp++ = *cp1++;
	}

	if (onbold)
	{
		strcpy (cp, LNORMAL);
		cp = strend (cp);
	}

	if (onul)
	{
		strcpy (cp, LULOFF);
		cp = strend (cp);
	}

	/*
	 *	Completa a linha.
	 */
	while (*cp1 != NOCHAR)
		*cp++ = *cp1++;

	*cp = NOCHAR;

	return (laserbuf);

}	/* end crialbuf */
