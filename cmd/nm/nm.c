/*
 ****************************************************************
 *								*
 *			nm.c					*
 *								*
 *	Imprime a tabela de símbolos de módulos objeto		*
 *								*
 *	Versão	2.0.0, de 09.05.88				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: nm						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <a.out.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 08.05.02";

#define	elif	else if

entry int	exit_code = 0;	/* Código de retorno */

/*
 ******	Indicadores *********************************************
 */
entry int	gflag;		/* Lista somente as variáveis globais */

entry int	uflag;		/* Lista as variáveis indefinidas */
entry int	tflag;		/* Lista TEXT */
entry int	dflag;		/* Lista DATA */
entry int	bflag;		/* Lista BSS */
entry int	cflag;		/* Lista COMMON */
entry int	aflag;		/* Lista ABS */
entry int	rflag;		/* Lista REG */
entry int	fflag;		/* Lista somente nomes dos módulos */

entry int	flag_sum;	/* Número de regiões acima a listar */

entry int	oflag;		/* Mostra o nome do módulo */
entry int	nflag; 		/* Ordena numericamente */
entry int	sflag = +1;	/* Ord. decres. (-1), não (0), cres. (+1) */

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */
entry int	szflag;		/* Imprime o tamanho do campo */

/*
 ****** Variáveis globais ***************************************
 */
entry SYM	**symtb_vec;	/* ponteiros para os elementos de tbsimb */

#define NOSYM (SYM *)NULL

/*
 ****** Protótipos de funções ***********************************
 */
void		mod_proc (const char *);
int		mod_open (const char *, HEADER *);
int		table_read (const char *, int, HEADER *);
int		symbol_type (SYM *);
int		sym_compare (SYM *, SYM *);
void		table_print (const char *, int, HEADER *);
void		help (void);

/*
 ****************************************************************
 *	Imprime a tabela de símbolos de módulos objeto		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, num;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "gutdbcarfonepvNH")) != EOF)
	{
		switch (opt)
		{
		    case 'g':			/* Variáveis globais */
			gflag++;
			break;

		    case 'u':			/* Variáveis indefinidas */
			uflag++;
			break;

		    case 't':			/* Variáveis do texto */
			tflag++;
			break;

		    case 'd':			/* Variáveis do data */
			dflag++;
			break;

		    case 'b':			/* Variáveis do bss */
			bflag++;
			break;

		    case 'c':			/* COMMON */
			cflag++;
			break;

		    case 'a':			/* ABS */
			aflag++;
			break;

		    case 'r':			/* REG */
			rflag++;
			break;

		    case 'f':			/* Nomes de módulos */
			fflag++;
			break;

		    case 'o':			/* Lista o nome do módulo */
			oflag++;
			break;

		    case 'n':			/* Ordenar numéricamente */
			nflag++;
			break;

		    case 'e':			/* Ordenação decrescente */
			sflag = -1;
			break;

		    case 'p':			/* Não ordena */
			sflag = 0;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'N':			/* Nomes do <stdin> */
			Nflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Pequena consistência
	 */
	flag_sum = uflag + tflag + dflag + bflag + cflag + aflag + rflag + fflag;

	if (flag_sum == 0)
	{
		uflag++; tflag++; dflag++; bflag++;
		cflag++; aflag++; rflag++; fflag++;

		flag_sum = 8;
	}

	if (nflag && sflag == 0)
	{
		error ("Opções conflitantes\n");
		help ();
	}

	/*
	 *	Analisa o size flag
	 */
	if (nflag && sflag == 1 && flag_sum == 1 && tflag + dflag + bflag)
		szflag++;

	/*
	 *	Analisa o Nflag
	 */
	if (Nflag)
	{
		if (*argv != NOSTR)
		{
			error ("Os argumentos supérfluos serão ignorados");
			*argv = NOSTR;
		}
	}
	else
	{
		if (*argv == NOSTR)
		{
			mod_proc ("a.out");

			exit (exit_code);
		}
	}

	/*
	 *	Procura os argumentos
	 */
	if (*argv == NOSTR) 		/* Não foram dados argumentos: lê do "stdin" */
	{
		char		*area = alloca (1024);

		for (num = 0; fngets (area, 1024, stdin) != NOSTR; num++)
		{
			if (!oflag)
			{
				if (num != 0)
					printf ("\n");
				
				printf ("%s:\n", area);
			}
	
			mod_proc (area);
		}
	}
	else				/* Foram dados argumentos */
	{
		for (num = 0; *argv; argv++, num++)
		{
			if (argc > 1  &&  !oflag)
			{
				if (num != 0)
					printf ("\n");
				
				printf ("%s:\n", *argv);
			}
	
			mod_proc (*argv);
		}
	}

	return (exit_code);

}	/* end nm */

/*
 ****************************************************************
 *	Processa um módulo					*
 ****************************************************************
 */
void
mod_proc (const char *mod_nm)
{
	int		mod_fd, n_sym;
	HEADER		h;

	/*
	 *	Verifica se consegue abrir o módulo
	 */
	if ((mod_fd = mod_open (mod_nm, &h)) < 0)
		{ exit_code++; return; }

	/*
	 *	Imprime os símbolos do módulo
	 */
	if ((n_sym = table_read (mod_nm, mod_fd, &h)) < 0)
		exit_code++;
	else
		table_print (mod_nm, n_sym, &h);

	close (mod_fd);

}	/* end prodmod */

/*
 ****************************************************************
 *	Abre o módulo						*
 ****************************************************************
 */
int
mod_open (const char *mod_nm, HEADER *hp)
{	
	int		mod_fd, sz;
	STAT		s;

	/*
	 *	Abre o módulo
	 */
	if ((mod_fd = open (mod_nm, O_RDONLY)) < 0 || fstat (mod_fd, &s) < 0)
	{
		error ("*Não consegui abrir \"%s\"", mod_nm);
		return (-1);
	}

	/*
	 *	Verifica se é um arquivo regular
 	 */
	if (!S_ISREG (s.st_mode))
	{
		error ("\"%s\" não é um arquivo regular", mod_nm);
		close (mod_fd); return (-1);
	}

	/*
	 *	Lê o header do módulo e verifica se é um módulo objeto
 	 */
	if ((sz = read (mod_fd, hp, sizeof (HEADER))) < 0)
	{
		error ("*Erro de leitura no cabeçalho de \"%s\"", mod_nm);
		close (mod_fd); return (-1);
	}

	if
	(	sz != sizeof (HEADER) ||
		hp->h_magic != FMAGIC  &&  hp->h_magic != NMAGIC  &&  hp->h_magic != SMAGIC
	)
	{
		error ("\"%s\" não é um módulo objeto", mod_nm);
		close (mod_fd); return (-1);
	}

	/*
	 *	Verifica se possui tabela de símbolos
	 */
	if (hp->h_ssize == 0)
	{
		error ("\"%s\" não possui tabela de símbolos", mod_nm);
		close (mod_fd); return (-1);
	}

	return (mod_fd);

}	/* end mod_open */	

/*
 ****************************************************************
 *	Lê a tabela de símbolos					*
 ****************************************************************
 */
int
table_read (const char *mod_nm, int mod_fd, HEADER *hp)
{
	SYM		*sp, *end_symtb;
	int		symtb_sz = 0, n_sym = 0;
	static SYM	*symtb;

	/*
	 *	Libera área utilizada anteriormente
	 */
	if (symtb != NOSYM)
		free (symtb);

	if (symtb_vec != (SYM **)NULL)
		free (symtb_vec);

	/*
	 *	Lê a tabela
	 */
	if ((symtb = malloc (hp->h_ssize)) == NOSYM)
	{
		error ("Não há memória suficiente para a tabela de símbolos de \"%s\"", mod_nm);
		return (-1);
	}

	lseek (mod_fd, hp->h_tsize + hp->h_dsize, L_CUR);

	if (read (mod_fd, symtb, hp->h_ssize) != hp->h_ssize)
	{
		error ("Erro de leitura em \"%s\"", mod_nm);
		return (-1);
	}

	/*
	 *	Conta o número de entradas
	 */
	for
	(	sp = symtb, end_symtb = (SYM *)((int)symtb + hp->h_ssize);
		sp < end_symtb;
		sp = SYM_NEXT_PTR (sp)
	)
	{
		symtb_sz++;
	}

	if (sp != end_symtb)
		error ("Erro de fase na tabela de símbolos de \"%s\"", mod_nm);

	/*
	 *	Aloca o vetor de ponteiros
	 */
	if ((symtb_vec = malloc (symtb_sz * sizeof (SYM *))) == (SYM **)NULL)
	{
		error ("Não há memória suficiente para a tabela de símbolos de \"%s\"", mod_nm);
		return (-1);
	}

	/*
	 *	Cria o vetor de ponteiros
	 */
	for
	(	sp = symtb, end_symtb = (SYM *)((int)symtb + hp->h_ssize);
		sp < end_symtb;
		sp = SYM_NEXT_PTR (sp)
	)
	{
		int		type;

		if ((type = symbol_type (sp)) < 0)
			continue;

		sp->s_type = type;

		symtb_vec[n_sym++] = sp;
	}

	return (n_sym);

}	/* end table_read */

/*
 ****************************************************************
 *	Identifica o tipo do símbolo				*
 ****************************************************************
 */
int
symbol_type (SYM *sp)
{
	/*
	 *	Identifica tipos incompatíveis com as opções
	 */
	if (gflag  &&  (sp->s_type & EXTERN) == 0)
		return (-1);

	/*
	 *	Classifica o tipo
	 */
	switch (sp->s_type)
	{
	    case UNDEF:
		if (sp->s_value != 0)
		{
			if (cflag)
				return ('c');
		}
		else
		{
			if (uflag)
				return ('u');
		}
		break;

	    case UNDEF|EXTERN:
		if (sp->s_value != 0)
		{
			if (cflag)
				return ('C');
		}
		else
		{
			if (uflag)
				return ('U');
		}
		break;

	    case ABS:
		if (aflag)
			return ('a');
		break;

	    case ABS|EXTERN:
		if (aflag)
			return ('A');
		break;

	    case REG:
		if (rflag)
			return ('r');
		break;

	    case REG|EXTERN:
		if (rflag)
			return ('R');
		break;

	    case TEXT:
		if (tflag)
			return ('t');
		break;

	    case TEXT|EXTERN:
		if (tflag)
			return ('T');
		break;

	    case DATA:
		if (dflag)
			return ('d');
		break;

	    case DATA|EXTERN:
		if (dflag)
			return ('D');
		break;

	    case BSS:
		if (bflag)
			return ('b');
		break;

	    case BSS|EXTERN:
		if (bflag)
			return ('B');
		break;

	    case SFILE:
	    case SFILE|EXTERN:
		if (fflag)
			return ('F');
		break;

	    default:
		return ('?');

	} 	/* end switch (sp->s_type) */

	return (-1);

}	/* end symbol_type */

/*
 ****************************************************************
 *	Função de comparação alfabética crescente		*
 ****************************************************************
 */
int
sym_compare_alpha_up (const void *vp1, const void *vp2)
{
	const SYM	*sp1 = vp1, *sp2 = vp2;

#define	FOLD
#ifdef	FOLD
	return (strttcmp (sp1->s_name, sp2->s_name, cmpfoldtb, cmpafoldtb));
#else
	return (strcmp (sp1->s_name, sp2->s_name));
#endif	FOLD

}	/* end sym_compare_alpha_up */

/*
 ****************************************************************
 *	Função de comparação alfabética decrescente		*
 ****************************************************************
 */
int
sym_compare_alpha_down (const void *vp1, const void *vp2)
{
	const SYM	*sp1 = vp1, *sp2 = vp2;

#ifdef	FOLD
	return (strttcmp (sp2->s_name, sp1->s_name, cmpfoldtb, cmpafoldtb));
#else
	return (strcmp (sp2->s_name, sp1->s_name));
#endif	FOLD

}	/* end sym_compare_alpha_down */

/*
 ****************************************************************
 *	Função de comparação numérica crescente			*
 ****************************************************************
 */
int
sym_compare_num_up (const void *vp1, const void *vp2)
{
	const SYM	*sp1 = vp1, *sp2 = vp2;
	int		diff;

	if ((diff = sp1->s_value - sp2->s_value) != 0)
		return (diff);

#ifdef	FOLD
	return (strttcmp (sp1->s_name, sp2->s_name, cmpfoldtb, cmpafoldtb));
#else
	return (strcmp (sp1->s_name, sp2->s_name));
#endif	FOLD

}	/* end sym_compare_num_up */

/*
 ****************************************************************
 *	Função de comparação numérica decrescente		*
 ****************************************************************
 */
int
sym_compare_num_down (const void *vp1, const void *vp2)
{
	const SYM	*sp1 = vp1, *sp2 = vp2;
	int		diff;

	if ((diff = sp2->s_value - sp1->s_value) != 0)
		return (diff);

#ifdef	FOLD
	return (strttcmp (sp2->s_name, sp1->s_name, cmpfoldtb, cmpafoldtb));
#else
	return (strcmp (sp2->s_name, sp1->s_name));
#endif	FOLD

}	/* end sym_compare_num_down */

/*
 ****************************************************************
 *	Lista os símbolos					*
 ****************************************************************
 */
void
table_print (const char *mod_nm, int n_sym, HEADER *hp)
{
	int	i, sz;
	SYM	*sp;
	SYM	**spp;

	/*
	 *	Ordena os símbolos
	 */
	if   (sflag > 0)	/* Crescente */
	{
		if (nflag)
			qtsort (symtb_vec, n_sym, sym_compare_num_up);
		else
			qtsort (symtb_vec, n_sym, sym_compare_alpha_up);
	}
	elif (sflag < 0)	/* Decrescente */
	{
		if (nflag)
			qtsort (symtb_vec, n_sym, sym_compare_num_down);
		else
			qtsort (symtb_vec, n_sym, sym_compare_alpha_down);
	}

	/*
	 *	Percorre os símbolos, listando-os
	 */
	for
	(	i = n_sym, spp = symtb_vec;
		i > 0;
		i--, spp++
	)
	{
		/*
		 *	Imprime o nome do símbolo
		 */
		sp = *spp;

		if (oflag)
			printf ("%s:\t", mod_nm);

		if (!uflag || flag_sum != 1)
		{
			if (sp->s_type == 'u'  ||  sp->s_type == 'U')
				printf ("         ");
			else
				printf ("%P", sp->s_value);
		}

		/*
		 *	Se for ordenação numérica crescente,
		 *	imprime o tamanho do elemento
		 */
		if (szflag)
		{
			if (i > 1)
			{
				sz = spp[1]->s_value - sp->s_value;
			}
			else
			{
				int		End = 0;

				switch (sp->s_type)
				{
				    case 't':
				    case 'T':
					End = hp->h_tsize;
					break;

				    case 'd':
				    case 'D':
					End = hp->h_dsize + hp->h_tsize;
					break;

				    case 'b':
				    case 'B':
					End = hp->h_bsize +
					      hp->h_dsize + hp->h_tsize;
					break;
				}

				sz = End - sp->s_value;
			}

			/*
			 *	Um teste levemente arbitrário
			 */
			if (sz < 0  ||  sz > 0x100000)
				printf ("        ");
			else
				printf (" %6d ", sz);
		}

		/*
		 *	Imprime o tipo e o nome do símbolo
		 */
		printf ("  %c  %s\n", sp->s_type, sp->s_name);
	}

}	/* end table_print */

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
		"%s - imprime a tabela de símbolos de módulos objeto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-gutdbcarfonepN] [<módulo> ...]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Sem nenhuma das opções \"-utdbcarf\", "
			"imprime todos os símbolos\n"
		"\t-g: Imprime somente os símbolos globais\n"

		"\t-u: Imprime os símbolos indefinidos\n"
		"\t-t: Imprime os símbolos do TEXT\n"
		"\t-d: Imprime os símbolos do DATA\n"
		"\t-b: Imprime os símbolos do BSS\n"
		"\t-c: Imprime os símbolos do COMMON\n"
		"\t-a: Imprime os símbolos absolutos\n"
		"\t-r: Imprime os nomes de registros\n"
		"\t-f: Imprime os nomes de módulos\n"

		"\t-o: Imprime o nome do <módulo> para cada símbolo\n"
		"\t-n: Ordena os símbolos numericamente pelo valor\n"
		"\t    (normalmente os símbolos são ordenados pelo nome)\n"
		"\t-e: Ordena decrescentemente\n"
		"\t-p: Não ordena os símbolos\n"
		"\t-N: Lê os nomes dos <módulo>s de \"stdin\"\n"

	);

	exit (2);

}	/* end help */
