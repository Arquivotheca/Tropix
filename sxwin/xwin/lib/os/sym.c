/*
 ****************************************************************
 *								*
 *			sym.c					*
 *								*
 *	Depuração com Tabela de Símbolos			*
 *								*
 *	Versão	1.0.0, de 04.03.97				*
 *		1.0.1, de 30.03.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <a.out.h>
#include <fcntl.h>
#include <unistd.h>

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#define	NOSYM		(SYM *)0
#define	NOFRAMEB	(FRAMEB *)0

typedef	struct frameb	FRAMEB;

struct frameb
{
	FRAMEB	*fp;
	void	*ret;
};

static	SYM		*symtb,
			*endsymtb;

SYM			*getsyment (void *);
int			symcmp (const void *, const void *);
void			print_symtb (void);
extern	FRAMEB		*get_fp (void);

/*
 ****************************************************************
 *	Imprime o histórico de chamadas				*
 ****************************************************************
 */
int
print_calls (FILE *fp)
{
	FRAMEB	*p;
	SYM	*sp;

	if (symtb == NOSYM)
		return (-1);

	fprintf (fp, "CALLs = ");

	for (p = get_fp (); p != NOFRAMEB; p = p->fp)
	{
		if ((sp = getsyment (p->ret)) == NOSYM)
			fprintf (fp, "%P  ", p->ret);
		else
			fprintf (fp, "%s  ", sp->s_name);

		fflush (fp);
	}

	putc ('\n', fp);
	fflush (fp);

	return (0);

}	/* end print_calls */

/*
 ****************************************************************
 *	Obtém o nome de uma rotina				*
 ****************************************************************
 */
char *
get_symbol_name (void *addr)
{
	SYM	*sp;

	if (symtb == NOSYM)
		return ("");

	sp = getsyment (addr);
	return (sp == NOSYM ? "" : (char *)sp->s_name);

}	/* end get_symbol_name */

/*
 ****************************************************************
 *	Obtém o nome da rotina correspondente a um endereço	*
 ****************************************************************
 */
SYM *
getsyment (void *addr)
{
	SYM	*sp;
#if (0)	/****************************************************/
	SYM	*sp, *max_sp;
	ulong	max, value;

	max = 0; max_sp = NOSYM;

	for (sp = symtb; sp < endsymtb; sp++)
	{
		value = sp->s_value;

		if (value <= addr && value > max)
			{ max = value; max_sp = sp; }
	}

	return (max_sp);
#endif	/****************************************************/

	endsymtb->s_value = (long)addr;		/* sentinela */

	for (sp = symtb; sp->s_value < (long)addr; sp++)
		/* vazio */;

	if (sp == endsymtb)
		return (NOSYM);

	if (sp > symtb && sp->s_value > (long)addr)
		sp--;

	return (sp);

}	/* end getsyment */

/*
 ****************************************************************
 *	Lê a tabela de símbolos de um módulo objeto		*
 ****************************************************************
 */
int
read_symtb (char *name)
{
	int	fd;
	HEADER	h;

	if (symtb != NOSYM)
		return (0);

	if ((fd = open (name, O_RDONLY)) < 0)
		return (-1);

	if
	(	read (fd, &h, sizeof (HEADER)) != sizeof (HEADER) ||
		(h.h_magic != FMAGIC && h.h_magic != NMAGIC) ||
		h.h_ssize == 0 ||
		(h.h_ssize % sizeof (SYM)) != 0 ||
		lseek (fd, sizeof (HEADER) + h.h_tsize + h.h_dsize, L_SET) < 0 ||
		(symtb = malloc (h.h_ssize + sizeof (SYM))) == NOSYM
	)
	{
		close (fd);
		return (-1);
	}

	if (read (fd, symtb, h.h_ssize) != h.h_ssize)
	{
		free (symtb);
		close (fd);
		return (-1);
	}

	endsymtb = (SYM *)((char *)symtb + h.h_ssize);

	qsort (symtb, endsymtb - symtb, sizeof (SYM), symcmp);

	return (close (fd));

}	/* end read_symtb */

/*
 ****************************************************************
 *	Função de comparação (para qsort)			*
 ****************************************************************
 */
int
symcmp (const void *sp1, const void *sp2)
{
	int	dif;

	dif = ((SYM *)sp1)->s_value - ((SYM *)sp2)->s_value;

	if (dif)
		return (dif);

	return ((int)((SYM *)sp2)->s_type - (int)((SYM *)sp1)->s_type);

}	/* end symcmp */

/*
 ****************************************************************
 *	Imprime a tabela de símbolos				*
 ****************************************************************
 */
void
print_symtb (void)
{
	SYM	*sp;

	for (sp = symtb; sp < endsymtb; sp++)
		fprintf (stderr, "%P %s\n", sp->s_value, sp->s_name);

	fflush (stderr);

}	/* end print_symtb */
