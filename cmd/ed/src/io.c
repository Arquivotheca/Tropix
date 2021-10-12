/*
 ****************************************************************
 *								*
 *			txt/io.c				*
 *								*
 *	Entrada e Saída de Texto				*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 27.05.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <a.out.h>

#undef	TEXT	/* "ed.h" tem outras idéias a cêrca de TEXT */

#include "../h/ed.h"
#include "../h/txtio.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições deste módulo					*
 ****************************************************************
 */
entry const char *M_txtio;	/* Mensagem correspondente a TXTIO */

static char	line[LSIZE];	/* Buffer para cada linha lida */

#define	RET(a,b) { M_txtio = b; return (a); }

/*
 ****************************************************************
 *	Leitura e Armazenamento de Arquivo.			*
 ****************************************************************
 */
TXTIO	
txt_read (const char *name, BASE *base, SIZE *size)
{
	BASE		*list = NOBASE, *anterior = NOBASE, *h = NOBASE;
	FILE		*fp;
	int		n;
	int		magic;
	STAT		fileinfo;
	HEADER		a_out_h;

	/*
	 *	Testes e abertura do arquivo
	 */
	if (stat (name, &fileinfo) < 0)
		RET (IO_NFOUND, M_nfound);

	if (fileinfo.st_mode & S_IFDIR)
		RET (IO_DIRECTORY, M_directory);

	if ((fileinfo.st_mode & S_IFREG) == 0)
		RET (IO_IREGULAR, M_iregular);

	if (access (name, R_OK))
		RET (IO_PERMISSION, M_permission);

	if ((fp = fopen (name, "r")) == NOFILE)
		RET (IO_NFOUND, M_nfound);

	if ((fread (&a_out_h, sizeof (HEADER), 1, fp)) == 1)
	{
		magic = a_out_h.h_magic;

		if (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC)
			RET (IO_OBJECT, M_object);
	}

	rewind (fp);

	/*
	 *	Inicialização da leitura
	 */
	size->s_nc = 0;
	size->s_nl = 0;

	/*
	 *	Formação de uma lista com as linhas lidas do arquivo
	 */
	while ((n = read_line (fp, line)) >= 0)
	{
		size->s_nc += n;
		size->s_nl++;

		h = (BASE *)malloc (sizeof (BASE));

		if (size->s_nl == 1)
			anterior = list = h;

		if (h == NOBASE)
			goto free_list;

		if ((h->ch = malloc (n+1)) == NULL)
			goto free_list;

		strcpy (h->ch, line);

		h->ant = anterior;
		anterior->pos = h;

		anterior = h;
	}

	if (ferror (fp))
		goto free_list;

	if (size->s_nl == 0)
		RET (IO_EMPTY, M_empty);

	size->s_nc += size->s_nl;

	/*
	 *	A lista é incorporada à lista global de ED
	 */
	if (base == NOBASE)		/* lista estava vazia */
	{
		Root = list;
		Root->ant = anterior;
		anterior->pos = Root;
	}
	else
	{
		base->pos->ant = anterior;
		anterior->pos = base->pos;
		base->pos = list;
		list->ant = base;
	}

	fclose (fp);

	RET (IO_OK, M_ok);

	/*
	 *     Caso não haja memória disponivel ou haja um erro de leitura,
	 *	libera memória pedida, fecha arquivo e retorna tipo de erro
	 */
    free_list:
	if (h != NOBASE)
		free (h);

	if (anterior != NOBASE)
		anterior->pos = list;

	txt_free (list);

	fclose (fp);

	if (ferror (fp))
		{ RET (IO_RDERROR, M_rderror); }
	else
		{ RET (IO_NSPACE, M_nspace); }

}	/* end txt_read */

/*
 ****************************************************************
 *	Escreve o Arquivo de first até last			*
 ****************************************************************
 */
TXTIO	
txt_write (const char *name, BASE *first, BASE *last, bool Append, bool tests)
{
	char		*pch;
	FILE		*fp;
	BASE		*b;

	if (access (name, F_OK) >= 0)
	{
		if (tests && !streq (name, Text.t_name))
			RET (IO_EXISTS, M_exists);

		if (access (name, W_OK) < 0)
		{
			Text.t_flag |= T_RDONLY;
			RET (IO_RDONLY, M_rdonly);
		}
	}

	if ((fp = fopen (name, Append ? "a" : "w")) == NOFILE)
		RET (IO_WRERROR, M_wrerror);

	b = first;

	do
	{
		for (pch = b->ch; *pch; pch++)
			putc (*pch, fp);

		putc ('\n', fp); b = b->pos;
	}
	while (b != last->pos);

	fclose (fp);
	RET (IO_OK, M_ok);

}	/* end txt_write */

/*
 ****************************************************************
 *	Le Linha - Retorna numero de caracteres lidos ou -1	*
 ****************************************************************
 */
int	
read_line (FILE *fp, char *linha)
{
	int		c;
	char		*cs;
	char		*fs;

	cs = linha; fs = linha + LSIZE;

	while (cs < fs  &&  (c = getc (fp)) != '\n'  &&  !feof (fp))
	{
		if (c)
			*cs++ = c;
	}

	*cs = '\0';

	if (feof (fp) && cs == linha)
		return (-1);

	return (cs - linha);

}	/* end read_line */
