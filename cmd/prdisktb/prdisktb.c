/*
 ****************************************************************
 *								*
 *			prdisktb.c				*
 *								*
 *	Imprime a tabela de discos				*
 *								*
 *	Versão	3.0.0, de 02.02.95				*
 *		4.6.0, de 14.04.04				*
 *								*
 *	Módulo: prdisktb					*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/disktb.h>
#include <sys/kcntl.h>
#include <sys/v7.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.6.0, de 14.04.04";

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	aflag;		/* Outras informações */
entry int	vflag;		/* Verbose */

/*
 ****** Uma entrada da tabela de partições **********************
 */
typedef struct
{
	char	head;		/* Cabeça */
	char	sect;		/* Setor */
	char	cyl;		/* Cilindro */

}	TRIO;

typedef struct			/* Tabela de partições */
{
	char	pt_active;	/* Entrada ativa se 0x80 */
	TRIO	pt_start;	/* Trio inicial */
	char	pt_type;	/* Tipo do sistema de arquivos */
	TRIO	pt_end;		/* Trio final */
	long	pt_offset;	/* Deslocamento (blocos) */
	long	pt_size;	/* Tamanho (blocos) */

}	PARTTB;

#define	NOTRIO	 (TRIO *)0	/* Ponteiro NULO */
#define NOPARTTB (PARTTB *)0	/* Ponteiro NULO */

/*
 ******	Definições úteis para a tabela de partições *************
 */
#define	NPART	 4		/* Número total de partições reg. */
#define	NLOGPART ('z'-'a'+ 1)	/* Número máximo de partições lógicas */

#define	DOSEXT		0x05	/* Código da partição estendida DOS */
#define	TROPIXEXT	0xAE	/* Código da partição estendida TROPIX */

#define	IS_EXT(t)	(t == DOSEXT || t == TROPIXEXT)

/*
 ******	A tabela de partições em um bloco ***********************
 */
typedef struct
{
	char	pr_unused[BLSZ - NPART * sizeof (PARTTB) - sizeof (ushort)];
	char	pr_parttb[NPART * sizeof (PARTTB)];	/* Desalinhado */
	ushort	pr_magic;

}	PR;

/*
 ******	Informação sobre a geometria ****************************
 */
typedef struct
{
	int	h_head;		/* No. de cabeças */
	int	h_sect;		/* No. de setores */
	int	h_cylsz;	/* Tamanho do cilindro */

}	HDINFO;

#define NOHDINFO (HDINFO *)0	/* Ponteiro NULO */

/*
 ****** Protótipos de funções ***********************************
 */
void		pr_principal (const DISKTB *);
void		pr_secundary (const DISKTB *);
void		read_parttb (const char *);
void		fdisk_print_parttb_entry (const PARTTB *);
void		print_parttb_trios (const PARTTB *, const HDINFO *, daddr_t);
char		*cv_blocks_to_KB_MB_or_GB (long);
char		*cv_dev_to_major_and_minor (int);
char		*cv_unit_and_target (const DISKTB *);
void		print_fs1_name (const DISKTB *);
void		print_fs2_name (const DISKTB *);
void		help (void);

int		geo_get_parttb_geo (const char *, const PR *, HDINFO *);
int		geo_get_std_method (const PR *, HDINFO *);
int		geo_get_cyl (const PARTTB *);
int		geo_get_sec (int, ulong, const TRIO *);
int		geo_get_heu_method (const PR *, HDINFO *);
long		geo_get_linear_blno (const TRIO *, const HDINFO *);

/*
 ****************************************************************
 *	Imprime a tabela de discos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, n;
	const DISKTB	*disktb;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "avH")) != EOF)
	{
		switch (opt)
		{
		    case 'a':			/* Informações alternativas */
			aflag++;
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

	argv += optind;
	argc -= optind;

	/*
	 *	Verifica se é para imprimir a tabela de um dispositivo
	 */
	if (argv[0] != NOSTR)
		{ read_parttb (argv[0]); exit (exit_code); }

	/*
	 *	Obtém a tabela de disco
	 */
	if ((n = kcntl (K_GET_DEV_TB, NODEV, NODISKTB)) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui obter o tamanho da tabela de discos (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	disktb = alloca (n * sizeof (DISKTB));

	if (kcntl (K_GET_DEV_TB, NODEV, disktb) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui obter a tabela de discos (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	if (aflag)
		pr_secundary (disktb);
	else
		pr_principal (disktb);

	return (exit_code);

}	/* end prdisktb */

/*
 ****************************************************************
 *	Imprime as informações principais			*
 ****************************************************************
 */
void
pr_principal (const DISKTB *disktb)
{
	const DISKTB	*dp;
	int		i;

	/*
	 *	Imprime a tabela
	 */
	printf ("TABELA DE DISCOS/PARTIÇÕES:\n");

	printf ("\n-NAME- A --OFFSET--  ---SIZE---  ---MB---  -DEV-  -U/T-  --------TYPE--------\n\n");

	for (dp = disktb, i = 0; dp->p_name[0]; dp++, i++)
	{
		if (!vflag && dp->p_name[0] == '*')
			continue;

		if (i && i % 5 == 0)
			printf ("\n");

		printf
		(	"%-6s " "%c " "%10d  " "%10d  " "%8.8s  "
			"%-5.5s  " " %-5.5s ",
			dp->p_name,
			dp->p_flags & DISK_ACTIVE ? '*' : ' ',
			dp->p_offset,
			dp->p_size,
			cv_blocks_to_KB_MB_or_GB (dp->p_size),
			cv_dev_to_major_and_minor (dp->p_dev),
			cv_unit_and_target (dp)
		);


		{
			char		type_name[PARTNM_MAXSZ];

			if (kcntl (K_GET_PARTNM, dp->p_type, type_name) < 0)
				printf ("%02X  ???", dp->p_type);
			else
				printf ("%02X  %s", dp->p_type, type_name);

			if (dp->p_size != 0)
			{
				if   (dp->p_type == 0xA8)
					print_fs1_name (dp);
				elif (dp->p_type == 0xA9)
					print_fs2_name (dp);
			}
		}

		printf ("\n");

	}	/* end for (tabela de partições) */

}	/* end pr_principal */

/*
 ****************************************************************
 *	Imprime as informações alternativas			*
 ****************************************************************
 */
void
pr_secundary (const DISKTB *disktb)
{
	const DISKTB	*dp;
	int		i;

	/*
	 *	Imprime a tabela
	 */
	printf ("TABELA DE DISCOS/PARTIÇÕES:\n");

	printf ("\n-NAME-  -DEV-  -U/T-  HEAD  SECT   CYL G/L  BLSZ OPEN  LOCK\n\n");

	for (dp = disktb, i = 0; dp->p_name[0]; dp++, i++)
	{
		if (!vflag && dp->p_name[0] == '*')
			continue;

		if (i && i % 5 == 0)
			printf ("\n");

		printf
		(	"%-6s  " "%-5.5s  " " %-5.5s "
			"%4d  " "%4d  " "%4d  " " %c " " %4d " "%4d  " "%4d  ",
			dp->p_name,
			cv_dev_to_major_and_minor (dp->p_dev),
			cv_unit_and_target (dp),

			dp->p_head,
			dp->p_sect,
			dp->p_cyl,
			dp->p_head ? (dp->p_flags & DISK_INT13_EXT ? 'L' : 'G') : ' ',
			1 << dp->p_blshift,
			dp->p_nopen,
			dp->p_lock
		);

		printf ("\n");

	}	/* end for (tabela de partições) */

}	/* end pr_secundary */

/*
 ****************************************************************
 *	Examina a tabela de partições de um dispositivo		*
 ****************************************************************
 */
void
read_parttb (const char *dev_nm)
{
	int		fd;
	int		reg_index, log_index;
	const PARTTB	*reg_pp, *pp;
	daddr_t		log_offset, ext_begin, ext_end;
	PR		mbr, br;
	STAT		s;
	HDINFO		hdinfo;
	PARTTB		log_parttb;
	PARTTB		empty;
#if (0)	/*******************************************************/
	const PARTTB	*pp;
#endif	/*******************************************************/

	/*
	 *	Abre o dispositivo
	 */
	if ((fd = open (dev_nm, O_RDONLY)) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"%s\" (%s)\n",
			dev_nm, strerror (errno)
		);
		exit (1);
	}

	if (fstat (fd, &s) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui obter o estado de \"%s\" (%s)\n",
			dev_nm, strerror (errno)
		);
		exit (1);
	}

	if (!S_ISBLK (s.st_mode) && !S_ISCHR (s.st_mode))
	{
		fprintf
		(	stderr,
			"O arquivo \"%s\" não é especial\n",
			dev_nm
		);
		exit (1);
	}

	/*
	 *	Le o MRB
	 */
	if (read (fd, &mbr, sizeof (PR)) != sizeof (PR))
	{
		fprintf
		(	stderr,
			"Não consegui ler o MBR de \"%s\" (%s)\n",
			dev_nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Verifica se o MBR tem 0x55AA
	 */
	if (mbr.pr_magic != 0xAA55)
	{
		fprintf
		(	stderr,
			"Dispositivo \"%s\": O MBR NÃO contém \"0x55AA\"\n",
			dev_nm
		);
	}
#if (0)	/*******************************************************/
	else
	{
		printf ("sizeof (PR) = %d\n", sizeof (PR));
	}
#endif	/*******************************************************/

	/*
	 *	Tenta obter a geometria
	 */
	if (geo_get_parttb_geo (dev_nm, &mbr, &hdinfo) < 0)
	{
		fprintf
		(	stderr,
			"Não consegui obter a geometria de \"%s\"\n",
			dev_nm
		);
		exit (1);
	}

	hdinfo.h_cylsz = hdinfo.h_head * hdinfo.h_sect;

	printf
	(	"Dispositivo \"%s\": %d cabeças, %d setores\n\n",
		dev_nm, hdinfo.h_head, hdinfo.h_sect
	);

	/*
	 *	Imprime as partições regulares
	 */
	for
	(	reg_index = 0, reg_pp = (PARTTB *)mbr.pr_parttb;
		reg_index < NPART;
		reg_index++, reg_pp++
	)
	{
		printf
		(	"=======> Partição %d: %s\n\n",
			reg_index + 1,
			IS_EXT (reg_pp->pt_type) ? "Estendida" : "Regular"
		);

		fdisk_print_parttb_entry (reg_pp);
		print_parttb_trios (reg_pp, &hdinfo, 0);

		printf ("\n");

		/*
		 *	Temos uma partição estendida
		 */
		if (!IS_EXT (reg_pp->pt_type))
			continue;

		ext_begin = reg_pp->pt_offset;
		ext_end   = reg_pp->pt_offset + reg_pp->pt_size;

		log_offset = ext_begin;

		/*
		 *	Percorre a cadeia de partições lógicas
		 */
		for
		(	log_index = 0;
			/* abaixo */;
			log_index++
		)
		{
			if (log_offset < ext_begin || log_offset >= ext_end)
			{
				printf
				(	"\n*** A cadeia de partições lógicas "
					"da partição estendida %d contém "
					"um ponteiro inválido: %d\n\n",
					reg_index, log_offset
				);
				break;
			}

#if (0)	/*******************************************************/
			if (log_offset % hdinfo.h_cylsz)
				printf ("\t*** Endereço do ponteiro %d desalinhado\n\n", log_offset);
#endif	/*******************************************************/

			if (lseek (fd, BLSZ * log_offset, SEEK_SET) == -1)
			{
				fprintf
				(	stderr,
					"Não consegui dar o SEEK do BR (bloco %d) de \"%s\" (%s)\n",
					log_offset, dev_nm, strerror (errno)
				);
				exit (1);
			}

			if (read (fd, &br, sizeof (PR)) != sizeof (PR))
			{
				fprintf
				(	stderr,
					"Não consegui ler o BR (bloco %d) de \"%s\" (%s)\n",
					log_offset, dev_nm, strerror (errno)
				);
				exit (1);
			}

			if (br.pr_magic != 0xAA55)
			{
				fprintf
				(	stderr,
					"Dispositivo \"%s\": O BR (bloco %d) NÃO contém \"0x55AA\"\n",
					log_offset, dev_nm
				);
			}

			pp = (PARTTB *)br.pr_parttb;
#ifdef	DEBUG
			if (fdisk_debug)
			{
				printf
				(	"\nPartição lógica "
					"(estendida %d, bloco %d):\n\n",
					reg_index, log_offset
				);

				fdisk_debug_print_parttb_entry (0, pp + 0);
				fdisk_debug_print_parttb_entry (1, pp + 1);

				printf ("\n");
			}
#endif	DEBUG
			if (pp[0].pt_size == 0)	/* Primeiro método de EOF */
				break;

			/*
			 *	Achou uma partição lógica
			 */
		   	memmove (&log_parttb, pp, sizeof (PARTTB));

		   /***	fdisk_log_offset[reg_index][log_index] = pp[0].pt_offset; ***/

		   	log_parttb.pt_offset += log_offset;

			/*
			 *	Uma partição lógica NÃO pode ser EXT
			 */
			if (IS_EXT (pp->pt_type))
			{
				printf
				(	"\n*** Tipo inválido para partição "
					"lógica: 0x%02X\n\n",
					pp->pt_type
				);
			}

			printf
			(	"Partição lógica %d (Ponteiro %d, %s):\n\n",
				log_index + 1, log_offset,
				log_offset % hdinfo.h_cylsz ? "DESALINHADO" : "alinhado"
			);

			fdisk_print_parttb_entry (&log_parttb);

			printf ("\n");

			fdisk_print_parttb_entry (pp);
			print_parttb_trios (pp, &hdinfo, log_offset);

			printf ("\n");

			/*
			 *	Procura a partição seguinte
			 */
			if (pp[1].pt_size == 0)	/* Segundo método de EOF */
			{
				memset (&empty, 0, sizeof (PARTTB));
		
				if (!memeq (pp + 1, &empty, sizeof (PARTTB)))
					printf ("\t*** Entrada vazia NÃO zerada\n");
		
				break;
			}

			fdisk_print_parttb_entry (pp + 1);
			print_parttb_trios (pp + 1, &hdinfo, ext_begin);

			printf ("\n");

			log_offset = ext_begin + pp[1].pt_offset;

		}	/* end for (cadeia de partições lógicas) */

	}	/* end for (partições regulares) */

}	/* end read_parttb */

/*
 ****************************************************************
 *	Imprime uma entrada da tabela de partições DOS		*
 ****************************************************************
 */
void
fdisk_print_parttb_entry (const PARTTB *pp)
{
	char		type_name[PARTNM_MAXSZ];
	PARTTB		empty;

	/*
	 *	Verifica se a partição está vazia
	 */
	if (pp->pt_size == 0)
	{
		printf ("\tEntrada vazia\n");

		memset (&empty, 0, sizeof (PARTTB));

		if (!memeq (pp, &empty, sizeof (PARTTB)))
			printf ("\t*** Entrada vazia NÃO zerada\n");

		return;
	}

	/*
	 *	A partição NÃO está vazia
	 */
	if   (pp->pt_active == 0x00)
		printf ("   ");
	elif (pp->pt_active == 0x80)
		printf (" * ");
	else
		printf (" %02X", pp->pt_active);

	printf
	(	"%8d " "%8d " "%8d ",
		pp->pt_offset,
		pp->pt_offset + pp->pt_size - 1,
		pp->pt_size
	);

	printf (" %10.10s", cv_blocks_to_KB_MB_or_GB (pp->pt_size));

	printf ("  %02X ", pp->pt_type);

	if (kcntl (K_GET_PARTNM, pp->pt_type, type_name) < 0)
		printf ("??? (0x%02X)", pp->pt_type);
	else
		printf (type_name);

	printf ("\n");

#if (0)	/*******************************************************/
	daddr_t		begin, End;
	/*
	 *	Confere os TRIOs
	 */
	begin = geo_get_linear_blno (&pp->pt_start, gp);
	End   = geo_get_linear_blno (&pp->pt_end, gp);

	printf
	(	"\tBegin = (%4d,%3d,%3d) => %d",
		pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2),
		pp->pt_start.head,
		pp->pt_start.sect & 0x3F,
		begin
	);

	if (pp->pt_offset + offset == begin)
		printf ("\t(OK)\n");
	else
		printf ("\t*** ERRO no TRIO: %d :: %d\n", pp->pt_offset + offset, begin);

	printf
	(	"\tEnd   = (%4d,%3d,%3d) => %d",
		pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2),
		pp->pt_end.head,
		pp->pt_end.sect & 0x3F,
		End
	);

	if (pp->pt_offset + pp->pt_size - 1 + offset == End)
		printf ("\t(OK)\n");
	else
		printf ("\t*** ERRO no TRIO: %d :: %d\n", pp->pt_offset + pp->pt_size - 1 + offset, End);
#endif	/*******************************************************/

}	/* end fdisk_print_parttb_entry */

/*
 ****************************************************************
 *	Imprime os TRIOs					*
 ****************************************************************
 */
void
print_parttb_trios (const PARTTB *pp, const HDINFO *gp, daddr_t offset)
{
	daddr_t		begin, End;

	if (pp->pt_size == 0)
		return;

	begin = geo_get_linear_blno (&pp->pt_start, gp);
	End   = geo_get_linear_blno (&pp->pt_end, gp);

	printf
	(	"\tBegin = (%4d,%3d,%3d) => %d",
		pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2),
		pp->pt_start.head,
		pp->pt_start.sect & 0x3F,
		begin
	);

	if (pp->pt_offset + offset == begin)
		printf ("\t(OK)\n");
	else
		printf ("\t*** ERRO no TRIO: %d :: %d\n", pp->pt_offset + offset, begin);

	printf
	(	"\tEnd   = (%4d,%3d,%3d) => %d",
		pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2),
		pp->pt_end.head,
		pp->pt_end.sect & 0x3F,
		End
	);

	if (pp->pt_offset + pp->pt_size - 1 + offset == End)
		printf ("\t(OK)\n");
	else
		printf ("\t*** ERRO no TRIO: %d :: %d\n", pp->pt_offset + pp->pt_size - 1 + offset, End);

}	/* end print_parttb_trios */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime o conteúdo de uma entrada de partições DOS	*
 ****************************************************************
 */
void
fdisk_debug_print_parttb_entry (int index, const PARTTB *pp)
{
	const PARTNM	*pnm;

	/*
	 *	Procura o tipo na tabela (termina com -1)
	 */
	for (pnm = partnm; pnm->pt_type >= 0; pnm++)
	{
		if (pnm->pt_type == pp->pt_type)
			break;
	}

	printf ("%2d:  ", index + 1);

	if   (pp->pt_active == 0x00)
		printf ("   ");
	elif (pp->pt_active == 0x80)
		printf ("*  ");
	else
		printf ("%02X ", pp->pt_active);

	printf
	(	"%9d %9d  ",
		pp->pt_offset, pp->pt_size
	);

	printf
	(	"(%4d,%2d,%2d) ",
		pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2),
		pp->pt_start.head,
		pp->pt_start.sect & 0x3F
	);

	printf
	(	"(%4d,%2d,%2d)  ",
		pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2),
		pp->pt_end.head,
		pp->pt_end.sect & 0x3F
	);

	if (pnm->pt_type >= 0)
		printf ("%s\n", pnm->pt_nm);
	else
		printf ("%02X\n", pp->pt_type);

}	/* end fdisk_debug_print_parttb_entry */
#endif	DEBUG

/*
 ****************************************************************
 *	Tenta obter a geometria a partir das partições		*
 ****************************************************************
 */
int
geo_get_parttb_geo (const char *dev_nm, const PR *mbr, HDINFO *gp)
{
	/*
	 *	Tenta os dois métodos
	 */
	if (geo_get_std_method (mbr, gp) >= 0)
		return (0);

	if (geo_get_heu_method (mbr, gp) >= 0)
		return (0);

	return (-1);

}	/* end geo_get_parttb_geo */

/*
 ****************************************************************
 *	Método 1: 2 equações com duas incógnitas		*
 ****************************************************************
 */
int
geo_get_std_method (const PR *mbr, HDINFO *gp)
{
	int		index;
	const PARTTB	*pp;
	int		CYLSZ;

	/*
	 *	Calcula o tamanho do cilindro
	 */
	for
	(	index = 0, pp = (PARTTB *)mbr->pr_parttb;
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		if ((CYLSZ = geo_get_cyl (pp)) > 0)
			break;
	}

	/*
	 *	Calcula o tamanho do setor
	 */
	for
	(	index = 0, pp = (PARTTB *)mbr->pr_parttb;
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		gp->h_sect = geo_get_sec
		(
			CYLSZ,
			pp->pt_offset,
			&pp->pt_start
		);

		if (gp->h_sect > 0)
			break;

		gp->h_sect = geo_get_sec
		(
			CYLSZ,
			pp->pt_offset + pp->pt_size - 1,
			&pp->pt_end
		);

		if (gp->h_sect > 0)
			break;
	}

	/*
	 *	Calcula o número de cabeças
	 */
	gp->h_head = CYLSZ / gp->h_sect;

	if (CYLSZ % gp->h_sect)
		return (-1);

	return (0);

}	/* end geo_get_std_method */

/*
 ****************************************************************
 *	Calcula o tamanho do cilindro				*
 ****************************************************************
 */
int
geo_get_cyl (const PARTTB *pp)
{
	ulong		start_bno = pp->pt_offset;
	ulong		end_bno = pp->pt_offset + pp->pt_size - 1;
	ulong		big_b_h, small_b_h;
	ulong		big_h_c, small_h_c;
	ulong		above, below, CYLSZ;

	big_b_h = (start_bno - (pp->pt_start.sect & 0x3F) + 1) * pp->pt_end.head;
	small_b_h = (end_bno - (pp->pt_end.sect & 0x3F) + 1) * pp->pt_start.head;

	big_h_c = pp->pt_end.head * (pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2));
	small_h_c = pp->pt_start.head * (pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2));

	above = big_b_h - small_b_h;
	below = big_h_c - small_h_c;

	if ((int)above < 0)
		{ above = -above; below = -below; }

	if (above == 0 || below == 0)
		return (-1);

	CYLSZ = above / below;

	if (above % below)
		return (-1);

	return (CYLSZ);

}	/* end geo_get_cyl */

/*
 ****************************************************************
 *	Calcula o número de setores por trilha			*
 ****************************************************************
 */
int
geo_get_sec (int CYLSZ, ulong bno, const TRIO *tp)
{
	ulong		above, below;
	int		NSECT;

	if ((below = tp->head) == 0)
		return (-1);

	above =  (bno - (tp->sect & 0x3F) + 1);
	above -= CYLSZ * (tp->cyl | ((tp->sect & 0xC0) << 2));

	if (above == 0)
		return (-1);

	NSECT = above / below;

	if (above % below)
		return (-1);

	return (NSECT);

}	/* end geo_get_sec */

/*
 ****************************************************************
 *    Heurística supondo que a partição termine em um cilindro	*
 ****************************************************************
 */
int
geo_get_heu_method (const PR *mbr, HDINFO *gp)
{
	const PARTTB	*pp;
	int		index;
	ulong		offset, last;

	/*
	 *	Primeiro procura uma partição talvez terminada em cilindro
	 */
	for
	(	index = 0, pp = (PARTTB *)mbr->pr_parttb;
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		gp->h_sect = (pp->pt_end.sect & 0x3F) - 1 + 1;
		gp->h_head =  pp->pt_end.head + 1;

		offset = geo_get_linear_blno (&pp->pt_start, gp);

		if (offset != pp->pt_offset)
			continue;

		last = geo_get_linear_blno (&pp->pt_end, gp);

		if (last + 1 == pp->pt_offset + pp->pt_size)
			break;
	}

	/*
	 *	Agora confere em todas as partições
	 */
	for
	(	index = 0, pp = (PARTTB *)mbr->pr_parttb;
		index < NPART;
		index++, pp++
	)
	{
		if (pp->pt_size == 0)
			continue;

		/* Lembrar os casos de discos com mais de 1024 cilindros */

		if (pp->pt_start.cyl > pp->pt_end.cyl)
			continue;

		offset = geo_get_linear_blno (&pp->pt_start, gp);

		if (offset != pp->pt_offset)
			return (-1);

		last = geo_get_linear_blno (&pp->pt_end, gp);

		if (last + 1 != pp->pt_offset + pp->pt_size)
			return (-1);
	}

	return (0);

}	/* end geo_get_heu_method */

/*
 ****************************************************************
 *	Calcula o bloco linear a partir do trio			*
 ****************************************************************
 */
long
geo_get_linear_blno (const TRIO *tp, const HDINFO *gp)
{
	long		blkno;

	blkno  = gp->h_head * (tp->cyl | ((tp->sect & 0xC0) << 2));
	blkno += tp->head;
	blkno *= gp->h_sect;
	blkno += (tp->sect & 0x3F) - 1;

	return (blkno);

}	/* end geo_get_linear_blno */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime os valores de uma partição			*
 ****************************************************************
 */
void
geo_print_size (int index, const PARTTB *pp)
{
	printf ("%4d:  ", index);

	printf
	(	"%9d %9d  ",
		pp->pt_offset, pp->pt_size
	);

	printf
	(	"(%4d,%3d,%3d) ",
		pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2),
		pp->pt_start.head,
		pp->pt_start.sect & 0x3F
	);

	printf
	(	"(%4d,%3d,%3d)  ",
		pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2),
		pp->pt_end.head,
		pp->pt_end.sect & 0x3F
	);

	printf ("\n");

}	/* end geo_print_size */
#endif	DEBUG







/*
 ****************************************************************
 *	Converte blocos em KB, MB ou GB				*
 ****************************************************************
 */
char *
cv_blocks_to_KB_MB_or_GB (long bl_cnt)
{
	static char	area[32];

#if (0)	/*******************************************************/
	if   (bl_cnt < (10*MBSZ/BLSZ))		/* Menor do que 10 MB */
	{
		bl_cnt += (KBSZ/BLSZ/2);	/* Arredonda 0.5 KB */

		sprintf
		(	area,
			"%d KB",
			(unsigned)bl_cnt >> (KBSHIFT-BLSHIFT)
		);
	}
	elif (bl_cnt < (GBSZ/BLSZ))		/* De 10 MB até 1024 MB */
#endif	/*******************************************************/
	{
		bl_cnt += (MBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 MB */

		sprintf
		(	area,
			"%d.%02d",
#if (0)	/*******************************************************/
			"%d.%02d MB",
#endif	/*******************************************************/
			(unsigned)bl_cnt >> (MBSHIFT-BLSHIFT),
			(100 * ((unsigned)bl_cnt & (MBSZ/BLSZ - 1)))
					>> (MBSHIFT-BLSHIFT)
		);
	}
#if (0)	/*******************************************************/
	else	/* 1024 MB ou acima */
	{
		bl_cnt += (GBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 GB */

		sprintf
		(	area,
			"%d.%02d GB",
			(unsigned)bl_cnt >> (GBSHIFT-BLSHIFT),
			(100 * ((unsigned)bl_cnt & (GBSZ/BLSZ - 1)))
					>> (GBSHIFT-BLSHIFT)
		);
	}
#endif	/*******************************************************/

	return (area);

}	/* end cv_blocks_to_KB_MB_or_GB */

/*
 ****************************************************************
 *	Converte o número do dispositivo			*
 ****************************************************************
 */
char *
cv_dev_to_major_and_minor (int dev)
{
	static char	area[16];

	sprintf (area, "%2d/%d", MAJOR (dev), MINOR (dev));

	return (area);

}	/* end cv_dev_to_major_and_minor */

/*
 ****************************************************************
 *	Converte o par (unit, target)				*
 ****************************************************************
 */
char *
cv_unit_and_target (const DISKTB *dp)
{
	static char	area[16];

	sprintf (area, "%1d/%d", dp->p_unit, dp->p_target);

	return (area);

}	/* end cv_dev_to_major_and_minor */

/*
 ****************************************************************
 *	Imprime o nome do sistema de arquivos TROPIX		*
 ****************************************************************
 */
void
print_fs1_name (const DISKTB *dp)
{
	char		device[30] = "/dev/r";
	int		fd;
	V7SB		sb;

	strcpy (device + 6, dp->p_name);

	if ((fd = open (device, O_RDONLY)) < 0)
		return;

	if
	(	lseek (fd, BLSZ, SEEK_SET) >= 0 &&
		read (fd, &sb, sizeof (V7SB)) == sizeof (V7SB) &&
		long_endian_cv (sb.s_magic) == V7_SBMAGIC &&
		((char *)sb.s_fname)[0] != '\0'
	)
	{
		printf (" (%s)", sb.s_fname);
	}

	close (fd);

}	/* end print_fs1_name */

/*
 ****************************************************************
 *	Imprime o nome do sistema de arquivos TROPIX T1		*
 ****************************************************************
 */
void
print_fs2_name (const DISKTB *dp)
{
	char		device[32] = "/dev/";
	int		fd;
	T1SB		sb;

	strcpy (device + 5, dp->p_name);

	if ((fd = open (device, O_RDONLY)) < 0)
		return;

	if
	(	lseek (fd, T1_SBNO * BL4SZ, SEEK_SET) >= 0 &&
		read (fd, &sb, sizeof (T1SB)) == sizeof (T1SB) &&
		sb.s_magic == T1_SBMAGIC &&
		((char *)sb.s_fname)[0] != '\0'
	)
	{
		printf (" (%s)", sb.s_fname);
	}

	close (fd);

}	/* end print_fs2_name */

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
		"%s - Imprime a tabela de discos/partições\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-av] [<dispositivo>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-a: Imprime informações alternativas\n"
		"\t-v: Verboso\n"
		"\nObs.:\tSe for dado o <dispositivo>, imprime a "
		"sua tabela de partições, ao invés da interna\n"
	);

	exit (2);

}	/* end help */
