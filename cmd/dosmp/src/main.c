/*
 ****************************************************************
 *								*
 *			dosmp.c					*
 *								*
 *	Monta/processa um sistema de arquivos no formato MS-DOS	*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		4.9.0, de 28.08.07				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "../h/common.h"
#include "../h/dos.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char		pgversion[] =  "Versão:	4.9.0, de 28.08.07";

/*
 ****** Indicadores do programa  ********************************
 */
entry int		cflag;		/* Cria o sistema de arquivos */
entry int		rflag;		/* Read only */
entry int		vflag;		/* Verbose */

/*
 ****** Variáveis de entrada/saída ******************************
 */
entry char		dev_nm[64];	/* Nome do dispositivo */
entry long		dev_mode;	/* Modo do dispositivo */
entry int		dev_fd;		/* Descritor do dispositivo */
entry int		dev_dev;	/* Major + minor do dispositivo */
entry int		dev_type;	/* Tipo da partição */
entry int		dev_ro;		/* Detetada proteção de escrita */

entry FS_STATUS fs_status = FS_CLOSED;	/* Estado atual */
entry char		vol_nm[LFN_SZ];	/* Nome do volume */
entry char		stdin_is_a_tty;	/* Estamos lendo do terminal */

/*
 ******	Quasi-constantes do tempo *******************************
 */
entry time_t		HOJE;		/* Início do dia de hoje (GMT) */
entry time_t		AMANHA;		/* Início do dia de amanhã (GMT) */
entry time_t		SEMANA;		/* Início da semana (GMT) */

/*
 ******	Dados da raiz *******************************************
 */
entry int		root_dev;	/* Dispositivo da raiz */
entry int		root_ino;	/* No. do inode da raiz */

/*
 ******	Bloco de informações universais *************************
 */
entry DOSSB		sb;		/* O Super bloco */
entry UNI		uni;		/* O bloco */
entry UTSNAME		*sys_uts;	/* Informações sobre o sis. operacional */
entry const char	*sys_nm;	/* Nome do sistema operacional */
const char		**color_vector;	/* Para as cores dos modos */

/*
 ****************************************************************
 *   Monta/processa um sistema de arquivos no formato MS-DOS	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt;
	const char	*dev_str = NOSTR;
	char		dev_no_nm[4];
	time_t		difuso;
	UTSNAME		u;
	STAT		s;
	char		*line = NOSTR;

	/*
	 *	Obtém o nome do sistema
	 */
	if (uname (&u) < 0)
	{
		fprintf
		(	stderr,
			"%s: Erro na chamada \"uname\"\n",
			pgname
		);
		exit (1);
	}

	sys_uts = &u;
	sys_nm = (char *)u.uts_sysname;

	/*
	 *	Analisa as opções
	 */
	cmd_optinit++;

	while ((opt = cmd_getopt (argv, "crwv0123456789H")) != EOF)
	{
		if (opt >= '0' && opt <= '9')
		{
			if (dev_str != NOSTR)
				help ();

			dev_no_nm[0] = '-';
			dev_no_nm[1] = opt;
			dev_no_nm[2] = '\0';

			dev_str = dev_no_nm;
		}
		else switch (opt)
		{
		    case 'c':			/* Cria o sistema de arq. */
			cflag++;
			break;

		    case 'r':			/* Read-only */
			rflag++;
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

	/*
	 *	Consistência do argumento
	 */
	argv += cmd_optind;
	argc -= cmd_optind;

	if (rflag && cflag)
	{
		printf ("Opções conflitantes: \"-c\" e \"-r\"\n\n");
		help ();
	}

	switch (argc)
	{
	    case 0:
		break;

	    case 1:
		if (dev_str != NOSTR)
			help ();

		dev_str = argv[0];
		break;

	    default:
		help ();
	}

	/*
	 *	Verifica se "stdin" é um terminal
	 */
	stdin_is_a_tty = isatty (0);

	/*
	 *	Obtém o vetor de cores dos modos
	 *	(apenas se a saída for para um terminal)
	 */
	modecolor (isatty (fileno (stdout)), (void *)NULL, &color_vector);

	/*
	 *	Prepara as datas para o cálculo de HOJE e dias da SEMANA
	 */
	time (&HOJE);				/* Agora GMT */

	difuso = gettzmin () * 60;

	HOJE += difuso;				/* Agora local */

	HOJE =  HOJE / (3600 * 24) * (3600 * 24);

	HOJE -= difuso;				/* Zero hora GMT ajustada */

	AMANHA = HOJE + (3600 * 24);		/* Zero hora de amanhã */

	SEMANA = HOJE - (3600 * 24 * 7);	/* Zero hora há uma semana */

#undef	DEBUG
#ifdef	DEBUG
	printf ("HOJE =		%s", btime (&HOJE));
	printf ("AMANHA =	%s", btime (&AMANHA));
	printf ("SEMANA =	%s", btime (&SEMANA));
#endif	DEBUG

	/*
	 *	Prepara o "dev" e "ino" da raiz
	 */
	if (stat ("/", &s) < 0)
	{
		error ("Não consegui obter o estado de \"/\"");
		exit (2);
	}

	root_dev = s.st_dev;
	root_ino = s.st_ino;

	/*
	 *	Monta o dispositivo, se dado
	 */
   /***	fs_status = FS_CLOSED;	***/
   /***	dev_nm[0] = '\0';	***/
   /***	vol_nm[0] = '\0';	***/
   /***	cwd_cluster = '\0';	***/

	if (dev_str != NOSTR)
	{
		char		*vstr;

		line = alloca (80);

		if (vflag)
			vstr = "-v";
		else
			vstr = "";

		if (cflag)
			sprintf (line, "mkfs %s %s", vstr, dev_str);
		else
			sprintf (line, "mount %s", dev_str);
	}

	exec_cmd (line);

	return (0);

}	/* end dosmp */

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
		"%s - monta/processa um sistema de arquivos no formato MS-DOS\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-crv] [-%s] [<dispositivo>]\n",
		pgname, pgversion, pgname, num_dev_list
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-c: Cria uma sistema de arquivos DOS no <dispositivo>\n"
		"\t-r: Monta o sistema de arquivos somente para leituras\n"
		"\t    e não aceita o comando \"rw\"\n"
		"\t-v: Verboso\n"
		"%s",
		num_dev_option
	);

	exit (2);

}	/* end help */
