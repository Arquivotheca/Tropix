/*
 ****************************************************************
 *								*
 *			clist.c					*
 *								*
 *	Gerencia das listas de caracteres dos terminais		*
 *								*
 *	Versão	3.0.0, de 14.12.94				*
 *		4.8.0, de 21.12.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/tty.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry LOCK	cfreelock;	/* Lock da Lista livre */
entry CBLK	*cfreelist;	/* Comeco da lista Livre */

/*
 ****************************************************************
 *	Transferência de uma CLIST para uma área		*
 ****************************************************************
 */
int
cread (CHEAD *q, void *area, int count, int cpflag)
{
	int		bcc;
	CBLK		*cp;
	void		*origarea;

	/*
	 *	cpflag:
	 *	    SS => leitura para uma area do SUPERVISOR
	 *	    SU => leitura para uma area do USUARIO
	 */
	origarea = area;

	SPINLOCK (&q->c_lock);

	count = MIN (count, q->c_count);

	while (count > 0)
	{
		/*
		 *	Calcula o no. de caracteres deste CBLK
		 */
		cp = (CBLK *)(((int)q->c_cf) & ~CLMASK);
		bcc = (int)(cp+1) - (int)q->c_cf;
		bcc = MIN (count, bcc);

		if (unimove (area, q->c_cf, bcc, cpflag) < 0)
			{ SPINFREE (&q->c_lock); u.u_error = EFAULT; return (-1); }

		area  += bcc;
		count -= bcc;

		/*
		 *	Verifica o novo Estado da CLIST
		 */
		if ((q->c_count -= bcc) <= 0)
		{
			/*
			 *	A CLIST ficou vazia
			 */
			q->c_cf = q->c_cl = NOSTR;
			q->c_count = 0;
			goto devolve;
		}
		elif (((int)(q->c_cf += bcc) & CLMASK) == 0)
		{
			/*
			 *	Este CBLK ficou vazio
			 */
			q->c_cf = cp->c_next->c_info;
		    devolve:
			SPINLOCK (&cfreelock);
			cp->c_next = cfreelist;
			cfreelist = cp;
			SPINFREE (&cfreelock);
		}

	}	/* end while (count > 0) */

	SPINFREE (&q->c_lock);

	/*
	 *	Retorna o No. de Caracteres lidos da CLIST
	 */
	return (area - origarea);

}	/* end cread */

/*
 ****************************************************************
 *	Transferência de uma área para uma CLIST 		*
 ****************************************************************
 */
int
cwrite (CHEAD *q, void *area, int count, int cpflag)
{
	int		bcc;
	CBLK		*cp;
	void		*origarea;

	/*
	 *	cpflag:
	 *	    SS => Escrita de uma area do SUPERVISOR para uma CLIST
	 *	    US => leitura de uma area do USUARIO    para uma CLIST
	 */
	origarea = area;

	SPINLOCK (&q->c_lock);

	while (count > 0)
	{
		if (q->c_count <= 0)
		{
			/*
			 *	A CLIST está vazia
			 */
			SPINLOCK (&cfreelock);

			if ((cp = cfreelist) == NOCBLK)
			{
			    empty:
				SPINFREE (&cfreelock);
				SPINFREE (&q->c_lock);
				printf ("cwrite: Não há mais CBLKs!\n");
				return (-1);
			}

			cfreelist = cp->c_next;
			SPINFREE (&cfreelock);

			cp->c_next = NOCBLK;

			q->c_cf = q->c_cl = cp->c_info;
			q->c_count = 0;
		}
		elif ((((int)q->c_cl) & CLMASK) == 0)
		{
			/*
			 *	Este CBLK está cheio
			 */
			cp = (CBLK *)q->c_cl - 1;

			SPINLOCK (&cfreelock);

			if ((cp->c_next = cfreelist) == NOCBLK)
				goto empty;

			cp = cp->c_next;
			cfreelist = cp->c_next;
			SPINFREE (&cfreelock);

			cp->c_next = NOCBLK;
			q->c_cl = cp->c_info;
		}
		else
		{
			/*
			 *	Este CBLK tem lugar
			 */
			cp = (CBLK *)((int)q->c_cl & ~CLMASK);
		}
			
		bcc = (int)(cp+1) - (int)q->c_cl;
		bcc = MIN (count, bcc);

		if (unimove (q->c_cl, area, bcc, cpflag) < 0)
			{ SPINFREE (&q->c_lock); u.u_error = EFAULT; return (-1); }

		area  += bcc;
		count -= bcc;

		q->c_count += bcc;
		q->c_cl    += bcc;

	}	/* end while (count > 0) */

	SPINFREE (&q->c_lock);

	/*
	 *	Retorna o No. de Caracteres inseridos na CLIST
	 */
	return (area-origarea);

}	/* end cwrite */

/*
 ****************************************************************
 *	Obtem um caracter de uma CLIST				*
 ****************************************************************
 */
int
cget (CHEAD *q)
{
	CBLK		*cp;
	int		c;

	/*
	 *	Esta função NÃO tranca a fila, deve ser feito pela função que a chama
	 *
	 *	Retorna < 0 se a lista está vazia
	 */
   /***	SPINLOCK (&q->c_lock);	***/

	if (q->c_count <= 0)
	{
		/*
		 *	CLIST vazia
		 */
		q->c_count = 0;
		q->c_cf = q->c_cl = NOSTR;

	   /***	SPINFREE (&q->c_lock);	***/
		return (-1);
	}

	/*
	 *	A CLIST tem pelo menos um caracter
	 */
	c = *q->c_cf++;

	if (--q->c_count <= 0)
	{
		/*
		 *	A CLIST esvaziou
		 */
		cp = (CBLK *)((int)(q->c_cf - 1) & ~CLMASK);
		q->c_cf = q->c_cl = NOSTR;
	   /***	SPINFREE (&q->c_lock);	***/

		goto devolve;
	}
	elif (((int)q->c_cf & CLMASK) == 0)
	{
		/*
		 *	Este CBLK esvaziou
		 */
		cp = (CBLK *)q->c_cf - 1;
		q->c_cf = cp->c_next->c_info;
	   /***	SPINFREE (&q->c_lock);	***/

	    devolve:
		SPINLOCK (&cfreelock);
		cp->c_next = cfreelist;
		cfreelist = cp;
		SPINFREE (&cfreelock);
	}
	else
	{
		/*
		 *	Nem a CLIST nem o CBLK esvaziou
		 */
	   /***	SPINFREE (&q->c_lock);	***/
	}

	return (c);

}	/* end cget */

/*
 ****************************************************************
 *	Coloca um caracter em uma CLIST				*
 ****************************************************************
 */
int
cput (int c, CHEAD *q)
{
	CBLK		*cp;
	char		*p;

	/*
	 *	Esta função NÃO tranca a fila, deve ser feito pela função que a chama
	 *
	 *	Retorna 0 se colocou o caracter na CLIST, e (-1) se não há mais lugar
	 */
   /***	SPINLOCK (&q->c_lock);	***/

	if ((p = q->c_cl) == NOSTR || q->c_count <= 0 )
	{
		/*
		 *	A CLIST está vazia
		 */
		SPINLOCK (&cfreelock);

		if ((cp = cfreelist) == NOCBLK)
		{
			/*
			 *	Não há mais Blocos!
			 */
		    empty:
			SPINFREE (&cfreelock);
		   /***	SPINFREE (&q->c_lock);	***/
			printf ("cput: Não há mais CBLKs!\n");
			return (-1);
		}
		cfreelist = cp->c_next;
		SPINFREE (&cfreelock);

		cp->c_next = NOCBLK;

		q->c_cf = p = cp->c_info;
		q->c_count = 0;

	}
	elif (((int)p & CLMASK) == 0)
	{
		/*
		 *	Este Bloco está Cheio
		 */
		cp = (CBLK *)p - 1;

		SPINLOCK (&cfreelock);

		if ((cp->c_next = cfreelist) == NOCBLK)
			goto empty;

		cp = cp->c_next;
		cfreelist = cp->c_next;
		SPINFREE (&cfreelock);

		cp->c_next = NOCBLK;
		p = cp->c_info;
	}

	/*
	 *	Coloca o caracter no CBLK
	 */
	*p++ = c;
	q->c_count++;
	q->c_cl = p;

   /***	SPINFREE (&q->c_lock);	***/

	return (0);

}	/* end cputc */

/*
 ****************************************************************
 *	Remove um caracter do final de uma CLIST		*
 ****************************************************************
 */
int
chop (CHEAD *q)
{
	CBLK		*cp;
	char		*p;
	int		c;

	/*
	 *	Se o caracter for o delimitador, não remove.
	 *	Retorna o caracter que removeu, e (-1) se não removeu
	 */
	SPINLOCK (&q->c_lock);

	if ((p = q->c_cl) == NOSTR || (c = *(--p)) == DELIM)
		{ SPINFREE (&q->c_lock); return (-1); }

	/*
	 *	Remove o Caracter
	 */
	q->c_cl = p;
	p -= sizeof (CBLK *);

	/*
	 *	Verifica o novo estado do CBLK
	 */
	if (--q->c_count <= 0)
	{
		/*
		 *	A CLIST ficou vazia
		 */
		cp = (CBLK *)((int)p & ~CLMASK);
		q->c_cf = q->c_cl = NOSTR;

		SPINLOCK (&cfreelock);
		cp->c_next = cfreelist;
		cfreelist = cp;
		SPINFREE (&cfreelock);
	}
	elif (((int)p & CLMASK) == 0)
	{
		/*
		 *	O ultimo CBLK da CLIST ficou vazio
		 */
		cp = (CBLK *)p;

		SPINLOCK (&cfreelock);
		cp->c_next = cfreelist;
		cfreelist = cp;
		SPINFREE (&cfreelock);

		cp = (CBLK *)((int)q->c_cf & ~CLMASK);

		while (cp->c_next != (CBLK *)p)
			cp = cp->c_next;

		cp->c_next = NOCBLK;
		q->c_cl = &cp->c_info[CLBSZ];
	}

	SPINFREE (&q->c_lock);

	return (c);

}	/* end chop */

/*
 ****************************************************************
 *	Inicializa a clist					*
 ****************************************************************
 */
void
cinit (void)
{
	CBLK		*cp;

	/*
	 *	Coloca todos os CBLKs na cfreelist. Supõe que tanto "cfree"
	 *	como "endcfree" são múltiplos de sizeof (CLBK)
	 */
	if ((int)scb.y_clist & CLMASK || (int)scb.y_endclist & CLMASK)
		panic ("cinit: Endereço inválido\n");

	for (cp = scb.y_clist; cp < scb.y_endclist; cp++)
		{ cp->c_next = cfreelist; cfreelist = cp; }

}	/* end cinit */
