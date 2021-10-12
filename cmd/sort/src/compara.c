/*
 ****************************************************************
 *								*
 *			compara.c				*
 *								*
 *	Funções de comparação					*
 *								*
 *	Versão	1.0.0, de 09.03.87				*
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
 *	Rotina de comparação ascendente com chave tipo FOLD	*
 ****************************************************************
 */
int
cmp_fold_up (const char *xp, const char *yp)
{
	return (strttcmp (xp, yp, cmpfoldtb, cmpafoldtb));

}	/* end cmp_fold_up */

/*
 ****************************************************************
 *	Rotina de comparação descendente com chave tipo FOLD	*
 ****************************************************************
 */
int
cmp_fold_down (const char *xp, const char *yp)
{
	return (strttcmp (yp, xp, cmpfoldtb, cmpafoldtb));

}	/* end cmp_fold_down */

/*
 ****************************************************************
 *	Rotina de comparação ascendente com chave tipo DIC	*
 ****************************************************************
 */
int
cmp_dic_up (register const char *xp, register const char *yp)
{
	register int	c;
	register char	*cp;
	char		a[256], b[256];

	/*
	 *	Transforma o primeiro argumento
	 */
	for (cp = a; (c = *xp++) != '\0'; /* no teste */)
	{
		if (cmpdictb[c] != ' ')
			*cp++ = c;
	}

	*cp = '\0';

	/*
	 *	Transforma o segundo argumento
	 */
	for (cp = b; (c = *yp++) != '\0'; /* no teste */)
	{
		if (cmpdictb[c] != ' ')
			*cp++ = c;
	}

	*cp = '\0';

	/*
	 *	Compara as cadeias transformadas
	 */
	return (strttcmp (a, b, cmpfoldtb, cmpafoldtb));

}	/* end cmp_dic_up */

/*
 ****************************************************************
 *	Rotina de comparação descendente com chave tipo DIC	*
 ****************************************************************
 */
int
cmp_dic_down (register const char *xp, register const char *yp)
{
	register int	c;
	register char	*cp;
	char		a[256], b[256];

	/*
	 *	Transforma o primeiro argumento
	 */
	for (cp = a; (c = *xp++) != '\0'; /* no teste */)
	{
		if (cmpdictb[c] != ' ')
			*cp++ = c;
	}

	*cp = '\0';

	/*
	 *	Transforma o segundo argumento
	 */
	for (cp = b; (c = *yp++) != '\0'; /* no teste */)
	{
		if (cmpdictb[c] != ' ')
			*cp++ = c;
	}

	*cp = '\0';

	/*
	 *	Compara as cadeias transformadas
	 */
	return (strttcmp (b, a, cmpfoldtb, cmpafoldtb));

}	/* end cmp_dic_down */

/*
 ****************************************************************
 *	Rotina de comparação quando chaves foram definidas	*
 ****************************************************************
 */
int
cmp_key (const char *xp, const char *yp)
{
	register const char	*ix, *iy;
	register char		*fx = NOSTR, *fy = NOSTR;
	register int		j;
	KEY			*qp, *qe;
	int			cmp = 0, len;
	char			delimx = '\0', delimy = '\0';

	/*
	 *	Se for a mesma cadeia, é igual
	 */
	if (xp == yp)
		return (0);

	/*
	 *	Examina cada uma das chaves (até a comparação dar desigual)
	 */
	for (qp = &key_vec[0], qe = &key_vec[numkeys]; qp < qe; qp++)
	{
		ix = xp;
		iy = yp;

		/*
		 *	Se "index" == 0, a chave é o registro todo
		 */
		if (qp->k_index > 0)
		{
			fx = (char *)ix - 1;
			fy = (char *)iy - 1;

			/*
			 *	Rasteja até a chave pedida ...
			 */
			for (j = 0; j < qp->k_index; j++)
			{
				ix = ++fx;
				iy = ++fy;
	
				/*
				 *	Se o delim é branco pula todos os brancos
				 */
				if (delim == ' ')
				{
					while (*fx != ' ' && *fx != '\t' && *fx != '\0')
						fx++;

					while (*fx == ' ' || *fx == '\t') 
						fx++;

					if (*fx != '\0')
						fx--;

					while (*fy != ' ' && *fy != '\t' && *fy != '\0')
						fy++;

					while (*fy == ' ' || *fy == '\t') 
						fy++;

					if (*fy != '\0')
						fy--;
				}
				else
				{
					while (*fx != delim && *fx != '\0')
						fx++;

					while (*fy != delim && *fy != '\0')
						fy++;
				}
#undef	DEBUG
#ifdef	DEBUG
				if (vflag)
				{
					fprintf
					(	stderr,
						"Início e final das cadeias: "
						"xp = \"%c\" \"%c\"   "
						"yp = \"%c\" \"%c\"\n",
						*ix, *fx, *iy, *fy
					);
				}
#endif	DEBUG
			}	/* end for */
	
			/*
			 *	Acerta o final das chaves
			 */
			if (*fx == '\0')
				fx = NOSTR;
			else
				{ delimx = *fx; *fx = '\0'; }
	
			if (*fy == '\0')
				fy = NOSTR;
			else
				{ delimy = *fy; *fy = '\0'; }

		}	/* end if */

		/*
		 *	Se foi dado o início do campo, ...
		 */
		if (qp->k_begin > 0)
		{
			ix += qp->k_begin - 1;
			iy += qp->k_begin - 1;
		}

#define	DEBUG
#ifdef	DEBUG
		if (vflag)
		{
			fprintf
			(	stderr,
				"Comparando cadeias \"%s\" e \"%s\"\n",
				ix, iy
			);
		}
#endif	DEBUG

		/*
		 *	Verifica se foi dado o tamanho do campo
		 */
		if ((len = qp->k_size) > 0)
		{
			switch (qp->k_code)
			{
			    case 'f':
				cmp = strnttcmp (ix, iy, len, cmpfoldtb, cmpafoldtb);
				break;
				
			    case 'd':
				cmp = strntcmp (ix, iy, len, cmpdictb);
				break;
				
			    case 'i':
				cmp = strntcmp (ix, iy, len, cmpisotb);
				break;
				
			    case 'a':
				cmp = strncmp (ix, iy, len);
				break;

			    default :
				fprintf
				(	stderr,
					"Tipo de ordenação inválido: \"%c\"\n",
					qp->k_code
				);
				exit (1);
			}
		}
		else
		{
			switch (qp->k_code)
			{
			    case 'f':
				cmp = strttcmp (ix, iy, cmpfoldtb, cmpafoldtb);
				break;
				
			    case 'd':
				cmp = strtcmp (ix, iy, cmpdictb);
				break;
				
			    case 'i':
				cmp = strtcmp (ix, iy, cmpisotb);
				break;

			    case 'a':
				cmp = strcmp (ix, iy);
				break;

			    default :
				fprintf
				(	stderr,
					"Tipo de ordenação inválido: \"%c\"\n",
					qp->k_code
				);
				exit (1);
			}
		}

		/*
		 *	Se a comparação for reversa, ...
		 */
		if (qp->k_up_down == 'd')
			cmp = -cmp;

		/*
		 *	Restaura o caractere do final das cadeias
		 */
		if (qp->k_index > 0)
		{
			if (fx != NOSTR)
				*fx = delimx;
	
			if (fy != NOSTR)
				*fy = delimy;
		}
	
		/*
		 *	Se deu desigual, terminou a comparação de chaves
		 */
		if (cmp != 0)
		{
			if (rflag)
				return (-cmp);
			else
				return (cmp);
		}

	}	/* end for (pelas várias chaves) */

	return (0);

}	/* end cmp_key */
