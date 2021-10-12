/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/ati/atiprobe.c,v 1.73 2006/03/19 19:06:35 tsi Exp $ */
/*
 * Copyright 1997 through 2006 by Marc Aurele La France (TSI @ UQV), tsi@xfree86.org
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Marc Aurele La France not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Marc Aurele La France makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as-is" without express or implied warranty.
 *
 * MARC AURELE LA FRANCE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL MARC AURELE LA FRANCE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "ati.h"
#include "atiadapter.h"
#include "atiadjust.h"
#include "atibus.h"
#include "atichip.h"
#include "aticonsole.h"
#include "atiident.h"
#include "atimach64io.h"
#include "atimodule.h"
#include "atipreinit.h"
#include "atiprobe.h"
#include "atiscreen.h"
#include "ativalid.h"
#include "ativersion.h"
#include "atividmem.h"
#include "atiwonderio.h"

#include "radeon_probe.h"
#include "radeon_version.h"
#include "r128_probe.h"
#include "r128_version.h"

/*
 * NOTES:
 *
 * - The driver private structures (ATIRec's) are allocated here, rather than
 *   in ATIPreInit().  This allows ATIProbe() to pass information to later
 *   stages.
 * - A minor point, perhaps, is that XF86Config Chipset names denote functional
 *   levels, rather than specific graphics controller chips.
 * - ATIProbe() does not call xf86MatchPciInstances(), because ATIProbe()
 *   should be able to match a mix of PCI and non-PCI devices to XF86Config
 *   Device sections.  Also, PCI configuration space for Mach32's is to be
 *   largely ignored.
 */

#ifdef XFree86LOADER

/*
 * The following exists to prevent the compiler from considering entry points
 * defined in a separate module from being constants.
 */
static xf86PreInitProc     * const volatile PreInitProc     = ATIPreInit;
static xf86ScreenInitProc  * const volatile ScreenInitProc  = ATIScreenInit;
static xf86SwitchModeProc  * const volatile SwitchModeProc  = ATISwitchMode;
static xf86AdjustFrameProc * const volatile AdjustFrameProc = ATIAdjustFrame;
static xf86EnterVTProc     * const volatile EnterVTProc     = ATIEnterVT;
static xf86LeaveVTProc     * const volatile LeaveVTProc     = ATILeaveVT;
static xf86FreeScreenProc  * const volatile FreeScreenProc  = ATIFreeScreen;
static xf86ValidModeProc   * const volatile ValidModeProc   = ATIValidMode;

#define ATIPreInit     PreInitProc
#define ATIScreenInit  ScreenInitProc
#define ATISwitchMode  SwitchModeProc
#define ATIAdjustFrame AdjustFrameProc
#define ATIEnterVT     EnterVTProc
#define ATILeaveVT     LeaveVTProc
#define ATIFreeScreen  FreeScreenProc
#define ATIValidMode   ValidModeProc

#endif

/* Used as a temporary buffer */
#define Identifier ((char *)(pATI->MMIOCache))

/*
 * An internal structure definition to facilitate the matching of detected
 * adapters to XF86Config Device sections.
 */
typedef struct _ATIGDev
{
    GDevPtr pGDev;
    int     iATIPtr;
    CARD8   Chipset;
} ATIGDev, *ATIGDevPtr;

/*
 * Definitions for I/O conflict avoidance.
 */
#define LongPort(_Port) GetBits(_Port, PCIGETIO(SPARSE_IO_BASE))
#define DomainSize      (LongPort(SPARSE_IO_BASE) + 1)
#define Allowed         (1 << 0)
#define DoProbe         (1 << 1)
#define DetectedVGA     (0 << 2)
#define Detected8514A   (1 << 2)
#define DetectedMach64  (2 << 2)
#define Conflict        (3 << 2)
#define BadRouting      (4 << 2)
typedef struct
{
    int    Domain;
    CARD16 Base;
    CARD8  Size;
    CARD8  Flag;
} PortRec, *PortPtr;

/* BIOS definitions */
static const CARD8 ATISignature[] = " 761295520";
static const CARD8 IBMSignature[] = "IBM";
#define ATISize   10
#define ATIOffset 0x30U
#define IBMSize   3
#define IBMOffset 0x1EU

#define PrefixSize    0x50U
#define BIOSWord(_n)  (BIOS[_n] | (BIOS[(_n) + 1] << 8))

/*
 * ATIScanPCIBases --
 *
 * This function loops though a device's PCI registered bases and accumulates
 * a list of block I/O bases in use in the system.  Of these bases, only those
 * that are, or may have, ISA aliases are of interest.
 */
static void
ATIScanPCIBases
(
    PortPtr      *PCIPorts,
    int          *nPCIPort,
    const int     Domain,
    const CARD32 *pBase,
    const int    *pSize,
    const CARD8  ProbeFlag
)
{
    CARD16 Base;
    int    i, j;

    for (i = 6;  --i >= 0;  pBase++, pSize++)
    {
        if (*pBase & PCI_MAP_IO)
        {
            if (*pBase > (CARD16)(-1))
                continue;

            Base = *pBase & ~IO_BYTE_SELECT;
            for (j = 0;  ;  j++)
            {
                if (j >= *nPCIPort)
                {
                    (*nPCIPort)++;
                    *PCIPorts = (PortPtr)xnfrealloc(*PCIPorts,
                        *nPCIPort * SizeOf(PortRec));
                    (*PCIPorts)[j].Domain = Domain;
                    (*PCIPorts)[j].Base = Base;
                    (*PCIPorts)[j].Size = (CARD8)*pSize;
                    (*PCIPorts)[j].Flag = ProbeFlag;
                    break;
                }

                if ((Domain == (*PCIPorts)[j].Domain) &&
                    (Base == (*PCIPorts)[j].Base))
                    break;
            }

            continue;
        }

        /* Allow for 64-bit addresses */
        if (!PCI_MAP_IS64BITMEM(*pBase))
            continue;

        i--;
        pBase++;
        pSize++;
    }
}

/*
 * ATICheckSparseIOBases --
 *
 * This function checks whether a sparse I/O base can safely be probed.
 */
static CARD8
ATICheckSparseIOBases
(
    pciVideoPtr  pVideo,
    const CARD8  *ProbeFlags,
    const int    Domain,
    const CARD16 IOBase,
    const int    Count,
    const Bool   Override
)
{
    CARD16 FirstPort, LastPort;
    CARD8  Result = DoProbe;

    if (!pVideo || !xf86IsPrimaryPci(pVideo))
    {
        int domain = Domain * DomainSize;

        FirstPort = LongPort(IOBase) + domain;
        LastPort  = LongPort(IOBase + Count - 1) + domain;

        for (;  FirstPort <= LastPort;  FirstPort++)
        {
            CARD8 ProbeFlag = ProbeFlags[FirstPort];

            if (ProbeFlag & DoProbe)
                continue;

            if (!(ProbeFlag & Allowed))
                return ProbeFlag;

            if (Override || (Result != DoProbe))
                continue;

            /* User might wish to override this decision */
            xf86Msg(X_WARNING,
                ATI_NAME ":  Sparse I/O base 0x%04X not probed in domain"
                " %d.\n", IOBase, Domain);
            Result = Allowed;
        }
    }

    return Result;
}

/*
 * ATISetSparseIOBases --
 *
 * This function updates the sparse I/O base table with information from the
 * hardware probes.
 */
static void
ATISetSparseIOBases
(
    CARD8        *ProbeFlags,
    const int    Domain,
    const CARD16 IOBase,
    const int    Count,
    const CARD8  ProbeFlag
)
{
    int    domain = Domain * DomainSize;
    CARD16 FirstPort = LongPort(IOBase) + domain,
           LastPort  = LongPort(IOBase + Count - 1) + domain;

    for (;  FirstPort <= LastPort;  FirstPort++)
        if ((ProbeFlag != Allowed) || (ProbeFlags[FirstPort] & Allowed))
            ProbeFlags[FirstPort] = ProbeFlag;
}

/*
 * ATIValidateVGAWonderBIOS --
 *
 * This function retrieves a VGAWonder port number from the first few bytes of
 * a video BIOS.
 */
static Bool
ATIValidateVGAWonderBIOS
(
    ATIPtr      pATI,
    const CARD8 *BIOS
)
{
    if ((BIOS[0x00U] == 0x55U) && (BIOS[0x01U] == 0xAAU) &&
        !(BIOS[0x10] & 0x01U) &&
        !memcmp(BIOS + IBMOffset, IBMSignature, IBMSize) &&
        !memcmp(BIOS + ATIOffset, ATISignature, ATISize) &&
        (pATI->CPIO_VGAWonder = (BIOSWord(0x10U) & SPARSE_IO_PORT)))
        return TRUE;

    return FALSE;
}

/*
 * ATIVGAProbe --
 *
 * This function looks for an IBM standard VGA, or clone, and sets
 * pATI->VGAAdapter if one is found.
 */
static ATIPtr
ATIVGAProbe
(
    ATIPtr          pATI,
    const IOADDRESS DomainIOBase,
    const int       Domain
)
{
    CARD8 IOValue1, IOValue2, IOValue3;

    if (!pATI)
    {
        pATI = (ATIPtr)xnfcalloc(1, SizeOf(ATIRec));
        pATI->Domain = Domain;
        pATI->DomainIOBase = DomainIOBase;
    }

    /*
     * VGA has one more attribute register than EGA.  See if it can be read and
     * written.  Note that the CRTC registers are not used here, so there's no
     * need to unlock them.
     */
    ATISetVGAIOBase(pATI, inb(R_GENMO));

    xf86InterceptSignals(&pATI->CaughtSignal);
    (void)inb(GENS1(pATI->CPIO_VGABase));
    IOValue1 = inb(ATTRX);
    (void)inb(GENS1(pATI->CPIO_VGABase));
    IOValue2 = GetReg(ATTRX, 0x14U | 0x20U);
    outb(ATTRX, IOValue2 ^ 0x0FU);
    IOValue3 = GetReg(ATTRX, 0x14U | 0x20U);
    outb(ATTRX, IOValue2);
    outb(ATTRX, IOValue1);
    (void)inb(GENS1(pATI->CPIO_VGABase));
    xf86InterceptSignals(NULL);

    if (IOValue3 == (IOValue2 ^ 0x0FU))
    {
        /* VGA device detected */
        if (pATI->Chip == ATI_CHIP_NONE)
            pATI->Chip = ATI_CHIP_VGA;
        if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
            pATI->VGAAdapter = ATI_ADAPTER_VGA;
        if (pATI->Adapter == ATI_ADAPTER_NONE)
            pATI->Adapter = ATI_ADAPTER_VGA;
    }
    else
    {
        pATI->VGAAdapter = ATI_ADAPTER_NONE;
    }

    return pATI;
}

/*
 * ATIVGAWonderProbe --
 *
 * This function determines if ATI extended VGA registers can be accessed
 * through the I/O port specified by pATI->CPIO_VGAWonder.  If not, the
 * function resets pATI->CPIO_VGAWonder to zero.
 */
static void
ATIVGAWonderProbe
(
    pciVideoPtr pVideo,
    ATIPtr      pATI,
    ATIPtr      p8514,
    CARD8       *ProbeFlags,
    Bool        *DontProbe,
    const int   Domain
)
{
    CARD8 IOValue1, IOValue2, IOValue3, IOValue4, IOValue5, IOValue6;

    switch (ATICheckSparseIOBases(pVideo, ProbeFlags, Domain,
        pATI->CPIO_VGAWonder, 2, TRUE))
    {
        case BadRouting:
            xf86Msg(X_WARNING,
                ATI_NAME ":  Expected VGA Wonder capability could not be"
                " detected at I/O port 0x%04lX in domain %d due to unsuitable"
                " PCI routing.\n", pATI->CPIO_VGAWonder, Domain);
            pATI->CPIO_VGAWonder = 0;
            break;

        case Conflict:
            xf86Msg(X_WARNING,
                ATI_NAME ":  Expected VGA Wonder capability could not be"
                " detected at I/O port 0x%04lX in domain %d because it would"
                " conflict with a non-video PCI/AGP device.\n",
                pATI->CPIO_VGAWonder, Domain);
            pATI->CPIO_VGAWonder = 0;
            break;

        case Detected8514A:
            xf86Msg(X_WARNING,
                ATI_NAME ":  Expected VGA Wonder capability could not be"
                " detected at I/O port 0x%04lX in domain %d because it would"
                " conflict with a %s %s.\n", pATI->CPIO_VGAWonder, Domain,
                ATIBusNames[p8514->BusType], ATIAdapterNames[p8514->Adapter]);
            pATI->CPIO_VGAWonder = 0;
            break;

        case DetectedMach64:
            xf86Msg(X_WARNING,
                ATI_NAME ":  Expected VGA Wonder capability could not be"
                " detected at I/O port 0x%04lX in domain %d because it would"
                " conflict with a Mach64.\n", pATI->CPIO_VGAWonder, Domain);
            pATI->CPIO_VGAWonder = 0;
            break;

        case DetectedVGA:
        default:                /* Must be DoProbe */
            if (!xf86CheckPciSparseIO(Domain, pATI->CPIO_VGAWonder, 2,
                                      (int)0xFFFF03FF, DontProbe))
            {
                xf86MsgVerb(X_INFO, 2,
                    ATI_NAME ":  Expected VGA Wonder capability could not be"
                    " detected at I/O port 0x%04lX in domain %d due to"
                    " unsuitable PCI routing.\n",
                    pATI->CPIO_VGAWonder, Domain);

                if (DontProbe)
                    ATISetSparseIOBases(ProbeFlags, Domain,
                        pATI->CPIO_VGAWonder, 2, BadRouting);

                pATI->CPIO_VGAWonder = 0;
                break;
            }

            if (DontProbe && *DontProbe)
            {
                xf86MsgVerb(X_INFO, 2,
                    ATI_NAME ":  Expected VGA Wonder capability could not be"
                    " detected at I/O port 0x%04lX in domain %d due to a"
                    " potential hard-failed master abort.\n",
                    pATI->CPIO_VGAWonder, Domain);

                ATISetSparseIOBases(ProbeFlags, Domain, pATI->CPIO_VGAWonder,
                    2, Allowed);

                pATI->CPIO_VGAWonder = 0;
                break;
            }

            if (pVideo && !xf86IsPrimaryPci(pVideo) &&
                (pATI->Chip <= ATI_CHIP_88800GXD))
            {
                /* Set up extended VGA register addressing */
                PutReg(GRAX, 0x50U, GetByte(pATI->CPIO_VGAWonder, 0));
                PutReg(GRAX, 0x51U,
                    GetByte(pATI->CPIO_VGAWonder, 1) | pATI->VGAOffset);
            }

            /*
             * Register 0xBB is used by the BIOS to keep track of various
             * things (monitor type, etc.).  Except for 18800-x's, register
             * 0xBC must be zero and causes the adapter to enter a test mode
             * when written to with a non-zero value.
             */
            IOValue1 = inb(pATI->CPIO_VGAWonder + pATI->DomainIOBase);
            IOValue2 = ATIGetExtReg(IOValue1);
            IOValue3 = ATIGetExtReg(0xBBU);
            ATIPutExtReg(0xBBU, IOValue3 ^ 0xAAU);
            IOValue4 = ATIGetExtReg(0xBBU);
            ATIPutExtReg(0xBBU, IOValue3 ^ 0x55U);
            IOValue5 = ATIGetExtReg(0xBBU);
            ATIPutExtReg(0xBBU, IOValue3);
            if (pATI->Chip <= ATI_CHIP_18800_1)
                IOValue6 = 0;
            else
                IOValue6 = ATIGetExtReg(0xBCU);
            ATIPutExtReg(IOValue1, IOValue2);

            if ((IOValue4 == (IOValue3 ^ 0xAAU)) &&
                (IOValue5 == (IOValue3 ^ 0x55U)) &&
                (IOValue6 == 0))
            {
                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  VGA Wonder at I/O port 0x%04lX detected in"
                    " domain %d.\n", pATI->CPIO_VGAWonder, Domain);
            }
            else
            {
                xf86Msg(X_WARNING,
                    ATI_NAME ":  Expected VGA Wonder capability at I/O port"
                    " 0x%04lX in domain %d was not detected.\n",
                    pATI->CPIO_VGAWonder, Domain);
                pATI->CPIO_VGAWonder = 0;
            }

            break;
    }
}

/*
 * ATI8514Probe --
 *
 * This function looks for an 8514/A compatible and returns an ATIRec if one is
 * found.  The function also determines whether or not the detected 8514/A
 * compatible device is actually a Mach8 or Mach32, and sets pATI->Adapter
 * accordingly.
 */
static ATIPtr
ATI8514Probe
(
    pciVideoPtr     pVideo,
    const IOADDRESS DomainIOBase,
    const int       Domain
)
{
    ATIPtr       pATI = (ATIPtr)xnfcalloc(1, SizeOf(ATIRec));
    pciConfigPtr pPCI;
    CARD16       IOValue1, IOValue2;
    CARD8        BIOS[PrefixSize];

    pATI->Domain = Domain;
    pATI->DomainIOBase = DomainIOBase;

    /*
     * Save register value to be modified, just in case there is no 8514/A
     * compatible accelerator.  Note that, in more ways than one,
     * SUBSYS_STAT == SUBSYS_CNTL.
     */
    IOValue1 = inw(SUBSYS_STAT);
    IOValue2 = IOValue1 & _8PLANE;

    /* Reset any 8514/A compatible adapter that might be present */
    outw(SUBSYS_CNTL, IOValue2 | (GPCTRL_RESET | CHPTEST_NORMAL));
    outw(SUBSYS_CNTL, IOValue2 | (GPCTRL_ENAB | CHPTEST_NORMAL |
        RVBLNKFLG | RPICKFLAG | RINVALIDIO | RGPIDLE));

    /* Probe for an 8514/A compatible */
    IOValue2 = inw(ERR_TERM);
    outw(ERR_TERM, 0x5A5AU);
    ProbeWaitIdleEmpty();
    if (inw(ERR_TERM) == 0x5A5AU)
    {
        outw(ERR_TERM, 0x2525U);
        if (inw(ERR_TERM) == 0x2525U)
        {
            pATI->Adapter = ATI_ADAPTER_8514A;
            pATI->ChipHasSUBSYS_CNTL = TRUE;
            pATI->PCIInfo = pVideo;
        }
    }
    outw(ERR_TERM, IOValue2);

    /* Restore register value clobbered by 8514/A reset attempt */
    if (!pATI->ChipHasSUBSYS_CNTL)
    {
        outw(SUBSYS_CNTL, IOValue1);
        xfree(pATI);
        return NULL;
    }

    /* Ensure any Mach8 or Mach32 is not in 8514/A emulation mode */
    IOValue1 = inw(CLOCK_SEL);
    outw(CLOCK_SEL, IOValue1);
    ProbeWaitIdleEmpty();

    IOValue1 = IOValue2 = inw(ROM_ADDR_1);
    outw(ROM_ADDR_1, 0x5555U);
    ProbeWaitIdleEmpty();
    if (inw(ROM_ADDR_1) == 0x5555U)
    {
        outw(ROM_ADDR_1, 0x2A2AU);
        ProbeWaitIdleEmpty();
        if (inw(ROM_ADDR_1) == 0x2A2AU)
            pATI->Adapter = ATI_ADAPTER_MACH8;
    }
    outw(ROM_ADDR_1, IOValue1);

    if (pATI->Adapter == ATI_ADAPTER_MACH8)
    {
        /* A Mach8 or Mach32 has been detected */
        IOValue1 = inw(READ_SRC_X);
        outw(DESTX_DIASTP, 0xAAAAU);
        ProbeWaitIdleEmpty();
        if (inw(READ_SRC_X) == 0x02AAU)
            pATI->Adapter = ATI_ADAPTER_MACH32;

        outw(DESTX_DIASTP, 0x5555U);
        ProbeWaitIdleEmpty();
        if (inw(READ_SRC_X) == 0x0555U)
        {
            if (pATI->Adapter != ATI_ADAPTER_MACH32)
                pATI->Adapter = ATI_ADAPTER_8514A;
        }
        else
        {
            if (pATI->Adapter != ATI_ADAPTER_MACH8)
                pATI->Adapter = ATI_ADAPTER_8514A;
        }
        outw(DESTX_DIASTP, IOValue1);
    }

    switch (pATI->Adapter)
    {
        case ATI_ADAPTER_8514A:
            pATI->Coprocessor = ATI_CHIP_8514A;
            IOValue1 = inb(EXT_CONFIG_3);
            outb(EXT_CONFIG_3, IOValue1 & 0x0FU);
            if (!(inb(EXT_CONFIG_3) & 0xF0U))
            {
                outb(EXT_CONFIG_3, IOValue1 | 0xF0U);
                if ((inb(EXT_CONFIG_3) & 0xF0U) == 0xF0U)
                    pATI->Coprocessor = ATI_CHIP_CT480;
            }
            outb(EXT_CONFIG_3, IOValue1);
            break;

        case ATI_ADAPTER_MACH8:
            pATI->Coprocessor = ATI_CHIP_38800_1;
            if (inw(CONFIG_STATUS_1) & MC_BUS)
                pATI->BusType = ATI_BUS_MCA16;
            break;

        case ATI_ADAPTER_MACH32:
            IOValue1 = inw(CONFIG_STATUS_1);
            pATI->BusType = GetBits(IOValue1, BUS_TYPE);
            pATI->BIOSBase = 0x000C0000U +
                (GetBits(IOValue2, BIOS_BASE_SEGMENT) << 11);
            do
            {
                if (IOValue1 & (_8514_ONLY | CHIP_DIS))
                    break;

                pATI->VGAAdapter = ATI_ADAPTER_MACH32;
                pATI->VGAOffset = 0x80U;

                /*
                 * Pick up the VGA Wonder port number from the BIOS.  For a PCI
                 * adapter whose initialised BIOS is either non-existent or not
                 * accessible, use its PCI ROM.
                 */
                if (xf86DomainHasBIOSSegments(Domain) &&
                    (xf86ReadBIOS(pATI->BIOSBase, 0, BIOS, SizeOf(BIOS)) ==
                     SizeOf(BIOS)) && ATIValidateVGAWonderBIOS(pATI, BIOS))
                    break;

                if (pVideo && (pPCI = pVideo->thisCard) &&
                    (xf86ReadPciBIOS(0, pPCI->tag, 0, BIOS, SizeOf(BIOS)) ==
                     SizeOf(BIOS)) && ATIValidateVGAWonderBIOS(pATI, BIOS))
                    break;

                pATI->CPIO_VGAWonder = 0x01CEU;
            } while (0);

            ATIMach32ChipID(pATI);
            break;

        default:
            break;
    }

    return pATI;
}

/*
 * ATIMach64Detect --
 *
 * This function determines if a Mach64 is detectable at a particular base
 * address.
 */
static Bool
ATIMach64Detect
(
    ATIPtr            pATI,
    const CARD16      ChipType,
    const ATIChipType Chip
)
{
    CARD32 IOValue, bus_cntl, gen_test_cntl;

    (void)ATIMapApertures(-1, pATI);    /* Ignore errors */

    if ((pATI->IODecoding == MEMORY_IO) && !pATI->pBlock[0])
    {
        ATIUnmapApertures(-1, pATI);
        return FALSE;
    }

    /* Make sure any Mach64 is not in some weird state */
    bus_cntl = inr(BUS_CNTL);
    if (Chip < ATI_CHIP_264VTB)
        outr(BUS_CNTL,
             (bus_cntl & ~(BUS_HOST_ERR_INT_EN | BUS_FIFO_ERR_INT_EN)) |
             (BUS_HOST_ERR_INT | BUS_FIFO_ERR_INT));
    else if (Chip < ATI_CHIP_264VT4)
        outr(BUS_CNTL, (bus_cntl & ~BUS_HOST_ERR_INT_EN) | BUS_HOST_ERR_INT);

    gen_test_cntl = inr(GEN_TEST_CNTL);
    IOValue = gen_test_cntl &
        (GEN_OVR_OUTPUT_EN | GEN_OVR_POLARITY | GEN_CUR_EN | GEN_BLOCK_WR_EN);
    outr(GEN_TEST_CNTL, IOValue | GEN_GUI_EN);
    outr(GEN_TEST_CNTL, IOValue);
    outr(GEN_TEST_CNTL, IOValue | GEN_GUI_EN);

    /* See if a Mach64 answers */
    IOValue = inr(SCRATCH_REG0);

    /* Test odd bits */
    outr(SCRATCH_REG0, 0x55555555U);
    if (inr(SCRATCH_REG0) == 0x55555555U)
    {
        /* Test even bits */
        outr(SCRATCH_REG0, 0xAAAAAAAAU);
        if (inr(SCRATCH_REG0) == 0xAAAAAAAAU)
        {
            /*
             * *Something* has a R/W 32-bit register at this address.  Try to
             * make sure it's a Mach64.  The following assumes that ATI will
             * not be producing any more adapters that do not register
             * themselves in PCI configuration space.
             */
            ATIMach64ChipID(pATI, ChipType);
            if ((pATI->Chip != ATI_CHIP_Mach64) ||
                (pATI->IODecoding == BLOCK_IO))
                pATI->Adapter = ATI_ADAPTER_MACH64;
        }
    }

    /* Restore clobbered register value */
    outr(SCRATCH_REG0, IOValue);

    /* If no Mach64 was detected, return now */
    if (pATI->Adapter != ATI_ADAPTER_MACH64)
    {
        outr(GEN_TEST_CNTL, gen_test_cntl);
        if (Chip < ATI_CHIP_264VT4)
            outr(BUS_CNTL, bus_cntl);
        ATIUnmapApertures(-1, pATI);
        return FALSE;
    }

    /* Determine legacy BIOS address */
    pATI->BIOSBase = 0x000C0000U +
        (GetBits(inr(SCRATCH_REG1), BIOS_BASE_SEGMENT) << 11);

    /*
     * Determine VGA capability.  VGA can always be enabled on integrated
     * controllers.  For the GX/CX, it's a board strap.
     */
    if (pATI->Chip >= ATI_CHIP_264CT)
    {
        pATI->VGAAdapter = ATI_ADAPTER_MACH64;
    }
    else
    {
        IOValue = inr(CONFIG_STATUS64_0);
        pATI->BusType = GetBits(IOValue, CFG_BUS_TYPE);
        IOValue &= (CFG_VGA_EN | CFG_CHIP_EN);
        if (pATI->Chip == ATI_CHIP_88800CX)
            IOValue |= CFG_VGA_EN;
        if (IOValue == (CFG_VGA_EN | CFG_CHIP_EN))
        {
            pATI->VGAAdapter = ATI_ADAPTER_MACH64;
            pATI->CPIO_VGAWonder = 0x01CEU;
            pATI->VGAOffset = 0x80U;
        }
    }

    ATIUnmapApertures(-1, pATI);
    return TRUE;
}

/*
 * ATIMach64Probe --
 *
 * This function looks for a Mach64 at a particular I/O or memory address and
 * returns an ATIRec if one is found.
 */
static ATIPtr
ATIMach64Probe
(
    pciVideoPtr       pVideo,
    IOADDRESS         IOBase,
    const IOADDRESS   DomainIOBase,
    const int         Domain,
    CARD8             IODecoding,
    const ATIChipType Chip
)
{
    ATIPtr pATI = (ATIPtr)xnfcalloc(1, SizeOf(ATIRec));
    CARD16 ChipType = 0;

    if (!IOBase)        /* Stupid OS ... */
        IODecoding = MEMORY_IO;

    if (pVideo)
    {
        if ((IODecoding == BLOCK_IO) &&
            ((pVideo->size[1] < 8) ||
             (IOBase >= (CARD32)(-1 << pVideo->size[1]))))
        {
            IOBase = 0;
            IODecoding = MEMORY_IO;
        }

        ChipType = pVideo->chipType;
    }

    if (IOBase)
        IOBase += DomainIOBase;

    pATI->Domain = Domain;
    pATI->DomainIOBase = DomainIOBase;
    pATI->CPIOBase = IOBase;
    pATI->IODecoding = IODecoding;
    pATI->PCIInfo = pVideo;

    if (IODecoding == MEMORY_IO)
    {
        if (pVideo)
        {
            /*
             * Probe through auxiliary MMIO aperture if one exists.  Because
             * such apertures can be enabled/disabled only through PCI, this
             * probes no further.
             */
            if ((pVideo->size[2] >= 12) &&
                (pATI->Block0Base = pVideo->memBase[2]) &&
                (pATI->Block0Base < (CARD32)(-1 << pVideo->size[2])))
            {
                pATI->Block0Base += 0x00000400U;
                goto LastProbe;
            }

            /*
             * Probe through the primary MMIO aperture that exists at the tail
             * end of the linear aperture.  Test for both 8MB and 4MB linear
             * apertures.
             */
            if ((pVideo->size[0] >= 22) &&
                (pATI->Block0Base = pVideo->memBase[0]))
            {
                pATI->Block0Base += 0x007FFC00U;
                if ((pVideo->size[0] >= 23) &&
                    ATIMach64Detect(pATI, ChipType, Chip))
                    return pATI;

                pATI->Block0Base -= 0x00400000U;
                if (ATIMach64Detect(pATI, ChipType, Chip))
                    return pATI;
            }
        }

        /* A last, perhaps desparate, probe attempt */
        pATI->Block0Base = 0x000BFC00U;
    }

LastProbe:
    if (ATIMach64Detect(pATI, ChipType, Chip))
        return pATI;

    xfree(pATI);
    return NULL;
}

/*
 * ATIAssignVGA --
 *
 * This function is called to associate a VGA interface with an accelerator.
 * This is done by temporarily configuring the accelerator to route VGA RAMDAC
 * I/O through the accelerator's RAMDAC.  A value is then written through the
 * VGA DAC ports and a check is made to see if the same value shows up on the
 * accelerator side.
 */
static void
ATIAssignVGA
(
    pciVideoPtr pVideo,
    ATIPtr      *ppVGA,
    ATIPtr      pATI,
    ATIPtr      p8514,
    CARD8       *ProbeFlags,
    Bool        *DontProbe,
    const int   Domain
)
{
    ATIPtr pVGA = *ppVGA;
    CARD8  OldDACMask;

    /* Assume unassignable VGA */
    pATI->VGAAdapter = ATI_ADAPTER_NONE;

    /* If no assignable VGA, return now */
    if ((pATI != pVGA) && (!pVGA || (pVGA->Adapter > ATI_ADAPTER_VGA)))
        return;

    (void)ATIMapApertures(-1, pATI);
    if ((pATI->IODecoding == MEMORY_IO) && !pATI->pBlock[0])
    {
        ATIUnmapApertures(-1, pATI);
        return;
    }

    switch (pATI->Adapter)
    {
        case ATI_ADAPTER_8514A:
            {
                /*
                 * Assumption:  Bit DISABPASSTHRU in ADVFUNC_CNTL is already
                 *              off.
                 */
                OldDACMask = inb(VGA_DAC_MASK);

                if (inb(IBM_DAC_MASK) == OldDACMask)
                {
                    outb(VGA_DAC_MASK, 0xA5U);
                    if (inb(IBM_DAC_MASK) == 0xA5U)
                        pATI->VGAAdapter = ATI_ADAPTER_VGA;
                    outb(VGA_DAC_MASK, OldDACMask);
                }
            }
            break;

        case ATI_ADAPTER_MACH8:
            {
                CARD16 ClockSel = inw(CLOCK_SEL);

                if (ClockSel & DISABPASSTHRU)
                    outw(CLOCK_SEL, ClockSel & ~DISABPASSTHRU);

                ProbeWaitIdleEmpty();

                OldDACMask = inb(VGA_DAC_MASK);

                if (inb(IBM_DAC_MASK) == OldDACMask)
                {
                    outb(VGA_DAC_MASK, 0xA5U);
                    if (inb(IBM_DAC_MASK) == 0xA5U)
                        pATI->VGAAdapter = ATI_ADAPTER_VGA;
                    outb(VGA_DAC_MASK, OldDACMask);
                }

                if (ClockSel & DISABPASSTHRU)
                    outw(CLOCK_SEL, ClockSel);
            }
            break;

        case ATI_ADAPTER_MACH32:
            {
                CARD16 ClockSel    = inw(CLOCK_SEL),
                       MiscOptions = inw(MISC_OPTIONS);

                if (ClockSel & DISABPASSTHRU)
                    outw(CLOCK_SEL, ClockSel & ~DISABPASSTHRU);
                if (MiscOptions & (DISABLE_VGA | DISABLE_DAC))
                    outw(MISC_OPTIONS,
                        MiscOptions & ~(DISABLE_VGA | DISABLE_DAC));

                ProbeWaitIdleEmpty();

                OldDACMask = inb(VGA_DAC_MASK);

                if (inb(IBM_DAC_MASK) == OldDACMask)
                {
                    outb(VGA_DAC_MASK, 0xA5U);
                    if (inb(IBM_DAC_MASK) == 0xA5U)
                        pATI->VGAAdapter = ATI_ADAPTER_MACH32;
                    outb(VGA_DAC_MASK, OldDACMask);
                }

                if (ClockSel & DISABPASSTHRU)
                    outw(CLOCK_SEL, ClockSel);
                if (MiscOptions & (DISABLE_VGA | DISABLE_DAC))
                    outw(MISC_OPTIONS, MiscOptions);
            }
            break;

        case ATI_ADAPTER_MACH64:
            {
                CARD32 DACCntl = inr(DAC_CNTL);

                if (!(DACCntl & DAC_VGA_ADR_EN))
                    outr(DAC_CNTL, DACCntl | DAC_VGA_ADR_EN);

                OldDACMask = inb(VGA_DAC_MASK);

                if (in8(M64_DAC_MASK) == OldDACMask)
                {
                    outb(VGA_DAC_MASK, 0xA5U);
                    if (in8(M64_DAC_MASK) == 0xA5U)
                        pATI->VGAAdapter = ATI_ADAPTER_MACH64;
                    outb(VGA_DAC_MASK, OldDACMask);
                }

                if (!(DACCntl & DAC_VGA_ADR_EN))
                    outr(DAC_CNTL, DACCntl);
            }
            break;

        default:
            break;
    }

    ATIUnmapApertures(-1, pATI);

    if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
        return;

    if (pATI->CPIO_VGAWonder)
    {
        ATIVGAWonderProbe(pVideo, pATI, p8514, ProbeFlags, DontProbe, Domain);
        if (!pATI->CPIO_VGAWonder)
        {
            /*
             * Some adapters are reputed to append ATI extended VGA registers
             * to the VGA Graphics controller registers.  In particular, 0x01CE
             * cannot, in general, be used in a PCI environment due to routing
             * of I/O through the bus tree.
             */
            pATI->CPIO_VGAWonder = 0x03CEU;
            ATIVGAWonderProbe(pVideo, pATI, p8514, ProbeFlags, DontProbe,
                Domain);
        }
    }

    if (pATI == pVGA)
    {
        pATI->SharedVGA = TRUE;
        return;
    }

    /* Assign the VGA to this adapter */
    xfree(pVGA);
    *ppVGA = pATI;

    xf86MsgVerb(X_INFO, 3, ATI_NAME ":  VGA assigned to this adapter.\n");
}

/*
 * ATIClaimVGA --
 *
 * Attempt to assign a non-shareable VGA to an accelerator.  If successful,
 * update ProbeFlags array.
 */
static void
ATIClaimVGA
(
    pciVideoPtr pVideo,
    ATIPtr      *ppVGA,
    ATIPtr      pATI,
    ATIPtr      p8514,
    CARD8       *ProbeFlags,
    Bool        *DontProbe,
    const int   Domain,
    const int   Detected
)
{
    ATIAssignVGA(pVideo, ppVGA, pATI, p8514, ProbeFlags, DontProbe, Domain);
    if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
        return;

    ATISetSparseIOBases(ProbeFlags, Domain, MonochromeIOBase, 48, Detected);
    if (!pATI->CPIO_VGAWonder)
        return;

    ATISetSparseIOBases(ProbeFlags, Domain, pATI->CPIO_VGAWonder, 2, Detected);
}

/*
 * ATIFindVGA --
 *
 * This function determines if a VGA associated with an ATI PCI adapter is
 * shareable.
 */
static void
ATIFindVGA
(
    pciVideoPtr pVideo,
    ATIPtr      *ppVGA,
    ATIPtr      *ppATI,
    ATIPtr      p8514,
    CARD8       *ProbeFlags,
    const int   Domain
)
{
    ATIPtr pATI = *ppATI;

    if (!*ppVGA)
    {
        /*
         * An ATI PCI adapter has been detected at this point, and its VGA, if
         * any, is shareable.  Ensure the VGA isn't in sleep mode.  Note that
         * it's possible GENENA (0x46E8) and/or GENVS (0x0102) are not routed
         * to the adapter, potentially causing a hang.  But then again, video
         * BIOS initialisation would have caused (or, in ATIPreInit(), will
         * cause) the same.
         */
        if (inb(GENENB) != 0x01U)
        {
            outb(GENENA, 0x16U);
            outb(GENVS, 0x01U);
            outb(GENENA, 0x0EU);
        }

        pATI = ATIVGAProbe(pATI, 0, Domain);
        if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
        {
            xf86Msg(X_WARNING,
                ATI_NAME ":  VGA not detected on this adapter.\n");
            return;
        }

        ppVGA = ppATI;
    }

    ATIAssignVGA(pVideo, ppVGA, pATI, p8514, ProbeFlags, NULL, Domain);
}

/*
 * ATIProbe --
 *
 * This function is called once, at the start of the first server generation to
 * do a minimal probe for supported hardware.
 */
Bool
ATIProbe
(
    DriverPtr pDriver,
    int       flags
)
{
#ifdef XFree86LOADER
    ModuleDescPtr       pModule;
#endif
    ATIPtr              pATI, *ATIPtrs = NULL;
    ATIPtr              *ppVGA = NULL, *pp8514 = NULL, *ppMach64 = NULL;
    GDevPtr             *GDevs, pGDev;
    pciVideoPtr         pVideo, *xf86PciVideoInfo = xf86GetPciVideoInfo();
    pciConfigPtr        pPCI, *xf86PciInfo = xf86GetPciConfigInfo();
    ATIGDev             *ATIGDevs = NULL, *pATIGDev;
    ScrnInfoPtr         pScreenInfo;
    IOADDRESS           *pDomainIOBase = NULL;
    PortPtr             PCIPorts = NULL;
    CARD8               *ProbeFlags = NULL;
    int                 ProbeSize;
    int                 nPCIPort = 0;
    int                 Domain, MaxDomain = 0;
    int                 i, j, k;
    int                 nGDev, nATIGDev = -1, nATIPtr = 0;
    int                 Chipset;
    Bool                DontProbe = FALSE;
    Bool                ProbeSuccess = FALSE;
    Bool                DoRage128 = FALSE, DoRadeon = FALSE;
    CARD32              PciReg;
    ATIChipType         Chip;
    ATIIODecodingType   IODecoding;
    static const CARD16 Mach64SparseIOBases[] = {0x02ECU, 0x01CCU, 0x01C8U};
    CARD16              Mach64SparseIOBase;
    CARD8               fChipsets[ATI_CHIPSET_MAX];
    CARD8               ProbeFlag;

    unsigned long       BIOSBase;
    CARD8               BIOS[PrefixSize];

#   define              AddAdapter(_p)                                     \
    do                                                                     \
    {                                                                      \
        nATIPtr++;                                                         \
        ATIPtrs = (ATIPtr *)xnfrealloc(ATIPtrs, SizeOf(ATIPtr) * nATIPtr); \
        ATIPtrs[nATIPtr - 1] = (_p);                                       \
        (_p)->iEntity = -2;                                                \
    } while (0)

    if ((ATIEndian.endian != ATI_LITTLE_ENDIAN) &&
        (ATIEndian.endian != ATI_BIG_ENDIAN))
    {
        xf86Msg(X_ERROR, ATI_NAME ":  Unsupported endianness:  0x%08X.\n",
            ATIEndian.endian);
        return FALSE;
    }

    (void)memset(fChipsets, FALSE, SizeOf(fChipsets));

    if (!(flags & PROBE_DETECT))
    {
        /*
         * Get a list of XF86Config device sections whose "Driver" is either
         * not specified, or specified as this driver.  From this list,
         * eliminate those device sections that specify a "Chipset" not
         * recognised by the driver.  Those device sections that specify a
         * "ChipRev" without a "ChipID" are also weeded out.
         */
        nATIGDev = 0;
        if ((nGDev = xf86MatchDevice(ATI_NAME, &GDevs)) > 0)
        {
            ATIGDevs = (ATIGDevPtr)xnfcalloc(nGDev, SizeOf(ATIGDev));

            for (i = 0, pATIGDev = ATIGDevs;  i < nGDev;  i++)
            {
                pGDev = GDevs[i];
                Chipset = ATIIdentProbe(pGDev->chipset);
                if (Chipset == -1)
                    continue;

                if ((pGDev->chipID > (int)((CARD16)(-1))) ||
                    (pGDev->chipRev > (int)((CARD8)(-1))))
                    continue;

                if ((pGDev->chipID < 0) && (pGDev->chipRev >= 0))
                    continue;

                pATIGDev->pGDev = pGDev;
                pATIGDev->Chipset = Chipset;
                nATIGDev++;
                pATIGDev++;

                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  Candidate \"Device\" section \"%s\".\n",
                    pGDev->identifier);

                fChipsets[Chipset] = TRUE;
            }

            xfree(GDevs);

            if (!nATIGDev)
            {
                xfree(ATIGDevs);
                ATIGDevs = NULL;
            }
        }

        if (xf86MatchDevice(R128_NAME, NULL) > 0)
            DoRage128 = TRUE;
        if (xf86MatchDevice(RADEON_NAME, NULL) > 0)
            DoRadeon = TRUE;
    }

    /*
     * Collect hardware information.
     *
     * First, scan PCI configuration space for registered I/O ports (which will
     * be block I/O bases).  Each such port is used to generate a list of
     * sparse I/O bases it precludes.  This list is then used to decide whether
     * or not certain sparse I/O probes are done.  These probes can be forced
     * in certain circumstances when an appropriate chipset specification is
     * used in any XF86Config Device section.
     *
     * Note that this is not bullet-proof.  Lockups can still occur, but they
     * will usually be due to devices that are misconfigured to respond to the
     * same I/O ports as 8514/A's or ATI sparse I/O devices without registering
     * them in PCI configuration space.
     */
    if (nATIGDev)
    {
        if (xf86PciVideoInfo)
        {
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if (!(pPCI = pVideo->thisCard))
                    continue;

                Domain = xf86GetPciDomain(pPCI->tag);
                if (Domain > MaxDomain)
                    MaxDomain = Domain;

                if ((pVideo->vendor == PCI_VENDOR_ATI))
                    continue;

                ATIScanPCIBases(&PCIPorts, &nPCIPort,
                    Domain, &pPCI->pci_base0, pVideo->size,
                    (pciReadLong(pPCI->tag, PCI_CMD_STAT_REG) &
                     PCI_CMD_IO_ENABLE) ? 0 : Allowed);
            }
        }

        /* Check non-video PCI devices for I/O bases */
        if (xf86PciInfo)
        {
            for (i = 0;  (pPCI = xf86PciInfo[i++]);  )
            {
                Domain = xf86GetPciDomain(pPCI->tag);
                if (Domain > MaxDomain)
                    MaxDomain = Domain;

                if ((pPCI->pci_vendor == PCI_VENDOR_ATI) ||
                    (pPCI->pci_base_class == PCI_CLASS_BRIDGE) ||
                    (pPCI->pci_header_type &
                      ~GetByte(PCI_HEADER_MULTIFUNCTION, 2)))
                    continue;

                ATIScanPCIBases(&PCIPorts, &nPCIPort,
                    Domain, &pPCI->pci_base0, pPCI->basesize,
                    (pciReadLong(pPCI->tag, PCI_CMD_STAT_REG) &
                     PCI_CMD_IO_ENABLE) ? Conflict : Allowed);
            }
        }

        MaxDomain++;

        /* Generate ProbeFlags array from list of registered PCI I/O bases */
        ProbeSize = MaxDomain * DomainSize;
        ProbeFlags = xnfalloc(ProbeSize);
        (void)memset(ProbeFlags, Allowed | DoProbe, ProbeSize);
        for (i = 0;  i < nPCIPort;  i++)
        {
            CARD32 Base = PCIPorts[i].Base;
            CARD16 Count = (1 << PCIPorts[i].Size) - 1;

            Domain = PCIPorts[i].Domain;
            ProbeFlag = PCIPorts[i].Flag;

            /*
             * The following reduction of Count is based on the assumption that
             * PCI-registered I/O port ranges do not overlap.
             */
            for (j = 0;  j < nPCIPort;  j++)
            {
                CARD32 Base2 = PCIPorts[j].Base;

                if ((Base < Base2) && (Domain == PCIPorts[j].Domain))
                    while ((Base + Count) >= Base2)
                        Count >>= 1;
            }

            Base = LongPort(Base) + (PCIPorts[i].Domain * DomainSize);
            Count = LongPort((Count | IO_BYTE_SELECT) + 1);
            while (Count--)
                ProbeFlags[Base++] &= ProbeFlag;
        }

        xfree(PCIPorts);

        /* Allocate various sets of ATIPtr pointers for background adapters */
        ppVGA = xnfcalloc(MaxDomain, SizeOf(*ppVGA));
        pp8514 = xnfcalloc(MaxDomain, SizeOf(*pp8514));
        ppMach64 = xnfcalloc(MaxDomain * NumberOf(Mach64SparseIOBases),
            SizeOf(*ppMach64));

        /* Ensure I/O is mapped in for all domains */
        pDomainIOBase = xnfcalloc(MaxDomain, SizeOf(*pDomainIOBase));

        if (xf86PciInfo)
        {
            for (i = 0, j = -1;  (pPCI = xf86PciInfo[i++]);  )
            {
                Domain = xf86GetPciDomain(pPCI->tag);
                if (Domain == j)
                    continue;

                j = Domain;
                pDomainIOBase[j] =
                    xf86MapDomainIO(-1, VIDMEM_MMIO, pPCI->tag, 0, 0x10000);
            }
        }

        /* Combine VGA chipset overrides into one */
        fChipsets[ATI_CHIPSET_IBMVGA] |=
            fChipsets[ATI_CHIPSET_ATIVGA] | fChipsets[ATI_CHIPSET_VGAWONDER];

        /* Ditto for the 8514/A ones */
        fChipsets[ATI_CHIPSET_IBM8514] |=
            fChipsets[ATI_CHIPSET_MACH8] | fChipsets[ATI_CHIPSET_MACH32];

        /*
         * A few notes on probe strategy.  I/O and memory response by certain
         * PCI devices has been disabled by the common layer at this point,
         * including any devices this driver might be interested in.  The
         * following does sparse I/O probes, followed by block I/O probes.
         * Block I/O probes are dictated by what is found to be of interest in
         * PCI configuration space.  All this will detect ATI adapters that do
         * not implement this disablement, pre-PCI or not.
         *
         * PCI configuration space is then scanned again for ATI devices that
         * failed to be detected the first time around.  Each such device is
         * probed for again, this time with I/O temporarily enabled through
         * PCI.
         *
         * It's possible that even this second PCI scan fails to detect an
         * adapter.  So far, two different causes have been observed:
         *
         * 1) The resources the adapter registers in PCI configuration space
         *    are not actually being routed to the adapter.  Barring OS and/or
         *    BIOS/firmware bugs, this is usually due to a prior X server crash
         *    that failed to properly restore PCI routing, in which case a
         *    power-down/reboot will normally rectify the problem.
         *
         * 2) The Linux kernel's atyfb can mis-guess the adapter's reference
         *    clock, so befuddling the adapter as to cause intermittent and/or
         *    incorrect response.  This is more likely to happen for adapters
         *    that have not been initialised through the system's BIOS or
         *    firmware.
         *
         * The rationale for all of this is as follows.
         *
         * First off, at the C source level, this driver is to be as OS-neutral
         * and architecture-neutral as possible.  Indeed, even endianness can
         * be accomodated at run-time.  This means that any lobotomies (of PCI
         * in particular) that occur on various architectures and OS'es are to
         * be dealt with in the common layer, not here.  For example, the
         * alleged lack of ISA support on some architectures is _not_ a concern
         * to be addressed here.  Among other benefits, this policy aids and
         * abets other drivers in dealing with the very same system-level
         * idioms.
         *
         * Secondly, the driver is obviously interested in non-PCI ATI adapters
         * that might exist in the system.  Sparse I/O probes are done to
         * detect such adapters.  On PCI systems, master aborts will be
         * generated should a probed-for adapter not exist.
         *
         * Third, a small number of ATI PCI adapters are known to claim
         * transactions aimed at them whether or not they are disabled through
         * the standard PCI mechanism.  The driver needs to be aware of such
         * anomalies so that it can correctly participate in the common layer's
         * resource tracking scheme.  Given no reliable list of affected
         * adapters is available, the driver tests all ATI PCI Mach32's and
         * Mach64's for this behaviour.  Thus, master aborts will be generated
         * for that majority of tested adapters that actually comply with PCI
         * disablement.
         *
         * Due to a PCI loophole, some systems hard-fail certain master aborts,
         * causing hangs, crashes or other more aberrant behaviours.  Before
         * each sparse I/O probe or PCI disablement test, the driver determines
         * the likelihood of hard-failed master aborts.  In this determination,
         * the driver, with the common layer's help, leverages the fact that
         *
         * 1) the common layer disables the hard-failing of master aborts on
         *    non-root PCI bus segments;  and
         * 2) the presence of a PCI-to-ISA, or similar, bridge on a PCI bus
         *    segment acts to prevent master aborts on that same bus segment.
         *
         * The driver skips a probe or test (with a message) if it determines
         * that a master abort on a PCI root segment cannot be completely
         * avoided.
         *
         * Sparse I/O probes are also skipped when the common layer helper
         * functions determine that the sparse I/O range is being routed to
         * more than one PCI bus segment.  In such a situation, any adapter
         * that might, by chance, respond to the probe, would be unusable
         * except perhaps as a VGA.
         *
         * Conflicts between sparse I/O ranges and pre-existing resource
         * allocations (which cause a probe to be skipped) are dealt with here,
         * rather than in the common layer, because the latter does not
         * currently provide a mechanism generic enough to handle the
         * ATI-specific probes this driver must perform.  Furthermore, by the
         * time ATIProbe() is called, the common layer has long ago forgotten
         * the distinction between active and inactive resource allocations.
         *
         * It might be said that this driver does not trust PCI as much as
         * other drivers do.  There is some truth to that, but the reality is
         * this driver's mission includes dealing with observed behaviours that
         * are not codified in, or not compliant with, the PCI specifications.
         */
        for (Domain = 0;  Domain < MaxDomain;  Domain++)
        {
            do  /* Probe for background VGA */
            {
                ProbeFlag = ATICheckSparseIOBases(NULL, ProbeFlags, Domain,
                    MonochromeIOBase, 48, fChipsets[ATI_CHIPSET_IBMVGA]);

                if (ProbeFlag != DoProbe)
                {
                    if (ProbeFlag == BadRouting)
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared VGA not probed in domain %d"
                            " due to unsuitable PCI routing.\n", Domain);
                    else
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared VGA not probed in domain %d"
                            " due to an I/O conflict.\n", Domain);
                    break;
                }

                if (!xf86CheckPciSparseIO(Domain, MonochromeIOBase, 48,
                                          (int)0xFFFF03FF, &DontProbe))
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared VGA not probed in domain %d due"
                        " to unsuitable PCI routing.\n", Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain, MonochromeIOBase,
                        48, BadRouting);
                    break;
                }

                if (DontProbe && !fChipsets[ATI_CHIPSET_IBMVGA])
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared VGA not probed in domain %d due"
                        " to a potential hard-failed master abort.\n", Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain, MonochromeIOBase,
                        48, Allowed);
                    break;
                }

                pATI = ATIVGAProbe(NULL, pDomainIOBase[Domain], Domain);
                if (pATI->Adapter == ATI_ADAPTER_NONE)
                {
                    xfree(pATI);

                    xf86MsgVerb(X_INFO, 4,
                        ATI_NAME ":  Unshared VGA not detected in domain"
                        " %d.\n", Domain);
                    break;
                }

                /*
                 * Claim all MDA/HGA/CGA/EGA/VGA I/O ports.  This might need to
                 * be more selective.
                 */
                ATISetSparseIOBases(ProbeFlags, Domain, MonochromeIOBase, 48,
                    DetectedVGA);

                ppVGA[Domain] = pATI;
                strcpy(Identifier, "Unshared VGA");
                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  %s detected in domain %d.\n",
                    Identifier, Domain);
            } while (0);

            do  /* Probe for background 8514/A */
            {
                ProbeFlag = ATICheckSparseIOBases(NULL, ProbeFlags, Domain,
                    0x02E8U, 8, fChipsets[ATI_CHIPSET_IBM8514]);

                if (ProbeFlag != DoProbe)
                {
                    if (ProbeFlag == BadRouting)
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared 8514/A not probed in domain"
                            " %d due to unsuitable PCI routing.\n", Domain);
                    else
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared 8514/A not probed in domain"
                            " %d due to an I/O conflict.\n", Domain);
                    break;
                }

                if (!xf86CheckPciSparseIO(Domain, 0x02E8U, 8,
                                          (int)0xFFFF03FF, &DontProbe))
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared 8514/A not probed in domain %d"
                        " due to unsuitable PCI routing.\n", Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain, 0x02E8U, 8,
                        BadRouting);
                    break;
                }

                if (DontProbe && !fChipsets[ATI_CHIPSET_IBM8514])
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared 8514/A not probed in domain %d"
                        " due to a potential hard-failed master abort.\n",
                        Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain, 0x02E8U, 8,
                        Allowed);
                    break;
                }

                if ((pATI = ATI8514Probe(NULL, pDomainIOBase[Domain], Domain)))
                {
                    strcpy(Identifier, "Unshared 8514/A");
                    xf86MsgVerb(X_INFO, 3,
                        ATI_NAME ":  %s detected in domain %d.\n",
                        Identifier, Domain);

                    AddAdapter(pp8514[Domain] = pATI);

                    if ((pATI->VGAAdapter != ATI_ADAPTER_NONE) ||
                        (pATI->Coprocessor != ATI_CHIP_NONE))
                        ATIClaimVGA(NULL, &ppVGA[Domain], pATI, pATI,
                            ProbeFlags, &DontProbe, Domain, Detected8514A);

                    ATISetSparseIOBases(ProbeFlags, Domain, 0x02E8U, 8,
                        Detected8514A);
                    break;
                }

                xf86MsgVerb(X_INFO, 4,
                    ATI_NAME ":  Unshared 8514/A not detected in domain %d.\n",
                    Domain);
            } while (0);

            /* Probe for background Mach64's */
            for (i = 0;  i < NumberOf(Mach64SparseIOBases);  i++)
            {
                ProbeFlag = ATICheckSparseIOBases(NULL, ProbeFlags, Domain,
                    Mach64SparseIOBases[i], 4, fChipsets[ATI_CHIPSET_MACH64]);

                if (ProbeFlag != DoProbe)
                {
                    if (ProbeFlag == BadRouting)
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared Mach64 at I/O base 0x%04X"
                            " not probed in domain %d due to unsuitable PCI"
                            " routing.\n", Mach64SparseIOBases[i], Domain);
                    else
                        xf86MsgVerb(X_INFO, 2,
                            ATI_NAME ":  Unshared Mach64 at I/O base 0x%04X"
                            " not probed in domain %d due to I/O conflict.\n",
                            Mach64SparseIOBases[i], Domain);
                    continue;
                }

                if (!xf86CheckPciSparseIO(Domain, Mach64SparseIOBases[i], 4,
                                          (int)0xFFFF03FF, &DontProbe))
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared Mach64 at I/O base 0x%04X not"
                        " probed in domain %d due to unsuitable PCI"
                        " routing.\n", Mach64SparseIOBases[i], Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain,
                        Mach64SparseIOBases[i], 4, BadRouting);
                    continue;
                }

                if (DontProbe && !fChipsets[ATI_CHIPSET_MACH64])
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Unshared Mach64 at I/O base 0x%04X not"
                        " probed in domain %d due to a potential master"
                        " abort.\n", Mach64SparseIOBases[i], Domain);

                    ATISetSparseIOBases(ProbeFlags, Domain,
                        Mach64SparseIOBases[i], 4, Allowed);
                    continue;
                }

                /*
                 * The first 3D Rage (the GT) appears to have been the last
                 * Mach64 to support sparse I/O.
                 */
                pATI = ATIMach64Probe(NULL, Mach64SparseIOBases[i],
                    pDomainIOBase[Domain], Domain, SPARSE_IO, ATI_CHIP_264GT);
                if (!pATI)
                {
                    xf86MsgVerb(X_INFO, 4,
                        ATI_NAME ":  Unshared Mach64 at I/O base 0x%04X not"
                        " detected in domain %d.\n",
                        Mach64SparseIOBases[i], Domain);
                    continue;
                }

                sprintf(Identifier,
                    "Unshared Mach64 at sparse I/O base 0x%04X",
                    Mach64SparseIOBases[i]);
                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  %s detected in domain %d.\n",
                    Identifier, Domain);

                AddAdapter(ppMach64[(Domain * 3) + i] = pATI);

                if (pATI->VGAAdapter != ATI_ADAPTER_NONE)
                    ATIClaimVGA(NULL, &ppVGA[Domain], pATI, pp8514[Domain],
                        ProbeFlags, &DontProbe, Domain, DetectedMach64);

                ATISetSparseIOBases(ProbeFlags, Domain, Mach64SparseIOBases[i],
                    4, DetectedMach64);
            }
        }
    }

    if (xf86PciVideoInfo)
    {
        if (nATIGDev)
        {
#ifdef __NOT_YET__
            /*
             * Code similar to the next block will probably be needed for PCI
             * Mach32's when the driver starts using their MEMORY_IO space.
             */
#endif

            /*
             * Mach64 PCI sparse I/O adapters can still be used through
             * MEMORY_IO.
             */
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if ((pVideo->vendor != PCI_VENDOR_ATI) ||
                    (pVideo->chipType == PCI_CHIP_MACH32) ||
                    pVideo->size[1] ||
                    !(pPCI = pVideo->thisCard))
                    continue;

                /*
                 * Possibly fix block I/O and GENENA decoding indicators in PCI
                 * configuration space.
                 */
                PciReg = pciReadLong(pPCI->tag, PCI_REG_USERCONFIG);
                if (PciReg & 0x0000000CU)
                    pciWriteLong(pPCI->tag, PCI_REG_USERCONFIG,
                        PciReg & ~0x0000000CU);

                Domain = xf86GetPciDomain(pPCI->tag);
                Mach64SparseIOBase = 0;
                j = PciReg & 0x03U;
                if (j != 0x03U)
                {
                    Mach64SparseIOBase = Mach64SparseIOBases[j];

                    switch (ATICheckSparseIOBases(pVideo, ProbeFlags, Domain,
                                Mach64SparseIOBase, 4, FALSE))
                    {
                        case Detected8514A:
                            xf86MsgVerb(X_WARNING, 1,
                                ATI_NAME ":  Disabled PCI Mach64 in slot"
                                " %d:%d:%d not probed due to a conflict with"
                                " an %s in domain %d.\n",
                                pVideo->bus, pVideo->device, pVideo->func,
                                ATIAdapterNames[pp8514[Domain]->Adapter],
                                Domain);
                            continue;

                        case DetectedMach64:
                            /* Already detected? */
                            pATI = ppMach64[(Domain * 3) + j];
                            if (pATI->BusType >= ATI_BUS_PCI)
                                pATI->PCIInfo = pVideo;
                            else
                                xf86MsgVerb(X_WARNING, 1,
                                    ATI_NAME ":  Disabled PCI Mach64 in slot"
                                    " %d:%d:%d not probed due to a conflict"
                                    " with another %s Mach64 at sparse I/O"
                                    " base 0x%04X in domain %d.\n",
                                    pVideo->bus, pVideo->device, pVideo->func,
                                    ATIBusNames[pATI->BusType],
                                    Mach64SparseIOBase, Domain);
                            continue;

                        case DoProbe:
                            /* Already probed (unsuccessfully) */
                            continue;

                        default:
                            /* Some kind of I/O problem */
                            break;
                    }
                }

                if (!xf86CheckPciVideo(pVideo, &DontProbe))
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Disabled PCI Mach64 in slot %d:%d:%d not"
                        " probed in domain %d due to unsuitable PCI"
                        " routing.\n",
                        pVideo->bus, pVideo->device, pVideo->func, Domain);
                    continue;
                }

                if (DontProbe)
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Disabled PCI Mach64 in slot %d:%d:%d not"
                        " probed in domain %d due to a potential hard-failed"
                        " master abort.\n",
                        pVideo->bus, pVideo->device, pVideo->func, Domain);
                    continue;
                }

                Chip = ATIChipID(pVideo->chipType, pVideo->chipRev);

                pATI = ATIMach64Probe(pVideo, Mach64SparseIOBase,
                     pDomainIOBase[Domain], Domain, MEMORY_IO, Chip);
                if (!pATI)
                    continue;

                sprintf(Identifier,
                    "Unshared PCI sparse I/O Mach64 in slot %d:%d:%d",
                    pVideo->bus, pVideo->device, pVideo->func);
                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  %s detected through Block 0 at 0x%08lX in"
                    " domain %d.\n", Identifier, pATI->Block0Base, Domain);
                AddAdapter(pATI);

                /* This is probably not necessary */
                if (pATI->VGAAdapter != ATI_ADAPTER_NONE)
                    ATIClaimVGA(pVideo, &ppVGA[Domain], pATI, pp8514[Domain],
                        ProbeFlags, &DontProbe, Domain, DetectedMach64);

                if (j != 0x03U)
                {
                    ppMach64[(Domain * 3) + j] = pATI;

                    ATISetSparseIOBases(ProbeFlags, Domain, Mach64SparseIOBase,
                        4, DetectedMach64);
                }
            }

            /* Probe for Mach64 PCI block I/O devices */
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if ((pVideo->vendor != PCI_VENDOR_ATI) ||
                    (pVideo->chipType == PCI_CHIP_MACH32) ||
                    !pVideo->size[1] ||
                    !(pPCI = pVideo->thisCard))
                    continue;

                /* For now, ignore Rage128's and Radeon's */
                Chip = ATIChipID(pVideo->chipType, pVideo->chipRev);
                if (Chip > ATI_CHIP_Mach64)
                    continue;

                /*
                 * Possibly fix block I/O and GENENA decoding indicators in PCI
                 * configuration space.
                 */
                PciReg = pciReadLong(pPCI->tag, PCI_REG_USERCONFIG);
                if ((PciReg & 0x00000008U) ||
                    ((Chip < ATI_CHIP_264VTB) && !(PciReg & 0x00000004U)))
                    pciWriteLong(pPCI->tag, PCI_REG_USERCONFIG,
                        (PciReg & ~0x00000008U) | 0x00000004U);

                Domain = xf86GetPciDomain(pPCI->tag);

                if (!xf86CheckPciVideo(pVideo, &DontProbe))
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Disabled PCI/AGP Mach64 in slot %d:%d:%d"
                        " not probed in domain %d due to unsuitable PCI"
                        " routing.\n",
                        pVideo->bus, pVideo->device, pVideo->func, Domain);
                    continue;
                }

                if (DontProbe)
                {
                    xf86MsgVerb(X_INFO, 2,
                        ATI_NAME ":  Disabled PCI/AGP Mach64 in slot %d:%d:%d"
                        " not probed in domain %d due to a potential"
                        " hard-failed master abort.\n",
                        pVideo->bus, pVideo->device, pVideo->func, Domain);
                    continue;
                }

                pATI = ATIMach64Probe(pVideo, pVideo->ioBase[1],
                    pDomainIOBase[Domain], Domain, BLOCK_IO, Chip);
                if (!pATI)
                    continue;

                sprintf(Identifier, "Unshared PCI/AGP Mach64 in slot %d:%d:%d",
                    pVideo->bus, pVideo->device, pVideo->func);
                xf86MsgVerb(X_INFO, 3,
                    ATI_NAME ":  %s detected in domain %d.\n",
                    Identifier, Domain);
                AddAdapter(pATI);

                /* This is probably not necessary */
                if (pATI->VGAAdapter != ATI_ADAPTER_NONE)
                    ATIClaimVGA(pVideo, &ppVGA[Domain], pATI, pp8514[Domain],
                        ProbeFlags, &DontProbe, Domain, DetectedMach64);
            }

            /*
             * This is the second pass through PCI configuration space.  Much
             * of this is verbiage to deal with potential situations that are
             * very unlikely to occur in practice.
             *
             * First, look for non-ATI shareable VGA's.
             */
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if ((pVideo->vendor == PCI_VENDOR_ATI) ||
                    !(pPCI = pVideo->thisCard))
                    continue;

                switch (pVideo->class)
                {
                    case PCI_CLASS_PREHISTORIC:
                        if (pVideo->subclass == PCI_SUBCLASS_PREHISTORIC_VGA)
                            break;
                        continue;

                    case PCI_CLASS_DISPLAY:
                        if (pVideo->subclass == PCI_SUBCLASS_DISPLAY_VGA)
                            break;
                        continue;

                    default:
                        continue;
                }

                if (!xf86CheckPciSlot(pVideo->bus,
                                      pVideo->device,
                                      pVideo->func))
                    continue;

                Domain = xf86GetPciDomain(pPCI->tag);
                if (ATICheckSparseIOBases(pVideo, ProbeFlags, Domain,
                         MonochromeIOBase, 48, TRUE) != DoProbe)
                    continue;

                xf86SetPciVideo(pVideo, MEM_IO);

                pATI = ATIVGAProbe(NULL, pDomainIOBase[Domain], Domain);
                if (pATI->Adapter == ATI_ADAPTER_NONE)
                {
                    xfree(pATI);
                    xf86MsgVerb(X_WARNING, 0,
                        ATI_NAME ":  PCI/AGP VGA compatible in slot %d:%d:%d"
                        " could not be detected!\n",
                        pVideo->bus, pVideo->device, pVideo->func);
                }
                else
                {
                    sprintf(Identifier,
                        "Shared non-ATI VGA in PCI/AGP slot %d:%d:%d",
                        pVideo->bus, pVideo->device, pVideo->func);
                    xf86MsgVerb(X_INFO, 3, ATI_NAME ":  %s detected.\n",
                        Identifier);
                    AddAdapter(pATI);
                    pATI->SharedVGA = TRUE;
                    pATI->BusType = ATI_BUS_PCI;
                    pATI->PCIInfo = pVideo;
                }

                xf86SetPciVideo(NULL, NONE);
            }

            /* Next, look for PCI Mach32's */
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if ((pVideo->vendor != PCI_VENDOR_ATI) ||
                    (pVideo->chipType != PCI_CHIP_MACH32) ||
                    !(pPCI = pVideo->thisCard))
                    continue;

                Domain = xf86GetPciDomain(pPCI->tag);

                switch (ATICheckSparseIOBases(pVideo, ProbeFlags, Domain,
                    0x02E8U, 8, TRUE))
                {
                    case Conflict:
                        xf86Msg(X_WARNING,
                            ATI_NAME ":  PCI Mach32 in slot %d:%d:%d will not"
                            " be enabled because it conflicts with a"
                            " non-video PCI/AGP device.\n",
                            pVideo->bus, pVideo->device, pVideo->func);
                        continue;

                    case Detected8514A:
                        pATI = pp8514[Domain];
                        if (pATI->BusType >= ATI_BUS_PCI)
                        {
                            if (pVideo == pATI->PCIInfo)
                               continue;

                            if (!pATI->PCIInfo)
                            {
                                pATI->PCIInfo = pVideo;
                                continue;
                            }
                        }

                        xf86Msg(X_WARNING,
                            ATI_NAME ":  PCI Mach32 in slot %d:%d:%d will not"
                            " be enabled because it conflicts with another %s"
                            " %s.\n",
                            pVideo->bus, pVideo->device, pVideo->func,
                            ATIBusNames[pATI->BusType],
                            ATIAdapterNames[pATI->Adapter]);
                        continue;

                    case DetectedMach64:
                        xf86Msg(X_WARNING,
                            ATI_NAME ":  PCI Mach32 in slot %d:%d:%d will not"
                            " be enabled because it conflicts with a Mach64 at"
                            " I/O base 0x02EC in domain %d.\n",
                            pVideo->bus, pVideo->device, pVideo->func, Domain);
                        continue;

                    default:    /* Must be DoProbe */
                        if (!xf86CheckPciSlot(pVideo->bus,
                                              pVideo->device,
                                              pVideo->func))
                            continue;

                        xf86SetPciVideo(pVideo, MEM_IO);

                        pATI = ATI8514Probe(pVideo, pDomainIOBase[Domain],
                            Domain);
                        if (!pATI)
                        {
                            xf86MsgVerb(X_WARNING, 0,
                                ATI_NAME ":  PCI Mach32 in slot %d:%d:%d could"
                                " not be detected!\n",
                                pVideo->bus, pVideo->device, pVideo->func);
                        }
                        else
                        {
                            sprintf(Identifier,
                                "Shared 8514/A in PCI slot %d:%d:%d",
                                pVideo->bus, pVideo->device, pVideo->func);
                            xf86MsgVerb(X_INFO, 3,
                                ATI_NAME ":  %s detected.\n", Identifier);
                            if (pATI->Adapter != ATI_ADAPTER_MACH32)
                                xf86MsgVerb(X_WARNING, 0,
                                    ATI_NAME ":  PCI Mach32 in slot %d:%d:%d"
                                    " could only be detected as an %s!\n",
                                    pVideo->bus, pVideo->device, pVideo->func,
                                    ATIAdapterNames[pATI->Adapter]);

                            AddAdapter(pATI);
                            pATI->SharedAccelerator = TRUE;

                            if ((pATI->VGAAdapter != ATI_ADAPTER_NONE) ||
                                (pATI->Coprocessor != ATI_CHIP_NONE))
                                ATIFindVGA(pVideo, &ppVGA[Domain], &pATI,
                                    pp8514[Domain], ProbeFlags, Domain);
                        }

                        xf86SetPciVideo(NULL, NONE);
                        continue;
                }
            }

            /* Next, look for sparse I/O Mach64's */
            for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
            {
                if ((pVideo->vendor != PCI_VENDOR_ATI) ||
                    (pVideo->chipType == PCI_CHIP_MACH32) ||
                    pVideo->size[1] ||
                    !(pPCI = pVideo->thisCard))
                    continue;

                PciReg = pciReadLong(pPCI->tag, PCI_REG_USERCONFIG);
                Domain = xf86GetPciDomain(pPCI->tag);

                IODecoding = MEMORY_IO;
                j = PciReg & 0x03U;
                if (j == 0x03U)
                {
                    /*
                     * These adapters can only be detected, let alone used,
                     * through MEMORY_IO.
                     */
                    for (k = 0;  k < nATIPtr;  k++)
                        if (ATIPtrs[k]->PCIInfo == pVideo)
                            goto NextSparseMach64;

                    Mach64SparseIOBase = 0;
                }
                else
                {
                    Mach64SparseIOBase = Mach64SparseIOBases[j];

                    switch (ATICheckSparseIOBases(pVideo, ProbeFlags, Domain,
                                Mach64SparseIOBase, 4, TRUE))
                    {
                        case Detected8514A:
                            xf86Msg(X_WARNING,
                                ATI_NAME ":  PCI Mach64 in slot %d:%d:%d will"
                                " not be enabled because it conflicts with an"
                                " %s in domain %d.\n",
                                pVideo->bus, pVideo->device, pVideo->func,
                                ATIAdapterNames[pp8514[Domain]->Adapter],
                                Domain);
                            continue;

                        case DetectedMach64:
                            /* Already detected? */
                            pATI = ppMach64[(Domain * 3) + j];
                            if (pATI->PCIInfo == pVideo)
                                continue;

                            xf86Msg(X_WARNING,
                                ATI_NAME ":  PCI Mach64 in slot %d:%d:%d will"
                                " not be enabled because it conflicts with"
                                " another %s Mach64 at sparse I/O base 0x%04X"
                                " in domain %d.\n",
                                pVideo->bus, pVideo->device, pVideo->func,
                                ATIBusNames[pATI->BusType], Mach64SparseIOBase,
                                Domain);
                            continue;

                        case DoProbe:
                            /* Do a sparse I/O probe */
                            IODecoding = SPARSE_IO;
                            break;

                        default:
                            /* Some kind of I/O routing problem */
                            break;
                    }
                }

                if (!xf86CheckPciSlot(pVideo->bus,
                                      pVideo->device,
                                      pVideo->func))
                    continue;

                xf86SetPciVideo(pVideo, MEM_IO);

                Chip = ATIChipID(pVideo->chipType, pVideo->chipRev);
                pATI = ATIMach64Probe(pVideo, Mach64SparseIOBase,
                    pDomainIOBase[Domain], Domain, IODecoding, Chip);
                if (!pATI)
                {
                    xf86MsgVerb(X_WARNING, 0,
                        ATI_NAME ":  PCI Mach64 in slot %d:%d:%d could not be"
                        " detected!\n",
                        pVideo->bus, pVideo->device, pVideo->func);
                }
                else
                {
                    sprintf(Identifier, "Shared PCI Mach64 in slot %d:%d:%d",
                        pVideo->bus, pVideo->device, pVideo->func);
                    xf86MsgVerb(X_INFO, 3, ATI_NAME ":  %s detected.\n",
                        Identifier);
                    AddAdapter(pATI);
                    pATI->SharedAccelerator = TRUE;

                    if (pATI->VGAAdapter != ATI_ADAPTER_NONE)
                        ATIFindVGA(pVideo, &ppVGA[Domain], &pATI,
                            pp8514[Domain], ProbeFlags, Domain);
                }

                xf86SetPciVideo(NULL, NONE);

        NextSparseMach64:;
            }
        }

        /* Lastly, look for block I/O devices */
        for (i = 0;  (pVideo = xf86PciVideoInfo[i++]);  )
        {
            if ((pVideo->vendor != PCI_VENDOR_ATI) ||
                (pVideo->chipType == PCI_CHIP_MACH32) ||
                !pVideo->size[1] ||
                !(pPCI = pVideo->thisCard))
                continue;

            /* Check for Rage128's, Radeon's and later adapters */
            Chip = ATIChipID(pVideo->chipType, pVideo->chipRev);
            if (Chip > ATI_CHIP_Mach64)
            {
                if (Chip <= ATI_CHIP_Rage128)
                    DoRage128 = TRUE;
                else if (Chip <= ATI_CHIP_Radeon)
                    DoRadeon = TRUE;

                continue;
            }

            if (!nATIGDev)
                continue;

            /* Check if this one has already been detected */
            for (j = 0;  j < nATIPtr;  j++)
                if (ATIPtrs[j]->PCIInfo == pVideo)
                    goto NextBlockMach64;

            if (!xf86CheckPciSlot(pVideo->bus, pVideo->device, pVideo->func))
                continue;

            /* Probe for it */
            xf86SetPciVideo(pVideo, MEM_IO);

            Domain = xf86GetPciDomain(pPCI->tag);
            pATI = ATIMach64Probe(pVideo, pVideo->ioBase[1],
                pDomainIOBase[Domain], Domain, BLOCK_IO, Chip);
            if (pATI)
            {
                sprintf(Identifier, "Shared PCI/AGP Mach64 in slot %d:%d:%d",
                    pVideo->bus, pVideo->device, pVideo->func);
                xf86MsgVerb(X_INFO, 3, ATI_NAME ":  %s detected.\n",
                    Identifier);
                AddAdapter(pATI);
                pATI->SharedAccelerator = TRUE;

                if (pATI->VGAAdapter != ATI_ADAPTER_NONE)
                    ATIFindVGA(pVideo, &ppVGA[Domain], &pATI, pp8514[Domain],
                        ProbeFlags, Domain);
            }
            else
            {
                xf86MsgVerb(X_WARNING, 0,
                    ATI_NAME ":  PCI/AGP Mach64 in slot %d:%d:%d could not be"
                    " detected!\n", pVideo->bus, pVideo->device, pVideo->func);
            }

            xf86SetPciVideo(NULL, NONE);

    NextBlockMach64:;
        }
    }

    /*
     * At this point, if there's a non-shareable VGA with its own framebuffer,
     * find out if it's an ATI VGA Wonder.
     */
    if (nATIGDev)
    {
        for (Domain = 0;  Domain < MaxDomain;  Domain++)
        {
            if (!(pATI = ppVGA[Domain]) ||
                 (pATI->VGAAdapter > ATI_ADAPTER_VGA))
                continue;

            /*
             * If it has not been assigned to a coprocessor, keep track of it.
             */
            if (pATI->Coprocessor == ATI_CHIP_NONE)
                AddAdapter(pATI);

            /* The following test isn't entirely correct but will do for now */
            if (!xf86DomainHasBIOSSegments(Domain))
                continue;

            /*
             * A VGA should have installed its int 10 vector.  Use that to find
             * its BIOS.  If this fails, scan all legacy BIOS segments, in
             * 512-byte increments.
             */
            if (xf86ReadBIOS(0U, 0x42U, BIOS, 2) != 2)
                continue;

            pATI = NULL;
            BIOSBase = 0;
            if (!(BIOS[0] & 0x1FU))
                BIOSBase = ((BIOS[1] << 8) | BIOS[0]) << 4;

            /* Look for its BIOS */
            for(;  ;  BIOSBase += 0x00000200U)
            {
                if (!BIOSBase)
                    goto SkipBiosSegment;

                if (BIOSBase >= 0x000F8000U)
                    goto NoVGAWonder;

                /* Skip over those that are already known */
                for (i = 0;  i < nATIPtr;  i++)
                    if ((ATIPtrs[i]->Domain == Domain) &&
                        (ATIPtrs[i]->BIOSBase == BIOSBase))
                        goto SkipBiosSegment;

                /* Get first 80 bytes of video BIOS */
                if (xf86ReadBIOS(BIOSBase, 0, BIOS, SizeOf(BIOS)) !=
                    SizeOf(BIOS))
                    goto NoVGAWonder;

                if ((BIOS[0x00U] == 0x55U) && (BIOS[0x01U] == 0xAAU) &&
                    !memcmp(BIOS + IBMOffset, IBMSignature, IBMSize))
                    break;

        SkipBiosSegment:
                if (pATI)
                    continue;

                pATI = ppVGA[Domain];
                BIOSBase = 0x000C0000U - 0x00000200U;
            }

            pATI = ppVGA[Domain];
            pATI->BIOSBase = BIOSBase;

            /* Look for the ATI signature string */
            if (memcmp(BIOS + ATIOffset, ATISignature, ATISize))
                continue;

            if (BIOS[0x40U] != '3')
                continue;

            switch (BIOS[0x41U])
            {
                case '1':
                    /* This is a Mach8 or VGA Wonder adapter of some kind */
                    if ((BIOS[0x43U] >= '1') && (BIOS[0x43U] <= '6'))
                        pATI->Chip = BIOS[0x43U] - ('1' - ATI_CHIP_18800);

                    switch (BIOS[0x43U])
                    {
                        case '1':       /* ATI_CHIP_18800 */
                            pATI->VGAOffset = 0xB0U;
                            pATI->VGAAdapter = ATI_ADAPTER_V3;
                            break;

                        case '2':       /* ATI_CHIP_18800_1 */
                            pATI->VGAOffset = 0xB0U;
                            if (BIOS[0x42U] & 0x10U)
                                pATI->VGAAdapter = ATI_ADAPTER_V5;
                            else
                                pATI->VGAAdapter = ATI_ADAPTER_V4;
                            break;

                        case '3':       /* ATI_CHIP_28800_2 */
                        case '4':       /* ATI_CHIP_28800_4 */
                        case '5':       /* ATI_CHIP_28800_5 */
                        case '6':       /* ATI_CHIP_28800_6 */
                            pATI->VGAOffset = 0xA0U;
                            if (BIOS[0x44U] & 0x80U)
                                pATI->VGAAdapter = ATI_ADAPTER_XL;
                            else
                                pATI->VGAAdapter = ATI_ADAPTER_PLUS;
                            break;

                        case 'a':       /* A crippled Mach32 */
                        case 'b':
                        case 'c':
                            pATI->VGAOffset = 0x80U;
                            pATI->VGAAdapter = ATI_ADAPTER_NONISA;
                            ATIMach32ChipID(pATI);
                            ProbeWaitIdleEmpty();
                            if (inw(SUBSYS_STAT) != (CARD16)(-1))
                                pATI->ChipHasSUBSYS_CNTL = TRUE;
                            break;
#if 0
                        case ' ':       /* A crippled Mach64 */
                            pATI->VGAOffset = 0x80U;
                            pATI->VGAAdapter = ATI_ADAPTER_NONISA;
                            ATIMach64ChipID(pATI, 0);
                            break;
#endif
                        default:
                            break;
                    }

                    if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
                        break;

                    /* Set VGA Wonder I/O port */
                    pATI->CPIO_VGAWonder = BIOSWord(0x10U) & SPARSE_IO_PORT;
                    if (!pATI->CPIO_VGAWonder)
                        pATI->CPIO_VGAWonder = 0x01CEU;

                    ATIVGAWonderProbe(NULL, pATI, pp8514[Domain], ProbeFlags,
                        NULL, Domain);
                    break;
#if 0
                case '2':
                    pATI->VGAOffset = 0xB0U;    /* Presumably */
                    pATI->VGAAdapter = ATI_ADAPTER_EGA_PLUS;
                    break;

                case '3':
                    pATI->VGAOffset = 0xB0U;    /* Presumably */
                    pATI->VGAAdapter = ATI_ADAPTER_BASIC;
                    break;

                case '?':       /* A crippled Mach64 */
                    pATI->VGAAdapter = ATI_ADAPTER_NONISA;
                    ATIMach64ChipID(pATI, 0);
                    break;
#endif
                default:
                    break;
            }

            if (pATI->Adapter <= ATI_ADAPTER_VGA)
                pATI->Adapter = pATI->VGAAdapter;

    NoVGAWonder:;
        }

        xfree(ppVGA);
        xfree(pp8514);
        xfree(ppMach64);
        xfree(pDomainIOBase);
        xfree(ProbeFlags);
    }

    if (flags & PROBE_DETECT)
    {
        /*
         * No XF86Config information available, so use the default Chipset of
         * "ati", and generate as many device sections as there are applicable
         * adapters.
         *
         * Within each domain, generate device sections in a specific order:
         * first non-PCI adapters in the order they are detected, then any PCI
         * primary, followed by non-primary PCI in increasing PCITAG order.
         */
        for (i = 0;  i < nATIPtr;  i++)
        {
            if (!(pATI = ATIPtrs[i]))
                continue;

            if ((pVideo = pATI->PCIInfo))
            {
                PCITAG PCITag = 0;

                Domain = pATI->Domain;
                if ((pPCI = pVideo->thisCard))
                    PCITag = pPCI->tag;

                for (k = -1, j = i;  ;  j++)
                {
                    if ((j >= nATIPtr) ||
                        ((pATI = ATIPtrs[j]) && (Domain != pATI->Domain)))
                    {
                        if (k >= 0)
                            i = k;
                        pATI = ATIPtrs[i];
                        break;
                    }

                    if (!pATI)
                        continue;

                    if (!(pVideo = pATI->PCIInfo))
                    {
                        i = j;
                        break;
                    }

                    if (k >= 0)
                        continue;

                    if (xf86IsPrimaryPci(pVideo))
                    {
                        k = j;
                        continue;
                    }

                    if (!PCITag)
                        continue;

                    if (!(pPCI = pVideo->thisCard))
                        PCITag = 0;
                    else if (PCITag > pPCI->tag)
                        PCITag = pPCI->tag;
                    else
                        continue;

                    i = j;
                }
            }

            if ((pATI->Adapter != ATI_ADAPTER_VGA) &&
                ((pATI->Adapter != ATI_ADAPTER_8514A) ||
                 ((pATI->VGAAdapter != ATI_ADAPTER_VGA) &&
                  (pATI->VGAAdapter != ATI_ADAPTER_NONE))))
            {
                ProbeSuccess = TRUE;
                pGDev = xf86AddDeviceToConfigure(ATI_DRIVER_NAME,
                    pATI->PCIInfo, ATI_CHIPSET_ATI);
                if (pGDev)
                {
                    /* Fill in additional information */
                    pGDev->vendor = ATI_NAME;
                    pGDev->chipset = (char *)ATIChipsetNames[ATI_CHIPSET_ATI];
                    if (!pATI->PCIInfo)
                        pGDev->busID = NULL;
                }
            }

            xfree(pATI);
            ATIPtrs[i] = NULL;
            i = -1;
        }
    }
    else
    {
        /*
         * Assign detected devices to XF86Config Device sections.  This is done
         * by comparing certain Device section specifications against the
         * corresponding adapter information.  Begin with those specifications
         * that are independent of the adapter's bus location.
         */
        for (i = 0, pATIGDev = ATIGDevs;  i < nATIGDev;  i++, pATIGDev++)
        {
            pGDev = pATIGDev->pGDev;

            for (j = 0;  j < nATIPtr;  j++)
            {
                pATI = ATIPtrs[j];

                /*
                 * First check the Chipset specification.  The placement of
                 * "break" and "continue" statements here is carefully chosen
                 * to produce the intended behaviour for each Chipset value.
                 */
                switch (pATIGDev->Chipset)
                {
                    case ATI_CHIPSET_ATI:
                        if (pATI->Adapter == ATI_ADAPTER_VGA)
                            continue;
                        if (pATI->Adapter != ATI_ADAPTER_8514A)
                            break;
                        /* Fall through */

                    case ATI_CHIPSET_ATIVGA:
                        if (pATI->VGAAdapter == ATI_ADAPTER_VGA)
                            continue;
                        /* Fall through */

                    case ATI_CHIPSET_IBMVGA:
                        if (pATI->VGAAdapter == ATI_ADAPTER_NONE)
                            continue;
                        break;

                    case ATI_CHIPSET_VGAWONDER:
                        if (!pATI->CPIO_VGAWonder)
                            continue;
                        break;

                    case ATI_CHIPSET_IBM8514:
                        if (pATI->Adapter == ATI_ADAPTER_8514A)
                            break;
                        /* Fall through */

                    case ATI_CHIPSET_MACH8:
                        if (pATI->Adapter == ATI_ADAPTER_MACH8)
                            break;
                        /* Fall through */

                    case ATI_CHIPSET_MACH32:
                        if (pATI->Adapter == ATI_ADAPTER_MACH32)
                            break;
                        continue;

                    case ATI_CHIPSET_MACH64:
                        if (pATI->Adapter == ATI_ADAPTER_MACH64)
                            break;
                        continue;

                    default:
                        continue;
                }

                /*
                 * The ChipID and ChipRev specifications are compared next.
                 * First, require these to be unspecified for anything other
                 * than Mach32 or Mach64 adapters.  ChipRev is also required to
                 * be unspecified for Mach32's.  ChipID is optional for
                 * Mach32's, and both specifications are optional for Mach64's.
                 * Lastly, allow both specifications to override their detected
                 * value in the case of Mach64 adapters whose ChipID is
                 * unrecognised.
                 */
                pVideo = pATI->PCIInfo;
                if (pGDev->chipID >= 0)
                {
                    if ((pATI->ChipType != pGDev->chipID) &&
                        (!pVideo || (pGDev->chipID != pVideo->chipType)))
                    {
                        if ((pATI->Adapter != ATI_ADAPTER_MACH64) ||
                            (pATI->Chip != ATI_CHIP_Mach64))
                            continue;

                        Chip = ATIChipID(pGDev->chipID, 0);
                        if ((Chip <= ATI_CHIP_264GTB) ||
                            (Chip == ATI_CHIP_Mach64))
                            continue;
                    }
                    if ((pGDev->chipRev >= 0) &&
                        (pATI->ChipRev != pGDev->chipRev) &&
                        (!pVideo || (pGDev->chipRev != pVideo->chipRev) ||
                         (pGDev->chipID != pVideo->chipType)))
                    {
                        if (pATI->Chip < ATI_CHIP_264CT)
                            continue;

                        if (pATI->Chip != ATI_CHIP_Mach64)
                        {
                            /*
                             * There are two foundry codes for UMC.  Some
                             * adapters will advertise one in CONFIG_CHIP_ID
                             * and the other in PCI configuration space.  For
                             * matching purposes, make both codes compare
                             * equal.
                             */
#                           define UMC_IGNORE \
                                (ATI_FOUNDRY_UMC ^ ATI_FOUNDRY_UMCA)
#                           define UMC_NOCARE \
                                GetBits(SetBits(UMC_IGNORE, CFG_CHIP_FOUNDRY), \
                                    CFG_CHIP_REV)

                            if ((pATI->ChipRev ^ pGDev->chipRev) & ~UMC_NOCARE)
                                continue;

                            if ((pATI->ChipFoundry != ATI_FOUNDRY_UMC) &&
                                (pATI->ChipFoundry != ATI_FOUNDRY_UMCA))
                                continue;

                            k = GetBits(pGDev->chipRev,
                                GetBits(CFG_CHIP_FOUNDRY, CFG_CHIP_REV));
                            if ((k != ATI_FOUNDRY_UMC) &&
                                (k != ATI_FOUNDRY_UMCA))
                                continue;
                        }
                    }
                }

                /*
                 * Compare BusID's.  This specification is only allowed for PCI
                 * Mach32's or Mach64's and is optional.
                 */
                if (pGDev->busID && pGDev->busID[0])
                {
                    pVideo = pATI->PCIInfo;

                    if (!pVideo ||
                        !xf86ComparePciBusString(pGDev->busID,
                            pVideo->bus, pVideo->device, pVideo->func))
                        continue;
                }

                /*
                 * Ensure no two adapters are assigned to the same XF86Config
                 * Device section.
                 */
                if (pATIGDev->iATIPtr)
                {
                    if (pATIGDev->iATIPtr < 0)
                        break;

                    xf86Msg(X_ERROR,
                        ATI_NAME ":  XF86Config Device section \"%s\" may not"
                        " be assigned to more than one adapter.\n",
                        pGDev->identifier);
                    pATIGDev->iATIPtr = -1;
                    break;
                }

                /* Assign adapter */
                pATIGDev->iATIPtr = j + 1;

                /*
                 * For compatibility with previous releases, assign the first
                 * applicable adapter if there is only one Device section.
                 */
                if (nATIGDev == 1)
                    break;
            }
        }

        /*
         * Ensure no two XF86Config Device sections are assigned to the same
         * adapter.  Then, generate screens for any that are left.
         */
        for (i = 0, pATIGDev = ATIGDevs;  i < nATIGDev;  i++, pATIGDev++)
        {
            pGDev = pATIGDev->pGDev;

            j = pATIGDev->iATIPtr;
            if (j <= 0)
                continue;

            for (k = i;  ++k < nATIGDev;  )
            {
                if (j == ATIGDevs[k].iATIPtr)
                {
                    xf86Msg(X_ERROR,
                        ATI_NAME ":  XF86Config Device sections \"%s\" and"
                        " \"%s\" may not be assigned to the same adapter.\n",
                        pGDev->identifier, ATIGDevs[k].pGDev->identifier);
                    pATIGDev->iATIPtr = ATIGDevs[k].iATIPtr = -1;
                }
            }

            j = ATIGDevs[i].iATIPtr;
            if (j <= 0)
                continue;

            pATI = ATIPtrs[j - 1];

            xf86MsgVerb(X_INFO, 3,
                ATI_NAME ":  %s assigned to %sactive \"Device\" section"
                " \"%s\".\n",
                Identifier, pGDev->active ? "" : "in", pGDev->identifier);

            /*
             * Attach adapter to XF86Config Device section and register its
             * resources.
             */
            if (ATIClaimBusSlot(pDriver, pATIGDev->Chipset,
                                pGDev, pGDev->active, pATI) < 0)
            {
                xf86Msg(X_ERROR,
                    ATI_NAME ":  Could not claim bus slot for %s.\n",
                    Identifier);
                continue;
            }

            if (!pGDev->active)
                continue;

            /* Allocate screen */
            pScreenInfo = xf86AllocateScreen(pDriver, 0);

#ifdef XFree86LOADER

            if (!(pModule = xf86LoadSubModule(pScreenInfo, "atimisc")))
            {
                xf86Msg(X_ERROR,
                    ATI_NAME ":  Failed to load \"atimisc\" module.\n");
                xf86DeleteScreen(pScreenInfo->scrnIndex, 0);
                continue;
            }

            xf86LoaderModReqSymLists(pModule, ATISymbols, NULL);

#endif

            /* Attach device to screen */
            xf86AddEntityToScreen(pScreenInfo, pATI->iEntity);

            ATIPtrs[j - 1] = NULL;

            /* Fill in probe data */
            pScreenInfo->driverVersion = ATI_VERSION_CURRENT;
            pScreenInfo->driverName    = ATI_DRIVER_NAME;
            pScreenInfo->name          = ATI_NAME;
            pScreenInfo->Probe         = ATIProbe;
            pScreenInfo->PreInit       = ATIPreInit;
            pScreenInfo->ScreenInit    = ATIScreenInit;
            pScreenInfo->SwitchMode    = ATISwitchMode;
            pScreenInfo->AdjustFrame   = ATIAdjustFrame;
            pScreenInfo->EnterVT       = ATIEnterVT;
            pScreenInfo->LeaveVT       = ATILeaveVT;
            pScreenInfo->FreeScreen    = ATIFreeScreen;
            pScreenInfo->ValidMode     = ATIValidMode;

            pScreenInfo->driverPrivate = pATI;

            pATI->Chipset = pATIGDev->Chipset;

            ProbeSuccess = TRUE;
        }

        /* Deal with unassigned adapters */
        for (i = 0;  i < nATIPtr;  i++)
        {
            if (!(pATI = ATIPtrs[i]))
                continue;

            if ((pATI->Adapter > ATI_ADAPTER_VGA) && (pATI->iEntity < 0))
                (void)ATIClaimBusSlot(pDriver, 0, NULL, FALSE, pATI);

            xfree(pATI);
        }

        xfree(ATIGDevs);
    }

    xfree(ATIPtrs);

    /* Call Rage 128 driver probe */
    if (DoRage128 && R128Probe(pDriver, flags))
        ProbeSuccess = TRUE;

    /* Call Radeon driver probe */
    if (DoRadeon && RADEONProbe(pDriver, flags))
        ProbeSuccess = TRUE;

    return ProbeSuccess;
}
