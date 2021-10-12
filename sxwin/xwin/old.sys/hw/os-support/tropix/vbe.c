/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/vbe/vbe.c,v 1.14 2000/12/06 15:35:32 eich Exp $ */

#include "xf86.h"
#include "xf86_ansic.h"
#include "vbe.h"
#if (0)	/*******************************************************/
#include "Xarch.h"
#endif	/*******************************************************/

#define VERSION(x) *((CARD8*)(&x) + 1),(CARD8)(x)

#ifndef	TROPIX
#if X_BYTE_ORDER == X_LITTLE_ENDIAN
#define B_O16(x)  (x) 
#define B_O32(x)  (x)
#else
#define B_O16(x)  ((((x) & 0xff) << 8) | (((x) & 0xff) >> 8))
#define B_O32(x)  ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) \
                  | (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#endif
#define L_ADD(x)  (B_O32(x) & 0xffff) + ((B_O32(x) >> 12) & 0xffff00)
#endif	TROPIX

vbeInfoPtr
VBEExtendedInit(xf86Int10InfoPtr pInt, int entityIndex, int Flags)
{
	return NULL;
}

void
vbeFree(vbeInfoPtr pVbe)
{
}

xf86MonPtr
vbeDoEDID(vbeInfoPtr pVbe, ModuleDescPtr pDDCModule)
{
	return NULL;
}

vbeInfoPtr
VBEInit(xf86Int10InfoPtr pInt, int entityIndex)
{
	return VBEExtendedInit (pInt, entityIndex, 0);
}

VbeInfoBlock *
VBEGetVBEInfo(vbeInfoPtr pVbe)
{
	return NULL;
}

void
VBEFreeVBEInfo(VbeInfoBlock *block)
{
}

Bool
VBESetVBEMode(vbeInfoPtr pVbe, int mode, VbeCRTCInfoBlock *block)
{
	return FALSE;
}

Bool
VBEGetVBEMode(vbeInfoPtr pVbe, int *mode)
{
	return FALSE;
}

VbeModeInfoBlock *
VBEGetModeInfo(vbeInfoPtr pVbe, int mode)
{
	return NULL;
}

void
VBEFreeModeInfo(VbeModeInfoBlock *block)
{
}

Bool
VBESaveRestore(vbeInfoPtr pVbe, vbeSaveRestoreFunction function, 
	       pointer *memory, int *size, int *real_mode_pages)
{
	return FALSE;
}

Bool
VBESetGetLogicalScanlineLength(vbeInfoPtr pVbe, vbeScanwidthCommand command,
				int width, int *pixels, int *bytes, int *max)
{
	return FALSE;
}

Bool
VBESetDisplayStart(vbeInfoPtr pVbe, int x, int y, Bool wait_retrace)
{
	return FALSE;
}

Bool
VBEGetDisplayStart(vbeInfoPtr pVbe, int *x, int *y)
{
	return FALSE;
}
