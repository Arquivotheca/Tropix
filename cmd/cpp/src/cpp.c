/*
 ****************************************************************
 *								*
 *			cpp.c					*
 *								*
 *	Préprocessador de fontes em "C"				*
 *								*
 *	Versão	2.1.0, de 19.09.88				*
 *		4.6.0, de 27.08.04				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*	
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../h/cpp.h"
#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"

/*
 ****************************************************************
 *	Variáveis e declarações globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.6.0, de 27.08.04";

#define	INCLSZ	40

entry FILE	*fpout;		/* Arquivo a ser Gerado */

const		char *srcname,	/* Nome do Arquivo Fonte */
		*dstname;	/* Nome do Arquivo Destino */

entry int	Cflag,		/* Mantém os Comentários */
		Pflag,		/* Não coloca controles na saída */
		Tflag,		/* Mais diagnósticos */
		vflag,		/* Modo Verboso */
		dflag;		/* Debug */

const char	*incld[INCLSZ + 1],	/* Diretórios para procura de includes */
		**incldp = incld + 1;

const char	**color_vector; /* Para as cores dos modos */

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		opt;

	while ((opt = getopt (argc, argv, "PCvD:U:I:TdMH")) != EOF)
	{
		switch (opt)
		{
		    case 'P':
			Pflag++;
			break;

		    case 'C':
			Cflag++;
			break;

		    case 'v':
			vflag++;
			break;

		    case 'D':
			if (CATEG (optarg) != LETTER)
			{
			    inv_mac:
				fprintf
				(	stderr,
					"Nome de macro inválido: \"%s\"\n\n",
					optarg
				);
				help ();
			}
			mkpredef (optarg, S_PREDEF);
			break;

		    case 'U':
			if (CATEG (optarg) != LETTER)
				goto inv_mac;
			mkpreundef (optarg);
			break;

		    case 'I':
			if (incldp >= &incld[INCLSZ])
			{
				fprintf	(stderr, "Excesso de Opções \"-I\"\n\n");
				help ();
			}
			
			*incldp++ = optarg;
			break;

		    case 'T':			/* LINT */
			Tflag++;
			break;

		    case 'd':
			dflag++;
			break;

		    case 'M':
			exit (0);

		    case 'H':
			help ();

		    default:
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	/*
	 *	Ajusta os ponteiros
	 */
	*incldp++ = "/usr/include";
	*incldp++ = "/usr/xwin/include";
	*incldp++ = "/usr/xwin/include/X11";
	*incldp   = NOSTR;

	argv += optind;

#undef	DEBUG
#ifdef	DEBUG
	/*
	 *	Testa a tabela de símbolos
	 */
    {
	SYMTB		*sp, *tst_sp, **hp, **link_place;
	int		n = 0;

	for (hp = &hashtb[0]; hp < &hashtb[HASHSZ]; hp++)
	{
		for (sp = *hp; sp != NOSYMTB; sp = sp->s_link)
		{
			tst_sp = hash (sp->s_name, &link_place);

			if   (tst_sp == NOSYMTB)
				{ error ("Símbolo \"%s\" não está na SYMTB", sp->s_name); n++; }
			elif (tst_sp != sp)
				{ error ("Símbolo \"%s\" está no local errado da SYMTB", sp->s_name); n++; }
			else
				error ("Símbolo \"%s\" está no local correto da SYMTB", sp->s_name);
		}
	}

	if (n == 0)
		error ("Tabela de símbolos correta");
    }
#endif	DEBUG

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Prepara o caso de erro
	 */
	error_default_function = quit;

	/*
	 *	Obtém o arquivo de entada
	 */
	if (*argv == NOSTR)
	{
		error ("Falta especificar o arquivo de <entrada>\n");
		help ();
	}

	srcname = *argv++;

	if (readfile (srcname) < 0)
		exit (1);

	/*
	 *	Prepara o arquivo de saída
	 */
	if   (*argv == NOSTR)
		fpout = stdout;
	elif ((fpout = fopen (dstname = *argv++, "w")) == NOFILE)
		error ("$*Não consegui criar \"%s\"", dstname);

	/*
	 *	Verifica se tem argumentos supérfluos
	 */
	if (*argv)
		error ("O argumento supérfluo \"%s\" será ignorado", *argv);

	/*
	 *	Passa à execução
	 */
	signal (SIGINT,  on_signal);
	signal (SIGQUIT, on_signal);

	parsefile ();

	/*
	 *	Se for verboso, ...
	 */
	if (vflag)
	{
		KPSZ		kpsz;

		/* Imprime a tabela de símbolos */

		printsymtb ();

		/* Imprime a Memória ocupada */

		kcntl (K_GETPSZ, &kpsz);

		printf
		(
			"Texto = %d, Data = %d, Bss = (%d + %d), "
			"Stack = %d, Total = %d\n",
			(int)&etext - 0x400000,
			(int)&edata - 0x20000000,
			(int)&end - (int)&edata,
			(int)_cbreak - (int)&end,
			kpsz.k_ssize,
			(int)&etext - 0x400000 + (int)_cbreak - 0x20000000 + kpsz.k_ssize
		);
	}

	quit (nerrors);

}	/* end main */

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
		"%s - préprocessador para fontes em \"C\"\n"
		"\n%s\n"
		"\nSintaxe:\n"
		" %s [-PCTv] [-D <def> ...] [-U <id> ...] "
			"[-I <dir> ...] <entrada> [<saída>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-P: Não coloca as linhas de controles na saída\n"
		"\t-C: Mantém os comentários na saída\n"
		"\t-T: Emite mais diagnósticos\n"
		"\t-v: Verboso\n"
		"\t-D: Predefine macros "
			"(<def> é da forma \"id\" ou \"id=valor\")\n"
		"\t-U: Não permite macros com o nome <id>\n"
		"\t-I: Acrescenta <dir> à lista de busca de includes\n"
	);

	exit (2);

}	/* end help */

/*
 ****************************************************************
 *	Abandona o Programa					*
 ****************************************************************
 */
void
quit (int n)
{
	if (n != 0 && fpout != NOFILE && fpout != stdout)
		unlink (dstname);

	exit (n);

}	/* end quit */

/*
 ****************************************************************
 *	Processa Sinais						*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	quit (2);

}	/* end on_signal */
