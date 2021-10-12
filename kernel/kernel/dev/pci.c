/*
 ****************************************************************
 *								*
 *			pci.c					*
 *								*
 *	Funções para controle do barramento PCI			*
 *								*
 *	Versão	4.0.0, de 21.12.00				*
 *		4.5.0, de 27.03.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/pci.h"

#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
/*
 ******	Portas PCI **********************************************
 */
#define CONF1_ADDR_PORT		0x0CF8
#define CONF1_DATA_PORT		0x0CFC

#define CONF1_ENABLE		0x80000000
#define CONF1_ENABLE_CHK	0x80000000
#define CONF1_ENABLE_MSK	0x7FF00000
#define CONF1_ENABLE_CHK1	0xFF000001
#define CONF1_ENABLE_MSK1	0x80000001
#define CONF1_ENABLE_RES1	0x80000000

#define CONF2_ENABLE_PORT	0x0CF8
#define CONF2_FORWARD_PORT	0x0CFA

#define CONF2_ENABLE_CHK	0x0E
#define CONF2_ENABLE_RES	0x0E

#define PCI_MAPMEM		0x01	/* memory map */
#define PCI_MAPMEMP		0x02	/* prefetchable memory map */
#define PCI_MAPPORT		0x04	/* port map */

#define PCI_MAXMAPS_0		6	/* max. no. of memory/port maps */
#define PCI_MAXMAPS_1		2	/* max. no. of maps for PCI to PCI bridge */
#define PCI_MAXMAPS_2		1	/* max. no. of maps for CardBus bridge */

/*
 ******	Lista de dispositivos a procurar ************************
 */
typedef struct pci_device
{
	char	*pd_name;
	char	*(*pd_probe) (PCIDATA *, ulong);
	void	(*pd_attach) (PCIDATA *, int);
	int	*pd_count;
	int	(*pd_shutdown) (void);

}	PCI_DEVICE;

#define	NO_PCI_DEVICE	(PCI_DEVICE *)NULL

char		*chipset_probe (PCIDATA *, ulong);
void		chipset_attach (PCIDATA *, int);
entry int	chipset_count;

entry PCI_DEVICE	chipset_device =
{
	"chip",
	chipset_probe,
   /***	chipset_attach, ***/	NULL,
	&chipset_count,
	NULL
};

char		*isa_probe (PCIDATA *, ulong);
void		isa_attach (PCIDATA *, int);
entry int	isa_count;

entry PCI_DEVICE	isa_device =
{
	"isa",
	isa_probe,
   /***	isa_attach, ***/	NULL,
	&isa_count,
	NULL
};

char		*pcib_probe (PCIDATA *, ulong);
void		pcib_attach (PCIDATA *, int);
entry int	pcib_count;

entry PCI_DEVICE	pcib_device =
{
	"pcib",
	pcib_probe,
   /***	pcib_attach, ***/	NULL,
	&pcib_count,
	NULL
};

char		*vga_probe (PCIDATA *, ulong);
void		vga_attach (PCIDATA *, int);
entry int	vga_count;

entry PCI_DEVICE	vga_device =
{
	"vga",
	vga_probe,
   /***	vga_attach, ***/	NULL,
	&vga_count,
	NULL
};

char		*uhci_probe (PCIDATA *, ulong);
void		uhci_attach (PCIDATA *, int);
entry int	uhci_count;

entry PCI_DEVICE	uhci_device =
{
	"uhci",
	uhci_probe,
	uhci_attach,
	&uhci_count,
	NULL
};

char		*ohci_probe (PCIDATA *, ulong);
void		ohci_attach (PCIDATA *, int);
entry int	ohci_count;

entry PCI_DEVICE	ohci_device =
{
	"ohci",
	ohci_probe,
	ohci_attach,
	&ohci_count,
	NULL
};

char		*ehci_probe (PCIDATA *, ulong);
void		ehci_attach (PCIDATA *, int);
entry int	ehci_count;

entry PCI_DEVICE	ehci_device =
{
	"ehci",
	ehci_probe,
	ehci_attach,
	&ehci_count,
	NULL
};
char		*ahc_pci_probe (PCIDATA *, ulong);
void		ahc_pci_attach (PCIDATA *, int);
entry int	ahc_count;

entry PCI_DEVICE	AHC_device =
{
	"ahc",
	ahc_pci_probe,
	ahc_pci_attach,
	&ahc_count,
	NULL
};

char		*pci_ed_probe (PCIDATA *, ulong);
void		pci_ed_attach (PCIDATA *, int);
entry int	pci_ed_count;

entry PCI_DEVICE	pci_ed_device =
{
	"ed",
	pci_ed_probe,
   	pci_ed_attach,
	&pci_ed_count,
	NULL
};

char		*pci_sio_probe (PCIDATA *, ulong);
void		pci_sio_attach (PCIDATA *, int);
entry int	pci_sio_count;

entry PCI_DEVICE	pci_sio_device =
{
	"sio",
	pci_sio_probe,
   	pci_sio_attach,
	&pci_sio_count,
	NULL
};

char		*pci_rtl_probe (PCIDATA *, ulong);
void		pci_rtl_attach (PCIDATA *, int);
entry int	pci_rtl_count;

entry PCI_DEVICE	pci_rtl_device =
{
	"rtl",
	pci_rtl_probe,
   	pci_rtl_attach,
	&pci_rtl_count,
	NULL
};

char		*pci_ata_probe (PCIDATA *, ulong);
void		pci_ata_attach (PCIDATA *, int);
entry int	pci_ata_count;

entry PCI_DEVICE	pci_ide_device =
{
	"ata",
	pci_ata_probe,
   	pci_ata_attach,
	&pci_ata_count,
	NULL
};

char		*pci_sb_probe (PCIDATA *, ulong);
void		pci_sb_attach (PCIDATA *, int);
entry int	pci_sb_count;

entry PCI_DEVICE	pci_sb_device =
{
	"sb",
	pci_sb_probe,
   	pci_sb_attach,
	&pci_sb_count,
	NULL
};

char		*pci_live_probe (PCIDATA *, ulong);
void		pci_live_attach (PCIDATA *, int);
entry int	pci_live_count;

entry PCI_DEVICE	pci_live_device =
{
	"live",
	pci_live_probe,
   	pci_live_attach,
	&pci_live_count,
	NULL
};

entry PCI_DEVICE	*pcidevice_set[] =
{
	&chipset_device,
	&isa_device,
	&pcib_device,
	&vga_device,
	&uhci_device,
	&ohci_device,
	&ehci_device,
	&AHC_device,
	&pci_ed_device,
	&pci_sio_device,
	&pci_rtl_device,
	&pci_ide_device,
	&pci_sb_device,
	&pci_live_device,
	(PCI_DEVICE *)NULL
};

/*
 ******	Variáveis ***********************************************
 */
entry ulong		pci_numdevs = 0;
entry ulong		pci_generation = 0;

entry int		cfgmech;
entry int		devmax;

/*
 ******	Protótipos de funções ***********************************
 */
void			scan_pci_bus (int);
int			pci_readcfg (PCIDATA *);
int			pci_enable (unsigned, unsigned, unsigned, int, int);
void			pci_disable (void);
ulong			pci_get_size (ulong base, ulong maxbase, ulong mask);
void			pci_get_base_addr (PCIDATA *pci);
void			pci_fixancient (PCIDATA *);
void			pci_hdrtypedata (PCIDATA *);
int			pci_open (void);
void			fixbushigh_i1225 (PCIDATA *pci);
void			fixwsc_natoma (PCIDATA *pci);
const char		*pci_bridge_type (PCIDATA *pci);

/*
 ****************************************************************
 *	Decobre os dispositivos PCI				*
 ****************************************************************
 */
int
pci_init (void)
{
	int		busno;

	if (pci_open () == 0)
	{
		printf ("pci_new_probe: pci_open retornou NULO\n");
		return (-1);
	}

	for (busno = 0; busno < 8; busno++)
		scan_pci_bus (busno);

#if (0)	/*******************************************************/
	printf ("pci_probe: cfgmech = %d, devmax = %d\n", cfgmech, devmax);

	scan_pci_bus (0);
	scan_pci_bus (1);
	scan_pci_bus (2);
#endif	/*******************************************************/

	return (0);

}	/* end pci_our_probe */

/*
 ****************************************************************
 *	Varre um barramento PCI					*
 ****************************************************************
 */
void
scan_pci_bus (int busno)
{
	int			end_func, unit;
	PCIDATA			parent_pcidata, child_pcidata; 
	PCIDATA			*pci = &parent_pcidata;
	const PCI_DEVICE	*dvp;
	PCI_DEVICE	 	**dvpp;
	int			slot, func;

	for (slot = 0; slot <= PCI_SLOTMAX; slot++)
	{
		pci = &parent_pcidata; end_func = 0;

		for (func = 0; func <= end_func; func++)
		{
			if (func > 0)
				pci = &child_pcidata;

			memclr (pci, sizeof (PCIDATA));

			pci->pci_slot = slot;
			pci->pci_func = func;
			pci->pci_hose = 0;
			pci->pci_bus  = busno;

			if (func > 0)
				pci->pci_parent = &parent_pcidata;

			if (pci_readcfg (pci) < 0)
				continue;

			if (func == 0 && pci->pci_mfdev)
				end_func = 7;

			/*
			 *	Testa a existência dos vários dispositivos
			 */
			for (dvpp = pcidevice_set; dvp = *dvpp++; /* vazio */)
			{
				if (dvp->pd_probe)
				{
					ulong type = (pci->pci_device << 16) | pci->pci_vendor;

					if (pci->pci_name = (*dvp->pd_probe) (pci, type))
						break;
				}
			};

			printf ("pci%d[%d,%d]:", pci->pci_bus, pci->pci_slot, pci->pci_func);

			if (pci->pci_name)
			{
				printf
				(	" %s <%s> rev %d\n",
					dvp->pd_name, pci->pci_name, pci->pci_revid
				);

				unit = (*dvp->pd_count)++;

				if (*dvp->pd_attach)
					(*dvp->pd_attach) (pci, unit);
			}
			else
			{
				printf
				(	" Dispositivo NÃO identificado: vendor = %04X, device = %04X\n",
					pci->pci_vendor, pci->pci_device
				);
			}

		}	/* end for (funcs ...) */

	}	/* end for (slots ...) */

}	/* end scan_pci_bus */

/*
 ****************************************************************
 *	Verifica se o mecanismo obtido é o correto		*
 ****************************************************************
 */
int
pci_check (int maxdev)
{
	uchar		device;

	for (device = 0; device < maxdev; device++)
	{
		unsigned	id, class, header;

		id = read_port_long (pci_enable (0, device, 0, 0, 4));

		if (id == 0 || id == -1)
			continue;

		class = read_port_long (pci_enable (0, device, 0, 8, 4)) >> 8;

		if (class == 0 || (class & 0xF870FF) != 0)
			continue;

		header = read_port (pci_enable (0, device, 0, 14, 1));

		if ((header & 0x7E) != 0)
			continue;

		pci_disable ();

		return (1);
	}

	pci_disable ();

	return (0);

}	/* end pci_check */

/*
 ****************************************************************
 *	Descobre o mecanismo					*
 ****************************************************************
 */
int
pci_open (void)
{
	ulong		mode1res, oldval1;
	uchar		mode2res, oldval2;

	/*
	 *	Tenta o mecanismo 1
	 */
	if (((oldval1 = read_port_long (CONF1_ADDR_PORT)) & CONF1_ENABLE_MSK) == 0)
	{
		cfgmech = 1;
		devmax = 32;

		write_port_long (CONF1_ENABLE_CHK, CONF1_ADDR_PORT);
		write_port (0, CONF1_ADDR_PORT +3);
		mode1res = read_port_long (CONF1_ADDR_PORT);
		write_port_long (oldval1, CONF1_ADDR_PORT);

		if (mode1res && pci_check (32)) 
			return (cfgmech);

		write_port_long (CONF1_ENABLE_CHK1, CONF1_ADDR_PORT);
		mode1res = read_port_long (CONF1_ADDR_PORT);
		write_port_long (oldval1, CONF1_ADDR_PORT);

		if ((mode1res & CONF1_ENABLE_MSK1) == CONF1_ENABLE_RES1)
		{
			if (pci_check (32)) 
				return (cfgmech);
		}
	}

	/*
	 *	Tenta o mecanismo 2
	 */
	if (((oldval2 = read_port (CONF2_ENABLE_PORT)) & 0xF0) == 0)
	{
		cfgmech = 2;
		devmax = 16;

		write_port (CONF2_ENABLE_CHK, CONF2_ENABLE_PORT);
		mode2res = read_port (CONF2_ENABLE_PORT);
		write_port (oldval2, CONF2_ENABLE_PORT);

		if (mode2res == CONF2_ENABLE_RES)
		{
			if (pci_check (16)) 
				return (cfgmech);
		}
	}

	cfgmech = 0;
	devmax = 0;

	return (cfgmech);

}	/* end pci_open */

/*
 ****************************************************************
 *	Obtém as características de um dispositivo		*
 ****************************************************************
 */
int
pci_readcfg (PCIDATA *pci)
{
	if (pci_read (pci, PCIR_DEVVENDOR, 4) == -1)
		return (-1);

	pci->pci_vendor		= pci_read (pci, PCIR_VENDOR, 2);
	pci->pci_device		= pci_read (pci, PCIR_DEVICE, 2);
#if (0)	/*******************************************************/
	pci->pci_cmdreg		= pci_read (pci, PCIR_COMMAND, 2);
	pci->pci_statreg	= pci_read (pci, PCIR_STATUS, 2);
#endif	/*******************************************************/
	pci->pci_baseclass	= pci_read (pci, PCIR_CLASS, 1);
	pci->pci_subclass	= pci_read (pci, PCIR_SUBCLASS, 1);
	pci->pci_progif		= pci_read (pci, PCIR_PROGIF, 1);
	pci->pci_revid		= pci_read (pci, PCIR_REVID, 1);
	pci->pci_hdrtype	= pci_read (pci, PCIR_HEADERTYPE, 1);
#if (0)	/*******************************************************/
	pci->pci_cachelnsz	= pci_read (pci, PCIR_CACHELNSZ, 1);
	pci->pci_lattimer	= pci_read (pci, PCIR_LATTIMER, 1);
#endif	/*******************************************************/
	pci->pci_intpin		= pci_read (pci, PCIR_INTPIN, 1);
	pci->pci_intline	= pci_read (pci, PCIR_INTLINE, 1);

	pci_get_base_addr (pci);

#ifdef APIC_IO
	if (pci->pci_intpin != 0)
	{
		int		airq;

		airq = pci_apic_irq (pci->pci_bus, pci->pci_slot, pci->pci_intpin);

		if (airq >= 0)
		{
			/* PCI specific entry found in MP table */

			if (airq != pci->pci_intline)
			{
				undirect_pci_irq (pci->pci_intline);
				pci->pci_intline = airq;
			}
		}
		else
		{
			/*
			 * PCI interrupts might be redirected to the
			 * ISA bus according to some MP tables. Use
			 * the same methods as used by the ISA
			 * devices devices to find the proper IOAPIC
			 * int pin.
			 */
			airq = isa_apic_irq (pci->pci_intline);

			if ((airq >= 0) && (airq != pci->pci_intline))
			{
				/* XXX: undirect_pci_irq () ? */

				undirect_isa_irq (pci->pci_intline);
				pci->pci_intline = airq;
			}
		}
	}
#endif				/* APIC_IO */

#if (0)	/*******************************************************/
	pci->pci_mingnt	= pci_read (pci, PCIR_MINGNT, 1);
	pci->pci_maxlat	= pci_read (pci, PCIR_MAXLAT, 1);
#endif	/*******************************************************/

	pci->pci_mfdev	= (pci->pci_hdrtype & PCIM_MFDEV) != 0;
	pci->pci_hdrtype   &= ~PCIM_MFDEV;

	pci_fixancient (pci);
	pci_hdrtypedata (pci);

	pci_numdevs++;
	pci_generation++;

	return (0);

}	/* end pci_readcfg */

/*
 ****************************************************************
 *	Trata os diferentes cabeçalhos (0, 1 ou 2)		*
 ****************************************************************
 */
void
pci_hdrtypedata (PCIDATA *pci)
{
	switch (pci->pci_hdrtype)
	{
	    case 0:
		pci->pci_subvendor	= pci_read (pci, PCIR_SUBVEND_0, 2);
		pci->pci_subdevice	= pci_read (pci, PCIR_SUBDEV_0, 2);
		pci->pci_nummaps	= PCI_MAXMAPS_0;
		break;

	    case 1:
		pci->pci_subvendor	= pci_read (pci, PCIR_SUBVEND_1, 2);
		pci->pci_subdevice	= pci_read (pci, PCIR_SUBDEV_1, 2);
		pci->pci_secondarybus = pci_read (pci, PCIR_SECBUS_1, 1);
		pci->pci_subordinatebus = pci_read (pci, PCIR_SUBBUS_1, 1);
		pci->pci_nummaps	= PCI_MAXMAPS_1;
#if (0)	/*******************************************************/
		pci->pci_hdrspec	= pci_readppb (pci);
#endif	/*******************************************************/
		break;

	    case 2:
		pci->pci_subvendor	= pci_read (pci, PCIR_SUBVEND_2, 2);
		pci->pci_subdevice	= pci_read (pci, PCIR_SUBDEV_2, 2);
		pci->pci_secondarybus = pci_read (pci, PCIR_SECBUS_2, 1);
		pci->pci_subordinatebus = pci_read (pci, PCIR_SUBBUS_2, 1);
		pci->pci_nummaps	= PCI_MAXMAPS_2;
#if (0)	/*******************************************************/
		pci->pci_hdrspec	= pci_readpcb (pci);
#endif	/*******************************************************/
		break;
	}

}	/* end pci_hdrtypedata */

/*
 ****************************************************************
 *	Obtém a extensão do espaço de registros			*
 ****************************************************************
 */
ulong
pci_get_size (ulong base, ulong maxbase, ulong mask)
{
	ulong		size;

	if ((size = mask & maxbase) == 0)
		return (0);

	size = size & ~(size - 1);
	size--;

	if (base == maxbase && ((base | size) & mask) != mask)
		return (0);

	return (size);

}	/* end pci_get_size */

/*
 ****************************************************************
 *	Obtém, do PCI, o "base_addr"				*
 ****************************************************************
 */
void
pci_get_base_addr (PCIDATA *pci)
{
	ulong	pos, reg, addr, vaddr, sz, port_mapped;

   /***	sc->sc_bus.base_addr = 0; ***/

	for (pos = 0; pos < MAX_PCI_ADDR; pos++)
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
			/* Mapeado em memória */
#ifndef	BOOT
			if ((addr & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64)
			{
				/* Endereçamento de 64 bits: lê a parte alta do endereço */

				pos++;

				if (pci_read (pci, reg + 4, 4) != 0)
				{
					printf ("ohci_attach: Endereçamento de 64 bits NÃO SUPORTADO\n");
					continue;
				}

				/* A parte alta é 0, usa a parte baixa apenas */
			}

			if ((sz = pci_get_size (addr, sz, PCI_BASE_ADDRESS_MEM_MASK)) == 0 || sz >= PGSZ)
				continue;

			addr &= PCI_BASE_ADDRESS_MEM_MASK;

			/* Mapeia os endereços */

			if ((vaddr = mmu_map_phys_addr (addr, sz + 1)) == 0)
			{
				printf ("pci_get_base_addr: Não consegui mapear o endereço físico %P\n", addr);
				continue;
			}
#else
			vaddr = 0;
#endif
			port_mapped = 0;
		}
		else
		{
			/* Mapeado em portas */

			if ((sz = pci_get_size (addr, sz, PCI_BASE_ADDRESS_IO_MASK & 0xFFFF)) == 0)
				continue;

			vaddr = addr & PCI_BASE_ADDRESS_IO_MASK;

			port_mapped = 1;
		}

		pci->pci_addr[pos] = vaddr;

		pci->pci_map_mask |= (port_mapped << pos);
	}

}	/* end pci_get_base_addr */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	read config data specific to header type 1 device (PCI to PCI bridge)
 ****************************************************************
 */
void *
pci_readppb (PCIDATA *pci)
{
	pcih1cfgregs		*p;

	if ((p = pci_malloc (sizeof (pcih1cfgregs))) == NULL)
		return (NULL);

	memclr (p, sizeof *p);

	p->secstat	= pci_read (pci, PCIR_SECSTAT_1, 2);
	p->bridgectl	= pci_read (pci, PCIR_BRIDGECTL_1, 2);

	p->seclat	= pci_read (pci, PCIR_SECLAT_1, 1);

	p->iobase	= PCI_PPBIOBASE
			  (
				pci_read (pci, PCIR_IOBASEH_1, 2), pci_read (pci, PCIR_IOBASEL_1, 1)
			  );

	p->iolimit	= PCI_PPBIOLIMIT
			  (
				pci_read (pci, PCIR_IOLIMITH_1, 2), pci_read (pci, PCIR_IOLIMITL_1, 1)
			  );

	p->membase	= PCI_PPBMEMBASE (0, pci_read (pci, PCIR_MEMBASE_1, 2));
	p->memlimit	= PCI_PPBMEMLIMIT (0, pci_read (pci, PCIR_MEMLIMIT_1, 2));

	p->pmembase	= PCI_PPBMEMBASE
			  (
				(pci_addr_t)pci_read (pci, PCIR_PMBASEH_1, 4),
				pci_read (pci, PCIR_PMBASEL_1, 2)
			  );

	p->pmemlimit	= PCI_PPBMEMLIMIT
			  (
				(pci_addr_t)pci_read (pci, PCIR_PMLIMITH_1, 4),
				pci_read (pci, PCIR_PMLIMITL_1, 2)
			  );

	return (p);

}	/* end pci_readppb */

/*
 ****************************************************************
 *	read config data specific to header type 2 device (PCI to CardBus bridge)
 ****************************************************************
 */
void *
pci_readpcb (PCIDATA *pci)
{
	pcih2cfgregs		*p;

	if ((p = pci_malloc (sizeof (pcih2cfgregs))) == NULL)
		return (NULL);

	memclr (p, sizeof *p);

	p->secstat	= pci_read (pci, PCIR_SECSTAT_2, 2);
	p->bridgectl	= pci_read (pci, PCIR_BRIDGECTL_2, 2);
	
	p->seclat	= pci_read (pci, PCIR_SECLAT_2, 1);

	p->membase0	= pci_read (pci, PCIR_MEMBASE0_2, 4);
	p->memlimit0	= pci_read (pci, PCIR_MEMLIMIT0_2, 4);
	p->membase1	= pci_read (pci, PCIR_MEMBASE1_2, 4);
	p->memlimit1	= pci_read (pci, PCIR_MEMLIMIT1_2, 4);

	p->iobase0	= pci_read (pci, PCIR_IOBASE0_2, 4);
	p->iolimit0	= pci_read (pci, PCIR_IOLIMIT0_2, 4);
	p->iobase1	= pci_read (pci, PCIR_IOBASE1_2, 4);
	p->iolimit1	= pci_read (pci, PCIR_IOLIMIT1_2, 4);

	p->pccardif	= pci_read (pci, PCIR_PCCARDIF_2, 4);

	return (p);

}	/* end pci_readpcb */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Compatibiliza com a versão 1.0 do PCI			*
 ****************************************************************
 */
void
pci_fixancient (PCIDATA *pci)
{
	if (pci->pci_hdrtype != 0)
		return;

	/* PCI to PCI bridges use header type 1 */

	if (pci->pci_baseclass == PCIC_BRIDGE && pci->pci_subclass == PCIS_BRIDGE_PCI)
		pci->pci_hdrtype = 1;

}	/* end pci_fixancient */

/*
 ****************************************************************
 *	Habilita o acesso a um registrador			*
 ****************************************************************
 */
int
pci_enable (unsigned bus, unsigned slot, unsigned func, int reg, int bytes)
{
	int		dataport = 0;

	if
	(	bus <= PCI_BUSMAX && slot < devmax && func <= PCI_FUNCMAX &&
		reg <= PCI_REGMAX && bytes != 3 && (unsigned) bytes <= 4 && (reg & (bytes -1)) == 0
	)
	{
		switch (cfgmech)
		{
		    case 1:
			write_port_long ((1 << 31) | (bus << 16) | (slot << 11) 
				| (func << 8) | (reg & ~0x03), CONF1_ADDR_PORT);
			dataport = CONF1_DATA_PORT + (reg & 0x03);
			break;

		    case 2:
			write_port (0xF0 | (func << 1), CONF2_ENABLE_PORT);
			write_port (bus, CONF2_FORWARD_PORT);
			dataport = 0xC000 | (slot << 8) | reg;
			break;
		}
	}

	return (dataport);

}	/* end pci_enable */

/*
 ****************************************************************
 *	Desabilita o acesso					*
 ****************************************************************
 */
void
pci_disable (void)
{
	switch (cfgmech)
	{
	    case 1:
		write_port_long (0, CONF1_ADDR_PORT);
		break;

	    case 2:
		write_port (0, CONF2_ENABLE_PORT);
		write_port (0, CONF2_FORWARD_PORT);
		break;
	}

}	/* end pci_disable */

/*
 ****************************************************************
 *	Lê um registrador de configuração			*
 ****************************************************************
 */
int
pci_read (const PCIDATA *pci, int reg, int bytes)
{
	int		data = -1;
	int		port;

	port = pci_enable (pci->pci_bus, pci->pci_slot, pci->pci_func, reg, bytes);

	if (port != 0)
	{
		switch (bytes)
		{
		    case 1:
			data = read_port (port);
			break;

		    case 2:
			data = read_port_short (port);
			break;

		    case 4:
			data = read_port_long (port);
			break;
		}

		pci_disable ();
	}

	return (data);

}	/* end pci_read */

/*
 ****************************************************************
 *	Escreve em um registrador de configuração		*
 ****************************************************************
 */
void
pci_write (const PCIDATA *pci, int reg, int data, int bytes)
{
	int		port;

	port = pci_enable (pci->pci_bus, pci->pci_slot, pci->pci_func, reg, bytes);

	if (port != 0)
	{
		switch (bytes)
		{
		    case 1:
			write_port (data, port);
			break;

		    case 2:
			write_port_short (data, port);
			break;

		    case 4:
			write_port_long (data, port);
			break;
		}

		pci_disable ();
	}

}	/* end pci_write */

/*
 ****************************************************************
 *	Função de "probe" dos conjuntos de "chips"		*
 ****************************************************************
 */
char *
chipset_probe (PCIDATA *pci, ulong dev_vendor)
{
	char		revid, subclass;

	switch (dev_vendor)
	{
		/* Intel -- vendor 0x8086 */

	    case 0x00088086:
		return ("???");

	    case 0x71108086:
		return (NOSTR);

	    case 0x12258086:
		fixbushigh_i1225 (pci);
		return ("Intel 824?? host to PCI bridge");

	    case 0x71808086:
		return ("Intel 82443LX (440 LX) host to PCI bridge");

	    case 0x71908086:
		return ("Intel 82443BX (440 BX) host to PCI bridge");

	    case 0x71928086:
		return ("Intel 82443BX host to PCI bridge (AGP disabled)");

 	    case 0x71a08086:
 		return ("Intel 82443GX host to PCI bridge");

 	    case 0x71a18086:
 		return ("Intel 82443GX host to AGP bridge");

 	    case 0x71a28086:
 		return ("Intel 82443GX host to PCI bridge (AGP disabled)");

	    case 0x84c48086:
		return ("Intel 82454KX/GX (Orion) host to PCI bridge");

	    case 0x84ca8086:
		return ("Intel 82451NX Memory and I/O controller");

	    case 0x04868086:
		return ("Intel 82425EX PCI system controller");

	    case 0x04838086:
		return ("Intel 82424ZX (Saturn) cache DRAM controller");

	    case 0x04a38086:
		revid = pci->pci_revid;

		if (revid == 16 || revid == 17)
		    return ("Intel 82434NX (Neptune) PCI cache memory controller");

		return ("Intel 82434LX (Mercury) PCI cache memory controller");

	    case 0x122d8086:
		return ("Intel 82437FX PCI cache memory controller");

	    case 0x12358086:
		return ("Intel 82437MX mobile PCI cache memory controller");

	    case 0x12508086:
		return ("Intel 82439HX PCI cache memory controller");

	    case 0x70308086:
		return ("Intel 82437VX PCI cache memory controller");

	    case 0x71008086:
		return ("Intel 82439TX System controller (MTXC)");

	    case 0x71138086:
		return ("Intel 82371AB Power management controller");

	    case 0x12378086:
		fixwsc_natoma (pci);
		return ("Intel 82440FX (Natoma) PCI and memory controller");

	    case 0x84c58086:
		return ("Intel 82453KX/GX (Orion) PCI memory controller");

	    case 0x71208086:
		return ("Intel 82810 (i810 GMCH) Host To Hub bridge");

	    case 0x71228086:
		return ("Intel 82810-DC100 (i810-DC100 GMCH) Host To Hub bridge");

	    case 0x71248086:
		return ("Intel 82810E (i810E GMCH) Host To Hub bridge");

	    case 0x24158086:
		return ("Intel 82801AA (ICH) AC'97 Audio Controller");

	    case 0x24258086:
		return ("Intel 82801AB (ICH0) AC'97 Audio Controller");

		/* Sony -- vendor 0x104d */

	    case 0x8009104d:
		return ("Sony CXD1847A FireWire Host Controller");

		/* SiS -- vendor 0x1039 */

	    case 0x04961039:
		return ("SiS 85c496 PCI/VL Bridge");

	    case 0x04061039:
		return ("SiS 85c501");

	    case 0x06011039:
		return ("SiS 85c601");

	    case 0x55911039:
		return ("SiS 5591 host to PCI bridge");

	    case 0x00011039:
		return ("SiS 5591 host to AGP bridge");
	
		/* VLSI -- vendor 0x1004 */

	    case 0x00051004:
		return ("VLSI 82C592 Host to PCI bridge");

	    case 0x01011004:
		return ("VLSI 82C532 Eagle II Peripheral controller");

	    case 0x01041004:
		return ("VLSI 82C535 Eagle II System controller");

	    case 0x01051004:
		return ("VLSI 82C147 IrDA controller");

		/* VIA Technologies -- vendor 0x1106 (0x1107 on the Apollo Master) */

	    case 0x15761107:
		return ("VIA 82C570 (Apollo Master) system controller");

	    case 0x05851106:
		return ("VIA 82C585 (Apollo VP1/VPX) system controller");

	    case 0x05951106:
	    case 0x15951106:
		return ("VIA 82C595 (Apollo VP2) system controller");

	    case 0x05971106:
		return ("VIA 82C597 (Apollo VP3) system controller");

	    case 0x05981106:
		return ("VIA 82C598MVP (Apollo MVP3) host bridge");

	    case 0x30401106:
		return ("VIA 82C586B ACPI interface");

	    case 0x30571106:
		return ("VIA 82C686 ACPI interface");

	    case 0x30581106:
		return ("VIA 82C686 AC97 Audio");

	    case 0x30681106:
		return ("VIA 82C686 AC97 Modem");

		/* AMD -- vendor 0x1022 */

	    case 0x70061022:
		return ("AMD-751 host to PCI bridge");

		/* NEC -- vendor 0x1033 */

	    case 0x00021033:
		return ("NEC 0002 PCI to PC-98 local bus bridge");

	    case 0x00161033:
		return ("NEC 0016 PCI to PC-98 local bus bridge");

		/* AcerLabs -- vendor 0x10b9 */

	    case 0x154110b9:
		return ("AcerLabs M1541 (Aladdin-V) PCI host bridge");

	    case 0x710110b9:
		return ("AcerLabs M15x3 Power Management Unit");

		/* OPTi -- vendor 0x1045 */

	    case 0xc8221045:
		return ("OPTi 82C822 host to PCI Bridge");

	/* Texas Instruments -- vendor 0x104c */

	    case 0xac1c104c:
		return ("Texas Instruments PCI1225 CardBus controller");

	    case 0xac50104c:
		return ("Texas Instruments PCI1410 CardBus controller");

	    case 0xac51104c:
		return ("Texas Instruments PCI1420 CardBus controller");

	    case 0xac1b104c:
		return ("Texas Instruments PCI1450 CardBus controller");

	    case 0xac52104c:
		return ("Texas Instruments PCI1451 CardBus controller");

		/* NeoMagic -- vendor 0x10c8 */

	    case 0x800510c8:
		return ("NeoMagic MagicMedia 256AX Audio controller");

	    case 0x800610c8:
		return ("NeoMagic MagicMedia 256ZX Audio controller");

		/* ESS Technology Inc -- vendor 0x125d */

	    case 0x1978125d:
		return ("ESS Technology Maestro 2E Audio controller");

	/* Toshiba -- vendor 0x1179 */

	    case 0x07011179:
		return ("Toshiba Fast Infra Red controller");

	/* NEC -- vendor 0x1033 */

	    case 0x00011033:
	    case 0x002c1033:
	    case 0x003b1033:
		return (NOSTR);

	}	/* end switch */;

	subclass = pci->pci_subclass;

	if
	(	pci->pci_baseclass == PCIC_BRIDGE && subclass != PCIS_BRIDGE_PCI &&
		subclass != PCIS_BRIDGE_ISA && subclass != PCIS_BRIDGE_EISA
	)
		return (char *)pci_bridge_type (pci);

	return (NOSTR);

}	/* end chip_probe */

/*
 ****************************************************************
 *	Ajusta o "i1225"					*
 ****************************************************************
 */
void
fixbushigh_i1225 (PCIDATA *pci)
{
	int	sublementarybus;

	if ((sublementarybus = pci_read (pci, 0x41, 1)) != 0xFF)
	{
		pci->pci_secondarybus = sublementarybus + 1;
		pci->pci_subordinatebus  = sublementarybus + 1;
	}

}	/* end fixbushigh_i1225 */

/*
 ****************************************************************
 *	Ajusta o "natoma"					*
 ****************************************************************
 */
void
fixwsc_natoma (PCIDATA *pci)
{
	int		pmccfg;

	pmccfg = pci_read (pci, 0x50, 2);

#if defined (SMP)
	if (pmccfg & 0x8000)
	{
		printf ("Correcting Natoma config for SMP\n");
		pmccfg &= ~0x8000;
		pci_write (pci, 0x50, 2, pmccfg);
	}
#else
	if ((pmccfg & 0x8000) == 0)
	{
		printf ("Correcting Natoma config for non-SMP\n");
		pmccfg |= 0x8000;
		pci_write (pci, 0x50, 2, pmccfg);
	}
#endif

}	/* end fixwsc_natoma */

/*
 ****************************************************************
 *	Obtém o tipo de ponte					*
 ****************************************************************
 */
const char *
pci_bridge_type (PCIDATA *pci)
{
	if (pci->pci_baseclass != PCIC_BRIDGE)
		return (NOSTR);

	switch (pci->pci_subclass)
	{
	    case PCIS_BRIDGE_HOST:
		return ("Host to PCI");

	    case PCIS_BRIDGE_ISA:
		return ("PCI to ISA");

	    case PCIS_BRIDGE_EISA:
		return ("PCI to EISA");

	    case PCIS_BRIDGE_MCA:
		return ("PCI to MCA");

	    case PCIS_BRIDGE_PCI:
		return ("PCI to PCI");

	    case PCIS_BRIDGE_PCMCIA:
		return ("PCI to PCMCIA");

	    case PCIS_BRIDGE_NUBUS:
		return ("PCI to NUBUS");

	    case PCIS_BRIDGE_CARDBUS:
		return ("PCI to CardBus");

	    case PCIS_BRIDGE_OTHER:
		return ("PCI to Other");

	}	/* end switch */

	return ("PCI to ???");

}	/* end pci_bridge_type */

/*
 ****************************************************************
 *	Identifica pontes PCI/ISA				*
 ****************************************************************
 */
char *
isa_probe (PCIDATA *pci, ulong dev_vendor)
{
	char		revid;

	switch (dev_vendor)
	{
	    case 0x04848086:
		revid = pci->pci_revid;

		if (revid == 3)
			return ("Intel 82378ZB PCI to ISA bridge");
		else
			return ("Intel 82378IB PCI to ISA bridge");

	    case 0x122e8086:
		return ("Intel 82371FB PCI to ISA bridge");

	    case 0x70008086:
		return ("Intel 82371SB PCI to ISA bridge");

	    case 0x71108086:
		return ("Intel 82371AB PCI to ISA bridge");

	    case 0x24108086:
		return ("Intel 82801AA (ICH) PCI to LPC bridge");

	    case 0x24208086:
		return ("Intel 82801AB (ICH0) PCI to LPC bridge");
	
		/* VLSI -- vendor 0x1004 */

	    case 0x00061004:
		return ("VLSI 82C593 PCI to ISA bridge");

		/* VIA Technologies -- vendor 0x1106 */

	    case 0x05861106: /* south bridge section */
		return ("VIA 82C586 PCI-ISA bridge");

	    case 0x05961106:
		return ("VIA 82C596B PCI-ISA bridge");

	    case 0x06861106:
		return ("VIA 82C686 PCI-ISA bridge");

		/* AcerLabs -- vendor 0x10b9 */

	    case 0x153310b9:
		return ("AcerLabs M1533 portable PCI-ISA bridge");

	    case 0x154310b9:
		return ("AcerLabs M1543 desktop PCI-ISA bridge");

		/* SiS -- vendor 0x1039 */

	    case 0x00081039:
		return ("SiS 85c503 PCI-ISA bridge");

		/* Cyrix -- vendor 0x1078 */

	    case 0x00001078:
		return ("Cyrix Cx5510 PCI-ISA bridge");

	    case 0x01001078:
		return ("Cyrix Cx5530 PCI-ISA bridge");

		/* NEC -- vendor 0x1033 */

	    case 0x00011033:
		return ("NEC 0001 PCI to PC-98 C-bus bridge");

	    case 0x002c1033:
		return ("NEC 002C PCI to PC-98 C-bus bridge");

	    case 0x003b1033:
		return ("NEC 003B PCI to PC-98 C-bus bridge");

		/* UMC United Microelectronics 0x1060 */

	    case 0x886a1060:
		return ("UMC UM8886 ISA Bridge with EIDE");

		/* Cypress -- vendor 0x1080 */

	    case 0xc6931080:
		if (pci->pci_baseclass == PCIC_BRIDGE && pci->pci_subclass == PCIS_BRIDGE_ISA)
			return ("Cypress 82C693 PCI-ISA bridge");

	}	/* end switch */

	if (pci->pci_baseclass == PCIC_BRIDGE && pci->pci_subclass == PCIS_BRIDGE_ISA)
		return (char *)pci_bridge_type (pci);

	return (NOSTR);

}	/* end isa_probe */

/*
 ****************************************************************
 *	Identifica outros chips					*
 ****************************************************************
 */
char *
pcib_probe (PCIDATA *pci, ulong dev_vendor)
{
	switch (dev_vendor)
	{
		/* Intel -- vendor 0x8086 */

	    case 0x71818086:
		return ("Intel 82443LX (440 LX) PCI-PCI (AGP) bridge");

	    case 0x71918086:
		return ("Intel 82443BX (440 BX) PCI-PCI (AGP) bridge");

	    case 0x71A18086:
		return ("Intel 82443GX (440 GX) PCI-PCI (AGP) bridge");

	    case 0x84cb8086:
		return ("Intel 82454NX PCI Expander Bridge");

	    case 0x124b8086:
		return ("Intel 82380FB mobile PCI to PCI bridge");

	    case 0x24188086:
		return ("Intel 82801AA (ICH) Hub to PCI bridge");

	    case 0x24288086:
		return ("Intel 82801AB (ICH0) Hub to PCI bridge");
	
		/* VLSI -- vendor 0x1004 */

	    case 0x01021004:
		return ("VLSI 82C534 Eagle II PCI Bus bridge");

	    case 0x01031004:
		return ("VLSI 82C538 Eagle II PCI Docking bridge");

		/* VIA Technologies -- vendor 0x1106 */

	    case 0x85981106:
		return ("VIA 82C598MVP (Apollo MVP3) PCI-PCI (AGP) bridge");

		/* AcerLabs -- vendor 0x10b9 */

	    case 0x524710b9:
		return ("AcerLabs M5247 PCI-PCI(AGP Supported) bridge");

	    case 0x524310b9:/* 5243 seems like 5247, need more info to divide*/
		return ("AcerLabs M5243 PCI-PCI bridge");

		/* AMD -- vendor 0x1022 */

	    case 0x70071022:
		return ("AMD-751 PCI-PCI (AGP) bridge");

		/* DEC -- vendor 0x1011 */

	    case 0x00011011:
		return ("DEC 21050 PCI-PCI bridge");

	    case 0x00211011:
		return ("DEC 21052 PCI-PCI bridge");

	    case 0x00221011:
		return ("DEC 21150 PCI-PCI bridge");

	    case 0x00241011:
		return ("DEC 21152 PCI-PCI bridge");

	    case 0x00251011:
		return ("DEC 21153 PCI-PCI bridge");

	    case 0x00261011:
		return ("DEC 21154 PCI-PCI bridge");

		/* Others */

	    case 0x00221014:
		return ("IBM 82351 PCI-PCI bridge");

		/* UMC United Microelectronics 0x1060 */

	    case 0x88811060:
		return ("UMC UM8881 HB4 486 PCI Chipset");

	}	/* end switch */

	if (pci->pci_baseclass == PCIC_BRIDGE && pci->pci_subclass == PCIS_BRIDGE_PCI)
		return (char *)pci_bridge_type (pci);

	return (NOSTR);

}	/* end pcib_probe */

/*
 ****************************************************************
 *	Identifica placas de vídeo				*
 ****************************************************************
 */
char *
vga_probe (PCIDATA *pci, ulong dev_vendor)
{
	const char	*vendor = NOSTR, *chip = NOSTR, *type = NOSTR;
	int		dev = dev_vendor >> 16;
	static char	area[80];

	/*
	 *	x
	 */
	switch (dev_vendor & 0xFFFF)
	{
	    case 0x003D:
		vendor = "Real 3D";

		switch (dev)
		{
		    case 0x00d1:
			chip = "i740"; break;
		}

		break;

	    case 0x10C8:
		vendor = "NeoMagic";

		switch (dev)
		{
		    case 0x0003:
			chip = "MagicGraph 128ZV"; break;

		    case 0x0004:
			chip = "MagicGraph 128XD"; break;

		    case 0x0005:
			chip = "MagicMedia 256AV"; break;

		    case 0x0006:
			chip = "MagicMedia 256ZX"; break;
		}
		break;

	    case 0x121A:
		vendor = "3Dfx";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x0001:
			chip = "Voodoo"; break;

		    case 0x0002:
			chip = "Voodoo 2"; break;

  		    case 0x0003:
			chip = "Voodoo Banshee"; break;

		    case 0x0005:
			chip = "Voodoo 3"; break;
		}
		break;

	    case 0x102B:
		vendor = "Matrox";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x0518:
			chip = "MGA 2085PX"; break;

		    case 0x0519:
			chip = "MGA Millennium 2064W"; break;

		    case 0x051a:
			chip = "MGA 1024SG/1064SG/1164SG"; break;

		    case 0x051b:
			chip = "MGA Millennium II 2164W"; break;

		    case 0x051f:
			chip = "MGA Millennium II 2164WA-B AG"; break;

		    case 0x0520:
			chip = "MGA G200"; break;

		    case 0x0521:
			chip = "MGA G200 AGP"; break;

		    case 0x0525:
			chip = "MGA G400 AGP"; break;

		    case 0x0d10:
			chip = "MGA Impression"; break;

		    case 0x1000:
			chip = "MGA G100"; break;

		    case 0x1001:
			chip = "MGA G100 AGP"; break;

		}
		break;

	    case 0x1002:
		vendor = "ATI";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x4158:
			chip = "Mach32"; break;

		    case 0x4354:
			chip = "Mach64-CT"; break;

		    case 0x4358:
			chip = "Mach64-CX"; break;

		    case 0x4554:
			chip = "Mach64-ET"; break;

		    case 0x4654:
		    case 0x5654:
			chip = "Mach64-VT"; break;

		    case 0x4742:
			chip = "Mach64-GB"; break;

		    case 0x4744:
			chip = "Mach64-GD"; break;

		    case 0x4749:
			chip = "Mach64-GI"; break;

		    case 0x474d:
			chip = "Mach64-GM"; break;

		    case 0x474e:
			chip = "Mach64-GN"; break;

		    case 0x474f:
			chip = "Mach64-GO"; break;

		    case 0x4750:
			chip = "Mach64-GP"; break;

		    case 0x4751:
			chip = "Mach64-GQ"; break;

		    case 0x4752:
			chip = "Mach64-GR"; break;

		    case 0x4753:
			chip = "Mach64-GS"; break;

		    case 0x4754:
			chip = "Mach64-GT"; break;

		    case 0x4755:
			chip = "Mach64-GU"; break;

		    case 0x4756:
			chip = "Mach64-GV"; break;

		    case 0x4757:
			chip = "Mach64-GW"; break;

		    case 0x4758:
			chip = "Mach64-GX"; break;

		    case 0x4c4d:
			chip = "Mobility-1"; break;

		    case 0x475a:
			chip = "Mach64-GZ"; break;

		    case 0x5245:
			chip = "Rage128-RE"; break;

		    case 0x5246:
			chip = "Rage128-RF"; break;

		    case 0x524b:
			chip = "Rage128-RK"; break;

		    case 0x524c:
			chip = "Rage128-RL"; break;
		}
		break;

	    case 0x1005:
		vendor = "Avance Logic";

		switch (dev)
		{
		    case 0x2301:
			chip = "ALG2301"; break;

		    case 0x2302:
			chip = "ALG2302"; break;
		}
		break;

	    case 0x100C:
		vendor = "Tseng Labs";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x3202:
		    case 0x3205:
		    case 0x3206:
		    case 0x3207:
			chip = "ET4000 W32P"; break;

		    case 0x3208:
			chip = "ET6000/ET6100"; break;

		    case 0x4702:
			chip = "ET6300"; break;
		}
		break;

	    case 0x100E:
		vendor = "Weitek";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x9001:
			chip = "P9000"; break;

		    case 0x9100:
			chip = "P9100"; break;
		}
		break;

	    case 0x1013:
		vendor = "Cirrus Logic";

		switch (dev)
		{
		    case 0x0038:
			chip = "GD7548"; break;

		    case 0x0040:
			chip = "GD7555"; break;

		    case 0x004c:
			chip = "GD7556"; break;

		    case 0x00a0:
			chip = "GD5430"; break;

		    case 0x00a4:
		    case 0x00a8:
			chip = "GD5434"; break;

		    case 0x00ac:
			chip = "GD5436"; break;

		    case 0x00b8:
			chip = "GD5446"; break;

		    case 0x00bc:
			chip = "GD5480"; break;

		    case 0x00d0:
			chip = "GD5462"; break;

		    case 0x00d4:
		    case 0x00d5:
			chip = "GD5464"; break;

		    case 0x00d6:
			chip = "GD5465"; break;

		    case 0x1200:
			chip = "GD7542"; break;

		    case 0x1202:
			chip = "GD7543"; break;

		    case 0x1204:
			chip = "GD7541"; break;
		}
		break;

	    case 0x1023:
		vendor = "Trident";
		break;		/* let default deal with it */

	    case 0x102c:
		vendor = "Chips & Technologies";

		switch (dev)
		{
		    case 0x00b8:
			chip = "64310"; break;

		    case 0x00d8:
			chip = "65545"; break;

		    case 0x00dc:
			chip = "65548"; break;

		    case 0x00c0:
			chip = "69000"; break;

		    case 0x00e0:
			chip = "65550"; break;

		    case 0x00e4:
			chip = "65554"; break;

		    case 0x00e5:
			chip = "65555"; break;

		    case 0x00f4:
			chip = "68554"; break;
                }
		break;

	    case 0x1033:
		vendor = "NEC";

		switch (dev)
		{
		    case 0x0009:
			type = "PCI to PC-98 Core Graph bridge";
			break;
		}
		break;

	    case 0x1039:
		vendor = "SiS";

		switch (dev)
		{
		    case 0x0001:
			chip = "86c201"; break;

		    case 0x0002:
			chip = "86c202"; break;

		    case 0x0205:
			chip = "86c205"; break;

		    case 0x0215:
			chip = "86c215"; break;

		    case 0x0225:
			chip = "86c225"; break;

		    case 0x0200:
			chip = "5597/98"; break;

		    case 0x6326:
			chip = "6326"; break;

		    case 0x6306:
			chip = "530/620"; break;
		}
		break;

	    case 0x105D:
		vendor = "Number Nine";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x2309:
			chip = "Imagine 128"; break;

		    case 0x2339:
			chip = "Imagine 128 II"; break;
		}
		break;

	    case 0x1142:
		vendor = "Alliance";

		switch (dev)
		{
		    case 0x3210:
			chip = "PM6410"; break;

		    case 0x6422:
			chip = "PM6422"; break;

		    case 0x6424:
			chip = "PMAT24"; break;
		}
		break;

	    case 0x1163:
		vendor = "Rendition Verite";

		switch (dev)
		{
		    case 0x0001:
			chip = "V1000"; break;

		    case 0x2000:
			chip = "V2000"; break;
		}
		break;

	    case 0x1236:
		vendor = "Sigma Designs";

		if (dev == 0x6401)
			chip = "REALmagic64/GX";
		break;

	    case 0x5333:
		vendor = "S3";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x8811:
			chip = "Trio"; break;

		    case 0x8812:
			chip = "Aurora 64"; break;

		    case 0x8814:
			chip = "Trio 64UV+"; break;

		    case 0x8901:
			chip = "Trio 64V2/DX/GX"; break;

		    case 0x8902:
			chip = "Plato"; break;

		    case 0x8904:
			chip = "Trio3D"; break;

		    case 0x8880:
			chip = "868"; break;

		    case 0x88b0:
			chip = "928"; break;

		    case 0x88c0:
		    case 0x88c1:
			chip = "864"; break;

		    case 0x88d0:
		    case 0x88d1:
			chip = "964"; break;

		    case 0x88f0:
			chip = "968"; break;

		    case 0x5631:
			chip = "ViRGE"; break;

		    case 0x883d:
			chip = "ViRGE VX"; break;

		    case 0x8a01:
			chip = "ViRGE DX/GX"; break;

		    case 0x8a10:
			chip = "ViRGE GX2"; break;
		    case 0x8a13:
			chip = "Trio3D/2X"; break;

		    case 0x8a20:
		    case 0x8a21:
			chip = "Savage3D"; break;

		    case 0x8a22:
			chip = "Savage 4"; break;

		    case 0x8c01:
			chip = "ViRGE MX"; break;

		    case 0x8c03:
			chip = "ViRGE MX+"; break;
		}
		break;

	    case 0xedd8:
		vendor = "ARK Logic";

		switch (dev)
		{
		    case 0xa091:
			chip = "1000PV"; break;

		    case 0xa099:
			chip = "2000PV"; break;

		    case 0xa0a1:
			chip = "2000MT"; break;

		    case 0xa0a9:
			chip = "2000MI"; break;
		}
		break;

	    case 0x3d3d:
		vendor = "3D Labs";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x0001:
			chip = "300SX"; break;

		    case 0x0002:
			chip = "500TX"; break;

		    case 0x0003:
			chip = "Delta"; break;

		    case 0x0004:
			chip = "PerMedia"; break;
		}
		break;

	    case 0x10de:
		vendor = "NVidia";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x0008:
			chip = "NV1"; break;

		    case 0x0020:
			chip = "Riva TNT"; break;	

		    case 0x0028:
			chip = "Riva TNT2"; break;

		    case 0x0029:
			chip = "Riva Ultra TNT2"; break;

		    case 0x002c:
			chip = "Riva Vanta TNT2"; break;

		    case 0x002d:
			chip = "Riva Ultra Vanta TNT2"; break;

		    case 0x00a0:
			chip = "Riva Integrated TNT2"; break;

		    case 0x0100:
			chip = "GeForce 256"; break;

		    case 0x0101:
			chip = "GeForce DDR"; break;

		    case 0x0103:
			chip = "Quadro"; break;

		    case 0x0150:
		    case 0x0151:
		    case 0x0152:
			chip = "GeForce2 GTS"; break;

		    case 0x0153:
			chip = "Quadro2"; break;
		}
		break;

	    case 0x12d2:
		vendor = "NVidia/SGS-Thomson";
		type = "graphics accelerator";

		switch (dev)
		{
		    case 0x0018:
			chip = "Riva128"; break;	
		}
		break;

	    case 0x104a:
		vendor = "SGS-Thomson";

		switch (dev)
		{
		    case 0x0008:
			chip = "STG2000"; break;
		}
		break;

	    case 0x8086:
		vendor = "Intel";

		switch (dev)
		{
		    case 0x7121:
			chip = "82810 (i810 GMCH)"; break;

		    case 0x7123:
			chip = "82810-DC100 (i810-DC100 GMCH)"; break;

		    case 0x7125:
			chip = "82810E (i810E GMCH)"; break;

		    case 0x7800:
			chip = "i740 AGP"; break;
		}
		break;

	    case 0x10ea:
		vendor = "Intergraphics";

		switch (dev)
		{
		    case 0x1680:
			chip = "IGA-1680"; break;

		    case 0x1682:
			chip = "IGA-1682"; break;
		}
		break;

	}	/* end switch */

	/*
	 *	x
	 */
	if (vendor != NOSTR && chip != NOSTR)
	{
		if (type == NOSTR)
			type = "SVGA controller";

		snprintf (area, sizeof (area), "%s %s %s", vendor, chip, type);

		return (area);
	}

	/*
	 *	x
	 */
	switch (pci->pci_baseclass)
	{
	    case PCIC_OLD:
		if (pci->pci_subclass != PCIS_OLD_VGA)
			return (NOSTR);

		if (type == NOSTR)
			type = "VGA-compatible display device";
		break;

	    case PCIC_DISPLAY:
		if (type == NOSTR)
		{
			if (pci->pci_subclass == PCIS_DISPLAY_VGA)
				type = "VGA-compatible display device";
			else /* If it isn't a vga display device, don't pretend we found one */
				return (NOSTR);
		}
		break;

	    default:
		return (NOSTR);

	}	/* end switch */

	/*
	 *	Não identificado
	 */
	if (vendor)
	{
		snprintf (area, sizeof (area), "%s model %04x %s", vendor, dev, type);
		return (area);
	}

	return ((char *)type);

}	/* end vga_probe */

#if (0)	/*******************************************************/
/* additional type 1 device config header information (PCI to PCI bridge) */

#define PCI_PPBMEMBASE(h,l)	(((l)<<16) & ~0xFFFFF)
#define PCI_PPBMEMLIMIT(h,l) (((l)<<16) | 0xFFFFF)

#define PCI_PPBIOBASE(h,l)	((((h)<<16) + ((l)<<8)) & ~0xFFF)
#define PCI_PPBIOLIMIT(h,l)	((((h)<<16) + ((l)<<8)) | 0xFFF)

typedef ulong	pci_addr_t;

typedef struct
{
	pci_addr_t	pmembase;	/* base address of prefetchable memory */
	pci_addr_t	pmemlimit;	/* topmost address of prefetchable memory */
	ulong		membase;	/* base address of memory window */
	ulong		memlimit;	/* topmost address of memory window */
	ulong		iobase;		/* base address of port window */
	ulong		iolimit;	/* topmost address of port window */
	ushort		secstat;	/* secondary bus status register */
	ushort		bridgectl;	/* bridge control register */
	uchar		seclat;		/* CardBus latency timer */

}	pcih1cfgregs;

/* additional type 2 device config header information (CardBus bridge) */

typedef struct
{
	ulong		membase0;	/* base address of memory window */
	ulong		memlimit0;	/* topmost address of memory window */
	ulong		membase1;	/* base address of memory window */
	ulong		memlimit1;	/* topmost address of memory window */
	ulong		iobase0;	/* base address of port window */
	ulong		iolimit0;	/* topmost address of port window */
	ulong		iobase1;	/* base address of port window */
	ulong		iolimit1;	/* topmost address of port window */
	ulong		pccardif;	/* PC Card 16bit IF legacy more base addr. */
	ushort		secstat;	/* secondary bus status register */
	ushort		bridgectl;	/* bridge control register */
	uchar		seclat;		/* CardBus latency timer */

}	pcih2cfgregs;

struct pci_quirk
{
	ulong		devid;	/* Vendor/device of the card */
	int	type;
#define PCI_QUIRK_MAP_REG	1	/* PCI map register in wierd place */
	int	arg1;
	int	arg2;
};

struct pci_quirk pci_quirks[] = {
	/*
	 * The Intel 82371AB has a map register at offset 0x90.
	 */
	{0x71138086, PCI_QUIRK_MAP_REG, 0x90, 0},

	{0}
};
#endif	/*******************************************************/

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	return base address of memory or port map		*
 ****************************************************************
 */
ulong
pci_mapbase (unsigned mapreg)
{
	int		mask = 0x03;

	if ((mapreg & 0x01) == 0)
		mask = 0x0F;

	return (mapreg & ~mask);

}	/* end pci_mapbase */

/*
 ****************************************************************
 *	Return map type of memory or port map			*
 ****************************************************************
 */
int
pci_maptype (unsigned mapreg)
{
	static uchar maptype[0x10] =
	{
		PCI_MAPMEM,		PCI_MAPPORT,
		PCI_MAPMEM,		0,
		PCI_MAPMEM,		PCI_MAPPORT,
		0,			0,
		PCI_MAPMEM|PCI_MAPMEMP,	PCI_MAPPORT,
		PCI_MAPMEM|PCI_MAPMEMP, 0,
		PCI_MAPMEM|PCI_MAPMEMP,	PCI_MAPPORT,
		0,			0,
	};

	return maptype[mapreg & 0x0F];

}	/* end pci_maptype */

/*
 ****************************************************************
 *   Return log2 of map size decoded for memory or port map	*
 ****************************************************************
 */
int
pci_mapsize (unsigned testval)
{
	int		ln2size;

	testval = pci_mapbase (testval);
	ln2size = 0;

	if (testval != 0)
	{
		while ((testval & 1) == 0)
		{
			ln2size++;
			testval >>= 1;
		}
	}

	return (ln2size);

}	/* end pci_mapsize */

/*
 ****************************************************************
 *  Return log2 of address range supported by map register	*
 ****************************************************************
 */
int
pci_maprange (unsigned mapreg)
{
	int		ln2range = 0;

	switch (mapreg & 0x07)
	{
	    case 0x00:
	    case 0x01:
	    case 0x05:
		ln2range = 32;
		break;

	    case 0x02:
		ln2range = 20;
		break;

	    case 0x04:
		ln2range = 64;
		break;
	}

	return (ln2range);

} 	/* end pci_maprange */
#endif	/*******************************************************/
