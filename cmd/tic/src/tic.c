/*
 ****************************************************************
 *								*
 *			tic.c					*
 *								*
 *	Compila e instala a descrição de um terminal/vídeo	*
 *								*
 *	Versão 2.0.0, de 21.04.87				*
 *	       3.0.0, de 12.07.97				*
 *								*
 *	Modulo: tic						*
 *		Utilitarios Basicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <terminfo.h>
#include <errno.h>

#include "../h/scan.h"
#include "../h/error.h"

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
entry const char pgversion[] =  "Versão: 3.0.0, de 12.07.97";

#define	elif	else if

entry int	fflag;		/* Instalação com os nomes corretos */

entry const char *cpnm;		/* Nome de instalação */

entry const char *source;	/* Nome do arquivo de Entrada */

entry FILE	*input;		/* fp da Entrada */

/*
 ****************************************************************
 *	Definição de Dimensões Máximas				*
 ****************************************************************
 */
#define	NAMESZ	256		/* Nomes do Terminal */
#define	STRSZ	512		/* Cada cadeia */
#define	TABSZ	1024		/* Tabela de Cadeias */

/*
 ******	Protótipos de funções ***********************************
 */
void		help (void);

/*
 ****************************************************************
 *	Compila e instala a descrição de um terminal/vídeo	*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	char		cap[16];
	CAPHASH		*hp;
	char		str[STRSZ];
	int		offset, size = 0, opt, g, i;
	INFO		info;

	/*
	 *	Interpretação das Opções e Parametros
	 */
	while ((opt = getopt (argc, argv, "fn:H")) != EOF)
	{
		switch (opt)
		{
		    case 'f':
			if (cpnm)
				help ();
			fflag++;
			break;

		    case 'n':
			if (fflag)
				help ();
			cpnm = optarg;
			break;

		    case 'H':
			help ();

		    default:
			putc ('\n', stderr);
			help ();
		}
	}

	argv += optind;
	argc -= optind;

	if (argc != 1)
		help ();

	/*
	 *	Abertura do Fonte (Legivel)
	 */
	source = argv[0];

	if ((input = fopen (source, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"%s: Não consegui abrir o arquivo de descrição \"%s\" (%s)\n",
			pgname, source, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Alocação de Memória para as Seções
	 */
	if
	(	(info.i_booleans = malloc (BOOLNO * sizeof (char))) == NOSTR ||
		(info.i_names = malloc (NAMESZ)) == NOSTR ||
		(info.i_numbers = malloc (NUMNO * sizeof (short))) == (short *)NULL ||
		(info.i_table = malloc (TABSZ * sizeof (char))) == NOSTR ||
		(info.i_strings = malloc (STRNO * sizeof (char *))) == (char **)NULL
	)
	{
		fprintf
		(	stderr,
			"%s: Memória esgotada\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Atribuição de valores iniciais para as Seções
	 */
	offset = 0;

	info.i_boolno = BOOLNO;
	for (i = 0; i < BOOLNO ; i++)
		info.i_booleans[i] = 0;

	info.i_numno = NUMNO;
	for (i = 0; i < NUMNO ; i++)
		info.i_numbers[i] = -1;

	info.i_strno = STRNO;
	for (i = 0; i < STRNO ; i++)
		info.i_strings[i] = NOSTR;

	/*
	 *	Leitura dos Nomes do Terminal
	 */
	if (!get_names (info.i_names))
		tic_error (FATAL, LMARK, "Erro nos nomes", NOSTR);

	/*
	 *	Leitura e Compilação das Capacidades
	 */
	while ((g = get_cap (cap)))
	{
		/*
		 *	Verifica se houve Erro
		 */
		if (g < 0)
		{
			skip (false);
			continue;
		}

		if ((hp = caphash (cap)) == NOCAPHASH)
			tic_error (ERRO, LMARK, "Capacidade Inexistente", NOSTR);

#undef	DEBUG
#ifdef	DEBUG
		printf
		(	"cap:%s, hp->h_id:%s, hp->h_class:%d, hp->h_order:%d\n",
			cap, hp->h_id, hp->h_class, hp->h_order
		);
#endif	DEBUG

		switch (hp->h_class)
		{
		    case C_BOOLEAN:
			info.i_booleans[hp->h_order] = 1;
			skip (true);
			break;

		    case C_NUMBER:
			info.i_numbers[hp->h_order] = get_number ();
			break;

		    case C_STRING:
			if (get_string (str))
			{
				/*
				 *	Acrescenta str na Tabela de Strings
				 */
				if ((size = offset + strlen (str) + 1) >= TABSZ)
				{
					fprintf
					(	stderr,
						"%s: Número excessivo de cadeias\n",
						pgname
					);
					exit (1);
				}

				info.i_strings[hp->h_order] = info.i_table + offset;
				strcpy (info.i_table + offset, str);
				offset = size;
			}
			break;

		    case C_PSTRING:
			if (get_string (str))
			{
				char	*s;

				/*
				 *	Compilação da String com Parâmetros
				 */
				if ((s = parmcomp (str)) == NOSTR)
				{
					tic_error
					(	ERRO, LMARK,
						"Cadeia paramétrica incorreta",
						NOSTR
					);
					break;
				}

				/*
				 *	Acrescenta s na Tabela de Strings
				 */
				if ((size = offset + strlen (s) + 1) >= TABSZ)
				{
					fprintf
					(	stderr,
						"%s: Número excessivo de cadeias\n",
						pgname
					);
					exit (1);
				}

				info.i_strings[hp->h_order] = info.i_table + offset;
				strcpy (info.i_table + offset, s);
				offset = size;
			}
			break;

		    case C_UNDEF:
			tic_error	
			(	ERRO, LMARK,
				"Capacidade desconhecida: \"%s\"", cap
			);
			skip (false);
			break;

		    default:
			{
				fprintf
				(	stderr,
					"%s: Classe \"%s\" indefinida\n",
					pgname, cap
				);
			}

		}	/* switch (hp->h_class) */

	}	/* while get_cap */

	/*
	 *	
	 */
	info.i_tabsz = size;
	realloc (info.i_table, size);
	realloc (info.i_names, strlen (info.i_names) + 1);

	/*
	 *	Encerra Leitura
	 */
	if (ferror (input))
		tic_error (FATAL, NOMARK, "Erro na leitura de \"%s\"", source);

	fclose (input);

	/*
	 *	Escreve Informações
	 */
	if   (fflag)
		cpnm = NOSTR;
	elif (cpnm == NOSTR)
		cpnm = "test";

	if (!putinfo (&info, (char *)cpnm))
	{
		fprintf
		(	stderr,
			"%s: Não consegui escrever o arquivo objeto \"%s\" (%s)\n",
			pgname, cpnm, strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end tic */

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
		"%s - compila e instala a descrição de um terminal/vídeo\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-f] [-n <nome>] <fonte>\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-f: Instala com os nomes definitivos\n"
		"\t-n: Instala com o nome \"<nome>\"\n"
		"\nObs.:\tSe não for dada nenhuma opção, "
		 " será instalado com o nome \"test\"\n"
	);

	exit (2);

}	/* end help */
