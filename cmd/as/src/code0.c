/*
 ****************************************************************
 *								*
 *			code0.c					*
 *								*
 *	Rotinas comuns para geração de código			*
 *								*
 *	Versão	1.0.00, de 13.06.86				*
 *		3.0.04, de 16.08.94				*
 *		3.0.11, de 15.07.95				*
 *								*
 *	Rotinas:						*
 *		checkexp	checkrexp	reloc		*
 *		putB		putW		putL		*
 *		putfloat	putdouble	putextd		*
 *		initcode	finishcode	invert		*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <a.out.h>
#include <time.h>
#include <errno.h>

#include "../h/common.h"
#include "../h/tokens.h"
#include "../h/scan.h"
#include "../h/id_sym_tb.h"
#include "../h/expr.h"
#include "../h/opnd.h"
#include "../h/msg.h"
#include "../h/as.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
entry HEADER	header;		/* Header para o "a.out" */

/*
 ******	Protótipos **********************************************
 */
void		header_endian_conversion (HEADER *);

/*
 ****************************************************************
 *	Verifica Expressões Relocáveis em um Intervalo		*
 ****************************************************************
 */
long
checkrexp (EXP *exp, int code_sz, int offset_sz)
{
	long		val;

	if (exp->ex_type != section)
	{
		if  (exp->ex_type == UNDEF)
		{
			msg
			(	ERRO, EXPMARK,
				"Esta expressão não poderia ser externa",
				NOSTR, exp
			);
			return (0);
		}
		elif (exp->ex_type == ABS)
		{
			msg
			(	ERRO, EXPMARK,
				"Esta expressão não poderia ser absoluta",
				NOSTR, exp
			);
			return (0);
		}
		elif
		(	exp->ex_type != TEXT  &&  exp->ex_type != CONST ||
			section	     != TEXT  &&  section      != CONST
		)
		{
			msg
			(	ERRO, EXPMARK,
				"Relocação para seção não válida neste contexto",
				NOSTR, exp
			);
			return (0);
		}
	}

	val = exp->ex_value - *lc - code_sz;

	switch (offset_sz)
	{
	    case RBYTE:
		if (val < -128 || val > 127)
		{
		    err:
			msg
			(	ERRO, EXPMARK,
				"Valor/deslocamento (%d) fora dos limites",
				val, exp
			);
			return (0);
		}

		return (val & 0xFF);

	    case RWORD:
		if (val < -32768 || val > 32767)
			goto err;

		return (val & 0xFFFF);

	    case RLONG:
		return (val);

	    default:
		msg
		(	ERRO, EXPMARK,
			"checkrexp: Tamanho Inválido para deslocamento: %X",
			offset_sz, exp
		);
		return (0);

	}	/* end switch */

}	/* end checkrexp */

/*
 ****************************************************************
 *	Gera um Valor Possivelmente Relocável			*
 ****************************************************************
 */
int
reloc (EXP *exp, int size)
{
	RELOC		r;

	r.r_symbol = 0;
	r.r_flags  = 0;

	switch (exp->ex_type)
	{
	    case ERR:
	    case ABS:
		goto noreloc;

	    case TEXT:
	    case CONST:
		r.r_flags |= RTEXT;
		break;

	    case DATA:
		r.r_flags |= RDATA;
		break;

	    case BSS:
		r.r_flags |= RBSS;
		break;

	    case UNDEF:
		r.r_symbol = exp->ex_ref->s_index;
		r.r_flags |= REXT;
		break;

	    default:
		msg (COMP, NOMARK, "Tipo Inválido para Relocação");

		/* NOTREACHED */
	}

	/*
	 *	Emite a relocação
	 */
	r.r_flags |= size;
	r.r_pos  = sectoff;

	if (section == TEXT || section == CONST)
	{
		header.h_rtsize += sizeof (RELOC);
		fwrite (&r, sizeof (RELOC), 1, reltext);
	}
	elif (section == DATA)
	{
		header.h_rdsize += sizeof (RELOC);
		fwrite (&r, sizeof (RELOC), 1, reldata);
	}

	/*
	 *	Emite a constante
	 */
    noreloc:
	switch (size)
	{
	    case RBYTE:
		return (putB (exp->ex_value));

	    case RWORD:
		return (putW (exp->ex_value));

	    case RLONG:
		return (putL (exp->ex_value));

	    default:
		msg (COMP, NOMARK, "Tamanho Inválido para Relocação");
		return (0);
	}

}	/* end reloc */

/*
 ****************************************************************
 *	Gera um valor longo com relocação relativa		*
 ****************************************************************
 */
int
reloc_rel (EXP *exp, int code_sz, int size)
{
	RELOC		r;

	if   (exp->ex_type != UNDEF)
	{
		msg
		(	ERRO, NOMARK,
			"reloc_rel: Tipo inválido para Relocação: %02X",
			exp->ex_type
		);
		return (4);
	}
	elif (section != TEXT && section != CONST)
	{
		msg
		(	ERRO, NOMARK,
			"reloc_rel: Seção inválida para Relocação: %02X",
			section
		);
		return (4);
	}

	r.r_symbol = exp->ex_ref->s_index;
	r.r_pos    = sectoff;

	switch (size)
	{
	    case RWORD:
		r.r_flags = REXTREL|RWORD;

		header.h_rtsize += sizeof (RELOC);
		fwrite (&r, sizeof (RELOC), 1, reltext);

		return (putW (exp->ex_value - *lc - code_sz));

	    case RLONG:
		r.r_flags = REXTREL|RLONG;

		header.h_rtsize += sizeof (RELOC);
		fwrite (&r, sizeof (RELOC), 1, reltext);

		return (putL (exp->ex_value - *lc - code_sz));

	    default:
		msg
		(	ERRO, NOMARK,
			"reloc_rel: Tamanho Inválido para Relocação: %X",
			size
		);
		return (4);

	}	/* end switch (size) */

}	/* end reloc_rel */

/*
 ****************************************************************
 *	Escreve um Caractere 					*
 ****************************************************************
 */
int
putB (int c)
{
	c &= 0xFF;

	if (section == BSS)
	{
		if (c)
			msg (ADVR, LONGMARK, "Valor não nulo em BSS");

		if (lflag)
			prntloc (*lc);
	}
	else
	{
		sectoff += sizeof (char);

		putc (c, aout);

		if (lflag)
			prntcode (c, 2);
	}

	return (sizeof (char));

}	/* end putB */

/*
 ****************************************************************
 *	Escreve uma palavra na ordem "inversa" (little endian)	*
 ****************************************************************
 */
int
putW (int s)
{
	s &= 0xFFFF;

	if (section == BSS)
	{
		if (s)
			msg (ADVR, LONGMARK, "Valor não nulo em BSS");

		if (lflag)
			prntloc (*lc);
	}
	else
	{
		sectoff += sizeof (short);

		putc (s, aout); 	putc (s >> 8, aout);

		if (lflag)
		{
			if (eflag)
				prntcode (s, 4);
			else
				prntbigcode (s, 4);
		}
	}

	return (sizeof (short));

}	/* end putW */

/*
 ****************************************************************
 *	Escreve uma palavra na ordem "normal" (big endian)	*
 ****************************************************************
 */
int
putbigW (int s)
{
	s &= 0xFFFF;

	if (section == BSS)
	{
		if (s)
			msg (ADVR, LONGMARK, "Valor não nulo em BSS");

		if (lflag)
			prntloc (*lc);
	}
	else
	{
		sectoff += sizeof (short);

		putc (s >> 8, aout); 	putc (s, aout);

		if (lflag)
			prntbigcode (s, 4);
	}

	return (sizeof (short));

}	/* end putbigW */

/*
 ****************************************************************
 *	Escreve um Longo					*
 ****************************************************************
 */
int
putL (ulong l)
{
	if (section == BSS)
	{
		if (l)
			msg (ADVR, LONGMARK, "Valor não nulo em BSS");

		if (lflag)
			prntloc (*lc);
	}
	else
	{
		sectoff += sizeof (long);

		putc (l,	aout); 		putc (l >>  8, aout);
		putc (l >> 16,	aout); 		putc (l >> 24, aout);

		if (lflag)
		{
			if (eflag)
			{
				prntcode (l & 0xFFFF,	4);
				prntcode (l >> 16,	4);
			}
			else
			{
				prntbigcode (l >> 16,	4);
				prntbigcode (l & 0xFFFF, 4);
			}
		}
	}

	return (sizeof (long));

}	/* end putL */

/*
 ****************************************************************
 *	Escreve um Float					*
 ****************************************************************
 */
int
putfloat (double d)
{
	ushort		*lp;
	float		f;

#if (0)	/*************************************/
	if (*lc & 1)
		msg (ERRO, NOMARK, "FLOAT começando em endereço ímpar");
#endif	/*************************************/

	if (section == BSS)
	{
		if (lflag)
			prntloc (*lc);
	}
	else
	{
		f = d; sectoff += sizeof (float); putout (&f, sizeof (float));

		if (lflag)
		{
			lp = (ushort *)&f;

			prntcode (*lp++, 4); prntcode (*lp,   4);
		}
	}

	return (sizeof (float));

}	/* end putfloat */

/*
 ****************************************************************
 *	Escreve um Double					*
 ****************************************************************
 */
int
putdouble (double d)
{
	ushort		*lp;

#if (0)	/*************************************/
	if (*lc & 1)
		msg (ERRO, NOMARK, "DOUBLE começando em endereço ímpar");
#endif	/*************************************/

	if (section == BSS)
	{
		if (lflag)
			prntloc (*lc);
	}
	else
	{
		sectoff += sizeof (double); putout (&d, sizeof (double));

		if (lflag)
		{
			lp = (ushort *)&d;

			prntcode (*lp++, 4); prntcode (*lp++, 4);
			prntcode (*lp++, 4); prntcode (*lp,   4);
		}
	}

	return (sizeof (double));

}	/* end putdouble */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Escreve um Extended					*
 ****************************************************************
 */
int
putextd (extd e)
{
	ushort		*lp;

#if (0)	/*************************************/
	if (*lc & 1)
		msg (ERRO, NOMARK, "EXTD começando em endereço ímpar");
#endif	/*************************************/

	if (section == BSS)
	{
		if (lflag)
			prntloc (*lc);
	}
	else
	{
#if (0)	/*************************************/
extern void	*fpetome (extd *);
		if (fpused)		/* converte o formato, se for o caso */
			fpetome (&e);
#endif	/*************************************/

		sectoff += sizeof (extd);
		putout (&e, sizeof (extd));

		if (lflag)
		{
			lp = (ushort *)&e;
			prntcode (*lp++, 4);
			prntcode (*lp++, 4);
			prntcode (*lp++, 4);
			prntcode (*lp++, 4);
#ifdef	ICA
			prntcode (*lp++, 4);
			prntcode (*lp,   4);
#endif	ICA
		}
	}

	return (sizeof (extd));

}	/* end putextd */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Escreve uma Sequencia de bytes no "a.out"		*
 ****************************************************************
 */
void
putout (const void *vp, int size)
{
	const char	*cp = vp;
	int		c;

	while (size-- > 0)
		{ c = *cp++; putc (c, aout); }

}	/* end putout */

/*
 ****************************************************************
 *	Inicializa a Geração de Codigo				*
 ****************************************************************
 */
void
initcode (void)
{
	/*
	 *	Abre o "a.out"
	 */
	if ((aout = fopen (objname, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir \"%s\" (%s)\n",
			pgname, objname, strerror (errno)
		);
		quit (1);
	}

	rtname = mktemp ("/tmp/asXXXXXX");

	if ((reltext = fopen (rtname, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir o temporário (%s)\n",
			pgname, strerror (errno)
		);
		quit (1);
	}

	rdname = mktemp ("/tmp/asXXXXXX");

	if ((reldata = fopen (rdname, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir o temporário (%s)\n",
			pgname, strerror (errno)
		);
		quit (1);
	}

	fseek (aout, sizeof (HEADER), SEEK_SET);
	sectoff = 0;

	header.h_rtsize = 0;
	header.h_rdsize = 0;

}	/* end initcode */

/*
 ****************************************************************
 *	Termina o "a.out"					*
 ****************************************************************
 */
void
finishcode (void)
{
	int 		n, frt, frd, a_out_fd;
	long		writesymtb ();
	char		area[4096];

	/*
	 *	Inicializa o header do arquivo e escreve a tabela
	 *	de símbolos.
	 */
	header.h_machine = 0x486;
	header.h_magic   = FMAGIC;
	header.h_version = 0;
	header.h_flags   = 0;
	header.h_time    = time ((time_t *)0);

	header.h_tstart  = 0;
	header.h_dstart  = tsize + csize;
	header.h_entry   = 0;

	header.h_tsize   = tsize + csize;
	header.h_dsize   = dsize;
	header.h_bsize   = bsize;
	header.h_ssize   = writesymtb ();

	fflush (aout);

	/*
	 *	Reabre os arquivos para acrescentar as relocações.
	 */
	reltext  = freopen (rtname, "r", reltext);
	reldata  = freopen (rdname, "r", reldata);

	frt	 = fileno (reltext);
	frd	 = fileno (reldata);
	a_out_fd = fileno (aout);

	lseek (a_out_fd, sizeof (HEADER) + tsize + csize + dsize + header.h_ssize, SEEK_SET);

	/*
	 *	Escreve a relocação do "text".
	 */
	while ((n = read (frt, area, sizeof (area))) > 0)
		write (a_out_fd, area, n);

	/*
	 *	Escreve a relocação do "data".
	 */
	while ((n = read (frd, area, sizeof (area))) > 0)
		write (a_out_fd, area, n);

#ifdef	DB
	/*
	 *	Escreve a Tabela de Linhas e de Depuração.
	 */
	header.h_lnosize = writelnotb ();
	header.h_dbsize = writedbtb ();
#endif	DB

	/*
	 *	Escreve o cabecalho
	 */
	lseek (a_out_fd, 0, SEEK_SET);
	write (a_out_fd, &header, sizeof (HEADER) );

	/*
	 *	Não havendo tabela de símbolos, verifica se faltam
	 *	bytes para completar um multiplo de 4 nos tamanhos
	 *	das seções.
	 */
	if (header.h_ssize + header.h_rtsize + header.h_rdsize == 0)
	{
		long	pos;

		pos = lseek (a_out_fd, 0, SEEK_END);

		if (pos < sizeof (HEADER) + tsize + csize + dsize)
		{
			lseek (a_out_fd, sizeof (HEADER) + tsize + csize + dsize - 1, SEEK_SET);
			write (a_out_fd, "\0", 1);
		}
	}

	close (frt);
	close (frd);
	fclose (aout);
   /***	close (a_out_fd); ***/

	unlink (rtname);
	unlink (rdname);

}	/* end finishcode */
