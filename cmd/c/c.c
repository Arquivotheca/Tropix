/*
 ****************************************************************
 *								*
 *			c.c					*
 *								*
 *	Escreve as linhas de um arquivo em várias colunas	*
 *								*
 *	Versão	3.0.0, de 20.04.97				*
 *		4.3.0, de 12.07.02				*
 *								*
 *	Módulo: c						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <signal.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.3.0, de 12.07.02";

#define	LINE_WIDTH	80	/* Comprimento da linha default */
#define	BLANK_COUNT	2	/* Separação entre colunas default */

#define	elif	else if
#define STR(x)	# x
#define XSTR(x)	STR (x)

const char	*proto = "/tmp/lfXXXXXX";	/* Arquivo temporário */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		on_signal (int, ...);
void		help (void);

/*
 ****************************************************************
 *	Escreve as linhas de um arquivo em várias colunas	*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register FILE		*in_fp = stdin, *out_fp;
	const char		*line_width_str = NOSTR;
	const char		*blank_count_str = NOSTR;
	int			line_width = LINE_WIDTH;
	int			blank_count = BLANK_COUNT;
	int			opt;
	register int		c, n, lw;
	int			biggest, n_a_line, cp;
	const char		*str;
	TERMIO			termio;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "l:b:vH")) != EOF)
	{
		switch (opt)
		{
		    case 'l':			/* Comprimento da linha */
			line_width_str = optarg;
			break;

		    case 'b':			/* No. de brancos separando */
			blank_count_str = optarg;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fprintf (stderr, "\n");
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	if (argc > 1)
		help ();

	/*
	 *	Examina o comprimento da linha
	 */
	if   (line_width_str != NOSTR)
	{
		if ((line_width = strtol (line_width_str, &str, 0)) <= 0 || *str != '\0')
			error ("$Comprimento de linha \"%s\" inválido", line_width_str);
	}
	elif (ioctl (1, TCGETS, &termio) < 0)
	{
		error ("*Não consegui obter o tamanho da linha. Usando %d", line_width);
	}
	else
	{
		line_width = termio.t_ncol;
	}

	/*
	 *	Examina a separação entre os itens
	 */
	if (blank_count_str != NOSTR)
	{
		if ((blank_count = strtol (blank_count_str, &str, 0)) <= 0 || *str != '\0')
			error ("$No. de brancos entre as colunas \"%s\" inválido", blank_count_str);
	}

	/*
	 *	Abre o arquivo de entrada
	 */
	if (argv[0] != NOSTR && (in_fp = fopen (argv[0], "r")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", argv[0]);

	/*
	 *	Abre o arquivo temporário
	 */
	if ((out_fp = fopen (proto = mktemp (proto), "w")) == NOFILE)
		error ("$*Não consegui abrir o temporário \"%s\"", proto);

	/*
	 *	Em caso de sinais, ...
	 */
	if (signal (SIGINT, SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_signal);

	if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
		signal (SIGTERM, on_signal);

	/*
	 *	Le o arquivo de entrada
	 */
	for (lw = biggest = 0; (c = getc (in_fp)) != EOF; putc (c, out_fp))
	{
		if (c == '\n')
		{
			if (lw > biggest)
				biggest = lw;

			lw = 0;
		}
		elif (c == '\t')
		{
			lw = (lw + 8) & ~07;
		}
		else
		{
			lw++;
		}
	}

	fclose (out_fp);

	/*
	 *	Calcula o número de itens por linha
	 */
	n_a_line = (line_width - 1) / (biggest += blank_count);

	if ((in_fp = fopen (proto, "r")) == NOFILE)
		error ("$*Não consegui reabrir o temporário \"%s\"", proto);
	
	/*
	 *	Escreve o arquivo de saída
	 */
	for (lw = cp = 0; (c = getc (in_fp)) != EOF; /* vazio */)
	{
		if (c == '\n')
		{
			if (++cp >= n_a_line)
			{
				putchar ('\n'); cp = 0;
			}
			else
			{
				for (n = lw; n < biggest; n++)
					putchar (' ');
			}

			lw = 0;
		}
		else
		{
			putchar (c);

			if (c =='\t')
				lw = (lw + 8) & ~07;
			else
				lw++;
		}
	}

	if (cp)
		putchar ('\n');

	/*
	 *	Remove o temporário
	 */
	unlink (proto);

	return (0);

}	/* end c */

/*
 ****************************************************************
 *	Em caso de sinais, ...					*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	unlink (proto);
	exit (1);

}	/* end on_signal */

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
		"%s - escreve as linhas de um arquivo em várias colunas\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-l <caracteres>] [-b <brancos>] [<arquivo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-l: Comprimento da linha (normalmente " XSTR (LINE_WIDTH) " caracteres)\n"
		"\t-n: No. de brancos entre as colunas (normalmente " XSTR (BLANK_COUNT) ")\n"
	);

	exit (2);

}	/* end help */

