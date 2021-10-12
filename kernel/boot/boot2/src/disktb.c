/*
 ****************************************************************
 *								*
 *			disktb.c				*
 *								*
 *	Processa a tabela de partição de discos 		*
 *								*
 *	Versão	3.0.0, de 26.07.94				*
 *		4.9.0, de 18.04.07				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include <bcb.h>
#include <disktb.h>
#include <devmajor.h>
#include <scsi.h>
#include <pci.h>
#include <ata.h>

#include "../h/common.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****** Definições globais **************************************
 */
#undef	DEBUG

#define	IS_ZIP_OR_JAZ(sp)				\
	(streq ((sp)->scsi_product, "ZIP 100") ||	\
	 streq ((sp)->scsi_product, "JAZ 1GB"))

/*
 ****** Tabela de tipos de sistemas de arquivos *****************
 */
const PARTNM	partnm[] =
{
	0x00,	"",		/* Vazio */
	0x01,	"DOS FAT12",
	0x02,	"XENIX root",
	0x03,	"XENIX usr",
	0x04,	"DOS FAT16 < 32 MB",
	0x05,	"DOS Extended",
	0x06,	"DOS FAT16 >= 32 MB",
	0x07,	"NTFS",
	0x08,	"AIX",
	0x09,	"AIX bootable",
	0x0A,	"OPUS",
	0x0B,	"DOS FAT32",
	0x0C,	"DOS FAT32 (L)",
	0x0E,	"DOS FAT16 (L)",
	0x0F,	"DOS Extended (L)",
	0x40,	"Venix 80286",
	0x4F,	"QNX",
	0x51,	"Novell (?)",
	0x52,	"Microport",
	0x63,	"GNU HURD",
	0x64,	"Novell",
	0x75,	"PC/IX",
	0x80,	"old MINIX",
	0x81,	"LINUX/MINIX",
	0x82,	"LINUX swap",
	0x83,	"LINUX ext",
	0x93,	"Amoeba",
	0x94,	"Amoeba BBT",
	0xA5,	"BSD 4.2",
	0xA8,	"TROPIX V7",
	0xA9,	"TROPIX T1",
	0xAA,	"TROPIX fs3",
	0xAE,	"TROPIX Extended",
	0xAF,	"TROPIX swap",
	0xB7,	"BSDI fs",
	0xB8,	"BSDI swap",
	0xC7,	"Syrinx",
	0xDB,	"CP/M",
	0xE1,	"DOS access",
	0xE3,	"DOS R/O",
	0xF2,	"DOS Secondary",
	0xFF,	"BBT",

	-1,	"???"		/* Final da tabela */
};

/*
 ****** Tabela de partições contendo os dispositivos ancestrais *
 */
entry DISKTB	proto_disktb[] =
{
    /*	  nm,	  (offset, sz), (type, flags, lock, bl_shift), dev, (unit, target), ... */

	{ "fd0",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (FD_MAJOR,	0),  0,  0 },
	{ "fd1",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (FD_MAJOR,	1),  0,  1 },
	{ "ramd0",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (RAMD_MAJOR,	2),  0,  0 },
	{ "ramd1",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (RAMD_MAJOR,	3),  0,  1 },
	{ "pipe",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (RAMD_MAJOR,	4),  0,  2 },
	{ "md0",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (MD_MAJOR,	5),  0,  MD_SWAP_MINOR },
	{ "md1",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (MD_MAJOR,	6),  0,  MD_ROOT_MINOR },
	{ "md2",	0, 0,	0, 0, 0, BLSHIFT, MAKEDEV (MD_MAJOR,	7),  0,  MD_HOME_MINOR },
};

/*
 ****** Tabela de partições a ser construída para o KERNEL ******
 */
#define	DISKTB_SZ	200

entry DISKTB		disktb[DISKTB_SZ];

entry const char	*boot1_dev_nm;		/* Sugestão de boot para o núcleo */

/*
 ******	Variáveis apenas deste módulo ***************************
 */
entry int		disktb_index;		/* Próxima entrada */
entry char		disktb_letter;		/* Letra da entrada atual */

/*
 ******	Vetor de ponteiros para entradas IDE ********************
 */
entry ATA_DEVICE	*ata_ptr[NATAC*NATAT + 1]; /* Contém somente as unidades presentes */
entry int		ata_index;		/* Índice da entrada seguinte */

/*
 ******	Vetor de ponteiros para entradas SCSI *******************
 */
entry SCSI		*scsi_ptr[MAX_TARGETs+1]; /* Contém somente as unidades presentes */
entry int		scsi_index;		/* Índice da entrada seguinte */

/*
 ******	Protótipos de funções ***********************************
 */
int		get_parttb (const DISKTB *, const HDINFO *);
void		print_parttb_entry (int, const PARTTB *, daddr_t);
int		make_disktb_entry (const DISKTB *, int, int, const PARTTB *,
						daddr_t, const HDINFO *);
int		make_zip_disktb_entry (const DISKTB *, const SCSI *);

/*
 ****************************************************************
 *	Obtém as partições dos discos				*
 ****************************************************************
 */
int
get_all_parttb (void)
{
	const SCSI		*sp;
	const ATA_DEVICE	*adp;
	const DISKTB		*dp;
	SCSI			**spp;
	ATA_DEVICE		**app;
	int		ret = 0;
	char		dev_code = 0x80 - 1;
	int		boot_index = -1;
	PARTTB		parttb;
	HDINFO		hdinfo;

	/*
	 *	Inicializa "disktb"
	 */
   	memsetl (disktb, 0, sizeof (disktb) / sizeof (long));

	memmove (disktb, proto_disktb, sizeof (proto_disktb)); 	/* fd0 + fd1 + ramd[] + md[] */

	disktb_index = sizeof (proto_disktb) / sizeof (DISKTB);

   	memsetl (&parttb, 0, sizeof (parttb) / sizeof (long));

	boot1_dev_nm = NOSTR;

	/*
	 *	Gera a tabela dos discos ATA
	 */
	for (app = ata_ptr; /* abaixo */; app++)
	{
		if ((adp = *app) == NOATADEVICE)
			break;

		disktb_letter = adp->ad_dev_nm[2];

		dp = adp->ad_disktb;

		hdinfo.h_head = 0; hdinfo.h_sect = 0; hdinfo.h_cyl = 0;
		hdinfo.h_flags = 0; hdinfo.h_bl_shift = BLSHIFT;

		switch (adp->ad_type)
		{
		    default:
			break;

		    case ATA_HD:
			if (bopen (dp) < 0)
				{ ret++; continue; }

			if (bread (dp, fdisk_parttb_orig_sect, parttb_orig_block, BLSZ) < 0)
				{ ret++; continue; }

		    case_zip:
			if (adp->ad_bios_head)
			{
				if (++dev_code == boot1_devcode)
					boot_index = disktb_index;

				hdinfo.h_head  = adp->ad_bios_head;
				hdinfo.h_sect  = adp->ad_bios_sect;
				hdinfo.h_cyl   = adp->ad_bios_cyl;
				hdinfo.h_flags = adp->ad_flags;

				if (geo_tst_geometry ((PARTTB *)(parttb_orig_block + PARTB_OFF), &hdinfo) < 0)
				{
					printf
					(	"\e[34m"
						"\n%s: A geometria da BIOS NÃO confere com a das partições\n"
						"\e[0m",
						adp->ad_dev_nm
					);
				}
			}
			else
			{
				geo_get_parttb_geo
				(
					(PARTTB *)(parttb_orig_block + PARTB_OFF),
					&hdinfo,
					adp->ad_disksz,
					adp->ad_dev_nm
				);

				printf
				(	"%s: Usando a geometria (%d, %d) da tabela de partições\n",
					adp->ad_dev_nm, hdinfo.h_head, hdinfo.h_sect
				);
			}

			parttb.pt_type   = 0;
			parttb.pt_size   = adp->ad_disksz;
			parttb.pt_active = 0;

			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);

			ret += get_parttb (dp, &hdinfo);
			break;

		    case ATAPI_ZIP:
			if (bopen (dp) >= 0 && bread (dp, fdisk_parttb_orig_sect, parttb_orig_block, BLSZ) >= 0)
			{
				/* Analisa o tipo de ZIP */

				switch (adp->ad_scsi->scsi_disksz)
				{
				    case ZIP_FLOPPY_CAPACITY:	/* Não possui tabela de partições */
					break;
			
				    case ZIP_REG_CAPACITY:	/* Possui tabela de partições */
					goto case_zip;
			
				    default:
					printf
					(	"Unidade ZIP de tamanho %d blocos desconhecida\n",
						adp->ad_scsi->scsi_disksz
					);
					break;
			
				}	/* end switch */
			}

			parttb.pt_size = BL4SZ / BLSZ;
			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);
			make_zip_disktb_entry (dp, adp->ad_scsi);
			break;

		    case ATAPI_CDROM:
			parttb.pt_type    = 0;
			parttb.pt_active  = 0;
			parttb.pt_size    = adp->ad_scsi->scsi_disksz << (adp->ad_scsi->scsi_blshift - BLSHIFT);
			hdinfo.h_bl_shift = adp->ad_scsi->scsi_blshift;

			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);
			break;

		}	/* end switch */

	}	/* end for (discos IDE) */

	/*
	 *	Gera a tabela dos discos SCSI
	 */
	for (spp = scsi_ptr; /* abaixo */; spp++)
	{
		if ((sp = *spp) == NOSCSI)
			break;

		if (!sp->scsi_is_present)
			continue;

		disktb_letter = sp->scsi_dev_nm[2];

	   	parttb.pt_type	 = 0;
		parttb.pt_size	 = sp->scsi_disksz << (sp->scsi_blshift - BLSHIFT);
		parttb.pt_active = 0;

		hdinfo.h_head = 0; hdinfo.h_sect = 0; hdinfo.h_cyl = 0;
		hdinfo.h_flags = 0; hdinfo.h_bl_shift = sp->scsi_blshift;

		dp = sp->scsi_disktb;

		/*
		 *	Verifica o tipo de dispositivo
		 */
		if   (!sp->scsi_is_disk || sp->scsi_read_only)
		{
			/* Fita ou CD-ROM */

			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);
		}
		elif (sp->scsi_nopen)
		{
			/* Disco regular ou removível presente */

			if (bread (dp, fdisk_parttb_orig_sect, parttb_orig_block, BLSZ) < 0)
				{ ret++; continue; }

			if (sp->scsi_bios_head)
			{
				if (++dev_code == boot1_devcode)
					boot_index = disktb_index;

				hdinfo.h_head = sp->scsi_bios_head;
				hdinfo.h_sect = sp->scsi_bios_sect;
				hdinfo.h_cyl  = sp->scsi_bios_cyl;

				if (sp->scsi_int13_ext)
					hdinfo.h_flags |= HD_INT13_EXT;
				else
					hdinfo.h_flags &= ~HD_INT13_EXT;

				if (geo_tst_geometry ((PARTTB *)(parttb_orig_block + PARTB_OFF), &hdinfo) < 0)
				{
					printf
					(	"\e[34m"
						"\n%s: A geometria da BIOS NÃO confere com a das partições\n"
						"\e[0m",
						sp->scsi_dev_nm
					);
				}
			}
			else
			{
				geo_get_parttb_geo
				(
					(PARTTB *)(parttb_orig_block + PARTB_OFF),
					&hdinfo,
					sp->scsi_disksz,
					sp->scsi_dev_nm
				);

				printf
				(	"%s: Usando a geometria (%d, %d) da tabela de partições\n",
					sp->scsi_dev_nm, hdinfo.h_head, hdinfo.h_sect
				);
			}

			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);
			ret += get_parttb (dp, &hdinfo);
		}
		elif (sp->scsi_removable)
		{
			/* Disco removível ausente */

			parttb.pt_size = BL4SZ / BLSZ;
			make_disktb_entry (dp, -1, -1, &parttb, 0, &hdinfo);

			if (IS_ZIP_OR_JAZ (sp))
				make_zip_disktb_entry (dp, sp);
		}
		else	/* ??? */
		{
			printf
			(	"get_all_parttb: Dispositivo de tipo desconhecido: \"%s\"\n",
				sp->scsi_dev_nm
			);
			continue;
		}

	}	/* Percorrendo os dispositivos SCSI */

	/*
	 *	Marca o final da tabela
	 */
	disktb[disktb_index].p_name[0] = '\0';

	/*
	 *	Verifica se consegue obter a partição do "boot1"
	 */
	if   (boot1_devcode == 0x31)
	{
		/* Extremamente PROVISÓRIO e deselegante */

		boot1_dev_nm = disktb[0].p_name;
	}
	elif (boot_index >= 0)
	{
		char		tipo, letra;

		dp = &disktb[boot_index];

		tipo = dp->p_name[0]; letra = dp->p_name[2];

		for (/* acima */; /* abaixo */; dp++)
		{
			if (dp->p_name[0] != tipo || dp->p_name[2] != letra)
				break;

			if (dp->p_offset == boot1_offset)
				{ boot1_dev_nm = dp->p_name; break; }
		}
	}

	return (ret);

}	/* end get_all_parttb */

/*
 ****************************************************************
 *	Obtém as partições de um disco				*
 ****************************************************************
 */
int
get_parttb (const DISKTB *dev_dp, const HDINFO *gp)
{
	int		reg_index, log_index;
	const PARTTB	*reg_pp, *log_pp;
	daddr_t		log_offset, ext_begin, ext_end;
	char		area[BLSZ];

	if (*(ushort *)(parttb_orig_block + MAGIC_OFF) != 0xAA55)
	{
		printf
		(	"\nDisco \"%s\", bloco %d NÃO contém \"0x55AA\"\n",
			dev_dp->p_name, 0
		);
		printf ("(Pode ser consertado com \"fdisk\")\n");
	}

	/*
	 *	Processa as partições regulares
	 */
	for
	(	reg_index = 0, reg_pp = (PARTTB *)(parttb_orig_block + PARTB_OFF);
		reg_index < NPART;
		reg_index++, reg_pp++
	)
	{
		if (reg_pp->pt_size == 0)
			continue;

		if
		(	make_disktb_entry
			(
				dev_dp,
				reg_index,
				-1,
				reg_pp,
				reg_pp->pt_offset,
				gp
			) < 0
		)
			return (0);

		if (!IS_EXT (reg_pp->pt_type))
			continue;

		/*
		 *	Temos uma partição estendida
		 */
		ext_begin = reg_pp->pt_offset;
		ext_end   = reg_pp->pt_offset + reg_pp->pt_size;

		log_offset = ext_begin;

		/*
		 *	Percorre a cadeia de partições lógicas
		 */
		for (log_index = 0; /* abaixo */; log_index++)
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

			if (bread (dev_dp, log_offset, area, BLSZ) < 0)
				return (-1);

			if (*(ushort *)(area + MAGIC_OFF) != 0xAA55)
			{
				printf
				(	"\n*** O bloco %d da cadeia de "
					"partições lógicas da partição "
					"estendida %d NÃO contém \"0x55AA\"\n\n",
					reg_index, log_offset
				);
				break;
			}

			log_pp = (PARTTB *)(area + PARTB_OFF);

			if (log_pp[0].pt_size == 0) /* Primeiro método de EOF */
				break;

			/*
			 *	Achou uma partição lógica
			 */
			if
			(	make_disktb_entry
				(
					dev_dp,
					reg_index,
					log_index,
					log_pp,
					log_offset + log_pp->pt_offset,
					gp
				) < 0
			)
				return (0);

			/*
			 *	Procura a partição seguinte
			 */
			if (log_pp[1].pt_size == 0)	/* Segundo método de EOF */
				break;

			log_offset = ext_begin + log_pp[1].pt_offset;

		}	/* end for (cadeia de partições lógicas) */

	}	/* end for (cadeia de partições regulares) */

	return (0);

}	/* end getparttb */

/*
 ****************************************************************
 *	Cria uma entrada da DISKTB				*
 ****************************************************************
 */
int
make_disktb_entry (const DISKTB *old_dp, int reg_index, int log_index,
		const PARTTB *pp, daddr_t offset, const HDINFO *gp)
{
	DISKTB		*dp = &disktb[disktb_index];
	char		*cp = dp->p_name;

	/*
	 *	Verifica se tem espaço na tabela
	 */
	if (disktb_index >= DISKTB_SZ - 1)
		{ printf ("\nTabela de partições ESGOTADA\n"); return (-1); }

	/*
	 *	Constrói artesanalmente o nome da partição
	 */
	*cp++ = old_dp->p_name[0]; *cp++ = 'd'; *cp++ = disktb_letter;

	if (reg_index >= 0)
		*cp++ = reg_index + 1 + '0';

	if (log_index >= 0)
		*cp++ = log_index + 'a';

	*cp = '\0';

	/*
	 *	Preenche o resto da entrada
	 */
	dp->p_offset = offset;
	dp->p_size   = pp->pt_size;

	if (gp != NOHDINFO)
	{
		dp->p_head   = gp->h_head;
		dp->p_sect   = gp->h_sect;
		dp->p_cyl    = gp->h_cyl;
	}

	dp->p_dev    = MAKEDEV (MAJOR (old_dp->p_dev), disktb_index);
	dp->p_unit   = old_dp->p_unit;
	dp->p_target = old_dp->p_target;

	dp->p_type   = pp->pt_type;
	dp->p_flags  = (pp->pt_active == 0x80) ?  DISK_ACTIVE : 0;

	if (gp->h_flags & HD_INT13_EXT)
		dp->p_flags |= DISK_INT13_EXT;

	dp->p_blshift = gp->h_bl_shift;

	disktb_index++;

	return (0);

}	/* end make_disktb_entry */

/*
 ****************************************************************
 *	Cria uma entrada para o ZIP 100				*
 ****************************************************************
 */
int
make_zip_disktb_entry (const DISKTB *old_dp, const SCSI *sp)
{
	DISKTB		*dp = &disktb[disktb_index];
	char		*cp = dp->p_name;

	/*
	 *	Verifica se tem espaço na tabela
	 */
	if (disktb_index >= DISKTB_SZ - 1)
		{ printf ("\nTabela de partições ESGOTADA\n"); return (-1); }

#if (0)	/*******************************************************/
	/*
	 *	Analisa o tipo de ZIP
	 */
	switch (sp->scsi_disksz)
	{
	    case ZIP_FLOPPY_CAPACITY:	/* Não possui tabela de partições */
		return (0);

	    case ZIP_REG_CAPACITY:	/* Possui tabela de partições */
		break;

	    default:
		printf ("Unidade ZIP de tamanho %d blocos desconhecida\n", sp->scsi_disksz);
		return (-1);

	}	/* end switch */
#endif	/*******************************************************/

	/*
	 *	Constrói artesanalmente o nome da partição
	 */
	*cp++ = old_dp->p_name[0]; *cp++ = 'd'; *cp++ = disktb_letter; *cp++ = '4'; *cp = '\0';

	/*
	 *	Preenche o resto da entrada
	 */
   /***	dp->p_offset = 0;	***/
	dp->p_size   = BL4SZ / BLSZ;

   /***	dp->p_head   = 0;	***/
   /***	dp->p_sect   = 0;	***/
   /***	dp->p_cyl    = 0;	***/
	dp->p_blshift = BLSHIFT;

	dp->p_dev    = MAKEDEV (MAJOR (old_dp->p_dev), disktb_index);
	dp->p_unit   = old_dp->p_unit;
	dp->p_target = old_dp->p_target;

   /***	dp->p_type   = 0;	***/
   /***	dp->p_flags  = 0;	***/

	disktb_index++;

	return (0);

}	/* end make_zip_disktb_entry */
/*
 ****************************************************************
 *	Imprime a tabela de dispositivos/partições		*
 ****************************************************************
 */
void
prtable (void)
{
	const DISKTB	*dp;
	int		i;

	/*
	 *	Imprime a tabela
	 */
	printf ("\nTABELA DE PARTIÇÕES:\n");

	printf ("\n-NAME- A --OFFSET--  --SIZE---   --MB--  --DEV--  --U/T--  TYPE\n\n");

	for (dp = disktb, i = 0; dp->p_name[0]; dp++, i++)
	{
		if (i && i % 5 == 0)
			printf ("\n");

		printf
		(	"%6s " "%c " "%10d  " "%9d ",
			dp->p_name,
			dp->p_flags & DISK_ACTIVE ? '*' : ' ',
			dp->p_offset,
			dp->p_size,
			1 << dp->p_blshift
		);

		fdisk_print_MB (dp->p_size, 5);

		printf
		(	"  (%1d,%3d)  " " (%1d,%2d)  ",
			MAJOR (dp->p_dev),
			MINOR (dp->p_dev),
			dp->p_unit,
			dp->p_target
		);

		print_part_type_nm (dp->p_type);

		printf ("\n");

	}	/* end for (tabela de partições) */

}	/* prtable */

/*
 ****************************************************************
 *	Escreve o tipo de uma partição				*
 ****************************************************************
 */
void
print_part_type_nm (int type)
{
	const PARTNM	*pnm;

	for (pnm = partnm; /* abaixo */; pnm++)
	{
		if (pnm->pt_type < 0)
			{ printf ("??? (0x%2X)", type); break; }

		if (pnm->pt_type == type)
			{ printf ("%s", pnm->pt_nm); break; }
	}

} 	/* end print_part_type_nm */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime o conteúdo de uma entrada de partições DOS	*
 ****************************************************************
 */
void
print_parttb_entry (int index, const PARTTB *pp, daddr_t offset)
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

	printf ("%2d: %9d %9d ", index + 1, offset, pp->pt_size);

	if   (pp->pt_active == 0x00)
		printf ("   ");
	elif (pp->pt_active == 0x80)
		printf ("*  ");
	else
		printf ("%2X ", pp->pt_active);

	if (pnm->pt_type >= 0)
		printf ("%s\n", pnm->pt_nm);
	else
		printf ("%2X\n", pp->pt_type);

}	/* end print_parttb_entry */
#endif	DEBUG
