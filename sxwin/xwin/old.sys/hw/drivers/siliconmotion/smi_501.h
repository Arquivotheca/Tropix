/*
Copyright (C) 1994-1999 The XFree86 Project, Inc.  All Rights Reserved.
Copyright (C) 2000 Silicon Motion, Inc.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FIT-
NESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
XFREE86 PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the names of the XFree86 Project and
Silicon Motion shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization from the XFree86 Project and Silicon Motion.
*/
/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/siliconmotion/smi_501.h,v 1.1 2006/03/09 17:37:08 tsi Exp $ */

#ifndef _SMI_501_H
#define _SMI_501_H


/******************************************************************************/
/*                      D E F I N I T I O N S                                 */
/******************************************************************************/

#define INT     int
#define LONG    int
#define DWORD   unsigned int
#define ULONG   unsigned int
#define FLONG   unsigned int
#define VOID    void
#define PUCHAR  unsigned char *
#define UCHAR   unsigned char

#define USE_CRYSTAL_12 0 /* Use PLL with 12MHz crystal instead of test clock. */
#define USE_CRYSTAL_24 1 /* Use PLL with 24MHz crystal instead of test clock. */

/* Power constants to use with setDPMS function. */
typedef enum _DPMS_t
{
    DPMS_ON,
    DPMS_STANDBY,
    DPMS_SUSPEND,
    DPMS_OFF
}
DPMS_t;

/* Display type constants to use with setMode function and others. */
typedef enum _display_t
{
    PANEL = 0,
    CRT = 1
}
display_t;

/* Type of LCD display */
typedef enum _lcd_display_t
{
    LCD_TFT = 0,
    LCD_STN_8 = 2,
    LCD_STN_12 = 3
}
lcd_display_t;

/* Polarity constants. */
typedef enum _polarity_t
{
    POSITIVE,
    NEGATIVE
}
polarity_t;

/* RGB color structure. */
typedef struct {
        UCHAR cBlue;
        UCHAR cGreen;
        UCHAR cRed;
        UCHAR cFiller;
} RGB;

/* Format of mode table record */
typedef struct _mode_table_t
{
    /* Horizontal timing */
    INT horizontal_total;
    INT horizontal_display_end;
    INT horizontal_sync_start;
    INT horizontal_sync_width;
    polarity_t horizontal_sync_polarity;

    /* Vertical timing. */
    INT vertical_total;
    INT vertical_display_end;
    INT vertical_sync_start;
    INT vertical_sync_height;
    polarity_t vertical_sync_polarity;

    /* Refresh timing. */
    LONG pixel_clock;
    LONG horizontal_frequency;
    LONG vertical_frequency;
}
mode_table_t, *pmode_table_t;

/* Clock value structure. */
typedef struct clock_select_t
{
    LONG mclk;
    INT divider;
    INT shift;
}
clock_select_t, *pclock_select_t;

/* Registers necessary to set mode. */
typedef struct _reg_table_t
{
    DWORD clock;
    DWORD control;
    DWORD fb_width;
    DWORD horizontal_total;
    DWORD horizontal_sync;
    DWORD vertical_total;
    DWORD vertical_sync;
    DWORD width;
    DWORD height;
    display_t display;
}
reg_table_t, *preg_table_t;

/* Structure used to initialize CRT hardware module */
typedef struct
{
    ULONG mask;       /* Holds flags indicating which register bitfields to init */
    ULONG fifo_level; /* FIFO request level */
    ULONG tvp;        /* TV clock phase select */
    ULONG cp;         /* CRT clock phase select */
    ULONG blank;      /* CRT data blanking */
    ULONG format;     /* CRT graphics plane format */
} init_crt, *pinit_crt;

/* Structure used to initialize CRT cursor hardware module */
typedef struct
{
    ULONG mask;       /* Holds flags indicating which register bitfields to init */
} init_crt_hwc, *pinit_crt_hwc;

/* Panel On/Off constants to use with panelPowerSequence. */
typedef enum _panel_state_t
{
    PANEL_OFF,
    PANEL_ON
}
panel_state_t;

/******************************************************************************/
/*                          M A C R O S                                       */
/******************************************************************************/
/* Direct register access macro  */
#define REG_READ8(r)        (*(volatile PUCHAR)  &g_pRegisters[r])
#define REG_READ16(r)       (*(volatile PUSHORT) &g_pRegisters[r])
#define REG_READ32(r)       (*(volatile PULONG)  &g_pRegisters[r])
#define REG_WRITE8(r,v)     {*(volatile PUCHAR)  &g_pRegisters[r] = (v);}
#define REG_WRITE16(r,v)    {*(volatile PUSHORT) &g_pRegisters[r] = (v);}
#define REG_WRITE32(r,v)    {*(volatile PULONG)  &g_pRegisters[r] = (v);}

/* Internal macros  */
#define _F_START(f)             (0 ? f)
#define _F_END(f)               (1 ? f)
#define _F_SIZE(f)              (1 + _F_END(f) - _F_START(f))
#define _F_MASK(f)              (((1 << _F_SIZE(f)) - 1) << _F_START(f))
#define _F_NORMALIZE(v, f)      (((v) & _F_MASK(f)) >> _F_START(f))
#define _F_DENORMALIZE(v, f)    (((v) << _F_START(f)) & _F_MASK(f))

/* Global macros  */
#define FIELD_GET(x, reg, field) \
( \
    _F_NORMALIZE((x), reg ## _ ## field) \
)

#define FIELD_SET(x, reg, field, value) \
( \
    (x & ~_F_MASK(reg ## _ ## field)) \
    | _F_DENORMALIZE(reg ## _ ## field ## _ ## value, reg ## _ ## field) \
)

#define FIELD_VALUE(x, reg, field, value) \
( \
    (x & ~_F_MASK(reg ## _ ## field)) \
    | _F_DENORMALIZE(value, reg ## _ ## field) \
)

#define FIELD_CLEAR(reg, field) \
( \
    ~ _F_MASK(reg ## _ ## field) \
)

/* Field Macros  */
#define FIELD_START(field)              (0 ? field)
#define FIELD_END(field)                (1 ? field)
#define FIELD_SIZE(field)               (1 + FIELD_END(field) - FIELD_START(field))
#define FIELD_MASK(field)               (((1 << (FIELD_SIZE(field)-1)) | ((1 << (FIELD_SIZE(field)-1)) - 1)) << FIELD_START(field))
#define FIELD_NORMALIZE(reg, field)     (((reg) & FIELD_MASK(field)) >> FIELD_START(field))
#define FIELD_DENORMALIZE(field, value) (((value) << FIELD_START(field)) & FIELD_MASK(field))

#define FIELD_INIT(reg, field, value)   FIELD_DENORMALIZE(reg ## _ ## field, \
                                                          reg ## _ ## field ## _ ## value)
#define FIELD_INIT_VAL(reg, field, value) \
                                        (FIELD_DENORMALIZE(reg ## _ ## field, value))
#define FIELD_VAL_SET(x, r, f, v)       x = x & ~FIELD_MASK(r ## _ ## f) \

/******************************************************************************/
/*                  F U N C T I O N   P R O T O T Y P E S                     */
/******************************************************************************/

/* Init flags and values used in init_crt structure */
#define DISP_CRT_TVP                       0x00000100    /* TV clock phase select */
#define DISP_CRT_TVP_HIGH                  0x00000000
#define DISP_CRT_TVP_LOW                   0x00008000

#define DISP_CRT_CP                        0x00000200    /* CRT clock phase select */
#define DISP_CRT_CP_HIGH                   0x00000000
#define DISP_CRT_CP_LOW                    0x00004000

#define DISP_CRT_BLANK                     0x00000400    /* CRT data blanking */
#define DISP_CRT_BLANK_OFF                 0x00000000
#define DISP_CRT_BLANK_ON                  0x00000400

#define DISP_CRT_FORMAT                    0x00000800    /* CRT graphics plane format */
#define DISP_CRT_FORMAT_8                  0x00000000
#define DISP_CRT_FORMAT_16                 0x00000001
#define DISP_CRT_FORMAT_32                 0x00000002

#define DISP_MODE_8_BPP                    0             /* 8 bits per pixel i8RGB                        */
#define DISP_MODE_16_BPP                   1             /* 16 bits per pixel RGB565                      */
#define DISP_MODE_32_BPP                   2             /* 32 bits per pixel RGB888                      */
#define DISP_MODE_YUV                      3             /* 16 bits per pixel YUV422                      */
#define DISP_MODE_ALPHA_8                  4             /* 8 bits per pixel a4i4RGB                      */
#define DISP_MODE_ALPHA_16                 5             /* 16 bits per pixel a4RGB444                    */

#define DISP_PAN_LEFT                      0             /* Pan left                                      */
#define DISP_PAN_RIGHT                     1             /* Pan right                                     */
#define DISP_PAN_UP                        2             /* Pan upwards                                   */
#define DISP_PAN_DOWN                      3             /* Pan downwards                                 */

#define DISP_DPMS_QUERY                    -1            /* Query DPMS value                              */
#define DISP_DPMS_ON                       0             /* DPMS on                                       */
#define DISP_DPMS_STANDBY                  1             /* DPMS standby                                  */
#define DISP_DPMS_SUSPEND                  2             /* DPMS suspend                                  */
#define DISP_DPMS_OFF                      3             /* DPMS off                                      */

#define DISP_DELAY_DEFAULT                 0             /* Default delay                                 */

#define DISP_HVTOTAL_UNKNOWN               -1            /* Used in panelSetTiming, crtSetTiming if       */
                                                         /* nHTotal, nVTotal not specified by user        */
#define DISP_HVTOTAL_SCALEFACTOR           1.25          /* Used in panelSetTiming, crtSetTiming if       */
                                                         /* nHTotal, nVTotal not specified by user        */

#define VGX_SIGNAL_PANEL_VSYNC             100           /* Panel VSYNC                                   */
#define VGX_SIGNAL_PANEL_PAN               101           /* Panel auto panning complete                   */
#define VGX_SIGNAL_CRT_VSYNC               102           /* CRT VSYNC                                     */

#define VSYNCTIMEOUT                       10000

#define ALPHA_MODE_PER_PIXEL               0             /* Use per-pixel alpha values                    */
#define ALPHA_MODE_ALPHA                   1             /* Use alpha value specified in Alpha bitfield   */
#define ALPHA_COLOR_LUT_SIZE               16            /* Number of colors in alpha/video alpha palette */

#define HWC_ON_SCREEN                      0             /* Cursor is within screen top/left boundary     */
#define HWC_OFF_SCREEN                     1             /* Cursor is outside screen top/left boundary    */
#define HWC_NUM_COLORS                     3             /* Number of cursor colors                       */

#define RGB565_R_MASK                      0xF8          /* Mask for red color                            */
#define RGB565_G_MASK                      0xFC          /* Mask for green color                          */
#define RGB565_B_MASK                      0xF8          /* Mask for blue color                           */

#define RGB565_R_SHIFT                     8             /* Number of bits to shift for red color         */
#define RGB565_G_SHIFT                     3             /* Number of bits to shift for green color       */
#define RGB565_B_SHIFT                     3             /* Number of bits to shift for blue color        */

#define RGB16(r, g, b) \
( \
    (unsigned short) ((((r) & RGB565_R_MASK) << RGB565_R_SHIFT) | \
                      (((g) & RGB565_G_MASK) << RGB565_G_SHIFT) | \
                      (((b) & RGB565_B_MASK) >> RGB565_B_SHIFT)) \
)

/* Initialize the panel hardware module */

/* Initialize the CRT hardware module */
VOID crtInit(pinit_crt init);
/* Enable CRT gamma control (RGB 5:6:5 and RGB 8:8:8 modes only) */
VOID crtSetMode(SMIPtr pSmi, ULONG nWidth, ULONG nHeight, FLONG fMode, ULONG nHertz, INT fbPitch, INT bpp);
/* Get and/or set CRT DPMS.*/
ULONG crtDPMS(LONG nDPMS);
/* Set CRT frame buffer pointer. */
VOID crtSetTiming(ULONG nHDisplay, ULONG nHTotal, ULONG nHSyncStart,
                  ULONG nHSyncEnd, ULONG nVDisplay, ULONG nVTotal,
                  ULONG nVSyncStart, ULONG nVSyncEnd, ULONG nPixelClock,
                  ULONG nHPolarity, ULONG nVPolarity);
/* Get CRT LUT palette. */
VOID crtHwcInit(pinit_crt_hwc init);
/* Enable CRT hardware cursor */
VOID crtHwcEnable(VOID);
/* Disable CRT hardware cursor */
VOID crtHwcDisable(VOID);
/* Set CRT pointer shape and/or colors. */
VOID crtPointerShape(PUCHAR pShape, INT nWidth, INT nHeight, INT nHotX,
                     INT nHotY, RGB rgbColor[3]);
/* Set CRT cursor on-screen position. */
VOID crtPointerPosition(INT nX, INT nY, INT nTopSelect, INT nLeftSelect);
/* Set CRT cursor on-screen position. */
VOID crtHwcSetPosition(INT nX, INT nY, INT nTopSelect, INT nLeftSelect);
/* Set CRT cursor colors */
VOID crtHwcSetColors(RGB rgbColor[HWC_NUM_COLORS]);

/* Route Panel data to CRT for Simultaneous mode */
VOID panelUseCRT(SMIPtr pSmi, BOOL bEnable);
/* Enable panel gamma control (RGB 5:6:5 and RGB 8:8:8 modes only) */
VOID panelSetMode(SMIPtr pSmi, ULONG nWidth, ULONG nHeight, FLONG fMode, ULONG nHertz, INT fbPitch, INT bpp);

/* REGISTER DEFINITIONS */

/* regSC.h */
#define SYSTEM_CTRL                                     0x000000
#define SYSTEM_CTRL_DPMS                                31:30
#define SYSTEM_CTRL_DPMS_VPHP                           0
#define SYSTEM_CTRL_DPMS_VPHN                           1
#define SYSTEM_CTRL_DPMS_VNHP                           2
#define SYSTEM_CTRL_DPMS_VNHN                           3
#define SYSTEM_CTRL_PCI_BURST                           29:29
#define SYSTEM_CTRL_PCI_BURST_DISABLE                   0
#define SYSTEM_CTRL_PCI_BURST_ENABLE                    1
#define SYSTEM_CTRL_CSC_STATUS                          28:28
#define SYSTEM_CTRL_CSC_STATUS_IDLE                     0
#define SYSTEM_CTRL_CSC_STATUS_BUSY                     1
#define SYSTEM_CTRL_PCI_MASTER                          25:25
#define SYSTEM_CTRL_PCI_MASTER_STOP                     0
#define SYSTEM_CTRL_PCI_MASTER_START                    1
#define SYSTEM_CTRL_LATENCY_TIMER                       24:24
#define SYSTEM_CTRL_LATENCY_TIMER_ENABLE                0
#define SYSTEM_CTRL_LATENCY_TIMER_DISABLE               1
#define SYSTEM_CTRL_PANEL_STATUS                        23:23
#define SYSTEM_CTRL_PANEL_STATUS_CURRENT                0
#define SYSTEM_CTRL_PANEL_STATUS_PENDING                1
#define SYSTEM_CTRL_VIDEO_STATUS                        22:22
#define SYSTEM_CTRL_VIDEO_STATUS_CURRENT                0
#define SYSTEM_CTRL_VIDEO_STATUS_PENDING                1
#define SYSTEM_CTRL_DE_FIFO                             20:20
#define SYSTEM_CTRL_DE_FIFO_NOT_EMPTY                   0
#define SYSTEM_CTRL_DE_FIFO_EMPTY                       1
#define SYSTEM_CTRL_DE_STATUS                           19:19
#define SYSTEM_CTRL_DE_STATUS_IDLE                      0
#define SYSTEM_CTRL_DE_STATUS_BUSY                      1
#define SYSTEM_CTRL_CRT_STATUS                          17:17
#define SYSTEM_CTRL_CRT_STATUS_CURRENT                  0
#define SYSTEM_CTRL_CRT_STATUS_PENDING                  1
#define SYSTEM_CTRL_ZVPORT                              16:16
#define SYSTEM_CTRL_ZVPORT_0                            0
#define SYSTEM_CTRL_ZVPORT_1                            1
#define SYSTEM_CTRL_PCI_BURST_READ                      15:15
#define SYSTEM_CTRL_PCI_BURST_READ_DISABLE              0
#define SYSTEM_CTRL_PCI_BURST_READ_ENABLE               1
#define SYSTEM_CTRL_DE_ABORT                            13:12
#define SYSTEM_CTRL_DE_ABORT_NORMAL                     0
#define SYSTEM_CTRL_DE_ABORT_2D_ABORT                   3
#define SYSTEM_CTRL_PCI_SUBSYS_LOCK                     11:11
#define SYSTEM_CTRL_PCI_SUBSYS_LOCK_DISABLE             0
#define SYSTEM_CTRL_PCI_SUBSYS_LOCK_ENABLE              1
#define SYSTEM_CTRL_PCI_RETRY                           7:7
#define SYSTEM_CTRL_PCI_RETRY_ENABLE                    0
#define SYSTEM_CTRL_PCI_RETRY_DISABLE                   1
#define SYSTEM_CTRL_PCI_CLOCK_RUN                       6:6
#define SYSTEM_CTRL_PCI_CLOCK_RUN_DISABLE               0
#define SYSTEM_CTRL_PCI_CLOCK_RUN_ENABLE                1
#define SYSTEM_CTRL_PCI_SLAVE_BURST_READ_SIZE           5:4
#define SYSTEM_CTRL_PCI_SLAVE_BURST_READ_SIZE_1         0
#define SYSTEM_CTRL_PCI_SLAVE_BURST_READ_SIZE_2         1
#define SYSTEM_CTRL_PCI_SLAVE_BURST_READ_SIZE_4         2
#define SYSTEM_CTRL_PCI_SLAVE_BURST_READ_SIZE_8         3
#define SYSTEM_CTRL_CRT_TRISTATE                        2:2
#define SYSTEM_CTRL_CRT_TRISTATE_DISABLE                0
#define SYSTEM_CTRL_CRT_TRISTATE_ENABLE                 1
#define SYSTEM_CTRL_INTMEM_TRISTATE                     1:1
#define SYSTEM_CTRL_INTMEM_TRISTATE_DISABLE             0
#define SYSTEM_CTRL_INTMEM_TRISTATE_ENABLE              1
#define SYSTEM_CTRL_PANEL_TRISTATE                      0:0
#define SYSTEM_CTRL_PANEL_TRISTATE_DISABLE              0
#define SYSTEM_CTRL_PANEL_TRISTATE_ENABLE               1

#define CURRENT_POWER_CLOCK                             0x00003C
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT               29:29
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER              28:27
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_5            2
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT                26:24
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_7              7
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT               20:20
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER              19:19
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT                18:16
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_7              7
#define CURRENT_POWER_CLOCK_MCLK_SELECT                 12:12
#define CURRENT_POWER_CLOCK_MCLK_SELECT_288             0
#define CURRENT_POWER_CLOCK_MCLK_SELECT_336             1
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER                11:11
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER_1              0
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER_3              1
#define CURRENT_POWER_CLOCK_MCLK_SHIFT                  10:8
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_0                0
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_1                1
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_2                2
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_3                3
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_4                4
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_5                5
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_6                6
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_7                7
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT               4:4
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER              3:3
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT                2:0
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_7              7

#define CURRENT_POWER_GATE                              0x000038
#define CURRENT_POWER_GATE_AC97_I2S                     18:18
#define CURRENT_POWER_GATE_AC97_I2S_DISABLE             0
#define CURRENT_POWER_GATE_AC97_I2S_ENABLE              1
#define CURRENT_POWER_GATE_8051                         17:17
#define CURRENT_POWER_GATE_8051_DISABLE                 0
#define CURRENT_POWER_GATE_8051_ENABLE                  1
#define CURRENT_POWER_GATE_PLL                          16:16
#define CURRENT_POWER_GATE_PLL_DISABLE                  0
#define CURRENT_POWER_GATE_PLL_ENABLE                   1
#define CURRENT_POWER_GATE_OSCILLATOR                   15:15
#define CURRENT_POWER_GATE_OSCILLATOR_DISABLE           0
#define CURRENT_POWER_GATE_OSCILLATOR_ENABLE            1
#define CURRENT_POWER_GATE_PLL_RECOVERY                 14:13
#define CURRENT_POWER_GATE_PLL_RECOVERY_32              0
#define CURRENT_POWER_GATE_PLL_RECOVERY_64              1
#define CURRENT_POWER_GATE_PLL_RECOVERY_96              2
#define CURRENT_POWER_GATE_PLL_RECOVERY_128             3
#define CURRENT_POWER_GATE_USB_SLAVE                    12:12
#define CURRENT_POWER_GATE_USB_SLAVE_DISABLE            0
#define CURRENT_POWER_GATE_USB_SLAVE_ENABLE             1
#define CURRENT_POWER_GATE_USB_HOST                     11:11
#define CURRENT_POWER_GATE_USB_HOST_DISABLE             0
#define CURRENT_POWER_GATE_USB_HOST_ENABLE              1
#define CURRENT_POWER_GATE_SSP0_SSP1                    10:10
#define CURRENT_POWER_GATE_SSP0_SSP1_DISABLE            0
#define CURRENT_POWER_GATE_SSP0_SSP1_ENABLE             1
#define CURRENT_POWER_GATE_UART1                        8:8
#define CURRENT_POWER_GATE_UART1_DISABLE                0
#define CURRENT_POWER_GATE_UART1_ENABLE                 1
#define CURRENT_POWER_GATE_UART0                        7:7
#define CURRENT_POWER_GATE_UART0_DISABLE                0
#define CURRENT_POWER_GATE_UART0_ENABLE                 1
#define CURRENT_POWER_GATE_GPIO_PWM_I2C                 6:6
#define CURRENT_POWER_GATE_GPIO_PWM_I2C_DISABLE         0
#define CURRENT_POWER_GATE_GPIO_PWM_I2C_ENABLE          1
#define CURRENT_POWER_GATE_ZVPORT                       5:5
#define CURRENT_POWER_GATE_ZVPORT_DISABLE               0
#define CURRENT_POWER_GATE_ZVPORT_ENABLE                1
#define CURRENT_POWER_GATE_CSC                          4:4
#define CURRENT_POWER_GATE_CSC_DISABLE                  0
#define CURRENT_POWER_GATE_CSC_ENABLE                   1
#define CURRENT_POWER_GATE_2D                           3:3
#define CURRENT_POWER_GATE_2D_DISABLE                   0
#define CURRENT_POWER_GATE_2D_ENABLE                    1
#define CURRENT_POWER_GATE_DISPLAY                      2:2
#define CURRENT_POWER_GATE_DISPLAY_DISABLE              0
#define CURRENT_POWER_GATE_DISPLAY_ENABLE               1
#define CURRENT_POWER_GATE_INTMEM                       1:1
#define CURRENT_POWER_GATE_INTMEM_DISABLE               0
#define CURRENT_POWER_GATE_INTMEM_ENABLE                1
#define CURRENT_POWER_GATE_HOST                         0:0
#define CURRENT_POWER_GATE_HOST_DISABLE                 0
#define CURRENT_POWER_GATE_HOST_ENABLE                  1

#define CURRENT_POWER_CLOCK                             0x00003C
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT               29:29
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_P2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER              28:27
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_P2XCLK_DIVIDER_5            2
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT                26:24
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_P2XCLK_SHIFT_7              7
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT               20:20
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_V2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER              19:19
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_V2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT                18:16
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_V2XCLK_SHIFT_7              7
#define CURRENT_POWER_CLOCK_MCLK_SELECT                 12:12
#define CURRENT_POWER_CLOCK_MCLK_SELECT_288             0
#define CURRENT_POWER_CLOCK_MCLK_SELECT_336             1
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER                11:11
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER_1              0
#define CURRENT_POWER_CLOCK_MCLK_DIVIDER_3              1
#define CURRENT_POWER_CLOCK_MCLK_SHIFT                  10:8
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_0                0
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_1                1
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_2                2
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_3                3
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_4                4
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_5                5
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_6                6
#define CURRENT_POWER_CLOCK_MCLK_SHIFT_7                7
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT               4:4
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT_288           0
#define CURRENT_POWER_CLOCK_M2XCLK_SELECT_336           1
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER              3:3
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER_1            0
#define CURRENT_POWER_CLOCK_M2XCLK_DIVIDER_3            1
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT                2:0
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_0              0
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_1              1
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_2              2
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_3              3
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_4              4
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_5              5
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_6              6
#define CURRENT_POWER_CLOCK_M2XCLK_SHIFT_7              7

#define POWER_MODE0_GATE                                0x000040
#define POWER_MODE0_GATE_AC97_I2S                       18:18
#define POWER_MODE0_GATE_AC97_I2S_DISABLE               0
#define POWER_MODE0_GATE_AC97_I2S_ENABLE                1
#define POWER_MODE0_GATE_8051                           17:17
#define POWER_MODE0_GATE_8051_DISABLE                   0
#define POWER_MODE0_GATE_8051_ENABLE                    1
#define POWER_MODE0_GATE_USB_SLAVE                      12:12
#define POWER_MODE0_GATE_USB_SLAVE_DISABLE              0
#define POWER_MODE0_GATE_USB_SLAVE_ENABLE               1
#define POWER_MODE0_GATE_USB_HOST                       11:11
#define POWER_MODE0_GATE_USB_HOST_DISABLE               0
#define POWER_MODE0_GATE_USB_HOST_ENABLE                1
#define POWER_MODE0_GATE_SSP0_SSP1                      10:10
#define POWER_MODE0_GATE_SSP0_SSP1_DISABLE              0
#define POWER_MODE0_GATE_SSP0_SSP1_ENABLE               1
#define POWER_MODE0_GATE_UART1                          8:8
#define POWER_MODE0_GATE_UART1_DISABLE                  0
#define POWER_MODE0_GATE_UART1_ENABLE                   1
#define POWER_MODE0_GATE_UART0                          7:7
#define POWER_MODE0_GATE_UART0_DISABLE                  0
#define POWER_MODE0_GATE_UART0_ENABLE                   1
#define POWER_MODE0_GATE_GPIO_PWM_I2C                   6:6
#define POWER_MODE0_GATE_GPIO_PWM_I2C_DISABLE           0
#define POWER_MODE0_GATE_GPIO_PWM_I2C_ENABLE            1
#define POWER_MODE0_GATE_ZVPORT                         5:5
#define POWER_MODE0_GATE_ZVPORT_DISABLE                 0
#define POWER_MODE0_GATE_ZVPORT_ENABLE                  1
#define POWER_MODE0_GATE_CSC                            4:4
#define POWER_MODE0_GATE_CSC_DISABLE                    0
#define POWER_MODE0_GATE_CSC_ENABLE                     1
#define POWER_MODE0_GATE_2D                             3:3
#define POWER_MODE0_GATE_2D_DISABLE                     0
#define POWER_MODE0_GATE_2D_ENABLE                      1
#define POWER_MODE0_GATE_DISPLAY                        2:2
#define POWER_MODE0_GATE_DISPLAY_DISABLE                0
#define POWER_MODE0_GATE_DISPLAY_ENABLE                 1
#define POWER_MODE0_GATE_INTMEM                         1:1
#define POWER_MODE0_GATE_INTMEM_DISABLE                 0
#define POWER_MODE0_GATE_INTMEM_ENABLE                  1
#define POWER_MODE0_GATE_HOST                           0:0
#define POWER_MODE0_GATE_HOST_DISABLE                   0
#define POWER_MODE0_GATE_HOST_ENABLE                    1

#define POWER_MODE0_CLOCK                               0x000044
#define POWER_MODE0_CLOCK_P2XCLK_SELECT                 29:29
#define POWER_MODE0_CLOCK_P2XCLK_SELECT_288             0
#define POWER_MODE0_CLOCK_P2XCLK_SELECT_336             1
#define POWER_MODE0_CLOCK_P2XCLK_DIVIDER                28:27
#define POWER_MODE0_CLOCK_P2XCLK_DIVIDER_1              0
#define POWER_MODE0_CLOCK_P2XCLK_DIVIDER_3              1
#define POWER_MODE0_CLOCK_P2XCLK_DIVIDER_5              2
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT                  26:24
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_0                0
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_1                1
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_2                2
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_3                3
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_4                4
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_5                5
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_6                6
#define POWER_MODE0_CLOCK_P2XCLK_SHIFT_7                7
#define POWER_MODE0_CLOCK_V2XCLK_SELECT                 20:20
#define POWER_MODE0_CLOCK_V2XCLK_SELECT_288             0
#define POWER_MODE0_CLOCK_V2XCLK_SELECT_336             1
#define POWER_MODE0_CLOCK_V2XCLK_DIVIDER                19:19
#define POWER_MODE0_CLOCK_V2XCLK_DIVIDER_1              0
#define POWER_MODE0_CLOCK_V2XCLK_DIVIDER_3              1
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT                  18:16
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_0                0
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_1                1
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_2                2
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_3                3
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_4                4
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_5                5
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_6                6
#define POWER_MODE0_CLOCK_V2XCLK_SHIFT_7                7
#define POWER_MODE0_CLOCK_MCLK_SELECT                   12:12
#define POWER_MODE0_CLOCK_MCLK_SELECT_288               0
#define POWER_MODE0_CLOCK_MCLK_SELECT_336               1
#define POWER_MODE0_CLOCK_MCLK_DIVIDER                  11:11
#define POWER_MODE0_CLOCK_MCLK_DIVIDER_1                0
#define POWER_MODE0_CLOCK_MCLK_DIVIDER_3                1
#define POWER_MODE0_CLOCK_MCLK_SHIFT                    10:8
#define POWER_MODE0_CLOCK_MCLK_SHIFT_0                  0
#define POWER_MODE0_CLOCK_MCLK_SHIFT_1                  1
#define POWER_MODE0_CLOCK_MCLK_SHIFT_2                  2
#define POWER_MODE0_CLOCK_MCLK_SHIFT_3                  3
#define POWER_MODE0_CLOCK_MCLK_SHIFT_4                  4
#define POWER_MODE0_CLOCK_MCLK_SHIFT_5                  5
#define POWER_MODE0_CLOCK_MCLK_SHIFT_6                  6
#define POWER_MODE0_CLOCK_MCLK_SHIFT_7                  7
#define POWER_MODE0_CLOCK_M2XCLK_SELECT                 4:4
#define POWER_MODE0_CLOCK_M2XCLK_SELECT_288             0
#define POWER_MODE0_CLOCK_M2XCLK_SELECT_336             1
#define POWER_MODE0_CLOCK_M2XCLK_DIVIDER                3:3
#define POWER_MODE0_CLOCK_M2XCLK_DIVIDER_1              0
#define POWER_MODE0_CLOCK_M2XCLK_DIVIDER_3              1
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT                  2:0
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_0                0
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_1                1
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_2                2
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_3                3
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_4                4
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_5                5
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_6                6
#define POWER_MODE0_CLOCK_M2XCLK_SHIFT_7                7

#define POWER_MODE1_GATE                                0x000048
#define POWER_MODE1_GATE_AC97_I2S                       18:18
#define POWER_MODE1_GATE_AC97_I2S_DISABLE               0
#define POWER_MODE1_GATE_AC97_I2S_ENABLE                1
#define POWER_MODE1_GATE_8051                           17:17
#define POWER_MODE1_GATE_8051_DISABLE                   0
#define POWER_MODE1_GATE_8051_ENABLE                    1
#define POWER_MODE1_GATE_USB_SLAVE                      12:12
#define POWER_MODE1_GATE_USB_SLAVE_DISABLE              0
#define POWER_MODE1_GATE_USB_SLAVE_ENABLE               1
#define POWER_MODE1_GATE_USB_HOST                       11:11
#define POWER_MODE1_GATE_USB_HOST_DISABLE               0
#define POWER_MODE1_GATE_USB_HOST_ENABLE                1
#define POWER_MODE1_GATE_SSP0_SSP1                      10:10
#define POWER_MODE1_GATE_SSP0_SSP1_DISABLE              0
#define POWER_MODE1_GATE_SSP0_SSP1_ENABLE               1
#define POWER_MODE1_GATE_UART1                          8:8
#define POWER_MODE1_GATE_UART1_DISABLE                  0
#define POWER_MODE1_GATE_UART1_ENABLE                   1
#define POWER_MODE1_GATE_UART0                          7:7
#define POWER_MODE1_GATE_UART0_DISABLE                  0
#define POWER_MODE1_GATE_UART0_ENABLE                   1
#define POWER_MODE1_GATE_GPIO_PWM_I2C                   6:6
#define POWER_MODE1_GATE_GPIO_PWM_I2C_DISABLE           0
#define POWER_MODE1_GATE_GPIO_PWM_I2C_ENABLE            1
#define POWER_MODE1_GATE_ZVPORT                         5:5
#define POWER_MODE1_GATE_ZVPORT_DISABLE                 0
#define POWER_MODE1_GATE_ZVPORT_ENABLE                  1
#define POWER_MODE1_GATE_CSC                            4:4
#define POWER_MODE1_GATE_CSC_DISABLE                    0
#define POWER_MODE1_GATE_CSC_ENABLE                     1
#define POWER_MODE1_GATE_2D                             3:3
#define POWER_MODE1_GATE_2D_DISABLE                     0
#define POWER_MODE1_GATE_2D_ENABLE                      1
#define POWER_MODE1_GATE_DISPLAY                        2:2
#define POWER_MODE1_GATE_DISPLAY_DISABLE                0
#define POWER_MODE1_GATE_DISPLAY_ENABLE                 1
#define POWER_MODE1_GATE_INTMEM                         1:1
#define POWER_MODE1_GATE_INTMEM_DISABLE                 0
#define POWER_MODE1_GATE_INTMEM_ENABLE                  1
#define POWER_MODE1_GATE_HOST                           0:0
#define POWER_MODE1_GATE_HOST_DISABLE                   0
#define POWER_MODE1_GATE_HOST_ENABLE                    1

#define POWER_MODE1_CLOCK                               0x00004C
#define POWER_MODE1_CLOCK_P2XCLK_SELECT                 29:29
#define POWER_MODE1_CLOCK_P2XCLK_SELECT_288             0
#define POWER_MODE1_CLOCK_P2XCLK_SELECT_336             1
#define POWER_MODE1_CLOCK_P2XCLK_DIVIDER                28:27
#define POWER_MODE1_CLOCK_P2XCLK_DIVIDER_1              0
#define POWER_MODE1_CLOCK_P2XCLK_DIVIDER_3              1
#define POWER_MODE1_CLOCK_P2XCLK_DIVIDER_5              2
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT                  26:24
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_0                0
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_1                1
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_2                2
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_3                3
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_4                4
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_5                5
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_6                6
#define POWER_MODE1_CLOCK_P2XCLK_SHIFT_7                7
#define POWER_MODE1_CLOCK_V2XCLK_SELECT                 20:20
#define POWER_MODE1_CLOCK_V2XCLK_SELECT_288             0
#define POWER_MODE1_CLOCK_V2XCLK_SELECT_336             1
#define POWER_MODE1_CLOCK_V2XCLK_DIVIDER                19:19
#define POWER_MODE1_CLOCK_V2XCLK_DIVIDER_1              0
#define POWER_MODE1_CLOCK_V2XCLK_DIVIDER_3              1
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT                  18:16
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_0                0
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_1                1
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_2                2
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_3                3
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_4                4
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_5                5
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_6                6
#define POWER_MODE1_CLOCK_V2XCLK_SHIFT_7                7
#define POWER_MODE1_CLOCK_MCLK_SELECT                   12:12
#define POWER_MODE1_CLOCK_MCLK_SELECT_288               0
#define POWER_MODE1_CLOCK_MCLK_SELECT_336               1
#define POWER_MODE1_CLOCK_MCLK_DIVIDER                  11:11
#define POWER_MODE1_CLOCK_MCLK_DIVIDER_1                0
#define POWER_MODE1_CLOCK_MCLK_DIVIDER_3                1
#define POWER_MODE1_CLOCK_MCLK_SHIFT                    10:8
#define POWER_MODE1_CLOCK_MCLK_SHIFT_0                  0
#define POWER_MODE1_CLOCK_MCLK_SHIFT_1                  1
#define POWER_MODE1_CLOCK_MCLK_SHIFT_2                  2
#define POWER_MODE1_CLOCK_MCLK_SHIFT_3                  3
#define POWER_MODE1_CLOCK_MCLK_SHIFT_4                  4
#define POWER_MODE1_CLOCK_MCLK_SHIFT_5                  5
#define POWER_MODE1_CLOCK_MCLK_SHIFT_6                  6
#define POWER_MODE1_CLOCK_MCLK_SHIFT_7                  7
#define POWER_MODE1_CLOCK_M2XCLK_SELECT                 4:4
#define POWER_MODE1_CLOCK_M2XCLK_SELECT_288             0
#define POWER_MODE1_CLOCK_M2XCLK_SELECT_336             1
#define POWER_MODE1_CLOCK_M2XCLK_DIVIDER                3:3
#define POWER_MODE1_CLOCK_M2XCLK_DIVIDER_1              0
#define POWER_MODE1_CLOCK_M2XCLK_DIVIDER_3              1
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT                  2:0
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_0                0
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_1                1
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_2                2
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_3                3
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_4                4
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_5                5
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_6                6
#define POWER_MODE1_CLOCK_M2XCLK_SHIFT_7                7

#define POWER_SLEEP_GATE                                0x000050
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK             22:19
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_4096        0
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_2048        1
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_1024        2
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_512         3
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_256         4
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_128         5
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_64          6
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_32          7
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_16          8
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_8           9
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_4           10
#define POWER_SLEEP_GATE_PLL_RECOVERY_CLOCK_2           11
#define POWER_SLEEP_GATE_PLL_RECOVERY                   14:13
#define POWER_SLEEP_GATE_PLL_RECOVERY_32                0
#define POWER_SLEEP_GATE_PLL_RECOVERY_64                1
#define POWER_SLEEP_GATE_PLL_RECOVERY_96                2
#define POWER_SLEEP_GATE_PLL_RECOVERY_128               3

#define POWER_MODE_CTRL                                 0x000054
#define POWER_MODE_CTRL_SLEEP_STATUS                    2:2
#define POWER_MODE_CTRL_SLEEP_STATUS_INACTIVE           0
#define POWER_MODE_CTRL_SLEEP_STATUS_ACTIVE             1
#define POWER_MODE_CTRL_MODE                            1:0
#define POWER_MODE_CTRL_MODE_MODE0                      0
#define POWER_MODE_CTRL_MODE_MODE1                      1
#define POWER_MODE_CTRL_MODE_SLEEP                      2

#define MISC_CTRL                                       0x000004
#define MISC_CTRL_PCI_PAD                               31:30
#define MISC_CTRL_PCI_PAD_24MA                          0
#define MISC_CTRL_PCI_PAD_12MA                          1
#define MISC_CTRL_PCI_PAD_8MA                           2
#define MISC_CTRL_48_SELECT                             29:28
#define MISC_CTRL_48_SELECT_CRYSTAL                     0
#define MISC_CTRL_48_SELECT_CPU_96                      2
#define MISC_CTRL_48_SELECT_CPU_48                      3
#define MISC_CTRL_UART1_SELECT                          27:27
#define MISC_CTRL_UART1_SELECT_UART                     0
#define MISC_CTRL_UART1_SELECT_SSP                      1
#define MISC_CTRL_8051_LATCH                            26:26
#define MISC_CTRL_8051_LATCH_DISABLE                    0
#define MISC_CTRL_8051_LATCH_ENABLE                     1
#define MISC_CTRL_FPDATA                                25:25
#define MISC_CTRL_FPDATA_18                             0
#define MISC_CTRL_FPDATA_24                             1
#define MISC_CTRL_CRYSTAL                               24:24
#define MISC_CTRL_CRYSTAL_24                            0
#define MISC_CTRL_CRYSTAL_12                            1
#define MISC_CTRL_DRAM_REFRESH                          22:21
#define MISC_CTRL_DRAM_REFRESH_8                        0
#define MISC_CTRL_DRAM_REFRESH_16                       1
#define MISC_CTRL_DRAM_REFRESH_32                       2
#define MISC_CTRL_DRAM_REFRESH_64                       3
#define MISC_CTRL_BUS_HOLD                              20:18
#define MISC_CTRL_BUS_HOLD_FIFO_EMPTY                   0
#define MISC_CTRL_BUS_HOLD_8                            1
#define MISC_CTRL_BUS_HOLD_16                           2
#define MISC_CTRL_BUS_HOLD_24                           3
#define MISC_CTRL_BUS_HOLD_32                           4
#define MISC_CTRL_HITACHI_READY                         17:17
#define MISC_CTRL_HITACHI_READY_NEGATIVE                0
#define MISC_CTRL_HITACHI_READY_POSITIVE                1
#define MISC_CTRL_INTERRUPT                             16:16
#define MISC_CTRL_INTERRUPT_NORMAL                      0
#define MISC_CTRL_INTERRUPT_INVERT                      1
#define MISC_CTRL_PLL_CLOCK_COUNT                       15:15
#define MISC_CTRL_PLL_CLOCK_COUNT_DISABLE               0
#define MISC_CTRL_PLL_CLOCK_COUNT_ENABLE                1
#define MISC_CTRL_DAC_BAND_GAP                          14:13
#define MISC_CTRL_DAC_POWER                             12:12
#define MISC_CTRL_DAC_POWER_ENABLE                      0
#define MISC_CTRL_DAC_POWER_DISABLE                     1
#define MISC_CTRL_USB_SLAVE_CONTROLLER                  11:11
#define MISC_CTRL_USB_SLAVE_CONTROLLER_CPU              0
#define MISC_CTRL_USB_SLAVE_CONTROLLER_8051             1
#define MISC_CTRL_BURST_LENGTH                          10:10
#define MISC_CTRL_BURST_LENGTH_8                        0
#define MISC_CTRL_BURST_LENGTH_1                        1
#define MISC_CTRL_USB_SELECT                            9:9
#define MISC_CTRL_USB_SELECT_MASTER                     0
#define MISC_CTRL_USB_SELECT_SLAVE                      1
#define MISC_CTRL_LOOPBACK                              8:8
#define MISC_CTRL_LOOPBACK_NORMAL                       0
#define MISC_CTRL_LOOPBACK_USB_HOST                     1
#define MISC_CTRL_CLOCK_DIVIDER_RESET                   7:7
#define MISC_CTRL_CLOCK_DIVIDER_RESET_ENABLE            0
#define MISC_CTRL_CLOCK_DIVIDER_RESET_DISABLE           1
#define MISC_CTRL_TEST_MODE                             6:5
#define MISC_CTRL_TEST_MODE_NORMAL                      0
#define MISC_CTRL_TEST_MODE_DEBUGGING                   1
#define MISC_CTRL_TEST_MODE_NAND                        2
#define MISC_CTRL_TEST_MODE_MEMORY                      3
#define MISC_CTRL_NEC_MMIO                              4:4
#define MISC_CTRL_NEC_MMIO_30                           0
#define MISC_CTRL_NEC_MMIO_62                           1
#define MISC_CTRL_CLOCK                                 3:3
#define MISC_CTRL_CLOCK_PLL                             0
#define MISC_CTRL_CLOCK_TEST                            1
#define MISC_CTRL_HOST_BUS                              2:0
#define MISC_CTRL_HOST_BUS_HITACHI                      0
#define MISC_CTRL_HOST_BUS_PCI                          1
#define MISC_CTRL_HOST_BUS_XSCALE                       2
#define MISC_CTRL_HOST_BUS_STRONGARM                    4
#define MISC_CTRL_HOST_BUS_NEC                          6

#define CMD_INTPR_STATUS                                0x000024
#define CMD_INTPR_STATUS_2D_MEMORY_FIFO                 20:20
#define CMD_INTPR_STATUS_2D_MEMORY_FIFO_NOT_EMPTY       0
#define CMD_INTPR_STATUS_2D_MEMORY_FIFO_EMPTY           1
#define CMD_INTPR_STATUS_COMMAND_FIFO                   19:19
#define CMD_INTPR_STATUS_COMMAND_FIFO_NOT_EMPTY         0
#define CMD_INTPR_STATUS_COMMAND_FIFO_EMPTY             1
#define CMD_INTPR_STATUS_CSC_STATUS                     18:18
#define CMD_INTPR_STATUS_CSC_STATUS_IDLE                0
#define CMD_INTPR_STATUS_CSC_STATUS_BUSY                1
#define CMD_INTPR_STATUS_MEMORY_DMA                     17:17
#define CMD_INTPR_STATUS_MEMORY_DMA_IDLE                0
#define CMD_INTPR_STATUS_MEMORY_DMA_BUSY                1
#define CMD_INTPR_STATUS_CRT_STATUS                     16:16
#define CMD_INTPR_STATUS_CRT_STATUS_CURRENT             0
#define CMD_INTPR_STATUS_CRT_STATUS_PENDING             1
#define CMD_INTPR_STATUS_CURRENT_FIELD                  15:15
#define CMD_INTPR_STATUS_CURRENT_FIELD_ODD              0
#define CMD_INTPR_STATUS_CURRENT_FIELD_EVEN             1
#define CMD_INTPR_STATUS_VIDEO_STATUS                   14:14
#define CMD_INTPR_STATUS_VIDEO_STATUS_CURRENT           0
#define CMD_INTPR_STATUS_VIDEO_STATUS_PENDING           1
#define CMD_INTPR_STATUS_PANEL_STATUS                   13:13
#define CMD_INTPR_STATUS_PANEL_STATUS_CURRENT           0
#define CMD_INTPR_STATUS_PANEL_STATUS_PENDING           1
#define CMD_INTPR_STATUS_CRT_SYNC                       12:12
#define CMD_INTPR_STATUS_CRT_SYNC_INACTIVE              0
#define CMD_INTPR_STATUS_CRT_SYNC_ACTIVE                1
#define CMD_INTPR_STATUS_PANEL_SYNC                     11:11
#define CMD_INTPR_STATUS_PANEL_SYNC_INACTIVE            0
#define CMD_INTPR_STATUS_PANEL_SYNC_ACTIVE              1
#define CMD_INTPR_STATUS_2D_SETUP                       2:2
#define CMD_INTPR_STATUS_2D_SETUP_IDLE                  0
#define CMD_INTPR_STATUS_2D_SETUP_BUSY                  1
#define CMD_INTPR_STATUS_2D_FIFO                        1:1
#define CMD_INTPR_STATUS_2D_FIFO_NOT_EMPTY              0
#define CMD_INTPR_STATUS_2D_FIFO_EMPTY                  1
#define CMD_INTPR_STATUS_2D_ENGINE                      0:0
#define CMD_INTPR_STATUS_2D_ENGINE_IDLE                 0
#define CMD_INTPR_STATUS_2D_ENGINE_BUSY                 1

/* regDC.h */

/* Panel Graphics Control */
#define PANEL_DISPLAY_CTRL                              0x080000
#define PANEL_DISPLAY_CTRL_FPEN                         27:27
#define PANEL_DISPLAY_CTRL_FPEN_LOW                     0
#define PANEL_DISPLAY_CTRL_FPEN_HIGH                    1
#define PANEL_DISPLAY_CTRL_VBIASEN                      26:26
#define PANEL_DISPLAY_CTRL_VBIASEN_LOW                  0
#define PANEL_DISPLAY_CTRL_VBIASEN_HIGH                 1
#define PANEL_DISPLAY_CTRL_DATA                         25:25
#define PANEL_DISPLAY_CTRL_DATA_DISABLE                 0
#define PANEL_DISPLAY_CTRL_DATA_ENABLE                  1
#define PANEL_DISPLAY_CTRL_FPVDDEN                      24:24
#define PANEL_DISPLAY_CTRL_FPVDDEN_LOW                  0
#define PANEL_DISPLAY_CTRL_FPVDDEN_HIGH                 1
#define PANEL_DISPLAY_CTRL_PATTERN                      23:23
#define PANEL_DISPLAY_CTRL_PATTERN_4                    0
#define PANEL_DISPLAY_CTRL_PATTERN_8                    1
#define PANEL_DISPLAY_CTRL_TFT                          22:21
#define PANEL_DISPLAY_CTRL_TFT_24                       0
#define PANEL_DISPLAY_CTRL_TFT_9                        1
#define PANEL_DISPLAY_CTRL_TFT_12                       2
#define PANEL_DISPLAY_CTRL_DITHER                       20:20
#define PANEL_DISPLAY_CTRL_DITHER_DISABLE               0
#define PANEL_DISPLAY_CTRL_DITHER_ENABLE                1
#define PANEL_DISPLAY_CTRL_LCD                          19:18
#define PANEL_DISPLAY_CTRL_LCD_TFT                      0
#define PANEL_DISPLAY_CTRL_LCD_STN_8                    2
#define PANEL_DISPLAY_CTRL_LCD_STN_12                   3
#define PANEL_DISPLAY_CTRL_FIFO                         17:16
#define PANEL_DISPLAY_CTRL_FIFO_1                       0
#define PANEL_DISPLAY_CTRL_FIFO_3                       1
#define PANEL_DISPLAY_CTRL_FIFO_7                       2
#define PANEL_DISPLAY_CTRL_FIFO_11                      3
#define PANEL_DISPLAY_CTRL_CLOCK_PHASE                  14:14
#define PANEL_DISPLAY_CTRL_CLOCK_PHASE_ACTIVE_HIGH      0
#define PANEL_DISPLAY_CTRL_CLOCK_PHASE_ACTIVE_LOW       1
#define PANEL_DISPLAY_CTRL_VSYNC_PHASE                  13:13
#define PANEL_DISPLAY_CTRL_VSYNC_PHASE_ACTIVE_HIGH      0
#define PANEL_DISPLAY_CTRL_VSYNC_PHASE_ACTIVE_LOW       1
#define PANEL_DISPLAY_CTRL_HSYNC_PHASE                  12:12
#define PANEL_DISPLAY_CTRL_HSYNC_PHASE_ACTIVE_HIGH      0
#define PANEL_DISPLAY_CTRL_HSYNC_PHASE_ACTIVE_LOW       1
#define PANEL_DISPLAY_CTRL_COLOR_KEY                    9:9
#define PANEL_DISPLAY_CTRL_COLOR_KEY_DISABLE            0
#define PANEL_DISPLAY_CTRL_COLOR_KEY_ENABLE             1
#define PANEL_DISPLAY_CTRL_TIMING                       8:8
#define PANEL_DISPLAY_CTRL_TIMING_DISABLE               0
#define PANEL_DISPLAY_CTRL_TIMING_ENABLE                1
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN_DIR             7:7
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN_DIR_DOWN        0
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN_DIR_UP          1
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN                 6:6
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN_DISABLE         0
#define PANEL_DISPLAY_CTRL_VERTICAL_PAN_ENABLE          1
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN_DIR           5:5
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN_DIR_RIGHT     0
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN_DIR_LEFT      1
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN               4:4
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN_DISABLE       0
#define PANEL_DISPLAY_CTRL_HORIZONTAL_PAN_ENABLE        1
#define PANEL_DISPLAY_CTRL_GAMMA                        3:3
#define PANEL_DISPLAY_CTRL_GAMMA_DISABLE                0
#define PANEL_DISPLAY_CTRL_GAMMA_ENABLE                 1
#define PANEL_DISPLAY_CTRL_PLANE                        2:2
#define PANEL_DISPLAY_CTRL_PLANE_DISABLE                0
#define PANEL_DISPLAY_CTRL_PLANE_ENABLE                 1
#define PANEL_DISPLAY_CTRL_FORMAT                       1:0
#define PANEL_DISPLAY_CTRL_FORMAT_8                     0
#define PANEL_DISPLAY_CTRL_FORMAT_16                    1
#define PANEL_DISPLAY_CTRL_FORMAT_32                    2

#define PANEL_PAN_CTRL                                  0x080004
#define PANEL_PAN_CTRL_VERTICAL_PAN                     31:24
#define PANEL_PAN_CTRL_VERTICAL_VSYNC                   21:16
#define PANEL_PAN_CTRL_HORIZONTAL_PAN                   15:8
#define PANEL_PAN_CTRL_HORIZONTAL_VSYNC                 5:0

#define PANEL_COLOR_KEY                                 0x080008
#define PANEL_COLOR_KEY_MASK                            31:16
#define PANEL_COLOR_KEY_VALUE                           15:0

#define PANEL_FB_ADDRESS                                0x08000C
#define PANEL_FB_ADDRESS_STATUS                         31:31
#define PANEL_FB_ADDRESS_STATUS_CURRENT                 0
#define PANEL_FB_ADDRESS_STATUS_PENDING                 1
#define PANEL_FB_ADDRESS_EXT                            27:27
#define PANEL_FB_ADDRESS_EXT_LOCAL                      0
#define PANEL_FB_ADDRESS_EXT_EXTERNAL                   1
#define PANEL_FB_ADDRESS_CS                             26:26
#define PANEL_FB_ADDRESS_CS_0                           0
#define PANEL_FB_ADDRESS_CS_1                           1
#define PANEL_FB_ADDRESS_ADDRESS                        25:0

#define PANEL_FB_WIDTH                                  0x080010
#define PANEL_FB_WIDTH_WIDTH                            29:16
#define PANEL_FB_WIDTH_OFFSET                           13:0

#define PANEL_WINDOW_WIDTH                              0x080014
#define PANEL_WINDOW_WIDTH_WIDTH                        27:16
#define PANEL_WINDOW_WIDTH_X                            11:0

#define PANEL_WINDOW_HEIGHT                             0x080018
#define PANEL_WINDOW_HEIGHT_HEIGHT                      27:16
#define PANEL_WINDOW_HEIGHT_Y                           11:0

#define PANEL_PLANE_TL                                  0x08001C
#define PANEL_PLANE_TL_TOP                              26:16
#define PANEL_PLANE_TL_LEFT                             10:0

#define PANEL_PLANE_BR                                  0x080020
#define PANEL_PLANE_BR_BOTTOM                           26:16
#define PANEL_PLANE_BR_RIGHT                            10:0

#define PANEL_HORIZONTAL_TOTAL                          0x080024
#define PANEL_HORIZONTAL_TOTAL_TOTAL                    27:16
#define PANEL_HORIZONTAL_TOTAL_DISPLAY_END              11:0

#define PANEL_HORIZONTAL_SYNC                           0x080028
#define PANEL_HORIZONTAL_SYNC_WIDTH                     23:16
#define PANEL_HORIZONTAL_SYNC_START                     11:0

#define PANEL_VERTICAL_TOTAL                            0x08002C
#define PANEL_VERTICAL_TOTAL_TOTAL                      26:16
#define PANEL_VERTICAL_TOTAL_DISPLAY_END                10:0

#define PANEL_VERTICAL_SYNC                             0x080030
#define PANEL_VERTICAL_SYNC_HEIGHT                      21:16
#define PANEL_VERTICAL_SYNC_START                       10:0

#define PANEL_CURRENT_LINE                              0x080034
#define PANEL_CURRENT_LINE_LINE                         10:0

/* CRT Graphics Control */

#define CRT_DISPLAY_CTRL                                0x080200
#define CRT_DISPLAY_CTRL_FIFO                           17:16
#define CRT_DISPLAY_CTRL_FIFO_1                         0
#define CRT_DISPLAY_CTRL_FIFO_3                         1
#define CRT_DISPLAY_CTRL_FIFO_7                         2
#define CRT_DISPLAY_CTRL_FIFO_11                        3
#define CRT_DISPLAY_CTRL_TV_PHASE                       15:15
#define CRT_DISPLAY_CTRL_TV_PHASE_ACTIVE_HIGH           0
#define CRT_DISPLAY_CTRL_TV_PHASE_ACTIVE_LOW            1
#define CRT_DISPLAY_CTRL_CLOCK_PHASE                    14:14
#define CRT_DISPLAY_CTRL_CLOCK_PHASE_ACTIVE_HIGH        0
#define CRT_DISPLAY_CTRL_CLOCK_PHASE_ACTIVE_LOW         1
#define CRT_DISPLAY_CTRL_VSYNC_PHASE                    13:13
#define CRT_DISPLAY_CTRL_VSYNC_PHASE_ACTIVE_HIGH        0
#define CRT_DISPLAY_CTRL_VSYNC_PHASE_ACTIVE_LOW         1
#define CRT_DISPLAY_CTRL_HSYNC_PHASE                    12:12
#define CRT_DISPLAY_CTRL_HSYNC_PHASE_ACTIVE_HIGH        0
#define CRT_DISPLAY_CTRL_HSYNC_PHASE_ACTIVE_LOW         1
#define CRT_DISPLAY_CTRL_BLANK                          10:10
#define CRT_DISPLAY_CTRL_BLANK_OFF                      0
#define CRT_DISPLAY_CTRL_BLANK_ON                       1
#define CRT_DISPLAY_CTRL_SELECT                         9:9
#define CRT_DISPLAY_CTRL_SELECT_PANEL                   0
#define CRT_DISPLAY_CTRL_SELECT_CRT                     1
#define CRT_DISPLAY_CTRL_TIMING                         8:8
#define CRT_DISPLAY_CTRL_TIMING_DISABLE                 0
#define CRT_DISPLAY_CTRL_TIMING_ENABLE                  1
#define CRT_DISPLAY_CTRL_PIXEL                          7:4
#define CRT_DISPLAY_CTRL_GAMMA                          3:3
#define CRT_DISPLAY_CTRL_GAMMA_DISABLE                  0
#define CRT_DISPLAY_CTRL_GAMMA_ENABLE                   1
#define CRT_DISPLAY_CTRL_PLANE                          2:2
#define CRT_DISPLAY_CTRL_PLANE_DISABLE                  0
#define CRT_DISPLAY_CTRL_PLANE_ENABLE                   1
#define CRT_DISPLAY_CTRL_FORMAT                         1:0
#define CRT_DISPLAY_CTRL_FORMAT_8                       0
#define CRT_DISPLAY_CTRL_FORMAT_16                      1
#define CRT_DISPLAY_CTRL_FORMAT_32                      2

#define CRT_FB_ADDRESS                                  0x080204
#define CRT_FB_ADDRESS_STATUS                           31:31
#define CRT_FB_ADDRESS_STATUS_CURRENT                   0
#define CRT_FB_ADDRESS_STATUS_PENDING                   1
#define CRT_FB_ADDRESS_EXT                              27:27
#define CRT_FB_ADDRESS_EXT_LOCAL                        0
#define CRT_FB_ADDRESS_EXT_EXTERNAL                     1
#define CRT_FB_ADDRESS_CS                               26:26
#define CRT_FB_ADDRESS_CS_0                             0
#define CRT_FB_ADDRESS_CS_1                             1
#define CRT_FB_ADDRESS_ADDRESS                          25:0

#define CRT_FB_WIDTH                                    0x080208
#define CRT_FB_WIDTH_WIDTH                              29:16
#define CRT_FB_WIDTH_OFFSET                             13:0

#define CRT_HORIZONTAL_TOTAL                            0x08020C
#define CRT_HORIZONTAL_TOTAL_TOTAL                      27:16
#define CRT_HORIZONTAL_TOTAL_DISPLAY_END                11:0

#define CRT_HORIZONTAL_SYNC                             0x080210
#define CRT_HORIZONTAL_SYNC_WIDTH                       23:16
#define CRT_HORIZONTAL_SYNC_START                       11:0

#define CRT_VERTICAL_TOTAL                              0x080214
#define CRT_VERTICAL_TOTAL_TOTAL                        26:16
#define CRT_VERTICAL_TOTAL_DISPLAY_END                  10:0

#define CRT_VERTICAL_SYNC                               0x080218
#define CRT_VERTICAL_SYNC_HEIGHT                        21:16
#define CRT_VERTICAL_SYNC_START                         10:0

#define CRT_SIGNATURE_ANALYZER                          0x08021C
#define CRT_SIGNATURE_ANALYZER_STATUS                   31:16
#define CRT_SIGNATURE_ANALYZER_ENABLE                   3:3
#define CRT_SIGNATURE_ANALYZER_ENABLE_DISABLE           0
#define CRT_SIGNATURE_ANALYZER_ENABLE_ENABLE            1
#define CRT_SIGNATURE_ANALYZER_RESET                    2:2
#define CRT_SIGNATURE_ANALYZER_RESET_NORMAL             0
#define CRT_SIGNATURE_ANALYZER_RESET_RESET              1
#define CRT_SIGNATURE_ANALYZER_SOURCE                   1:0
#define CRT_SIGNATURE_ANALYZER_SOURCE_RED               0
#define CRT_SIGNATURE_ANALYZER_SOURCE_GREEN             1
#define CRT_SIGNATURE_ANALYZER_SOURCE_BLUE              2

#define CRT_CURRENT_LINE                                0x080220
#define CRT_CURRENT_LINE_LINE                           10:0

#define CRT_MONITOR_DETECT                              0x080224
#define CRT_MONITOR_DETECT_ENABLE                       24:24
#define CRT_MONITOR_DETECT_ENABLE_DISABLE               0
#define CRT_MONITOR_DETECT_ENABLE_ENABLE                1
#define CRT_MONITOR_DETECT_RED                          23:16
#define CRT_MONITOR_DETECT_GREEN                        15:8
#define CRT_MONITOR_DETECT_BLUE                         7:0

/* CRT Cursor Control */

#define CRT_HWC_ADDRESS                                 0x080230
#define CRT_HWC_ADDRESS_ENABLE                          31:31
#define CRT_HWC_ADDRESS_ENABLE_DISABLE                  0
#define CRT_HWC_ADDRESS_ENABLE_ENABLE                   1
#define CRT_HWC_ADDRESS_EXT                             27:27
#define CRT_HWC_ADDRESS_EXT_LOCAL                       0
#define CRT_HWC_ADDRESS_EXT_EXTERNAL                    1
#define CRT_HWC_ADDRESS_CS                              26:26
#define CRT_HWC_ADDRESS_CS_0                            0
#define CRT_HWC_ADDRESS_CS_1                            1
#define CRT_HWC_ADDRESS_ADDRESS                         25:0

#define CRT_HWC_LOCATION                                0x080234
#define CRT_HWC_LOCATION_TOP                            27:27
#define CRT_HWC_LOCATION_TOP_INSIDE                     0
#define CRT_HWC_LOCATION_TOP_OUTSIDE                    1
#define CRT_HWC_LOCATION_Y                              26:16
#define CRT_HWC_LOCATION_LEFT                           11:11
#define CRT_HWC_LOCATION_LEFT_INSIDE                    0
#define CRT_HWC_LOCATION_LEFT_OUTSIDE                   1
#define CRT_HWC_LOCATION_X                              10:0

#define CRT_HWC_COLOR_12                                0x080238
#define CRT_HWC_COLOR_12_2_RGB565                       31:16
#define CRT_HWC_COLOR_12_1_RGB565                       15:0

#define CRT_HWC_COLOR_3                                 0x08023C
#define CRT_HWC_COLOR_3_RGB565                          15:0

#define CRT_HWC_COLOR_01                                0x080238
#define CRT_HWC_COLOR_01_1_RED                          31:27
#define CRT_HWC_COLOR_01_1_GREEN                        26:21
#define CRT_HWC_COLOR_01_1_BLUE                         20:16
#define CRT_HWC_COLOR_01_0_RED                          15:11
#define CRT_HWC_COLOR_01_0_GREEN                        10:5
#define CRT_HWC_COLOR_01_0_BLUE                         4:0

#define CRT_HWC_COLOR_2                                 0x08023C
#define CRT_HWC_COLOR_2_RED                             15:11
#define CRT_HWC_COLOR_2_GREEN                           10:5
#define CRT_HWC_COLOR_2_BLUE                            4:0

#define CRT_PALETTE_RAM                                 0x080400
#define PANEL_PALETTE_RAM                               0x080800
#define VIDEO_PALETTE_RAM                               0x080C00

#endif  /*_SMI_H*/
