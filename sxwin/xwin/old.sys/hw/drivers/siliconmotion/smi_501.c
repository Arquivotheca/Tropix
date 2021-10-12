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

Except as contained in this notice, the names of The XFree86 Project and
Silicon Motion shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization from The XFree86 Project or Silicon Motion.
*/
/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/siliconmotion/smi_501.c,v 1.1 2006/03/09 17:37:08 tsi Exp $ */

#include "xf86Resources.h"
#include "xf86RAC.h"
#include "xf86DDC.h"
#include "xf86int10.h"
#include "vbe.h"
#include "shadowfb.h"

#include "smi.h"
#include "smi_501.h"

#include "globals.h"
#define DPMS_SERVER
#include <X11/extensions/dpms.h>

/*
 * Forward definitions for the functions that make up the driver.
 */

static VOID panelPowerSequence(SMIPtr pSmi, panel_state_t on_off, INT vsync_delay);
static VOID setDPMS(SMIPtr pSmi, DPMS_t state);
static VOID setPower(SMIPtr pSmi, ULONG nGates, ULONG Clock);

/*
 * Add comment here about this module.
 */

/* Mode table. */
static mode_table_t mode_table[] =
{
    /*----------------------------------------------------------------------------------------
     * H.   H.    H.     H.   H.        V.   V.    V.    V.   V.        Pixel     H.     V.
     * tot. disp. sync   sync sync      tot. disp. sync  sync sinc      clock     freq.  freq.
     *      end   start  wdth polarity       end   start hght polarity
     *---------------------------------------------------------------------------------------*/

    /* 640 x 480 */
    {  800, 640,  656,   96,  NEGATIVE, 525, 480,  490,  2,   NEGATIVE, 25175000, 31469, 60 },
    {  832, 640,  664,   40,  NEGATIVE, 520, 480,  489,  3,   NEGATIVE, 31500000, 37861, 72 },
    {  840, 640,  656,   64,  NEGATIVE, 500, 480,  481,  3,   NEGATIVE, 31500000, 37500, 75 },
    {  832, 640,  696,   56,  NEGATIVE, 509, 480,  481,  3,   NEGATIVE, 36000000, 43269, 85 },

    /* 800 x 600 */
    { 1024, 800,  824,   72,  POSITIVE, 625, 600,  601,  2,   POSITIVE, 36000000, 35156, 56 },
    { 1056, 800,  840,  128,  POSITIVE, 628, 600,  601,  4,   POSITIVE, 40000000, 37879, 60 },
    { 1040, 800,  856,  120,  POSITIVE, 666, 600,  637,  6,   POSITIVE, 50000000, 48077, 72 },
    { 1056, 800,  816,   80,  POSITIVE, 625, 600,  601,  3,   POSITIVE, 49500000, 46875, 75 },
    { 1048, 800,  832,   64,  POSITIVE, 631, 600,  601,  3,   POSITIVE, 56250000, 53674, 85 },

    /* 1024 x 768*/
    { 1344, 1024, 1048, 136,  NEGATIVE, 806, 768,  771,  6,   NEGATIVE, 65000000, 48363, 60 },
    { 1328, 1024, 1048, 136,  NEGATIVE, 806, 768,  771,  6,   NEGATIVE, 75000000, 56476, 70 },
    { 1312, 1024, 1040,  96,  POSITIVE, 800, 768,  769,  3,   POSITIVE, 78750000, 60023, 75 },
    { 1376, 1024, 1072,  96,  POSITIVE, 808, 768,  769,  3,   POSITIVE, 94500000, 68677, 85 },

    /* End of table. */
    { 0, 0, 0, 0, NEGATIVE, 0, 0, 0, 0, NEGATIVE, 0, 0, 0 },
};

/**********************************************************************
 * regRead32
 *    Read the value of the 32-bit register specified by nOffset
 **********************************************************************/
static ULONG regRead32(SMIPtr pSmi, ULONG nOffset)
{
    ULONG result;
    result = READ_SCR(pSmi, nOffset);
    /* ErrorF("READ_SCR %8X from register %8X\n", result, nOffset); */
    return (result);
}

/**********************************************************************
 * regWrite32
 *    Write the 32-bit value, nData, to the 32-bit register specified by
 *    nOffset
 **********************************************************************/
static void regWrite32(SMIPtr pSmi, ULONG nOffset, ULONG nData)
{
    /* ErrorF("MMIOWriting %8X to register %8X\n", nData, nOffset);  */
    WRITE_SCR(pSmi, nOffset, nData);
}

/* Perform a rounded division. */
static LONG roundDiv(LONG num, LONG denom)
{
    /* n / d + 1 / 2 = (2n + d) / 2d */
    return (2 * num + denom) / (2 * denom);
}

/* Finds clock closest to the requested. */
static LONG findClock(LONG requested_clock, clock_select_t *clock, display_t display)
{
    LONG mclk;
    INT divider, shift;
    LONG best_diff = 999999999;

    /* Try 288MHz and 336MHz clocks. */
    for (mclk = 288000000; mclk <= 336000000; mclk += 48000000)
    {
        /* For CRT, try dividers 1 and 3, for panel, try divider 5 as well. */
        for (divider = 1; divider <= (display == PANEL ? 5 : 3);
             divider += 2)
        {
            /* Try all 8 shift values. */
            for (shift = 0; shift < 8; shift++)
            {
                /* Calculate difference with requested clock. */
                LONG diff = roundDiv(mclk, divider << shift) - requested_clock;
                if (diff < 0)
                {
                    diff = -diff;
                }

                /* If the difference is less than the current, use it. */
                if (diff < best_diff)
                {
                    /* Store best difference. */
                    best_diff = diff;

                    /* Store clock values. */
                    clock->mclk = mclk;
                    clock->divider = divider;
                    clock->shift = shift;
                }
            }
        }
    }

    /* Return best clock. */
    return clock->mclk / (clock->divider << clock->shift);
}

/* Finds the requested mode in the mode table. */
static mode_table_t *findMode(mode_table_t *mode_table, INT width, INT height,
                       INT refresh_rate)
{
    /* Walk the entire mode table. */
    while (mode_table->pixel_clock != 0)
    {
        /* If this mode matches the requested mode, return it! */
        if ((mode_table->horizontal_display_end == width)
        && (mode_table->vertical_display_end == height)
        && (mode_table->vertical_frequency == refresh_rate))
        {
            return(mode_table);
        }

        /* Next entry in the mode table. */
        mode_table++;
    }

    /* No mode found. */
    return(NULL);
}

/* Converts the VESA timing into Voyager timing. */
static void adjustMode(mode_table_t *vesaMode, mode_table_t *mode, display_t display)
{
    LONG blank_width, sync_start, sync_width;
    clock_select_t clock;

    /* Calculate the VESA line and screen frequencies. */
    vesaMode->horizontal_frequency = roundDiv(vesaMode->pixel_clock,
                                           vesaMode->horizontal_total);
    vesaMode->vertical_frequency = roundDiv(vesaMode->horizontal_frequency,
                                         vesaMode->vertical_total);

    /* Calculate the sync percentages of the VESA mode. */
    blank_width = vesaMode->horizontal_total
                - vesaMode->horizontal_display_end;
    sync_start = roundDiv((vesaMode->horizontal_sync_start -
                       vesaMode->horizontal_display_end) * 100, blank_width);
    sync_width = roundDiv(vesaMode->horizontal_sync_width * 100, blank_width);

    /* Copy VESA mode into Voyager mode. */
    *mode = *vesaMode;

    /* Find the best pixel clock. */
    mode->pixel_clock = findClock(vesaMode->pixel_clock * 2, &clock, display) / 2;

    /* Calculate the horizontal total based on the pixel clock and VESA line
     * frequency.
     */
    mode->horizontal_total = roundDiv(mode->pixel_clock,
                                   vesaMode->horizontal_frequency);

    /* Calculate the sync start and width based on the VESA percentages. */
    blank_width = mode->horizontal_total - mode->horizontal_display_end;
    mode->horizontal_sync_start = mode->horizontal_display_end
                                + roundDiv(blank_width * sync_start, 100);
    mode->horizontal_sync_width = roundDiv(blank_width * sync_width, 100);

    /* Calculate the line and screen frequencies. */
    mode->horizontal_frequency = roundDiv(mode->pixel_clock,
                                       mode->horizontal_total);
    mode->vertical_frequency = roundDiv(mode->horizontal_frequency,
                                     mode->vertical_total);
}

/* Fill the register structure. */
static void setModeRegisters(reg_table_t *register_table, mode_table_t *mode,
                      display_t display, INT bpp, INT fbPitch)
{
    clock_select_t clock;
    memset(&clock, 0, sizeof(clock));

    /* Calculate the clock register values. */
    findClock(mode->pixel_clock * 2, &clock, display);

    if (display == PANEL)
    {
        /* Set clock value for panel. */
        register_table->clock
            = (clock.mclk == 288000000
                ? FIELD_SET(0, CURRENT_POWER_CLOCK, P2XCLK_SELECT, 288)
                : FIELD_SET(0, CURRENT_POWER_CLOCK, P2XCLK_SELECT, 336))
            | (clock.divider == 1
                ? FIELD_SET(0, CURRENT_POWER_CLOCK, P2XCLK_DIVIDER, 1)
                : (clock.divider == 3
                    ? FIELD_SET(0, CURRENT_POWER_CLOCK, P2XCLK_DIVIDER, 3)
                    : FIELD_SET(0, CURRENT_POWER_CLOCK, P2XCLK_DIVIDER, 5)))
            | FIELD_VALUE(0, CURRENT_POWER_CLOCK, P2XCLK_SHIFT, clock.shift);

        /* Set control register value. */
        register_table->control
            = (mode->vertical_sync_polarity == POSITIVE
                ? FIELD_SET(0, PANEL_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_HIGH)
                : FIELD_SET(0, PANEL_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_LOW))
            | (mode->horizontal_sync_polarity == POSITIVE
                ? FIELD_SET(0, PANEL_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_HIGH)
                : FIELD_SET(0, PANEL_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_LOW))
            | FIELD_SET(0, PANEL_DISPLAY_CTRL, TIMING, ENABLE)
            | FIELD_SET(0, PANEL_DISPLAY_CTRL, PLANE, ENABLE)
            | (bpp == 8
                ? FIELD_SET(0, PANEL_DISPLAY_CTRL, FORMAT, 8)
                : (bpp == 16
                    ? FIELD_SET(0, PANEL_DISPLAY_CTRL, FORMAT, 16)
                    : FIELD_SET(0, PANEL_DISPLAY_CTRL, FORMAT, 32)));




        /* Set timing registers. */
        register_table->horizontal_total
            = FIELD_VALUE(0, PANEL_HORIZONTAL_TOTAL, TOTAL,
                          mode->horizontal_total - 1)
            | FIELD_VALUE(0, PANEL_HORIZONTAL_TOTAL, DISPLAY_END,
                          mode->horizontal_display_end - 1);

        register_table->horizontal_sync
            = FIELD_VALUE(0, PANEL_HORIZONTAL_SYNC, WIDTH,
                          mode->horizontal_sync_width)
            | FIELD_VALUE(0, PANEL_HORIZONTAL_SYNC, START,
                          mode->horizontal_sync_start - 1);

        register_table->vertical_total
            = FIELD_VALUE(0, PANEL_VERTICAL_TOTAL, TOTAL,
                          mode->vertical_total - 1)
            | FIELD_VALUE(0, PANEL_VERTICAL_TOTAL, DISPLAY_END,
                          mode->vertical_display_end - 1);

        register_table->vertical_sync
            = FIELD_VALUE(0, PANEL_VERTICAL_SYNC, HEIGHT,
                          mode->vertical_sync_height)
            | FIELD_VALUE(0, PANEL_VERTICAL_SYNC, START,
                          mode->vertical_sync_start - 1);
    }
    else
    {
        /* Set clock value for CRT. */
        register_table->clock
            = (clock.mclk == 288000000
                ? FIELD_SET(0, CURRENT_POWER_CLOCK, V2XCLK_SELECT, 288)
                : FIELD_SET(0, CURRENT_POWER_CLOCK, V2XCLK_SELECT, 336))
            | (clock.divider == 1
                ? FIELD_SET(0, CURRENT_POWER_CLOCK, V2XCLK_DIVIDER, 1)
                : FIELD_SET(0, CURRENT_POWER_CLOCK, V2XCLK_DIVIDER, 3))
            | FIELD_VALUE(0, CURRENT_POWER_CLOCK, V2XCLK_SHIFT, clock.shift);

        /* Set control register value.*/
        register_table->control
            = (mode->vertical_sync_polarity == POSITIVE
                ? FIELD_SET(0, CRT_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_HIGH)
                : FIELD_SET(0, CRT_DISPLAY_CTRL, VSYNC_PHASE, ACTIVE_LOW))
            | (mode->horizontal_sync_polarity == POSITIVE
                ? FIELD_SET(0, CRT_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_HIGH)
                : FIELD_SET(0, CRT_DISPLAY_CTRL, HSYNC_PHASE, ACTIVE_LOW))
            | FIELD_SET(0, CRT_DISPLAY_CTRL, SELECT, CRT)
            | FIELD_SET(0, CRT_DISPLAY_CTRL, TIMING, ENABLE)
            | FIELD_SET(0, CRT_DISPLAY_CTRL, PLANE, ENABLE)
            | (bpp == 8
                ? FIELD_SET(0, CRT_DISPLAY_CTRL, FORMAT, 8)
                : (bpp == 16
                    ? FIELD_SET(0, CRT_DISPLAY_CTRL, FORMAT, 16)
                    : FIELD_SET(0, CRT_DISPLAY_CTRL, FORMAT, 32)));

        /* Set timing registers. */
        register_table->horizontal_total
            = FIELD_VALUE(0, CRT_HORIZONTAL_TOTAL, TOTAL,
                          mode->horizontal_total - 1)
            | FIELD_VALUE(0, CRT_HORIZONTAL_TOTAL, DISPLAY_END,
                          mode->horizontal_display_end - 1);

        register_table->horizontal_sync
            = FIELD_VALUE(0, CRT_HORIZONTAL_SYNC, WIDTH,
                          mode->horizontal_sync_width)
            | FIELD_VALUE(0, CRT_HORIZONTAL_SYNC, START,
                          mode->horizontal_sync_start - 1);

        register_table->vertical_total
            = FIELD_VALUE(0, CRT_VERTICAL_TOTAL, TOTAL,
                          mode->vertical_total - 1)
            | FIELD_VALUE(0, CRT_VERTICAL_TOTAL, DISPLAY_END,
                          mode->vertical_display_end - 1);
        register_table->vertical_sync
            = FIELD_VALUE(0, CRT_VERTICAL_SYNC, HEIGHT,
                          mode->vertical_sync_height)
            | FIELD_VALUE(0, CRT_VERTICAL_SYNC, START,
                          mode->vertical_sync_start - 1);
    }

    /* Set up framebuffer pitch, from passed in value */
    register_table->fb_width = mode->horizontal_display_end * (bpp / 8);
    register_table->fb_width = fbPitch;

    /* Calculate frame buffer width and height. */
    register_table->width    = mode->horizontal_display_end;
    register_table->height   = mode->vertical_display_end;

    /* Save display type. */
    register_table->display = display;
}

/* Program the mode with the registers specified. */
static void programMode(SMIPtr pSmi, reg_table_t *register_table)
{
    DWORD value, gate, clock;
    DWORD palette_ram;
    DWORD fb_size, offset;

    /* Get current power configuration. */
    gate = regRead32(pSmi, CURRENT_POWER_GATE);
    gate |= 0x08;   /* Enable power to 2D engine */
    gate = FIELD_SET(gate, CURRENT_POWER_GATE, CSC,          ENABLE);
    gate = FIELD_SET(gate, CURRENT_POWER_GATE, ZVPORT,       ENABLE);
    gate = FIELD_SET(gate, CURRENT_POWER_GATE, GPIO_PWM_I2C, ENABLE);

    clock = regRead32(pSmi, CURRENT_POWER_CLOCK);

    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, MCLK_SELECT, 336);
    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, MCLK_DIVIDER, 3);
    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, MCLK_SHIFT, 0);
    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, M2XCLK_SELECT, 336);
    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, M2XCLK_DIVIDER, 1);
    clock = FIELD_SET(clock, CURRENT_POWER_CLOCK, M2XCLK_SHIFT, 1);

    /* Program panel. */
    if (register_table->display == PANEL)
    {
        /* Program clock, enable display controller. */
        gate = FIELD_SET(gate, CURRENT_POWER_GATE, DISPLAY, ENABLE);
        clock &= FIELD_CLEAR(CURRENT_POWER_CLOCK, P2XCLK_SELECT)
              &  FIELD_CLEAR(CURRENT_POWER_CLOCK, P2XCLK_DIVIDER)
              &  FIELD_CLEAR(CURRENT_POWER_CLOCK, P2XCLK_SHIFT);
        setPower(pSmi, gate, clock | register_table->clock);

        /* Calculate frame buffer address. */
        value = 0;
        fb_size = register_table->fb_width * register_table->height;
        if (FIELD_GET(regRead32(pSmi, CRT_DISPLAY_CTRL),
                      CRT_DISPLAY_CTRL,
                      PLANE) == CRT_DISPLAY_CTRL_PLANE_ENABLE)
        {
            value = FIELD_GET(regRead32(pSmi, CRT_FB_ADDRESS),
                              CRT_FB_ADDRESS,
                              ADDRESS);
            if (fb_size < value)
            {
                value = 0;
            }
            else
            {
                value += FIELD_GET(regRead32(pSmi, CRT_FB_WIDTH),
                                   CRT_FB_WIDTH,
                                   OFFSET)
                      *  (FIELD_GET(regRead32(pSmi, CRT_VERTICAL_TOTAL),
                                   CRT_VERTICAL_TOTAL,
                                   DISPLAY_END) + 1);
            }
        }

        /* Program panel registers. */
        regWrite32(pSmi, PANEL_FB_ADDRESS,
                          FIELD_SET(0, PANEL_FB_ADDRESS, STATUS, PENDING)   |
                          FIELD_SET(0, PANEL_FB_ADDRESS, EXT, LOCAL)        |
                          FIELD_VALUE(0, PANEL_FB_ADDRESS, ADDRESS, value)  );

        regWrite32(pSmi, PANEL_FB_WIDTH,
                          FIELD_VALUE(0, PANEL_FB_WIDTH, WIDTH,
                                      register_table->fb_width)             |
                          FIELD_VALUE(0, PANEL_FB_WIDTH, OFFSET,
                                      register_table->fb_width)             );

        regWrite32(pSmi, PANEL_WINDOW_WIDTH,
                          FIELD_VALUE(0, PANEL_WINDOW_WIDTH, WIDTH,
                                      register_table->width)                |
                          FIELD_VALUE(0, PANEL_WINDOW_WIDTH, X, 0)          );

        regWrite32(pSmi, PANEL_WINDOW_HEIGHT,
                          FIELD_VALUE(0, PANEL_WINDOW_HEIGHT, HEIGHT,
                                      register_table->height)               |
                          FIELD_VALUE(0, PANEL_WINDOW_HEIGHT, Y, 0)         );

        regWrite32(pSmi, PANEL_PLANE_TL,
                          FIELD_VALUE(0, PANEL_PLANE_TL, TOP, 0)            |
                          FIELD_VALUE(0, PANEL_PLANE_TL, LEFT, 0)           );

        regWrite32(pSmi, PANEL_PLANE_BR,
                          FIELD_VALUE(0, PANEL_PLANE_BR, BOTTOM,
                                      register_table->height - 1)           |
                          FIELD_VALUE(0, PANEL_PLANE_BR, RIGHT,
                                      register_table->width - 1)            );

        regWrite32(pSmi, PANEL_HORIZONTAL_TOTAL,
                          register_table->horizontal_total);
        regWrite32(pSmi, PANEL_HORIZONTAL_SYNC,
                          register_table->horizontal_sync);
        regWrite32(pSmi, PANEL_VERTICAL_TOTAL,
                          register_table->vertical_total);
        regWrite32(pSmi, PANEL_VERTICAL_SYNC,
                          register_table->vertical_sync);

        /* Program panel display control register. */
        value = regRead32(pSmi, PANEL_DISPLAY_CTRL)
              & FIELD_CLEAR(PANEL_DISPLAY_CTRL, VSYNC_PHASE)
              & FIELD_CLEAR(PANEL_DISPLAY_CTRL, HSYNC_PHASE)
              & FIELD_CLEAR(PANEL_DISPLAY_CTRL, TIMING)
              & FIELD_CLEAR(PANEL_DISPLAY_CTRL, PLANE)
              & FIELD_CLEAR(PANEL_DISPLAY_CTRL, FORMAT);

        regWrite32(pSmi, PANEL_DISPLAY_CTRL,
                          value | register_table->control);

        /* Palette RAM. */
        palette_ram = PANEL_PALETTE_RAM;

        /* Turn on panel. */
        panelPowerSequence(pSmi, PANEL_ON, 4);
    }

    /* Program CRT. */
    else
    {
        /* Program clock, enable display controller. */
        gate = FIELD_SET(gate, CURRENT_POWER_GATE, DISPLAY, ENABLE);
        clock &= FIELD_CLEAR(CURRENT_POWER_CLOCK, V2XCLK_SELECT)
              &  FIELD_CLEAR(CURRENT_POWER_CLOCK, V2XCLK_DIVIDER)
              &  FIELD_CLEAR(CURRENT_POWER_CLOCK, V2XCLK_SHIFT);

        setPower(pSmi, gate, clock | register_table->clock);

        /* Turn on DAC. */
        regWrite32(pSmi, MISC_CTRL, FIELD_SET(regRead32(pSmi, MISC_CTRL),
                                               MISC_CTRL,
                                               DAC_POWER,
                                               ENABLE));

        /* Calculate frame buffer address. */
        value = 0;
        fb_size = register_table->fb_width * register_table->height;
        if (FIELD_GET(regRead32(pSmi, PANEL_DISPLAY_CTRL),
                      PANEL_DISPLAY_CTRL,
                      PLANE) == PANEL_DISPLAY_CTRL_PLANE_ENABLE)
        {
            value = FIELD_GET(regRead32(pSmi, PANEL_FB_ADDRESS),
                              PANEL_FB_ADDRESS,
                              ADDRESS);
            if (fb_size < value)
            {
                value = 0;
            }
            else
            {
                value += FIELD_GET(regRead32(pSmi, PANEL_FB_WIDTH),
                                   PANEL_FB_WIDTH,
                                   OFFSET)
                      *  FIELD_GET(regRead32(pSmi, PANEL_WINDOW_HEIGHT),
                                   PANEL_WINDOW_HEIGHT,
                                   HEIGHT);
            }
        }

        /* Program CRT registers. */
        regWrite32(pSmi, CRT_FB_ADDRESS,
                          FIELD_SET(0, CRT_FB_ADDRESS, STATUS, PENDING)     |
                          FIELD_SET(0, CRT_FB_ADDRESS, EXT, LOCAL)          |
                          FIELD_VALUE(0, CRT_FB_ADDRESS, ADDRESS, value)    );

        regWrite32(pSmi, CRT_FB_WIDTH,
                          FIELD_VALUE(0, CRT_FB_WIDTH, WIDTH,
                                      register_table->fb_width)             |
                          FIELD_VALUE(0, CRT_FB_WIDTH, OFFSET,
                                      register_table->fb_width)             );

        regWrite32(pSmi, CRT_HORIZONTAL_TOTAL,
                          register_table->horizontal_total);
        regWrite32(pSmi, CRT_HORIZONTAL_SYNC,
                          register_table->horizontal_sync);
        regWrite32(pSmi, CRT_VERTICAL_TOTAL,
                          register_table->vertical_total);
        regWrite32(pSmi, CRT_VERTICAL_SYNC,
                          register_table->vertical_sync);

        /* Program CRT display control register. */
        value = regRead32(pSmi, CRT_DISPLAY_CTRL)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, VSYNC_PHASE)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, HSYNC_PHASE)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, SELECT)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, TIMING)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, PLANE)
              & FIELD_CLEAR(CRT_DISPLAY_CTRL, FORMAT);

        regWrite32(pSmi, CRT_DISPLAY_CTRL,
                          value | register_table->control);

        /* Palette RAM. */
        palette_ram = CRT_PALETTE_RAM;

        /* Turn on CRT. */
        setDPMS(pSmi, DPMS_ON);
    }

    /* In case of 8-bpp, fill palette. */
    if (FIELD_GET(register_table->control,
                  PANEL_DISPLAY_CTRL,
                  FORMAT) == PANEL_DISPLAY_CTRL_FORMAT_8)
    {
        /* Start with RGB = 0,0,0. */
        BYTE red = 0, green = 0, blue = 0;
        DWORD gray = 0;
        for (offset = 0; offset < 256 * 4; offset += 4)
        {
            /* Store current RGB value. */
            /* ERROR!!!!! IGX RGB should be a function, maybe RGB16?
            regWrite32(pSmi, (palette_ram + offset),
                    (gray ? (RGB((gray + 50) / 100,
                             (gray + 50) / 100,
                             (gray + 50) / 100))
                          : (RGB(red, green, blue))));
                 */

            if (gray)
            {
                /* Walk through grays (40 in total). */
                gray += 654;
            }

            else
            {
                /* Walk through colors (6 per base color). */
                if (blue != 255)
                {
                    blue += 51;
                }
                else if (green != 255)
                {
                    blue = 0;
                    green += 51;
                }
                else if (red != 255)
                {
                    green = blue = 0;
                    red += 51;
                }
                else
                {
                    gray = 1;
                }
            }
        }
    }

    /* For 16- and 32-bpp,  fill palette with gamma values. */
    else
    {
        /* Start with RGB = 0,0,0. */
        value = 0x000000;
        for (offset = 0; offset < 256 * 4; offset += 4)
        {
            regWrite32(pSmi, palette_ram + offset, value);

            /* Advance RGB by 1,1,1. */
            value += 0x010101;
        }
    }
}

static VOID SetMode(SMIPtr pSmi, ULONG nWidth, ULONG nHeight, FLONG fMode, ULONG nHertz, display_t display, INT fbPitch, INT bpp)
{
    mode_table_t mode;
    pmode_table_t vesaMode;
    reg_table_t register_table;

    /* Locate the mode */
    vesaMode = findMode(mode_table, nWidth, nHeight, nHertz);

    if (vesaMode != NULL)
    {
        /* Convert VESA timing into Voyager timing */
        adjustMode(vesaMode, &mode, display);

        /* Fill the register structure */
        setModeRegisters(&register_table, &mode, display, bpp, fbPitch);

        /* Program the registers */
        programMode(pSmi, &register_table);
    }
}

VOID panelSetMode(SMIPtr pSmi, ULONG nWidth, ULONG nHeight, FLONG fMode, ULONG nHertz, INT fbPitch, INT bpp)
{
    SetMode(pSmi, nWidth, nHeight, fMode, 60 /* was nHertz */, PANEL, fbPitch, bpp);
}

VOID crtSetMode(SMIPtr pSmi, ULONG nWidth, ULONG nHeight, FLONG fMode, ULONG nHertz, INT fbPitch, INT bpp)
{
    SetMode(pSmi, nWidth, nHeight, fMode, nHertz, CRT, fbPitch, bpp);
}

/*
 *
 *
 *  From POWER.C
 *
 *
 */

/* Program new power mode. */
static VOID setPower(SMIPtr pSmi, ULONG nGates, ULONG Clock)
{
    ULONG gate_reg, clock_reg;
    ULONG control_value;

    /* Get current power mode. */
    control_value = FIELD_GET(regRead32(pSmi, POWER_MODE_CTRL),
                              POWER_MODE_CTRL,
                              MODE);

    switch (control_value)
    {
    case POWER_MODE_CTRL_MODE_MODE0:

        /* Switch from mode 0 to mode 1. */
        gate_reg = POWER_MODE1_GATE;
        clock_reg = POWER_MODE1_CLOCK;
        control_value = FIELD_SET(control_value,
                                  POWER_MODE_CTRL, MODE, MODE1);
        break;

    case POWER_MODE_CTRL_MODE_MODE1:
    case POWER_MODE_CTRL_MODE_SLEEP:

        /* Switch from mode 1 or sleep to mode 0. */
        gate_reg = POWER_MODE0_GATE;
        clock_reg = POWER_MODE0_CLOCK;
        control_value = FIELD_SET(control_value, POWER_MODE_CTRL, MODE, MODE0);
        break;

    default:
        /* Invalid mode */
        return;
    }

    /* Program new power mode. */
    regWrite32(pSmi, gate_reg, nGates);
    regWrite32(pSmi, clock_reg, Clock);
    regWrite32(pSmi, POWER_MODE_CTRL, control_value);

    /* When returning from sleep, wait until finished. */
    /* IGX -- comment out for now, gets us in an infinite loop!
    while (FIELD_GET(regRead32(pSmi, POWER_MODE_CTRL),
                     POWER_MODE_CTRL,
                     SLEEP_STATUS) == POWER_MODE_CTRL_SLEEP_STATUS_ACTIVE) ;
     */
}

/* Set DPMS state. */
static VOID setDPMS(SMIPtr pSmi, DPMS_t state)
{
    ULONG value;

    value = regRead32(pSmi, SYSTEM_CTRL);
    switch (state)
    {
    case DPMS_ON:
        value = FIELD_SET(value, SYSTEM_CTRL, DPMS, VPHP);
        break;

    case DPMS_STANDBY:
        value = FIELD_SET(value, SYSTEM_CTRL, DPMS, VPHN);
        break;

    case DPMS_SUSPEND:
        value = FIELD_SET(value, SYSTEM_CTRL, DPMS, VNHP);
        break;

    case DPMS_OFF:
        value = FIELD_SET(value, SYSTEM_CTRL, DPMS, VNHN);
        break;
    }

    regWrite32(pSmi, SYSTEM_CTRL, value);
}

/* Panel Code */

/**********************************************************************
 *
 * panelWaitVSync
 *
 * Purpose
 *    Wait for the specified number of panel Vsyncs
 *
 * Parameters
 *    [in]
 *        vsync_count - Number of Vsyncs to wait
 *
 *    [out]
 *        None
 *
 * Returns
 *    Nothing
 *
 **********************************************************************/
static VOID panelWaitVSync(SMIPtr pSmi, INT vsync_count)
{
    ULONG status;
    ULONG timeout;

    while (vsync_count-- > 0)
    {
        /* Wait for end of vsync */
        timeout = 0;
        do
        {
            status = FIELD_GET(regRead32(pSmi, CMD_INTPR_STATUS),
                               CMD_INTPR_STATUS,
                               PANEL_SYNC);
            if (++timeout == VSYNCTIMEOUT)
                break;
        }
        while (status == CMD_INTPR_STATUS_PANEL_SYNC_ACTIVE);

        /* Wait for start of vsync */
        timeout = 0;
        do
        {
            status = FIELD_GET(regRead32(pSmi, CMD_INTPR_STATUS),
                               CMD_INTPR_STATUS,
                               PANEL_SYNC);
            if (++timeout == VSYNCTIMEOUT)
                break;
        }
        while (status == CMD_INTPR_STATUS_PANEL_SYNC_INACTIVE);
    }
}

/**********************************************************************
 *
 * panelPowerSequence
 *
 * Purpose
 *    Turn the panel On/Off
 *
 * Parameters
 *    [in]
 *        on_off      - Turn panel On/Off. Can be:
 *                      PANEL_ON
 *                      PANEL_OFF
 *        vsync_delay - Number of Vsyncs to wait after each signal is
 *                      turned on/off
 *
 *    [out]
 *        None
 *
 * Returns
 *    Nothing
 *
 **********************************************************************/
static VOID panelPowerSequence(SMIPtr pSmi, panel_state_t on_off, INT vsync_delay)
{
    ULONG panelControl = regRead32(pSmi, PANEL_DISPLAY_CTRL);

    if (on_off == PANEL_ON)
    {
        /* Turn on FPVDDEN. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, FPVDDEN, HIGH);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /* Turn on FPDATA. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, DATA, ENABLE);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /*  Turn on FPVBIAS. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, VBIASEN, HIGH);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /* Turn on FPEN. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, FPEN, HIGH);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
    }

    else
    {
        /* Turn off FPEN. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, FPEN, LOW);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /*  Turn off FPVBIASEN. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, VBIASEN, LOW);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /* Turn off FPDATA. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, DATA, DISABLE);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
        panelWaitVSync(pSmi, vsync_delay);

        /* Turn off FPVDDEN. */
        panelControl = FIELD_SET(panelControl,
                                 PANEL_DISPLAY_CTRL, FPVDDEN, LOW);
        regWrite32(pSmi, PANEL_DISPLAY_CTRL, panelControl);
    }
}

/**********************************************************************
 *
 * panelUseCRT
 *
 * Purpose
 *    Enable/disable routing of panel output to CRT monitor
 *
 * Parameters
 *    [in]
 *        bEnable - TRUE enables routing of panel output to CRT monitor
 *                  FALSE disables routing of panel output to CRT monitor
 *
 *    [out]
 *        None
 *
 * Returns
 *    Nothing
 *
 **********************************************************************/
VOID panelUseCRT(SMIPtr pSmi, BOOL bEnable)
{
    ULONG panel_ctrl = 0;
    ULONG crt_ctrl   = 0;

    panel_ctrl = regRead32(pSmi, PANEL_DISPLAY_CTRL);
    crt_ctrl   = regRead32(pSmi, CRT_DISPLAY_CTRL);

    if (bEnable)
    {
        /* Enable panel graphics plane */
        panel_ctrl = FIELD_SET(panel_ctrl, PANEL_DISPLAY_CTRL, PLANE, ENABLE);

        /* Disable CRT graphics plane */
        crt_ctrl = FIELD_SET(crt_ctrl, CRT_DISPLAY_CTRL, PLANE, DISABLE);

        /* Route panel data to CRT monitor */
        crt_ctrl = FIELD_SET(crt_ctrl, CRT_DISPLAY_CTRL, SELECT, PANEL);
    }
    else
    {
        /* Disable panel graphics plane */
        panel_ctrl = FIELD_SET(panel_ctrl, PANEL_DISPLAY_CTRL, PLANE, DISABLE);

        /* Enable CRT graphics plane */
        crt_ctrl = FIELD_SET(crt_ctrl, CRT_DISPLAY_CTRL, PLANE, ENABLE);

        /* Do not route panel data to CRT monitor */
        crt_ctrl = FIELD_SET(crt_ctrl, CRT_DISPLAY_CTRL, SELECT, CRT);
    }

    regWrite32(pSmi, PANEL_DISPLAY_CTRL, panel_ctrl);
    regWrite32(pSmi, CRT_DISPLAY_CTRL,   crt_ctrl);
}
