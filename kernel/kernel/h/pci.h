/*
 ****************************************************************
 *								*
 *			<sys/pci.h>				*
 *								*
 *	Definições acerca do barramento PCI			*
 *								*
 *	Versão	3.0.0, de 13.04.96				*
 *		4.9.0, de 26.03.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	PCI_H

/* some PCI bus constants */

#define PCI_BUSMAX		255
#define PCI_SLOTMAX		31
#define PCI_FUNCMAX		7
#define PCI_REGMAX		255

/* PCI config header registers for all devices */

#define PCIR_DEVVENDOR		0x00
#define PCIR_VENDOR		0x00
#define PCIR_DEVICE		0x02
#define PCIR_COMMAND		0x04
#define PCIR_STATUS		0x06
#define PCIR_REVID		0x08
#define PCIR_PROGIF		0x09
#define PCIR_SUBCLASS		0x0A
#define PCIR_CLASS		0x0B
#define PCIR_CACHELNSZ		0x0C
#define PCIR_LATTIMER		0x0D
#define PCIR_HEADERTYPE		0x0E
#define PCIM_MFDEV		0x80
#define PCIR_BIST		0x0F

#define PCIM_CMD_PORTEN		0x0001
#define PCIM_CMD_MEMEN		0x0002
#define PCIM_CMD_BUSMASTEREN	0x0004
#define PCIM_CMD_MWRICEN	0x0010
#define PCIM_CMD_PERRESPEN	0x0040

#define PCI_BASE_ADDRESS_SPACE		0x01	/* 0 = memory, 1 = I/O */
#define PCI_BASE_ADDRESS_SPACE_IO	0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY	0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK	0x06
#define PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define PCI_BASE_ADDRESS_MEM_MASK	(~0x0F)
#define PCI_BASE_ADDRESS_IO_MASK	(~0x03)

#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define PCI_ROM_ADDRESS_ENABLE	0x01
#define PCI_ROM_ADDRESS_MASK	(~0x7FF)

/* config registers for header type 0 devices */

#define PCIR_MAPS		0x10
#define PCIR_CARDBUSCIS		0x28
#define PCIR_SUBVEND_0		0x2C
#define PCIR_SUBDEV_0		0x2E
#define PCIR_BIOS		0x30
#define PCIM_BIOS_ENABLE	0x01
#define PCIR_INTLINE		0x3C
#define PCIR_INTPIN		0x3E
#define PCIR_MINGNT		0x3E
#define PCIR_MAXLAT		0x3F

#define PCIR_BARS		0x10
#define PCIR_BAR(x)		(PCIR_BARS + (x) * 4)

#define	IOMASK			0xFFFFFFFC

/* config registers for header type 1 devices */

#define PCIR_SECSTAT_1		0

#define PCIR_PRIBUS_1		0x18
#define PCIR_SECBUS_1		0x19
#define PCIR_SUBBUS_1		0x1A
#define PCIR_SECLAT_1		0x1B

#define PCIR_IOBASEL_1		0x1C
#define PCIR_IOLIMITL_1		0x1D
#define PCIR_IOBASEH_1		0
#define PCIR_IOLIMITH_1		0

#define PCIR_MEMBASE_1		0x20
#define PCIR_MEMLIMIT_1		0x22

#define PCIR_PMBASEL_1		0x24
#define PCIR_PMLIMITL_1		0x26
#define PCIR_PMBASEH_1		0
#define PCIR_PMLIMITH_1		0

#define PCIR_BRIDGECTL_1	0

#define PCIR_SUBVEND_1		0x34
#define PCIR_SUBDEV_1		0x36

/* config registers for header type 2 devices */

#define PCIR_SECSTAT_2		0x16

#define PCIR_PRIBUS_2		0x18
#define PCIR_SECBUS_2		0x19
#define PCIR_SUBBUS_2		0x1A
#define PCIR_SECLAT_2		0x1B

#define PCIR_MEMBASE0_2		0x1C
#define PCIR_MEMLIMIT0_2	0x20
#define PCIR_MEMBASE1_2		0x24
#define PCIR_MEMLIMIT1_2	0x28
#define PCIR_IOBASE0_2		0x2C
#define PCIR_IOLIMIT0_2		0x30
#define PCIR_IOBASE1_2		0x34
#define PCIR_IOLIMIT1_2		0x38

#define PCIR_BRIDGECTL_2	0x3E

#define PCIR_SUBVEND_2		0x40
#define PCIR_SUBDEV_2		0x42

#define PCIR_PCCARDIF_2		0x44

/*
 ****************************************************************
 *	Classes, SubClasses e ProgIfs				*
 ****************************************************************
 *
 *	PCI device class, subclass and programming interface definitions
 */
#define PCIC_OLD			0x00
#define PCIS_OLD_NONVGA			0x00
#define PCIS_OLD_VGA			0x01

#define PCIC_STORAGE			0x01
#define PCIS_STORAGE_SCSI		0x00
#define PCIS_STORAGE_IDE		0x01
#define PCIP_STORAGE_IDE_MODEPRIM	0x01
#define PCIP_STORAGE_IDE_PROGINDPRIM	0x02
#define PCIP_STORAGE_IDE_MODESEC	0x04
#define PCIP_STORAGE_IDE_PROGINDSEC	0x08
#define PCIP_STORAGE_IDE_MASTERDEV	0x80
#define PCIS_STORAGE_FLOPPY		0x02
#define PCIS_STORAGE_IPI		0x03
#define PCIS_STORAGE_RAID		0x04
#define PCIS_STORAGE_OTHER		0x80

#define PCIC_NETWORK			0x02
#define PCIS_NETWORK_ETHERNET		0x00
#define PCIS_NETWORK_TOKENRING		0x01
#define PCIS_NETWORK_FDDI		0x02
#define PCIS_NETWORK_ATM		0x03
#define PCIS_NETWORK_OTHER		0x80

#define PCIC_DISPLAY			0x03
#define PCIS_DISPLAY_VGA		0x00
#define PCIS_DISPLAY_XGA		0x01
#define PCIS_DISPLAY_OTHER		0x80

#define PCIC_MULTIMEDIA			0x04
#define PCIS_MULTIMEDIA_VIDEO		0x00
#define PCIS_MULTIMEDIA_AUDIO		0x01
#define PCIS_MULTIMEDIA_OTHER		0x80

#define PCIC_MEMORY			0x05
#define PCIS_MEMORY_RAM			0x00
#define PCIS_MEMORY_FLASH		0x01
#define PCIS_MEMORY_OTHER		0x80

#define PCIC_BRIDGE			0x06
#define PCIS_BRIDGE_HOST		0x00
#define PCIS_BRIDGE_ISA			0x01
#define PCIS_BRIDGE_EISA		0x02
#define PCIS_BRIDGE_MCA			0x03
#define PCIS_BRIDGE_PCI			0x04
#define PCIS_BRIDGE_PCMCIA		0x05
#define PCIS_BRIDGE_NUBUS		0x06
#define PCIS_BRIDGE_CARDBUS		0x07
#define PCIS_BRIDGE_OTHER		0x80

#define PCIC_SIMPLECOMM			0x07
#define PCIS_SIMPLECOMM_UART		0x00
#define PCIP_SIMPLECOMM_UART_16550A	0x02
#define PCIS_SIMPLECOMM_PAR		0x01
#define PCIS_SIMPLECOMM_OTHER		0x80

#define PCIC_BASEPERIPH			0x08
#define PCIS_BASEPERIPH_PIC		0x00
#define PCIS_BASEPERIPH_DMA		0x01
#define PCIS_BASEPERIPH_TIMER		0x02
#define PCIS_BASEPERIPH_RTC		0x03
#define PCIS_BASEPERIPH_OTHER		0x80

#define PCIC_INPUTDEV			0x09
#define PCIS_INPUTDEV_KEYBOARD		0x00
#define PCIS_INPUTDEV_DIGITIZER		0x01
#define PCIS_INPUTDEV_MOUSE		0x02
#define PCIS_INPUTDEV_OTHER		0x80

#define PCIC_DOCKING			0x0A
#define PCIS_DOCKING_GENERIC		0x00
#define PCIS_DOCKING_OTHER		0x80

#define PCIC_PROCESSOR			0x0B
#define PCIS_PROCESSOR_386		0x00
#define PCIS_PROCESSOR_486		0x01
#define PCIS_PROCESSOR_PENTIUM		0x02
#define PCIS_PROCESSOR_ALPHA		0x10
#define PCIS_PROCESSOR_POWERPC		0x20
#define PCIS_PROCESSOR_COPROC		0x40

#define PCIC_SERIALBUS			0x0C
#define PCIS_SERIALBUS_FW		0x00
#define PCIS_SERIALBUS_ACCESS		0x01
#define PCIS_SERIALBUS_SSA		0x02
#define PCIS_SERIALBUS_USB		0x03
#define PCIS_SERIALBUS_FC		0x04
#define PCIS_SERIALBUS
#define PCIS_SERIALBUS

#define PCIC_OTHER			0xFF

/*
 ****************************************************************
 *	Compatibilidade com versões antigas			*
 ****************************************************************
 */
#define PCI_ID_REG			0x00
#define PCI_COMMAND_STATUS_REG		0x04
#define	PCI_COMMAND_IO_ENABLE		0x00000001
#define	PCI_COMMAND_MEM_ENABLE		0x00000002
#define PCI_CLASS_REG			0x08
#define PCI_CLASS_MASK			0xFF000000
#define PCI_SUBCLASS_MASK		0x00FF0000
#define	PCI_REVISION_MASK		0x000000FF
#define PCI_CLASS_PREHISTORIC		0x00000000
#define PCI_SUBCLASS_PREHISTORIC_VGA	0x00010000
#define PCI_CLASS_MASS_STORAGE		0x01000000
#define PCI_CLASS_DISPLAY		0x03000000
#define PCI_SUBCLASS_DISPLAY_VGA	0x00000000
#define PCI_CLASS_BRIDGE		0x06000000
#define PCI_MAP_REG_START		0x10
#define	PCI_MAP_REG_END			0x28
#define	PCI_MAP_IO			0x00000001
#define	PCI_INTERRUPT_REG		0x3C

/*
 ****************************************************************
 *	Dados de um Dispositivo PCI				*
 ****************************************************************
 */
enum { MAX_PCI_ADDR = 6 };

#define	NOPCIDATA	(PCIDATA *)NULL

typedef struct pcidata	PCIDATA;

struct pcidata
{
	const char	*pci_name;		/* Nome do dispositivo */
	PCIDATA		*pci_parent;		/* Ponteiro para o pai */
	void		*pci_hdrspec;

	ushort		pci_vendor;		/* chip vendor ID */
	ushort		pci_device;		/* chip device ID, assigned by chip vendor */

	ushort		pci_subvendor;		/* card vendor ID */
	ushort		pci_subdevice;		/* card device ID, assigned by card vendor */

	ushort		pci_cmdreg;		/* disable/enable chip and PCI options */
	ushort		pci_statreg;		/* supported PCI features and error state */

	uchar		pci_baseclass;		/* chip PCI class */
	uchar		pci_subclass;		/* chip PCI subclass */
	uchar		pci_progif;		/* chip PCI programming interface */
	uchar		pci_revid;		/* chip revision ID */

	uchar		pci_hdrtype;		/* chip config header type */
	uchar		pci_cachelnsz;		/* cache line size in 4byte units */
	uchar		pci_intpin;		/* PCI interrupt pin */
	uchar		pci_intline;		/* interrupt line (IRQ for PC arch) */

	int		pci_addr[MAX_PCI_ADDR];	/* endereços-base */
	int		pci_map_mask;

	uchar		pci_mingnt;		/* min. useful bus grant time in 250ns units */
	uchar		pci_maxlat;		/* max. tolerated bus grant latency in 250ns */
	uchar		pci_lattimer;		/* latency timer in units of 30ns bus cycles */

	uchar		pci_mfdev;		/* multi-function device (from hdrtype reg) */
	uchar		pci_nummaps;		/* actual number of PCI maps used */

	uchar		pci_hose;		/* hose which bus is attached to */
	uchar		pci_bus;		/* config space bus address */
	uchar		pci_slot;		/* config space slot address */
	uchar		pci_func;		/* config space function number */

	uchar		pci_secondarybus;	/* bus on secondary side of bridge, if any */
	uchar		pci_subordinatebus;	/* topmost bus number behind bridge, if any */

	void		*pci_dev_info_ptr;	/* informações adicionais */
};

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
extern int		pci_read (const PCIDATA *, int, int);
extern void		pci_write (const PCIDATA *, int, int, int);
