/*
 ****************************************************************
 *								*
 *			diff3.c					*
 *								*
 *	Comparação diferencial de dois arquivos			*
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
#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <a.out.h>
#include <stat.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "diff.h"

/*
 ****************************************************************
 *	Definições e variáveis globais				*
 ****************************************************************
 */
#define	INCR		4192	/* Tamanho Basico para Alocacao */

#define	ROUND(x) ((x + 3) & ~3)	/* Arredondamento do tamanho */

entry char	*mem0,		/* Ponteiros para a área disponível */
		*mem1,
		*mem2;

static char	*tempfile;	/* Usado para a cópia da entrada padrão */

/*
 ****************************************************************
 *	Copia o arquivo da entrada padrão			*
 ****************************************************************
 */
char *
copytemp (void)
{
	char		buf[BUFSIZ];
	int		i, fd;

	signal (SIGHUP,  on_signal);
	signal (SIGINT,  on_signal);
	signal (SIGQUIT, on_signal);
	signal (SIGPIPE, on_signal);
	signal (SIGTERM, on_signal);

	tempfile = mktemp ("/tmp/diffXXXXXX");

	if ((fd = creat (tempfile, 0600)) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar o arquivo temporário \"%s\" (%s)\n",
			pgname, tempfile, strerror (errno)
		);
		done ();
	}

	while ((i = read (0, buf, BUFSIZ)) > 0)
	{
		if (write (fd, buf, i) != i)
		{
			fprintf
			(	stderr,
				"%s: Erro de escrita no arquivo "
				"temporário \"%s\" (%s)\n",
				pgname, tempfile, strerror (errno)
			);
			done ();
		}
	}

	if (i < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro de leitura da entrada padrão (%s)\n",
			pgname, strerror (errno)
		);
		done ();
	}

	close (fd);

	return (tempfile);

}	/* end copytemp */

/*
 ****************************************************************
 *	Compõe o nome do arquivo, se for dado como diretório	*
 ****************************************************************
 */
char *
dirfilenm (const char *dir, const char *file, int copy)
{
	const char	*tail;
	char		buf[BUFSIZ];

	if (copy)
	{
		fprintf
		(	stderr,
			"%s: Não podemos especificar a entrada padrão "
			"e um diretório\n\n",
			pgname
		);
		help ();
	}

	tail = strrchr (file, '/');

	if ((tail = strrchr (file, '/')) == NOSTR)
		tail = file;
	else
		tail++;

	sprintf (buf, "%s/%s", dir, tail);

	return (strdup (buf));

}	/* end  dirfilenm */

/*
 ****************************************************************
 *	Lê um arquivo						*
 ****************************************************************
 */
void
readfile (const char *filenm, STAT *files, char **fileaddr)
{
	char		*addr, *cp;
	HEADER		*hp;
	int		fd;

	/*
	 *	Em primeiro lugar, aloca memória
	 */
	if (files->st_size <= 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o tamanho de \"%s\"\n",
			pgname, filenm
		);
		done ();
	}

	addr = diff_malloc (files->st_size + 2);

	/*
	 *	Abre o arquivo fonte
	 */
	if ((fd = open (filenm, O_RDONLY)) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"%s\" (%s)\n",
			filenm, strerror (errno)
		);
		done ();
	}

	/*
	 *	Lê o conteúdo do arquivo (addr é múltiplo de 4)
	 */
	if (read (fd, addr, files->st_size) != files->st_size)
	{
		fprintf
		(	stderr,
			"Erro na leitura de \"%s\" (%s)\n",
			filenm, strerror (errno)
		);
		done ();
	}

	close (fd);

	/*
	 *	Verifica se é um módulo objeto 
	 */
	if (files->st_size >= sizeof (HEADER))
	{
		hp = (HEADER *)addr;

		if (hp->h_magic == FMAGIC || hp->h_magic == NMAGIC)
		{
			fprintf
			(	stderr,
				"%s: O arquivo \"%s\" é um módulo objeto\n",
				pgname, filenm
			);
			done ();
		}
	}

	/*
	 *	Prepara o final do programa
	 */
	cp = addr + files->st_size - 1;

	if (*cp != '\n')
	{
		fprintf
		(	stderr,
			"%s: O arquivo \"%s\" não termina por um \"\\n\"\n",
			pgname, filenm
		);
		*++cp = '\n';
	}

	*++cp = '\0';

	*fileaddr = addr;

}	/* end readfile */

/*
 ****************************************************************
 *	Alocação de Memória					*
 ****************************************************************
 */
void *
diff_malloc (int size)
{
	void		*addr;

	size = ROUND (size);

	/*
	 *	Verifica se tem espaço
	 */
	while (mem1 + size > mem2)
	{
		if (mem0 == NOSTR)
		{
			addr = mem0 = mem1 = sbrk (INCR);
			mem2 = mem1 + INCR;
		}
		else
		{
			addr = sbrk (INCR);
			mem2 += INCR;
		}

		if ((int)addr < 0)
		{
			fprintf (stderr, "%s: Memória excedida", pgname);
			done ();
		}
	}

	/*
	 *	Acerta os indicadores
	 */
	addr = mem1;
	mem1 += size;

	return (addr);

}	/* end diff_malloc */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Rotina de liberação de memória dummy			*
 ****************************************************************
 */
void
free (void *a)
{
	if (vflag)
		printf ("Free: %P\n", a);

}	/* end free */
#endif	/*******************************************************/

#undef	MDEALLOC
#ifdef	MDEALLOC
/*
 ****************************************************************
 *	Devolve um trecho não usado de memória			* 
 ****************************************************************
 */
void
mdealloc (void *addr)
{
	char		*a;

	a = (void *)ROUND ((int)addr);

	if (a >= mem0 && a <= mem1) 
	{
		mem1 = a;
	}
	else
	{
		fprintf
		(	stderr,
			"%s: Devolução inválida de memória: %X\n",
			pgname,
			addr
		);
	}

}	/* end mdealloc */
#endif	MDEALLOC

/*
 ****************************************************************
 *	Em caso de sinal, ...					*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	done ();

}	/* end on_signal */

/*
 ****************************************************************
 *	Encerra o programa					*
 ****************************************************************
 */
void
done (void)
{
	/*
	 *	Remove, se for o caso, a cópia da entrada padrão
	 */
	if (tempfile)
		unlink (tempfile);

	/*
	 *	Imprime a Memória ocupada
	 */
	if (vflag)
	{
		KPSZ		kpsz;
		char		*endbrk;

		kcntl (K_GETPSZ, &kpsz);
		endbrk = sbrk (0);

		fprintf
		(
			stderr,
			"Texto = %d, Data = %d, Bss = (%d + %d), "
			"Stack = %d, Total = %d\n",
			(int)&etext,
			(int)&edata - (int)&etext,
			(int)&end - (int)&edata,
			endbrk - (int)&end,
			kpsz.k_ssize,
			endbrk + kpsz.k_ssize
		);
	}

	exit (status);

}	/* end done */

#undef	DEBUG
#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime uma linha terminada com '\n'			*
 ****************************************************************
 */
void
printline (char *cp, int n)
{
	int		c;

	while ((c = *cp++) != '\n' && n-- > 0)
		fputc (c, stdout);

	fputc ('\n', stdout);

}	/* end printline */
#endif	DEBUG
