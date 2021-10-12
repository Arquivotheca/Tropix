/*
 ****************************************************************
 *								*
 *			ld.c					*
 *								*
 *	Link-editor de módulos objeto				*
 *								*
 *	Versão	1.0.0, de 08.01.87				*
 *		4.3.0, de 12.07.02				*
 *								*
 *	Módulo: ld						*
 *		Utilitários básicos				*
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
#include <signal.h>
#include <a.out.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão:	4.3.0, de 12.07.02";

/*
 ****** Opções de link-edição ***********************************
 */
entry int		rflag,		/* Mantem a relocação, não define common */
			bflag,		/* Gera uma biblioteca compartilhada */
			nflag,		/* módulo executavel reentrante */
			sflag,		/* "Strip" - Remove SYMTB, RT e RD */
			vflag,		/* Verbose */
			mflag,		/* Gera mapa de link-edição */
			uflag,		/* lista os não referenciados */
			dotflag;	/* Temporárias no diretório corrente */

/*
 ****** Variáveis globais ***************************************
 */
entry int		shared_lib_given; /* Foram dadas bibliotecas compartilhadas */

entry const char	*obj_nm = "a.out"; /* Nome do módulo final */

entry const char	*entry_nm;	/* Nome do símbolo de "Entry-Point" */

entry off_t		TEXT_ORG = -1,	/* Endereco inicial do "TEXT" */
			DATA_ORG = -1,	/* Endereco inicial do "DATA" */
			COMMON_ORG,	/* Endereco inicial do "COMMON" */
			BSS_ORG;	/* Endereco inicial do "BSS" */

/*
 ******	Protótipos de funções ***********************************
 */
void			on_signal (int, ...);
void	 		quit (int);
void			help (void);

/*
 ****************************************************************
 *	Link-editor de módulos objeto				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*str;

	/*
	 *	Analisa a linha de argumentos para o programa
	 *
	 *	ld [-rbsmnvu.] [-o <obj>] [-e <ent>] [-T <tb>] [-D <db>] <m> ... [-l<x> ...]
	 *
	 */
	while ((opt = getopt (argc, argv, "rbsmnvu.o:e:T:D:MH")) != EOF)
	{
		switch (opt)
		{
		    case 'r':			/* Mantenha a relocação */
			rflag++;
			break;

		    case 'b':			/* Gera uma biblioteca compartilhada */
			bflag++;
			break;

		    case 's':			/* Sem tabela de símbolos */
			sflag++;
			break;

		    case 'm':			/* Imprime o mapa de módulos */
			mflag++;
			break;

		    case 'n':			/* Programa reentrante */
			nflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'u':			/* Lista os não referenciados */
			uflag++;
			break;

		    case '.':			/* Temporárias no diretório corrente */
			dotflag++;
			break;

		    case 'o':			/* Nome do objeto gerado */
			obj_nm = optarg;
			break;

		    case 'e':			/* Nome do ponto de entrada */
			entry_nm = optarg;
			break;

		    case 'T':			/* Origem da seção TEXT */
			TEXT_ORG = strtol (optarg, &str, 0);

			if (*str != '\0')
				error ("@Origem da seção TEXT \"%s\" inválida", optarg);

			break;

		    case 'D':			/* Origem da seção DATA */
			DATA_ORG = strtol (optarg, &str, 0);

			if (*str != '\0')
				error ("@Origem da seção DATA \"%s\" inválida", optarg);

			break;

		    case 'M':
			exit(0);

		    case 'H':		/* Help */
			help ();

		    default:		/* Erro */
			fprintf (stderr, "\n");
			help ();

		}	/* end switch */

	}	/* end while */

	if (nflag + rflag + bflag > 1)
		error ("@Opções conflitantes: \"-n\", \"-r\" e \"-b\" são mutuamente exclusivas");

	if ((rflag + bflag) && sflag)
		error ("@Opções conflitantes: \"-r\" ou \"-b\" com \"-s\"");

	if (error_standard_count)
		quit (1);

   /***	argc -= optind; ***/
	argv += optind;

	if (*argv == NOSTR)
		return (0);

	/*
	 *	Programa os sinais INT e QUIT
	 */
	if (signal (SIGINT,  SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_signal);

	if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
		signal (SIGQUIT, on_signal);

	/*
	 *	Prepara "error"
	 */
	error_default_function = quit;

	/*
	 ******	Executa os diversas passos ******************************
	 */

	/*
	 *	Passo 1: Monta a Tabela Global de Símbolos
	 */
	pass1 (argv);

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 2: Verifica os módulos necessários das bibliotecas regulares
	 */
	pass2 ();

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 3: Atribui valores definitivos aos símbolos
	 */
	pass3 ();

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 4: Prepara os arquivos para a geração
	 */
	pass4 ();

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 5: Gera as diversas seções
	 */
	pass5 ();

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 6: Verifica os símbolos das bibliotecas compartilhadas
	 */
	pass6 ();

	if (error_standard_count)
		quit (1);

	/*
	 *	Passo 7: Coda: Escreve as diversas seções e o cabeçalho
	 */
	pass7 ();

	if (error_standard_count)
		quit (1);

#ifdef	COLISÃO
	colision_tst ();
#endif	COLISÃO

	return (0);

}	/* end ld */

/*
 ****************************************************************
 *	Em caso de sinal, ...					*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	quit (1);

}	/* end on_signal */

/*
 ****************************************************************
 *	Termina o Programa Anormalmente				*
 ****************************************************************
 */
void
quit (int n)
{
	if (aout_fd >= 0)
		unlink (obj_nm);

   /***	unlink (text_nm); ***/
   /***	unlink (data_nm); ***/

	if (rt_nm)
		unlink (rt_nm);

	if (rd_nm)
		unlink (rd_nm);

	exit (n);

}	/* end quit */

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
		"%s - link-editor de módulos objeto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"  %s [-rbsmnvu.] [-o <obj>] [-e <ent>] "
		"[-T <tb>] [-D <db>] <m> ... [-l<x> ...]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-r: Mantem a relocação, não define COMMON\n"
		"\t-b: Gera uma biblioteca compartilhada\n"
		"\t-s: Gera o módulo objeto sem tabela de símbolos\n"
		"\t-m: Gera um mapa de link-edição\n"
		"\t-n: Gera o módulo objeto reentrante\n"
		"\t-v: Verboso\n"
		"\t-u: Lista os símbolos não referenciados\n"
		"\t    Se for dado duas vezes, lista também os "
			"das bibliotecas\n"
		"\t-.: Usa os arquivos temporários no diretório corrente\n"
	);

	fprintf
	(	stderr,
		"\t-o: O nome do módulo gerado será <obj> ao invés de \"a.out\"\n"
		"\t-e: O nome do símbolo de entrada será <ent> ao invés de \"start\"\n"
		"\t-T: O endereço do início da seção \"text\" será <tb>\n"
		"\t-D: O endereço do início da seção \"data\" será <db>\n"
		"\nObs.:"
		"\t<m> ... são módulos objeto\n"
		"\t<x> ... são abreviaturas para nomes de bibliotecas\n"
	);

	exit (2);

}	/* end help */
