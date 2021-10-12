/*
 ****************************************************************
 *								*
 *			tblmt.c					*
 *								*
 *	Monta a tabela.						*
 *								*
 *	Versão	2.0.0, de 08.07.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		mttbl		formcmp				*
 *		atlarg		compnegr			*
 *		compul		inicdiv				*
 *		imprdiv		inicimpr			*
 *		fimimpr		libtbl				*
 *		mostra						*
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
#include <malloc.h>

#include "../h/sroff.h"
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variaveis globais.					*
 ****************************************************************
 */
static	char	*cpout;
static	char	*cptbl;
static	char	*cpout2;
static	char	*cpout3;
static	int	larg;
static	int	largtbl;
static	char	divlin0[BLSZ];
static	char	divlin1[BLSZ];
static	char	divlin2[BLSZ];
static	char	divlin3[BLSZ];
static	int	vezlindiv;
static	char	buftitlin[BLSZ];
static	char	buftitdiv[BLSZ];
static	char	buftitul[BLSZ];

/*
 ****************************************************************
 *	Monta a tabela.						*
 ****************************************************************
 */
void
mttbl (void)
{
	register 	i,
			num,
			nb;
	long		contcol,
			lcol,
			coef,
			esp,
			ncol,
			maisum;
	FORMATO		*fmtp;
	DADOS		*dtp;

	/*
	 *	Calcula largura total da tabela.
	 */
	if (doublebox)
		largtbl = 6;
	else
		if (box  ||  allbox)
			largtbl = 4;
		else
			largtbl = 0;

	atlarg ();

	for (i = 0, esp = 0; i < numcol; i++)
	{
		largtbl += crtcol[i].c_larg;
		esp += crtcol[i].c_esp;
	}
	largtbl += (esp - crtcol[numcol - 1].c_esp / 2 -
				crtcol[numcol - 1].c_esp % 2);

	if (largtbl > BLSZ)
	{
		fprintf (stderr, "%s: Tabela com linha muito grande\n", pgname);
		libtbl ();
		return;
	}

	if (debug)
		mostra ();

	if (expand)
	{
		/*
		 *	Expande a tabela.
		 */
		if (esp > 0)
			coef = (tamlin - largtbl) / esp;
		else
			coef = 0;

		if (coef > 1)
		{
			largtbl -= esp;

			for (i = 0; i < numcol; i++)
			{
				crtcol[i].c_esp *= coef;
				largtbl += crtcol[i].c_esp;
			}
		}
	}

	if (center)
	{
		/*
		 *	Centra a tabela.
		 */
		nbrancos += ((tamlin - largtbl) / 2);
	}

	if (allbox)
	{
		/*
		 *	Inicializa linhas divisorias.
		 */
		inicdiv (divlin0);
		inicdiv (divlin1);
		inicdiv (divlin2);
		inicdiv (divlin3);
		vezlindiv = 0;
	}
	cpout = bfout1;
	inicimpr ();

	/*
	 *	Arruma as linhas da tabela.
	 */
	contdata = 0;
	while (contdata < maxdata)
	{
		dtp = dtpont[contdata];
		fmtp = dtp->d_fmtp;
#undef	DEBUG
#ifdef	DEBUG
		if (fmtp == (FORMATO *)NULL)
			fprintf (stderr, "PONTO 1\n");
#endif	DEBUG

		contcol = 0;
		cpout = cptbl;

		if (dtp->d_clinha != NOCHAR)
		{
			/*
			 *	Linha com um unico tipo de caracter.
			 */
			if (allbox)
				imprdiv ();

			for (i = 0; i < larg; i++)
				*cpout++ = dtp->d_clinha;

			if (ul > 0)
				mtsub ();

			if (tpchar == NEGRITO)
				out2 = bfout1;

			/*
			 *	Guarda linha de título.
			 */
			if (contdata == lintit)
			{
				temtit = TRUE;
				strcpy (buftitlin, bfout1);
				strcpy (buftitdiv, divlin2);
				if (ul > 0)
				{
					titul = TRUE;
					strcpy (buftitul, out3);
				}
				if (tpchar == NEGRITO)
					titneg = TRUE;
			}	

			trtpag (0);

			contdata++;

			continue;
		}

		cpout2 = NOSTR;
		for (i = 0; i < numcol; i++)
		{
			if (fmtp->f_tipo != tpchar)
			{
				/*
				 *	Linha com campos com tipos diferentes.
				 */
				cpout2 = bfout2;
				*cpout2++ = '|';
				*cpout2++ = '|';
				cpout2 += (largtbl - 4);
				*cpout2++ = '|';
				*cpout2++ = '|';
				*cpout2 = NOCHAR;
				cpout2 = bfout2 + (cptbl - out1);
				break;
			}
			fmtp++;
		}

		fmtp = dtp->d_fmtp;
#ifdef	DEBUG
		if (fmtp == (FORMATO *)NULL)
			fprintf (stderr, "PONTO 2\n");
#endif	DEBUG

		if (ul > 0)
		{
			/*
			 *	Linha cujos campos devem ser sublinhados.
			 */
			cpout3 = bfout3;
			*cpout3++ = ' ';
			*cpout3++ = ' ';
			cpout3 += (largtbl - 4);
			*cpout3++ = ' ';
			*cpout3++ = ' ';
			*cpout3 = NOCHAR;
			cpout3 = bfout3 + (cptbl - out1);
		}
		else
			cpout3 = NOSTR;

		if (allbox  ||  box  ||  doublebox)
			if (dtp->d_ccampo == NOCHAR)
			{
				*cpout++ = ' ';
				compnegr (ROMANO, ' ', 1, NOSTR);
				compul (1, NOSTR);
				maisum = 0;
			}
			else
				maisum = 1;
		else
			maisum = 0;	

		/*
		 *	Arruma os campos da linha.
		 */
		for (/* vazio */; /* vazio */; /* vazio */)
		{
			/*
			 *	Calcula largura do campo.
			 */
			lcol = crtcol[contcol].c_larg;
			ncol = fmtp->f_ncol;

			for (i = 0; i < ncol; /* vazio */)
			{
				lcol += crtcol[contcol + i++].c_esp;
				lcol += crtcol[contcol + i].c_larg;
			}

			if (dtp->d_ccampo != NOCHAR)
			{
				/*
				 *	Campo formado por um tipo de caracter.
				 */
				num = lcol +
				    crtcol[contcol + ncol].c_esp / 2 + maisum;
				for (i = 0; i < num; i++)
					*cpout++ = dtp->d_ccampo;
				compnegr (fmtp->f_tipo, dtp->d_ccampo,
								num, NOSTR);
				compul (num, NOSTR);
			}
			else
			{
				if (dtp->d_celemto != NOCHAR)
				{
					/*
					 *	Elemento formado por
					 *	um tipo de caracter.
					 */
					for (i = 0; i < lcol; i++)
						*cpout++ = dtp->d_celemto;
					compnegr (fmtp->f_tipo, dtp->d_celemto,
								lcol, NOSTR);
					compul (lcol, NOSTR);

					nb = crtcol[contcol + ncol].c_esp / 2;

					for (i = 0; i < nb; i++)
						*cpout++ = ' ';
					compnegr (ROMANO, ' ', nb, NOSTR);
					compul (nb, NOSTR);
				}
				else
					/*
					 *	Campo formado por uma cadeia.
					 */
					formcmp (contcol, dtp, fmtp,
								lcol, ncol);
			}

			contcol += ncol;

			if ((contcol + 1) >= numcol)
				/*
				 *	Fim da linha.
				 */
				break;
			else
			{
				/*
				 *	Não foi o ultimo campo.
				 *	Coloca separacao para o proximo.
				 */
				num = crtcol[contcol].c_esp / 2 +
						crtcol[contcol].c_esp % 2;

				if (fmtp->f_sep > 0)
				{
					/*
					 *	Coloca separadores verticais.
					 */
					num -= fmtp->f_sep;
					for (i = 0; i < fmtp->f_sep; i++)
					{
						if (allbox)
						{
							divlin0[cpout -
								bfout1] = '+';
							divlin1[cpout -
								bfout1] = '+';
							divlin2[cpout -
								bfout1] = '+';
						}
						*cpout++ = '|';
					}
					compnegr (tpchar, '|', num, NOSTR);
					compul (num, NOSTR);
				}
				else
				{
					if (allbox)
					{
						/*
						 *	Coloca separador.
						 */
						num--;
						divlin0[cpout - bfout1] = '+';
						divlin1[cpout - bfout1] = '+';
						divlin2[cpout - bfout1] = '+';
						*cpout++ = '|';
						compnegr (tpchar, '|',
								1, NOSTR);
						compul (1, NOSTR);
					}
				}

				/*
				 *	Inicia proximo campo.
				 */
				if ((++dtp)->d_ccampo == NOCHAR)
				{
					for (i = 0; i < num; i++)
						*cpout++ = ' ';
					compnegr (ROMANO, ' ', num, NOSTR);
					compul (num, NOSTR);
				}
				else
				{
					for (i = 0; i < num; i++)
						*cpout++ = dtp->d_ccampo;
#ifdef	DEBUG
		if (dtp->d_fmtp == (FORMATO *)NULL)
			fprintf (stderr, "PONTO 3\n");
#endif	DEBUG

if (dtp->d_fmtp != (FORMATO *)NULL)
{
					compnegr
					(	dtp->d_fmtp->f_tipo,
						dtp->d_ccampo,
						num,
						NOSTR
					);
					compul (num, NOSTR);
}
else
{
					compnegr
					(	0,
						0,
						num,
						NOSTR
					);
					compul (num, NOSTR);
}
				}
				dtp--;

				if (maisum == 1)
					maisum = 0;

				contcol++;
				ncol++;
				dtp += ncol;
				fmtp += ncol;
			}
		}

		/*
		 *	Termina linha.
		 */
		if (allbox  ||  box  || doublebox)
		{
			if (dtp->d_ccampo != NOCHAR)
			{
				*cpout = dtp->d_ccampo;
				compnegr (fmtp->f_tipo, dtp->d_ccampo,
								1, NOSTR);
				compul (1, NOSTR);
			}
			else
			{
				*cpout = ' ';
				compnegr (ROMANO, ' ', 1, NOSTR);
				compul (1, NOSTR);
			}
		}

		/*
		 *	Guarda linha de título.
		 */
		if (contdata == lintit)
		{
			temtit = TRUE;
			strcpy (buftitlin, bfout1);
			strcpy (buftitdiv, divlin2);
			if (cpout3 != NOSTR)
			{
				titul = TRUE;
				strcpy (buftitul, bfout3);
			}
			if (tpchar == NEGRITO)
				titneg = TRUE;
		}	

		/*
		 *	Imprime linha divisoria.
		 */
		if (allbox)
			imprdiv ();

		/*
		 *	Imprime a linha.
		 */
		if (cpout2 != NOSTR)
			out2 = bfout2;
		else
			if (tpchar == NEGRITO)
				out2 = bfout1;

		if (cpout3 != NOSTR)
			out3 = bfout3;

		trtpag (0);

		contdata++;
	}

	/*
	 *	Imprime ultima linha divisoria.
	 */
	if (allbox)
	{
		out1 = (vezlindiv == 0 ? divlin0 : divlin1);
		if (tpchar == NEGRITO)
			out2 = out1;

		trtpag (0);
		out1 = bfout1;
	}

	fimimpr ();

	atnbrancos ();

	libtbl ();

}	/* end mttbl */

/*
 ****************************************************************
 *	Coloca campo alfanumerico na tabela.			*
 ****************************************************************
 */
int
formcmp (int contcol, DADOS *dtp, FORMATO *fmtp, int lcol, int ncol)
{
	register int	i,
			nb;

	/*
	 *	O campo e' colocado de acordo com o formato correspondente.
	 */
	switch (fmtp->f_fmt)
	{
	    case 's':
		nb = lcol + crtcol[contcol].c_esp / 2;
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);
		break;
		
	    case 'c':
		nb = (lcol - dtp->d_tam) / 2;
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		strcpy (cpout, dtp->d_elemto);
		compnegr (fmtp->f_tipo, NOCHAR, 0, dtp->d_elemto);
		compul (0, dtp->d_elemto);
		cpout = strend (cpout);

		nb = lcol - dtp->d_tam - nb +
			(crtcol[contcol + ncol].c_esp / 2); 
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		break;

	    case 'l':
		strcpy (cpout, dtp->d_elemto);
		compnegr (fmtp->f_tipo, NOCHAR, 0, dtp->d_elemto);
		compul (0, dtp->d_elemto);
		cpout = strend (cpout);

		nb = lcol - dtp->d_tam + (crtcol[contcol + ncol].c_esp / 2);
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		break;

	    case 'r':
		nb = lcol - dtp->d_tam;
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		strcpy (cpout, dtp->d_elemto);
		compnegr (fmtp->f_tipo, NOCHAR, 0, dtp->d_elemto);
		compul (0, dtp->d_elemto);
		cpout = strend (cpout);

		nb = crtcol[contcol + ncol].c_esp / 2;
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		break;

	    case 'a':
		nb = (lcol - crtcol[contcol].c_subcol) / 2;
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		strcpy (cpout, dtp->d_elemto);
		compnegr (fmtp->f_tipo, NOCHAR, 0, dtp->d_elemto);
		compul (0, dtp->d_elemto);
		cpout = strend (cpout);

		nb = lcol - nb - dtp->d_tam +
			(crtcol[contcol + ncol].c_esp / 2);

		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		break;

	    case 'n':
		nb = (lcol - crtcol[contcol].c_esq -
		      crtcol[contcol].c_ponto - crtcol[contcol].c_dir) / 2;
		nb += (crtcol[contcol].c_esq - dtp->d_tamesq);
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		strcpy (cpout, dtp->d_elemto);
		compnegr (fmtp->f_tipo, NOCHAR, 0, dtp->d_elemto);
		compul (0, dtp->d_elemto);
		cpout = strend (cpout);

		nb = lcol - nb - dtp->d_tam +
			(crtcol[contcol + ncol].c_esp / 2);
		for (i = 0; i < nb; i++)
			*cpout++ = ' ';
		compnegr (ROMANO, ' ', nb, NOSTR);
		compul (nb, NOSTR);

		break;
	}

	return (ncol);

}	/* end formcmp */

/*
 ****************************************************************
 *	Atualiza as larguras das colunas de acordo com eflag.	*
 ****************************************************************
 */
void
atlarg (void)
{
	register int	i,
			l,
			prim;

	for (prim = 0, l = 0, i = 0; i < numcol; i++)
	{
		if (crtcol[i].c_eflag)
		{
			if (l > crtcol[i].c_larg)
				crtcol[i].c_larg = l;
			else
			{
				if (l != crtcol[i].c_larg)
				{
					l = crtcol[i].c_larg;
					prim = i;
				}
			}
		}
	}

	for (i = 0; i < prim; i++)
	{
		if (crtcol[i].c_eflag)
			crtcol[i].c_larg = l;
	}

}	/* end atlarg */

/*
 ****************************************************************
 *	Preenche linha com negrito.				*
 ****************************************************************
 */
void
compnegr (TPCHAR tipo, char c, int num, char *cp)
{
	register int	i;

	if (cpout2 != NOSTR)
	{
		if (tipo == NEGRITO)
		{
			if (cp == NOSTR)
			{
				for (i = 0; i < num; i++)
					*cpout2++ = c;
			}
			else
			{
				strcpy (cpout2, cp);
				cpout2 = strend (cpout2);
			}
		}
		else
		{
			if (cp == NOSTR)
			{
				for (i = 0; i < num; i++)
					*cpout2++ = ' ';
			}
			else
			{
				for (i = strlen (cp); i > 0; i--)
					*cpout2++ = ' ';
			}
		}
	}

}	/* end compnegr */

/*
 ****************************************************************
 *	Preenche linha com 'sublinhados'.			*
 ****************************************************************
 */
void
compul (int num, char *cp)
{
	register int	i;

	if (cpout3 != NOSTR)
	{
		if (cp == NOSTR)
		{
			for (i = 0; i < num; i++)
				*cpout3++ = ' ';
		}
		else
		{
			for (i = strlen (cp); i > 0; i--)
				*cpout3++ = '_';
		}
	}

}	/* end compul */

/*
 ****************************************************************
 *	Inicia linha divisoria.					*
 ****************************************************************
 */
void
inicdiv (char *cp)
{
	register int	i;

	if (doublebox)
	{
		*cp++ = '|';
		*cp++ = '+';
		for (i = 0; i < (largtbl - 4); i++)
			*cp++ = '-';
		*cp++ = '+';
		*cp++ = '|';
		*cp = NOCHAR;
	}
	else
	{
		*cp++ = '+';
		for (i = 0; i < (largtbl - 2); i++)
			*cp++ = '-';
		*cp++ = '+';
		*cp = NOCHAR;
	}

}	/* end inicdiv */

/*
 ****************************************************************
 *	Imprime uma linha disória				*
 ****************************************************************
 */
void
imprdiv (void)
{
	char	*cp;
	char	*cpaux1;
	char	*cpaux2;
	char	*cpaux3;
	char	bufaux[BLSZ];

	if (!tembloco  &&  (linhaescrita + (3 * ls)) > tampag)
	{
		if (contdata == 0)
			out1 = divlin2;
		else
			out1 = divlin3;

		if (tpchar == NEGRITO)
			out2 = out1;

		trtpag (0);

		if (linhaescrita != 0)
			trtpag (-1);

		if (vezlindiv == 0)
		{
			strcpy (divlin0, divlin1);
			cp = divlin1;
		}
		else
		{
			strcpy (divlin1, divlin0);
			cp = divlin0;
		}

		if (temtit  &&  contdata != lintit)
		{
			out1 = buftitdiv;
			if (titneg)
				out2 = buftitdiv;
			trtpag (0);

			out1 = buftitlin;
			if (titneg)
				out2 = buftitdiv;
			if (titul)
				out3 = buftitul;
			trtpag (0);

			cpaux1 = cp;
			cpaux2 = buftitdiv;
			cpaux3 = bufaux;
			while (*cpaux1 != NOCHAR)
			{
				if (*cpaux1 == '+'  ||  *cpaux2 == '+')
					*cpaux3 = '+';
				else
					*cpaux3 = '-';

				cpaux1++;
				cpaux2++;
				cpaux3++;
			}
			*cpaux3 = NOCHAR;
		}
	}
	else
	{
		if (vezlindiv == 0)
		{
			cp = divlin0;
			vezlindiv = 1;
		}
		else
		{
			cp = divlin1;
			vezlindiv = 0;
		}
	}

	out1 = cp;
	if (tpchar == NEGRITO)
		out2 = cp;

	trtpag (0);
	out1 = bfout1;

	inicdiv (cp);
	strcpy (divlin3, divlin2);
	inicdiv (divlin2);

}	/* end imprdiv */

/*
 ****************************************************************
 *	Inicia montagem da tabela.				*
 ****************************************************************
 */
void
inicimpr (void)
{
	register int	i;

	if (doublebox)
	{
		/*
		 *	Coloca caixa dupla.
		 */
		cpout = bfout1;
		*cpout++ = '+';
		for (i = 0; i < largtbl - 2; i++)
			*cpout++ = '-';
		*cpout++ = '+';
		*cpout = NOCHAR;

		if (tpchar == NEGRITO)
			out2 = bfout1;

		if (!tembloco  &&  (linhaescrita + (ls * 4)) > tampag)
			trtpag (-1);

		trtpag (0);

		if (!allbox)
		{
			cpout = bfout1;
			*cpout++ = '|';
			*cpout = '+';
			cpout += (largtbl - 2); 
			*cpout-- = '|';
			*cpout = '+';

			if (tpchar == NEGRITO)
				out2 = bfout1;

			trtpag (0);
		}

		cpout = bfout1;
		*cpout++ = '|';
		*cpout = '|';
		cpout += (largtbl - 3);
		*cpout++ = '|';
		*cpout = '|';

		cptbl = bfout1 + 2;
		larg = largtbl - 4;
	}
	else
	{
		if (box  ||  allbox)
		{
			/*
			 *	Coloca caixa simples.
			 */
			if (!tembloco  &&  (linhaescrita + (ls * 3)) > tampag)
				trtpag (-1);

			if (!allbox)
			{
				cpout = bfout1;
				*cpout++ = '+';
				for (i = 0; i < largtbl - 2; i++)
					*cpout++ = '-';
				*cpout++ = '+';
				*cpout = NOCHAR;
		
				if (tpchar == NEGRITO)
					out2 = bfout1;

				trtpag (0);
			}

			cpout = bfout1;
			*cpout = '|';
			cpout += (largtbl - 1);
			*cpout++ = '|';
			*cpout = NOCHAR;
			cptbl = bfout1 + 1;
			larg = largtbl - 2;
		}
		else
		{
			cptbl = bfout1;
			cpout = bfout1 + largtbl;
			*cpout = NOCHAR;
			larg = largtbl;
		}
	}

}	/* end inicimpr */

/*
 ****************************************************************
 *	Termina montagem da tabela.				*
 ****************************************************************
 */
void
fimimpr (void)
{
	register int	i;

	if (doublebox)
	{
		/*
		 *	Coloca caixa dupla.
		 */
		if (allbox)
		{
			cpout = bfout1;
			*cpout++ = '+';
			for (i = 0; i < (largtbl - 2); i++)
				*cpout++ = '-';
			*cpout = '+';

			if (tpchar == NEGRITO)
				out2 = bfout1;

			trtpag (0);
		}
		else
		{
			cpout = bfout1 + 1;
			*cpout++ = '+';
			for (i = 0; i < (largtbl - 4); i++)
				*cpout++ = '-';
			*cpout++ = '+';

			if (tpchar == NEGRITO)
				out2 = bfout1;

			trtpag (0);

			cpout = bfout1;
			*cpout++ = '+';
			*cpout = '-';
			cpout += (largtbl - 2);
			*cpout-- = '+';
			*cpout = '-';

			if (tpchar == NEGRITO)
				out2 = bfout1;

			trtpag (0);
		}
	}
	else
	{
		if (box  &&  !allbox)
		{
			cpout = bfout1;
			*cpout++ = '+';
			for (i = 0; i < (largtbl - 2); i++)
				*cpout++ = '-';
			*cpout = '+';

			if (tpchar == NEGRITO)
				out2 = bfout1;

			trtpag (0);
		}
	}

}	/* end fimimpr */

/*
 ****************************************************************
 *	Libera area da tabela.					*
 ****************************************************************
 */
void
libtbl (void)
{
	sfree ();
	crtcol = (COLUNA *)NULL;
	free (fmtpont);
	fmtpont = (FORMATO **)NULL;
	free (dtpont);
	dtpont = (DADOS **)NULL;

}	/* end libtbl */

/*
 ****************************************************************
 *	DEBUG:	Mostra o conteudo da estrutura de dados.	*
 ****************************************************************
 */
void
mostra (void)
{
	register int	i, j;
	FORMATO		*fp;
	DADOS		*dp;

	for (i = 0; i < numcol; i++)
	{
		fprintf
		(	stderr,
			"larg = %d * esq = %d * ponto = %d *"
			"dir = %d * esp = %d * subcol = %d\n",
			crtcol[i].c_larg, crtcol[i].c_esq,
			crtcol[i].c_ponto, crtcol[i].c_dir,
			crtcol[i].c_esp, crtcol[i].c_subcol
		);
	}
	fprintf (stderr, "\n");

	for (j = 0; j <= maxfmt; j++)
	{
		fp = fmtpont[j];

		for ( i = 0; i < numcol; i++)
		{
			fprintf
			(	stderr,
				"fmt = %c * sep = %d * ncol = %d\n",
				fp->f_fmt, fp->f_sep, fp->f_ncol
			);  
			fp++;
		}
		fprintf (stderr, "\n");
	}

	for (j = 0; j < maxdata; j++)
	{
		dp = dtpont[j];

		for ( i = 0; i < numcol; i++)
		{
			fprintf
			(	stderr,
				"elemto = %s * campo = %c * elemto = %c * linha = %c\n",
				dp->d_elemto, dp->d_ccampo,
				dp->d_celemto, dp->d_clinha
			);

			fprintf	
			(	stderr,
				"tam = %d * tamesq = %d\n",
				dp->d_tam, dp->d_tamesq
			);
			dp++;
		}
		fprintf (stderr, "\n");
	}

}	/* end mostra */
