/*
 ****************************************************************
 *								*
 *			prrom.c					*
 *								*
 *	Imprime informações da ROM				*
 *								*
 *	Versão	2.3.0, de 24.08.90				*
 *		2.3.1, de 15.10.91				*
 *								*
 *	Módulo: prrom						*
 *		Utilitários de manutenção			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/disktb.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	2.3.1, de 15.10.90";

#define	KBSZ	1024

#define	MAXPD	100		/* No. max de partições */

entry char	*pgname;	/* Nome do programa */

entry int	vflag;		/* Verbose */

entry int	DIFF;		/* Diferença entre os endereços físico/virtual */

/*
 ****** Endereço da ROM *****************************************
 */
#define	ROM_ADDR	0xFC0000

/*
 ****** Estruturas da ROM ***************************************
 */
typedef struct
{
	long	r_reset;	/* Prepara um stack pointer fictício */
	long	r_main;		/* Prepara o program counter real */
	long	r_autoteste;	/* Endereco da rotina de autoteste */
	long	r_boot;		/* Endereco da rotina de bootstrap */
	long	r_scsiio;	/* Endereco da rotina de disco SCSI */
	long	r_dskio;	/* Endereco da rotina de diskette */
	long	r_tmem;		/* Endereco da rotina de teste de memoria */
	long	r_Hg;		/* Endereco da rotina de testes gerais */
	long	r_disktb;	/* Endereco da tabela de discos */
	long	r_sernum;	/* Endereco do numero de serie */
	long	r_godep;	/* Endereco de re-entrada do depurador */

}	ROM_ATB;

typedef struct
{
	long	r_sn;		/* Número de série */
	time_t	r_time;		/* Data de geração da ROM */

}	SERNUM;

/*
 ****** Protótipos de funções ***********************************
 */
void		prtable (DISKTB *);
void		help (void);

char		*btime (time_t *);

/*
 ****************************************************************
 *	Imprime informações da ROM				*
 ****************************************************************
 */
main (int argc, register char *argv[])
{
	register int	opt, fd;
	ROM_ATB		rom_atb;
	SERNUM		sernum;
	DISKTB		disktb[MAXPD];

	pgname = argv[0];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
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
	 *	Abre o dispositivo da memória
	 */
	if ((fd = open ("/dev/mem", O_RDONLY)) <= 0)
	{
		fprintf (stderr, "Não consegui abrir \"/dev/mem\"\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Le a tabela de endereços
	 */
	lseek (fd, ROM_ADDR, L_SET);
	
	if (read (fd, &rom_atb, sizeof (ROM_ATB)) != sizeof (ROM_ATB))
	{
		fprintf (stderr, "Não consegui ler a tabela de endereços\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Le a estrutura do número de série
	 */
	lseek (fd, rom_atb.r_sernum, L_SET);

	if (read (fd, &sernum, sizeof (SERNUM)) != sizeof (SERNUM))
	{
		fprintf (stderr, "Não consegui ler o número de série\n");
		perror (pgname);
		exit (1);
	}

	printf
	(	"Número de série = %d, Data de geração da ROM = %s\n",
		sernum.r_sn, btime (&sernum.r_time)
	);

	/*
	 *	Le a tabela dos discos
	 */
	lseek (fd, rom_atb.r_disktb, L_SET);

	if (read (fd, disktb, sizeof (disktb)) != sizeof (disktb))
	{
		fprintf (stderr, "Não consegui ler a tabela dos discos\n");
		perror (pgname);
		exit (1);
	}

	prtable (disktb);

	exit (0);

}	/* end prrom */

/*
 ****************************************************************
 *	Imprime a tabela de dispositivos			*
 ****************************************************************
 */
void
prtable (DISKTB *tb)
{
	register DISKTB	*pt;
	register int	i;

	for (pt = tb, i = 0; pt->p_name[0]; pt++)
		i++;

	printf ("A tabela contém %d entradas\n\n", i);

	printf ("NAME -SIZE- OFFSET UNIT MAJOR MINOR PORTA PERIF NSECT  CYL HEAD FLAG\n\n");

	for (pt = tb, i = 0; pt->p_name[0]; pt++, i++)
	{
		if (i && i % 5 == 0)
			fputc ('\n', stdout);

		printf
		(	"%4s %6d %6d %4d %5d %5d %5d %5d %5d %4d %4d  %02X\n",
			pt->p_name,
			pt->p_nblocks,
			pt->p_blkoff,
			pt->p_unit,
			MAJOR (pt->p_dev),
			MINOR (pt->p_dev),
			pt->p_porta,
			pt->p_per,
			pt->p_nsect,
			pt->p_c,
			pt->p_h,
			pt->p_flags
		);
	}

}	/* prtable */

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
		"%s - imprime informações da ROM\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
