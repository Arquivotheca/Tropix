/*
 ****************************************************************
 *								*
 *			gar.c					*
 *								*
 *	General Archiver -- utilitário para fitas/disquetes	*
 *								*
 *	Versão	1.1.0, de 11.01.88				*
 *		4.9.0, de 20.07.06				*
 *								*
 *	Módulo:	GAR						*
 *		utilitários básicos				*
 *		Categoria A					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "../h/common.h"
#include "../h/gar.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.9.0, de 20.07.06";

const char	headerversion[] =  "19.03.03";

#define	NPAT	20		/* No. maximo de Padrões */

#define	COL_AREA_RAW	(20 * KBSZ)	/* Tamanho padrão da área de entrada */
#define	COL_AREA_STD	 (4 * KBSZ)	/* Tamanho padrão da área de entrada */

/*
 ****** Indicadores *********************************************
 */
entry FUNC		func  = NOFTN;	/* Função */

entry void		(*function) ();	/* Função a executar para cada bloco */

entry MODIF		modif = NOMODIF; /* Modificador */

entry int		Tflag;		/* Gera o formato TAR */
entry int		Dflag = 1;	/* Comprime (ZIP) */
entry int		nflag;		/* Interativo */
entry int		uflag;		/* Comparação com atualização */
entry int		rflag;		/* Sempre cheque os CRCs */
entry int		vflag;		/* Verbose */
entry int		dflag;		/* Diretório corrente/Não inclui diretórios na coleção */
entry int		fflag;		/* Copia mesmo que já exista */
entry int		mflag;		/* Restaura dono, grupo e datas */
entry int		gflag;		/* Processa o "garfield" */
entry int		dotflag;	/* Não ignora ".old" etc ... */
entry int		crcflag;	/* Não gera CRC na compressão */
entry int		Gflag;		/* Debug */
entry int		Hflag;		/* Help flag */

entry const char	*inc_pat[NPAT+1];	/* Os Padrões */
entry int		inc_pati;

entry const char	*exc_pat[NPAT+1]; /* Os Padrões de exclusão */
entry int		exc_pati;

entry int		exit_code = 0;	/* Código de retorno */

/*
 ******	Área de Entrada/Saida ***********************************
 */
entry BUF		*b;		/* Continuação + leitura/escrita da coleção */

entry int		col_area_sz;	/* Tamanho do buffer em vigor (blocagem) */

entry int		col_area_avail_sz; /* No. de bytes disponiveis no buffer */

entry int		col_is_a_tape;	/* A coleção é uma fita */

/*
 ****** Variáveis diversas **************************************
 */
entry KHEAD		khead;		/* Cabeçalho interno */

entry const char	*g_filenm;	/* Nome do Arquivo em Processamento */
entry int		g_filenmput;	/* O Nome do Arquivo ja foi impresso */
entry int		g_nlput;	/* O <nl> já foi escrito */

entry time_t		date;		/* Data de criação da Coleção GAR */

entry int		vol;		/* No. do Volume GAR */

entry FILE		*ttyfp;		/* Para ler do terminal */

entry TYPE		type;		/* Tipo da Fita */

entry const char	**color_vector;	/* Para as cores dos modos */

/*
 ****** Garfield ************************************************
 */
const char		garfield[] = "garfield";
entry FILE		*gfp;

/*
 ****** Coleção *************************************************
 */
entry int		device_code = 0; /* Código do dispositivo */
const char		*device_nm = NOSTR; /* Nome do Dispositivo */

entry int		colfd;		/* File Descriptor da Coleção */
entry dev_t		coldev;		/* Dispositivo da Coleção */
entry ino_t		colino;		/* Inode da Coleção */

/*
 ******	Cadeias magicas do GAR	*********************************
 */
const char		garvolume[GARMSZ+1]	= "GAR VOLUME  ";
const char		garheader[GARMSZ+1]	= "GAR HEADER  ";
const char		garlink[GARMSZ+1]	= "GAR LINK    ";
const char		gartrailer[GARMSZ+1]	= "GAR TRAILER ";

/*
 ******	Protótipos de funções ***********************************
 */
void			put_s_option (void);
int			put_p_option (const char *, const char *[], int *);

/*
 ****************************************************************
 *	Ponto de Entrada					*
 ****************************************************************
 */
void
main (int argc, const char *argv[])
{
	int		i, opt;
	const char	*str;
	const char	*bl_str = NOSTR;
	const char	*level_str = NOSTR;
	STAT		dev_s;		/* Bloco para stat */

	/*
	 *	Pequena inicialização
	 */
	error_default_function = quit;

	/*
	 *	Analisa as Opções
	 */
	while ((opt = getopt (argc, argv, "ioaxtceF:TDl:rB:sp:P:nuvdfmg.0123GMH")) != EOF)
	{
		switch (opt)
		{
		    case 'i':			/* Entrada */
			if (func != NOFTN)
				help ();

			func = IN;
			break;

		    case 'o':			/* Saida */
			if (func != NOFTN)
				help ();

			func = OUT;
			break;

		    case 'a':			/* Append */
			if (func != NOFTN)
				help ();

			func = OUT; modif = APPEND;
			break;

		    case 'x':			/* Extração */
			if (func != IN)
				help ();

			if (modif != NOMODIF)
				help ();

			modif = CREATE;
			break;

		    case 't':			/* Tabela */
			if (func != IN)
				help ();

			if (modif != NOMODIF)
				help ();

			modif = TAB;
			break;

		    case 'c':			/* Comparação */
			if (func != IN)
				help ();

			if (modif != NOMODIF)
				help ();

			modif = COMP;
			break;

		    case 'e':			/* Pipe */
			if (func != IN)
				help ();

			if (modif != NOMODIF)
				help ();

			modif = PIPE;
			break;

		    case 'F':			/* Dispositivo da coleção */
			if (device_code || device_nm)
				help ();

			device_nm = optarg;
			break;

		    case 'T':			/* Gere o formato TAR */
			Tflag++;
			break;

		    case 'D':			/* NÃO Comprime */
			Dflag = 0;
			break;

		    case 'l':			/* Nível de compressão */
			level_str = optarg;
			break;

		    case 'r':			/* Sempre cheque os CRCs */
			rflag++;
			break;

		    case 'B':			/* Blocagem */
			bl_str = optarg;
			break;

		    case 's':			/* Padrões = 	*.[cshryv] e */
			put_s_option ();
			break;

		    case 'p':		/* Padrões de inclusão */
			put_p_option (optarg, inc_pat, &inc_pati);
			break;

		    case 'P':		/* Padrões de exclusão */
			put_p_option (optarg, exc_pat, &exc_pati);
			break;

		    case 'n':			/* Interativo */
			nflag++;
			break;

		    case 'u':			/* Comparação com atualização */
			if (func != IN)
				help ();

			if (modif == NOMODIF)
				modif = COMP;

			if (modif != COMP)
				help ();

			uflag++;
			break;

		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'd':			/* Diretorio Corrente */
			switch (func)
			{
			    case IN:
				if (modif != CREATE && modif != COMP)
					help ();
				break;

			    case OUT:
				break;

			    default:
				help ();
			}

			dflag = 1;
			break;

		    case 'f':			/* Force */
#if (0)	/*******************************************************/
			if (func != IN)
				help ();

			if (modif != CREATE && modif != COMP)
				help ();
#endif	/*******************************************************/

			fflag++;
			break;

		    case 'm':			/* Restaura dono, etc... */
			if (func != IN)
				help ();

			if (modif != CREATE && modif != COMP)
				help ();
			mflag++;
			break;

		    case 'g':			/* Processa "garfield" */
			if (func == IN && modif != CREATE && modif != COMP)
				help ();

			gflag++;
			break;

		    case '.':			/* Não ignora ".old" etc ... */
			if (func != OUT)
				help ();

			dotflag++;
			break;

		    case '0':
		    case '1':
		    case '2':
		    case '3':
#if (0)	/****************************************************/
		    case '4':
		    case '5':
		    case '8':
		    case '9':
#endif	/****************************************************/
			if (device_code || device_nm)
				help ();

			device_code = opt;
			break;

		    case 'G':			/* Debug */
			Gflag++;
			break;

		    case 'M':			/* Texto residente */
			exit (0);

		    case 'H':			/* help */
			Hflag++;
			break;

		    default:			/* erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end getopt */

	if (Hflag)
		help ();

	argv += optind;

	/*
	 *	Faz uma consistencia dos Argumentos
	 */
	if (func == NOFTN)
		{ error ("É necessário dar \"-i\", \"-o\" ou \"-a\"\n"); help (); }

	if (func == IN && modif == NOMODIF)
	{
		error ("No caso \"-i\" é necessário dar \"x\", \"t\", \"c\" ou \"e\"\n");
		help ();
	}

	/*
	 *	Prepara os sinais
	 */
	if (signal (SIGHUP,  SIG_IGN) != SIG_IGN)
		signal (SIGHUP, on_signal);

	if (signal (SIGINT,  SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_signal);

	if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
		signal (SIGTERM, on_signal);

	/*
	 *	Analisa o nível de deflação
	 */
	if (level_str != NOSTR)
	{
		if
		(	(ZIP_LEVEL = strtol (level_str, &str, 0)) < MIN_LEVEL ||
			ZIP_LEVEL > MAX_LEVEL || *str != '\0'
		)
		{
			error ("$Nível de compressão ZIP inválido: \"%s\"", level_str);
		}
	}
	else
	{
		ZIP_LEVEL = DEF_LEVEL;
	}

	/*
	 *	Prepara o "garfield"
	 */
	if (gflag)
	{
		if (func == OUT)
		{
			if (freopen (garfield, "r", stdin) == NOFILE)
				error ("$*Não consegui abrir \"%s\"", garfield);
		}
		else
		{
			char *modo = (modif == COMP ? "w" : "r" /* CREATE */);

			if ((gfp = fopen (garfield, modo)) == NOFILE)
				error ("$*Não consegui abrir \"%s\"", garfield);
		}
	}

	/*
	 *	Prepara a TTY para Interativo
	 */
	if ((ttyfp = fopen ("/dev/tty", "r")) == NOFILE)
		error ("$*Não consegui abrir \"/dev/tty\"");

	askyesno_stdin = ttyfp;
	askyesno_set (1);

	if (nflag && func == IN && modif != CREATE && modif != COMP)
	{
		error
		(	"O modo interativo só pode ser usado na\n"
			"\tinclusão/criação/comparação de arquivos\n"
		);
		help ();
	}

	if (nflag)
		vflag = 0;

	/*
	 *	Processa "modecolor"
	 */
	modecolor (isatty (fileno (stderr)), (void *)NULL, &color_vector);

	/*
	 *	Prepara o nome do dispositivo
	 */
	switch (device_code)
	{
	    case 0:
		break;

	    case '0':
		device_nm = "/dev/fd0";
		break;

	    case '1':
		device_nm = "/dev/fd1";
		break;

	    case '2':
		device_nm = "/dev/rzip0";
		break;

	    case '3':
		device_nm = "/dev/rzip1";
		break;

	    default:
		error ("$Nesta versão, o dispositivo # %c não está definido", device_code);

	}	/* end switch */

	/*
	 *	Analisa o dispositivo
	 */
	if (device_nm != NOSTR)
	{
		/*
		 *	Dispositivo de entrada/saída dado
		 */
		int	oflag;

		if   (func == IN)
		{
			oflag = O_RDONLY;
		}
		elif (modif == APPEND)
		{
			oflag = O_RDWR|O_LOCK|O_CREAT;
		}
		else
		{
			oflag = O_WRONLY|O_LOCK|O_CREAT;

			if (!fflag && stat (device_nm, &dev_s) >= 0 && S_ISREG (dev_s.st_mode))
			{
				fprintf
				(	stderr,
					"%s: \"%s\" já existe. Apaga/reescreve? (n): ",
					pgname, device_nm
				);

				if (askyesno () <= 0)
					exit (0);
			}
		}

		if ((colfd = open (device_nm, oflag, 0644)) < 0)
			error ("$*Não consegui abrir \"%s\"", device_nm);
	}
	else	/* device_nm == NOSTR */
	{
		/*
		 *	Entrada/saída padrão
		 */
		if (modif == APPEND)
			error ("$Não podemos usar a entrada/saída padrão para acréscimos");

		if (func == IN)
			{ device_nm = "<stdin>";  colfd = 0; }
		else
			{ device_nm = "<stdout>"; colfd = 1; }
	}

	if (fstat (colfd, &dev_s) < 0)
		error ("$*Não consegui obter estado de \"%s\"", device_nm);

	coldev = dev_s.st_dev;
	colino = dev_s.st_ino;

	/*
	 *	Analisa a blocagem
	 */
	if   (bl_str != NOSTR)
	{
		if ((i = strtol (bl_str, &str, 0)) <= 0 || *str)
			error ("$Blocagem inválida: \"%s\"", bl_str);

		col_area_sz = i * KBSZ;
	}
	elif (S_ISCHR (dev_s.st_mode))
	{
		col_area_sz = COL_AREA_RAW;
	}
	else
	{
		col_area_sz = COL_AREA_STD;
	}

	/*
	 *	Tenta obter o tamanho do dispositivo
	 */
	get_dev_size (colfd, &dev_s);

#if (0)	/*************************************/
#include <sys/ioctl.h>
	/*
	 *	Heurística para obter a blocagem boa
	 */
	if (col_area_sz == NBMAX * BLSZ  &&  func == IN  &&  col_area_sz_given == 0)
	{
		if (ioctl (colfd, MTISATAPE, 0) < 0)
		{
			if   (rflag)
				col_area_sz = NBDEF * BLSZ;
			elif (qflag || nflag || inc_pati > 0 || exc_pati > 0)
				col_area_sz = BLSZ;
			elif (modif == TAB || modif == CREATE && gflag)
				col_area_sz = BLSZ;
			else
				col_area_sz = NBDEF * BLSZ;
		}
	}

	/*
	 *	Verifique se é SU (em virtude do 'm')
	 */
	if (geteuid () != 0 && mflag)
	{
		erro
		(	"O usuário não é SUPERUSUÁRIO: "
			"a opção \"m\" será ignorada"
		);
		mflag = 0;
	}
#endif	/*************************************/

	/*
	 *	Verifique se tem argumentos superfluos
	 */
	if (func == IN  &&  *argv != NOSTR)
		{ error ("Na entrada podem apenas ser dados padrões\n"); help (); }

	/*
	 *	Aloca a área de entrada/saída
	 */
	if ((b = malloc (col_area_sz + sizeof (GVOL))) == (BUF *)NULL)
		error ("$Não obtive memória para a área de entrada/saída");

	/*
	 *	Chama a Rotina Adequada
	 */
	if   (func == IN)
	{
		in ();
	}
	elif (modif == APPEND)
	{
		append (argv);
	}
	else	/* OUT */
	{
		col_area_avail_sz = col_area_sz;
		volhwrite ();
		out (argv);
	}

	quit (0);

}	/* end gar */

/*
 ****************************************************************
 *	Executa a opção "-s" (padrões de "C")			*
 ****************************************************************
 */
void
put_s_option (void)
{
	put_p_option ("*.[cshryv]", inc_pat, &inc_pati);
	put_p_option ("*[Mm]akefile", inc_pat, &inc_pati);

}	/* end put_s_option */

/*
 ****************************************************************
 *	Executa a opção "-p" (outros padrões)			*
 ****************************************************************
 */
int
put_p_option (const char *cp, const char *pat_vec[], int *pati_ptr)
{
	char		c;

	for (EVER)
	{
		while ((c = *cp) == ' ' || c == '\t' )
			cp++;

		if (c == '\0')
			return (0);

		if (*pati_ptr >= NPAT)
		{
			error ("No momento somente aceitamos " XSTR (NPAT) " padrões\n");
			help ();
		}

		pat_vec[(*pati_ptr)++] = cp++;

		while ((c = *cp) != '\0' && c != ' ' && c != '\t')
			cp++;

		if (c == '\0')
			return (0);

		*(char *)cp++ = '\0';

	}	/* end for (EVER) */

}	/* end put_p_option */

/*
 ****************************************************************
 *	Verifica se o nome do arquivo satisfaz um dos padrões	*
 ****************************************************************
 */
int
pattern_accept (const char *file_nm)
{
	const char	**pp;

	/*
	 *	Agora testa os padrões de inclusão
	 */
	if ((pp = inc_pat)[0] != NOSTR)
	{
		for (/* vazio */; /* vazio */; pp++)
		{
			if (*pp == NOSTR)
				return (0);

			if (patmatch (file_nm, *pp))
				break;
		}
	}

	/*
	 *	Finalmente testa os padrões de exclusão
	 */
	if ((pp = exc_pat)[0] != NOSTR)
	{
		for (/* vazio */; *pp != NOSTR; pp++)
		{
			if (patmatch (file_nm, *pp))
				return (0);
		}
	}

	return (1);

}	/* end pattern_accept */

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
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - utilitário para a criação/atualização "
			"de coleções de arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n",
		pgname, pgversion
	);

	if (Hflag > 1) fprintf
	(	stderr,
		"\t%s -i [-xtce] [-F <col>] [-rnvugfmd#] "
		"[-B <n>] <PAD>\n\n"
		"\t%s -o [-Tngv#] [-F <col>] "
		"[-B <n>] <COMP> < <nomes>\n"
		"\t%s -o [-Tnvd.#] [-F <col>] "
		"[-B <n>] <PAD> <COMP> <nome> ...\n\n"
		"\t%s -a [-ngv#] [-F <col>] "
		"[-B <n>] <COMP> < <nomes>\n"
		"\t%s -a [-nvd.#] [-F <col>] "
		"[-B <n>] <PAD> <COMP> <nome> ...\n"
		"\nValores de <PAD>: "
		"[-s] [-p \"<p>\"] [-P \"<P>\"]\n"
		"\nValores de <COMP>: "
		"[-D] [-b <b>] [-l <l>]\n",
		pgname, pgname, pgname, pgname, pgname
	);
	else fprintf
	(	stderr,
		"\t%s -i [-xtce] [-F <col>] [-vugmd#]\n\n"
		"\t%s -o [-v#]   [-F <col>] < <nomes>\n"
		"\t%s -o [-vs#]  [-F <col>] [-p \"<p>\"] <nome> ...\n\n"
		"\t%s -a [-v#]   [-F <col>] < <nomes>\n"
		"\t%s -a [-vs#]  [-F <col>] [-p \"<p>\"] <nome> ...\n",
		pgname, pgname, pgname, pgname, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-i: Entrada\n"
		"\t-o: Saída\n"
		"\t-a: Acréscimo\n"
		"\t-x: Extração\n"
		"\t-t: Tabela de conteúdo\n"
		"\t-c: Comparação\n"
		"\t-e: Escrita dos arquivos na saída padrão\n"
		"\t-F: O dispositivo da coleção é <col>\n"
		"\t-0: O dispositivo da coleção é \"/dev/fd0\"\n"
		"\t-1: O dispositivo da coleção é \"/dev/fd1\"\n"
		"\t-2: O dispositivo da coleção é \"/dev/rzip0\"\n"
		"\t-3: O dispositivo da coleção é \"/dev/rzip1\"\n"
		"\t-v: Verboso\n"
		"\t-u: Comparação com atualização\n"
		"\t-g: Processa \"garfield\"\n"
		"\t-m: Restaura o estado do arquivo\n"
		"\t-d: Diretório corrente/Não inclui diretórios na coleção\n"
		"\t-p: Inclui apenas os arquivos contendo os padrões \"<p>\"\n"
		"\t-s: Utiliza os padrões para programas fonte em \"C\"\n"
	);
 
	if (Hflag < 2) fprintf
	(	stderr,
		"\nObs.:\tPara uma lista completa das opções, use \"%s -HH\" ou \"man %s\"\n",
		pgname, pgname
	);
	else fprintf
	(	stderr,
		"\t-T: Gera a coleção no formato TAR\n"
		"\t-D: NÃO comprime os arquivos regulares\n"
		"\t-l: Utiliza o nível <l> ao invés de "
			XSTR (DEF_LEVEL)  " na compressão ZIP\n"
		"\t-r: Sempre verifica os CRCs\n"
		"\t-n: Interativo\n"
		"\t-f: Força\n"
		"\t-B: Blocagem de <n> KB\n"
		"\t-P: Exclui os arquivos contendo os padrões <P>\n"
		"\t-.: Grava também \"arquivos ocultos\"\n"
	);

	quit (2);

}	/* end help */
