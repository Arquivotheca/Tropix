/*
 ****************************************************************
 *								*
 *			ahc_pci.c				*
 *								*
 *	Reconhecimento das placas SCSI				*
 *								*
 *	Versão	4.0.0, de 19.03.01				*
 *		4.0.0, de 07.06.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

#include <stddef.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"

#include "../h/intr.h"
#include "../h/pci.h"
#include "../h/bhead.h"
#include "../h/scsi.h"
#include "../h/devhead.h"

#include "../h/uerror.h"
#include "../h/proto.h"
#include "../h/extern.h"

#include "aic7xxx_seq.h"
#include "aic7xxx.h"
#include "asm_format.h"

/*
 ******	Protótipos de funções ***********************************
 */
typedef int	(ahc_device_setup_t) (const PCIDATA *, struct ahc_probe_config *);

ahc_device_setup_t		ahc_aic7850_setup;
ahc_device_setup_t		ahc_aic7855_setup;
ahc_device_setup_t		ahc_aic7860_setup;
ahc_device_setup_t		ahc_aic7870_setup;
ahc_device_setup_t		ahc_aha394X_setup;
ahc_device_setup_t		ahc_aha494X_setup;
ahc_device_setup_t		ahc_aha398X_setup;
ahc_device_setup_t		ahc_aic7880_setup;
ahc_device_setup_t		ahc_2940Pro_setup;
ahc_device_setup_t		ahc_aha394XU_setup;
ahc_device_setup_t		ahc_aha398XU_setup;
ahc_device_setup_t		ahc_aic7890_setup;
ahc_device_setup_t		ahc_aic7892_setup;
ahc_device_setup_t		ahc_aic7895_setup;
ahc_device_setup_t		ahc_aic7896_setup;
ahc_device_setup_t		ahc_aic7899_setup;
ahc_device_setup_t		ahc_raid_setup;
ahc_device_setup_t		ahc_aha394XX_setup;
ahc_device_setup_t		ahc_aha494XX_setup;
ahc_device_setup_t		ahc_aha398XX_setup;

/*
 ******	Nomes dos diversos "chips" ******************************
 */
const char	*const ahc_chip_names[] =
{
	"NONE",
	"aic7770",
	"aic7850",
	"aic7855",
	"aic7859",
	"aic7860",
	"aic7870",
	"aic7880",
	"aic7895",
	"aic7895C",
	"aic7890/91",
	"aic7896/97",
	"aic7892",
	"aic7899"
};

/*
 ******	O nosso sequenciador ************************************
 */
#include "seq.out"

int		ahc_check_patch (struct ahc_softc *, struct patch **, uint, uint *);

/*
 ****************************************************************
 *	Tabela de Identificação					*
 ****************************************************************
 */
struct ahc_pci_identity
{
	unsigned long		i_id;		/* Device + Vendor */
	unsigned long		i_subid;	/* SubDevice + SubVendor */
	char			i_full;		/* Confere ambas acima */
	ahc_device_setup_t	*i_setup;	/* Função para obter as características */
	const char		*i_name;	/* Nome do controlador */
};

struct ahc_pci_identity ahc_pci_ident_table[] =
{
	/* aic7850 based controllers */

	0x50789004, 0x78509004, 1, ahc_aic7850_setup,	"Adaptec 2910/15/20/30C SCSI adapter",

	/* aic7860 based controllers */

	0x38609004, 0x38699004,	1, ahc_aic7860_setup,	"Adaptec 2930CU SCSI adapter",
	0x60759004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec 1480A Ultra SCSI adapter",
	0x61789004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec 2940A Ultra SCSI adapter",
	0x21789004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec 2940A/CN Ultra SCSI adapter",
	0x60389004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec 2930C SCSI adapter (VAR)",

	/* aic7870 based controllers */

	0x71789004, 0x00000000, 1, ahc_aic7870_setup,	"Adaptec 2940 SCSI adapter",
	0x72789004, 0x00000000, 1, ahc_aha394X_setup,	"Adaptec 3940 SCSI adapter",
	0x73789004, 0x00000000, 1, ahc_aha398X_setup,	"Adaptec 398X SCSI RAID adapter",
	0x74789004, 0x00000000, 1, ahc_aic7870_setup,	"Adaptec 2944 SCSI adapter",
	0x75789004, 0x00000000, 1, ahc_aha394X_setup,	"Adaptec 3944 SCSI adapter",
	0x76789004, 0x00000000, 1, ahc_aha494X_setup,	"Adaptec 4944 SCSI adapter",

	/* aic7880 based controllers */

	0x81789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec 2940 Ultra SCSI adapter",
	0x82789004, 0x00000000, 0, ahc_aha394XU_setup,	"Adaptec 3940 Ultra SCSI adapter",
	0x84789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec 2944 Ultra SCSI adapter",
	0x85789004, 0x00000000, 0, ahc_aha394XU_setup,	"Adaptec 3944 Ultra SCSI adapter",
	0x83789004, 0x00000000, 0, ahc_aha398XU_setup,	"Adaptec 398X Ultra SCSI RAID adapter",
	0x86789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec 4944 Ultra SCSI adapter",
	0x88789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec 2930 Ultra SCSI adapter",
	0x87789004, 0x00000000, 0, ahc_2940Pro_setup,	"Adaptec 2940 Pro Ultra SCSI adapter",
	0x00789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec 2940/CN Ultra SCSI adapter",

	/* aic7890 based controllers */

	0x00119005, 0x01819005, 1, ahc_aic7890_setup,	"Adaptec 2930 Ultra2 SCSI adapter",
	0x00109005, 0xA1009005, 1, ahc_aic7890_setup,	"Adaptec 2940B Ultra2 SCSI adapter",
	0x00109005, 0x21809005, 1, ahc_aic7890_setup,	"Adaptec 2940 Ultra2 SCSI adapter (OEM)",
	0x00109005, 0xA1809005, 1, ahc_aic7890_setup,	"Adaptec 2940 Ultra2 SCSI adapter",
	0x00109005, 0xE1009005, 1, ahc_aic7890_setup,	"Adaptec 2950 Ultra2 SCSI adapter",
	0x00139005, 0x00039005, 1, ahc_aic7890_setup,	"Adaptec AAA-131 Ultra2 RAID adapter",

	/* aic7892 based controllers */

	0x00809005, 0xE2A09005, 1, ahc_aic7892_setup,	"Adaptec 29160 Ultra160 SCSI adapter",
	0x00809005, 0xE2A00E11, 1, ahc_aic7892_setup,	"Adaptec (Compaq OEM) 29160 Ultra160 SCSI adapter",
	0x00809005, 0x62A09005, 1, ahc_aic7892_setup,	"Adaptec 29160N Ultra160 SCSI adapter",
	0x00809005, 0xE2209005, 1, ahc_aic7892_setup,	"Adaptec 29160B Ultra160 SCSI adapter",
	0x00819005, 0x62A19005, 1, ahc_aic7892_setup,	"Adaptec 19160B Ultra160 SCSI adapter",

	/* aic7895 based controllers */

	0x78959004, 0x78919004, 1, ahc_aic7895_setup,	"Adaptec 2940/DUAL Ultra SCSI adapter",
	0x78959004, 0x78929004, 1, ahc_aic7895_setup,	"Adaptec 3940A Ultra SCSI adapter",
	0x78959004, 0x78949004, 1, ahc_aic7895_setup,	"Adaptec 3944A Ultra SCSI adapter",

	/* aic7896/97 based controllers */

	0x00509005, 0xFFFF9005, 1, ahc_aic7896_setup,	"Adaptec 3950B Ultra2 SCSI adapter",
	0x00509005, 0xF5009005, 1, ahc_aic7896_setup,	"Adaptec 3950B Ultra2 SCSI adapter",
	0x00519005, 0xFFFF9005, 1, ahc_aic7896_setup,	"Adaptec 3950D Ultra2 SCSI adapter",
	0x00519005, 0xB5009005, 1, ahc_aic7896_setup,	"Adaptec 3950D Ultra2 SCSI adapter",

	/* aic7899 based controllers */

	0x00C09005, 0xF6209005, 1, ahc_aic7899_setup,	"Adaptec 3960D Ultra160 SCSI adapter",
	0x00C09005, 0xF6200E11, 1, ahc_aic7899_setup,	"Adaptec (Compaq OEM) 3960D Ultra160 SCSI adapter",

	/* Generic chip probes for devices we don't know 'exactly' */

	0x50789004, 0x00000000, 0, ahc_aic7850_setup,	"Adaptec aic7850 SCSI adapter",
	0x55789004, 0x00000000, 0, ahc_aic7855_setup,	"Adaptec aic7855 SCSI adapter",
	0x38609004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec aic7859 Ultra SCSI adapter",
	0x60789004, 0x00000000, 0, ahc_aic7860_setup,	"Adaptec aic7860 SCSI adapter",
	0x70789004, 0x00000000, 0, ahc_aic7870_setup,	"Adaptec aic7870 SCSI adapter",
	0x80789004, 0x00000000, 0, ahc_aic7880_setup,	"Adaptec aic7880 Ultra SCSI adapter",
	0x001F9005, 0x00000000, 0, ahc_aic7890_setup,	"Adaptec aic7890/91 Ultra2 SCSI adapter",
	0x008F9005, 0x00000000, 0, ahc_aic7892_setup,	"Adaptec aic7892 Ultra160 SCSI adapter",
	0x78959004, 0x00000000, 0, ahc_aic7895_setup,	"Adaptec aic7895 Ultra SCSI adapter",
	0x78939004, 0x00000000, 0, ahc_aic7895_setup,	"Adaptec aic7895 Ultra SCSI adapter (RAID PORT)",
	0x005F9005, 0x00000000, 0, ahc_aic7896_setup,	"Adaptec aic7896/97 Ultra2 SCSI adapter",
	0x00CF9005, 0x00000000, 0, ahc_aic7899_setup,	"Adaptec aic7899 Ultra160 SCSI adapter",
	0x10789004, 0x00000000, 0, ahc_raid_setup,	"Adaptec aic7810 RAID memory controller",
	0x78159004, 0x00000000, 0, ahc_raid_setup,	"Adaptec aic7815 RAID memory controller",

	/* Final da Tabela */

	0,	    0,		0, 0,			NOSTR

}	/* end struct ahc_pci_identity ahc_pci_ident_table */ ;

/*
 ******	Definições **********************************************
 */
#define AHC_394X_SLOT_CHANNEL_A	4
#define AHC_394X_SLOT_CHANNEL_B	5

#define AHC_398X_SLOT_CHANNEL_A	4
#define AHC_398X_SLOT_CHANNEL_B	8
#define AHC_398X_SLOT_CHANNEL_C	12

#define AHC_494X_SLOT_CHANNEL_A	4
#define AHC_494X_SLOT_CHANNEL_B	5
#define AHC_494X_SLOT_CHANNEL_C	6
#define AHC_494X_SLOT_CHANNEL_D	7

#define	DEVCONFIG		0x40

#define		SCBSIZE32	0x00010000	/* aic789X only */
#define		MPORTMODE	0x00000400	/* aic7870 only */
#define		RAMPSM		0x00000200	/* aic7870 only */
#define		VOLSENSE	0x00000100
#define		SCBRAMSEL	0x00000080
#define		MRDCEN		0x00000040
#define		EXTSCBTIME	0x00000020	/* aic7870 only */
#define		EXTSCBPEN	0x00000010	/* aic7870 only */
#define		BERREN		0x00000008
#define		DACEN		0x00000004
#define		STPWLEVEL	0x00000002
#define		DIFACTNEGEN	0x00000001	/* aic7870 only */

#define	CSIZE_LATTIME		0x0C

#define		CACHESIZE	0x0000003F	/* only 5 bits */
#define		LATTIME		0x0000FF00

/*
 ******	Definições da SEEPROM ***********************************
 */
typedef enum { C46 = 6, C56_66	= 8 }	seeprom_chip_t;

struct seeprom_descriptor
{
	struct ahc_softc	*sd_ahc;
	uint			sd_control_offset;
	uint			sd_status_offset;
	uint			sd_dataout_offset;
	seeprom_chip_t		sd_chip;
	ushort			sd_MS;
	ushort			sd_RDY;
	ushort			sd_CS;
	ushort			sd_CK;
	ushort			sd_DO;
	ushort			sd_DI;
};

/*
 * This function will read count 16-bit words from the serial EEPROM and
 * return their value in buf.  The port address of the aic7xxx serial EEPROM
 * control register is passed in as offset.  The following parameters are
 * also passed in:
 *
 *   CS  - Chip select
 *   CK  - Clock
 *   DO  - Data out
 *   DI  - Data in
 *   RDY - SEEPROM ready
 *   MS  - Memory port mode select
 *
 *  A failed read attempt returns 0, and a successful read returns 1.
 */

#define	SEEPROM_INB(sd)		ahc_inb (sd->sd_ahc, sd->sd_control_offset)

#define	SEEPROM_OUTB(sd, value)					\
	ahc_outb (sd->sd_ahc, sd->sd_control_offset, value);	\
	ahc_inb (sd->sd_ahc, INTSTAT);

#define	SEEPROM_STATUS_INB(sd)	ahc_inb (sd->sd_ahc, sd->sd_status_offset)

#define	SEEPROM_DATA_INB(sd)	ahc_inb (sd->sd_ahc, sd->sd_dataout_offset)

/*
 ******	Protótipos de funções ***********************************
 */
int		ahc_pci_config (struct ahc_softc *ahc, const struct ahc_pci_identity *ip);
void		ahc_scbram_config (struct ahc_softc *ahc, int enable, int pcheck, int fast, int large);
int		ahc_probe_scbs (struct ahc_softc *ahc);
void		ahc_check_extport (struct ahc_softc *ahc, uint *sxfrctl1);
int		ahc_ext_scbram_present (struct ahc_softc *ahc);
int		ahc_acquire_seeprom (struct ahc_softc *ahc, struct seeprom_descriptor *sd);
void		ahc_configure_termination (struct ahc_softc *, struct seeprom_descriptor *, uint, uint *);
void		ahc_term_detect (struct ahc_softc *, int *, int *, int *, int *, int *);
void		aic787X_cable_detect (struct ahc_softc *, int *, int *, int *, int *);
void		aic785X_cable_detect (struct ahc_softc *, int *, int *, int *);
int		ahc_reset (struct ahc_softc *ahc);
void		ahc_probe_ext_scbram (struct ahc_softc *ahc);
void		ahc_write_brdctl (struct ahc_softc * ahc, uchar value);
uchar		ahc_read_brdctl (struct ahc_softc * ahc);
int		ahc_verify_cksum (struct seeprom_config *sc);
int		ahc_read_seeprom (struct seeprom_descriptor *sd, ushort *buf, uint start_addr, uint count);

/*
 ****************************************************************
 *	Identifica o dispositivo				*
 ****************************************************************
 */
const struct ahc_pci_identity *
ahc_find_pci_device (PCIDATA *pci, ulong id)
{
	unsigned long			subid;
	const struct ahc_pci_identity	*ip;

	/*
	 *	Compõe o sub-identificador
	 */
	subid = (pci->pci_subdevice << 16) | pci->pci_subvendor;

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"ahc_find_pci_device: Procurando: "
		"dev = %04X, vendor = %04X, subdev = %04X, subvendor = %04X\n",
		id >> 16, id & 0xFFFF, subid >> 16, subid & 0xFFFF
	);
#endif	DEBUG

	/*
	 *	Procura a entrada correspondente da tabela
	 */
	for (ip = ahc_pci_ident_table; /* abaixo */; ip++)
	{
		if (ip->i_name == NOSTR)
			return ((struct ahc_pci_identity *)NULL);

		if (ip->i_id != id)
			continue;

		if (ip->i_full && ip->i_subid != subid)
			continue;

		return (ip);
	}

}	/* ahc_find_pci_device */

/*
 ****************************************************************
 *	Testa a existência de um controlador			*
 ****************************************************************
 */
const char *
ahc_pci_probe (PCIDATA *pci, ulong type)
{
	const struct ahc_pci_identity	*ip;

	if ((ip = ahc_find_pci_device (pci, type)) != NULL)
		return (ip->i_name);

	return (NOSTR);

}	/* ahc_pci_probe */

/*
 ****************************************************************
 *	Aloca os recursos do controlador			*
 ****************************************************************
 */
int
ahc_pci_attach (PCIDATA *pci, int unit)
{
	const struct ahc_pci_identity	*ip;
	struct ahc_softc		*ahc;
	AHCINFO				*sp;

	/*
	 *	Identifica novamente o controlador
	 */
	if ((ip = ahc_find_pci_device (pci, (pci->pci_device << 16) | pci->pci_vendor)) == NULL)
		return (-1);

	/*
	 *	Alloca uma área para os dados da controladora
	 */
	if ((unsigned)unit >= NAHC)
	{
		printf ("aic7xxx: Unidade %d excessiva\n", unit);
		return (-1);
	}

	if ((ahc = malloc_byte (sizeof (struct ahc_softc))) == NULL)
	{
		printf ("aic7xxx: Não obtive memória\n");
		return (-1);
	}

	memclr (ahc, sizeof (struct ahc_softc));

	ahc->unit = unit;
	snprintf (ahc->name, sizeof (ahc->name), "ahc%d", unit);

	/*
	 *	Configura a placa
	 */
	ahc_softc_vec[unit] = ahc;
	ahc->dev_softc = pci;

	if (ahc_pci_config (ahc, ip) < 0)
		return (-1);

	/*
	 *	Imprime as características da controladora
	 */
	printf
	(	"ahc: [%d: 0x%X, %d, %s %s Channel, SCSI Id = %d, ",
		ahc->unit, ahc->port_base, ahc->irq,
		ahc_chip_names[ahc->chip & AHC_CHIPID_MASK],
		(ahc->features & AHC_WIDE) ? "Wide" : "Single",
		ahc->our_id
	);

	if ((ahc->flags & AHC_PAGESCBS) != 0)
		printf ("%d/%d SCBs]\n", ahc->maxhscbs, AHC_SCB_MAX);
	else
		printf ("%d SCBs]\n", ahc->maxhscbs);

#if (0)	/*******************************************************/
	printf
	(	"%s: chip = %P, features = %P, bug = %P, flag = %P\n",
		ahc->name, ahc->chip, ahc->features, ahc->bugs, ahc->flags
	);
#endif	/*******************************************************/

	/*
	 *	Inicializa o número máximo de operações simultâneas (tag)
	 */
	for (sp = &ahc->ahc_info[0]; sp < &ahc->ahc_info[NTARGET]; sp++)
	{
		sp->info_max_tag = AHC_MAX_TAG;
		sp->info_inc_op  = AHC_INC_OP;
	}

	/*
	 *	Se for o BOOT, procura os alvos
	 */
#ifdef	BOOT
	ahc_get_targets (ahc);
#endif	BOOT

	return (0);

}	/* end ahc_pci_attach */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ahc_pci_config (struct ahc_softc *ahc, const struct ahc_pci_identity *ip)
{
	struct ahc_probe_config	probe_config;
	uint			command, our_id = 0;
	uint			sxfrctl1, scsiseq, dscommand0;
	uchar			sblkctl;
	int			irq;

	/*
	 *	Prepara a configuração provisória
	 */
	probe_config.channel	 = 'A';
	probe_config.channel_b	 = 'B';
	probe_config.chip	 = AHC_NONE;
	probe_config.features	 = AHC_FENONE;
	probe_config.bugs	 = AHC_BUGNONE;
	probe_config.flags	 = AHC_FNONE;

	/*
	 *	Configura, de acordo com o "chip"
	 */
	if (ip->i_setup (ahc->dev_softc, &probe_config) < 0)
		return (-1);

	probe_config.chip	|= AHC_PCI;

	/*
	 *	Obtém a porta de E/S
	 */
	if ((pci_read (ahc->dev_softc, PCIR_COMMAND, /* bytes */ 1) & PCIM_CMD_PORTEN) == 0)
	{
		printf ("%s: Não está mapeando as portas\n", ahc->name);
		return (-1);
	}

	ahc->port_base = pci_read (ahc->dev_softc, PCI_MAP_REG_START, 4) & ~1;
#if (0)	/*******************************************************/
	ahc->port_base = ahc_get_pci_base_addr (ahc->dev_softc);
#endif	/*******************************************************/

	/*
	 *	Ensure busmastering is enabled
	 */
	command = pci_read (ahc->dev_softc, PCIR_COMMAND, /* bytes */ 1);
	command |= PCIM_CMD_BUSMASTEREN;
	DELAY (1000);
	pci_write (ahc->dev_softc, PCIR_COMMAND, command, /* bytes */ 1);

	/* On all PCI adapters, we allow SCB paging */

	probe_config.flags |= AHC_PAGESCBS;

	/*
	 *	Guarda as características da unidade
	 */
	ahc->chip	 = probe_config.chip;
	ahc->features	 = probe_config.features;
	ahc->bugs	 = probe_config.bugs;
	ahc->flags	 = probe_config.flags;
	ahc->channel	 = probe_config.channel;
	ahc->unpause	 = (ahc_inb (ahc, HCNTRL) & IRQMS) | INTEN;

	/* The IRQMS bit is only valid on VL and EISA chips */

	if ((ahc->chip & AHC_PCI) != 0)
		ahc->unpause &= ~IRQMS;

	ahc->pause = ahc->unpause | PAUSE;

	/*
	 *	Remember how the card was setup in case there is no SEEPROM
	 */
	pause_sequencer (ahc);

	if ((ahc->features & AHC_ULTRA2) != 0)
		our_id = ahc_inb (ahc, SCSIID_ULTRA2) & OID;
	else
		our_id = ahc_inb (ahc, SCSIID) & OID;

	sxfrctl1 = ahc_inb (ahc, SXFRCTL1) & STPWEN;
	scsiseq  = ahc_inb (ahc, SCSISEQ);

	if (ahc_reset (ahc) < 0)
		return (-1);

	if ((ahc->features & AHC_DT) != 0)
	{
		uint	sfunct;

		/* Perform ALT-Mode Setup */

		sfunct = ahc_inb (ahc, SFUNCT) & ~ALT_MODE;
		ahc_outb (ahc, SFUNCT, sfunct | ALT_MODE);
		ahc_outb (ahc, OPTIONMODE, OPTIONMODE_DEFAULTS);

		/* Send CRC info in target mode every 4K */

		ahc_outb (ahc, TARGCRCCNT, 0);
		ahc_outb (ahc, TARGCRCCNT + 1, 0x10);
		ahc_outb (ahc, SFUNCT, sfunct);

		/* Normal mode setup */

		ahc_outb (ahc, CRCCONTROL1, CRCVALCHKEN|CRCENDCHKEN|CRCREQCHKEN |TARGCRCENDEN|TARGCRCCNTEN);
	}

	/*
	 *	Prepara o IRQ
	 */
#ifndef	BOOT
	ahc->irq = irq = ahc->dev_softc->pci_intline;

	if (set_dev_irq (irq, PL, ahc->unit, ahc_intr) < 0)
		return (-1);
#endif	BOOT

	/*
	 *	Handle chips that must have cache line streaming (dis/en)abled
	 */
	dscommand0 = ahc_inb (ahc, DSCOMMAND0) | MPARCKEN | CACHETHEN;

	if (ahc->features & AHC_ULTRA2)	/* DPARCKEN doesn't work correctly on some MBs so don't use it */
		dscommand0 &= ~DPARCKEN;

	if ((ahc->bugs & AHC_CACHETHEN_DIS_BUG) != 0)
		dscommand0 |= CACHETHEN;

	if ((ahc->bugs & AHC_CACHETHEN_BUG) != 0)
		dscommand0 &= ~CACHETHEN;

	ahc_outb (ahc, DSCOMMAND0, dscommand0);

	ahc->pci_cachesize = pci_read (ahc->dev_softc, CSIZE_LATTIME, /* bytes */ 1) & CACHESIZE;
	ahc->pci_cachesize *= 4;

	/* See if we have a SEEPROM and perform auto-term */

	ahc_check_extport (ahc, &sxfrctl1);

	/*
	 *	Take the LED out of diagnostic mode
	 */
	sblkctl = ahc_inb (ahc, SBLKCTL);
	ahc_outb (ahc, SBLKCTL, (sblkctl & ~(DIAGLEDEN | DIAGLEDON)));

	if ((ahc->features & AHC_ULTRA2) != 0)
	{
		ahc_outb (ahc, DFF_THRSH, RD_DFTHRSH_MAX | WR_DFTHRSH_MAX);
	}
	else
	{
		ahc_outb (ahc, DSPCISTATUS, DFTHRSH_100);
	}

	/*
	 *	PCI Adapter default setup Should only be used if the adapter does not have a SEEPROM
	 */
	if (ahc->flags & AHC_USEDEFAULTS)
	{
		if (scsiseq != 0)	/* See if someone else set us up already */
		{
			printf ("%s: Using left over BIOS settings\n", ahc->name);
			ahc->flags &= ~AHC_USEDEFAULTS;
		}
		else			/* Assume only one connector and always turn on termination */
		{
			our_id = 0x07; sxfrctl1 = STPWEN;
		}

		ahc_outb (ahc, SCSICONF, our_id | ENSPCHK | RESET_SCSI);

		ahc->our_id = our_id;
	}

	/*
	 *	Take a look to see if we have external SRAM. We currently do not
	 *	attempt to use SRAM that is shared among multiple controllers
	 */
	ahc_probe_ext_scbram (ahc);

	/*
	 *	Record our termination setting for the generic initialization routine
	 */
	if ((sxfrctl1 & STPWEN) != 0)
		ahc->flags |= AHC_TERM_ENB_A;

	/*
	 *	Core initialization
	 */
	if (ahc_init (ahc) < 0)
		return (-1);

	return (0);

}	/* end ahc_pci_config */

/*
 ****************************************************************
 *	Start the board, ready for normal operation		*
 ****************************************************************
 */
int
ahc_init (struct ahc_softc *ahc)
{
	int		max_target, i, term, target;
	uint		scsi_conf, scsiseq_template, ultraenb, discenable, tagenable;
	ulong		physaddr;

	/*
	 *	Assume we have a board at this stage and it has been reset.
	 */
	if ((ahc->flags & AHC_USEDEFAULTS) != 0)
		ahc->our_id = 7;

	/*
	 *	Default to allowing initiator operations.
	 */
	ahc->flags |= AHC_INITIATORROLE;

	/*
	 *	Não aceitamos TARGET_MODE
	 */
	ahc->features &= ~AHC_TARGETMODE;

	/*
	 *	DMA tag for our command fifos and other data in system memory the
	 *	card's sequencer must be able to access.  For initiator roles, we
	 *	need to allocate space for the the qinfifo and qoutfifo. The
	 *	qinfifo and qoutfifo are composed of 256 1 byte elements. When
	 *	providing for the target mode role, we must additionally provide
	 *	space for the incoming target command fifo and an extra byte to
	 *	deal with a dma bug in some chip versions.
	 */
	if ((ahc->qoutfifo = malloc_byte (2 * 256 * sizeof (uchar))) == NOVOID)
		return (-1);

	ahc->qinfifo = &ahc->qoutfifo[256];

	/* Allocate SCB data now that buffer_dmat is initialized */

	if (ahc->maxhscbs == 0 && ahc_init_scbdata (ahc) != 0)
		return (-1);

	ahc_outb (ahc, SEQ_FLAGS, 0);

	if (ahc->maxhscbs < AHC_SCB_MAX)
	{
		ahc->flags |= AHC_PAGESCBS;
	}
	else
	{
		ahc->flags &= ~AHC_PAGESCBS;
	}

#ifndef	BOOT
	if (scb.y_boot_verbose)
	{
		printf
		(	"%s: Hardware SCB %d bytes, Kernel SCB %d bytes\n",
			ahc->name, sizeof (struct hardware_scb), sizeof (struct ahc_scb)
		);
	}
#endif	BOOT

	/*
	 *	Set the SCSI Id, SXFRCTL0, SXFRCTL1, and SIMODE1, for both channels
	 */
	term = (ahc->flags & AHC_TERM_ENB_A) != 0 ? STPWEN : 0;

	if ((ahc->features & AHC_ULTRA2) != 0)
		ahc_outb (ahc, SCSIID_ULTRA2, ahc->our_id);
	else
		ahc_outb (ahc, SCSIID, ahc->our_id);

	scsi_conf = ahc_inb (ahc, SCSICONF);

	ahc_outb (ahc, SXFRCTL1, (scsi_conf & (ENSPCHK | STIMESEL)) | term | ENSTIMER | ACTNEGEN);

	ahc_outb (ahc, SIMODE1, ENSELTIMO | ENSCSIRST | ENSCSIPERR);
	ahc_outb (ahc, SXFRCTL0, DFON | SPIOEN);

	if ((scsi_conf & RESET_SCSI) != 0 && (ahc->flags & AHC_INITIATORROLE) != 0)
		ahc->flags |= AHC_RESET_BUS_A;

	/*
	 *	Look at the information that board initialization or the board bios has left us.
	 */
	ultraenb = 0; tagenable = ALL_TARGETS_MASK;

	/* Grab the disconnection disable table and invert it for our needs */

	if (ahc->flags & AHC_USEDEFAULTS)
	{
		printf ("%s: Host Adapter Bios disabled.  Using default SCSI device parameters\n", ahc->name);

		ahc->flags |= AHC_EXTENDED_TRANS_A | AHC_EXTENDED_TRANS_B |
			AHC_TERM_ENB_A | AHC_TERM_ENB_B;

		discenable = ALL_TARGETS_MASK;

		if ((ahc->features & AHC_ULTRA) != 0)
			ultraenb = ALL_TARGETS_MASK;
	}
	else
	{
		discenable = ~((ahc_inb (ahc, DISC_DSB + 1) << 8) | ahc_inb (ahc, DISC_DSB));

		if ((ahc->features & (AHC_ULTRA | AHC_ULTRA2)) != 0)
			ultraenb = (ahc_inb (ahc, ULTRA_ENB + 1) << 8) | ahc_inb (ahc, ULTRA_ENB);
	}

	/*
	 *	Obtém o número de alvos
	 */
	if (ahc->features & AHC_WIDE)
		max_target = NTARGET;		/* 16 */
	else
		max_target = NTARGET / 2;	/*  8 */

	/*
	 *	Obtém os parâmetros dos alvos
	 */
	for (target = 0; target < max_target; target++)
	{
		struct ahc_tinfo_trio	*tinfo;
		uint			our_id;

		our_id = ahc->our_id;

		tinfo = &ahc->ahc_info[target].transinfo;

		/* Default to async narrow across the board */

		memclr (tinfo, sizeof (struct ahc_tinfo_trio));

		if (ahc->flags & AHC_USEDEFAULTS)
		{
			if ((ahc->features & AHC_WIDE) != 0)
				tinfo->user.width = MSG_EXT_WDTR_BUS_16_BIT;

			/*
			 *	These will be truncated when we determine the
			 *	connection type we have with the target.
			 */
			tinfo->user.period = ahc_syncrates->period;
			tinfo->user.offset = ~0;
		}
		else
		{
			uint		scsirate;
			ushort		mask;

			/* Take the settings leftover in scratch RAM. */

			scsirate = ahc_inb (ahc, TARG_SCSIRATE + target);
			mask = (0x01 << target);

			if   ((ahc->features & AHC_ULTRA2) != 0)
			{
				uint           offset;
				uint           maxsync;

				if ((scsirate & SOFS) == 0x0F)
				{
					/*
					 *	Haven't negotiated yet, so the format is different.
					 */
					scsirate = (scsirate & SXFR) >> 4 | (ultraenb & mask) ?
						0x08 : 0x0 | (scsirate & WIDEXFER);

					offset = MAX_OFFSET_ULTRA2;
				}
				else
				{
					offset = ahc_inb (ahc, TARG_OFFSET + target);
				}

				maxsync = AHC_SYNCRATE_ULTRA2;

				if ((ahc->features & AHC_DT) != 0)
					maxsync = AHC_SYNCRATE_DT;

				tinfo->user.period = ahc_find_period (ahc, scsirate, maxsync);

				if (offset == 0)
					tinfo->user.period = 0;
				else
					tinfo->user.offset = ~0;

				if ((scsirate & SXFR_ULTRA2) <= 8 /* 10MHz */ && (ahc->features & AHC_DT) != 0)
					tinfo->user.ppr_options = MSG_EXT_PPR_DT_REQ;
			}
			elif ((scsirate & SOFS) != 0)
			{
				if ((scsirate & SXFR) == 0x40 && (ultraenb & mask) != 0)
				{
					/* Treat 10MHz as a non-ultra speed */

					scsirate &= ~SXFR;
					ultraenb &= ~mask;
				}

				tinfo->user.period = ahc_find_period (ahc, scsirate, (ultraenb & mask) ?
							 AHC_SYNCRATE_ULTRA : AHC_SYNCRATE_FAST);

				if (tinfo->user.period != 0)
					tinfo->user.offset = ~0;
			}

			if ((scsirate & WIDEXFER) != 0 && (ahc->features & AHC_WIDE) != 0)
				tinfo->user.width = MSG_EXT_WDTR_BUS_16_BIT;

			tinfo->user.protocol_version = 4;

			if ((ahc->features & AHC_DT) != 0)
				tinfo->user.transport_version = 3;
			else
				tinfo->user.transport_version = 2;

			tinfo->goal.protocol_version = 2;
			tinfo->goal.transport_version = 2;
			tinfo->current.protocol_version = 2;
			tinfo->current.transport_version = 2;
		}

		ahc->ultraenb = ultraenb;
		ahc->discenable = discenable;
		ahc->tagenable = 0;	/* Wait until the XPT says its okay */

	}	/* end for (targets) */

	ahc->user_discenable = discenable;
	ahc->user_tagenable = tagenable;

	/* There are no untagged SCBs active yet. */

	for (target = 0; target < max_target; target++)
	{
		ahc_index_busy_tcl (ahc, target, 0, /* unbusy */ TRUE);

		if ((ahc->features & AHC_SCB_BTT) != 0)
		{
			int             lun;

			/*
			 *	The SCB based BTT allows an entry per target and lun pair.
			 */
			for (lun = 1; lun < AHC_NUM_LUNS; lun++)
			{
				ahc_index_busy_tcl (ahc, target, lun, /* unbusy */ TRUE);
			}
		}
	}

	/* All of our queues are empty */

	for (i = 0; i < 256; i++)
		ahc->qoutfifo[i] = SCB_LIST_NULL;

	for (i = 0; i < 256; i++)
		ahc->qinfifo[i] = SCB_LIST_NULL;

	if ((ahc->features & AHC_MULTI_TID) != 0)
	{
		ahc_outb (ahc, TARGID, 0);
		ahc_outb (ahc, TARGID + 1, 0);
	}

	/*
	 *	Tell the sequencer where it can find our arrays in memory.
	 */
#ifndef	BOOT
	physaddr = VIRT_TO_PHYS_ADDR (ahc->hscbs);
#else
	physaddr = (unsigned)ahc->hscbs;
#endif	BOOT

	ahc_outb (ahc, HSCB_ADDR,     physaddr);
	ahc_outb (ahc, HSCB_ADDR + 1, physaddr >> 8);
	ahc_outb (ahc, HSCB_ADDR + 2, physaddr >> 16);
	ahc_outb (ahc, HSCB_ADDR + 3, physaddr >> 24);

#ifndef	BOOT
	physaddr = VIRT_TO_PHYS_ADDR (ahc->qoutfifo);
#else
	physaddr = (unsigned)ahc->qoutfifo;
#endif	BOOT

	ahc_outb (ahc, SHARED_DATA_ADDR,     physaddr);
	ahc_outb (ahc, SHARED_DATA_ADDR + 1, physaddr >> 8);
	ahc_outb (ahc, SHARED_DATA_ADDR + 2, physaddr >> 16);
	ahc_outb (ahc, SHARED_DATA_ADDR + 3, physaddr >> 24);

	/*
	 * Initialize the group code to command length table. This overrides
	 * the values in TARG_SCSIRATE, so only setup the table after we have
	 * processed that information.
	 */
	ahc_outb (ahc, CMDSIZE_TABLE, 5);
	ahc_outb (ahc, CMDSIZE_TABLE + 1, 9);
	ahc_outb (ahc, CMDSIZE_TABLE + 2, 9);
	ahc_outb (ahc, CMDSIZE_TABLE + 3, 0);
	ahc_outb (ahc, CMDSIZE_TABLE + 4, 15);
	ahc_outb (ahc, CMDSIZE_TABLE + 5, 11);
	ahc_outb (ahc, CMDSIZE_TABLE + 6, 0);
	ahc_outb (ahc, CMDSIZE_TABLE + 7, 0);

	/* Tell the sequencer of our initial queue positions */

	ahc_outb (ahc, KERNEL_QINPOS, 0);
	ahc_outb (ahc, QINPOS, 0);
	ahc_outb (ahc, QOUTPOS, 0);

	/* Don't have any special messages to send to targets */

	ahc_outb (ahc, TARGET_MSG_REQUEST, 0);
	ahc_outb (ahc, TARGET_MSG_REQUEST + 1, 0);

	/*
	 *	Use the built in queue management registers if they are available.
	 */
	if ((ahc->features & AHC_QUEUE_REGS) != 0)
	{
		ahc_outb (ahc, QOFF_CTLSTA, SCB_QSIZE_256);
		ahc_outb (ahc, SDSCB_QOFF, 0);
		ahc_outb (ahc, SNSCB_QOFF, 0);
		ahc_outb (ahc, HNSCB_QOFF, 0);
	}

	/* We don't have any waiting selections */

	ahc_outb (ahc, WAITING_SCBH, SCB_LIST_NULL);

	/* Our disconnection list is empty too */

	ahc_outb (ahc, DISCONNECTED_SCBH, SCB_LIST_NULL);

	/* Message out buffer starts empty */

	ahc_outb (ahc, MSG_OUT, MSG_NOOP);

	/*
	 * Setup the allowed SCSI Sequences based on operational mode. If we
	 * are a target, we'll enalbe select in operations once we've had a
	 * lun enabled.
	 */
	scsiseq_template = ENSELO | ENAUTOATNO | ENAUTOATNP;

	if ((ahc->flags & AHC_INITIATORROLE) != 0)
		scsiseq_template |= ENRSELI;

	ahc_outb (ahc, SCSISEQ_TEMPLATE, scsiseq_template);

	/*
	 * Load the Sequencer program and Enable the adapter in "fast" mode.
	 */
#ifndef	BOOT
	if (scb.y_boot_verbose)
		printf ("%s: Downloading Sequencer Program ...", ahc->name);
#endif	BOOT

	ahc_loadseq (ahc);

	if ((ahc->features & AHC_ULTRA2) != 0)
	{
		int             wait;

		/*
		 * Wait for up to 500ms for our transceivers to settle.  If
		 * the adapter does not have a cable attached, the
		 * tranceivers may never settle, so don't complain if we fail
		 * here.
		 */
		pause_sequencer (ahc);

		for (wait = 5000; (ahc_inb (ahc, SBLKCTL) & (ENAB40 | ENAB20)) == 0 && wait; wait--)
			ahc_delay (100);

		unpause_sequencer (ahc);
	}

	/*
	 *	Prepara a primeira negociação
	 */
	for (i = 0; i < max_target; i++)
		XPT_SET_TRAN_SETTINGS (ahc, i);

	/*
	 *	x
	 */

	return (0);

}	/* end ahc_init */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/************************** Busy Target Table *********************************/

/*
 * Return the untagged transaction id for a given target/lun.
 * Optionally, clear the entry.
 */
uint
ahc_index_busy_tcl (struct ahc_softc *ahc, int target, int lun, int unbusy)
{
	uint           scbid;

	if ((ahc->features & AHC_SCB_BTT) != 0)
	{
		uint           saved_scbptr;

		saved_scbptr = ahc_inb (ahc, SCBPTR);
		ahc_outb (ahc, SCBPTR, lun);
		scbid = ahc_inb (ahc, SCB_64_BTT + target);

		if (unbusy)
			ahc_outb (ahc, SCB_64_BTT + target, SCB_LIST_NULL);

		ahc_outb (ahc, SCBPTR, saved_scbptr);
	}
	else
	{
		scbid = ahc_inb (ahc, BUSY_TARGETS + target);

		if (unbusy)
			ahc_outb (ahc, BUSY_TARGETS + target, SCB_LIST_NULL);
	}

	return (scbid);

}	/* end ahc_index_busy_tcl */

/*
 ****************************************************************
 *	Procura SRAM externa					*
 ****************************************************************
 */
void
ahc_probe_ext_scbram (struct ahc_softc *ahc)
{
	int		num_scbs, test_num_scbs;
	int		enable, pcheck, fast, large;

	enable	 = FALSE;
	pcheck	 = FALSE;
	fast	 = FALSE;
	large	 = FALSE;
	num_scbs = 0;

	if (ahc_ext_scbram_present (ahc) == 0)
		goto done;

	/*
	 *	Probe for the best parameters to use
	 */
	ahc_scbram_config (ahc, /* enable */ TRUE, pcheck, fast, large);

	if ((num_scbs = ahc_probe_scbs (ahc)) == 0)
		goto done;	/* The SRAM wasn't really present */

	enable = TRUE;

	/*
	 *	Clear any outstanding parity error and ensure that parity error reporting is enabled
	 */
	ahc_outb (ahc, SEQCTL, 0);
	ahc_outb (ahc, CLRINT, CLRPARERR);
	ahc_outb (ahc, CLRINT, CLRBRKADRINT);

	/*
	 *	Now see if we can do parity
	 */
	ahc_scbram_config (ahc, enable, /* pcheck */ TRUE, fast, large);
	num_scbs = ahc_probe_scbs (ahc);

	if ((ahc_inb (ahc, INTSTAT) & BRKADRINT) == 0 || (ahc_inb (ahc, ERROR) & MPARERR) == 0)
		pcheck = TRUE;

	/* Clear any resulting parity error */

	ahc_outb (ahc, CLRINT, CLRPARERR);
	ahc_outb (ahc, CLRINT, CLRBRKADRINT);

	/* Now see if we can do fast timing */

	ahc_scbram_config (ahc, enable, pcheck, /* fast */ TRUE, large);

	if
	(	(test_num_scbs = ahc_probe_scbs (ahc)) == num_scbs &&
		((ahc_inb (ahc, INTSTAT) & BRKADRINT) == 0 || (ahc_inb (ahc, ERROR) & MPARERR) == 0)
	)
		fast = TRUE;

	/*
	 *	See if we can use large SCBs and still maintain the same overall count of SCBs
	 */
	if ((ahc->features & AHC_LARGE_SCBS) != 0)
	{
		ahc_scbram_config (ahc, enable, pcheck, fast, /* large */ TRUE);

		if ((test_num_scbs = ahc_probe_scbs (ahc)) >= num_scbs)
		{
			large = TRUE;

			if ((num_scbs = test_num_scbs) >= 64)
			{
				/*
				 *	We have enough space to move the "busy targets table" into
				 *	SCB space and make it qualify all the way to the lun level
				 */
				ahc->flags |= AHC_SCB_BTT;
			}
		}
	}

	/*
	 *	Disable parity error reporting until we can load instruction ram
	 */
    done:
	ahc_outb (ahc, SEQCTL, PERRORDIS | FAILDIS);

	/* Clear any latched parity error */

	ahc_outb (ahc, CLRINT, CLRPARERR);
	ahc_outb (ahc, CLRINT, CLRBRKADRINT);

#ifndef	BOOT
	if (scb.y_boot_verbose)
	{
		if (enable)
		{
			printf
			(	"%s: External SRAM, %s access%s, %d bytes/SCB\n",
				ahc->name, fast ? "fast" : "slow",
				pcheck ? ", parity checking enabled" : "",
				large ? 64 : 32
			);
		}
		else
		{
			printf ("%s: NO external SRAM\n", ahc->name);
		}
	}
#endif	BOOT

	ahc_scbram_config (ahc, enable, pcheck, fast, large);

}	/* end ahc_probe_ext_scbram */

/*
 ****************************************************************
 *	Procura SRAM externa não compartilhada			*
 ****************************************************************
 */
int
ahc_ext_scbram_present (struct ahc_softc *ahc)
{
	uint		chip;
	int		ramps, single_user;
	ulong		devconfig;

	chip = ahc->chip & AHC_CHIPID_MASK;
	devconfig = pci_read (ahc->dev_softc, DEVCONFIG, /* bytes */ 4);
	single_user = (devconfig & MPORTMODE) != 0;

	if   ((ahc->features & AHC_ULTRA2) != 0)
		ramps = (ahc_inb (ahc, DSCOMMAND0) & RAMPS) != 0;
	elif (chip >= AHC_AIC7870)
		ramps = (devconfig & RAMPSM) != 0;
	else
		ramps = 0;

	if (ramps && single_user)
		return (1);

	return (0);

}	/* end ahc_ext_scbram_present */

/*
 ****************************************************************
 *	Enable external scbram					*
 ****************************************************************
 */
void
ahc_scbram_config (struct ahc_softc *ahc, int enable, int pcheck, int fast, int large)
{
	ulong		devconfig;

	/*
	 *	Set the SCB Base addr (highest address bit) depending on which channel we are
	 */
	if (ahc->features & AHC_MULTI_FUNC)
		ahc_outb (ahc, SCBBADDR, ahc->dev_softc->pci_func);

	devconfig = pci_read (ahc->dev_softc, DEVCONFIG, /* bytes */ 4);

	if ((ahc->features & AHC_ULTRA2) != 0)
	{
		uint		dscommand0;

		dscommand0 = ahc_inb (ahc, DSCOMMAND0);

		if (enable)
			dscommand0 &= ~INTSCBRAMSEL;
		else
			dscommand0 |= INTSCBRAMSEL;

		if (large)
			dscommand0 &= ~USCBSIZE32;
		else
			dscommand0 |= USCBSIZE32;

		ahc_outb (ahc, DSCOMMAND0, dscommand0);
	}
	else
	{
		if (fast)
			devconfig &= ~EXTSCBTIME;
		else
			devconfig |= EXTSCBTIME;

		if (enable)
			devconfig &= ~SCBRAMSEL;
		else
			devconfig |= SCBRAMSEL;

		if (large)
			devconfig &= ~SCBSIZE32;
		else
			devconfig |= SCBSIZE32;
	}

	if (pcheck)
		devconfig |= EXTSCBPEN;
	else
		devconfig &= ~EXTSCBPEN;

	pci_write (ahc->dev_softc, DEVCONFIG, devconfig, /* bytes */ 4);

}	/* end ahc_scbram_config */

/*
 ****************************************************************
 *	Examina a SEEPROM e terminação dos cabos		*
 ****************************************************************
 */
void
ahc_check_extport (struct ahc_softc *ahc, uint *sxfrctl1)
{
	struct seeprom_descriptor	sd;
	struct seeprom_config		sc;
	uint				scsi_conf, adapter_control;
	int				have_seeprom, have_autoterm;

	sd.sd_ahc		= ahc;
	sd.sd_control_offset	= SEECTL;
	sd.sd_status_offset	= SEECTL;
	sd.sd_dataout_offset	= SEECTL;

	/*
	 *	For some multi-channel devices, the c46 is simply too small to work.  For the other
	 *	controller types, we can get our information from either SEEPROM type.  Set the type
	 *	to start our probe with accordingly
	 */
	if (ahc->flags & AHC_LARGE_SEEPROM)
		sd.sd_chip = C56_66;
	else
		sd.sd_chip = C46;

	sd.sd_MS  = SEEMS;
	sd.sd_RDY = SEERDY;
	sd.sd_CS  = SEECS;
	sd.sd_CK  = SEECK;
	sd.sd_DO  = SEEDO;
	sd.sd_DI  = SEEDI;

	if (have_seeprom = ahc_acquire_seeprom (ahc, &sd))
	{
#ifndef	BOOT
		if (scb.y_boot_verbose)
			printf ("%s: Reading SEEPROM ...", ahc->name);
#endif	BOOT
		for (EVER)
		{
			uint		start_addr;

			start_addr = 32 * (ahc->channel - 'A');

			have_seeprom = ahc_read_seeprom (&sd, (ushort *)&sc, start_addr, sizeof (sc) / 2);

			if (have_seeprom)
				have_seeprom = ahc_verify_cksum (&sc);

			if (have_seeprom != 0 || sd.sd_chip == C56_66)
			{
#ifndef	BOOT
				if (scb.y_boot_verbose)
				{
					if (have_seeprom == 0)
						printf (" checksum error\n");
					else
						printf (" done\n");
				}
#endif	BOOT
				break;
			}

			sd.sd_chip = C56_66;

		}	/* end for (EVER) */

	}	/* end if (have seeprom) */

	/*
	 *	Verifica se temos SEEPROM
	 */
	if (!have_seeprom)
	{
		printf ("%s: No SEEPROM available!\n", ahc->name);
		ahc->flags |= AHC_USEDEFAULTS;
	}
	else
	{
		/*
		 *	Put the data we've collected down into SRAM where ahc_init will find it
		 */
		int		i, max_targ = sc.max_targets & CFMAXTARG;
		ushort		discenable, ultraenb;

		discenable = 0; ultraenb = 0;

		if ((sc.adapter_control & CFULTRAEN) != 0)
		{
			/*
			 *	Determine if this adapter has a "newstyle" SEEPROM format
			 */
			for (i = 0; i < max_targ; i++)
			{
				if ((sc.device_flags[i] & CFSYNCHISULTRA) != 0)
				{
					ahc->flags |= AHC_NEWEEPROM_FMT;
					break;
				}
			}
		}

		for (i = 0; i < max_targ; i++)
		{
			uint		scsirate;
			ushort		target_mask;

			target_mask = 0x01 << i;

			if (sc.device_flags[i] & CFDISC)
				discenable |= target_mask;

			if   ((ahc->flags & AHC_NEWEEPROM_FMT) != 0)
			{
				if ((sc.device_flags[i] & CFSYNCHISULTRA) != 0)
					ultraenb |= target_mask;
			}
			elif ((sc.adapter_control & CFULTRAEN) != 0)
			{
				ultraenb |= target_mask;
			}

			if ((sc.device_flags[i] & CFXFER) == 0x04 && (ultraenb & target_mask) != 0)
			{
				/* Treat 10MHz as a non-ultra speed */

				sc.device_flags[i] &= ~CFXFER;
				ultraenb &= ~target_mask;
			}

			if ((ahc->features & AHC_ULTRA2) != 0)
			{
				uint		offset;

				if (sc.device_flags[i] & CFSYNCH)
					offset = MAX_OFFSET_ULTRA2;
				else
					offset = 0;

				ahc_outb (ahc, TARG_OFFSET + i, offset);

				/*
				 * The ultra enable bits contain the high bit of the ultra2 sync rate field
				 */
				scsirate = (sc.device_flags[i] & CFXFER) | ((ultraenb & target_mask) ? 8 : 0);

				if (sc.device_flags[i] & CFWIDEB)
					scsirate |= WIDEXFER;
			}
			else
			{
				scsirate = (sc.device_flags[i] & CFXFER) << 4;

				if (sc.device_flags[i] & CFSYNCH)
					scsirate |= SOFS;
				if (sc.device_flags[i] & CFWIDEB)
					scsirate |= WIDEXFER;
			}

			ahc_outb (ahc, TARG_SCSIRATE + i, scsirate);
		}

		ahc->our_id = sc.brtime_id & CFSCSIID;

		scsi_conf = (ahc->our_id & 0x7);

		if (sc.adapter_control & CFSPARITY)
			scsi_conf |= ENSPCHK;
		if (sc.adapter_control & CFRESETB)
			scsi_conf |= RESET_SCSI;

		if (sc.bios_control & CFEXTEND)
			ahc->flags |= AHC_EXTENDED_TRANS_A;

		if (ahc->features & AHC_ULTRA && (ahc->flags & AHC_NEWEEPROM_FMT) == 0)
		{
			/* Should we enable Ultra mode? */

			if (!(sc.adapter_control & CFULTRAEN))
				ultraenb = 0; 		/* Treat us as a non-ultra card */
		}

		if (sc.signature == CFSIGNATURE)
		{
			ulong		devconfig;

			/* Honor the STPWLEVEL settings */

			devconfig = pci_read (ahc->dev_softc, DEVCONFIG, /* bytes */ 4);
			devconfig &= ~STPWLEVEL;

			if ((sc.bios_control & CFSTPWLEVEL) != 0)
				devconfig |= STPWLEVEL;

			pci_write (ahc->dev_softc, DEVCONFIG, devconfig, /* bytes */ 4);
		}

		/* Set SCSICONF info */

		ahc_outb (ahc, SCSICONF, scsi_conf);
		ahc_outb (ahc, DISC_DSB,      ~discenable);
		ahc_outb (ahc, DISC_DSB + 1, ~(discenable >> 8));
		ahc_outb (ahc, ULTRA_ENB,      ultraenb);
		ahc_outb (ahc, ULTRA_ENB + 1, (ultraenb >> 8));
	}

	/*
	 *	Cards that have the external logic necessary to talk to a SEEPROM,
	 *	are almost certain to have the remaining logic necessary for
	 *	auto-termination control.  This assumption hasn't failed yet ...
	 */
	have_autoterm = have_seeprom;

	if (have_seeprom)
		adapter_control = sc.adapter_control;
	else
		adapter_control = CFAUTOTERM;

	/*
	 *	Some low-cost chips have SEEPROM and auto-term control built in,
	 *	instead of using a GAL.  They can tell us directly if the
	 *	termination logic is enabled
	 */
	if ((ahc->features & AHC_SPIOCAP) != 0)
	{
		if ((ahc_inb (ahc, SPIOCAP) & SSPIOCPS) != 0)
			have_autoterm = TRUE;
		else
			have_autoterm = FALSE;
	}

	if (have_autoterm)
		ahc_configure_termination (ahc, &sd, adapter_control, sxfrctl1);

	/* Release access to the memory port and the serial EEPROM. (release_seeprom (&sd);) */

	SEEPROM_OUTB ((&sd), 0);

}	/* end ahc_check_extport */

/*
 ****************************************************************
 *	Configura a terminação					*
 ****************************************************************
 */
void
ahc_configure_termination (struct ahc_softc *ahc, struct seeprom_descriptor *sd, uint control, uint *sxfrctl1)
{
	uchar		brddat = 0;

	/*
	 *	Update the settings in sxfrctl1 to match the termination settings
	 */
	*sxfrctl1 = 0;

	/*
	 *	SEECS must be on for the GALS to latch the data properly.
	 * 	Be sure to leave MS on or we will release the seeprom
	 */
	SEEPROM_OUTB (sd, sd->sd_MS | sd->sd_CS);

	if ((control & CFAUTOTERM) != 0 || (ahc->features & AHC_NEW_TERMCTL) != 0)
	{
		int		internal50_present, internal68_present, externalcable_present;
		int		eeprom_present;
		int		enableSEC_low = 0, enableSEC_high = 0;
		int		enablePRI_low = 0, enablePRI_high = 0;

		if   ((ahc->features & AHC_NEW_TERMCTL) != 0)
		{
			ahc_term_detect
			(	ahc, &enableSEC_low, &enableSEC_high, &enablePRI_low,
				&enablePRI_high, &eeprom_present
			);

			if ((control & CFSEAUTOTERM) == 0)
			{
#ifndef	BOOT
				if (scb.y_boot_verbose)
					printf ("%s: Manual SE Termination\n", ahc->name);
#endif	BOOT
				enableSEC_low = (control & CFSELOWTERM);
				enableSEC_high = (control & CFSEHIGHTERM);
			}

			if ((control & CFAUTOTERM) == 0)
			{
#ifndef	BOOT
				if (scb.y_boot_verbose)
					printf ("%s: Manual LVD Termination\n", ahc->name);
#endif	BOOT
				enablePRI_low = (control & CFSTERM);
				enablePRI_high = (control & CFWSTERM);
			}

			/* Make the table calculations below happy */

			internal50_present = 0;
			internal68_present = 1;
			externalcable_present = 1;
		}
		elif ((ahc->features & AHC_SPIOCAP) != 0)
		{
			aic785X_cable_detect
			(	ahc, &internal50_present, &externalcable_present, &eeprom_present
			);
		}
		else
		{
			aic787X_cable_detect
			(	ahc, &internal50_present, &internal68_present,
				&externalcable_present, &eeprom_present
			);
		}

		if ((ahc->features & AHC_WIDE) == 0)
			internal68_present = 0;
#ifndef	BOOT
		if (scb.y_boot_verbose)
		{
			if ((ahc->features & AHC_ULTRA2) == 0)
			{
				printf
				(	"%s: internal 50 cable %s present, internal 68 cable %s present\n",
					ahc->name,
					internal50_present ? "is" : "not",
					internal68_present ? "is" : "not"
				);

				printf
				(	"%s: external cable %s present\n",
					ahc->name, externalcable_present ? "is" : "not"
				);
			}

			printf
			(	"%s: BIOS eeprom %s present\n",
				ahc->name, eeprom_present ? "is" : "not"
			);
		}
#endif	BOOT

		/*
		 *	The 50 pin connector is a separate bus, so force
		 *	it to always be terminated. In the future, perform
		 *	current sensing to determine if we are in the
		 *	middle of a properly terminated bus
		 */
		if ((ahc->flags & AHC_INT50_SPEEDFLEX) != 0)
			internal50_present = 0;

		/*
		 *	Now set the termination based on what we found. Flash
		 *	Enable = BRDDAT7 Secondary High Term Enable = BRDDAT6
		 *	Secondary Low Term Enable = BRDDAT5 (7890) Primary High
		 *	Term Enable = BRDDAT4 (7890)
		 */
		if
		(	(ahc->features & AHC_ULTRA2) == 0 && (internal50_present != 0) &&
			(internal68_present != 0) && (externalcable_present != 0)
		)
		{
			printf
			(	"%s: Illegal cable configuration!!. Only two connectors on the "
				"adapter may be used at a time!\n", ahc->name
			);
		}

		if
		(	(ahc->features & AHC_WIDE) != 0 && ((externalcable_present == 0) ||
			(internal68_present == 0) || (enableSEC_high != 0))
		)
		{
			brddat |= BRDDAT6;
#ifndef	BOOT
			if (scb.y_boot_verbose)
			{
				if ((ahc->flags & AHC_INT50_SPEEDFLEX) != 0)
				{
					printf ("%s: 68 pin termination Enabled\n", ahc->name);
				}
				else
				{
					printf
					(	"%s: %sHigh byte termination Enabled\n",
						ahc->name, enableSEC_high ? "Secondary " : ""
					);
				}
			}
#endif	BOOT
		}

		if
		(	((internal50_present ? 1 : 0) + (internal68_present ? 1 : 0) +
			(externalcable_present ? 1 : 0)) <= 1 || (enableSEC_low != 0)
		)
		{
			if ((ahc->features & AHC_ULTRA2) != 0)
				brddat |= BRDDAT5;
			else
				*sxfrctl1 |= STPWEN;
#ifndef	BOOT
			if (scb.y_boot_verbose)
			{
				if ((ahc->flags & AHC_INT50_SPEEDFLEX) != 0)
				{
					printf ("%s: 50 pin termination Enabled\n", ahc->name);
				}
				else
				{
					printf
					(	"%s: %sLow byte termination Enabled\n",
						ahc->name, enableSEC_low ? "Secondary " : ""
					);
				}
			}
#endif	BOOT
		}

		if (enablePRI_low != 0)
		{
			*sxfrctl1 |= STPWEN;
#ifndef	BOOT
			if (scb.y_boot_verbose)
				printf ("%s: Primary Low Byte termination Enabled\n", ahc->name);
#endif	BOOT
		}

		/*
		 *	Setup STPWEN before setting up the rest of the termination
		 *	per the tech note on the U160 cards
		 */
		ahc_outb (ahc, SXFRCTL1, *sxfrctl1);

		if (enablePRI_high != 0)
		{
			brddat |= BRDDAT4;
#ifndef	BOOT
			if (scb.y_boot_verbose)
				printf ("%s: Primary High Byte termination Enabled\n", ahc->name);
#endif	BOOT
		}

		ahc_write_brdctl (ahc, brddat);
	}
	else
	{
		if ((control & CFSTERM) != 0)
		{
			*sxfrctl1 |= STPWEN;
#ifndef	BOOT
			if (scb.y_boot_verbose)
			{
				printf
				(	"%s: %sLow byte termination Enabled\n",
					ahc->name, (ahc->features & AHC_ULTRA2) ? "Primary " : ""
				);
			}
#endif	BOOT
		}

		if ((control & CFWSTERM) != 0)
		{
			brddat |= BRDDAT6;
#ifndef	BOOT
			if (scb.y_boot_verbose)
			{
				printf
				(	"%s: %sHigh byte termination Enabled\n",
					ahc->name, (ahc->features & AHC_ULTRA2) ? "Secondary " : ""
				);
			}
#endif	BOOT
		}

		/*
		 *	Setup STPWEN before setting up the rest of the termination
		 *	per the tech note on the U160 cards
		 */
		ahc_outb (ahc, SXFRCTL1, *sxfrctl1);

		ahc_write_brdctl (ahc, brddat);
	}

	SEEPROM_OUTB (sd, sd->sd_MS);	/* Clear CS */

}	/* end ahc_configure_termination */

/*
 ****************************************************************
 *	Deteta as terminações					*
 ****************************************************************
 */
void
ahc_term_detect
(	struct ahc_softc *ahc, int *enableSEC_low, int *enableSEC_high,
	int *enablePRI_low, int *enablePRI_high, int *eeprom_present
)
{
	uchar		brdctl;

	/*
	 *	BRDDAT7 = Eeprom
	 *	BRDDAT6 = Enable Secondary High Byte termination
	 *	BRDDAT5 = Enable Secondary Low Byte termination
	 *	BRDDAT4 = Enable Primary high byte termination
	 *	BRDDAT3 = Enable Primary low byte termination
	 */
	brdctl = ahc_read_brdctl (ahc);

	*eeprom_present = (brdctl & BRDDAT7);
	*enableSEC_high = (brdctl & BRDDAT6);
	*enableSEC_low  = (brdctl & BRDDAT5);
	*enablePRI_high = (brdctl & BRDDAT4);
	*enablePRI_low  = (brdctl & BRDDAT3);

}	/* end ahc_term_detect */

/*
 ****************************************************************
 *	Deteta a presença dos cabos do chip 787x		*
 ****************************************************************
 */
void
aic787X_cable_detect
(	struct ahc_softc *ahc, int *internal50_present, int *internal68_present,
	int *externalcable_present, int *eeprom_present
)
{
	uchar		brdctl;

	/*
	 *	First read the status of our cables. Set the rom bank to 0 since
	 *	the bank setting serves as a multiplexor for the cable detection
	 *	logic. BRDDAT5 controls the bank switch
	 */
	ahc_write_brdctl (ahc, 0);

	/*
	 *	Now read the state of the internal connectors.  BRDDAT6 is INT50 and BRDDAT7 is INT68
	 */
	brdctl = ahc_read_brdctl (ahc);
	*internal50_present = !(brdctl & BRDDAT6);
	*internal68_present = !(brdctl & BRDDAT7);

	/*
	 *	Set the rom bank to 1 and determine the other signals
	 */
	ahc_write_brdctl (ahc, BRDDAT5);

	/*
	 *	Now read the state of the external connectors.  BRDDAT6 is EXT68 and BRDDAT7 is EPROMPS
	 */
	brdctl = ahc_read_brdctl (ahc);
	*externalcable_present = !(brdctl & BRDDAT6);
	*eeprom_present = brdctl & BRDDAT7;

}	/* end aic787X_cable_detect */

/*
 ****************************************************************
 *	Deteta a presença dos cabos do chip 785x		*
 ****************************************************************
 */
void
aic785X_cable_detect
(	struct ahc_softc *ahc, int *internal50_present,
	int *externalcable_present, int *eeprom_present
)
{
	uchar		brdctl;

	ahc_outb (ahc, BRDCTL, BRDRW | BRDCS);
	ahc_outb (ahc, BRDCTL, 0);
	brdctl = ahc_inb (ahc, BRDCTL);

	*internal50_present = !(brdctl & BRDDAT5);
	*externalcable_present = !(brdctl & BRDDAT6);
	*eeprom_present = (ahc_inb (ahc, SPIOCAP) & EEPROM) != 0;

}	/* end aic785X_cable_detect */

/*
 ******	SEEPROM *************************************************
 *
 *
 *   The instruction set of the 93C66/56/46/26/06 chips are as follows:
 *
 *               Start  OP	    *
 *     Function   Bit  Code  Address**  Data     Description
 *     -------------------------------------------------------------------
 *     READ        1    10   A5 - A0             Reads data stored in memory,
 *                                               starting at specified address
 *     EWEN        1    00   11XXXX              Write enable must preceed
 *                                               all programming modes
 *     ERASE       1    11   A5 - A0             Erase register A5A4A3A2A1A0
 *     WRITE       1    01   A5 - A0   D15 - D0  Writes register
 *     ERAL        1    00   10XXXX              Erase all registers
 *     WRAL        1    00   01XXXX    D15 - D0  Writes to all registers
 *     EWDS        1    00   00XXXX              Disables all programming
 *                                               instructions
 *     *Note: A value of X for address is a don't care condition.
 *    **Note: There are 8 address bits for the 93C56/66 chips unlike
 *	      the 93C46/26/06 chips which have 6 address bits.
 *
 *   The 93C46 has a four wire interface: clock, chip select, data in, and
 *   data out.  In order to perform one of the above functions, you need
 *   to enable the chip select for a clock period (typically a minimum of
 *   1 usec, with the clock high and low a minimum of 750 and 250 nsec
 *   respectively).  While the chip select remains high, you can clock in
 *   the instructions (above) starting with the start bit, followed by the
 *   OP code, Address, and Data (if needed).  For the READ instruction, the
 *   requested 16-bit register contents is read from the data out line but
 *   is preceded by an initial zero (leading 0, followed by 16-bits, MSB
 *   first).  The clock cycling from low to high initiates the next data
 *   bit to be sent from the chip.
 *
 *	Right now, we only have to read the SEEPROM.  But we make it easier to
 *	add other 93Cx6 functions.
 */
entry struct seeprom_cmd
{
  	uchar		len;
 	uchar		bits[3];

}	ahc_seeprom_read = {3, {1, 1, 0}};

/*
 *	Wait for the SEERDY to go high; about 800 ns
 */
#define CLOCK_PULSE(sd, rdy)				\
	while ((SEEPROM_STATUS_INB(sd) & rdy) == 0) {	\
		;  /* Do nothing */			\
	}						\
	SEEPROM_INB (sd);	/* Clear clock */

/*
 ****************************************************************
 *	Verifica se há e acessa a SEEPROM			*
 ****************************************************************
 */
int
ahc_acquire_seeprom (struct ahc_softc *ahc, struct seeprom_descriptor *sd)
{
	int		wait;

	if ((ahc->features & AHC_SPIOCAP) != 0 && (ahc_inb (ahc, SPIOCAP) & SEEPROM) == 0)
		return (0);

	/*
	 *	Request access of the memory port.  When access is granted, SEERDY
	 *	will go high.  We use a 1 second timeout which should be near 1
	 *	second more than is needed.  Reason: after the chip reset, there
	 *	should be no contention
	 */
	SEEPROM_OUTB (sd, sd->sd_MS);

	wait = 1000;		/* 1 second timeout in msec */

	while (--wait && ((SEEPROM_STATUS_INB (sd) & sd->sd_RDY) == 0))
	{
		ahc_delay (1000);	/* delay 1 msec */
	}

	if ((SEEPROM_STATUS_INB (sd) & sd->sd_RDY) == 0)
	{
		SEEPROM_OUTB (sd, 0);
		return (0);
	}

	return (1);

}	/* end ahc_acquire_seeprom */

/*
 ****************************************************************
 *	Lê a EEPROM						*
 ****************************************************************
 */
int
ahc_read_seeprom (struct seeprom_descriptor *sd, ushort *buf, uint start_addr, uint count)
{
	int		i = 0;
	uint		k = 0;
	ushort		v;
	uchar		temp;

	/*
	 *	Read the serial EEPROM and returns 1 if successful and 0 if not successful
	 *
	 *	Read the requested registers of the seeprom. The loop will range from 0 to count-1.
	 */
	for (k = start_addr; k < count + start_addr; k++)
	{
		/* Send chip select for one clock cycle. */

		temp = sd->sd_MS ^ sd->sd_CS;
		SEEPROM_OUTB (sd, temp ^ sd->sd_CK);
		CLOCK_PULSE (sd, sd->sd_RDY);

		/*
		 *	Now we're ready to send the read command followed by the
		 *	address of the 16-bit register we want to read.
		 */
		for (i = 0; i < ahc_seeprom_read.len; i++)
		{
			if (ahc_seeprom_read.bits[i] != 0)
				temp ^= sd->sd_DO;

			SEEPROM_OUTB (sd, temp);
			CLOCK_PULSE (sd, sd->sd_RDY);
			SEEPROM_OUTB (sd, temp ^ sd->sd_CK);
			CLOCK_PULSE (sd, sd->sd_RDY);

			if (ahc_seeprom_read.bits[i] != 0)
				temp ^= sd->sd_DO;
		}

		/* Send the 6 or 8 bit address (MSB first, LSB last). */

		for (i = (sd->sd_chip - 1); i >= 0; i--)
		{
			if ((k & (1 << i)) != 0)
				temp ^= sd->sd_DO;

			SEEPROM_OUTB (sd, temp);
			CLOCK_PULSE (sd, sd->sd_RDY);
			SEEPROM_OUTB (sd, temp ^ sd->sd_CK);
			CLOCK_PULSE (sd, sd->sd_RDY);

			if ((k & (1 << i)) != 0)
				temp ^= sd->sd_DO;
		}

		/*
		 *	Now read the 16 bit register.  An initial 0 precedes the
		 *	register contents which begins with bit 15 (MSB) and ends
		 *	with bit 0 (LSB).  The initial 0 will be shifted off the
		 *	top of our word as we let the loop run from 0 to 16.
		 */
		v = 0;

		for (i = 16; i >= 0; i--)
		{
			SEEPROM_OUTB (sd, temp);
			CLOCK_PULSE (sd, sd->sd_RDY);
			v <<= 1;

			if (SEEPROM_DATA_INB (sd) & sd->sd_DI)
				v |= 1;

			SEEPROM_OUTB (sd, temp ^ sd->sd_CK);
			CLOCK_PULSE (sd, sd->sd_RDY);
		}

		buf[k - start_addr] = v;

		/* Reset the chip select for the next command cycle. */

		temp = sd->sd_MS;
		SEEPROM_OUTB (sd, temp);
		CLOCK_PULSE (sd, sd->sd_RDY);
		SEEPROM_OUTB (sd, temp ^ sd->sd_CK);
		CLOCK_PULSE (sd, sd->sd_RDY);
		SEEPROM_OUTB (sd, temp);
		CLOCK_PULSE (sd, sd->sd_RDY);
	}

#ifdef AHC_DUMP_EEPROM
	printf ("\nSerial EEPROM:\n\t");
	for  (k = 0; k < count; k = k + 1) {
		if (((k % 8) == 0) && (k != 0)) {
			printf ("\n\t");
		}
		printf (" 0x%x", buf[k]);
	}
	printf ("\n");
#endif
	return (1);

}	/* end read_seeprom */

/*
 ****************************************************************
 *	Examina o "checksum" da SEEPROM				*
 ****************************************************************
 */
int
ahc_verify_cksum (struct seeprom_config *sc)
{
	int		i, maxaddr;
	ulong		checksum;
	ushort		*scarray;

	maxaddr = (sizeof (struct seeprom_config) / 2) - 1;
	checksum = 0;
	scarray = (ushort *)sc;

	for (i = 0; i < maxaddr; i++)
		checksum = checksum + scarray[i];

	if (checksum == 0 || (checksum & 0xFFFF) != sc->checksum)
	{
		return (0);
	}
	else
	{
		return (1);
	}

}	/* end ahc_verify_cksum */

/*
 ****************************************************************
 *	Escreve no registro de função desconhecida		*
 ****************************************************************
 */
void
ahc_write_brdctl (struct ahc_softc *ahc, uchar value)
{
	uchar		brdctl;

	if   ((ahc->chip & AHC_CHIPID_MASK) == AHC_AIC7895)
	{
		brdctl = BRDSTB;

		if (ahc->channel == 'B')
			brdctl |= BRDCS;
	}
	elif ((ahc->features & AHC_ULTRA2) != 0)
	{
		brdctl = 0;
	}
	else
	{
		brdctl = BRDSTB | BRDCS;
	}

	ahc_outb (ahc, BRDCTL, brdctl);
	ahc_delay (20);
	brdctl |= value;
	ahc_outb (ahc, BRDCTL, brdctl);
	ahc_delay (20);

	if ((ahc->features & AHC_ULTRA2) != 0)
		brdctl |= BRDSTB_ULTRA2;
	else
		brdctl &= ~BRDSTB;

	ahc_outb (ahc, BRDCTL, brdctl);
	ahc_delay (20);

	if ((ahc->features & AHC_ULTRA2) != 0)
		brdctl = 0;
	else
		brdctl &= ~BRDCS;

	ahc_outb (ahc, BRDCTL, brdctl);

}	/* end ahc_write_brdctl */

/*
 ****************************************************************
 *	Le o registro de função desconhecida			*
 ****************************************************************
 */
uchar
ahc_read_brdctl (struct ahc_softc *ahc)
{
	uchar		brdctl, value;

	if   ((ahc->chip & AHC_CHIPID_MASK) == AHC_AIC7895)
	{
		brdctl = BRDRW;

		if (ahc->channel == 'B')
			brdctl |= BRDCS;
	}
	elif ((ahc->features & AHC_ULTRA2) != 0)
	{
		brdctl = BRDRW_ULTRA2;
	}
	else
	{
		brdctl = BRDRW | BRDCS;
	}

	ahc_outb (ahc, BRDCTL, brdctl);
	ahc_delay (20);
	value = ahc_inb (ahc, BRDCTL);
	ahc_outb (ahc, BRDCTL, 0);

	return (value);

}	/* end ahc_read_brdctl */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7850			*
 ****************************************************************
 */
int
ahc_aic7850_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7850;
	probe_config->features	 = AHC_AIC7850_FE;
	probe_config->bugs	|= AHC_TMODE_WIDEODD_BUG|AHC_CACHETHEN_BUG|AHC_PCI_MWI_BUG;

	return (0);

}	/* end ahc_aic7850_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7855			*
 ****************************************************************
 */
int
ahc_aic7855_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7855;
	probe_config->features	 = AHC_AIC7855_FE;
	probe_config->bugs	|= AHC_TMODE_WIDEODD_BUG | AHC_CACHETHEN_BUG | AHC_PCI_MWI_BUG;

	return (0);

}	/* end ahc_aic7855_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7860			*
 ****************************************************************
 */
int
ahc_aic7860_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7860;
	probe_config->features	 = AHC_AIC7860_FE;
	probe_config->bugs	|= AHC_TMODE_WIDEODD_BUG | AHC_CACHETHEN_BUG | AHC_PCI_MWI_BUG;

	if (pci->pci_revid >= 1)
		probe_config->bugs |= AHC_PCI_2_1_RETRY_BUG;

	return (0);

}	/* end ahc_aic7860_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7870			*
 ****************************************************************
 */
int
ahc_aic7870_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7870;
	probe_config->features	 = AHC_AIC7870_FE;
	probe_config->bugs	|= AHC_TMODE_WIDEODD_BUG | AHC_CACHETHEN_BUG | AHC_PCI_MWI_BUG;

	return (0);

}	/* end ahc_aic7870_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 394x		*
 ****************************************************************
 */
int
ahc_aha394X_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	if ((error = ahc_aic7870_setup (pci, probe_config)) == 0)
		error = ahc_aha394XX_setup (pci, probe_config);

	return (error);

}	/* end ahc_aha394X_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 398x		*
 ****************************************************************
 */
int
ahc_aha398X_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	if ((error = ahc_aic7870_setup (pci, probe_config)) == 0)
		error = ahc_aha398XX_setup (pci, probe_config);

	return (error);

}	/* end ahc_aha398X_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 494x		*
 ****************************************************************
 */
int
ahc_aha494X_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	if ((error = ahc_aic7870_setup (pci, probe_config)) == 0)
		error = ahc_aha494XX_setup (pci, probe_config);

	return (error);

}	/* end ahc_aha494X_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7880			*
 ****************************************************************
 */
int
ahc_aic7880_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7880;
	probe_config->features	 = AHC_AIC7880_FE;
	probe_config->bugs	|= AHC_TMODE_WIDEODD_BUG;

	if (pci->pci_revid >= 1)
		probe_config->bugs |= AHC_PCI_2_1_RETRY_BUG;
	else
		probe_config->bugs |= AHC_CACHETHEN_BUG | AHC_PCI_MWI_BUG;

	return (0);

}	/* end ahc_aic7880_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 2940 Pro	*
 ****************************************************************
 */
int
ahc_2940Pro_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	probe_config->flags |= AHC_INT50_SPEEDFLEX;
	error = ahc_aic7880_setup (pci, probe_config);

	return (error);

}	/* end ahc_2940Pro_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 394xU		*
 ****************************************************************
 */
int
ahc_aha394XU_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	if ((error = ahc_aic7880_setup (pci, probe_config)) == 0)
		error = ahc_aha394XX_setup (pci, probe_config);

	return (error);

}	/* end ahc_aha394XU_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 398xU		*
 ****************************************************************
 */
int
ahc_aha398XU_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	int		error;

	if ((error = ahc_aic7880_setup (pci, probe_config)) == 0)
		error = ahc_aha398XX_setup (pci, probe_config);

	return (error);

}	/* end ahc_aha398XU_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7890			*
 ****************************************************************
 */
int
ahc_aic7890_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7890;
	probe_config->features	 = AHC_AIC7890_FE;
	probe_config->flags	|= AHC_NEWEEPROM_FMT;

	if (pci->pci_revid == 0)
		probe_config->bugs |= AHC_AUTOFLUSH_BUG | AHC_CACHETHEN_BUG;

	return (0);

}	/* end ahc_aic7890_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7892			*
 ****************************************************************
 */
int
ahc_aic7892_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = 'A';
	probe_config->chip	 = AHC_AIC7892;
	probe_config->features	 = AHC_AIC7892_FE;
	probe_config->flags	|= AHC_NEWEEPROM_FMT;
	probe_config->bugs	|= AHC_SCBCHAN_UPLOAD_BUG;

	return (0);

}	/* end ahc_aic7892_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7895			*
 ****************************************************************
 */
int
ahc_aic7895_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel = pci->pci_func == 1 ? 'B' : 'A';

	/*
	 *	The 'C' revision of the aic7895 has a few additional features
	 */
	if (pci->pci_revid >= 4)
	{
		probe_config->chip = AHC_AIC7895C;
		probe_config->features = AHC_AIC7895C_FE;
	}
	else
	{
		uint	command;

		probe_config->chip = AHC_AIC7895;
		probe_config->features = AHC_AIC7895_FE;

		/*
		 *	The BIOS disables the use of MWI transactions since it
		 *	does not have the MWI bug work around we have.  Disabling
		 *	MWI reduces performance, so turn it on again
		 */
		command = pci_read (pci, PCIR_COMMAND, /* bytes */ 1);
		command |= PCIM_CMD_MWRICEN;
		pci_write (pci, PCIR_COMMAND, command, /* bytes */ 1);
		probe_config->bugs |= AHC_PCI_MWI_BUG;
	}

	/*
	 *	XXX Does CACHETHEN really not work???  What about PCI retry? on C
	 *	level chips.  Need to test, but for now, play it safe
	 */
	probe_config->bugs |= AHC_TMODE_WIDEODD_BUG | AHC_PCI_2_1_RETRY_BUG | AHC_CACHETHEN_BUG;
	probe_config->flags |= AHC_NEWEEPROM_FMT;

	return (0);

}	/* end ahc_aic7895_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7896			*
 ****************************************************************
 */
int
ahc_aic7896_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = pci->pci_func == 1 ? 'B' : 'A';
	probe_config->chip	 = AHC_AIC7896;
	probe_config->features	 = AHC_AIC7896_FE;
	probe_config->flags	|= AHC_NEWEEPROM_FMT;
	probe_config->bugs	|= AHC_CACHETHEN_DIS_BUG;

	return (0);

}	/* end ahc_aic7896_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "chip" 7899			*
 ****************************************************************
 */
int
ahc_aic7899_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	probe_config->channel	 = pci->pci_func == 1 ? 'B' : 'A';
	probe_config->chip	 = AHC_AIC7899;
	probe_config->features	 = AHC_AIC7899_FE;
	probe_config->flags	|= AHC_NEWEEPROM_FMT;
	probe_config->bugs	|= AHC_SCBCHAN_UPLOAD_BUG;

	return (0);

}	/* end ahc_aic7899_setup */

/*
 ****************************************************************
 *	Configuração inicial para o "raid"			*
 ****************************************************************
 */
int
ahc_raid_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	printf ("RAID functionality unsupported\n");
	return (-1);

}	/* end ahc_raid_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 394xx		*
 ****************************************************************
 */
int
ahc_aha394XX_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	switch (pci->pci_slot)
	{
	    case AHC_394X_SLOT_CHANNEL_A:
		probe_config->channel = 'A';
		break;

	    case AHC_394X_SLOT_CHANNEL_B:
		probe_config->channel = 'B';
		break;

	    default:
		printf
		(	"Adapter at unexpected slot %d unable to map to a channel\n",
			pci->pci_slot
		);
		probe_config->channel = 'A';
	}

	return (0);

}	/* end ahc_aha394XX_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 398xx		*
 ****************************************************************
 */
int
ahc_aha398XX_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	switch (pci->pci_slot)
	{
	    case AHC_398X_SLOT_CHANNEL_A:
		probe_config->channel = 'A';
		break;

	    case AHC_398X_SLOT_CHANNEL_B:
		probe_config->channel = 'B';
		break;

	    case AHC_398X_SLOT_CHANNEL_C:
		probe_config->channel = 'C';
		break;

	    default:
		printf
		(	"Adapter at unexpected slot %d unable to map to a channel\n",
			pci->pci_slot
		);
		probe_config->channel = 'A';
		break;
	}

	probe_config->flags |= AHC_LARGE_SEEPROM;

	return (0);

}	/* end ahc_aha398XX_setup */

/*
 ****************************************************************
 *	Configuração inicial para o controlador 494xx		*
 ****************************************************************
 */
int
ahc_aha494XX_setup (const PCIDATA *pci, struct ahc_probe_config *probe_config)
{
	switch (pci->pci_slot)
	{
	    case AHC_494X_SLOT_CHANNEL_A:
		probe_config->channel = 'A';
		break;

	    case AHC_494X_SLOT_CHANNEL_B:
		probe_config->channel = 'B';
		break;

	    case AHC_494X_SLOT_CHANNEL_C:
		probe_config->channel = 'C';
		break;

	    case AHC_494X_SLOT_CHANNEL_D:
		probe_config->channel = 'D';
		break;

	    default:
		printf
		(	"Adapter at unexpected slot %d unable to map to a channel\n",
			pci->pci_slot
		);
		probe_config->channel = 'A';
	}

	probe_config->flags |= AHC_LARGE_SEEPROM;

	return (0);

}	/* end ahc_aha494XX_setup */

/*
 ****************************************************************
 *	Inicializa o controlador				*
 ****************************************************************
 */
int
ahc_reset (struct ahc_softc *ahc)
{
	uint		sblkctl, sxfrctl1_a, sxfrctl1_b;
	int		wait;

	/*
	 *	Reset the controller and record some information about it
	 *	that is only availabel just after a reset
	 *
	 *	Preserve the value of the SXFRCTL1 register for all channels. It
	 *	contains settings that affect termination and we don't want to
	 *	disturb the integrity of the bus
	 */
	pause_sequencer (ahc);

	sxfrctl1_b = 0;

	sxfrctl1_a = ahc_inb (ahc, SXFRCTL1);

	ahc_outb (ahc, HCNTRL, CHIPRST | ahc->pause);

	/*
	 *	Ensure that the reset has finished
	 */
	wait = 1000;

	do
	{
		ahc_delay (1000);
	}
	while (--wait && !(ahc_inb (ahc, HCNTRL) & CHIPRSTACK));

	if (wait == 0)
	{
		printf ("%s: WARNING - Failed chip reset! Trying to initialize anyway\n", ahc->name);
		ahc_outb (ahc, HCNTRL, ahc->pause);
	}

	/* Determine channel configuration */

	sblkctl = ahc_inb (ahc, SBLKCTL) & (SELBUSB | SELWIDE);

	/* No Twin Channel PCI cards */

	if ((ahc->chip & AHC_PCI) != 0)
		sblkctl &= ~SELBUSB;

	switch (sblkctl)
	{
	    case 0:
		/* Single Narrow Channel */
		break;

	    case 2:
		/* Wide Channel */
		ahc->features |= AHC_WIDE;
		break;

	    default:
		printf (" Unsupported adapter type.  Ignoring\n");
		return (-1);
	}

	/*
	 *	Reload sxfrctl1
	 *
	 *	We must always initialize STPWEN to 1 before we restore the saved
	 *	values.  STPWEN is initialized to a tri-state condition which can
	 *	only be cleared by turning it on
	 */
	ahc_outb (ahc, SXFRCTL1, sxfrctl1_a);

	return (0);

}	/* end ahc_reset */

/*
 ****************************************************************
 *   Determine the number of SCBs available on the controller	*
 ****************************************************************
 */
int
ahc_probe_scbs (struct ahc_softc *ahc)
{
	int		i;

	for (i = 0; i < AHC_SCB_MAX; i++)
	{
		ahc_outb (ahc, SCBPTR, i);
		ahc_outb (ahc, SCB_BASE, i);

		if (ahc_inb (ahc, SCB_BASE) != i)
			break;

		ahc_outb (ahc, SCBPTR, 0);

		if (ahc_inb (ahc, SCB_BASE) != 0)
			break;
	}

	return (i);

}	/* end ahc_probe_scbs */

/*
 ****************************************************************
 *	Inicializa a parte de SCBs				*
 ****************************************************************
 */
int
ahc_init_scbdata (struct ahc_softc *ahc)
{
	struct ahc_scb		*scbp;
	struct hardware_scb	*hscb;
	int			i;
	void			*p;

	/*
	 *	Aloca os SCBs de software
	 *
	 *	Cuidado: O endereço do campo "sg_list" deve ser múltiplo de 8
	 */
	if (sizeof (struct ahc_scb) & 7)
	{
		printf ("%s: O tamanho da estrutura \"ahc_scb\" não é múltiplo de 8\n", ahc->name);
		return (-1);
	}

	if (offsetof (struct ahc_scb, sg_list) & 7)
	{
		printf ("%s: O deslocamento do membro \"sg_list\" não é múltiplo de 8\n", ahc->name);
		return (-1);
	}

	if ((p = malloc_byte (AHC_SCB_MAX * sizeof (struct ahc_scb) + 4)) == NULL)
	{
		printf ("%s: Não obtive memória\n", ahc->name);
		return (-1);
	}

	if ((unsigned)p & 7)
		p += 4;

	ahc->scbarray = p;

	memclr (ahc->scbarray, AHC_SCB_MAX * sizeof (struct ahc_scb));

	/*
	 *	Determine the number of hardware SCBs and initialize them
	 */
	ahc->maxhscbs = ahc_probe_scbs (ahc);

	if ((ahc->flags & AHC_PAGESCBS) != 0)
		ahc_outb (ahc, FREE_SCBH, 0);		/* SCB 0 heads the free list */
	else
		ahc_outb (ahc, FREE_SCBH, SCB_LIST_NULL);

	for (i = 0; i < ahc->maxhscbs; i++)
	{
		ahc_outb (ahc, SCBPTR, i);

		/* Clear the control byte. */

		ahc_outb (ahc, SCB_CONTROL, 0);

		/* Set the next pointer */

		if ((ahc->flags & AHC_PAGESCBS) != 0)
			ahc_outb (ahc, SCB_NEXT, i + 1);
		else
			ahc_outb (ahc, SCB_NEXT, SCB_LIST_NULL);

		/* Make the tag number invalid */

		ahc_outb (ahc, SCB_TAG, SCB_LIST_NULL);
	}

	/* Make sure that the last SCB terminates the free list */

	ahc_outb (ahc, SCBPTR, i - 1);
	ahc_outb (ahc, SCB_NEXT, SCB_LIST_NULL);

	/* Ensure we clear the 0 SCB's control byte. */

	ahc_outb (ahc, SCBPTR, 0);
	ahc_outb (ahc, SCB_CONTROL, 0);

	ahc->maxhscbs = i;

	if (ahc->maxhscbs == 0)
		panic ("%s: No SCB space found", ahc->name);

	/*
	 *	Allocation for our hardware SCBs
	 */
	if ((ahc->hscbs = malloc_byte (AHC_SCB_MAX * sizeof (struct hardware_scb))) == NOVOID)
		return (-1);

	memclr (ahc->hscbs, AHC_SCB_MAX * sizeof (struct hardware_scb));

	/*
	 *	Inicializa o vetor de SCBs de software
	 */
	scbp = &ahc->scbarray[0];
	hscb = &ahc->hscbs[0];
   /***	ahc->free_scbs = NULL; ***/

	for (i = 0; i < AHC_SCB_MAX; i++)
	{
		scbp->ahc_softc	= ahc;
		scbp->flags	= SCB_FREE;

		scbp->hscb	= hscb;
		scbp->hscb->tag	= i;

		scbp->free_next = ahc->free_scbs; ahc->free_scbs = scbp;

		scbp++;
		hscb++;
	}

	ahc->numscbs = AHC_SCB_MAX;

	/*
	 *	Tell the sequencer which SCB will be the next one it receives
	 */
	ahc->next_queued_scb = ahc_get_scb (ahc);
	ahc_outb (ahc, NEXT_QUEUED_SCB, ahc->next_queued_scb->hscb->tag);

	/*
	 *	Note that we were successfull
	 */
	return (0);

}	/* end ahc_init_scbdata */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_loadseq (struct ahc_softc *ahc)
{
	struct cs	cs_table[num_critical_sections];
	uint	begin_set[num_critical_sections];
	uint	end_set[num_critical_sections];
	struct patch	*cur_patch;
	uint	cs_count;
	uint	cur_cs;
	uint	i;
	int 		downloaded;
	uint	skip_addr;
	uint	sg_prefetch_cnt;
	uchar		download_consts[7];

	/*
	 * Start out with 0 critical sections that apply to this firmware load.
	 */
	cs_count = 0;
	cur_cs = 0;
	memclr (begin_set, sizeof (begin_set));
	memclr (end_set, sizeof (end_set));

	/* Setup downloadable constant table */

	download_consts[QOUTFIFO_OFFSET] = 0;

	if (ahc->targetcmds != NULL)
		download_consts[QOUTFIFO_OFFSET] += 32;

	download_consts[QINFIFO_OFFSET] = download_consts[QOUTFIFO_OFFSET] + 1;
	download_consts[CACHESIZE_MASK] = ahc->pci_cachesize - 1;
	download_consts[INVERTED_CACHESIZE_MASK] = ~(ahc->pci_cachesize - 1);
	sg_prefetch_cnt = ahc->pci_cachesize;

	if (sg_prefetch_cnt < (2 * sizeof (struct ahc_dma_seg)))
		sg_prefetch_cnt = 2 * sizeof (struct ahc_dma_seg);

	download_consts[SG_PREFETCH_CNT] = sg_prefetch_cnt;
	download_consts[SG_PREFETCH_ALIGN_MASK] = ~(sg_prefetch_cnt - 1);
	download_consts[SG_PREFETCH_ADDR_MASK] = (sg_prefetch_cnt - 1);

	cur_patch = patches;
	downloaded = 0;
	skip_addr = 0;
	ahc_outb (ahc, SEQCTL, PERRORDIS | FAILDIS | FASTMODE | LOADRAM);
	ahc_outb (ahc, SEQADDR0, 0);
	ahc_outb (ahc, SEQADDR1, 0);

	for (i = 0; i < sizeof (seqprog) / 4; i++)
	{
		if (ahc_check_patch (ahc, &cur_patch, i, &skip_addr) == 0)
		{
			/*
			 * Don't download this instruction as it is in a patch that was removed.
			 */
			continue;
		}

		/*
		 * Move through the CS table until we find a CS that might
		 * apply to this instruction.
		 */
		for (/* vazio */; cur_cs < num_critical_sections; cur_cs++)
		{
			if (critical_sections[cur_cs].end <= i)
			{
				if (begin_set[cs_count] == TRUE && end_set[cs_count] == FALSE)
				{
					cs_table[cs_count].end = downloaded;
					end_set[cs_count] = TRUE;
					cs_count++;
				}
				continue;
			}

			if (critical_sections[cur_cs].begin <= i && begin_set[cs_count] == FALSE)
			{
				cs_table[cs_count].begin = downloaded;
				begin_set[cs_count] = TRUE;
			}

			break;
		}

		ahc_download_instr (ahc, i, download_consts);
		downloaded++;
	}

	ahc->num_critical_sections = cs_count;

	if (cs_count != 0)
	{

		cs_count *= sizeof (struct cs);

		if ((ahc->critical_sections = malloc_byte (cs_count)) == NULL)
			panic ("%s: Memória esgotada para seções críticas", ahc->name);

		memmove (ahc->critical_sections, cs_table, cs_count);
	}

	ahc_outb (ahc, SEQCTL, PERRORDIS | FAILDIS | FASTMODE);

	restart_sequencer (ahc);

#ifndef	BOOT
	if (scb.y_boot_verbose)
		printf (" %d instructions downloaded\n", downloaded);
#endif	BOOT

}	/* end ahc_loadseq */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
ahc_check_patch
(	struct ahc_softc *ahc, struct patch **start_patch,
	uint start_instr, uint *skip_addr
)
{
	struct patch	*cur_patch;
	struct patch	*last_patch;
	uint	num_patches;

	num_patches = sizeof (patches) / sizeof (struct patch);
	last_patch = &patches[num_patches];
	cur_patch = *start_patch;

	while (cur_patch < last_patch && start_instr == cur_patch->begin)
	{

		if (cur_patch->patch_func (ahc) == 0)
		{
			/* Start rejecting code */

			*skip_addr = start_instr + cur_patch->skip_instr;
			cur_patch += cur_patch->skip_patch;
		}
		else
		{
			/*
			 * Accepted this patch.  Advance to the next one and
			 * wait for our intruction pointer to hit this point.
			 */
			cur_patch++;
		}
	}

	*start_patch = cur_patch;

	if (start_instr < *skip_addr) 		/* Still skipping */
		return (0);

	return (1);

}	/* end ahc_check_patch */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_download_instr (struct ahc_softc *ahc, uint instrptr, uchar * dconsts)
{
	union ins_formats instr;
	struct ins_format1 *fmt1_ins;
	struct ins_format3 *fmt3_ins;
	uint           opcode;

	/* Structure copy */

	instr = *(union ins_formats *) & seqprog[instrptr * 4];

	fmt1_ins = &instr.format1;
	fmt3_ins = NULL;

	/* Pull the opcode */
	opcode = instr.format1.opcode;
	switch (opcode)
	{
	    case AIC_OP_JMP:
	    case AIC_OP_JC:
	    case AIC_OP_JNC:
	    case AIC_OP_CALL:
	    case AIC_OP_JNE:
	    case AIC_OP_JNZ:
	    case AIC_OP_JE:
	    case AIC_OP_JZ:
		{
			struct patch   *cur_patch;
			int             address_offset;
			uint           address;
			uint           skip_addr;
			uint           i;

			fmt3_ins = &instr.format3;
			address_offset = 0;
			address = fmt3_ins->address;
			cur_patch = patches;
			skip_addr = 0;

			for (i = 0; i < address;)
			{

				ahc_check_patch (ahc, &cur_patch, i, &skip_addr);

				if (skip_addr > i)
				{
					int             end_addr;

					end_addr = MIN (address, skip_addr);
					address_offset += end_addr - i;
					i = skip_addr;
				}
				else
				{
					i++;
				}
			}
			address -= address_offset;
			fmt3_ins->address = address;
			/* FALLTHROUGH */
		}
	    case AIC_OP_OR:
	    case AIC_OP_AND:
	    case AIC_OP_XOR:
	    case AIC_OP_ADD:
	    case AIC_OP_ADC:
	    case AIC_OP_BMOV:
		if (fmt1_ins->parity != 0)
		{
			fmt1_ins->immediate = dconsts[fmt1_ins->immediate];
		}
		fmt1_ins->parity = 0;
		if ((ahc->features & AHC_CMD_CHAN) == 0
		    && opcode == AIC_OP_BMOV)
		{
			/*
			 * Block move was added at the same time as the
			 * command channel.  Verify that this is only a move
			 * of a single element and convert the BMOV to a MOV
			 * (AND with an immediate of FF).
			 */
			if (fmt1_ins->immediate != 1)
				panic ("%s: BMOV ainda não supportado", ahc->name);

			fmt1_ins->opcode = AIC_OP_AND;
			fmt1_ins->immediate = 0xFF;
		}
		/* FALLTHROUGH */
	    case AIC_OP_ROL:
		if ((ahc->features & AHC_ULTRA2) != 0)
		{
			int             i, count;

			/* Calculate odd parity for the instruction */
			for (i = 0, count = 0; i < 31; i++)
			{
				ulong        mask;

				mask = 0x01 << i;
				if ((instr.integer & mask) != 0)
					count++;
			}
			if ((count & 0x01) == 0)
				instr.format1.parity = 1;
		}
		else
		{
			/* Compress the instruction for older sequencers */
			if (fmt3_ins != NULL)
			{
				instr.integer =
					fmt3_ins->immediate
					| (fmt3_ins->source << 8)
					| (fmt3_ins->address << 16)
					| (fmt3_ins->opcode << 25);
			}
			else
			{
				instr.integer =
					fmt1_ins->immediate
					| (fmt1_ins->source << 8)
					| (fmt1_ins->destination << 16)
					| (fmt1_ins->ret << 24)
					| (fmt1_ins->opcode << 25);
			}
		}

		ahc_outsb (ahc, SEQRAM, instr.bytes, 4);
		break;

	    default:
		panic ("%s: Código inválido no programa do sequenciador", ahc->name);
		break;
	}

}	/* end ahc_download_instr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
ahc_clear_critical_section (struct ahc_softc *ahc)
{
	int		stepping;
	uint		simode0, simode1;

	if (ahc->num_critical_sections == 0)
		return;

	stepping = FALSE;
	simode0 = 0;
	simode1 = 0;

	for (EVER)
	{
		struct cs      *cs;
		uint           seqaddr;
		uint           i;

		seqaddr = ahc_inb (ahc, SEQADDR0) | (ahc_inb (ahc, SEQADDR1) << 8);

		cs = ahc->critical_sections;

		for (i = 0; i < ahc->num_critical_sections; i++, cs++)
		{
			if (cs->begin < seqaddr && cs->end >= seqaddr)
				break;
		}

		if (i == ahc->num_critical_sections)
			break;

		if (stepping == FALSE)
		{

			/*
			 * Disable all interrupt sources so that the
			 * sequencer will not be stuck by a pausing interrupt
			 * condition while we attempt to leave a critical
			 * section.
			 */
			simode0 = ahc_inb (ahc, SIMODE0);
			ahc_outb (ahc, SIMODE0, 0);
			simode1 = ahc_inb (ahc, SIMODE1);
			ahc_outb (ahc, SIMODE1, 0);
			ahc_outb (ahc, CLRINT, CLRSCSIINT);
			ahc_outb (ahc, SEQCTL, ahc_inb (ahc, SEQCTL) | STEP);
			stepping = TRUE;
		}

		ahc_outb (ahc, HCNTRL, ahc->unpause);

		/* Espera entrar em pausa */
 
		do
		{
			ahc_delay (200);

		}	while ((ahc_inb (ahc, HCNTRL) & PAUSE) == 0);

	}

	if (stepping)
	{
		ahc_outb (ahc, SIMODE0, simode0);
		ahc_outb (ahc, SIMODE1, simode1);
		ahc_outb (ahc, SEQCTL, ahc_inb (ahc, SEQCTL) & ~STEP);
	}

}	/* end ahc_clear_critical_section */

#if (0)	/*******************************************************/
int		ahc_get_pci_base_addr (PCIDATA *pci);
/*
 ****************************************************************
 *	Obtém, do PCI, o "base_addr"				*
 ****************************************************************
 */
int
ahc_get_pci_base_addr (PCIDATA *pci)
{
	ulong	pos, reg, addr, sz;

	for (pos = 0; pos < 6; pos++)
	{
		reg = PCIR_MAPS + (pos << 2);

		addr = pci_read (pci, reg, 4);
		pci_write (pci, reg, ~0, 4);

		sz = pci_read (pci, reg, 4);
		pci_write (pci, reg, addr, 4);

		if (sz == 0 || sz == 0xFFFFFFFF)
			continue;

		if (addr == 0xFFFFFFFF)
			addr = 0;

		if ((addr & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY)
		{
panic ("%g: Mapeado em memória?");
#if (0)	/*******************************************************/
			/* Mapeado em memória */

			if ((addr & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64)
			{
				/* Endereçamento de 64 bits: lê a parte alta do endereço */

				pos++;

				if (pci_read (pci, reg + 4, 4) != 0)
				{
					printf ("%g: Endereçamento de 64 bits NÃO SUPORTADO\n");
					continue;
				}

				/* A parte alta é 0, usa a parte baixa apenas */
			}

			if ((sz = ohci_pci_size (addr, sz, PCI_BASE_ADDRESS_MEM_MASK)) == 0)
				continue;

			addr &= PCI_BASE_ADDRESS_MEM_MASK;

			/* Mapeia os endereços */

			if ((sc->sc_bus.base_addr = mmu_map_phys_addr (addr, sz + 1)) == 0)
				printf ("ohci_attach: Não consegui mapear os registros de usb%d\n", usb_unit);

			return;
#endif	/*******************************************************/
		}
		else
		{
			/* Mapeado em portas */

			if ((sz = ohci_pci_size (addr, sz, PCI_BASE_ADDRESS_IO_MASK & 0xFFFF)) == 0)
				continue;

			return (addr & PCI_BASE_ADDRESS_IO_MASK);
		}
	}

panic ("%g: Não achei a porta");
return (0);

}	/* end aic_get_pci_base_addr */
#endif	/*******************************************************/
