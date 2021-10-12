/*
 ****************************************************************
 *								*
 *			main.c					*
 *								*
 *	Módulo principal, funcionando em 32 bits		*
 *								*
 *	Versão	3.0.0, de 05.07.94				*
 *		4.9.0, de 17.11.08				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <sync.h>

#include <a.out.h>
#include <stat.h>
#include <bcb.h>
#include <disktb.h>
#include <t1.h>
#include <cdfs.h>
#include <v7.h>

#include "../h/common.h"
#include "../h/deflate.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
const char	pgversion[] =  "TROPIX boot2, Versão: 4.9.0, de 17.11.08\n";

#define SVGA_ADDR	(2040 * MBSZ)	/* Deve ser igual à "h/mmu.h"	*/

#define	LINESZ		16

#define BOOT_WAIT (15 * 1000)	/* Tempo de espera em ms */

#define	NODIR	(V7DIR *)NULL

entry long	DELAY_value;	/* Valor usado para calibrar DELAY */
entry int	CPU_speed;	/* Velocidade aproximada da CPU em MHz */

extern long	cpu_flags;
extern ulong	end_mem_addr;

/*
 ****** Conversões para MB **************************************
 */
#define KBTOMB_INT(x)	((unsigned)(x) >> (MBSHIFT-KBSHIFT))
#define KBTOMB_DEC(x)	((100 * ((unsigned)(x) & (MBSZ/KBSZ - 1))) >> (MBSHIFT-KBSHIFT))

/*
 ******	Parâmetros do RAMD **************************************
 */
entry void	*ramd_addr;

/*
 ****** Defaults ************************************************
 */
#ifndef	DEFDEV
#error ***** Não foi definido DEFDEV
#endif	DEFDEV

#ifndef	DEFPGM
#error ***** Não foi definido DEFPGM
#endif	DEFPGM

#undef	NODEV	/* Para evitar que XSTR faça a substituição */

const char	make_dev_nm[32] = XSTR (DEFDEV); /* Dispositivo do MAKE */
const char	make_pgm_nm[64] = XSTR (DEFPGM); /* Programa do MAKE */

const char	*def_dev_nm = make_dev_nm;	/* Dispositivo default */
const char	*def_pgm_nm = make_pgm_nm;	/* Programa default */
const char	*def_arq_nm = "boot.gz";	/* Arquivo default */

/*
 ****** Variváveis globais **************************************
 */
entry BCB	bcb;		/* Bloco de informações para o KERNEL */

entry int	cd_boot;	/* O BOOT foi dado por CDROM */

entry long	cpu_type;	/* Tipo da CPU */
extern char	cpu_vendor[16];	/* Nome do fabricante */
entry long	cpu_id;		/* Dados do PENTIUM */
entry long	cpu_features;	/* Mais dados do PENTIUM */
entry long	phys_mem;	/* Memória física (Kb) */
entry int	counted;	/* Contou a Memória na mão */

entry int	vflag;		/* Verboso */
entry int	time_out;	/* O usuário nada teclou */

/*
 ******	Tabela de informações dos discos ************************
 */
entry DISK_INFO	*disk_info_table_ptr = disk_info_table;

/*
 ******	Protótipos de funções ***********************************
 */
void		smpattach (void);
int		do_exec_pgm (const char *, const char *, const char *, int);
#undef	EXTRA
#ifdef	EXTRA
void		do_lc (void);
void		do_copy (void);
#endif	EXTRA
void		do_dev_dump (void);
void		do_mem_dump (void);
void		dump_hex (const char *area, int count, long addr);
int		do_ramd (const char *, const char *);
int		analyse_gzip_header (const char *area);
int		there_is_memory (ulong addr);
void		do_test_a20 (int);
DISKTB		*get_dev (const char *);
int		check_tropix_magic (const DISKTB *);
int		wait_for_char_or_timeout (void);
void		init_DELAY (void);
void		do_delay_tst (void);
void		help (void);


/*
 ****************************************************************
 *	Programa principal do BOOT2				*
 ****************************************************************
 */
void
main (void)
{
	const DISKTB	*pp;
	const DISK_INFO	*dp;
	char		yes = 0, total = 0;
	char		line[120];
	char		dev_nm_str[32];
	const char	*cp, *nm = NOSTR;
	char		*bp;
	ulong		low_value, high_value, tst_value;
	const char	*dev_nm, *pgm_nm;
	int		code;
#ifdef	EXTRA
	enum { EXEC, TABLE, FDISK, LIST, DUMP, MEM, COPY, RAMD, DELAY_TST } func;
#else
	enum { EXEC, TABLE, FDISK, DUMP, MEM, RAMD, INSTALL, DELAY_TST } func;
#endif	EXTRA
#undef	VESA
#ifdef	VESA
	extern short	intr_addr_limit;
	extern void	**intr_addr_base;
#endif	VESA

   /***	printf ("%s", pgversion); ***/		/* Em seg.32.s */

	printf
	(	"\e[32m"
		"\nCopyright © 1988-2007 NCE/UFRJ\n"
		"\e[0m"
	);

	malloc_mem0 = malloc_area_begin;	/* Inicializa "malloc" */

	/*
	 ****** Informa sobre erro do SVGA **********************
	 */
	if (supervga_code)
		printf ("\nNÃO foi possível usar o modo SVGA (código %d)\n\n", supervga_code);

	/*
	 ******	Obtém o total de memória disponível *************
	 */
	if ((tst_value = end_mem_addr) < 8 * MBSZ)
	{
		/* Realiza a contagem de memória */

		counted++;
		low_value  = 0    * MBSZ;
		high_value = 2024 * MBSZ;
		tst_value  = 2024 * MBSZ;

		for (EVER)
		{
			/* Testa o limite superior */

			if (there_is_memory (tst_value) >= 0)
				low_value  = tst_value;
			else
				high_value = tst_value;

			if (high_value - low_value <= 16 * BL4SZ)
				break;

			/* Acha o ponto médio */

			tst_value = (low_value + high_value) >> 1;
		}

		/*
		 *	Vai baixando até encontrar o final da memória
		 */
		for (tst_value = (high_value + BL4SZ) & ~BL4MASK; /* abaixo */; tst_value -= BL4SZ)
		{
			if (there_is_memory (tst_value) >= 0)
				break;
		}
	}

	if (tst_value > 2 * GBSZ)
	{
		printf ("\nMemória de tamanho excessivo: truncando a 2 GB\n");
		tst_value = 2 * GBSZ;
	}

	phys_mem = tst_value / KBSZ;

#undef	MEM_DEBUG
#ifdef	MEM_DEBUG
	print_bios_mem_tb ();
#endif	MEM_DEBUG

	/*
	 ******	Inicializa "DELAY" e avalia a velocidade da CPU *
	 */
	init_DELAY ();

	/*
	 ******	Analisa o tipo de processador *******************
	 */
	printf ("\nProcessador");

	if (streq (cpu_vendor, "GenuineIntel") && (cpu_id & 0xF00) > 0x300)
	{
		switch (cpu_id & 0x3000)
		{
		    case 0x1000:
			printf (" Overdrive");
			break;

		    case 0x2000:
			printf (" Dual");
			break;
		}

		switch (cpu_id & 0xF00)
		{
		    case 0x400:
			printf (" 486");
			break;

		    case 0x500:
			printf (" PENTIUM");

		        switch (cpu_id & 0xF0)
			{
			    case 0x00:
			        printf (" A-step");
				break;

			    case 0x10:
			        printf ("/P5");
				break;

			    case 0x20:
			        printf ("/P54C");
				break;

			    case 0x30:
			        printf ("/P54T Overdrive");
				break;

			    case 0x40:
			        printf ("/P55C");
				break;

			    case 0x70:
			        printf ("/P54C");
				break;

			    case 0x80:
			        printf ("/P55C (quarter-micron)");
				break;

			    default:
			        /* nothing */
				break;

			}	/* end switch */

			break;

		    case 0x600:
			printf (" PENTIUM");

			switch (cpu_id & 0xF0)
			{
			    case 0x00:
			        printf (" Pro A-step");
				break;

			    case 0x10:
			        printf (" Pro");
				break;

			    case 0x30:
			    case 0x50:
			    case 0x60:
				printf (" II/PENTIUM II Xeon/Celeron");
				break;

			    case 0x70:
			    case 0x80:
			    case 0xA0:
				printf (" III/PENTIUM III Xeon");
				break;

			    case 0xB0:
				printf (" III Tualatin");
				break;

			    default:
				printf (" (desconhecido %04X)", cpu_id);
				break;
			}

			break;

		    case 0xF00:
			printf (" PENTIUM");
			printf (" IV");

#if (0)	/*******************************************************/
			switch (cpu_id & 0xFF)
			{
			    case 0x24:
			    case 0x25:
			    case 0x26:
			    case 0x27:
				printf (" IV");
				break;

			    default:
				printf (" (desconhecido %04X)", cpu_id);
				break;
			}
#endif	/*******************************************************/

			break;

		    default:
			printf (" (desconhecido %04X)", cpu_id);
			break;
		}

		switch (cpu_id & 0xFF0)
		{
		    case 0x400:
		    case 0x410:
			printf (" DX"); break;

		    case 0x420:
			printf (" SX"); break;

		    case 0x430:
			printf (" DX2"); break;

		    case 0x440:
			printf (" SL"); break;

		    case 0x450:
			printf (" SX2"); break;

		    case 0x470:
			printf (" DX2 Write-Back Enhanced");
			break;

		    case 0x480:
			printf (" DX4"); break;

		}	/* end switch (cpu_id & 0xFF0) */

	}
	elif (streq (cpu_vendor, "AuthenticAMD"))
	{
		printf (" AMD ");

		switch (cpu_id & 0xFF0)
		{
		    case 0x410:
			printf ("Standard Am486DX");
			break;

		    case 0x430:
			printf ("Am486DX2/4 Write-Through");
			break;

		    case 0x470:
			printf ("Enhanced Am486DX4 Write-Back");
			break;

		    case 0x480:
			printf ("Enhanced Am486DX4 Write-Through");
			break;

		    case 0x490:
			printf ("Enhanced Am486DX4 Write-Back");
			break;

		    case 0x4E0:
			printf ("Am5x86 Write-Through");
			break;

		    case 0x4F0:
			printf ("Am5x86 Write-Back");
			break;

		    case 0x500:
			printf ("K5 model 0");
			break;

		    case 0x510:
			printf ("K5 model 1");
			break;

		    case 0x520:
			printf ("K5 PR166 (model 2)");
			break;

		    case 0x530:
			printf ("K5 PR200 (model 3)");
			break;

		    case 0x560:
			printf ("K6");
			break;

		    case 0x570:
			printf ("K6 266 (model 1)");
			break;

		    case 0x580:
			printf ("K6-2");
			break;

		    case 0x590:
			printf ("K6-III");
			break;

		    case 0x620:
			printf ("Athlon");
			break;

		    default:
			printf ("(desconhecido)");
			break;
		}
	}
	elif (streq (cpu_vendor, "CyrixInstead"))
	{
		printf (" CYRIX ");

		switch (cpu_id & 0xFF0)
		{
		    case 0x440:
			printf ("MediaGX");
			break;

		    case 0x520:
			printf ("6x86");
			break;

		    case 0x540:
			printf ("GXm");
			break;

		    case 0x600:
			printf ("6x86MX");
			break;

		    default:
			printf ("(desconhecido)");
			break;
		}
	}
	else	/* NOT GenuineIntel */
	{
		if (cpu_type == 586)
			printf (" 586");
		else
			printf (" %d", cpu_type);

	}

	if (CPU_speed)
		printf (", %d MHz", CPU_speed);

	if (cpu_vendor[0])
		printf (", Origem = \"%s\"\n", cpu_vendor);
	else
		printf ("\n");

	printf ("ID = %04X", cpu_id);

	if (cpu_features != 0)
	{
		printf
		(	", Features = 0x%B",
			cpu_features,
			"\x1A" "XMM"	/* Katami SIMD/MMX2 instructions */
			"\x19" "FXSR"	/* FXSAVE/FXRSTOR */
			"\x18" "MMX"	/* MMX instructions */
			"\x13" "PN"	/* Processor Serial number */
			"\x12" "PSE36"	/* 36 bit address space support */
			"\x11" "PAT"	/* Page attributes table */
			"\x10" "CMOV"	/* CMOV instruction */
			"\x0E" "PGE"	/* PG_G (global bit) support */
			"\x0A" "APIC"	/* SMP local APIC */
			"\x09" "CX8"	/* CMPEXCH8 instruction */
			"\x08" "MCE"	/* Machine Check support */
		   	"\x07" "PAE"	/* Physical address extension */
			"\x06" "MSR"	/* Machine specific registers */
			"\x05" "TSC"	/* Timestamp counter */
			"\x04" "PSE"	/* 4MByte page tables */
			"\x03" "IOB"
			"\x02" "VME"	/* Extended VM86 mode support */
			"\x01" "FPU"	/* Integral FPU */
		);
	}

	printf ("\n");

	/*
	 *	Imprime o tamanho da memória
	 */
	printf
	(	"Memória = %d.%02d MB (%d KB, %c)\n",
		KBTOMB_INT (phys_mem), KBTOMB_DEC (phys_mem), phys_mem, counted ? 'C' : 'M'
	);

	bcb.y_version	   = BCB_VERSION;

	bcb.y_cputype	   = cpu_type;
	bcb.y_cpu_features = cpu_features;
	bcb.y_DELAY_value  = DELAY_value;
	bcb.y_physmem	   = phys_mem;

	/*
	 *	Imprime se a BIOS usa interrupções
	 */
	printf ("BIOS %s Interrupções", (cpu_flags & 0x200) ? "COM" : "SEM");

	/*
	 *	Verifica se a BIOS tem as extensões da INT 13
	 */
	for (dp = disk_info_table; dp->info_present; dp++, total++)
	{
		if (dp->info_int13ext)
			yes++;
	}

	if (total) printf
	(
		", %s Extensões da INT 13%s",
		yes == 0 ? "SEM" : "COM",
		yes == 0 ? "" : (yes == total ? "" : " para ALGUNS dos discos")
	);

	printf ("\n");

	cd_boot = (boot1_devcode == 0xFE);

	if (cd_boot)
		printf ("BOOT através de um CDROM\n");

#if (0)	/*******************************************************/
	/*
	 *	Verifica se o BOOT foi dado a partir de um CDROM
	 *	(consulta a estrutura retornada pela INT 13/4B, ainda em 16 bits)
	 */
	if (boot_info.b_size == 0x13) /* && (boot_info.b_media & 0x0F) == 2) */
	{
		cd_boot = 1;

		printf ("BOOT através de um CDROM:\n");

		printf
		(	"   status = %04X, size = %02X, media = %02X, drive = %02X, index = %02X\n",
			boot_info.b_status, boot_info.b_size, boot_info.b_media,
			boot_info.b_drive, boot_info.b_ctrl_index
		);

		printf
		(	"   lba = %P, dspec = %04X, area = %04X, load = %04X, sectors = %d\n", 
			boot_info.b_lba, boot_info.b_drive_spec, boot_info.b_area_seg,
			boot_info.b_load_seg, boot_info.b_sector_cnt
		);

		printf
		(	"   res[0] = %02X, res[1] = %02X, res[2] = %02X\n",
			boot_info.b_res[0], boot_info.b_res[1], boot_info.b_res[2]
		);
	}
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	/*
	 *	Multiprocessamento
	 */
	smpattach ();
#endif	/*******************************************************/

	/*
	 *	Imprime os valores obtidos
	 */
#ifdef	VESA

	printf ("intr_addr_limit = %d, intr_addr_base = %P\n", intr_addr_limit, intr_addr_base);

	printf ("Vetor[15] = %P\n", intr_addr_base[15]);
	printf ("Vetor[16] = %P\n", intr_addr_base[16]);
	printf ("Vetor[17] = %P\n", intr_addr_base[17]);

	bcb.y_int_10_addr = intr_addr_base[16];
	
#endif	VESA




	/*
	 *	Varre o barramento PCI
	 */
	pci_init ();

	/*
	 *	Obtém os tipos dos disquetes
	 */
	fdattach ();

	/*
	 *	Lê a tabela dos discos ATA
	 */
	ataattach ();

	/*
	 *	Inicializa o controlador SCSI Adaptec 1542
	 */
	sdattach ();

	/*
	 *	Ordena os dispositivos SCSI
	 */
	scsi_sort ();

	/*
	 *	Lê as tabelas de particão
	 */
	get_all_parttb ();

	/*
	 *	Lê a linha de controle do usuário
	 *
	 *		[-x] [-v] [-<dev>] [<pgm>]
	 *		 -t  [-v]
	 *		 -f  [-v]
	 *		 -l  [-v]	Só com EXTRA
	 *		 -c  [-v]	Só com EXTRA
	 *		 -C
	 *		 -d  [-v]
	 *		 -m  [-v]
	 *		 -r  [-v] [-<dev>] [<arq>]
	 *		 -s  [-v]
	 *		 -i  [-v]
	 */
    prompt:
	printf ("\nboot> ");

	if (time_out || wait_for_char_or_timeout () < 0)
		{ printf ("\n"); cp = ""; }
	else
		cp = gets (line);

	if (cp[0] == '?')
		{ help (); goto prompt; }

	/*
	 *	Se não foi dado dispositivo "default", ...
	 */
	if   (cd_boot)
	{
		def_dev_nm = NOSTR;

		/* Procura o CDROM de onde o BOOT foi dado */

		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '\0')
			{
				if (def_dev_nm == NOSTR)
					def_dev_nm = "fd0";
				break;
			}

			if (pp->p_blshift == BLSHIFT + 2)
			{
				def_dev_nm = boot1_dev_nm = pp->p_name;

				if (pp->p_size != 0)	/* Há meio presente ? */
					break;
			}
		}
	}
	elif (streq (make_dev_nm, "NODEV") || streq (make_dev_nm, "nodev"))
	{
		def_dev_nm = NOSTR;

		/* Procura a 1ª partição TROPIX (V7 ou T1) ativa */

		for (pp = disktb; /* abaixo */; pp++)
		{
			if (pp->p_name[0] == '\0')
			{
				if (def_dev_nm == NOSTR)
					def_dev_nm = "fd0";
				break;
			}

			if (pp->p_type != TROPIX_T1 && pp->p_type != TROPIX_V7)
				continue;

			if (pp->p_flags & DISK_ACTIVE)
				{ def_dev_nm = pp->p_name; break; }

			if (def_dev_nm == NOSTR)
				def_dev_nm = pp->p_name;
		}
	}

	/*
	 *	Valores iniciais
	 */
	pgm_nm = def_pgm_nm;
	dev_nm = NOSTR;

	vflag = 0; 	func = EXEC;

	/*
	 *	analisa a linha lida
	 */
    next:
	for (/* acima */; (*cp == ' ' || *cp == '\t'); cp++)
		/* vazio */;

	/*
	 *	Verifica se foi dada uma opção
	 */
	if (cp[0] == '-' && (cp[2] == ' ' || cp[2] == '\t' || cp[2] == '\0'))
	{
		switch (cp[1])
		{
		    case 'H':
			{ help (); goto prompt; }

		    case 'v':
			{ vflag++; cp += 2; goto next; }

		    case 'x':
			{ func = EXEC; cp += 2; goto next; }

		    case 't':
			{ func = TABLE; cp += 2; goto next; }

		    case 'f':
			{ func = FDISK; cp += 2; goto next; }
#ifdef	EXTRA
		    case 'l':
			{ func = LIST; cp += 2; goto next; }

		    case 'c':
			{ func = COPY; cp += 2; goto next; }
#endif	EXTRA
		    case 'C':
			{ cd_boot ^= 1; printf ("\nCD_BOOT %sligado\n", cd_boot ? "" : "des"); goto prompt; }

		    case 'd':
			{ func = DUMP; cp += 2; goto next; }

		    case 'm':
			{ func = MEM; cp += 2; goto next; }

		    case 'r':
			{ func = RAMD; cp += 2; goto next; }

		    case 'i':
			{ func = INSTALL; cp += 2; goto next; }

		    case 's':
			{ func = DELAY_TST; cp += 2; goto next; }

		    default:
			printf ("Opção inválida: \"-%c\"\n", cp[1]);
			help ();
			goto prompt;

		}	/* end switch */

	}	/* end if ("-x") */

	/*
	 *	Procura fontes do nome do dispositivo
	 */
	if (cp[0] == '-') 		/* Verifica se foi dado o nome do dispositivo */
	{
		bp = dev_nm_str;	cp++;

		while (*cp != ' ' && *cp != '\t' && *cp != '\0')
			*bp++ = *cp++;

		*bp++ = '\0'; 	dev_nm = dev_nm_str;	goto next;
	}

	if (dev_nm == NOSTR)		/* Verifica se herdou de "boot1" */
		dev_nm = boot1_dev_nm;

	if (dev_nm == NOSTR)		/* Usa o valor "default" */
		dev_nm = def_dev_nm;

	/*
	 *	Guarda <pgm> ou <tb>
	 */
	if (cp[0] == '\0')
		nm = NOSTR;
	else
		nm = cp;

	/*
	 *	Executa a função
	 */
	switch (func)
	{
	    case EXEC:
		if (nm != NOSTR)
			pgm_nm = nm;

		do_exec_pgm (dev_nm, def_dev_nm, pgm_nm, 1 /* confirma */);
		goto prompt;

	    case TABLE:
		prtable ();
		goto prompt;

	    case FDISK:
		fdisk ();
		goto prompt;

#ifdef	EXTRA
	    case LIST:
		do_lc ();
		goto prompt;

	    case COPY:
		do_copy ();
		goto prompt;
#endif	EXTRA

	    case DUMP:
		do_dev_dump ();
		goto prompt;

	    case MEM:
		do_mem_dump ();
		goto prompt;

	    case RAMD:
		if (nm != NOSTR)
			pgm_nm = nm;

		do_ramd (dev_nm, pgm_nm);
		goto prompt;

	    case INSTALL:
		printf ("\nDescompactando a imagem da RAIZ. Isto pode levar algum tempo ...\n\n");

		draw_file_bar = 1;

		if (cd_boot)
			code = do_ramd (def_dev_nm, def_arq_nm);
		else
			code = do_ramd ("fd0", def_arq_nm);

		draw_file_bar = 0;

		printf ("\n");

		if (code >= 0)
			do_exec_pgm ("ramd0", "ramd0", "tropix", 0 /* Não confirma */);

		goto prompt;

	    case DELAY_TST:
		do_delay_tst ();
		goto prompt;

	}	/* end switch */

}	/* end main */

/*
 ****************************************************************
 *	Carrega e executa o programa				*
 ****************************************************************
 */
int
do_exec_pgm (const char *dev_nm, const char *alt_dev_nm, const char *pgm_nm, int ask)
{
	DISKTB		*pp;
	HEADER		h, *hp;
	char		area[BL4SZ];
	long		tsize, dsize, bsize, ssize;
	long		symtb, memp, entry_pt;
	daddr_t		lbno = 0, daddr;
	int		i, code;
	int		count = BL4SZ, shift = 3, offset;
	char		gzip = 0, p_type;

	/*
	 *	Procura o dispositivo e arquivo
	 */
	if (ask)
	{
		printf ("\nCarrega (%s, %s)? (s): ", dev_nm, pgm_nm);

		if   (time_out)
		{
			printf ("s\n");
		}
		elif (askyesno (1) == 0)
		{
			if (streq (dev_nm, alt_dev_nm))
				return (-1);

			dev_nm = alt_dev_nm;

			printf ("\nCarrega (%s, %s)? (s): ", dev_nm, pgm_nm);

			if (askyesno (1) == 0)
				return (-1);
		}
	}

	if ((pp = get_dev (dev_nm)) == NODISKTB)
		goto bad;

	code = check_tropix_magic (pp);

	switch (code)
	{
	    case FS_T1:
	    {
		T1DINO		*np;

		if ((np = t1_iname (pp, pgm_nm)) == NOT1DINO)
		{
			printf
			(	"\e[31m"
				"\nPara iniciar uma instalação, tecle \"-i\"\n"
				"\e[0m"
			);
			goto bad;
		}

		if (!S_ISREG (np->n_mode))
			{ printf ("\nO arquivo não é regular\n"); goto bad; }

		if ((np->n_mode & 0111) == 0)
			printf ("\nADVR: O arquivo não é executável\n");

		count = BL4SZ; shift = 3; file_size = np->n_size; p_type = TROPIX_T1;

		break;
	    }

	    case FS_CD:
	    {
		CDDIR		*dp;

		if ((dp = cd_iname (pp, pgm_nm)) == NOCDDIR)
			goto bad;

		if ((dp->d_flags & ISO_IFMT) != ISO_REG)
			{ printf ("\nO arquivo não é regular\n"); goto bad; }

		count = ISO_BLSZ; shift = 2; file_size = ISO_GET_LONG (dp->d_size); p_type = 0;

		break;
	    }

	    case FS_V7:
	    {
		V7DINO		*np;

		if ((np = v7_iname (pp, pgm_nm)) == NOV7DINO)
			goto bad;

		if (!S_ISREG (np->n_mode))
			{ printf ("\nO arquivo não é regular\n"); goto bad; }

		if ((np->n_mode & 0111) == 0)
			printf ("\nADVR: O arquivo não é executável\n");

		count = BLSZ; shift = 0; file_size = np->n_size; p_type = TROPIX_V7;

		break;
	    }

	    default:
		goto bad;
	}

	symtb = 1;	/* No momento sempre carregando */

	/*
	 *	Lê o primeiro bloco do arquivo
	 */
	if ((daddr = daddrvec[lbno++]) == 0)
	{
	    premeof:
		printf ("\nEOF inesperado do arquivo\n");
		goto bad;
	}

	if (bread (pp, daddr << shift, area, count) < 0)
		goto bad;

	/*
	 *	Verifica se o arquivos está comprimido
	 */
	if ((offset = analyse_gzip_header (area)) >= 0)
	{
		file_skip   = offset;
		file_header = hp = &h;
		file_dst    = NOVOID;
	   /***	file_size   = ...; /* acima ***/
		file_count  = count;
		file_pp     = pp;
		file_shift  = shift;

		inflate ();

		gzip = 1;
	}
	else
	{
		hp = (HEADER *)area;
	}

	/*
	 *	Ve se é 407 - aceita BIG ou LITTLE endian
	 */
	if (hp->h_magic != FMAGIC)
	{
		printf ("\nO cabeçalho do Arquivo não é 407\n");
		return (-1);
	}

	tsize	 = hp->h_tsize;
	dsize	 = hp->h_dsize;
	bsize	 = hp->h_bsize;
	ssize	 = hp->h_ssize;
	entry_pt = hp->h_entry;

	/*
	 *	Verifica a SYMTB
	 */
	if (symtb)
	{
		if (ssize == 0)
		{
			printf ("\nO Programa não possui tabela de símbolos\n");
			symtb = 0;
		}
	}
	else	/* não pediu symtb */
	{
		ssize = 0;
	}

	/*
	 *	Verifica se pode carregar na memória abaixo de 640Kb
	 */
	if (tsize + dsize + bsize + ssize > 640 * KBSZ)
	{
		printf ("\nO programa + tabela de símbolos ultrapassa 640 Kb\n");
		return (-1);
	}

	bcb.y_rootdev = pp->p_dev;
	bcb.y_ssize = ssize;

	if (gzip == 0)
	{
		/*
		 *	Copia bloco 0 do texto para a memoria
		 */
		memmove ((char *)0, area + sizeof (HEADER), count - sizeof (HEADER));

		/*
		 *	carrega restante do texto + data + symtb
		 */
		for
		(	memp = count - sizeof (HEADER);
			memp < tsize + dsize + ssize;
			memp += count
		)
		{
			if ((daddr = daddrvec[lbno++]) == 0)
				goto premeof;
	
			if (bread (pp, daddr << shift, (void *)memp, count) < 0)
				return (-1);
		}
	}
#if (0)	/*******************************************************/
	else
	{
		memmove (NOVOID, (void *)sizeof (HEADER), (tsize + dsize + bsize));
	}
#endif	/*******************************************************/

	/*
	 *	Copia a SYMTB para o local correto
	 */
	if (symtb)
	{
		memmove
		(	(void *)(tsize + dsize + bsize),
			(void *)(tsize + dsize),
			ssize
		);
	}

#if (0)	/*******************************************************/
	/*
	 *	Acerta os endereços da "symtb"
	 */
	if (symtb && big_endian)
	{
		SYM	*sp, *endsymtb;

		sp	 = (SYM *)(tsize + dsize + bsize);
		endsymtb = (SYM *)((char *)sp + ssize);

		for (/* acima */; sp < endsymtb; sp++)
		{
		   /***	sp->s_res2 =  ENDIAN_SHORT (sp->s_res2); ***/
			sp->s_value = ENDIAN_LONG (sp->s_value);
		}
	}
#endif	/*******************************************************/

	/*
	 *	Zera BSS
	 */
	memset ((void *)(tsize + dsize), 0, bsize);

	/*
	 *	Verifica se deseja executar o programa
	 */
#if (0)	/*******************************************************/
	fd_close_all ();	/* Desliga as unidades de disquette */

printf ("entry_pt = %P\n", entry_pt);
#endif	/*******************************************************/

	if (ask)
	{
		printf ("\nExecuta (%s, %s)? (s): ", pp->p_name, pgm_nm);

		if   (time_out)
			printf ("s\n");
		elif (askyesno (1) == 0)
			return (-1);
	}

	/*
	 *	Calcula o tamanho da tabela de discos
	 */
	if (pp->p_type == 0)
		pp->p_type = p_type;

	for (pp = disktb; pp->p_name[0] != '\0'; pp++)
		/* vazio */;

	bcb.y_disktb = disktb;
	bcb.y_disktb_sz = (int)pp - (int)disktb + sizeof (DISKTB);

#if (0)	/*******************************************************/
	bcb.y_video_pos = video_pos;
#endif	/*******************************************************/
	bcb.y_video_pos = 23 * 80;

	/*
	 *	Desvia para executar o programa
	 */
	fd_close_all ();	/* Desliga as unidades de disquette */

	i = (*((int (*) (BCB *))entry_pt)) (&bcb);

	printf
	(	"\nO programa \"%s\" retornou com o código %d\n",
		pgm_nm, i
	);

	return (-1);

	/*
	 *	Em caso de erro, ...
	 */
    bad:
#if (0)	/*******************************************************/
	fd_close_all ();	/* Desliga as unidades de disquette */
#endif	/*******************************************************/

	return (-1);

}	/* end do_exec_pgm */

#ifdef	EXTRA
/*
 ****************************************************************
 *	Imprime o conteúdo de um diretório			*
 ****************************************************************
 */
void
do_lc (void)
{
	const DISKTB	*pp;
	V7DINO		*ip;
	V7DIR		*dp, *end_dp;
	char		area[BLSZ];
	long		i = 0, n;
	daddr_t		lbno = 0, pbno;

	/*
	 *	Dispositivo de entrada
	 */
	printf ("\nDispositivo de entrada: "); 	gets (area);

	if ((pp = get_dev (area)) == NODISKTB)
		return;

	if (check_tropix_magic (pp) < 0)
		return;

	/*
	 *	Nome do arquivo
	 */
	printf ("\nNome do diretório: "); 	gets (area);

	if ((ip = v7_iname (pp, area)) == NOV7DINO)
		return;

	/*
	 *	Verifica se é um diretório
	 */
	if (!S_ISDIR (ip->n_mode))
	{
		printf ("\nO arquivo \"%s\" NÃO é um diretório\n", area);
		return;
	}

	/*
	 *	Le o diretório
	 */
	for (n = ip->n_size, dp = end_dp = NODIR; n > 0; n -= sizeof (V7DIR), dp++)
	{
		if (dp >= end_dp)
		{
			if ((pbno = daddrvec[lbno++]) == 0)
				break;

			if (bread (pp, pbno, area, BLSZ) < 0)
				break;

			dp = (V7DIR *)area; end_dp = (V7DIR *)(area + BLSZ);
		}

		if (dp->d_ino == 0)
			continue;

		if (streq (dp->d_name, ".") || streq (dp->d_name, ".."))
			continue;

		if (i % 5 == 0)
			printf ("\n");
	
		printf (" %14s", dp->d_name);

		i++;
	}

	if (i > 0)
		printf ("\n");

}	/* end do_lc */

/*
 ****************************************************************
 *	Cópia geral						*
 ****************************************************************
 */
void
do_copy (void)
{
	const DISKTB	*in_pp, *out_pp;
	const V7DINO	*ip;
	char		*cp;
	const char	*ccp;
	const char	*dev_nm;
	int		i, n = 0, in_off, out_off, bl_sz = 0;
	char		area[32];
	daddr_t		daddr;
	char		old_boot[BLSZ];

	/*
	 *	Dispositivo de entrada
	 */
	printf ("\nDispositivo de entrada (\"%s\"): ", def_dev_nm); gets (area);

	if (area[0] == '\0')
		dev_nm = def_dev_nm;
	else
		dev_nm = area;

	if ((in_pp = get_dev (dev_nm)) == NODISKTB)
		return;

	/*
	 *	Nome do arquivo
	 */
	printf ("\nArquivo (ou <nl> para ler \"%s\"): ", in_pp->p_name);
	gets (area);

	if (area[0] == '\0')
	{
		ip = NOV7DINO;
	}
	else
	{
		if (check_tropix_magic (in_pp) < 0)
			return;

		if ((ip = v7_iname (in_pp, area)) == NOV7DINO)
			return;

		if (!S_ISREG (ip->n_mode))
			{ printf ("\nO arquivo não é regular\n"); return; }

		if ((bl_sz = BYTOBL (ip->n_size)) <= 0)
			{ printf ("\nO arquivo tem tamanho NULO\n"); return; }
	}

	/*
	 *	Deslocamento da entrada
	 */
	printf ("\nDeslocamento da entrada (0): "); 	gets (area);

	if   (area[0] == 'n')
		return;
	elif (area[0] == '\0')
		in_off = 0;
	elif ((in_off = atol (area)) < 0)		/* EOF */
		return;

	if (ip != NOV7DINO && in_off >= bl_sz)
		{ printf ("\nO arquivo possui apenas %d blocos\n", bl_sz); return; }
 
	/*
	 *	Número de blocos
	 */
	if (ip != NOV7DINO)
	{
		printf ("\nNúmero de blocos (%d): ", bl_sz - in_off);
		gets (area);

		if (area[0] == 'n')
			return;

		if (area[0] == '\0')
			n = 0;
		else
			n = atol (area);

		if   (n == 0)
			n = bl_sz - in_off;
		elif (n > bl_sz - in_off)
			{ printf ("\nO arquivo possui apenas %d bloco(s)\n", bl_sz); return; }
	}
	else
	{
		printf ("\nNúmero de blocos: "); 	gets (area);

		if (area[0] == 'n')
			return;

		n = atol (area);
	}

	if (n <= 0)			/* EOF */
		return;

	/*
	 *	Lê a entrada
	 */
	if (ip == NOV7DINO)
	{
		for (i = 0, cp = (char *)(2 * MBSZ); i < n; i++, cp += BLSZ)
		{
			if (bread (in_pp, in_off + i, cp, BLSZ) < 0)
				return;
		}
	}
	else	/* Arquivo regular */
	{
		for (i = 0, cp = (char *)(2 * MBSZ); i < n; i++, cp += BLSZ)
		{
			if ((daddr = daddrvec[in_off + i]) <= 0)
				{ printf ("\nEOF prematuro\n"); return; }

			if (bread (in_pp, daddr, cp, BLSZ) < 0)
				return;
		}
	}

	/*
	 *	Dispositivo de saída
	 */
	printf ("\nDispositivo de saída: "); 		gets (area);

	if ((out_pp = get_dev (area)) == NODISKTB)
		return;

	/*
	 *	Deslocamento da saída
	 */
	printf ("\nDeslocamento da saída (0): "); 	gets (area);

	if   (area[0] == 'n')
		return;
	elif (area[0] == '\0')
		out_off = 0;
	elif ((out_off = atol (area)) < 0)		/* EOF */
		return;

	/*
	 *	Verifica se vai reescrever o boot0
	 */
	ccp = out_pp->p_name;

	if (out_off == 0 && (ccp[0] == 'h'|| ccp[0] == 's') && ccp[3] == '\0')
	{
		printf
		(	"\nO bloco 0 do dispositivo \"%s\" será modificado\n",
			ccp
		);

		printf ("\nAtualiza a tabela de partições? (n): ");

		if (askyesno (0) <= 0)
		{
			if (bread (out_pp, 0, old_boot, BLSZ) < 0)
				return;

			cp = (char *)(2 * MBSZ);

			memmove (cp + 446, old_boot + 446, 64);

			cp[510] = 0x55; 	cp[511] = 0xAA;

			printf ("\nConservando a tabela de partições original!\n");
		}
		else
		{
			printf ("\nAtualizando a tabela de partições!\n");
		}
	}
 
	/*
	 *	Pede a confirmação de escrita
	 */
	printf ("\nÚltima chance antes de escrever em \"%s\"", out_pp->p_name);

	if (out_pp->p_type != 0)
	{
		printf (" (\"");
		print_part_type_nm (out_pp->p_type);
		printf ("\")");
	}

	printf (". Continua? (n): ");

	if (askyesno (0) <= 0)
		return;

	/*
	 *	Escreve a saída
	 */
	for (i = 0, cp = (char *)(2 * MBSZ); i < n; i++, cp += BLSZ)
	{
		if (bwrite (out_pp, out_off + i, cp, BLSZ) < 0)
			return;
	}

}	/* end do_copy */
#endif	EXTRA

/*
 ****************************************************************
 *	Imprime DUMP de um dispositivo				*
 ****************************************************************
 */
void
do_dev_dump (void)
{
	const DISKTB	*pp;
	char		area[BLSZ];
	int		i;

	/*
	 *	Procura o dispositivo
	 */
	printf ("\nNome do dispositivo: "); 		gets (area);

	if ((pp = get_dev (area)) == NODISKTB)
		return;

	for (EVER)
	{
		printf ("\nNúmero do bloco (0): "); 	gets (area);

		if (area[0] == 'n')
			return;

		if   (area[0] == '\0')
			i = 0;
		elif ((i = atol (area)) < 0)		/* EOF */
			return;

		for (EVER)
		{
			printf ("\nBloco %d:\n\n", i);

			if (bread (pp, i, area, BLSZ) < 0)
				return;

			dump_hex (area, BLSZ, 0);

			printf ("Continua? (s): ");

			if (askyesno (1) <= 0)
				break;

			i++;
		}
	}

}	/* end do_dev_dump */

/*
 ****************************************************************
 *	Imprime DUMP da memória					*
 ****************************************************************
 */
void
do_mem_dump (void)
{
	unsigned int	i;
	char		area[32];

	printf ("\nEndereço inicial (hexadecimal): "); gets (area);

	if (area[0] == 'n')
		return;

	i = xtol (area) & ~0x0F;		/* Alinha em 16 */

	for (EVER)
	{
#if (0)	/*******************************************************/
		if ((j = i) >= (unsigned)SYS_ADDR)
			j -= (unsigned)SYS_ADDR;

		if (j >= 64 * MBSZ)
			{ printf ("\nEndereço inválido\n"); return; }
#endif	/*******************************************************/

		dump_hex ((char *)i, BLSZ, i);

		printf ("Continua? (s): ");

		if (askyesno (1) <= 0)
			break;

		i += BLSZ;
	}

}	/* end do_mem_dump */

/*
 ****************************************************************
 *	Imprime um "dump" em hexadecimal e ISO			*
 ****************************************************************
 */
void
dump_hex (const char *area, int count, long addr)
{
	int		lineno = 0, i, n;
	const char	*cp;

	/*
	 *	Inicia o dump
	 */
	while (count > 0)
	{
		if (count >= LINESZ)
			n = LINESZ;
		else
			n = count;

		printf ("%P:   ", addr);

		for (i = 0; i < LINESZ; i++)
		{
			if ((i & 1) == 0)
				printf (" ");

			if (i == LINESZ/2)
				printf (" ");

			if (i < n)
				printf ("%02X", area[i]);
			else
				printf ("  ");
		}

		printf ("     ");

		/*
		 *	Escreve caracteres em ascii
		 */
		for (cp = area, i = 0; i < n; cp++, i++)
		{
			if (*cp < 0x20)
				putchar ('.');
			else
				putchar (*cp);
		}

		putchar ('\n');

		/*
		 *	Pula uma linha de 8 em 8 linhas
		 */
		area += n; addr += n; count -= n;

		if (++lineno >= 8)
			{ putchar ('\n'); wait_for_ctl_Q (); lineno = 0; }

	}	/* end while */

}	/* end dump_hex */

/*
 ****************************************************************
 *	Cópia um dispositivo para um RAMD			*
 ****************************************************************
 */
int
do_ramd (const char *dev_nm, const char *arq_nm)
{
	DISKTB		*pp;
	int		count, shift, code, offset, diff;
	daddr_t		lbno = 0, daddr;
	int		RAMDSZ;
	void		*end_ramd_addr;
	char		area[BL4SZ];

	/*
	 *	Dispositivo de entrada
	 */
	ramd_addr = NOVOID;

	if ((pp = get_dev (dev_nm)) == NODISKTB)
		return (-1);

	if ((code = check_tropix_magic (pp)) < 0)
		return (-1);

	switch (code)
	{
	    case FS_T1:
	    {
		T1DINO		*np;

		if ((np = t1_iname (pp, arq_nm)) == NOT1DINO)
			return (-1);

		if (!S_ISREG (np->n_mode))
			{ printf ("\nO arquivo não é regular\n"); return (-1); }

		count = BL4SZ; shift = 3; file_size = np->n_size;

		RAMDSZ = 4 * MBSZ;		/* Para o caso do disquete */

		break;
	    }

	    case FS_CD:
	    {
		CDDIR		*dp;

		if ((dp = cd_iname (pp, arq_nm)) == NOCDDIR)
			return (-1);

		if ((dp->d_flags & ISO_IFMT) != ISO_REG)
			{ printf ("\nO arquivo não é regular\n"); return (-1); }

		count = ISO_BLSZ; shift = 2; file_size = ISO_GET_LONG (dp->d_size);

		RAMDSZ = 32 * MBSZ;		/* Para o caso do CD */

		break;
	    }

	    default:
		printf ("\nO dispositivo \"%s\" NÃO contém um sistema de arquivos T1 ou ISO\n");
		return (-1);
	}

	/*
	 *	Lê o primeiro bloco
	 */
	if ((daddr = daddrvec[lbno++]) == 0)
		{ printf ("\nEOF inesperado do arquivo\n"); return (-1); }

	if (bread (pp, (daddr << shift), area, count) < 0)
		return (-1);

	if ((offset = analyse_gzip_header (area)) < 0)
		{ printf ("\nO arquivo não está no formato GZIP\n"); return (-1); }

	/*
	 *	Prepara a descompressão
	 */
	ramd_addr = (void *)(bcb.y_physmem << KBSHIFT) - RAMDSZ;

	end_ramd_addr = ramd_addr + RAMDSZ;

	if ((diff = (int)end_ramd_addr - (int)SVGA_ADDR) > 0)
		ramd_addr -= diff;

	file_skip   = offset;
	file_header = NOVOID;
	file_dst    = ramd_addr;
   /***	file_size   = ...; /* acima ***/
	file_count  = count;
	file_pp     = pp;
	file_shift  = shift;

	if (inflate () < 0)
		{ ramd_addr = NOVOID; return (-1); }

	/*
	 *	Atualiza a entrada do RAMD
	 */
	pp = disktb + 2;		/* Entrada do RAMD0 */

	pp->p_size = BYTOBL (RAMDSZ);

	return (0);

}	/* end do_ramd */

/*
 ****************************************************************
 *	Analisa o cabeçalho GZIP				*
 ****************************************************************
 */
int
analyse_gzip_header (const char *area)
{
	const char	*cp = area;
	char		zip_flags;

	/*
	 *	Verifica se o arquivos está comprimido
	 */
	if (cp[0] != 0x1F || cp[1] != 0x8B)	/* Código do GZIP */
		return (-1);

	/*
	 *	Analisa o cabeçalho
	 */
	cp += 3;	/* Pula o código do GZIP e o método */

	zip_flags = *cp++;

	cp += 4 + 2;		/* Pula a data, indicadores extra e SO */

	if ((zip_flags & CONTINUATION) != 0)
		cp += 2;

	if ((zip_flags & EXTRA_FIELD) != 0)
	{
		unsigned	len = *cp++;

		len |= (*cp++) << 8;

		while (len--)
			cp++;
	}

	/* Get original file name if it was truncated */

	if ((zip_flags & ORIG_NAME) != 0)
	{
		char	old_name[16];
		char	*p = old_name;

		for (EVER)
		{
			*p = *cp++;

			if (*p++ == '\0')
				break;
		}

#if (0)	/*******************************************************/
		printf ("Nome = \"%s\"\n", old_name);
#endif	/*******************************************************/

	}	/* end orig_name */

	/* Discard file comment if any */

	if ((zip_flags & COMMENT) != 0)
	{
		while (*cp++ != 0)
			/* vazio */;
	}

	return (cp - area);

}	/* end analyse_gzip_header */

/*
 ****************************************************************
 *	Procura na tabela de partições e abre um dispositivo	*
 ****************************************************************
 */
DISKTB *
get_dev (const char *nm)
{
	DISKTB		*pp;

	if (streq (nm, "n"))
		return (NODISKTB);

	for (pp = disktb; /* abaixo */; pp++)
	{
		if (pp->p_name[0] == '\0')
		{
			printf ("\nNão achei \"%s\" na tabela de partições\n", nm);
			return (NODISKTB);
		}

		if (streq (pp->p_name, nm))
			break;
	}

	if (bopen (pp) < 0)
	{
		printf ("\nNão consegui abrir \"%s\"\n", pp->p_name);
		return (NODISKTB);
	}

	return (pp);

}	/* end get_dev */

/*
 ****************************************************************
 *	Verifica se contém um sistema de arquivos TROPIX	*
 ****************************************************************
 */
int
check_tropix_magic (const DISKTB *pp)
{
	char		area[BL4SZ];
	const T1SB	*t1sp;
	const CDVOL	*cdsp;
	const V7SB	*v7sp;

	/*
	 *	Tenta T1
	 */
	if (bread (pp, T1_BL4toBL (T1_SBNO), area, BL4SZ) < 0)
		return (-1);

	t1sp = (T1SB *)area;

	if (t1sp->s_magic == T1_SBMAGIC)
		return (FS_T1);

	/*
	 *	Tenta o CD
	 */
	if (bread (pp, ISOBL_TO_BL (ISO_SBNO), area, BL4SZ) < 0)
		return (-1);

	cdsp = (CDVOL *)area;

	if (memeq (cdsp->cd_id, ISO_STANDARD_ID, 5))
		return (FS_CD);

	/*
	 *	Tenta V7
	 */
	if (bread (pp, V7_SBNO, area, BLSZ) < 0)
		return (-1);

	v7sp = (V7SB *)area;

	if (long_endian_cv (v7sp->s_magic) == V7_SBMAGIC)
		return (FS_V7);

	/*
	 *	x
	 */
	printf ("\nEste dispositivo NÃO contém um sistema de arquivos TROPIX\n");
	return (-1);

}	/* end check_tropix_magic */

/*
 ****************************************************************
 *	Testa uma posição de memória				*
 ****************************************************************
 */
int
there_is_memory (ulong addr)
{
	ulong		*ptr;

	ptr = (ulong *)(addr + 2 * GBSZ - sizeof (ulong));

	*ptr = 0x55555555;

	if (*ptr != 0x55555555)
		return (-1);

	*ptr = 0xAAAAAAAA;

	if (*ptr != 0xAAAAAAAA)
		return (-1);

	return (+1);

}	/* end there_is_memory */

/*
 ****************************************************************
 *	Responde SIM ou NÃO					*
 ****************************************************************
 */
int
askyesno (int defau)
{
	char		*cp;
	char		area[32];
	
	for (EVER)
	{
		cp = gets (area);

		switch (*cp)
		{
		    case 's':
		    case 'S':
		    case 'y':
		    case 'Y':
			return (1);

		    case 'n':
		    case 'N':
			return (0);

		    case '\0':
			return (defau);

		}	/* end switch */

		printf ("\nSim ou não? (%c): ", defau > 0 ? 's' : 'n');
	}

}	/* end askyesno */

/*
 ****************************************************************
 *	Obtém um <^Q> ou <nl>					*
 ****************************************************************
 */
void
wait_for_ctl_Q (void)
{
	int		c;

	for (EVER)
	{
		if (wait_for_char_or_timeout () < 0)
			break;

		if ((c = getchar ()) == 0x11 || c == '\n' || c == '\r')
			break;
	}

	video_line = 0;

}	/* end wait_for_ctl_Q */

/*
 ****************************************************************
 *	Espera até que algo seja teclado			*
 ****************************************************************
 */
int
wait_for_char_or_timeout (void)
{
	int		i;
	static char	seen_char = 0;

	for (i = BOOT_WAIT; /* abaixo */; i--)
	{
		if (!seen_char && i <= 0)
			{ time_out++; return (-1); }

		if (haschar ())
			{ seen_char = 1; return (0); }

		DELAY (1000);
	}

}	/* end wait_for_char_or_timeout */

/*
 ****************************************************************
 *	Definições e variáveis usadas por DELAY			*
 ****************************************************************
 */
#define PIT_MAX 0xFFFF		/* Valor máximo do PIT */
#define MILHAO	 1000000	/* No. de microsegundos por segundo */
#define PIT_FREQ 1193182	/* Freqüência do relógio do PIT */

#define	DELAY_CALIBRATE_TIME	1000
#define	DELAY_TST_VALUE		200	/* Valor provisório */

#define DELTA_50 ((50 * PIT_FREQ + 500) / 1000)	/* Decremento do PIT para 50 ms */
#define VALUE_50 (PIT_MAX - DELTA_50)		/* Valor final após 50 ms */

/*
 ****************************************************************
 *	Mede a velocidade da CPU (para DELAY)			*
 ****************************************************************
 */
void
init_DELAY (void)
{
	int		PIT_val;
	long		old_tsc, new_tsc;
	int		old_PIT_val, new_PIT_val, delta, micro_seg;

	/*
	 *	Inicializa o PIT (Programmable Interval Timer)
	 */
	write_port (0x34, 0x43);
	write_port (PIT_MAX,	  0x40);
	write_port (PIT_MAX >> 8, 0x40);

	/*
	 *	Calcula o parametro para "DELAY"
	 */
	DELAY_value = DELAY_TST_VALUE;	/* Valor provisório */

	write_port (0x00, 0x43);	/* Latch */
	old_PIT_val = read_port (0x40) + (read_port (0x40) << 8);

	DELAY (DELAY_CALIBRATE_TIME);	/* Espera o tempo a avaliar */

	write_port (0x00, 0x43);	/* Latch */
	new_PIT_val = read_port (0x40) + (read_port (0x40) << 8);

	delta = old_PIT_val - new_PIT_val;

	micro_seg = mul_div_64 (delta, MILHAO, PIT_FREQ);

	DELAY_value = mul_div_64 (DELAY_CALIBRATE_TIME, DELAY_TST_VALUE, micro_seg);

	/*
	 *	Avalia a velocidade da CPU (somente se possui TSC)
	 */
	if (cpu_features & 0x10)	/* Possui TSC ? */
	{
		write_port (0x34, 0x43);
		write_port (PIT_MAX,	  0x40);
		write_port (PIT_MAX >> 8, 0x40);

		old_tsc = read_TSC ();

		for (EVER)
		{
			write_port (0x00, 0x43);	/* Latch */
			PIT_val = read_port (0x40) + (read_port (0x40) << 8);

			if (PIT_val < VALUE_50)
				break;
		}

		new_tsc = read_TSC ();

		CPU_speed = (new_tsc - old_tsc + 25000) / 50000;	/* 50 ms */
	}

}	/* end init_DELAY */

/*
 ****************************************************************
 *	Testa a calibração de DELAY				*
 ****************************************************************
 */
void
do_delay_tst (void)
{
	int		i;

	/*
	 *	Reavalia a velocidade
	 */
	init_DELAY ();

	/*
	 *	Imprime alguns dados
	 */
	printf
	(	"\nCPU_speed = %d MHz, DELAY_value = %d\n\n",
		CPU_speed, DELAY_value
	);

	/*
	 *	Imprime a mensagem de segundo em segundo
	 */
	for (i = 0; i <= 10; i++)
	{
		printf ("\r%d segundo(s)", i);
		DELAY (1000000);
	}

	printf ("\n");

}	/* end do_delay_tst */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	printf
	(	"%s\n"
		"Sintaxe:\n"
		"\t[-x] [-v] [-<dev>] [<pgm>]\n"
		"\t -r  [-v] [-<dev>] [<arq>]\n"
		"\t -*  [-v] (veja a lista de opções abaixo)\n",
		pgversion
	);

	printf
	(	"\nOpções:"
		"\t-x: Carrega (e executa) <pgm>\n"
		"\t-t: Imprime a tabela de partições em uso\n"
		"\t-f: Edita as tabelas de partições (fdisk)\n"
#ifdef	EXTRA
		"\t-l: Imprime o conteúdo de um diretório\n"
		"\t-c: Copia um dispositivo/arquivo regular\n"
#endif	EXTRA
		"\t-d: Imprime um DUMP de um dispositivo\n"
		"\t-m: Imprime um DUMP da memória\n"
		"\t-r: Descompacta <arq> para uma área de RAMD\n"
		"\t-i: Inicia uma instalação/conserto a partir de um disquete\n"
		"\t-s: Reavalia a velocidade da CPU\n"
		"\t-v: Verboso\n"
	);

	printf
	(	"\nArgs.:"
		"\t<dev>: Dispositivo		"
		"\t(Default = \"%s\")\n"
		"\t<pgm>: Nome do programa a ser executado"
		"\t(Default = \"%s\")\n"
		"\t<arq>: Nome do arquivo a ser carregado"
		"\t(Default = \"%s\")\n",
#if (0)	/*******************************************************/
		"\nObs.:\tPara todo número pedido, \"n\" cancela a operação",
#endif	/*******************************************************/
		def_dev_nm, def_pgm_nm,	def_arq_nm
	);

   /***	printf ("\n"); ***/

}	/* end help */

#ifdef	MEM_DEBUG
/*
 ****************************************************************
 *	Imprime a tabela de regiões de memória			*
 ****************************************************************
 */
enum
{
	M_NONE, M_RAM, M_RESERVED, M_ACPI, M_NVS
};

typedef struct
{
	ulong	m_addr_low,	/* Endereço inicial: parte baixa */
		m_addr_high;	/* Endereço inicial: parte alta  */

	ulong	m_size_low,	/* Tamanho: parte baixa */
		m_size_high;	/* Tamanho: parte alta  */

	ulong	m_type;		/* Tipo */

}	MEMTB;

entry const char *typetostr[] = { "NONE", "RAM ", "RES ", "ACPI", "NVS " };

extern MEMTB	bios_mem_tb[];	/* Em "src/seg.16.s" */

ulong
print_bios_mem_tb (void)
{
	MEMTB		*mp;
	ulong		kbsz, addr = 0;

	printf ("\nMapa de Memória:\n\n");

	for (mp = bios_mem_tb; mp->m_type != 0; mp++)
	{
		printf
		(	"%2d:  %s  %P - %P  ",
			mp - bios_mem_tb,
			mp->m_type > M_NVS ? "????" : typetostr[mp->m_type],
			mp->m_addr_low, mp->m_addr_low + mp->m_size_low - 1
		);

		kbsz = mp->m_size_low / 1024;

		printf ("%9d KB", kbsz);

		if (mp->m_size_low >= MBSZ)
			printf (" (%d.%02d MB)", KBTOMB_INT (kbsz), KBTOMB_DEC (kbsz));

		printf ("\n");

		if (mp->m_type == M_RAM && mp->m_addr_low + mp->m_size_low > addr)
			addr = mp->m_addr_low + mp->m_size_low;
	}

	printf ("\nend_mem_addr = %P\n", end_mem_addr);

	return (addr);

}	/* end print_bios_mem_tb */
#endif	MEM_DEBUG
