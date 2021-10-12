/*
 ****************************************************************
 *								*
 *			io.c					*
 *								*
 *	Entrada e saida de texto				*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.2, de 18.05.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
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
#include <string.h>
#include <stat.h>
#include <unistd.h>
#include <a.out.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
entry char	*M_txtio;	/* Mensagem correspondente a TXTIO */

entry char	dos_mode;	/* Modo DOS (<cr><nl>) */

/*
 ****************************************************************
 *	Definição de Macros Locais				*
 ****************************************************************
 */
#define	RET(a,b)	{ M_txtio = b; return (a); }

/*
 ******	Protótipos de funções ***********************************
 */
static int	 read_line (FILE *, char *);

/*
 ****************************************************************
 *	Leitura e armazenamento de arquivo			*
 ****************************************************************
 */
TXTIO	
txt_read (char *name, BASE *base, SIZE *size)
{
	BASE	 	*list = NOBASE;		/* CUIDADO */
	BASE 		*anterior = NOBASE;	/* CUIDADO */
	BASE 		*h = NOBASE;		/* CUIDADO */
	FILE		*fp;
	int		n;
	struct		stat	fileinfo;
	char		bufio[16 * BLSZ];
	HEADER		a_out_h;

	/*
	 *	Testes e abertura do arquivo
	 */
	if (stat (name, &fileinfo) < 0)
		RET (IO_NFOUND, "Não encontrei \"%s\"");

	if (fileinfo.st_mode & S_IFDIR)
		RET (IO_DIRECTORY, "\"%s\" é um diretório");

	if ((fileinfo.st_mode & S_IFREG) == 0)
		RET (IO_IREGULAR, "\"%s\" não é um arquivo regular");

	if (access (name, R_OK) < 0)
		RET (IO_PERMISSION, "Permissão para leitura de \"%s\" negada");

	if (access (name, W_OK) < 0)
		set (Text->t_flags, T_RDONLY);

	if ((fp = fopen (name, "r")) == NOFILE)
		RET (IO_NFOUND, "Não encontrei \"%s\"");

	if (setvbuf (fp, bufio, _IOFBF, sizeof (bufio)) < 0)
		RET (IO_NFOUND, "Erro em \"setvbuf\"");

	if ((fread (&a_out_h, sizeof (HEADER), 1, fp)) == 1)
	{
		int		magic;

		magic = a_out_h.h_magic;

		if (magic == FMAGIC || magic == NMAGIC || magic == SMAGIC)
			RET (IO_OBJECT, "\"%s\" é um arquivo objeto");
	}

	rewind (fp);

	/*
	 *	Inicialização da leitura
	 */
	size->s_nc = 0;
	size->s_nl = 0;

	/*
	 *	Formação de uma lista local com as linhas lidas do arquivo
	 */
	dos_mode = 0;

	while ((n = read_line (fp, temp_buffer)) >= 0)
	{
		size->s_nc += n;
		size->s_nl++;

		h = malloc (sizeof (BASE));

		if (size->s_nl == 1)
			anterior = list = h;

		if (h == NOBASE)
			goto free_list;

		if ((h->ch = malloc (n+1)) == NOSTR)
			goto free_list;

		strcpy (h->ch, temp_buffer);

		h->ant = anterior;
		anterior->pos = h;

		anterior = h;
	}

	if (ferror (fp))
		goto free_list;

	if (size->s_nl == 0)
		RET (IO_EMPTY, "\"%s\" está vazio");

	size->s_nc += size->s_nl;

	/*
	 *	A lista local é incorporada à lista global de ED
	 */
	if (base == NOBASE)		/* lista estava vazia */
	{
		NEWFIRST (list);
		NEWLAST (anterior);
	}
	else
	{
		base->pos->ant = anterior;
		anterior->pos = base->pos;
		base->pos = list;
		list->ant = base;
	}

	fclose (fp);

	RET (IO_OK, "Ok");

	/*
	 *	Caso não haja memória disponivel ou haja um erro de leitura,
	 *	libera memória pedida, fecha arquivo e retorna tipo de erro
	 */
    free_list:
	if (h != NOBASE)
		free (h);

	if (anterior != NOBASE)
		txt_free (list, anterior);

	fclose (fp);

	if (ferror (fp))
		{ RET (IO_RDERROR, "Erro na leitura"); }
	else
		{ RET (IO_NSPACE, "Não há memória para \"%s\""); }

}	/* end txt_read */

/*
 ****************************************************************
 *	Escreve o arquivo de "first" até "last"			*
 ****************************************************************
 */
TXTIO	
txt_write (const char *name, BASE *first, BASE *last, bool append, bool tests)
{
	char		*pch;
	FILE		*fp;
	BASE		*b;

	if (access (name, F_OK) >= 0)
	{
		if (tests && !streq (name, Text->t_name))
			RET (IO_EXISTS, "\"%s\" já existe");

		if (access (name, W_OK) < 0)
		{
			set (Text->t_flags, T_RDONLY);
			RET (IO_RDONLY, "\"%s\" só tem permissão para leitura");
		}
	}

	if ((fp = fopen (name, append ? "a" : "w")) == NOFILE)
		RET (IO_WRERROR, "Erro na escrita");

	b = first;

	do
	{
		for (pch = b->ch; *pch; pch++)
			putc (*pch, fp);

		if (dos_mode)
			putc ('\r', fp);

		putc ('\n', fp);
		b = b->pos;
	}
	while (b != last->pos);

	fclose (fp);
	RET (IO_OK, "Ok");

}	/* end txt_write */

/*
 ****************************************************************
 *	Le linha - retorna numero de caracteres lidos ou -1	*
 ****************************************************************
 */
static int	
read_line (FILE *fp, char *line)
{
	int		c;
	char		*cs;
	const char	*fs;

	for (cs = line, fs = line + LSIZE - 1; /* abaixo */; /* abaixo */)
	{
		if ((c = getc (fp)) < 0 || c == '\n')
			break;

		if (c == '\0')
			continue;

		if (c == '\r')
		{
			if ((c = getc (fp)) < 0)
				break;

			if (c == '\0')
				continue;

			if (c == '\n')
				{ dos_mode = 1; break; }

			if (cs < fs)
				*cs++ = '\r';
		}

		if (cs < fs)
			*cs++ = c;
	}

#if (0)	/*******************************************************/
	while (cs < fs  &&  (c = getc (fp)) != '\n'  &&  !feof (fp))
	{
		if (c)
			*cs++ = c;
	}
#endif	/*******************************************************/

	*cs = '\0';

	if (feof (fp) && cs == line)
		return (-1);

	return (cs - line);

}	/* end read_line */
