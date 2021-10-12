/*
 ****************************************************************
 *								*
 *			pnp.c					*
 *								*
 *	Descobre as placas Plug and Play ISA			*
 *								*
 *	Versão	4.0.0, de 24.09.00				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"

#include "../h/extern.h"
#include "../h/proto.h"

/* Maximum Number of PnP Devices.  8 should be plenty */
#define MAX_PNP_CARDS 8
/*
 * the following is the maximum number of PnP Logical devices that
 * userconfig can handle.
 */
#define MAX_PNP_LDN	20

/* Static ports to access PnP state machine */
#define _PNP_ADDRESS		0x279
#define _PNP_WRITE_DATA		0xA79

/* PnP Registers.  Write to ADDRESS and then use WRITE/READ_DATA */
#define SET_RD_DATA		0x00
	/***
	Writing to this location modifies the address of the port used for
	reading from the Plug and Play ISA cards.   Bits[7:0] become I/O
	read port address bits[9:2].  Reads from this register are ignored.
	***/

#define SERIAL_ISOLATION	0x01
	/***
	A read to this register causes a Plug and Play cards in the Isolation
	state to compare one bit of the boards ID.
	This register is read only.
	***/

#define	CONFIG_CONTROL			0x02
#define CONFIG_CONTROL_RESET_CSN	0x04
#define CONFIG_CONTROL_WAIT_FOR_KEY	0x02
#define CONFIG_CONTROL_RESET		0x01

#define	CONFIG_CONTROL		0x02
	/***
	Bit[2]  Reset CSN to 0
	Bit[1]  Return to the Wait for Key state
	Bit[0]  Reset all logical devices and restore configuration
		registers to their power-up values.

	A write to bit[0] of this register performs a reset function on
	all logical devices.  This resets the contents of configuration
	registers to  their default state.  All card's logical devices
	enter their default state and the CSN is preserved.
		      
	A write to bit[1] of this register causes all cards to enter the
	Wait for Key state but all CSNs are preserved and logical devices
	are not aFFected.
			    
	A write to bit[2] of this register causes all cards to reset their
	CSN to zero .
			  
	This register is write-only.  The values are not sticky, that is,
	hardware will automatically clear them and there is no need for
	software to clear the bits.
	***/

#define WAKE			0x03
	/***
	A write to this port will cause all cards that have a CSN that
	matches the write data[7:0] to go from the Sleep state to the either
	the Isolation state if the write data for this command is zero or
	the Config state if the write data is not zero.  Additionally, the
	pointer to the byte-serial device is reset.  This register is  
	writeonly.
	***/

#define	RESOURCE_DATA		0x04
	/***
	A read from this address reads the next byte of resource information.
	The Status register must be polled until bit[0] is set before this
	register may be read.  This register is read only.
	***/

#define STATUS			0x05
	/***
	Bit[0] when set indicates it is okay to read the next data byte  
	from the Resource Data register.  This register is readonly.
	***/

#define SET_CSN			0x06
	/***
	A write to this port sets a card's CSN.  The CSN is a value uniquely
	assigned to each ISA card after the serial identification process
	so that each card may be individually selected during a Wake[CSN]
	command. This register is read/write. 
	***/

#define SET_LDN			0x07
	/***
	Selects the current logical device.  All reads and writes of memory,
	I/O, interrupt and DMA configuration information access the registers
	of the logical device written here.  In addition, the I/O Range
	Check and Activate  commands operate only on the selected logical
	device.  This register is read/write. If a card has only 1 logical
	device, this location should be a read-only value of 0x00.
	***/

/*** addresses 0x08 - 0x1F Card Level Reserved for future use ***/
/*** addresses 0x20 - 0x2F Card Level, Vendor Defined ***/

#define ACTIVATE		0x30
	/***
	For each logical device there is one activate register that controls
	whether or not the logical device is active on the ISA bus.  Bit[0],
	if set, activates the logical device.  Bits[7:1] are reserved and
	must return 0 on reads.  This is a read/write register. Before a
	logical device is activated, I/O range check must be disabled.
	***/

#define IO_RANGE_CHECK		0x31
	/***
	This register is used to perform a conflict check on the I/O port
	range programmed for use by a logical device.

	Bit[7:2]  Reserved and must return 0 on reads
	Bit[1]    Enable I/O Range check, if set then I/O Range Check
	is enabled. I/O range check is only valid when the logical
	device is inactive.

	Bit[0], if set, forces the logical device to respond to I/O reads
	of the logical device's assigned I/O range with a 0x55 when I/O
	range check is in operation.  If clear, the logical device drives
	0xAA.  This register is read/write.
	***/

/*** addr 0x32 - 0x37 Logical Device Control Reserved for future use ***/
/*** addr 0x38 - 0x3F Logical Device Control Vendor Define ***/

#define MEM_CONFIG		0x40
	/***
	Four memory resource registers per range, four ranges.
	Fill with 0 if no ranges are enabled.

	OFFset 0:	RW Memory base address bits[23:16]
	OFFset 1:	RW Memory base address bits[15:8]
	OFFset 2:	Memory control
	    Bit[1] specifies 8/16-bit control.  This bit is set to indicate
	    16-bit memory, and cleared to indicate 8-bit memory.
	    Bit[0], if cleared, indicates the next field can be used as a range
	    length for decode (implies range length and base alignment of memory
	    descriptor are equal).
	    Bit[0], if set, indicates the next field is the upper limit for
	    the address. -  - Bit[0] is read-only.
	OFFset 3:	RW upper limit or range len, bits[23:16]
	OFFset 4:	RW upper limit or range len, bits[15:8]
	OFFset 5-OFFset 7: filler, unused.
	***/

#define IO_CONFIG_BASE		0x60
	/***
	Eight ranges, two bytes per range.
	OFFset 0:		I/O port base address bits[15:8]
	OFFset 1:		I/O port base address bits[7:0]
	***/

#define IRQ_CONFIG		0x70
	/***
	Two entries, two bytes per entry.
	OFFset 0:	RW interrupt level (1..15, 0=unused).
	OFFset 1:	Bit[1]: level(1:hi, 0:low),
			Bit[0]: type (1:level, 0:edge)
		byte 1 can be readonly if 1 type of int is used.
	***/

#define DRQ_CONFIG		0x74
	/***
	Two entries, one byte per entry. Bits[2:0] select
	which DMA channel is in use for DMA 0.  Zero selects DMA channel
	0, seven selects DMA channel 7. DMA channel 4, the cascade channel
	is used to indicate no DMA channel is active.
	***/

/*
 ****************************************************************
 *	Macros para acesso aos recursos				*
 ****************************************************************
 */
/* Macros to parse Resource IDs */
#define PNP_RES_TYPE(a)		((a) >> 7)
#define PNP_SRES_NUM(a)		((a) >> 3)
#define PNP_SRES_LEN(a)		((a) & 0x07)
#define PNP_LRES_NUM(a)		((a) & 0x7F)

/* Small Resource Item names */
#define PNP_TAG_VERSION		0x1
#define PNP_TAG_LOGICAL_DEVICE	0x2
#define PNP_TAG_COMPAT_DEVICE	0x3
#define PNP_TAG_IRQ_FORMAT	0x4
#define PNP_TAG_DMA_FORMAT	0x5
#define PNP_TAG_START_DEPENDANT	0x6
#define PNP_TAG_END_DEPENDANT	0x7
#define PNP_TAG_IO_RANGE	0x8
#define PNP_TAG_IO_FIXED	0x9
#define PNP_TAG_RESERVED	0xA-0xD
#define PNP_TAG_VENDOR		0xE
#define PNP_TAG_END		0xF

/* Large Resource Item names */
#define PNP_TAG_MEMORY_RANGE	0x1
#define PNP_TAG_ID_ANSI		0x2
#define PNP_TAG_ID_UNICODE	0x3
#define PNP_TAG_LARGE_VENDOR	0x4
#define PNP_TAG_MEMORY32_RANGE	0x5
#define PNP_TAG_MEMORY32_FIXED	0x6
#define PNP_TAG_LARGE_RESERVED	0x7-0x7F

typedef	struct
{
	char	*pnp_name;
	int	(*pnp_probe) (ulong);
	void	(*pnp_attach) (ushort *, uchar *, uchar *);
	ulong	*pnp_count;

}	PNP_DEVICE;

/*
 ****************************************************************
 *	Estrutura de um dispositivo				*
 ****************************************************************
 */
typedef struct
{
	/*
	 *	Os três campos a seguir são obtidos com pnp_get_serial
	 */
	ulong		vendor_id;
	ulong		serial;
	uchar		checksum;

	int		csn;
	char		ansi_id[128];

	uchar		irq[2];		/* IRQ Number */
	uchar		irq_type[2];	/* IRQ Type */
	uchar		drq[2];
	uchar		enable;

	ushort		port[8];	/* The Base Address of the Port */

	struct
	{
		ulong	base;		/* Memory Base Address */
		int	control;	/* Memory Control Register */
		ulong	range;		/* Memory Range *OR* Upper Limit */

	}	mem[4];

}	PNPID;

/*
 ****************************************************************
 *	O conjunto de dispositivos PnP aceitos			*
 ****************************************************************
 */
/*
 ******	Placa de som Sound Blaster ******************************
 */
extern int	pnp_sb_probe  (ulong);
extern void	pnp_sb_attach (ushort *, uchar *, uchar *);
ulong		pnp_sb_count = 0;

entry	PNP_DEVICE	pnp_sb_device =
{
	"pnp_sb",
	pnp_sb_probe,
	pnp_sb_attach,
	&pnp_sb_count
};

/*
 ******	Placa Ethernet (NE2000) *********************************
 */
extern int	pnp_ed_probe  (ulong);
extern void	pnp_ed_attach (ushort *, uchar *, uchar *);
ulong		pnp_ed_count = 0;

entry	PNP_DEVICE	pnp_ed_device =
{
	"pnp_ed",
	pnp_ed_probe,
	pnp_ed_attach,
	&pnp_ed_count
};

/*
 ******	O conjunto de dispositivos previstos ********************
 */
entry	PNP_DEVICE	*pnpdevice_set[] =
{
	&pnp_sb_device,
	&pnp_ed_device,
	NULL
};

/* The READ_DATA port that we are using currently */
static int pnp_rd_port;

void	pnp_send_initiation_key (void);
int	pnp_get_serial (PNPID *p);
void	config_pnp_device (PNPID *p);
int	pnp_isolation_protocol (void);
uchar	pnp_read (int d);
int     read_pnp_parms (PNPID *);

/*
 ****************************************************************
 *	Descomprime a identificação do dispositivo		*
 ****************************************************************
 */
char *
pnp_unpack (ulong id)
{
	uchar			*data = (uchar *) &id;
	static char		buf[8];
	static const char	hextoascii[] = "0123456789ABCDEF";

	buf[0] = '@' + ((data[0] & 0x7C) >> 2);
	buf[1] = '@' + (((data[0] & 0x3) << 3) + ((data[1] & 0xE0) >> 5));
	buf[2] = '@' + (data[1] & 0x1F);

	buf[3] = hextoascii[(data[2] >> 4)];
	buf[4] = hextoascii[(data[2] & 0xF)];
	buf[5] = hextoascii[(data[3] >> 4)];
	buf[6] = hextoascii[(data[3] & 0xF)];

	buf[7] = 0;

	return (buf);

}	/* end pnp_unpack */

/*
 ****************************************************************
 *	Escreve em um registrador do PnP			*
 ****************************************************************
 */
void
pnp_write (int d, uchar r)
{
	write_port (d, _PNP_ADDRESS);
	write_port (r, _PNP_WRITE_DATA);

}	/* end pnp_write */

/*
 ****************************************************************
 *	Lê de um registrador do PnP				*
 ****************************************************************
 */
uchar
pnp_read (int d)
{
	write_port (d, _PNP_ADDRESS);
	return (read_port (3 | (pnp_rd_port << 2)));

}	/* end pnp_read */

/*
 ****************************************************************
 *	Inicialização						*
 ****************************************************************
 *
 * Send Initiation LFSR as described in "Plug and Play ISA Specification",
 * Intel May 94.
 */
void
pnp_send_initiation_key (void)
{
	int	cur, i;

	/* Reset the LFSR */
	write_port (0, _PNP_ADDRESS);
	write_port (0, _PNP_ADDRESS); /* yes, we do need it twice! */

	cur = 0x6A;
	write_port (cur, _PNP_ADDRESS);

	for (i = 1; i < 32; i++)
	{
		cur = (cur >> 1) | (((cur ^ (cur >> 1)) << 7) & 0xFF);
		write_port (cur, _PNP_ADDRESS);
	}

}	/* end pnp_send_initiation_key */

/*
 ****************************************************************
 *	Obtém o nr. de série do dispositivo			*
 ****************************************************************
 */
int
pnp_get_serial (PNPID *p)
{
	int	i, bite, valid = 0, sum = 0x6a;
	uchar	*data = (uchar *)p;
	int	port = (pnp_rd_port << 2) | 3;

	memclr (data, sizeof (char) * 9);

	write_port (SERIAL_ISOLATION, _PNP_ADDRESS);

	for (i = 0; i < 72; i++)
	{
		bite = read_port (port) == 0x55;
		DELAY (250);	/* Delay 250 usec */

		/* Can't Short Circuit the next evaluation, so 'and' is last */
		bite = (read_port (port) == 0xAA) && bite;
		DELAY (250);	/* Delay 250 usec */

		valid = valid || bite;

		if (i < 64)
			sum = (sum >> 1) | (((sum ^ (sum >> 1) ^ bite) << 7) & 0xFF);

		data[i / 8] = (data[i / 8] >> 1) | (bite ? 0x80 : 0);
	}

	valid = valid && (data[8] == sum);

	return (valid);

}	/* end pnp_get_serial */

/*
 ****************************************************************
 *	Lê os recursos de um dispositivo			*
 ****************************************************************
 */
int
pnp_read_bytes (char *bp, int len)
{
	int	j, count, port = (pnp_rd_port << 2) | 3;
	char	temp;

	for (count = 0; count < len; count++)
	{
		write_port (STATUS, _PNP_ADDRESS);

		for (j = 10000; j > 0; j--)
		{
			if (read_port (port) & 1)
				break;

			DELAY (1);
		}

		if (j == 0)
		{
			printf ("pnp: tempo esgotado na leitura\n");
			return (count);
		}

		write_port (RESOURCE_DATA, _PNP_ADDRESS);

		temp = read_port (port);

		if (bp != NULL)
			*bp++ = temp;
	}

	return (count);

}	/* end pnp_read_bytes */

/*
 ****************************************************************
 *	Lê o PORT, IRQ e DMA de um dispositivo			*
 ****************************************************************
 */
int
read_pnp_parms (PNPID *p)
{
	int	i;

	pnp_write (SET_LDN, 0);

	if ((i = pnp_read (SET_LDN)) != 0)
		return (-1);

	for (i = 0; i < 8; i++)
	{
		p->port[i]  = pnp_read (IO_CONFIG_BASE + i * 2) << 8;
		p->port[i] |= pnp_read (IO_CONFIG_BASE + i * 2 + 1);

		if (i < 4)
		{
			p->mem[i].base    = pnp_read (MEM_CONFIG + i*8 + 0) << 16;
			p->mem[i].base   |= pnp_read (MEM_CONFIG + i*8 + 1) << 8;
			p->mem[i].control = pnp_read (MEM_CONFIG + i*8 + 2);
			p->mem[i].range   = pnp_read (MEM_CONFIG + i*8 + 3) << 16;
			p->mem[i].range  |= pnp_read (MEM_CONFIG + i*8 + 4) << 8;
		}

		if (i < 2)
		{
			p->irq[i]      = pnp_read (IRQ_CONFIG + i * 2);
			p->irq_type[i] = pnp_read (IRQ_CONFIG + 1 + i * 2);
			p->drq[i]      = pnp_read (DRQ_CONFIG + i);
		}
	}

	p->enable = pnp_read (ACTIVATE);

	printf ("%s: Port = 0x%X", p->ansi_id, p->port[0]);

	for (i = 1; i < 8; i++)
	{
		if (p->port[i])
			printf (" 0x%X", p->port[i]);
		else
			break;
	}

	printf (", irq = %d", p->irq[0]);

	if (p->irq[1])
		printf ("/%d", p->irq[1]);

	printf (", dma = %d", p->drq[0]);

	if (p->drq[1])
		printf ("/%d", p->drq[1]);

	if (!p->enable)
		printf (", (disabled)");

	printf ("\n");

#if (0)	/*******************************************************/
	printf
	(	"%s: port 0x%04X 0x%04X 0x%04X 0x%04X irq %d:%d drq %d:%d en %d\n",
		p->ansi_id,
		p->port[0], p->port[1], p->port[2], p->port[3],
		p->irq[0],  p->irq[1],	p->drq[0], p->drq[1],
		p->enable
	);
#endif	/*******************************************************/

	return (1); /* success */

}	/* end read_pnp_parms */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Configure PnP devices. PNPID is made of:
 *	4 bytes: board id (which can be printed as an ascii string);
 *	4 bytes: board serial number (often 0 or -1 ?)
 */
void
config_pnp_device (PNPID *p)
{
	PNP_DEVICE	*dvp, **dvpp;
	char		vendor[8];
	int		res;

	strcpy (vendor, pnp_unpack (p->vendor_id));

	printf
	(	"pnp[%d]: <%s>, vendor = 0x%P (%s)\n",
		p->csn,
		p->ansi_id[0] ? p->ansi_id : vendor,
		p->vendor_id, vendor
	);

	res = 0;

	for (dvpp = pnpdevice_set; dvp = *dvpp++; /* vazio */)
	{
		if (dvp->pnp_probe)
		{
			if (res = (*dvp->pnp_probe) (p->vendor_id))
				break;
		}
	}

	if (res)
	{
		read_pnp_parms (p);

		(*dvp->pnp_attach) (p->port, p->irq, p->drq);
	}

}	/* end config_pnp_device */

/*
 ****************************************************************
 *	Obtém os recursos de um dispositivo PnP			*
 ****************************************************************
 */
int
pnp_scan_resources (PNPID *p)
{
	char	tag;
	int	len;
	char	buf[512];

	for (EVER)
	{
		if (pnp_read_bytes (&tag, 1) != 1)
			return (-1);

		if (PNP_RES_TYPE (tag) == 0)
		{
			len = PNP_SRES_LEN (tag);

			if (pnp_read_bytes (buf, len) != len)
				return (-1);

			if (PNP_SRES_NUM (tag) == PNP_TAG_END)
				return (0);
		}
		else
		{
			len = 0;

			if (pnp_read_bytes ((char *)&len, 2) != 2)
				return (-1);

			if (pnp_read_bytes (buf, len) != len)
				return (-1);

			buf[len] = 0;

			if (PNP_LRES_NUM (tag) == PNP_TAG_ID_ANSI)
			{
				if (p->ansi_id[0] == 0)
					strcpy (p->ansi_id, buf);
			}
		}
	}

}	/* end pnp_scan_resources */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
/*
 * Run the isolation protocol. Use pnp_rd_port as the READ_DATA port
 * value (caller should try multiple READ_DATA locations before giving
 * up). Upon exiting, all cards are aware that they should use
 * pnp_rd_port as the READ_DATA port.
 *
 * In the first pass, a csn is assigned to each board and PNPID's
 * are saved to an array, pnp_devices. In the second pass, each
 * card is woken up and the device configuration is called.
 */
int
pnp_isolation_protocol (void)
{
	int		csn, found = 0;
	PNPID		id;

	pnp_send_initiation_key ();

	pnp_write (CONFIG_CONTROL, CONFIG_CONTROL_RESET_CSN);	/* Reset CSN for All Cards */

	for (csn = 1; csn < MAX_PNP_CARDS; csn++)
	{
		pnp_write (WAKE, 0);
		pnp_write (SET_RD_DATA, pnp_rd_port);

		write_port (SERIAL_ISOLATION, _PNP_ADDRESS);
		DELAY (1000);	/* Delay 1 msec */

		if (pnp_get_serial (&id) == 0)
			break;

		pnp_write (SET_CSN, csn);

		id.csn        = csn;
		id.ansi_id[0] = 0;

		if (pnp_scan_resources (&id) < 0)
		{
			printf ("csn = %d: pnp_scan_resources falhou\n", csn);
			break;
		}

		config_pnp_device (&id);

		found++;
	}

	pnp_write (CONFIG_CONTROL, CONFIG_CONTROL_WAIT_FOR_KEY);	/* Reset CSN for All Cards */

	return (found);

}	/* end pnp_isolation_protocol */

/*
 ****************************************************************
 *	Procura por dispositivos PnP ISA			*
 ****************************************************************
 */
void
pnp_init (void)
{
	for (pnp_rd_port = 0x90; (pnp_rd_port < 0xFF); pnp_rd_port += 0x10)
	{
		if (pnp_isolation_protocol ())
			break;
	}

}	/* end pnp_init */
