/*
 ****************************************************************
 *								*
 *			live.c					*
 *								*
 *	"Driver" para controlador RealTek Fast ETHERNET		*
 *								*
 *	Versão	4.4.0, de 12.12.02				*
 *		4.4.0, de 12.12.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/pci.h"
#include "../h/map.h"
#include "../h/live.h"
#include "../h/frame.h"
#include "../h/intr.h"

#if (0)	/*******************************************************/
#include "../h/scb.h"
#include "../h/map.h"

#include "../h/itnet.h"
#include "../h/ed.h"
#include "../h/tty.h"

#include "../h/kfile.h"
#include "../h/timeout.h"
#endif	/*******************************************************/

#include "../h/inode.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
#define	MUSICPGSZ	8

#define PL	4		/* Prioridade de interrupção */
#define spllive	spl4		/* Função de prioridade de interrupção */

entry int	int_count;
#if (0)	/*******************************************************/
#define	NED	2		/* Número de unidades */
#endif	/*******************************************************/

#if (0)	/*******************************************************/
/*
 ******	Definições da placa *************************************
 */
#define PTR			0x00		/* Indexed register set pointer register	*/
						/* NOTE: The CHANNELNUM and ADDRESS words can	*/
						/* be modified independently of each other.	*/
#define PTR_CHANNELNUM_MASK	0x0000003f	/* For each per-channel register, indicates the	*/
						/* channel number of the register to be		*/
						/* accessed.  For non per-channel registers the	*/
						/* value should be set to zero.			*/
#define PTR_ADDRESS_MASK	0x07ff0000	/* Register index				*/

#define DATA			0x04		/* Indexed register set data register		*/

#define IPR			0x08		/* Global interrupt pending register		*/
						/* Clear pending interrupts by writing a 1 to	*/
						/* the relevant bits and zero to the other bits	*/
#endif	/*******************************************************/

/*
 ******	Estruturas globais **************************************
 */
typedef struct
{
	char	l_present;	/* A unidade está ativa */
	char	l_tos_link;
	char	l_APS;
	char	l_open;		/* No. de opens */

	int	l_port;		/* A base das portas */
	int	*l_ptb_pages;	/* Ponteiros para áreas de E/S */
	int	*l_silent_page;	/* Silêncio */
	int	*l_music_page;	/* Música */
	int	l_timer;
	int	l_timerinterval;

}	LIVE;

entry LIVE	live;

/*
 ******	Dados de uma VOZ ****************************************
 */
typedef struct voice	VOICE;

struct voice
{
	int	v_num;		/* No. da voz */
	int	v_speed;	/* Velocidade */
	int	v_start, v_end;	/* Definição da área */
	int	v_vol;		/* Volume */

	char	v_16;		/* É 16 bits */
	char	v_stereo;	/* É stereo */
	char	v_ismaster;	/* É mestre */

	VOICE	*v_slave;
};

entry VOICE	voice[2];

/*
 ******	Variáveis globais ***************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
int		live_init (const PCIDATA *);
void		live_initefx (const LIVE *lp);
void		emu_vwrite (LIVE *lp, VOICE *vp);
void		emu_vtrigger (LIVE *lp, VOICE *vp, int go);
void		emu_wrptr (const LIVE *lp, int chn, int reg, int data);
int		emu_rdcd (const LIVE *lp, int regno);
void		emu_wrcd (const LIVE *lp, int regno, int data);
void		emu_addefxop (const LIVE *lp, int op, int z, int w, int x, int y, int *pc);
void		liveint (struct intr_frame);
int		emu_enatimer (LIVE *lp, int go);
void		emu_enastop (LIVE *lp, int channel, int enable);
int		emu_rate_to_pitch (int rate);
unsigned	emu_rate_to_linearpitch (unsigned rate);

/*
 ****************************************************************
 *	Identifica a placa Creative LIVE			*
 ****************************************************************
 */
char *
pci_live_probe (PCIDATA * tag, ulong type)
{
	switch (type)
	{
	    case 0x00021102:
		return ("Creative SB Live");

	}	/* end switch (type) */

	return (NOSTR);

}	/* end pci_live_probe */

/*
 ****************************************************************
 *	Função de "attach"					*
 ****************************************************************
 */
void
pci_live_attach (PCIDATA * pci, int unit)
{
	uint		command;
	int		irq;

	command = pci_read (pci, PCIR_COMMAND, /* bytes */ 2);
	command |= (PCIM_CMD_PORTEN|PCIM_CMD_BUSMASTEREN);
	pci_write (pci, PCIR_COMMAND, command, /* bytes */ 2);

	live.l_port = pci_read (pci, PCI_MAP_REG_START, 4) & ~1;
	irq = pci->pci_intline;

	spllive ();

	if (live_init (pci) < 0)
	{
		printf ("pci_live_attach: Não consegui inicializar a placa\n");
		return;
	}

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (irq, PL, unit, liveint) < 0)
		return;

	/*
	 *	Imprime o que encontrou
	 */
	printf ("live: [0x%X, %d]\n", live.l_port, irq);

	live.l_present = 1;	/* a unidade realmente está ativa */

	spl0 ();

}	/* end pci_live_attach */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
live_init (const PCIDATA *pci)
{
	int		spcs, ch, silence, music, tmp, i;
	LIVE		*lp = &live;
	pg_t		pgno;

	/*
	 *	Desabilita o áudio e tranca o "cache"
	 */
	write_port_long (HCFG_LOCKSOUNDCACHE|HCFG_LOCKTANKCACHE|HCFG_MUTEBUTTONENABLE, lp->l_port + HCFG);

	/*
	 *	Inicializa as áreas de gravação
	 */
	emu_wrptr (lp, 0, MICBS, ADCBS_BUFSIZE_NONE);
	emu_wrptr (lp, 0, MICBA, 0);
	emu_wrptr (lp, 0, FXBS, ADCBS_BUFSIZE_NONE);
	emu_wrptr (lp, 0, FXBA, 0);
	emu_wrptr (lp, 0, ADCBS, ADCBS_BUFSIZE_NONE);
	emu_wrptr (lp, 0, ADCBA, 0);

	/*
	 *	Desabilita a interrupção do canal
	 */
	write_port_long (INTE_INTERVALTIMERENB|INTE_SAMPLERATETRACKER|INTE_PCIERRORENABLE, lp->l_port + INTE);

	emu_wrptr (lp, 0, CLIEL, 0);
	emu_wrptr (lp, 0, CLIEH, 0);
	emu_wrptr (lp, 0, SOLEL, 0);
	emu_wrptr (lp, 0, SOLEH, 0);

	/*
	 *	Inicializa a máquina de envelopes
	 */
	for (ch = 0; ch < NUM_G; ch++)
	{
		emu_wrptr (lp, ch, DCYSUSV, ENV_OFF);
		emu_wrptr (lp, ch, IP, 0);
		emu_wrptr (lp, ch, VTFT, 0xffff);
		emu_wrptr (lp, ch, CVCF, 0xffff);
		emu_wrptr (lp, ch, PTRX, 0);
		emu_wrptr (lp, ch, CPF, 0);
		emu_wrptr (lp, ch, CCR, 0);

		emu_wrptr (lp, ch, PSST, 0);
		emu_wrptr (lp, ch, DSL, 0x10);
		emu_wrptr (lp, ch, CCCA, 0);
		emu_wrptr (lp, ch, Z1, 0);
		emu_wrptr (lp, ch, Z2, 0);
		emu_wrptr (lp, ch, FXRT, 0xd01c0000);

		emu_wrptr (lp, ch, ATKHLDM, 0);
		emu_wrptr (lp, ch, DCYSUSM, 0);
		emu_wrptr (lp, ch, IFATN, 0xffff);
		emu_wrptr (lp, ch, PEFE, 0);
		emu_wrptr (lp, ch, FMMOD, 0);
		emu_wrptr (lp, ch, TREMFRQ, 24);	/* 1 Hz */
		emu_wrptr (lp, ch, FM2FRQ2, 24);	/* 1 Hz */
		emu_wrptr (lp, ch, TEMPENV, 0);

		/*** these are last so OFF prevents writing ***/

		emu_wrptr (lp, ch, LFOVAL2, 0);
		emu_wrptr (lp, ch, LFOVAL1, 0);
		emu_wrptr (lp, ch, ATKHLDV, 0);
		emu_wrptr (lp, ch, ENVVOL, 0);
		emu_wrptr (lp, ch, ENVVAL, 0);

#if (0)	/*******************************************************/
		sc->voice[ch].vnum = ch;
		sc->voice[ch].slave = NULL;
		sc->voice[ch].busy = 0;
		sc->voice[ch].ismaster = 0;
		sc->voice[ch].running = 0;
		sc->voice[ch].b16 = 0;
		sc->voice[ch].stereo = 0;
		sc->voice[ch].speed = 0;
		sc->voice[ch].start = 0;
		sc->voice[ch].end = 0;
		sc->voice[ch].channel = NULL;
#endif	/*******************************************************/
       }

#if (0)	/*******************************************************/
       sc->pnum = sc->rnum = 0;
#endif	/*******************************************************/

	/*
	 *  Init to 0x02109204 :
	 *  Clock accuracy    = 0     (1000ppm)
	 *  Sample Rate       = 2     (48kHz)
	 *  Audio Channel     = 1     (Left of 2)
	 *  Source Number     = 0     (Unspecified)
	 *  Generation Status = 1     (Original for Cat Code 12)
	 *  Cat Code          = 12    (Digital Signal Mixer)
	 *  Mode              = 0     (Mode 0)
	 *  Emphasis          = 0     (None)
	 *  CP                = 1     (Copyright unasserted)
	 *  AN                = 0     (Audio data)
	 *  P                 = 0     (Consumer)
	 */
	spcs = SPCS_CLKACCY_1000PPM | SPCS_SAMPLERATE_48 |
	       SPCS_CHANNELNUM_LEFT | SPCS_SOURCENUM_UNSPEC |
	       SPCS_GENERATIONSTATUS | 0x00001200 | 0x00000000 |
	       SPCS_EMPHASIS_NONE | SPCS_COPYRIGHT;

	emu_wrptr (lp, 0, SPCS0, spcs);
	emu_wrptr (lp, 0, SPCS1, spcs);
	emu_wrptr (lp, 0, SPCS2, spcs);

	live_initefx (lp);

#if (0)	/*******************************************************/
	SLIST_INIT(&sc->mem.blocks);
#endif	/*******************************************************/

	/*
	 *	Aloca as diversas áreas
	 */
	if ((pgno = malloce (M_CORE, BYUPPG (MAXPAGES * sizeof (int)))) == NOPG)
		return (-1);

	lp->l_ptb_pages = (int *)PGTOBY (pgno);

	if ((pgno = malloce (M_CORE, BYUPPG (EMUPAGESIZE))) == NOPG)
		return (-1);

	lp->l_silent_page = (void *)PGTOBY (pgno);

	memclr (lp->l_silent_page, EMUPAGESIZE);

	silence = VIRT_TO_PHYS_ADDR (lp->l_silent_page) << 1;

	for (i = 0; i < MAXPAGES; i++)
		lp->l_ptb_pages[i] = silence | i;

	if ((pgno = malloce (M_CORE, MUSICPGSZ)) == NOPG)
		return (-1);

	lp->l_music_page = (void *)PGTOBY (pgno); 

	memclr (lp->l_music_page, PGTOBY (MUSICPGSZ));

	music = VIRT_TO_PHYS_ADDR (lp->l_music_page) << 1;

	for (i = 1; i <= MUSICPGSZ; i++)
		lp->l_ptb_pages[i] = music | i;

	emu_wrptr (lp, 0, PTB,  VIRT_TO_PHYS_ADDR (lp->l_ptb_pages));
	emu_wrptr (lp, 0, TCB,  0);	/* taken from original driver */
	emu_wrptr (lp, 0, TCBS, 0);	/* taken from original driver */

	for (ch = 0; ch < NUM_G; ch++)
	{
		emu_wrptr (lp, ch, MAPA, silence|MAP_PTI_MASK);
		emu_wrptr (lp, ch, MAPB, silence|MAP_PTI_MASK);
	}

	/* emu_memalloc(sc, EMUPAGESIZE); */
	/*
	 *  Hokay, now enable the AUD bit
	 *
	 *   Enable Audio = 1
	 *   Mute Disable Audio = 0
	 *   Lock Tank Memory = 1
	 *   Lock Sound Memory = 0
	 *   Auto Mute = 1
	 */
#if (0)	/*******************************************************/
	tmp = HCFG_AUDIOENABLE|HCFG_LOCKTANKCACHE|HCFG_AUTOMUTE;
#endif	/*******************************************************/
	tmp = HCFG_AUDIOENABLE|HCFG_LOCKTANKCACHE;

	if (pci->pci_revid >= 6)
		tmp |= HCFG_JOYENABLE;

	write_port_long (tmp, lp->l_port + HCFG);

	/* TOSLink detection */

	lp->l_tos_link = 0;
	tmp = read_port_long (lp->l_port + HCFG);

	if (tmp & (HCFG_GPINPUT0|HCFG_GPINPUT1))
	{
		write_port_long (tmp|0x800, lp->l_port + HCFG);

		DELAY (50);

		if (tmp != (read_port_long (lp->l_port + HCFG) & ~0x800))
		{
			lp->l_tos_link = 1;
			write_port_long (tmp, lp->l_port + HCFG);
		}
	}

emu_wrcd (lp, AC97_MASTERVOLUME, 0x7FFF);
emu_wrcd (lp, AC97_MASTERVOLUMEMONO, 0x7FFF);
emu_wrcd (lp, AC97_PCMOUTVOLUME, 0x7FFF);


#if (0)	/*******************************************************/
printf ("Master vol = %d\n", emu_rdcd (lp, AC97_MASTERVOLUME));
printf ("Master vol = %d\n", emu_rdcd (lp, AC97_MASTERVOLUMEMONO));
printf ("PCM    vol = %d\n", emu_rdcd (lp, AC97_PCMOUTVOLUME));
#endif	/*******************************************************/


	return (0);

}	/* end live_init */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
live_initefx (const LIVE *lp)
{
	int		i, pc = 16;

	for (i = 0; i < 512; i++)
	{
		emu_wrptr (lp, 0, MICROCODEBASE + i * 2,     0x010040);
		emu_wrptr (lp, 0, MICROCODEBASE + i * 2 + 1, 0x610040);
	}

	for (i = 0; i < 256; i++)
		emu_wrptr (lp, 0, FXGPREGBASE + i, 0);

	/*
	 *	FX-8010 DSP Registers:
	 *
	 * FX Bus
	 *   0x000-0x00f : 16 registers
	 * Input
	 *   0x010/0x011 : AC97 Codec (l/r)
	 *   0x012/0x013 : ADC, S/PDIF (l/r)
	 *   0x014/0x015 : Mic(left), Zoom (l/r)
	 *   0x016/0x017 : APS S/PDIF?? (l/r)
	 * Output
	 *   0x020/0x021 : AC97 Output (l/r)
	 *   0x022/0x023 : TOS link out (l/r)
	 *   0x024/0x025 : ??? (l/r)
	 *   0x026/0x027 : LiveDrive Headphone (l/r)
	 *   0x028/0x029 : Rear Channel (l/r)
	 *   0x02a/0x02b : ADC Recording Buffer (l/r)
	 * Constants
	 *   0x040 - 0x044 = 0 - 4
	 *   0x045 = 0x8, 0x046 = 0x10, 0x047 = 0x20
	 *   0x048 = 0x100, 0x049 = 0x10000, 0x04a = 0x80000
	 *   0x04b = 0x10000000, 0x04c = 0x20000000, 0x04d = 0x40000000
	 *   0x04e = 0x80000000, 0x04f = 0x7fffffff
	 * Temporary Values
	 *   0x056 : Accumulator
	 *   0x058 : Noise source?
	 *   0x059 : Noise source?
	 * General Purpose Registers
	 *   0x100 - 0x1ff
	 * Tank Memory Data Registers
	 *   0x200 - 0x2ff
	 * Tank Memory Address Registers
	 *   0x300 - 0x3ff
	 *
	 * Operators:
	 * 0 : z := w + (x * y >> 31)
	 * 4 : z := w + x * y
	 * 6 : z := w + x + y
	 */

	/* Routing - this will be configurable in later version */

	/* GPR[0/1] = FX * 4 + SPDIF-in */

	emu_addefxop (lp, 4, 0x100, 0x12, 0, 0x44, &pc);
	emu_addefxop (lp, 4, 0x101, 0x13, 1, 0x44, &pc);

	/* GPR[0/1] += APS-input */

	emu_addefxop (lp, 6, 0x100, 0x100, 0x40, lp->l_APS ? 0x16 : 0x40, &pc);
	emu_addefxop (lp, 6, 0x101, 0x101, 0x40, lp->l_APS ? 0x17 : 0x40, &pc);

	/* FrontOut (AC97) = GPR[0/1] */

	emu_addefxop (lp, 6, 0x20, 0x40, 0x40, 0x100, &pc);
	emu_addefxop (lp, 6, 0x21, 0x40, 0x41, 0x101, &pc);

	/* RearOut = (GPR[0/1] * RearVolume) >> 31 */
	/*   RearVolume = GRP[0x10/0x11] */

	emu_addefxop (lp, 0, 0x28, 0x40, 0x110, 0x100, &pc);
	emu_addefxop (lp, 0, 0x29, 0x40, 0x111, 0x101, &pc);

	/* TOS out = GPR[0/1] */

	emu_addefxop (lp, 6, 0x22, 0x40, 0x40, 0x100, &pc);
	emu_addefxop (lp, 6, 0x23, 0x40, 0x40, 0x101, &pc);

	/* Mute Out2 */

	emu_addefxop (lp, 6, 0x24, 0x40, 0x40, 0x40, &pc);
	emu_addefxop (lp, 6, 0x25, 0x40, 0x40, 0x40, &pc);

	/* Mute Out3 */

	emu_addefxop (lp, 6, 0x26, 0x40, 0x40, 0x40, &pc);
	emu_addefxop (lp, 6, 0x27, 0x40, 0x40, 0x40, &pc);

	/* Input0 (AC97) -> Record */

	emu_addefxop (lp, 6, 0x2a, 0x40, 0x40, 0x10, &pc);
	emu_addefxop (lp, 6, 0x2b, 0x40, 0x40, 0x11, &pc);

	emu_wrptr (lp, 0, DBG, 0);

}	/* end live_initefx */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
void
liveopen (dev_t dev, int oflag)
{
	int		unit;
	LIVE		*lp;

	/*
	 *	Vê se a unidade é válida e está aberta
	 */
	if ((unsigned)(unit = MINOR (dev)) != 0)
		{ u.u_error = ENXIO; return; }

	lp = &live;

	if (!lp->l_present)
		{ u.u_error = ENXIO; return; }

	if (lp->l_open == 0)
		lp->l_open++;
	else
		u.u_error = EBUSY;

}	/* end liveopen */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
liveclose (dev_t dev, int flag)
{
	int		unit;
	LIVE		*lp;

	/*
	 *	Vê se a unidade é válida e está aberta
	 */
	if ((unsigned)(unit = MINOR (dev)) != 0)
		{ u.u_error = ENXIO; return; }

	lp = &live;

	if (!lp->l_present)
		{ u.u_error = ENXIO; return; }

	if (lp->l_open > 0)
		lp->l_open--;

}	/* end liveclose */

/*
 ****************************************************************
 *	Função  de "write"					*
 ****************************************************************
 */
void
livewrite (IOREQ *iop)
{
	LIVE		*lp = &live;
	VOICE		*vp = &voice[0];
	int		rate;

	/*
	 *	x
	 */
	if (iop->io_count > PGTOBY (MUSICPGSZ))
		{ u.u_error = E2BIG; return; }

	if (unimove (lp->l_music_page, iop->io_area, iop->io_count, US) < 0)
		{ u.u_error = EFAULT; return; }

#if (0)	/*******************************************************/
printf ("livewrite: Ponto 1, count = %d\n", iop->io_count);
printf ("Area[0] = %P, Area[32K] = %P\n", lp->l_music_page[0], lp->l_music_page[8191]);
#endif	/*******************************************************/

	iop->io_count = 0;

	/*
	 *	x
	 */
	vp->v_num	= 0;
	vp->v_speed	= 44100;
	vp->v_start	= PGTOBY (1);
	vp->v_end	= PGTOBY (MUSICPGSZ + 1);
	vp->v_vol	= 0xFF;
	vp->v_16	= 1;
	vp->v_stereo	= 1;
	vp->v_ismaster	= 1;
	vp->v_slave	= &voice[1];

	vp++;

	vp->v_num	= 1;
	vp->v_speed	= 44100;
	vp->v_start	= PGTOBY (1);
	vp->v_end	= PGTOBY (MUSICPGSZ + 1);
	vp->v_vol	= 0xFF;
	vp->v_16	= 1;
	vp->v_stereo	= 1;
	vp->v_ismaster	= 0;
	vp->v_slave	= NULL;

	/*
	 *	x
	 */
	emu_vwrite (lp, vp);

#if (0)	/*******************************************************/
printf ("livewrite: Ponto 2\n");
#endif	/*******************************************************/
	/*
	 *	Configura o despertador
	 */
	rate = vp->v_speed * 4 / PGSZ;

	if   (rate < 48)
		rate = 48;
	elif (rate > 9600)
		rate = 9600;

	lp->l_timerinterval = 48000 / rate;

#if (0)	/*******************************************************/
printf ("livewrite: Ponto 3\n");
lp->l_timerinterval = 0x03FF;
#endif	/*******************************************************/
	write_port_short (lp->l_timerinterval & 0x03FF, lp->l_port + TIMER);

#if (0)	/*******************************************************/
printf ("Master vol = %d\n", emu_rdcd (lp, AC97_MASTERVOLUME));
printf ("PCM    vol = %d\n", emu_rdcd (lp, AC97_PCMOUTVOLUME));

printf ("livewrite: Ponto 4\n");
#endif	/*******************************************************/
	emu_enatimer (lp, 1);

#if (0)	/*******************************************************/
printf ("livewrite: Ponto 5\n");
#endif	/*******************************************************/
	emu_vtrigger (lp, vp, 1);

#if (0)	/*******************************************************/
printf ("livewrite: Ponto 6\n");
#endif	/*******************************************************/

int_count = 10;

}	/* end livewrite */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_vwrite (LIVE *lp, VOICE *vp)
{
	int		s, l, r, x, y;
	int		sa, ea, start, val, silent_page;

	s = (vp->v_stereo ? 1 : 0) + (vp->v_16 ? 1 : 0);

	sa = vp->v_start >> s;
	ea = vp->v_end >> s;

	l = r = x = y = vp->v_vol;

	if (vp->v_stereo)
	{
		l = vp->v_ismaster ? l : 0;
		r = vp->v_ismaster ? 0 : r;
	}

	emu_wrptr (lp, vp->v_num, CPF, vp->v_stereo? CPF_STEREO_MASK : 0);

	val = vp->v_stereo? 28 : 30;
	val *= vp->v_16? 1 : 2;
	start = sa + val;

	emu_wrptr (lp, vp->v_num, FXRT, 0xd01c0000);

	emu_wrptr (lp, vp->v_num, PTRX, (x << 8) | r);
	emu_wrptr (lp, vp->v_num, DSL, ea | (y << 24));
	emu_wrptr (lp, vp->v_num, PSST, sa | (l << 24));
	emu_wrptr (lp, vp->v_num, CCCA, start | (vp->v_16? 0 : CCCA_8BITSELECT));

	emu_wrptr (lp, vp->v_num, Z1, 0);
	emu_wrptr (lp, vp->v_num, Z2, 0);

	silent_page = (VIRT_TO_PHYS_ADDR (lp->l_silent_page) << 1) | MAP_PTI_MASK;
	emu_wrptr (lp, vp->v_num, MAPA, silent_page);
	emu_wrptr (lp, vp->v_num, MAPB, silent_page);

	emu_wrptr (lp, vp->v_num, CVCF, CVCF_CURRENTFILTER_MASK);
	emu_wrptr (lp, vp->v_num, VTFT, VTFT_FILTERTARGET_MASK);
	emu_wrptr (lp, vp->v_num, ATKHLDM, 0);
	emu_wrptr (lp, vp->v_num, DCYSUSM, DCYSUSM_DECAYTIME_MASK);
	emu_wrptr (lp, vp->v_num, LFOVAL1, 0x8000);
	emu_wrptr (lp, vp->v_num, LFOVAL2, 0x8000);
	emu_wrptr (lp, vp->v_num, FMMOD, 0);
	emu_wrptr (lp, vp->v_num, TREMFRQ, 0);
	emu_wrptr (lp, vp->v_num, FM2FRQ2, 0);
	emu_wrptr (lp, vp->v_num, ENVVAL, 0x8000);

	emu_wrptr (lp, vp->v_num, ATKHLDV, ATKHLDV_HOLDTIME_MASK | ATKHLDV_ATTACKTIME_MASK);
	emu_wrptr (lp, vp->v_num, ENVVOL, 0x8000);

	emu_wrptr (lp, vp->v_num, PEFE_FILTERAMOUNT, 0x7f);
	emu_wrptr (lp, vp->v_num, PEFE_PITCHAMOUNT, 0);

	if (vp->v_slave != NULL)
		emu_vwrite (lp, vp->v_slave);

}	/* end emu_vwrite */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_vtrigger (LIVE *lp, VOICE *vp, int go)
{
	int	pitch_target, initial_pitch;
	int	cra, cs, ccis;
	int	sample, i;

	if (go)
	{
		cra = 64;
		cs = vp->v_stereo ? 4 : 2;
		ccis = vp->v_stereo ? 28 : 30;
		ccis *= vp->v_16 ? 1 : 2;
		sample = vp->v_16 ? 0x00000000 : 0x80808080;

		for (i = 0; i < cs; i++)
			emu_wrptr (lp, vp->v_num, CD0 + i, sample);

		emu_wrptr (lp, vp->v_num, CCR_CACHEINVALIDSIZE, 0);
		emu_wrptr (lp, vp->v_num, CCR_READADDRESS, cra);
		emu_wrptr (lp, vp->v_num, CCR_CACHEINVALIDSIZE, ccis);

		emu_wrptr (lp, vp->v_num, IFATN, 0xff00);
		emu_wrptr (lp, vp->v_num, VTFT, 0xffffffff);
		emu_wrptr (lp, vp->v_num, CVCF, 0xffffffff);
		emu_wrptr (lp, vp->v_num, DCYSUSV, 0x00007f7f);
		emu_enastop (lp, vp->v_num, 0);

		pitch_target = emu_rate_to_linearpitch(vp->v_speed);
		initial_pitch = emu_rate_to_pitch(vp->v_speed) >> 8;
		emu_wrptr (lp, vp->v_num, PTRX_PITCHTARGET, pitch_target);
		emu_wrptr (lp, vp->v_num, CPF_CURRENTPITCH, pitch_target);
		emu_wrptr (lp, vp->v_num, IP, initial_pitch);
	}
	else
	{
		emu_wrptr (lp, vp->v_num, PTRX_PITCHTARGET, 0);
		emu_wrptr (lp, vp->v_num, CPF_CURRENTPITCH, 0);
		emu_wrptr (lp, vp->v_num, IFATN, 0xffff);
		emu_wrptr (lp, vp->v_num, VTFT, 0x0000ffff);
		emu_wrptr (lp, vp->v_num, CVCF, 0x0000ffff);
		emu_wrptr (lp, vp->v_num, IP, 0);
		emu_enastop (lp, vp->v_num, 1);
	}

	if (vp->v_slave != NULL)
		emu_vtrigger(lp, vp->v_slave, go);

}	/* end emu_vtrigger */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_wrptr (const LIVE *lp, int chn, int reg, int data)
{
	int		ptr, mask, size, offset;

	ptr = ((reg << 16) & PTR_ADDRESS_MASK) | (chn & PTR_CHANNELNUM_MASK);

	write_port_long (ptr, lp->l_port + PTR);

	if (reg & 0xFF000000)
	{
		size = (reg >> 24) & 0x3F;
		offset = (reg >> 16) & 0x1F;
		mask = ((1 << size) - 1) << offset;
		data <<= offset;
		data &= mask;
		data |= read_port_long (lp->l_port + DATA) & ~mask;
	}

	write_port_long (data, lp->l_port + DATA);

}	/* end emu_wrptr */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
emu_rdcd (const LIVE *lp, int regno)
{
	write_port (regno, lp->l_port + AC97ADDRESS);
	return (read_port_short (lp->l_port + AC97DATA));

}	/* end emu_rdcd */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_wrcd (const LIVE *lp, int regno, int data)
{
	write_port (regno, lp->l_port + AC97ADDRESS);
	write_port_short (data, lp->l_port + AC97DATA);

}	/* end emu_wrcd */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_addefxop (const LIVE *lp, int op, int z, int w, int x, int y, int *pc)
{
	emu_wrptr (lp, 0, MICROCODEBASE + (*pc) * 2,	  (x << 10) | y);
	emu_wrptr (lp, 0, MICROCODEBASE + (*pc) * 2 + 1 , (op << 20) | (z << 10) | w);

	(*pc)++;

}	/* end emu_addefxop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
liveint (struct intr_frame frame)
{
	int		stat, x, ack;
	LIVE		*lp = &live;

#if (0)	/*******************************************************/
printf ("live INT\n");
#endif	/*******************************************************/

	for (EVER)
	{
		if ((stat = read_port_long (lp->l_port + IPR)) == 0)
			break;

		ack = 0;

		/* process irq */

		if (stat & IPR_INTERVALTIMER)
		{
			ack |= IPR_INTERVALTIMER;

			x = 0;

#if (0)	/*******************************************************/
			for (i = 0; i < EMU_CHANS; i++)
			{
				if (sc->pch[i].run)
				{
					x = 1;
					chn_intr(sc->pch[i].channel);
				}
			}
#endif	/*******************************************************/

if (--int_count < 0)
			if (x == 0)
				emu_enatimer (lp, 0);
		}


#if (0)	/*******************************************************/
		if (stat & (IPR_ADCBUFFULL | IPR_ADCBUFHALFFULL)) {
			ack |= stat & (IPR_ADCBUFFULL | IPR_ADCBUFHALFFULL);
			if (sc->rch[0].channel)
				chn_intr(sc->rch[0].channel);
		}
		if (stat & (IPR_EFXBUFFULL | IPR_EFXBUFHALFFULL)) {
			ack |= stat & (IPR_EFXBUFFULL | IPR_EFXBUFHALFFULL);
			if (sc->rch[1].channel)
				chn_intr(sc->rch[1].channel);
		}
		if (stat & (IPR_MICBUFFULL | IPR_MICBUFHALFFULL)) {
			ack |= stat & (IPR_MICBUFFULL | IPR_MICBUFHALFFULL);
			if (sc->rch[2].channel)
				chn_intr(sc->rch[2].channel);
		}
		if (stat & IPR_PCIERROR) {
			ack |= IPR_PCIERROR;
			device_printf(sc->dev, "pci error\n");
			/* we still get an nmi with ecc ram even if we ack this */
		}
		if (stat & IPR_SAMPLERATETRACKER) {
			ack |= IPR_SAMPLERATETRACKER;
			/* device_printf(sc->dev, "sample rate tracker lock status change\n"); */
		}

		if (stat & ~ack)
			device_printf(sc->dev, "dodgy irq: %x (harmless)\n", stat & ~ack);
#endif	/*******************************************************/

		write_port_long (stat, lp->l_port + IPR);
	}

}	/* end liveint */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
emu_enatimer (LIVE *lp, int go)
{
	int		x;

	if (go)
	{
		if (lp->l_timer++ == 0)
		{
			x =  read_port_long (lp->l_port + INTE);
			x |= INTE_INTERVALTIMERENB;
			write_port_long (x, lp->l_port + INTE);
		}
	}
	else
	{
		lp->l_timer = 0;

		x =  read_port_long (lp->l_port + INTE);
		x &= ~INTE_INTERVALTIMERENB;
		write_port_long (x, lp->l_port + INTE);
	}

	return (0);

}	/* end emu_enatimer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
emu_enastop (LIVE *lp, int channel, int enable)
{
	int		reg;

	reg = (channel & 0x20) ? SOLEH : SOLEL;
	channel &= 0x1F;
	reg |= 1 << 24;
	reg |= channel << 16;

	emu_wrptr (lp, 0, reg, enable);

}	/* end emu_enastop */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
emu_rate_to_pitch (int rate)
{
	static unsigned	logMagTable[128] =
	{
		0x00000, 0x02dfc, 0x05b9e, 0x088e6, 0x0b5d6, 0x0e26f, 0x10eb3, 0x13aa2,
		0x1663f, 0x1918a, 0x1bc84, 0x1e72e, 0x2118b, 0x23b9a, 0x2655d, 0x28ed5,
		0x2b803, 0x2e0e8, 0x30985, 0x331db, 0x359eb, 0x381b6, 0x3a93d, 0x3d081,
		0x3f782, 0x41e42, 0x444c1, 0x46b01, 0x49101, 0x4b6c4, 0x4dc49, 0x50191,
		0x5269e, 0x54b6f, 0x57006, 0x59463, 0x5b888, 0x5dc74, 0x60029, 0x623a7,
		0x646ee, 0x66a00, 0x68cdd, 0x6af86, 0x6d1fa, 0x6f43c, 0x7164b, 0x73829,
		0x759d4, 0x77b4f, 0x79c9a, 0x7bdb5, 0x7dea1, 0x7ff5e, 0x81fed, 0x8404e,
		0x86082, 0x88089, 0x8a064, 0x8c014, 0x8df98, 0x8fef1, 0x91e20, 0x93d26,
		0x95c01, 0x97ab4, 0x9993e, 0x9b79f, 0x9d5d9, 0x9f3ec, 0xa11d8, 0xa2f9d,
		0xa4d3c, 0xa6ab5, 0xa8808, 0xaa537, 0xac241, 0xadf26, 0xafbe7, 0xb1885,
		0xb3500, 0xb5157, 0xb6d8c, 0xb899f, 0xba58f, 0xbc15e, 0xbdd0c, 0xbf899,
		0xc1404, 0xc2f50, 0xc4a7b, 0xc6587, 0xc8073, 0xc9b3f, 0xcb5ed, 0xcd07c,
		0xceaec, 0xd053f, 0xd1f73, 0xd398a, 0xd5384, 0xd6d60, 0xd8720, 0xda0c3,
		0xdba4a, 0xdd3b4, 0xded03, 0xe0636, 0xe1f4e, 0xe384a, 0xe512c, 0xe69f3,
		0xe829f, 0xe9b31, 0xeb3a9, 0xecc08, 0xee44c, 0xefc78, 0xf148a, 0xf2c83,
		0xf4463, 0xf5c2a, 0xf73da, 0xf8b71, 0xfa2f0, 0xfba57, 0xfd1a7, 0xfe8df
	};

	static char	logSlopeTable[128] =
	{
		0x5c, 0x5c, 0x5b, 0x5a, 0x5a, 0x59, 0x58, 0x58,
		0x57, 0x56, 0x56, 0x55, 0x55, 0x54, 0x53, 0x53,
		0x52, 0x52, 0x51, 0x51, 0x50, 0x50, 0x4f, 0x4f,
		0x4e, 0x4d, 0x4d, 0x4d, 0x4c, 0x4c, 0x4b, 0x4b,
		0x4a, 0x4a, 0x49, 0x49, 0x48, 0x48, 0x47, 0x47,
		0x47, 0x46, 0x46, 0x45, 0x45, 0x45, 0x44, 0x44,
		0x43, 0x43, 0x43, 0x42, 0x42, 0x42, 0x41, 0x41,
		0x41, 0x40, 0x40, 0x40, 0x3f, 0x3f, 0x3f, 0x3e,
		0x3e, 0x3e, 0x3d, 0x3d, 0x3d, 0x3c, 0x3c, 0x3c,
		0x3b, 0x3b, 0x3b, 0x3b, 0x3a, 0x3a, 0x3a, 0x39,
		0x39, 0x39, 0x39, 0x38, 0x38, 0x38, 0x38, 0x37,
		0x37, 0x37, 0x37, 0x36, 0x36, 0x36, 0x36, 0x35,
		0x35, 0x35, 0x35, 0x34, 0x34, 0x34, 0x34, 0x34,
		0x33, 0x33, 0x33, 0x33, 0x32, 0x32, 0x32, 0x32,
		0x32, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30,
		0x30, 0x30, 0x30, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f
	};

	int i;

	if (rate == 0)				/* Bail out if no leading "1" */
		return 0;

	rate *= 11185;	/* Scale 48000 to 0x20002380 */

	for (i = 31; i > 0; i--)
	{
		if (rate & 0x80000000)		/* Detect leading "1" */
		{
			return
			(	((unsigned)(i - 15) << 20) + logMagTable[0x7f & (rate >> 24)] +
				(0x7f & (rate >> 17)) * logSlopeTable[0x7f & (rate >> 24)]
			);
		}

		rate <<= 1;
	}

	return 0;		/* Should never reach this point */

}	/* end emu_rate_to_pitch */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
unsigned
emu_rate_to_linearpitch (unsigned rate)
{
	rate = (rate << 8) / 375;
	return (rate >> 1) + (rate & 1);

}	/* end emu_rate_to_linearpitch */

