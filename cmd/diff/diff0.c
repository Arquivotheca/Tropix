/*
 ****************************************************************
 *								*
 *			diff0.c					*
 *								*
 *	Comparador diferencial de dois arquivos			*
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
#include <string.h>
#include <time.h>
#include <errno.h>

#include "diff.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 24.04.97";

entry FUNCTION	function = NORMAL; /* Função escolhida */

entry int	bflag;		/* Ignore brancos nas comparacões */
entry int	vflag;		/* Verbose */
entry int	tflag;		/* Time */
entry int	dflag;		/* Debug */

entry const char *ifdefname;	/* Nome do ifdef para a função IFDEF */

entry int	context = 2;	/* Tamanho do contexto para a função CONTEXT */

entry int	status = 2;	/* Código de retorno do programa */

entry int	anychange;	/* Houve alguma diferença entre os arquivos */

/*
 ******	Variáveis dos arquivos e estruturas de dados ************
 */
entry const char *oldnm,		/* Nome dos arquivos */
		*newnm;

entry STAT	olds, 		/* Estado dos arquivos */
 		news;

entry char	*oldaddr, 	/* Endereço dos arquivos */
		*newaddr;

entry PLINE	*oldpline,	/* Endereços dos PLINEs dos arquivos */
		*newpline;

entry int	oldlineno,	/* No. de linhas dos arquivos */
		newlineno;

entry int	prefixsz,	/* Tamanho dos prefixo e sufixo */
		sufixsz;

entry PLINE	*oldshortpline,	/* Endereços dos PLINEs sem prefixo/sufixo */
		*newshortpline;

entry int	oldshortlineno,	/* No. de linhas dos arquivos sem pref/suf */
		newshortlineno;

entry PLINE	**oldsortvec,	/* Ponteiro para o vetor ordenado */
		**newsortvec;

/*
 ****************************************************************
 *	Compara dois arquivos					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt, i;
	const char	*ptr;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "efci:C:bvtdH")) != EOF)
	{
		switch (opt)
		{
		    case 'e':
			if (function != NORMAL)
				help ();
			function = EDIT;
			break;

		    case 'f':
			if (function != NORMAL)
				help ();
			function = REVEDIT;
			break;

		    case 'c':
			if (function != NORMAL)
				help ();
			function = CONTEXT;
			break;

		    case 'i':
			if (function != NORMAL)
				help ();
			function = IFDEF;
			ifdefname = optarg;
			break;

		    case 'C':
			if (function != NORMAL && function != CONTEXT)
				help ();
			function = CONTEXT;

			if ((i = strtol (optarg, &ptr, 0)) <= 0 || ptr[0])
			{
				fprintf
				(	stderr,
					"%s: contexto inválido: \"%s\", "
					"usando contexto = %d\n\n",
					pgname, optarg, context
				);
			}
			else
			{
				context = i;
			}
			break;

		    case 'b':			/* Ignore brancos nas cmp */
			bflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 't':			/* Time */
			tflag++;
			break;

		    case 'd':			/* Debug */
			dflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fputc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Pequena consistência
	 */
	if (argc != 2)
	{
		fprintf
		(	stderr,
			"%s: Devem ser dados dois <arquivo>s\n\n",
			pgname
		);
		help ();
	}

	oldnm = argv[0];
	newnm = argv[1];

	diffcmp ();

	done ();

}	/* end diff */

/*
 ****************************************************************
 *	Analisa os arquivos e compara				*
 ****************************************************************
 */
void
diffcmp (void)
{
	char		*oldcp, *newcp;
	int		i;
	int		copy = 0;
	char		clock_line[128];

	/*
	 *	Examina o primeiro arquivo
	 */
	if (streq (oldnm, "-"))
	{
		if (streq (newnm, "-"))
		{
			fprintf
			(	stderr,
				"%s: Apenas um dos <arquivo>s pode ser lido "
				"da entrada padrão\n\n",
				pgname
			);
			help ();
		}

		oldnm = copytemp ();
		copy++;
	}

	if (stat (oldnm, &olds) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, oldnm, strerror (errno)
		);
		done ();
	}

	/*
	 *	Examina o segundo arquivo
	 */
	if (streq (newnm, "-"))
	{
		newnm = copytemp ();
		copy++;
	}

	if (stat (newnm, &news) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui obter o estado de \"%s\" (%s)\n",
			pgname, newnm, strerror (errno)
		);
		done ();
	}

	/*
	 *	Acerta o nome do arquivo (de acordo com o diretório)
	 */
	if   (S_ISDIR (news.st_mode))
	{
		if (S_ISDIR (olds.st_mode))
		{
			fprintf
			(	stderr,
				"%s: Não podemos especificar 2 diretórios\n\n",
				pgname
			);
			help ();
		}

		newnm = dirfilenm (newnm, oldnm, copy);

		if (stat (newnm, &news) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o estado de \"%s\" (%s)\n",
				pgname, newnm, strerror (errno)
			);
			done ();
		}
	}
	elif (S_ISDIR (olds.st_mode))
	{
		oldnm = dirfilenm (oldnm, newnm, copy);

		if (stat (oldnm, &olds) < 0)
		{
			fprintf
			(	stderr,
				"%s: Não consegui obter o estado de \"%s\" (%s)\n",
				pgname, oldnm, strerror (errno)
			);
			done ();
		}
	}

	/*
	 *   Não esquece de verificar se se trata na realidade do mesmo arquivo
	 */
	if (olds.st_dev == news.st_dev  &&  olds.st_ino == news.st_ino)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" e \"%s\" são o mesmo arquivo\n",
			pgname, oldnm, newnm
		);
		done ();
	}

	/*
	 *	Lê os arquivos
	 */
#define	TIME
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	readfile (oldnm, &olds, &oldaddr); 
	readfile (newnm, &news, &newaddr); 

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Leitura dos arquivos:\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

	/*
	 *	Certifica-se de que os arquivos não são iguais
	 */
	if (olds.st_size == news.st_size)
	{
		oldcp = oldaddr; newcp = newaddr;

		for (i = 0; /* teste abaixo */; i++)
		{
			if (i >= olds.st_size)
				{ status = 0; done (); }

			if (*oldcp++ != *newcp++)
				break;
		}
	}

	/*
	 *	Começa a comparação
	 */
	status = 1;

#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	mkfiledata (oldaddr, &oldpline, &oldlineno);
	mkfiledata (newaddr, &newpline, &newlineno);

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Estrutura de dados:\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#undef	DEBUG
#ifdef	DEBUG
	if (dflag)
	{
		PLINE	*lp;

		printf ("\nArquivo \"%s\", %d linhas\n\n", oldnm, oldlineno);

		for (lp = oldpline + 1; lp->l_text; lp++)
		{
			printf
			(	"No. = %3d, Hash = %8d, text = ",
				lp - oldpline, lp->l_hash
			); 
			printline (lp->l_text, 30);
		}

		printf ("\nArquivo \"%s\", %d linhas\n\n", newnm, newlineno);

		for (lp = newpline + 1; lp->l_text; lp++)
		{
			printf
			(	"No. = %3d, Hash = %8d, text = ",
				lp - newpline, lp->l_hash
			); 
			printline (lp->l_text, 30);
		}
	}
#endif	DEBUG

	/*
	 *	Procura prefixos e sufixos comuns
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	getprefixsufix ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Prefixo/sufixo:\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#ifdef	DEBUG
	if (dflag)
	{
		printf ("\nPrefixo = %d, Sufixo = %d\n", prefixsz, sufixsz);
		printf
		(	"oldshortlineno = %d, newshortlineno = %d\n",
			 oldshortlineno,      newshortlineno
		);
	}
#endif	DEBUG

	/*
	 *	Ordena pelo "hash"
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	sort (oldshortpline, oldshortlineno, &oldsortvec);
	sort (newshortpline, newshortlineno, &newsortvec);

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Ordenação:\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#undef	DEBUG
#ifdef	DEBUG
	if (dflag)
	{
		PLINE	**lpp, *lp;

		printf ("\nArquivo \"%s\", %d linhas\n\n", oldnm, oldshortlineno);

		for (lpp = oldsortvec, i = 0; i < oldshortlineno; lpp++, i++)
		{
			lp = *lpp;

			printf
			(	"No. = %3d, Hash = %8d, text = ",
				lp->l_serial, lp->l_hash
			); 
			printline (lp->l_text, 30);
		}

		printf ("\nArquivo \"%s\", %d linhas\n\n", newnm, newshortlineno);

		for (lpp = newsortvec, i = 0; i < newshortlineno; lpp++, i++)
		{
			lp = *lpp;

			printf
			(	"No. = %3d, Hash = %8d, text = ",
				lp->l_serial, lp->l_hash
			); 
			printline (lp->l_text, 30);
		}
		printf ("\n");
	}
#endif	DEBUG

	/*
	 *	Procura as linhas correspondentes
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	correspondence ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Correspondência:\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#ifdef	DEBUG
	if (dflag)
	{
		for (i = 1; i <= newshortlineno; i++)
		{
			printf ("i = %2d, member = %8d, text = ", i, member[i]);
			if (member[i] > 0)
				printline (newshortpline[member[i]].l_text, 30);
			else
				printline (newshortpline[-member[i]].l_text, 30);
		}
		printf ("\n");
	}
#endif	DEBUG

	/*
	 *	Parte superinteligente do algoritmo
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	stonehenge ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Stonehenge:\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#ifdef	DEBUG
	if (dflag)
	{
		CAND	*cp, **cpp;

		printf ("stonehenge retornou %d\n\n", lastklist - klist);

		for (cpp = klist + 1; cpp <= lastklist; cpp++)
			printf ("%2d: klist = %8d\n", cpp-klist, cpp[0]-clist);
		printf ("\n");

		for (cp = clist + 1; /* vazio */; cp++)
		{
			if (cp->c_oldlineno == 0)
				break;
			printf
			(	"%2d: clist = (%d,%d,%d)\n",
				cp-clist, cp->c_oldlineno,
				cp->c_newlineno, cp->c_pred - clist
			);
		}

		printf ("\n");
	}
#endif	DEBUG

	/*
	 *	Compõe
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	compose ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Compose:\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

#ifdef	DEBUG
	if (dflag)
	{
		int	j;

		for (j = 1; j <= oldlineno; j++)
			printf ("%2d: J = %d\n", j, J[j]);
		printf ("\n");
	}
#endif	DEBUG

	/*
	 *	Verifica se não houve colisão no HASH
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	check ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Check:\t\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

	/*
	 *	Saída dos resultados
	 */
#ifdef	TIME
	if (tflag)
		clockstamp ();
#endif	TIME

	output ();

#ifdef	TIME
	if (tflag)
		fprintf (stderr, "Output:\t\t\t%s\n", clocktostr (clock_line)+24);
#endif	TIME

	status = anychange;

}	/* end  diffcmp */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help ()
{
	fprintf
	(	stderr,
		"%s - comparador diferencial de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-efc] [-i <nm>] [-C <n>] [-b] <arquivo1> <arquivo2>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Gera uma lista de diferenças entre os <arquivo>s\n"
		"\t-e: Gera uma seqüência de comandos para o \"ed\"\n"
		"\t-f: Idem, ordem reversa\n"
		"\t-c: Gera um lista de diferenças com contexto\n"
		"\t-i: Gera um arquivo com \"ifdef\"s <nm>\n"
		"\t-C: O contexto deve ser de <n> ao invés de %d linhas\n"
		"\t-b: Não considera diferenças de brancos/espaços\n",
		context
	);

	done ();

}	/* end help */
