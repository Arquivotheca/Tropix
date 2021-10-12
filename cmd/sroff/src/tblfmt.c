/*
 ****************************************************************
 *								*
 *			tblfmt.c				*
 *								*
 *	Trata as linhas de formato da tabela.			*
 *								*
 *	Versão	2.0.0, de 08.07.87				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		trtfmt		calcnumcol			*
 *		identatr	lenum				*
 *		incr						*
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
#include <malloc.h>

#include "../h/sroff.h"
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Trata os formatos dos campos.				*
 ****************************************************************
 */
void
trtfmt (void)
{
	char	 	 *cp;
	FORMATO		 **paux;
	register FORMATO *fmtp;
	register int	 cont;
	register int	 i;
	
	/*
	 *	Aloca area para vetor de ponteiros de formatos.
	 */
	if (maxfmt % 10 == 0)
	{
		if ((paux = (FORMATO **)realloc (fmtpont,
		    (maxfmt + 10) * sizeof (FORMATO *))) == (FORMATO **)NULL)
		{
			fprintf (stderr, no_mem_text, pgname);
			return;
		}
		fmtpont = paux;
	}
	maxfmt++;

	/*
	 *	Cria vetor de caracteristicas das colunas.
	 */
	if (numcol == 0)
		if ((numcol = calcnumcol ()) == 0)
			return;

	/*
	 *	Cria vetor de formato das colunas.
	 */
	if ((fmtpont[contfmt] =
		(FORMATO *)smalloc ((int)(numcol * sizeof (FORMATO)))) ==
		(FORMATO *)NULL)
	{
		fprintf (stderr, no_mem_text, pgname);
		return;
	}

	/*
	 *	Inicializa o vetor de formatos.
	 */
	fmtp = fmtpont[contfmt];
	for (i = 0; i < numcol; i++)
	{
		fmtp->f_fmt = 'l';
		fmtp->f_tipo = tpchar;
		fmtp->f_ncol = fmtp->f_sep = 0;	
		fmtp++;
	}

	/*
	 *	Percorre a linha.
	 */
	cp = bufin;
	cont = 0;
	fmtp = fmtpont[contfmt];
	while (cont < numcol)
	{
		/*
		 *	Identifica o formato.
		 */
		switch (*cp)
		{
		    case 'l':
		    case 'r':
		    case 'c':
		    case 'n':
		    case 'a':
			fmtp->f_fmt = *cp++;
			cp = identatr (cp, fmtp, cont);
			break;

		    case 's':
#if (0)	/*************************************/
			if (cont == 0)
			{
				cont--;
				break;
			}
#endif	/*************************************/

			if (cont == 0)
			{
				errcmd ();
				cp++;
				break;
			}

			fmtp->f_fmt = *cp++;
			cp = identatr (cp, fmtp, cont);
			incr (NCOL, fmtp);

			break;

		    case '-':
		    case '=':
		    case '^':
			if (*cp == '^')
				fmtp->f_fmt = ' ';
			else
				fmtp->f_fmt = *cp;

			cp++;
			cp = identatr (cp, fmtp, cont);
			break;

		    case '|':
			if (cont == 0)
				errcmd ();

			while (*cp == '|')
			{
				if (cont != 0)
					incr (SEP, fmtp - 1);

				cp++;
			}
			cont--;
			fmtp--;
			break;

		    default:
			errcmd ();
			cp++;
			cont--;
			fmtp--;
			break;
		}

		if (*cp == '.')
		{
			/*
			 *	Fim dos formatos.
			 */
			tbl = 4;
			maxfmt = contfmt;
			contfmt = inicfmt;
			return;
		}

		if (*cp == NOCHAR)
			break;

		while (ESPACO (*cp))
			cp++;

		cont++;
		fmtp++;
	}

	/*
	 *	Percorre a linha ate o fim, procurando o '.'.
	 */
	while (*cp != NOCHAR)
	{
		if (*cp == '.')
		{
			tbl = 4;
			maxfmt = contfmt;
			contfmt = inicfmt;
			return;
		}
		cp++;
	}	

	contfmt++;

}	/* end trtfmt */

/*
 ****************************************************************
 *	Calcula numero de colunas da tabela.			*
 ****************************************************************
 */
int
calcnumcol (void)
{
	register char	*cp;
	register int	cont,
			i;

	cp = bufin;
	cont = 0;
	while (*cp != NOCHAR)
	{
		switch (*cp)
		{
		    case 'l':
		    case 'r':
		    case 'c':
		    case 'a':
		    case 'n':
		    case 's':
		    case '^':
		    case '-':
		    case '=':
			cont++;
			cp++;
			break;

		    default:
			cp++;
			break;
		}
	}

	/*
	 *	Aloca area para o vetor de caracteristicas das colunas.
	 */
	if ((crtcol = (COLUNA *)smalloc (cont * sizeof (COLUNA))) ==
		(COLUNA *)NULL)
	{
		fprintf (stderr, no_mem_text, pgname);
		return (0);
	}

	/*
	 *	Inicializa as caracteristicas das colunas.
	 */
	for (i = 0; i < cont; i++)
	{
		crtcol[i].c_larg = 0;	
		crtcol[i].c_esq = 0;	
		crtcol[i].c_ponto = 0;	
		crtcol[i].c_dir = 0;	
		crtcol[i].c_subcol = 0;	
		crtcol[i].c_esp = 3;	
		crtcol[i].c_eflag = FALSE;	
	}
	crtcol[i - 1].c_esp = 0;	

	return (cont);

}	/* end calcnumcol */

/*
 ****************************************************************
 *	Reconhece os atributos dos comandos.			*
 ****************************************************************
 */
char *
identatr (char *cp, FORMATO *fmtp, int cont)
{
	register int	num;
	char		*cpaux;

	while (!ESPACO (*cp)  &&  *cp != NOCHAR  &&  *cp != '.')
	{
		switch (*cp)
		{
		    case 'f':
			if (*++cp != 'b'  &&  *cp != 'i'  &&
				*cp != 'r')
				errcmd ();
			break;

		    case 'r':
			fmtp->f_tipo = ROMANO;
			cp++;
			break;

		    case 'i':
			fmtp->f_tipo = ITALICO;
			cp++;
			break;

		    case 'b':
			fmtp->f_tipo = NEGRITO;
			cp++;
			break;

		    case 'e':
			crtcol[cont].c_eflag = TRUE;
			cp++;
			break;


		    case 'w':
			if (*++cp != '(')
			{
				errcmd ();
				break;
			}
			cp++;
			num = lenum (&cp, &cpaux);

			if (cpaux == NOSTR)
			{
				errcmd ();
				break;
			}

			if (*cp++ != ')')
				errcmd ();

			if  (num > crtcol[cont].c_larg)
				crtcol[cont].c_larg = num;

			break;

		    default:
			num = lenum (&cp, &cpaux);

			if (cpaux == NOSTR)
				return (cp);

			if (num > crtcol[cont].c_esp)
				crtcol[cont].c_esp = num;

			break;

		}	/* switch */

	}	/* while */

	return (cp);

}	/* end identatr */

/*
 ****************************************************************
 *	Le numero.						*
 ****************************************************************
 */
int
lenum (char **cpp, char **cppaux)
{
	register char	*cp;
	const char 	*cpaux;
	register int	num;

	cp = *cpp;
	num = (int)strtol (cp, &cpaux, 10);
	if (cpaux == cp)
		*cppaux = NOSTR;
	*cpp = (char *)cpaux;

	return (num);

}	/* end lenum */

/*
 ****************************************************************
 *	Atualiza formato anterior.				*
 ****************************************************************
 */
void
incr (int campo, FORMATO *fmt)
{
	while (fmt->f_fmt == 's')
		fmt--;

	if (campo == SEP)
		fmt->f_sep++;
	else
		fmt->f_ncol++;

}	/* end incr */
