/*
 ****************************************************************
 *								*
 *			sort.c					*
 *								*
 *	Ordena e/ou intercala arquivos				*
 *								*
 *	Versão	1.0.0, de 03.10.86				*
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

#include <sys/types.h>
#include <sys/syscall.h>

#define	DEBUG
#ifdef	DEBUG
#include <sys/kcntl.h>
#endif	DEBUG

#include <stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "../h/sort.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 05.07.97";

#define	BLKMEM	4096

/*
 ****** Indicadores *********************************************
 */
entry int	vflag;		/* Verbose */
entry int	mflag;		/* Intercalação */
entry int	cflag;		/* Verifica se já está ordenado */
entry int	tflag = 'a';	/* Código de caracteres */
entry int	rflag;		/* Ordenação reversa */

/*
 ****** Variáveis globais ***************************************
 */
entry char	delim = ' ';	/* Delimitador entre as chaves */
entry int	numkeys;	/* No. de chaves dadas */
entry FILE	*out_fp = stdout; /* Arquivo de saída */

/*
 ****************************************************************
 *	Ordena e/ou intercala arquivos				*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	const char	*out_nm = NOSTR;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vmcraidfo:t:k:H")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'm':			/* Intercalação */
			mflag++;
			break;

		    case 'c':			/* Já está ordenado? */
			cflag++;
			break;

		    case 'r':			/* Ordenação reversa */
			rflag++;
			break;

		    case 'a':			/* código LEXICOGRÁFICO */
			tflag = 'a';
			break;

		    case 'i':			/* código ISO */
			tflag = 'i';
			break;

		    case 'd':			/* código DICIONÁRIO */
			tflag = 'd';
			break;

		    case 'f':			/* código FOLD */
			tflag = 'f';
			break;

		    case 'o':			/* Nome do arquivo de saída */
			out_nm = optarg;
			break;

		    case 't':			/* Delimitador */
			delim = *optarg;
			break;

		    case 'k':			/* Chaves */
			get_keys (optarg);
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	Pequena consistência
	 */
	argv += optind;
	argc -= optind;

	if (!mflag && argc > 1)
		help ();

	/*
	 *	Abre o arquivo de saída
	 */
	if (out_nm != NOSTR && (out_fp = fopen (out_nm, "w")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui criar \"%s\" (%s)\n",
			pgname, out_nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Escolhe a rotina de comparação e atribui em "compar"
	 */
	set_cmp_function ();

	/*
	 *	Se foi dada a opção "-m" então trata-se de intercalação
	 */
	if (mflag)
		merge (argv);
	else
		sort (argv[0]);

#ifdef	DEBUG
	/*
	 *	Imprime o total de memória
	 */
	if (vflag)
	{
		KPSZ	kpsz;
	
		kcntl (K_GETPSZ, &kpsz);

		fprintf
		(	stderr,
			"text = %d, data+bss = %d, stack = %d\n",
			kpsz.k_tsize, kpsz.k_dsize, kpsz.k_ssize
		);
	}
#endif	DEBUG

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Ordena							*
 ****************************************************************
 */
void
sort (const char *file_nm)
{
	register int	c, file_fd;
	register char	*ep, *cp;
	register char	**tp, **te;
	char		*file_area = NOSTR;
	char		**tag_area = NOVOID;
	int		tag_sz;
	off_t		file_sz;
	STAT		s;

	/*
	 *	Lê o arquivo (começa a área com um "\n")
	 */
	if (file_nm != NOSTR)
	{
		if ((file_fd = open (file_nm, O_RDONLY)) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui abrir \"%s\" (%s)\n",
				pgname, file_nm, strerror (errno)
			);
			exit (1);
		}

		if (fstat (file_fd, &s) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o estado de \"%s\" (%s)\n",
				pgname, file_nm, strerror (errno)
			);
			exit (1);
		}

		if (!S_ISREG (s.st_mode))
		{
			fprintf (stderr, "%s: \"%s\" não é regular\n", pgname, file_nm);
			exit (1);
		}

		file_sz = s.st_size + 1;	/* Não esquece o "\n" extra */

		if ((file_area = malloc (file_sz)) == NOSTR)
			{ fprintf (stderr, "Memória esgotada\n"); exit (1); }

		file_area[0] = '\n';

		if (s.st_size && read (file_fd, file_area + 1, s.st_size) != s.st_size)
		{
			fprintf
			(	stderr,
				"%s: Não consegui ler \"%s\" (%s)\n",
				pgname, file_nm, strerror (errno)
			);
			exit (1);
		}
	}
	else		/* PIPE */
	{
		file_sz = 0;	ep = cp = NOSTR;

		while ((c = getchar ()) != EOF)
		{
			if (cp >= ep)
			{
				if ((file_area = realloc (file_area, file_sz + BLKMEM)) == NOSTR)
					{ fprintf (stderr, "Memória esgotada\n"); exit (1); }

				cp = file_area + file_sz;
				ep = cp + BLKMEM;

				if (file_sz == 0) /* Não esquece o "\n" extra */
					*cp++ = '\n';

				file_sz += BLKMEM;
			}

			*cp++ = c;
		}

		file_sz = cp - file_area;
	}
	
	if (file_sz <= 1)
	{
		fprintf (stderr, "%s: Arquivo de tamanho NULO?\n", pgname);
		exit (1);
	}

	/*
	 *	Cria os "tags"
	 */
	tag_sz = 0;	tp = te = NOVOID;

	for (cp = file_area, ep = cp + file_sz; cp < ep; cp++)
	{
		if (*cp == '\n')
		{
			if (tp >= te)
			{
				if ((tag_area = realloc (tag_area, tag_sz * sizeof (char *) + BLKMEM)) == NOVOID)
					{ fprintf (stderr, "Memória esgotada\n"); exit (1); }
	
				tp = tag_area + tag_sz;
				te = tp + (BLKMEM / sizeof (char *));
	
				tag_sz += (BLKMEM / sizeof (char *));
			}

			*cp = '\0';		*tp++ = cp + 1;
		}
	}

	tp--; 	tag_sz = tp - tag_area; 	/* O último não existe */

	te = tp;	/* Final dos "tags" */

	if (vflag)
		fprintf (stderr, "No. de elementos = %d\n", tag_sz);

	/*
	 *	É apenas para verificar a ordenação?
	 */
	if (cflag) for (tp = tag_area; /* abaixo */; tp++)
	{
		if (tp >= te - 1)
		{
			fprintf
			(	stderr, file_nm ?
				"O arquivo \"%s\" já está ordenado\n" :
				"O arquivo já está ordenado\n", file_nm
			);
			exit (0);
		}

		if (compar (tp[0], tp[1]) > 0)
		{
			fprintf
			(	stderr, file_nm ?
				"O arquivo \"%s\" NÃO está ordenado\n" :
				"O arquivo NÃO está ordenado\n", file_nm
			);
			exit (1);
		}
	}

	/*
	 *	Ordena
	 */
	qtsort (tag_area, tag_sz, compar);

	/*
	 *	Escreve o arquivo ordenado
	 */
	for (tp = tag_area; tp < te; tp++)
		fnputs (tp[0], out_fp);

}	/* end sort */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - ordena e/ou intercala arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"    %s [-vcraidf]   [-o <saída>] "
		"[-t <d>] [-k \"x:y.zot ... \"] [<entrada>]\n"
		"    %s -m [-vraidf] [-o <saída>] "
		"[-t <d>] [-k \"x:y.zot ... \"] <entrada> ...\n",
		pgname, pgversion, pgname, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\t-m: Intercalação (\"merge\")\n"
		"\t-c: Verifica se o arquivo já está ordenado\n"
		"\t-r: Ordenação reversa (descendente)\n"
		"\t-a: Tipo de ordenação LEXICOGRÁFICO (tipo por omissão)\n"
		"\t-i: Tipo de ordenação ISO\n"
		"\t-d: Tipo de ordenação DICIONÁRIO\n"
		"\t-f: Tipo de ordenação FOLD (maíusculas = minúsculas)\n"
	);
	fprintf
	(	stderr,
		"\t-o: Escreve o arquivo ordenado em <saída> ao invés de \"stdout\"\n"
		"\t-t  Utiliza o caracter <d> ao invés do branco como delimitador de campos\n"
		"\t-k: Especifica chaves:\n"
		"\t\t x: No. do campo\n"
		"\t\t y: Início da chave dentro do campo\n"
		"\t\t z: Largura da chave\n"
		"\t\t o: Ordem ascendente/descendente (\"a\" ou \"d\")\n"
		"\t\t t: Tipo (\"a\", \"i\", \"d\", ou \"f\"), como acima\n"
		"\nObs.:\tSem opções: ordem ascendente, tipo LEXICOGRÁFICO, saída em \"stdout\"\n"
	);

	exit (2);

}	/* end help */
