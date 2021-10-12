/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/xgi/vb_util.c,v 1.2 2005/06/03 01:13:02 tsi Exp $ */

#include "osdef.h"
#include "vb_def.h"
#include "vgatypes.h"
#include "vb_struct.h"
#include "vb_util.h"

#ifdef LINUX_KERNEL
#include "XGIfb.h"
#include <asm/io.h>
#include <linux/types.h>
#endif

#ifdef TC
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#endif

#ifdef WIN2000
#include <dderror.h>
#include <devioctl.h>
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>

#include "xgiv.h"
#include "dd_i2c.h"
#include "tools.h"
#endif

#ifdef LINUX_XF86
#include "xf86.h"
#include "xf86PciInfo.h"
#include "xgi.h"
#include "xgi_regs.h"
#endif


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetReg1 */
/* Input : */
/* Output : */
/* Description : SR CRTC GR */
/* --------------------------------------------------------------------- */
void XGINew_SetReg1( USHORT port , USHORT index , USHORT data )
{
    OutPortByte( ( ULONG )port , index ) ;
    OutPortByte( ( ULONG )port + 1 , data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetReg2 */
/* Input : */
/* Output : */
/* Description : AR( 3C0 ) */
/* --------------------------------------------------------------------- */
/*void XGINew_SetReg2( USHORT port , USHORT index , USHORT data )
{
    InPortByte( ( PUCHAR )port + 0x3da - 0x3c0 ) ;
    OutPortByte( XGINew_P3c0 , index ) ;
    OutPortByte( XGINew_P3c0 , data ) ;
    OutPortByte( XGINew_P3c0 , 0x20 ) ;
}*/


/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetReg3( USHORT port , USHORT data )
{
    OutPortByte( port , data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetReg4 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetReg4( USHORT port , ULONG data )
{
    OutPortLong( port , data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_GetReg1 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGINew_GetReg1( USHORT port , USHORT index )
{
    UCHAR data ;

    OutPortByte( ( ULONG )port , index ) ;
    data = InPortByte( ( ULONG )port + 1 ) ;

    return( data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_GetReg2 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
UCHAR XGINew_GetReg2( USHORT port )
{
    UCHAR data ;

    data = InPortByte( port ) ;

    return( data ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_GetReg3 */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
ULONG XGINew_GetReg3( USHORT port )
{
    ULONG data ;

    data = InPortLong( port ) ;

    return( data ) ;
}



/* --------------------------------------------------------------------- */
/* Function : XGINew_SetRegANDOR */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetRegANDOR( USHORT Port , USHORT Index , USHORT DataAND , USHORT DataOR )
{
    USHORT temp ;

    temp = XGINew_GetReg1( Port , Index ) ;		/* XGINew_Part1Port index 02 */
    temp = ( temp & ( DataAND ) ) | DataOR ;
    XGINew_SetReg1( Port , Index , temp ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetRegAND */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetRegAND(USHORT Port,USHORT Index,USHORT DataAND)
{
    USHORT temp ;

    temp = XGINew_GetReg1( Port , Index ) ;	/* XGINew_Part1Port index 02 */
    temp &= DataAND ;
    XGINew_SetReg1( Port , Index , temp ) ;
}


/* --------------------------------------------------------------------- */
/* Function : XGINew_SetRegOR */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_SetRegOR( USHORT Port , USHORT Index , USHORT DataOR )
{
    USHORT temp ;

    temp = XGINew_GetReg1( Port , Index ) ;	/* XGINew_Part1Port index 02 */
    temp |= DataOR ;
    XGINew_SetReg1( Port , Index , temp ) ;
}



/* --------------------------------------------------------------------- */
/* Function : */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGINew_ClearDAC( PUCHAR port )
{
    int i ;

    OutPortByte( port , 0 ) ;
    port++ ;
    for( i = 0 ; i < 256 * 3 ; i++ )
    {
        OutPortByte( port , 0 ) ;
    }
}


#if 0
/* --------------------------------------------------------------------- */
/* Function : NewDelaySecond */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
static void NewDelaySeconds( int seconds )
{
    int i ;


    for( i = 0 ; i < seconds ; i++ )
    {
#ifdef TC
        delay( 1000 ) ;
#endif

#ifdef WIN2000

        for ( j = 0 ; j < 20000 ; j++ )
            VideoPortStallExecution( 50 ) ;
#endif

#ifdef WINCE_HEADER
#endif

#ifdef LINUX_KERNEL
#endif
    }
}
#endif


/* --------------------------------------------------------------------- */
/* Function : XGI_Newdebugcode */
/* Input : */
/* Output : */
/* Description : */
/* --------------------------------------------------------------------- */
void XGI_Newdebugcode( UCHAR code )
{
    OutPortByte ( 0x80 , code ) ;
#if 0
    OutPortByte ( 0x300 , code ) ;
    NewDelaySeconds( 0x3 ) ;
#endif
}



