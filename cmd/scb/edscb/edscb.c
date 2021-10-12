/*
 ****************************************************************
 *								*
 *			edscb.c					*
 *								*
 *   Imprime e edita o bloco de controle do núcleo do TROPIX	*
 *								*
 *	Versão	1.0.0, de 17.03.87				*
 *		4.6.0, de 13.08.04				*
 *								*
 *	Módulo: scb						*
 *		Utilitário especial				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/kcntl.h>
#include <sys/devmajor.h>
#include <sys/syscall.h>
#include <sys/a.out.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nlist.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.6.0, de 13.08.04";

#define	VIRTUALTOKB(i) 	(i == NOVOID ? 0 : (unsigned)((i) - SYS_ADDR) >> KBSHIFT)
#define	VPGTOKB(i) 	(i == 0 ? 0 : (((unsigned)(i) << PGSHIFT) - (int)SYS_ADDR) >> KBSHIFT)

#define	KERNEL_DEF_NM	"/tropix"

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Variáveis globais ***************************************
 */
entry SCB	scb;		/* O SCB */

entry const char *kernel_nm;	/* Nome do KERNEL */

entry int	modif = 0;	/* O SCB foi modificado */

entry char	ronly = 0;	/* Somente para leituras */

entry char	screen = 'a';	/* Tela atual */

/*
 ****** Estrutura das opções do dialogo do SCB ******************
 */
typedef	struct
{
	char	s_name[14];	/* Nome da variável */
	char	s_screen;	/* Tela da variável */
	char	s_sizeof;	/* Tamanho (em bytes) da variável */
	void	*s_ptr;		/* Ponteiro para a variável */

}	SCBOPT;

#define	SZ_PTR(member) sizeof (scb.member), (void *)(&scb.member)

/*
 ******	Opções do dialogo do SCB ********************************
 */
const SCBOPT	scbopt[] =
{
	"nproc",	'a',	SZ_PTR (y_nproc),
	"ninode",	'a',	SZ_PTR (y_ninode),
	"nbuf",		'a',	SZ_PTR (y_nbuf),
	"nfile",	'a',	SZ_PTR (y_nkfile),
	"nkfile",	'a',	SZ_PTR (y_nkfile),
	"nclist",	'a',	SZ_PTR (y_nclist),
	"nmap",		'a',	SZ_PTR (y_nmap),
	"nhshlib",	'a',	SZ_PTR (y_n_shlib_hash),
	"ntout",	'a',	SZ_PTR (y_ntout),
	"npty",		'a',	SZ_PTR (y_npty),
	"ncon",		'a',	SZ_PTR (y_NCON),
	"nlockf",	'a',	SZ_PTR (y_nlockf),
	"nmount",	'a',	SZ_PTR (y_nmount),
	"nitblock",	'a',	SZ_PTR (y_n_itblock),

	"neventl",	'a',	SZ_PTR (y_nueventl),
	"neventg",	'a',	SZ_PTR (y_nueventg),
	"nsemal",	'a',	SZ_PTR (y_nusemal),
	"nsemag",	'a',	SZ_PTR (y_nusemag),
	"nregionl",	'a',	SZ_PTR (y_nregionl),
	"nregiong",	'a',	SZ_PTR (y_nregiong),
	"ndns",		'a',	SZ_PTR (y_n_dns),
	"nroute",	'a',	SZ_PTR (y_n_route),
	"nether",	'a',	SZ_PTR (y_n_ether),
	"nrawep",	'a',	SZ_PTR (y_n_raw_ep),
	"nudpep",	'a',	SZ_PTR (y_n_udp_ep),
	"ntcpep",	'a',	SZ_PTR (y_n_tcp_ep),

	"rootdev",	'a',	SZ_PTR (y_rootdev),
	"pipedev",	'a',	SZ_PTR (y_pipedev),
	"swapdev",	'a',	SZ_PTR (y_swapdev),
	"swaplow",	'a',	SZ_PTR (y_swaplow),
	"nswap",	'a',	SZ_PTR (y_nswap),

	"ramdsz",	'a',	SZ_PTR (y_ramd0sz),
	"ramd0sz",	'a',	SZ_PTR (y_ramd0sz),
	"ramd1sz",	'a',	SZ_PTR (y_ramd1sz),

	"tslice",	'a',	SZ_PTR (y_tslice),
	"incpri",	'a',	SZ_PTR (y_incpri),
	"decpri",	'a',	SZ_PTR (y_decpri),
	"ttyinc",	'a',	SZ_PTR (y_ttyinc),
	"nfactor",	'a',	SZ_PTR (y_nfactor),

	"maxppu",	'b',	SZ_PTR (y_maxppu),
	"maxpsz",	'b',	SZ_PTR (y_maxpsz),
	"umask",	'b',	SZ_PTR (y_umask),
	"ncexec",	'b',	SZ_PTR (y_ncexec),
	"stksz",	'b',	SZ_PTR (y_stksz),
	"stkincr",	'b',	SZ_PTR (y_stkincr),

	"hz",		'b',	SZ_PTR (y_hz),
	"scrsaver",	'b',	SZ_PTR (y_screen_saver_time),

	"preempt",	'b',	SZ_PTR (y_preemption_mask[0]),
	"key",		'b',	SZ_PTR (y_keyboard),
	"tzmin",	'b',	SZ_PTR (y_tzmin),
	"utclock",	'b',	SZ_PTR (y_ut_clock),
	"dmesgsz",	'b',	SZ_PTR (y_dmesg_sz),
	"dma",		'b',	SZ_PTR (y_dma_enable),
	"dmaenable",	'b',	SZ_PTR (y_dma_enable),

	"synctime",	'b',	SZ_PTR (y_sync_time),
	"blocktime",	'b',	SZ_PTR (y_block_time),
	"dirtytime",	'b',	SZ_PTR (y_dirty_time),
	"maxflush",	'b',	SZ_PTR (y_max_flush),

	"sioport0",	'c',	SZ_PTR (y_sio_port[0]),
	"sioport1",	'c',	SZ_PTR (y_sio_port[1]),
	"sioport2",	'c',	SZ_PTR (y_sio_port[2]),
	"sioport3",	'c',	SZ_PTR (y_sio_port[3]),
	"sioport4",	'c',	SZ_PTR (y_sio_port[4]),
	"sioport5",	'c',	SZ_PTR (y_sio_port[5]),
	"sioport6",	'c',	SZ_PTR (y_sio_port[6]),
	"sioport7",	'c',	SZ_PTR (y_sio_port[7]),

	"sioirq0",	'c',	SZ_PTR (y_sio_irq[0]),
	"sioirq1",	'c',	SZ_PTR (y_sio_irq[1]),
	"sioirq2",	'c',	SZ_PTR (y_sio_irq[2]),
	"sioirq3",	'c',	SZ_PTR (y_sio_irq[3]),
	"sioirq4",	'c',	SZ_PTR (y_sio_irq[4]),
	"sioirq5",	'c',	SZ_PTR (y_sio_irq[5]),
	"sioirq6",	'c',	SZ_PTR (y_sio_irq[6]),
	"sioirq7",	'c',	SZ_PTR (y_sio_irq[7]),

	"lpport0",	'c',	SZ_PTR (y_lp_port[0]),
	"lpport1",	'c',	SZ_PTR (y_lp_port[1]),
	"lpport2",	'c',	SZ_PTR (y_lp_port[2]),
	"lpport3",	'c',	SZ_PTR (y_lp_port[3]),

	"lpirq0",	'c',	SZ_PTR (y_lp_irq[0]),
	"lpirq1",	'c',	SZ_PTR (y_lp_irq[1]),
	"lpirq2",	'c',	SZ_PTR (y_lp_irq[2]),
	"lpirq3",	'c',	SZ_PTR (y_lp_irq[3]),

	"edport0",	'c',	SZ_PTR (y_ed_port[0]),
	"edport1",	'c',	SZ_PTR (y_ed_port[1]),
	"edport2",	'c',	SZ_PTR (y_ed_port[2]),
	"edport3",	'c',	SZ_PTR (y_ed_port[3]),

	"edirq0",	'c',	SZ_PTR (y_ed_irq[0]),
	"edirq1",	'c',	SZ_PTR (y_ed_irq[1]),
	"edirq2",	'c',	SZ_PTR (y_ed_irq[2]),
	"edirq3",	'c',	SZ_PTR (y_ed_irq[3]),

	"usb0",		'c',	SZ_PTR (y_usb_enable[0]),
	"usb1",		'c',	SZ_PTR (y_usb_enable[1]),
	"usb2",		'c',	SZ_PTR (y_usb_enable[2]),
	"usb3",		'c',	SZ_PTR (y_usb_enable[3]),
	"usb4",		'c',	SZ_PTR (y_usb_enable[4]),
	"usb5",		'c',	SZ_PTR (y_usb_enable[5]),
	"usb6",		'c',	SZ_PTR (y_usb_enable[6]),
	"usb7",		'c',	SZ_PTR (y_usb_enable[7]),

	"usb8",		'c',	SZ_PTR (y_usb_enable[8]),
	"usb9",		'c',	SZ_PTR (y_usb_enable[9]),
	"usb10",	'c',	SZ_PTR (y_usb_enable[10]),
	"usb11",	'c',	SZ_PTR (y_usb_enable[11]),
	"usb12",	'c',	SZ_PTR (y_usb_enable[12]),
	"usb13",	'c',	SZ_PTR (y_usb_enable[13]),
	"usb14",	'c',	SZ_PTR (y_usb_enable[14]),
	"usb15",	'c',	SZ_PTR (y_usb_enable[15]),

	"sysname",	'd',	SZ_PTR (y_uts.uts_sysname),
	"systype",	'd',	SZ_PTR (y_uts.uts_systype),
	"nodename",	'd',	SZ_PTR (y_uts.uts_nodename),
	"version",	'd',	SZ_PTR (y_uts.uts_version),
	"date",		'd',	SZ_PTR (y_uts.uts_date),
	"time",		'd',	SZ_PTR (y_uts.uts_time),
	"machine",	'd',	SZ_PTR (y_uts.uts_machine),
	"customer",	'd',	SZ_PTR (y_uts.uts_customer),
	"depto",	'd',	SZ_PTR (y_uts.uts_depto),
	"sysserno",	'd',	SZ_PTR (y_uts.uts_sysserno),

	""
};

/*
 ****** Protótipos de funções ***********************************
 */
void		scbdialog (const char *);
void		prscb (int);
void		prscb_0 (void);
void		prscb_a (int);
void		prscb_b (int);
void		prscb_c (int);
void		prscb_d (int);
void		scbprdev (dev_t);
void		header_endian_conversion (HEADER *);
void		help (void);

/*
 ****************************************************************
 *   Imprime e edita o bloco de controle do núcleo do TROPIX	*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd;
	int		value;
	const char	*scbfile_nm;
	const char 	**scbfile_nm_ptr;
	char		scbfile_given = 0;
	HEADER		h;
	NLIST		nl[2];

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
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Primeiro caso: Imprime os parametros em uso
	 */
	if (argc == 0)
	{
		if ((int)getscb (&scb) == -1)
			error ("$*Não consegui ler o SCB");

		prscb_0 ();
		prscb_a (0 /* Sem nome da tela */);
		prscb_b (0 /* Sem nome da tela */);
		prscb_c (0 /* Sem nome da tela */);
		prscb_d (0 /* Sem nome da tela */);

		exit (0);
	}

	/*
	 *	Segundo caso: Imprime/edita os parametros de um núcleo
	 */
	kernel_nm	= *argv++;
	scbfile_nm_ptr	=  argv;

	if (*scbfile_nm_ptr != NOSTR)
		scbfile_given++;

	/*
	 *	Obtem o endereco do SCB
	 */
	nl[0].n_name = "_scb"; nl[1].n_name = NOSTR;

	if ((value = nlist (kernel_nm, nl)) < 0)
		error ("$*Não consegui obter o endereço do SCB de \"%s\"", kernel_nm);

	/*
	 *	Le o Header
	 */
	errno = 0;

	if ((fd = open (kernel_nm, O_RDWR)) < 0 && errno == EROFS)
		fd = open (kernel_nm, O_RDONLY);

	if (fd < 0) 
		error ("$*Não consegui abrir \"%s\"", kernel_nm);

	if (errno == EROFS)
	{
		error ("Abrindo \"%s\" somente para leituras!", kernel_nm);
		ronly++;
	}

	if (read (fd, &h, sizeof (HEADER)) < 0)
		error ("$*Erro na leitura do cabeçalho de \"%s\"", kernel_nm);

	/*
	 *	Le o SCB
	 */
	lseek
	(	fd,
		sizeof (HEADER) + h.h_tsize + nl[0].n_value - h.h_dstart,
		L_SET
	);

	if (read (fd, &scb, sizeof (SCB)) < 0)
		error ("$*Erro na leitura do SCB");

	/*
	 *	Modifica (se for o caso) o SCB
	 */
	if (*scbfile_nm_ptr)
	{
		while (scbfile_nm = *scbfile_nm_ptr++)
			scbdialog (scbfile_nm);
	}
	else
	{
		scbdialog (NOSTR);
	}

	/*
	 *	Escreve o SCB
	 */
	if (modif == 0)
		exit (exit_code);

	if (scbfile_given && exit_code)
		exit (exit_code);

	if (scbfile_given)
	{
		if (ronly) error
		(	"$NÃO posso modificar o SCB de \"%s\" (aberto somente para leituras)",
			kernel_nm
		);
	}
	else /* !scbfile_given */
	{
		if (ronly)
			exit (1);

		fprintf
		(	stderr,
			"\nO SCB foi modificado - Atualiza \"%s\"? (n): ",
			kernel_nm
		);

		if (askyesno () <= 0)
			exit (0);
	}

	lseek
	(	fd,
		sizeof (HEADER) + h.h_tsize + nl[0].n_value - h.h_dstart,
		L_SET
	);

	if (write (fd, &scb, sizeof (SCB)) < 0)
		error ("$*Erro na escrita do SCB", strerror (errno));

	if (!scbfile_given)
		error ("Atualizado o SCB de \"%s\"", kernel_nm);

	return (0);

}	/* end edscb */

/*
 ****************************************************************
 *	Processa o arquivo de parametros			*
 ****************************************************************
 */
void
scbdialog (const char *scbfile_nm)
{
	FILE		*scb_fp;
	SCB		*sp = &scb;
	int		n = 0;
	const SCBOPT	*op;
	char		*np;
	const char	*cp, *str;
	char		c;
	char		line[80];

	/*
	 *	Analisa o arquivo de comandos
	 */
	if (scbfile_nm != NOSTR)
	{
		if ((scb_fp = fopen (scbfile_nm, "r")) == NOFILE)
			error ("$*Não consegui abrir \"%s\"", scbfile_nm);
	}
	else
	{
		scb_fp = stdin;
	}

	/*
	 *	Examina cada linha de comando
	 */
    again:
	if (!scbfile_nm)
		prscb (1 /* Com o nome da tela */);

    prompt:
	if (!scbfile_nm)
		printf ("\n> ");

	if (fngets (line, sizeof (line), scb_fp) != line)
		return;

	cp = &line[0];

	while (*cp == ' ' || *cp == '\t')
		cp++;

	if (cp[0] == '#')
		goto prompt;

	if (cp[0] == '\0' || cp[0] == '?')
	{
		if (!scbfile_nm) printf
		( 	"\n%s, %s\n"
			"\nTecle:"
			"	\"a\", \"b\", \"c\", \"d\""
			"\t   para mostrar a tela \"a\", \"b\", \"c\", \"d\"\n"
			"	\"<param>=<valor>\""
			"  para atualizar <param> com o <valor> dado\n"
			"	\"w\", \"q\" ou <^D>"
			"   para encerrar o programa\n"
			"			"
			"   (com a possibilidade de atualizar o SCB)\n",
			pgname, pgversion
		);
		goto prompt;
	}

	if (streq (cp, "q"))
		return;

	if (streq (cp, "w"))
	{
		if (modif)
			return;

		printf ("O SCB ainda NÃO foi modificado\n");
		goto prompt;
	}

	if (cp[1] == '\0')
	{
		if (cp[0] == 'a' || cp[0] == 'A')
			{ screen = 'a'; goto again; }

		if (cp[0] == 'b' || cp[0] == 'B')
			{ screen = 'b'; goto again; }

		if (cp[0] == 'c' || cp[0] == 'C')
			{ screen = 'c'; goto again; }

		if (cp[0] == 'd' || cp[0] == 'D')
			{ screen = 'd'; goto again; }
	}

	if ((np = strchr (cp, '=')) == NOSTR)
	{
		if (scbfile_nm)
			puts (line);
		printf ("\t*** Não achei o \"=\" na linha\n");
		exit_code++;
		goto prompt;
	}

	*np++ = '\0';

	/*
	 *	Procura o ID na tabela
	 */
	for (op = scbopt; /* abaixo */; op++)
	{
		if (op->s_name[0] == '\0')
		{
			if (scbfile_nm)
				{ np[-1] = '='; puts (line); }

			printf ("\t*** Parâmetro \"%s\" desconhecido\n", cp);
			exit_code++;
			goto prompt;
		}

		if (streq (cp, op->s_name))
			break;
	}

	/*
	 *	Encontrou o parametro na tabela
	 */
	modif++; screen = op->s_screen;

	if   (op->s_sizeof == sizeof (scb.y_uts.uts_sysname))
	{
		/* UTS => nenhuma conversão */
	}
	elif (streq (&cp[4], "dev") && isalpha (*np))
	{
		if   (streq (np, "nodev") || streq (np, "NODEV"))
		{
			n = NODEV;
		}
		elif ((n = kcntl (K_GETDEV, np)) < 0)
		{
			printf ("\t*** Dispositivo \"%s\" desconhecido\n", np);
			exit_code++;
			goto prompt;
		}
	}
	elif (streq (&cp[0], "umask"))
	{
		n = strtol (np, &str, 8);

		if (*str != '\0')
			goto bad_value;
	}
	elif (streq (&cp[0], "tzmin"))
	{
		n = strtol (np, &str, 0);

		if (*str != '\0')
			goto bad_value;
	}
	else
	{
		if ((n = strtol (np, &str, 0)) < 0)
		{
		    bad_value:
			if (scbfile_nm)
				{ np[-1] = '='; puts (line); }
			printf ("\t*** Valor inválido\n");
			exit_code++;
			goto prompt;
		}

		if   ((c = *str) == 'k' || c == 'K')
			{ str++; n <<= 10; }
		elif (c == 'm' || c == 'M')
			{ str++; n <<= 20; }

		if (*str != '\0' && *str != ' ')
			goto bad_value;
	}

	/*
	 *	Analisa o argumento - existem algumas mudanças de unidades
	 */
	if (strncmp (&cp[0], "ramd", 4) == 0)
		n *= KBSZ;

	if   (streq (cp, "maxpsz") || streq (cp, "mmulimit"))
		n = KBTOPG (n);
	elif (streq (cp, "rootdev"))
		sp->y_pipedev = n;

	if   (op->s_sizeof == sizeof (long))
		*(long *)op->s_ptr = n;
	elif (op->s_sizeof == sizeof (short))
		*(short *)op->s_ptr = n;
	elif (op->s_sizeof == sizeof (char))
		*(char *)op->s_ptr = n;
	elif (op->s_sizeof == sizeof (scb.y_uts.uts_sysname))
		strscpy (op->s_ptr, np, sizeof (scb.y_uts.uts_sysname));

	goto again;

}	/* end scbdialog */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (umas das telas)		*
 ****************************************************************
 */
void
prscb (int tela)
{
	switch (screen)
	{
	    case 'a':
	    default:
		prscb_a (tela);
		break;

	    case 'b':
		prscb_b (tela);
		break;

	    case 'c':
		prscb_c (tela);
		break;

	    case 'd':
		prscb_d (tela);
		break;
	}

}	/* end prscb */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (tela "0")		*
 ****************************************************************
 */
void
prscb_0 (void)
{
	SCB		*sp = &scb;

	/*
	 *	Tela 0
	 */

	/*
	 *	CPUs
	 */
	printf ("ncpu     = %5d        ", sp->y_ncpu);
	printf ("initpid  = %5d        ", sp->y_initpid);
	printf ("cputype  = ");

	switch (sp->y_cputype)
	{
	    case 386:
		printf ("INTEL 386\n");
		break;

	    case 486:
		printf ("INTEL 486\n");
		break;

	    case 586:
		printf ("INTEL PENTIUM\n");
		break;

	    default:
		printf ("????\n");
		break;

	}	/* end switch */

	printf ("DELAY    = %5d        ", sp->y_DELAY_value);
	printf ("pgsz     = %5d        ", sp->y_pgsz);
	printf ("\n");

	printf ("nblkdev  = %5d        ", sp->y_nblkdev);
	printf ("nchrdev  = %5d\n\n",	  sp->y_nchrdev);

	printf ("nphash   = %5d, %-5d ",  sp->y_nphash, sp->y_phmask);
	printf ("nihash   = %5d        ", sp->y_nihash);
	printf ("nbhash   = %5d\n",	  sp->y_nbhash);

	printf ("ucore    = %6d KB    ", VPGTOKB (sp->y_ucore1) );
	printf ("enducore = %6d KB    ", VPGTOKB (sp->y_enducore1) );
	printf ("endpmem  = %6d KB\n\n",  VIRTUALTOKB (sp->y_endpmem) ); 

	printf ("ramd1    = %6d KB    ",  VIRTUALTOKB (sp->y_ramd1) );
	printf ("endramd1 = %6d KB    ",   VIRTUALTOKB (sp->y_endramd1) ); 
	printf ("ramd1sz  = %5d KB\n",    BYTOKB (sp->y_ramd1sz) ); 

	printf ("ramd0    = %6d KB    ",  VIRTUALTOKB (sp->y_ramd0) );
	printf ("endramd0 = %6d KB    ",  VIRTUALTOKB (sp->y_endramd0) ); 
	printf ("ramd0sz  = %5d KB\n",    BYTOKB (sp->y_ramd0sz) ); 

}	/* end prscb_0 */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (tela "a")		*
 ****************************************************************
 */
void
prscb_a (int tela)
{
	SCB		*sp = &scb;

	/*
	 *	Tela A
	 */
	if (tela)
	{
		printf
		(	"\f\t*** SCB (\"%s\"%s): "
			"TELA \"A\" ***\n\n",
			kernel_nm, ronly ? ", somente para LEITURAS" : ""
		);
	}
	else
	{
		printf ("\n");
	}

	printf ("nproc    = %5d        ", sp->y_nproc);
	printf ("ninode   = %5d        ", sp->y_ninode);
	printf ("nbuf     = %5d\n",	  sp->y_nbuf);

	printf ("nkfile   = %5d        ", sp->y_nkfile);
	printf ("nclist   = %5d        ", sp->y_nclist);
	printf ("nmap     = %5d\n", 	  sp->y_nmap);

	printf ("nhshlib  = %5d        ", sp->y_n_shlib_hash);
	printf ("ntout    = %5d        ", sp->y_ntout);
	printf ("npty     = %5d\n",	  sp->y_npty);

	printf ("ncon     = %5d        ", sp->y_NCON);
	printf ("nlockf   = %5d        ", sp->y_nlockf);
	printf ("nmount   = %5d\n",	  sp->y_nmount);

	printf ("nitblock = %5d\n\n",	  sp->y_n_itblock);

	/*
	 *	Tela A do BOOT
	 */
	printf ("neventl  = %5d        ", sp->y_nueventl);
	printf ("neventg  = %5d        ", sp->y_nueventg);
	printf ("nsemal   = %5d\n",	  sp->y_nusemal);

	printf ("nsemag   = %5d        ", sp->y_nusemag);
	printf ("nregionl = %5d        ", sp->y_nregionl);
	printf ("nregiong = %5d\n",	  sp->y_nregiong);

	printf ("ndns     = %5d        ", sp->y_n_dns);
	printf ("nroute   = %5d        ", sp->y_n_route);
	printf ("nether   = %5d\n",	  sp->y_n_ether);

	printf ("nrawep   = %5d        ", sp->y_n_raw_ep);
	printf ("nudpep   = %5d        ", sp->y_n_udp_ep);
	printf ("ntcpep   = %5d\n\n",	  sp->y_n_tcp_ep);

	/*
	 *	Dispositivos e memória
	 */
	printf ("rootdev  = "); scbprdev (sp->y_rootdev); printf ("\t");
	printf ("pipedev  = "); scbprdev (sp->y_pipedev); printf ("\t");
	printf ("swapdev  = "); scbprdev (sp->y_swapdev); printf ("\n");
	printf ("swaplow  = %5d        ", sp->y_swaplow);
	printf ("nswap    = %5d\n", sp->y_nswap);

	printf ("ramd0sz  = %5d KB     ",    BYTOKB (sp->y_ramd0sz));
	printf ("ramd1sz  = %5d KB\n\n",  BYTOKB (sp->y_ramd1sz));

	/*
	 *	Scheduler
	 */
	printf ("tslice   = %5d Ticks  ", sp->y_tslice);
	printf ("incpri   = %5d        ", sp->y_incpri);
	printf ("decpri   = %5d\n",	  sp->y_decpri);

	printf ("ttyinc   = %5d        ", sp->y_ttyinc);
	printf ("nfactor  = %5d\n      ", sp->y_nfactor);

}	/* end prscb_a */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (tela "b")		*
 ****************************************************************
 */
void
prscb_b (int tela)
{
	SCB		*sp = &scb;

	/*
	 *	Tela B
	 */
	if (tela)
	{
		printf
		(	"\f\t*** SCB (\"%s\"%s): "
			"TELA \"B\" ***\n\n",
			kernel_nm, ronly ? ", somente para LEITURAS" : ""
		);
	}
	else
	{
		printf ("\n");
	}

	/*
	 *	Processo do usuário
	 */
	printf ("maxppu   = %5d        ", sp->y_maxppu);
	printf ("maxpsz   = %5d KB     ", PGTOKB (sp->y_maxpsz));
	printf ("umask    =   %03o\n",	  sp->y_umask);

	printf ("ncexec   = %5d        ", sp->y_ncexec);
	printf ("stksz    = %5d        ", sp->y_stksz);
	printf ("stkincr  = %5d\n\n",	  sp->y_stkincr);

	/*
	 *	Diversos
	 */
	printf ("hz       = %5d        ", sp->y_hz);
	printf ("scrsaver = %5d\n",	  sp->y_screen_saver_time);

	printf ("preempt  =  0x%02X        ", sp->y_preemption_mask[0]);
	printf ("key      = %5d        ", sp->y_keyboard);
	printf ("dmesgsz  = %5d\n",	  sp->y_dmesg_sz);

	printf ("tzmin    = %5d        ", sp->y_tzmin);
	printf ("utclock  = %5d\n\n",	  sp->y_ut_clock);

	printf ("dmaenable= %5d        ", sp->y_dma_enable);

	/*
	 *	Parametros da atualização do "cache"
	 */
	printf ("synctime = %5d        ", sp->y_sync_time);
	printf ("blocktime= %5d\n",	  sp->y_block_time);
	printf ("dirtytime= %5d        ", sp->y_dirty_time);
	printf ("maxflush = %5d\n\n",	  sp->y_max_flush);

	/*
	 *	As semi constantes
	 */
	printf ("NCON     = %5d        ", sp->y_NCON);
	printf ("NSIO     = %5d\n",	  sp->y_NSIO);

	printf ("USIZE    = %5d        ", sp->y_USIZE);
	printf ("NUFILE   = %5d\n\n",	  sp->y_NUFILE);

	printf ("SYS_ADDR = %P (%d MB)\t\t", sp->y_SYS_ADDR, sp->y_SYS_ADDR >> MBSHIFT);
	printf ("UPROC_ADDR=%P (%d MB)\n", sp->y_UPROC_ADDR, sp->y_UPROC_ADDR >> MBSHIFT);

	printf ("USER_TEXT= %P (%d MB)\t\t", sp->y_USER_TEXT_ADDR, sp->y_USER_TEXT_ADDR >> MBSHIFT);
	printf ("USER_DATA= %P (%d MB)\n", sp->y_USER_DATA_ADDR, sp->y_USER_DATA_ADDR >> MBSHIFT);

}	/* end prscb_b */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (tela "c")		*
 ****************************************************************
 */
void
prscb_c (int tela)
{
	SCB		*sp = &scb;

	/*
	 *	Tela C
	 */
	if (tela)
	{
		printf
		(	"\f\t*** SCB (\"%s\"%s): "
			"TELA \"C\" ***\n\n",
			kernel_nm, ronly ? ", somente para LEITURAS" : ""
		);
	}
	else
	{
		printf ("\n");
	}

	/*
	 *	Portas e interrupções das linhas serias
	 */
	printf ("sioport0 =   0x%X\t",    sp->y_sio_port[0]);
	printf ("sioirq0  = %d\t", 	  sp->y_sio_irq[0]);
	printf ("sioport1 =   0x%X\t", 	  sp->y_sio_port[1]);
	printf ("sioirq1  = %d\n", 	  sp->y_sio_irq[1]);
	printf ("sioport2 =   0x%X\t", 	  sp->y_sio_port[2]);
	printf ("sioirq2  = %d\t", 	  sp->y_sio_irq[2]);
	printf ("sioport3 =   0x%X\t", 	  sp->y_sio_port[3]);
	printf ("sioirq3  = %d\n", 	  sp->y_sio_irq[3]);
	printf ("sioport4 =   0x%X\t", 	  sp->y_sio_port[4]);
	printf ("sioirq4  = %d\t", 	  sp->y_sio_irq[4]);
	printf ("sioport5 =   0x%X\t", 	  sp->y_sio_port[5]);
	printf ("sioirq5  = %d\n", 	  sp->y_sio_irq[5]);
	printf ("sioport6 =   0x%X\t", 	  sp->y_sio_port[6]);
	printf ("sioirq6  = %d\t", 	  sp->y_sio_irq[6]);
	printf ("sioport7 =   0x%X\t", 	  sp->y_sio_port[7]);
	printf ("sioirq7  = %d\n", 	  sp->y_sio_irq[7]);

	printf ("\nlpport0  =   0x%X\t",  sp->y_lp_port[0]);
	printf ("lpirq0   = %d\t", 	  sp->y_lp_irq[0]);
	printf ("lpport1  =   0x%X\t", 	  sp->y_lp_port[1]);
	printf ("lpirq1   = %d\n", 	  sp->y_lp_irq[1]);
	printf ("lpport2  =   0x%X\t", 	  sp->y_lp_port[2]);
	printf ("lpirq2   = %d\t", 	  sp->y_lp_irq[2]);
	printf ("lpport3  =   0x%X\t", 	  sp->y_lp_port[3]);
	printf ("lpirq3   = %d\n", 	  sp->y_lp_irq[3]);

	printf ("\nedport0  =   0x%X\t",  sp->y_ed_port[0]);
	printf ("edirq0   = %d\t", 	  sp->y_ed_irq[0]);
	printf ("edport1  =   0x%X\t", 	  sp->y_ed_port[1]);
	printf ("edirq1   = %d\n", 	  sp->y_ed_irq[1]);
	printf ("edport2  =   0x%X\t", 	  sp->y_ed_port[2]);
	printf ("edirq2   = %d\t", 	  sp->y_ed_irq[2]);
	printf ("edport3  =   0x%X\t", 	  sp->y_ed_port[3]);
	printf ("edirq3   = %d\n", 	  sp->y_ed_irq[3]);

	printf ("\nusb0     =   %d\t\t",  sp->y_usb_enable[0]);
	printf ("usb1     = %d\t",	  sp->y_usb_enable[1]);
	printf ("usb2     =   %d\t\t",	  sp->y_usb_enable[2]);
	printf ("usb3     = %d\n",	  sp->y_usb_enable[3]);
	printf ("usb4     =   %d\t\t",	  sp->y_usb_enable[4]);
	printf ("usb5     = %d\t",	  sp->y_usb_enable[5]);
	printf ("usb6     =   %d\t\t",	  sp->y_usb_enable[6]);
	printf ("usb7     = %d\n",	  sp->y_usb_enable[7]);

	printf ("\nusb8     =   %d\t\t",  sp->y_usb_enable[8]);
	printf ("usb9     = %d\t",	  sp->y_usb_enable[9]);
	printf ("usb10    =   %d\t\t",	  sp->y_usb_enable[10]);
	printf ("usb11    = %d\n",	  sp->y_usb_enable[11]);
	printf ("usb12    =   %d\t\t",	  sp->y_usb_enable[12]);
	printf ("usb13    = %d\t",	  sp->y_usb_enable[13]);
	printf ("usb14    =   %d\t\t",	  sp->y_usb_enable[14]);
	printf ("usb15    = %d\n",	  sp->y_usb_enable[15]);

}	/* end prscb_c */

/*
 ****************************************************************
 *	Imprime o estado atual do SCB (tela "c")		*
 ****************************************************************
 */
void
prscb_d (int tela)
{
	SCB		*sp = &scb;

	/*
	 *	Tela D
	 */
	if (tela)
	{
		printf
		(	"\f\t*** SCB (\"%s\"%s): "
			"TELA \"D\" ***\n\n",
			kernel_nm, ronly ? ", somente para LEITURAS" : ""
		);
	}
	else
	{
		printf ("\n");
	}

	/*
	 *	Estrutura UTS(NAME)
	 */
	printf ("sysname  = \"%s\"\n", sp->y_uts.uts_sysname);
	printf ("systype  = \"%s\"\n", sp->y_uts.uts_systype);
	printf ("nodename = \"%s\"\n", sp->y_uts.uts_nodename);
	printf ("version  = \"%s\"\n", sp->y_uts.uts_version);
	printf ("date     = \"%s\"\n", sp->y_uts.uts_date);
	printf ("time     = \"%s\"\n", sp->y_uts.uts_time);
	printf ("machine  = \"%s\"\n", sp->y_uts.uts_machine);
	printf ("customer = \"%s\"\n", sp->y_uts.uts_customer);
	printf ("depto    = \"%s\"\n", sp->y_uts.uts_depto);
	printf ("sysserno = \"%s\"\n\n", sp->y_uts.uts_sysserno);

}	/* end prscb_d */

/*
 ****************************************************************
 *	Imprime o Nome do Dispositivo				*
 ****************************************************************
 */
void
scbprdev (dev_t dev)
{
	char		id[16];

	if   (dev == NODEV)
	{
		printf ("NODEV", kcntl (K_GETDEVNM, dev, id));
	}
	elif ((int)kcntl (K_GETDEVNM, dev, id) != -1)
	{
		printf ("%s (%d/%d)", id, MAJOR (dev), MINOR (dev));
	}
	elif (MAJOR (dev) == MD_MAJOR)
	{
		printf ("md%d (%d/%d)", MINOR (dev), MAJOR (dev), MINOR (dev));
	}
	else
	{
		printf ("??? (%d/%d)", MAJOR (dev), MINOR (dev));
	}

}	/* end scbprdev */

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
	 	"%s - imprime e edita o bloco de controle do núcleo do TROPIX\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [<kernel> [<scbfile> ...]]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
		"\nObs.:\tSe <kernel> não for dado, será impresso o\n"
		"\tbloco de controle do núcleo em execução\n"
	);

	exit (2);

}	/* end help */
