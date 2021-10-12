/*
 ****************************************************************
 *								*
 *			procarq.c				*
 *								*
 *	Trata os arquivos.					*
 *								*
 *	Versão	2.0.0, de 11.05.86				*
 *		3.0.0, de 07.07.97				*
 *								*
 *	Rotinas:						*
 *		procarq		lelinha				*
 *		trttxt		trtcmd				*
 *		errcmd						*
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
#include "../h/tbl.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis estáticas					*
 ****************************************************************
 */
static	BOOL	coment = FALSE;

/*
 ****************************************************************
 *	Encaminha as linhas lidas.				*
 ****************************************************************
 */
void
procarq (FILE *fp)
{
	/*
	 *	Le o arquivo, linha a linha, ate' o fim.
	 */
	linhalida = 0;
	while (lelinha (fp) != NOSTR)
	{
		if (coment)
		{
			if (bufin[0] == '.'  &&
					bufin[1] == 'e'  &&  bufin[2] == 'c')
				coment = FALSE;

			continue;
		}

		if (lst)
		{
			/*
			 *	A linha faz parte de uma lista.
			 */
			lista ();
			if (ul > 0)
				ul--;
			continue;
		}

		if (lit)
		{
			/*
			 *	A linha deve ser copiada literalmente.
			 */
			literal ();
			if (ul > 0)
				ul--;
			continue;
		}

		if (bufin[0] == '.')
		{
			/*
			 *	Linha de comando.
			 */
			if (tbl == 0  ||  tbl == 4)
				trtcmd ();
			else
				errcmd ();
		}
		else
		{
			switch (tbl)
			{
			    case 0:
				break;

			    case 1:
			    case 2:
				trtopcao ();
				if (ul > 0)
					ul--;
				continue;

			    case 3:
				trtfmt ();
				if (ul > 0)
					ul--;
				continue;

			    case 4:
				trtdata ();
				if (ul > 0)
					ul--;
				continue;
			}

			if (bufin[0] == ' ')
			{
				/*
				 *	Deve haver uma quebra na ultima linha.
				 */
				trtlin (QUEBRA); 
			}

			if (ce > 0)
			{
				/*
				 *	A linha deve ser centrada.
				 */
				centra ();
				if (ul > 0)
					ul--;
				continue;
			}

			/*
			 *	A linha e' uma linha comum de texto.
			 */
			trttxt ();
		}

		if (ul > 0)
			ul--;
	}

	/*
	 *	Imprime a ultima linha do arquivo.
 	 */
	trtlin (QUEBRA);

}	/* end procarq */

/*
 ****************************************************************
 *	Le a proxima linha do arquivo.				*
 ****************************************************************
 */
char *
lelinha (FILE *fp)
{
	linhalida++;

	return (fngets (bufin, BLSZ, fp));

}	/* end lelinha */

/*
 ****************************************************************
 *	Trata uma linha de texto.				*
 ****************************************************************
 */
void
trttxt (void)
{
	register char	*cp;

	cp = bufin;

	/*
	 *	Procura o primeiro caracter nao separador da linha.
	 */
	while (ESPACO (*cp))
		cp++;

	/*
	 *	Percorre a linha.
	 */
	while (*cp != NOCHAR)
	{
		/*
		 *	Encaminha cada inicio de palavra `a formatacao.
		 */
		cp = poepalvr (cp, CADEIA);

		while (ESPACO (*cp))
			cp++;
	}

}	/* end trttxt */

/*
 ****************************************************************
 *	Trata uma linha de comando.				*
 ****************************************************************
 */
void
trtcmd (void)
{
	register long	cmd;
	register char	*cp;
	static	 int	stin,
			stip;

	cp = bufin;

	if (*++cp == NOCHAR)
	{
		trtlin (QUEBRA);
		errcmd ();
		return;
	}

	/*
	 *	Identifica o comando.
	 */
	cmd = (long)(*cp++) << 8;
	if (*cp != NOCHAR  &&  !ESPACO (*cp))
		cmd |= (long)*cp++;

	if (!ESPACO (*cp)  &&  *cp != NOCHAR)
	{
		trtlin (QUEBRA);
		errcmd ();
		return;
	}

	while (ESPACO (*cp))
		cp++;

	if (*cp == NOCHAR)
		cp = NOSTR;

	/*
	 *	Trata o comando.
	 */
	switch (cmd)
	{
	    case BC:
#if (0)	/*************************************/
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
#endif	/*************************************/
		coment = TRUE;
		break;

	    case PL:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (PL, cp);
		break;

	    case LN:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (LN, cp);
		break;

	    case M1:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (M1, cp);
		break;

	    case M2:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (M2, cp);
		break;

	    case M3:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (M3, cp);
		break;

	    case M4:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (M4, cp);
		break;

	    case LS:
		trtlin (QUEBRA);
		altvar (LS, cp);
		break;

	    case LL:
		trtlin (QUEBRA);
		altvar (LL, cp);
		break;

	    case PO:
		trtlin (QUEBRA);
		altvar (PO, cp);
		break;

	    case IN:
		trtlin (QUEBRA);
		altvar (IN, cp);
		break;

	    case BP:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}
		altvar (BP, cp);
		break;

	    case PN:
		trtlin (QUEBRA);
		altvar (PN, cp);
		break;

	    case CE:
		trtlin (QUEBRA);
		altvar (CE, cp);
		break;

	    case UL:
		altvar (UL, cp);
		ul++;
		break;

	    case SP:
		trtlin (QUEBRA);
		altvar (SP, cp);
		break;


	    case NR:
		trtlin (QUEBRA);
		altvar (NR, cp);
		break;

	    case SH:
		trtlin (QUEBRA);
		secnum (cp);
		break;

	    case HE:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (HE, cp);
		break;

	    case EH:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (EH, cp);
		break;

	    case OH:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (OH, cp);
		break;

	    case FO:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (FO, cp);
		break;

	    case EF:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (EF, cp);
		break;

	    case OF:
		trtlin (QUEBRA);
		if (cp == NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		trttit (OF, cp);
		break;

	    case TL:
		trtlin (QUEBRA);
		if (cp == NOSTR)
		{
			errcmd ();
			break;
		}
		if (codiftit (cp, tl, (TITULO *)NULL))
			poetit (tl);
		break;

	    case TP:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}
		titpag ();
		break;

	    case BR:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		break;

	    case NF:
	    case LI:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		lit = TRUE;
		break;

	    case PP:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		parag (PP);
		break;

	    case LP:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		parag (LP);
		break;

	    case IP:
		trtlin (QUEBRA);
		iparag (IP, cp);
		break;

	    case IT:
		trtlin (QUEBRA);
		iparag (IT, cp);
		break;

	    case EP:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		ip = pp = 0;
		attamlin ();
		atnbrancos ();
		break;

	    case AQ:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		iniciocit ();
		break;

	    case FQ:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		if (cit == 0)
			errcmd ();
		else
			fimcit ();
		break;

	    case AL:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			break;
		}
		lst = TRUE;
		break;

	    case AB:
		trtlin (QUEBRA);
		if (tembloco)
		{
			errcmd ();
			break;
		}

		lsblc = ls;
		stin = in;
		stip = ip;
		ls = 1;
		in = ip = 0;
		attamlin ();
		atnbrancos ();

		trtblc (cp);
		break;

	    case FB:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  !tembloco)
		{
			errcmd ();
			break;
		}

		if (temflut)
			fimflut ();
		else
			imprfixo ();

		ls = lsblc;
		in = stin;
		ip = stip;
		attamlin ();
		atnbrancos ();

		if (fimbloco)
			fimbloco = FALSE;
		break;

	    case AT:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}

		blfixo = blccorr = criablc ();
		break;

	    case FT:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  !tembloco)
		{
			errcmd ();
			break;
		}

		imprfixo ();

		if (fimbloco)
			fimbloco = FALSE;
		break;

	    case AF:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  tembloco)
		{
			errcmd ();
			break;
		}

		lsblc = ls;
		ls = 1;
		stin = in;
		stip = ip;
		in = ip = 0;
		attamlin ();
		atnbrancos ();

		trtrod ();	
		break;

	    case FF:
		trtlin (QUEBRA);
		if (cp != NOSTR  ||  !tembloco)
		{
			errcmd ();
			break;
		}

		fimrod ();

		ls = lsblc;
		in = stin;
		ip = stip;
		attamlin ();
		atnbrancos ();

		if (fimbloco)
			fimbloco = FALSE;
		break;

	    case I:
		trtcarac (I, cp);
		break;

	    case R:
		trtcarac (R, cp);
		break;

	    case B:
		trtcarac (B, cp);
		break;

	    case WO:
		if (cp == NOSTR)
		{
			errcmd ();
			break;
		}
		trtwo (cp);
		break;

	    case TS:
		trtlin (QUEBRA);
		if (cp != NOSTR)
		{
			errcmd ();
			return;
		}
		tbl = 1;
		inictbl ();
		break;

	    case TQ:
		if (cp != NOSTR  ||  tbl == 0)
		{
			errcmd ();
			return;
		}

		if (contfmt != maxfmt)
		{
			errcmd ();
			contfmt = maxfmt;
		}
		inicfmt = ++contfmt;

		tbl = 3;
		break;

	    case TE:
		if (cp != NOSTR  ||  tbl == 0)
		{
			errcmd ();
			return;
		}
		mttbl ();
		tbl = 0;
		break;

	    default:
		trtlin (QUEBRA);
		errcmd ();
	}

}	/* end trtcmd */

/*
 ****************************************************************
 *	Mostra a linha com erro de comando.			*
 ****************************************************************
 */
void
errcmd (void)
{
	fprintf
	(	stderr,
		"%s: Erro de comando na linha %d de \"%s\"\n",
		pgname, linhalida, fn
	);
	fprintf (stderr, "%s\n", bufin);

}	/* end errcmd */
