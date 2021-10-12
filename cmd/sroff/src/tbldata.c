/*
 ****************************************************************
 *								*
 *			tbldata.c				*
 *								*
 *	Trata as linhas de dado da tabela.			*
 *								*
 *	Versão	2.0.0, de 08.07.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtdata		alelemto			*
 *		inicdat		calclarg			*
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
 *	Trata os dados (conteudo da tabela).			*
 ****************************************************************
 */
void
trtdata (void)
{
	DADOS		**paux;
	DADOS		*dtp;
	FORMATO		*fmtp;
	int		tam,
			tamaux,
			tamesq,
			tamdir;
	int		cont;
	register int	i;
	register char	*cp,
			*cpaux,
			*cptab,
			*cppt;
	
	/*
	 *	Aloca area para vetor de ponteiros para vetores de dados.
	 */
	if (maxdata % 20 == 0)
	{
		if ((paux = (DADOS **)realloc (dtpont,
			(maxdata + 20) * sizeof (DADOS *))) == (DADOS **)NULL)
		{
			fprintf (stderr, no_mem_text, pgname);
			return;
		}
		dtpont = paux;
	}
	maxdata++;

	/*
	 *	Cria vetor de dados.
	 */
	if ((dtpont[contdata] =
		(DADOS *)smalloc (numcol * sizeof (DADOS))) == (DADOS *)NULL)
	{
		fprintf (stderr, no_mem_text, pgname);
		return;
	}
	
	dtp = dtpont[contdata];
	inicdat (dtp);

	/*
	 *	Identifica linha com um unico caracter.
	 */
	cp = bufin;
	switch (*cp)
	{
	    case '\0':
		dtp->d_clinha = ' ';
		contdata++;
		return;

	    case ' ':
	    case '-':
	    case '=':
		if (*++cp == NOCHAR)
		{
			dtp->d_clinha = *--cp;
			contdata++;
			return;
		}
		cp--;
	}

	cpaux = strend (bufin);
	*cpaux = tab;
	*++cpaux = NOCHAR;

	fmtp = fmtpont[contfmt]; 
	dtp->d_fmtp = fmtp;	
	cont = 0;

	/*
	 *	Trata os campos das linhas de dados.
	 */
	while (cont < numcol)
	{
		if (*cp == NOCHAR  ||  (cptab = strchr (cp, tab)) == NOSTR)
		{
			/*
			 *	Fim da linha.
			 */
			for (i = cont; i < numcol; i++)
			{
				dtp->d_ccampo = ' ';
				dtp++;
			}

			contdata++;
			if (contfmt < maxfmt)
				contfmt++;
			return;
		}

		*cptab = NOCHAR;

		if (*cp == NOCHAR  ||  streq (cp, "\\^"))
		{
			/*
			 *	Campo em branco.
			 */
			dtp->d_ccampo = ' ';
			cp = cptab + 1;
			dtp += (fmtp->f_ncol + 1);
			cont += (fmtp->f_ncol + 1);
			fmtp += (fmtp->f_ncol + 1);
			continue;
		}

		if (streq (cp, "-")  ||  streq (cp, "="))
		{
			/*
			 *	Campo com '=' ou '-'.
			 */
			dtp->d_ccampo = *cp;
			cp = cptab + 1;
			dtp += (fmtp->f_ncol + 1);
			cont += (fmtp->f_ncol + 1);
			fmtp += (fmtp->f_ncol + 1);
			continue;
		}

		if (streq (cp, "\\-")  ||  streq (cp, "\\="))
		{
			/*
			 *	Elemento com '=' ou '-'.
			 */
			dtp->d_celemto = *++cp;
			cp = cptab + 1;
			dtp += (fmtp->f_ncol + 1);
			cont += (fmtp->f_ncol + 1);
			fmtp += (fmtp->f_ncol + 1);
			continue;
		}

		if (*cp == '\\'  &&  *(cp + 1) == 'R'  &&  *(cp + 3) == NOCHAR)
		{
			/*
			 *	Elemento com um caracter definido.
			 */
			dtp->d_celemto = *(cp + 2);
			cp = cptab + 1;
			dtp += (fmtp->f_ncol + 1);
			cont += (fmtp->f_ncol + 1);
			fmtp += (fmtp->f_ncol + 1);
			continue;
		}

		if (*cp == '\\'  &&  *(cp + 1) == '&'  &&  *(cp + 3) == NOCHAR)
			/*
			 *	Elemento e' o caracter definido.
			 */
			cp += 2;

		/*
		 *	Trata o campo de acordo com o formato correspondente.
		 */
		switch (fmtp->f_fmt)
		{
		    case 'l':
		    case 'r':
		    case 'c':
			if ((tam = alelemto (dtp, cp)) == 0)
				return;

			tamaux = calclarg (fmtp, tam, cont);
			if (tamaux > crtcol[cont].c_larg)
				crtcol[cont].c_larg = tamaux;

			dtp += fmtp->f_ncol;
			cont += fmtp->f_ncol;
			fmtp += fmtp->f_ncol;
			break;

		    case 'a':
			if ((tam = alelemto (dtp, cp)) == 0)
				return;

			tamaux = calclarg (fmtp, tam, cont);
			if (tamaux > crtcol[cont].c_larg)
				crtcol[cont].c_larg = tamaux;

			if (tam > crtcol[cont].c_subcol)
				crtcol[cont].c_subcol = tam;

			dtp += fmtp->f_ncol;
			cont += fmtp->f_ncol;
			fmtp += fmtp->f_ncol;
			break;

		    case 's':
			continue;

		    case '-':
		    case '=':
		    case ' ':
			dtp->d_ccampo = fmtp->f_fmt;
			break;

		    case 'n':
			if ((tam = alelemto (dtp, cp)) == 0)
				return;

			if ((cppt = strrchr (cp, '.')) == NOSTR)
			{
				/*
				 *	Numero sem ponto decimal.
				 */
				dtp->d_tamesq = tam;
				if (tam > crtcol[cont].c_esq)
				{
					crtcol[cont].c_esq = tam;
					tam += (crtcol[cont].c_dir +
						crtcol[cont].c_ponto);
					tamaux = calclarg (fmtp, tam, cont);
					if (tamaux > crtcol[cont].c_larg)
						crtcol[cont].c_larg = tamaux;
				}

				dtp += fmtp->f_ncol;
				cont += fmtp->f_ncol;
				fmtp += fmtp->f_ncol;
				break;
			}
			*cppt = NOCHAR;

			/*
			 *	Numero com ponto decimal.
			 */
			tamesq = strlen (cp);
			tamdir = strlen (++cppt);
			dtp->d_tamesq = tamesq;

			if (tamesq > crtcol[cont].c_esq)
				crtcol[cont].c_esq = tamesq;

			if (tamdir > crtcol[cont].c_dir)
				crtcol[cont].c_dir = tamdir;

			crtcol[cont].c_ponto = 1;

			tam = crtcol[cont].c_esq + crtcol[cont].c_dir + 1;
			tamaux = calclarg (fmtp, tam, cont);

			if (tamaux > crtcol[cont].c_larg)
				crtcol[cont].c_larg = tamaux;

			dtp += fmtp->f_ncol;
			cont += fmtp->f_ncol;
			fmtp += fmtp->f_ncol;
			break;

		}	/* switch */

		dtp++;
		fmtp++;
		cont++;
		cp = cptab + 1;

	}

	if (contfmt < maxfmt)
		contfmt++;
	contdata++;

}	/* end trtdata */

/*
 ****************************************************************
 *	Aloca area para o elemento da tabela.			*
 ****************************************************************
 */
int
alelemto (DADOS *dtp, char *cp)
{
	int	tam;

	if ((dtp->d_elemto = (char *)smalloc ((tam = strlen (cp)) + 1)) == NOSTR)
	{
		fprintf (stderr, no_mem_text, pgname);
		return (0); 
	}
	strcpy (dtp->d_elemto, cp);
	dtp->d_tam = tam;
	return (tam);

}	/* end alelemto */

/*
 ****************************************************************
 *	Inicia estrutura de dados.				*
 ****************************************************************
 */
void
inicdat (DADOS *dtp)
{
	register int	i;

	for (i = 0; i < numcol; i++)
	{
		dtp->d_elemto = NOSTR;
		dtp->d_tam = 0;
		dtp->d_tamesq = 0;
		dtp->d_fmtp = (FORMATO *)NULL;
		dtp->d_ccampo = dtp->d_celemto = dtp->d_clinha = NOCHAR;
		dtp++;
	}

}	/* end inicdat */

/*
 ****************************************************************
 *	Divide o tamanho da coluna pelas colunas 's'.		*
 ****************************************************************
 */
int
calclarg (FORMATO *fmtp, int tam, int cont)
{
	register int	i,
			tamaux;

	if (fmtp->f_ncol != 0)
	{
		for (i = 0; i < fmtp->f_ncol; i++)
			tam -= crtcol[cont + i].c_esp;

		tamaux = tam / (fmtp->f_ncol + 1) +
			((tam % (fmtp->f_ncol + 1) == 0) ? 0 : 1);

		for (i = 1; i <= fmtp->f_ncol; i++)
		{
			if (tamaux > crtcol[cont + i].c_larg)
				crtcol[cont + i].c_larg = tamaux;
		}
	}
	else
		return (tam);

	return (tamaux);

}	/* end calclarg */

