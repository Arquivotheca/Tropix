/*
 ****************************************************************
 *								*
 *			diff1.c					*
 *								*
 *	Compara dois arquivos					*
 *								*
 *	Versão	2.3.0, de 25.01.90				*
 *		3.0.0, de 24.04.97				*
 *								*
 *	Módulo: diff						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "diff.h"

/*
 ****************************************************************
 *	Definições locais deste arquivo				*
 ****************************************************************
 */
#define INCR(shift)	shift >= 9 ? (shift -= 9) : (shift += 7)

static char	*nextp;		/* Ponteiro para o próximo caracter */ 

entry int	*member;	/* Membros equivalentes de newfile */

entry CAND	*clist;		/* Início da lista de candidatos */ 

entry CAND	**klist,	/* Início e final dos ponteiros à clist */
		**lastklist;

entry int	*J;		/* Vetor resultado dos casamentos das linhas */

/*
 ****************************************************************
 *   Examina o arquivo e cria os ponteiros e dados das linhas	*
 ****************************************************************
 */
void
mkfiledata (char *textaddr, PLINE **plineaddr, int *filelineno)
{
	char		*cp;
	PLINE		*lp;
	int		i = 0;

	/*
	 *	Cria o vetor de PLINEs para o arquivo
	 *
	 *	Repare que as chamadas a diff_malloc devolvem endereços contíguos
	 */
	*plineaddr = lp =  diff_malloc (sizeof (PLINE));

	lp->l_text = NOSTR;
	lp->l_hash = 0;

	for (cp = textaddr; *cp; i++)
	{
		lp = diff_malloc (sizeof (PLINE));

		lp->l_text = cp;
		lp->l_hash = linehash (cp);	/* Repare que sempre > 0 */

		cp = nextp;
	}

	/*
	 *	Põe um indicador de final
	 */
	lp = diff_malloc (sizeof (PLINE));

	lp->l_text = NOSTR;
	lp->l_hash = 0;

	*filelineno = i; 

}	/* mkfiledata */

/*
 ****************************************************************
 *	Calcula o "hash" de uma linha				*
 ****************************************************************
 */
long
linehash (char *cp)
{
	unsigned int	shift;
	long		sum;
	int		space;
	long		c;

	sum   = 1;
	space = 0;

	/*
	 *	Cálculo do hash normal
	 */
	if (!bflag)
	{
		for (shift = 0; (c = *cp++) != '\n'; INCR (shift))
			sum += (c << shift);

		nextp = cp;
		return (sum);
	}

	/*
	 *	Cálculo do hash ignorando os brancos
	 */
	for (shift = 0; /* vazio */; /* vazio */)
	{
		switch (c = *cp++)
		{
		    case '\t':
		    case ' ':
			space++;
			continue;

		    default:
			if (space)
			{
				INCR (shift);
				space = 0;
			}

			sum += (c << shift);
			INCR (shift);
			continue;

		    case '\n':
			break;

		}	/* end switch */

		break;

	}	/* end for */

	nextp = cp;
	return (sum);

}	/* end linehash */

/*
 ****************************************************************
 *	Obtém os tamanhos do prefixo e sufixo			*
 ****************************************************************
 */
void
getprefixsufix (void)
{
	PLINE		*oldlp, *newlp, *lastlp;
	int		i;

	/*
	 *	Acha o comprimento do prefixo comum entre os arquivos
	 */
	oldlp = oldpline + 1; newlp = newpline + 1;

	for (i = 0; oldlp->l_text && newlp->l_text; oldlp++, newlp++)
	{
		if (oldlp->l_hash == newlp->l_hash)
			i++;
		else
			break;
	}

	prefixsz = i;

	/*
	 *	Acha o comprimento do sufixo comum entre os arquivos
	 */
	oldlp = oldpline + oldlineno;
	newlp = newpline + newlineno;

	for (i = 0; oldlp->l_text && newlp->l_text; oldlp--, newlp--)
	{
		if (oldlp->l_hash == newlp->l_hash)
			i++;
		else
			break;
	}

	sufixsz = i;

	/*
	 *	Não esquecer o caso da colisão do HASH
	 */
	if (sufixsz + prefixsz >= oldlineno && sufixsz + prefixsz >= newlineno)
		sufixsz = 0;

	/*
	 *	Obtém as linhas sem prefixo nem sufixo
	 */
	oldshortpline  = oldpline  + prefixsz;
	oldshortlineno = oldlineno - prefixsz - sufixsz;

	newshortpline  = newpline  + prefixsz;
	newshortlineno = newlineno - prefixsz - sufixsz;

	/*
	 *	Numera as linhas diferentes
	 */
	lastlp = oldpline + oldlineno - sufixsz + 1;

	for (i = 0, oldlp = oldshortpline; oldlp < lastlp; oldlp++, i++) 
		oldlp->l_serial = i;

	lastlp = newpline + newlineno - sufixsz + 1;

	for (i = 0, newlp = newshortpline; newlp < lastlp; newlp++, i++) 
		newlp->l_serial = i;

}	/* end getprefixsufix */

/*
 ****************************************************************
 *	Ordena o arquivo pelo "hash"				*
 ****************************************************************
 */
void
sort (PLINE *shortpline, int shortlineno, PLINE ***sortvec)
{
	PLINE		*lp, *lastlp, **vp;

	*sortvec = vp = diff_malloc (shortlineno * sizeof (PLINE *));

	lastlp = shortpline + shortlineno + 1;

	for (lp = shortpline + 1; lp < lastlp; lp++)
		*vp++ = lp;

	if (shortlineno > 1)
	{
		qtsort
		(	(void **)*sortvec,
			shortlineno,
			(int (*) (const void *, const void *))sortcompare
		);
	}

}	/* end sort */

/*
 ****************************************************************
 *	Rotina de comparação 					*
 ****************************************************************
 */
int
sortcompare (const PLINE *lp1, const PLINE *lp2)
{
	/*
	 *	-1 => Não troca
	 */
	if (lp1->l_hash < lp2->l_hash)
		return (-1);

	if (lp1->l_hash > lp2->l_hash)
		return (+1);

	if (lp1 < lp2)
		return (-1);

	return (+1);

}	/* end sortcompare */

/*
 ****************************************************************
 *	Procura linhas iguais dos dois arquivos			*
 ****************************************************************
 */
void
correspondence (void)
{
	PLINE		**oldlpp, **newlpp;
	PLINE		*oldlp, *newlp;
	PLINE		**lastold, **lastnew;
	int		*mp;

	/*
	 *	Procura linhas iguais
	 */
	oldlpp = oldsortvec; 	lastold = oldlpp + oldshortlineno;
	newlpp = newsortvec; 	lastnew = newlpp + newshortlineno;

	while (oldlpp < lastold && newlpp < lastnew)
	{
		oldlp = *oldlpp; newlp = *newlpp;

		if   (oldlp->l_hash <  newlp->l_hash)
			{ oldlp->l_hash = 0; oldlpp++; }
		elif (oldlp->l_hash == newlp->l_hash)
			{ oldlp->l_hash = newlpp-newsortvec+1; oldlpp++; }
		else
			newlpp++;
	}

	/*
	 *	Indica que as linhas restantes não tem correspondentes
	 */
	while (oldlpp < lastold)
		{ (*oldlpp)->l_hash = 0; oldlpp++; }


	/*
	 *	Resume "newnm" no vetor de inteiros "member".
	 */
	member = mp = diff_malloc ((newshortlineno + 2) * sizeof (int));

	lastnew--; 	mp++;

	for (newlpp = newsortvec; newlpp < lastnew; newlpp++)
	{
		*mp++ = - newlpp[0]->l_serial;

		while (newlpp[1]->l_hash == newlpp[0]->l_hash)
		{
			*mp++ = newlpp[1]->l_serial;

			if (++newlpp >= lastnew)
				{ newlpp--; break; }
		}
	}

	if (newlpp <= lastnew)
		*mp++ = - newlpp[0]->l_serial;

	*mp = -1;

}	/* end correspondence */

/*
 ****************************************************************
 *	Procura as subseqüências comuns mais longas possíveis	*
 ****************************************************************
 */
void
stonehenge (void)
{
	int		oldline, newline;
	int		j, i;
	CAND		*oldc;
	CAND		**l, **oldl, **k;
	CAND		*tempc;
	CAND		***lindex;

	klist = k = diff_malloc ((oldshortlineno + 2) * sizeof (CAND *));

	lindex = diff_malloc ((newshortlineno + 2) * sizeof (CAND **));

	clist = k[0] = newcand (0, 0, (CAND *)NULL);

	/*
	 *	Percorre "oldfile"
	 */
	for (oldline = 1; oldline <= oldshortlineno; oldline++)
	{
		if ((j = oldshortpline[oldline].l_hash) == 0)
			continue;

		newline = -member[j];	/* No. de série em newfile */

		oldl = klist;
		oldc = clist;

		/*
		 *	Percorre os membros da classe de equivalencia
		 */
		do
		{
			if (newline <= oldc->c_newlineno)
				continue;

			/*
			 *	Procura a "newline" na "klist"
			 */
			if   (l = lindex[newline])
			{
				/*
				 *	Achou
				 */
				if (l != oldl + 1)
					oldc = l[-1];
				continue;
			}
			elif (k[0]->c_newlineno < newline)
			{
				/*
				 *	Não achou, e está fora
				 */
				l = k + 1;
				if (l != oldl + 1)
					oldc = k[0]; /* ==  klist[l-1] */

				l[0] = newcand (oldline, newline, oldc);
				lindex[newline] = l;
				k++;
				break;
			}
			else
			{
				/*
				 *	Não achou, mas está dentro
				 */
				i = newline + 1;
				while ((l = lindex[i]) == (CAND **)NULL)
					i++;
#define	CHECK
#ifdef	CHECK
				if (l > k)
				{
					fprintf
					(	stderr,
						"%s: stonehenge: ERRO 1\n",
						pgname
					);
				}
#endif	CHECK
				if (l != oldl + 1)
					oldc = l[-1];
				if (l[0]->c_newlineno <= newline)
					continue;
				tempc = l[0];
				lindex[i] = (CAND **)NULL;
#ifdef	CHECK
				if (i != tempc->c_newlineno)
				{
					fprintf
					(	stderr,
						"%s: stonehenge: ERRO 2\n",
						pgname
					);
				}
#endif	CHECK
				l[0] = newcand (oldline, newline, oldc);
				lindex[newline] = l;

				oldc = tempc;
				oldl = l;
			}
		}
		while ((newline = member[++j]) > 0);

	}	/* end for (oldline = 1; oldline <= n; oldline++) */

	lastklist = k;

}	/* end stonehenge */

/*
 ****************************************************************
 * Aloca memória para um novo candidato a  par casado de linhas	*
 ****************************************************************
 */
CAND *
newcand (int oldline, int newline, CAND *pred)
{
	CAND		*qp;

	/*
	 *	As várias chamadas a "diff_malloc" fornecem endereços contíguos
	 */
	qp = diff_malloc (sizeof (CAND));

	qp->c_oldlineno = oldline;
	qp->c_newlineno = newline;
	qp->c_pred      = pred;

	return (qp);

}	/* end newcand */

/*
 ****************************************************************
 *	Recompõe com o prefixo e sufixo 			*
 ****************************************************************
 */
void
compose (void)
{
	int		i;
	CAND		*q;

	/*
	 *	Aloca
	 */
	J = diff_malloc ((oldlineno + 2) * sizeof (int));

	/*
	 *	Coloca a correspondencia no prefixo
	 */
	for (i = 0; i <= prefixsz; i++)
		J[i] = i;

	/*
	 *	Coloca a correspondencia no sufixo
	 */
	for (i = oldlineno - sufixsz + 1; i <= oldlineno; i++)
		J[i] = i + newlineno - oldlineno;

	/*
	 *	Coloca os pares casados da "clist"
	 */
	for (q = *lastklist; q->c_newlineno != 0; q = q->c_pred)
		J[q->c_oldlineno + prefixsz] = q->c_newlineno + prefixsz;

}	/* end compose */

/*
 ****************************************************************
 *	Verifica que se o "hash" teve colisão			*
 ****************************************************************
 */
void
check (void)
{
	int		j;
	char		*oldcp, *newcp;
	PLINE		*oldlp;
	int		*jp;
	char		c, d;

	jp = J + 1;

	/*
	 *	Percorre as linhas do arquivo
	 */
	for (oldlp = oldpline+1; oldlp->l_text; oldlp++, jp++)
	{
		if ((j = *jp) == 0)
			continue;

		oldcp = oldlp->l_text;
		newcp = newpline[j].l_text;

		/*
		 *	Examina uma linha
		 */
		for (EVER)
		{
			c = *oldcp++;
			d = *newcp++;

			if (bflag && isspace (c) && isspace (d))
			{
				do
				{
					if (c == '\n')
						break;
				}
				while (isspace (c = *oldcp++));

				do
				{
					if (d == '\n')
						break;
				}
				while (isspace (d = *newcp++));
			}

			if (c != d)
			{
				fprintf
				(	stderr,
					"%s: Advertência: colisão do HASH\n",
					pgname
				);
				*jp = 0;
				break;
			}

			if (c == '\n')
				break;
		}

	}	/* end for */

}	/* end check */
