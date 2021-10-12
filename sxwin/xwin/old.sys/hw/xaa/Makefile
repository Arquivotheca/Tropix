#*
#****************************************************************
#	Makefile para o diretório "hw/xaa"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/include 			\
			-I $xc/sys/cfb 				\
			-I $xc/sys/mfb 				\
			-I $xc/sys/mi 	   			\
			-I $xc/sys/Xext 			\
			-I $xc/sys/render			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts 	   	\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF $EXTDEF

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xaa

$LIB/.LIBSYM:		$LIB					\
			$LIB/xaaInit.o				\
			$LIB/xaaGC.o				\
			$LIB/xaaInitAccel.o			\
			$LIB/xaaFallback.o			\
			$LIB/xaaBitBlt.o			\
			$LIB/xaaCpyArea.o			\
			$LIB/xaaGCmisc.o			\
			$LIB/xaaCpyWin.o			\
			$LIB/xaaCpyPlane.o			\
			$LIB/xaaFillRect.o			\
			$LIB/xaaTEText.o			\
			$LIB/xaaNonTEText.o			\
			$LIB/xaaPCache.o			\
			$LIB/xaaSpans.o				\
			$LIB/xaaROP.o				\
			$LIB/xaaImage.o				\
			$LIB/xaaPaintWin.o			\
			$LIB/xaaRect.o				\
			$LIB/xaaLineMisc.o			\
			$LIB/xaaBitOrder.o			\
			$LIB/xaaFillPoly.o			\
			$LIB/xaaWideLine.o			\
			$LIB/xaaTables.o			\
			$LIB/xaaFillArc.o			\
			$LIB/xaaLine.o				\
			$LIB/xaaSeg.o				\
			$LIB/xaaDashLine.o			\
			$LIB/xaaDashSeg.o			\
			$LIB/xaaOverlay.o			\
			$LIB/xaaOffscreen.o			\
			$LIB/xaaOverlayDF.o			\
			$LIB/xaaStateChange.o			\
			$LIB/xaaPict.o				\
			$LIB/lfr_Bitmap.o			\
			$LIB/lfr_Bitmap3.o			\
			$LIB/lfr_Stipple.o			\
			$LIB/lfr_Stipple3.o			\
			$LIB/lfr_TEGlyph.o			\
			$LIB/mfr_Bitmap.o			\
			$LIB/mfr_Bitmap3.o			\
			$LIB/mfr_Stipple.o			\
			$LIB/mfr_Stipple3.o			\
			$LIB/mfr_TEGlyph.o			\
			$LIB/lfi_Bitmap.o			\
			$LIB/lfi_Bitmap3.o			\
			$LIB/lfi_Stipple.o			\
			$LIB/lfi_Stipple3.o			\
			$LIB/lfi_TEGlyph.o			\
			$LIB/mfi_Bitmap.o			\
			$LIB/mfi_Bitmap3.o			\
			$LIB/mfi_Stipple.o			\
			$LIB/mfi_Stipple3.o			\
			$LIB/mfi_TEGlyph.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xaaInit.o:		xaaInit.c
			$CC $COPTS $DEF $INC xaaInit.c

$LIB/xaaGC.o:		xaaGC.c
			$CC $COPTS $DEF $INC xaaGC.c

$LIB/xaaInitAccel.o:	xaaInitAccel.c
			$CC $COPTS $DEF $INC xaaInitAccel.c

$LIB/xaaFallback.o:	xaaFallback.c
			$CC $COPTS $DEF $INC xaaFallback.c

$LIB/xaaBitBlt.o:	xaaBitBlt.c
			$CC $COPTS $DEF $INC xaaBitBlt.c

$LIB/xaaCpyArea.o:	xaaCpyArea.c
			$CC $COPTS $DEF $INC xaaCpyArea.c

$LIB/xaaGCmisc.o:	xaaGCmisc.c
			$CC $COPTS $DEF $INC xaaGCmisc.c

$LIB/xaaCpyWin.o:	xaaCpyWin.c
			$CC $COPTS $DEF $INC xaaCpyWin.c

$LIB/xaaCpyPlane.o:	xaaCpyPlane.c
			$CC $COPTS $DEF $INC xaaCpyPlane.c

$LIB/xaaFillRect.o:	xaaFillRect.c
			$CC $COPTS $DEF $INC xaaFillRect.c

$LIB/xaaTEText.o:	xaaTEText.c
			$CC $COPTS $DEF $INC xaaTEText.c

$LIB/xaaNonTEText.o:	xaaNonTEText.c
			$CC $COPTS $DEF $INC xaaNonTEText.c

$LIB/xaaPCache.o:	xaaPCache.c
			$CC $COPTS $DEF $INC xaaPCache.c

$LIB/xaaSpans.o:	xaaSpans.c
			$CC $COPTS $DEF $INC xaaSpans.c

$LIB/xaaROP.o:		xaaROP.c
			$CC $COPTS $DEF $INC xaaROP.c

$LIB/xaaImage.o:	xaaImage.c
			$CC $COPTS $DEF $INC xaaImage.c

$LIB/xaaPaintWin.o:	xaaPaintWin.c
			$CC $COPTS $DEF $INC xaaPaintWin.c

$LIB/xaaRect.o:		xaaRect.c
			$CC $COPTS $DEF $INC xaaRect.c

$LIB/xaaLineMisc.o:	xaaLineMisc.c
			$CC $COPTS $DEF $INC xaaLineMisc.c

$LIB/xaaBitOrder.o:	xaaBitOrder.c
			$CC $COPTS $DEF $INC xaaBitOrder.c

$LIB/xaaFillPoly.o:	xaaFillPoly.c
			$CC $COPTS $DEF $INC xaaFillPoly.c

$LIB/xaaWideLine.o:	xaaWideLine.c
			$CC $COPTS $DEF $INC xaaWideLine.c

$LIB/xaaTables.o:	xaaTables.c
			$CC $COPTS $DEF $INC xaaTables.c

$LIB/xaaFillArc.o:	xaaFillArc.c
			$CC $COPTS $DEF $INC xaaFillArc.c

$LIB/xaaLine.o:		xaaLine.c
			$CC $COPTS $DEF $INC xaaLine.c

$LIB/xaaSeg.o:		xaaLine.c
			$CC $COPTS $DEF -D POLYSEGMENT $INC xaaLine.c

$LIB/xaaDashLine.o:	xaaDashLine.c
			$CC $COPTS $DEF $INC xaaDashLine.c

$LIB/xaaDashSeg.o:	xaaDashLine.c
			$CC $COPTS $DEF -D POLYSEGMENT $INC xaaDashLine.c

$LIB/xaaOverlay.o:	xaaOverlay.c
			$CC $COPTS $DEF $INC xaaOverlay.c

$LIB/xaaOffscreen.o:	xaaOffscreen.c
			$CC $COPTS $DEF $INC xaaOffscreen.c

$LIB/xaaOverlayDF.o:	xaaOverlayDF.c
			$CC $COPTS $DEF $INC xaaOverlayDF.c

$LIB/xaaStateChange.o:	xaaStateChange.c
			$CC $COPTS $DEF $INC xaaStateChange.c

$LIB/xaaPict.o:		xaaPict.c
			$CC $COPTS $DEF $INC xaaPict.c

$LIB/lfr_Bitmap.o:	xaaBitmap.c
			$CC $COPTS $DEF -D LSBFIRST $INC xaaBitmap.c

$LIB/lfr_Bitmap3.o:	xaaBitmap.c
			$CC $COPTS $DEF -D LSBFIRST -D TRIPLE_BITS $INC xaaBitmap.c

$LIB/lfr_Stipple.o:	xaaStipple.c
			$CC $COPTS $DEF -D LSBFIRST $INC xaaStipple.c

$LIB/lfr_Stipple3.o:	xaaStipple.c
			$CC $COPTS $DEF -D LSBFIRST -D TRIPLE_BITS $INC xaaStipple.c

$LIB/lfr_TEGlyph.o:	xaaTEGlyph.c
			$CC $COPTS $DEF -D LSBFIRST $INC xaaTEGlyph.c

$LIB/mfr_Bitmap.o:	xaaBitmap.c
			$CC $COPTS $DEF -D MSBFIRST $INC xaaBitmap.c

$LIB/mfr_Bitmap3.o:	xaaBitmap.c
			$CC $COPTS $DEF -D MSBFIRST -D TRIPLE_BITS $INC xaaBitmap.c

$LIB/mfr_Stipple.o:	xaaStipple.c
			$CC $COPTS $DEF -D MSBFIRST $INC xaaStipple.c

$LIB/mfr_Stipple3.o:	xaaStipple.c
			$CC $COPTS $DEF -D MSBFIRST -D TRIPLE_BITS $INC xaaStipple.c

$LIB/mfr_TEGlyph.o:	xaaTEGlyph.c
			$CC $COPTS $DEF -D MSBFIRST $INC xaaTEGlyph.c

$LIB/lfi_Bitmap.o:	xaaBitmap.c
			$CC $COPTS $DEF -D LSBFIRST -D FIXEDBASE $INC xaaBitmap.c

$LIB/lfi_Bitmap3.o:	xaaBitmap.c
			$CC $COPTS $DEF -D LSBFIRST -D FIXEDBASE -D TRIPLE_BITS $INC xaaBitmap.c

$LIB/lfi_Stipple.o:	xaaStipple.c
			$CC $COPTS $DEF -D LSBFIRST -D FIXEDBASE $INC xaaStipple.c

$LIB/lfi_Stipple3.o:	xaaStipple.c
			$CC $COPTS $DEF -D LSBFIRST -D FIXEDBASE -D TRIPLE_BITS $INC xaaStipple.c

$LIB/lfi_TEGlyph.o:	xaaTEGlyph.c
			$CC $COPTS $DEF -D LSBFIRST -D FIXEDBASE $INC xaaTEGlyph.c

$LIB/mfi_Bitmap.o:	xaaBitmap.c
			$CC $COPTS $DEF -D MSBFIRST -D FIXEDBASE $INC xaaBitmap.c

$LIB/mfi_Bitmap3.o:	xaaBitmap.c
			$CC $COPTS $DEF -D MSBFIRST -D FIXEDBASE -D TRIPLE_BITS $INC xaaBitmap.c

$LIB/mfi_Stipple.o:	xaaStipple.c
			$CC $COPTS $DEF -D MSBFIRST -D FIXEDBASE $INC xaaStipple.c

$LIB/mfi_Stipple3.o:	xaaStipple.c
			$CC $COPTS $DEF -D MSBFIRST -D FIXEDBASE -D TRIPLE_BITS $INC xaaStipple.c

$LIB/mfi_TEGlyph.o:	xaaTEGlyph.c
			$CC $COPTS $DEF -D MSBFIRST -D FIXEDBASE $INC xaaTEGlyph.c
