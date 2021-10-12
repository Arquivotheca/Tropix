/*
 ****************************************************************
 *								*
 *			crypt.c					*
 *								*
 *	Codifica/decodifica arquivos			  	*
 *								*
 *	Versão	1.0.0, de 14.10.86				*
 *		3.0.0, de 20.04.97				*
 *								*
 *	Módulo: crypt						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 20.04.97";

entry int	cflag;		/* Pede a complexidade */
entry int	vflag;		/* Verbose */

/*
 ******	Constantes do algoritmo *********************************
 */
#define CPX	48		/* Complexidade padrao */

#define RSZ	256		/* Tamanho do Rotor */
#define M8 	0xFF		/* Mascara para um caracter */

#define	MULT	3183098861	/* Multiplicador para ger. no. aleat. */
#define	INCR	907633385	/* Incremento para ger. no. aleat. */

entry short	gnd, gns;	/* Posição inicial dos rotores */

/*
 ****** Protótipos de funções ***********************************
 */
void		rotor_init (char [], char [], char []);
void		help (void);

/*
 ****************************************************************
 *	Codifica/decodifica arquivos			  	*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	register short	ch, nd, ns;
	char		DIRETO[RSZ];
	char		INVERSO[RSZ];
	char		SIMETRICO[RSZ];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "cvH")) != EOF)
	{
		switch (opt)
		{
		    case 'c':			/* Complexidade */
			cflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	Analisa os argumentos
	 */
	argv += optind;
	argc -= optind;

	switch (argc)
	{
		/*
		 *	Dada a entrada e saída
		 */
	    case 2:
		if (freopen (argv[1], "w", stdout) == NOFILE)
		{
			fprintf
			(	stderr,
				"Não consegui abrir \"%s\" (%s)\n",
				argv[1], strerror (errno)
			);
			exit (1);
		}
		/* continua */

		/*
		 *	Dada apenas a entrada
		 */
	    case 1:
		if (freopen (argv[0], "r", stdin) == NOFILE)
		{
			fprintf
			(	stderr,
				"Não consegui abrir \"%s\" (%s)\n",
				argv[0], strerror (errno)
			);
			exit (1);
		}
		break;


		/*
		 *	Não dada a entrada nem saída
		 */
	    case 0:
		break;

		/*
		 *	Dados argumentos demais
		 */
	    default:
		help ();

	}	/* end switch */

	/*
	 *	Inicializa o ENIGMA
	 */
	rotor_init (DIRETO, INVERSO, SIMETRICO);

	nd = gnd;
	ns = gns;

	/*
	 *	Faz a transformação
	 */
	while ((ch = getchar ()) >= 0)
	{
		ch = DIRETO[(ch + nd) & M8];
		ch = SIMETRICO[(ch + ns) & M8];
		ch = INVERSO[(ch - ns) & M8];
		ch =    (ch - nd) & M8 ;
	
		putchar (ch);

		if (++nd >= RSZ)
		{
			nd = 0;

			if (++ns >= RSZ)
				ns = 0;
		}

	}	/* end while */

	return (0);

}	/* end crypt */

/*
 ****************************************************************
 *	Rotina de Inicialização das Tranformações		*
 ****************************************************************
 */
void
rotor_init (register char DIRETO[], register char INVERSO[], register char SIMETRICO[])
{
	register int	i;
	register char	*ps;
	unsigned short	seed[IDSZ/2];
	short		s[IDSZ];
	long		sd, ss, nd, ns;
	short		id, is;
	short		cx;
	short		aux[RSZ];
	const char	*str;

	/*
	 *	Processa a senha
	 */
	for (i = 0; i < IDSZ; i++)
		s[i] = 0;

	ps = getpass ("Senha: ");

	for (i = 0; i < IDSZ; i++)
	{
		if ((s[i] = *ps & M8) == 0)	 
			break;

		*ps++ = '\0';
	}

	for (i = 0; i < (IDSZ-1); i += 2)
		seed[i/2] = s[i] * s[i+1] + (s[i] << 8) | s[i+1];

	for (i = 0; i < IDSZ; i++)
		s[i] = 0;

	sd = ((seed[0]) << 16) | seed[1];
	ss = ((seed[2]) << 16) | seed[3];
	nd = ((seed[4]) << 16) | seed[5];
	ns = ((seed[5]) << 16) | seed[6];

	for (i = 0; i < IDSZ/2; i++) 	
		seed[i] = 0;

	/*
	 *	Processa a complexidade
	 */
	if (cflag)
	{
		ps = getpass ("Complexidade: ");

		cx = strtol (ps, &str, 0) * 2 - 1;

		if (*str)
			cx = -1;

		for (i = 0; i < IDSZ; i++)
			*ps++ = '\0';
	}
	else
	{
		cx = CPX * 2 - 1;
	}

	if (cx < 0)
	{
		fprintf
		(	stderr,
			"%s: Complexidade com valor incorreto\n\n",
			pgname
		);

		help ();
	}

	/*
	 *	Inicialização do gerador
	 */
	setrand (MULT, INCR, cx);

	/*
	 *	Transformações Direta e Inversa
	 */

	for (i = 0; i < RSZ; i++)
		aux[i] = 0;

	for (i = 0; i < RSZ; i++)
	{
		sd = getrand (sd);

		id = (sd >> 24) & M8;

		while (aux[id] != 0)
		{
			id++;
			if (id >= RSZ)
				id = 0;
		}
	
		DIRETO[i]  = id;
		INVERSO[id] = i;
	
		aux[id] = 1;
	}

	/*
  	 *	Tranformação  Simetrica
	 */
	for (i = 0; i < RSZ; i++)
		aux[i] = 0;

	for (i = 0; i < RSZ; i++)
	{
		if (aux[i] != 0)
			continue;

		aux[i] = 1;

		ss = getrand (ss);

		is = (ss >> 24) & M8;

		while (aux[is] != 0)
		{
			is--;
			if (is <= i)
				is = RSZ - 1;
		}

		SIMETRICO[i]  = is;
		SIMETRICO[is] = i;

		aux[is] = 1;

	}	/* end for */


	/*
 	 *	Posição Inicial dos Rotores
 	 */
	nd = getrand (nd);

	gnd = (nd >> 24) & M8;

	ns = getrand (ns);

	gns = (ns >> 24) & M8;

}	/* end rotor_init */

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
		"%s - codifica/decodifica arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-c] [<entrada> [<saída>]]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-c: Pede a complexidade pelo terminal "
			"(o valor por omissão é %d)\n"
		"\nObs.:\tA senha é pedida pelo terminal. "
		"Se a <entrada>/<saída> não for dada,\n"
		"\tela será lida da entrada padrão/escrita na saída padrão\n",
		CPX
	);

	exit (2);

}	/* end help */
