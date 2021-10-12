#*
#****************************************************************
#	Makefile para o diretório "hw/xf4bpp"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/xf1bpp 			\
			-I $xc/sys/hw/os-support 		\
			-I $xc/sys/hw/os-support/bus 		\
			-I $xc/sys/hw/common			\
			-I $xc/sys/mfb 				\
			-I $xc/sys/cfb 				\
			-I $xc/sys/mi 				\
			-I $xc/sys/include 			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF					\
#			-D IN_MODULE				\
#			-D XFree86Module			\
			-D XF86VGA16

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xf4bpp

$LIB/.LIBSYM:		$LIB					\
			$LIB/ppcArea.o				\
			$LIB/ppcBStore.o			\
			$LIB/ppcClip.o				\
			$LIB/ppcCpArea.o			\
			$LIB/ppcCReduce.o			\
			$LIB/ppcDepth.o				\
			$LIB/ppcFillRct.o			\
			$LIB/ppcWinFS.o				\
			$LIB/ppcPixFS.o				\
			$LIB/ppcGC.o				\
			$LIB/ppcGetSp.o				\
			$LIB/ppcImg.o				\
			$LIB/ppcPixmap.o			\
			$LIB/ppcPntWin.o			\
			$LIB/ppcPolyPnt.o			\
			$LIB/ppcPolyRec.o			\
			$LIB/ppcQuery.o				\
			$LIB/ppcRslvC.o				\
			$LIB/ppcSetSp.o				\
			$LIB/ppcWindow.o			\
			$LIB/ppcIO.o				\
			$LIB/emulOpStip.o			\
			$LIB/emulRepAre.o			\
			$LIB/emulTile.o				\
			$LIB/vgaGC.o				\
			$LIB/vgaBitBlt.o			\
			$LIB/vgaImages.o			\
			$LIB/vgaStipple.o			\
			$LIB/vgaSolid.o				\
			$LIB/offscreen.o			\
			$LIB/wm3.o				\
#			$LIB/vgamodule.o			\
			$LIB/mfbimggblt.o			\
			$LIB/mfbline.o				\
			$LIB/mfbseg.o				\
			$LIB/mfbhrzvert.o			\
			$LIB/mfbbres.o				\
			$LIB/mfbbresd.o				\
			$LIB/mfbfillarc.o			\
			$LIB/mfbzerarc.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/ppcArea.o:		ppcArea.c
			$CC $COPTS $DEF $INC ppcArea.c

$LIB/ppcBStore.o:	ppcBStore.c
			$CC $COPTS $DEF $INC ppcBStore.c

$LIB/ppcClip.o:		ppcClip.c
			$CC $COPTS $DEF $INC ppcClip.c

$LIB/ppcCpArea.o:	ppcCpArea.c
			$CC $COPTS $DEF $INC ppcCpArea.c

$LIB/ppcCReduce.o:	ppcCReduce.c
			$CC $COPTS $DEF $INC ppcCReduce.c

$LIB/ppcDepth.o:	ppcDepth.c
			$CC $COPTS $DEF $INC ppcDepth.c

$LIB/ppcFillRct.o:	ppcFillRct.c
			$CC $COPTS $DEF $INC ppcFillRct.c

$LIB/ppcWinFS.o:	ppcWinFS.c
			$CC $COPTS $DEF $INC ppcWinFS.c

$LIB/ppcPixFS.o:	ppcPixFS.c
			$CC $COPTS $DEF $INC ppcPixFS.c

$LIB/ppcGC.o:		ppcGC.c
			$CC $COPTS $DEF $INC ppcGC.c

$LIB/ppcGetSp.o:	ppcGetSp.c
			$CC $COPTS $DEF $INC ppcGetSp.c

$LIB/ppcImg.o:		ppcImg.c
			$CC $COPTS $DEF $INC ppcImg.c

$LIB/ppcPixmap.o:	ppcPixmap.c
			$CC $COPTS $DEF $INC ppcPixmap.c

$LIB/ppcPntWin.o:	ppcPntWin.c
			$CC $COPTS $DEF $INC ppcPntWin.c

$LIB/ppcPolyPnt.o:	ppcPolyPnt.c
			$CC $COPTS $DEF $INC ppcPolyPnt.c

$LIB/ppcPolyRec.o:	ppcPolyRec.c
			$CC $COPTS $DEF $INC ppcPolyRec.c

$LIB/ppcQuery.o:	ppcQuery.c
			$CC $COPTS $DEF $INC ppcQuery.c

$LIB/ppcRslvC.o:	ppcRslvC.c
			$CC $COPTS $DEF $INC ppcRslvC.c

$LIB/ppcSetSp.o:	ppcSetSp.c
			$CC $COPTS $DEF $INC ppcSetSp.c

$LIB/ppcWindow.o:	ppcWindow.c
			$CC $COPTS $DEF $INC ppcWindow.c

$LIB/ppcIO.o:		ppcIO.c
			$CC $COPTS $DEF $INC ppcIO.c

$LIB/emulOpStip.o:	emulOpStip.c
			$CC $COPTS $DEF $INC emulOpStip.c

$LIB/emulRepAre.o:	emulRepAre.c
			$CC $COPTS $DEF $INC emulRepAre.c

$LIB/emulTile.o:	emulTile.c
			$CC $COPTS $DEF $INC emulTile.c

$LIB/vgaGC.o:		vgaGC.c
			$CC $COPTS $DEF $INC vgaGC.c

$LIB/vgaBitBlt.o:	vgaBitBlt.c
			$CC $COPTS $DEF $INC vgaBitBlt.c

$LIB/vgaImages.o:	vgaImages.c
			$CC $COPTS $DEF $INC vgaImages.c

$LIB/vgaStipple.o:	vgaStipple.c
			$CC $COPTS $DEF $INC vgaStipple.c

$LIB/vgaSolid.o:	vgaSolid.c
			$CC $COPTS $DEF $INC vgaSolid.c

$LIB/offscreen.o:	offscreen.c
			$CC $COPTS $DEF $INC offscreen.c

$LIB/wm3.o:		wm3.c
			$CC $COPTS $DEF $INC wm3.c

$LIB/mfbimggblt.o:	mfbimggblt.c
			$CC $COPTS $DEF $INC mfbimggblt.c

$LIB/mfbline.o:		mfbline.c
			$CC $COPTS $DEF $INC mfbline.c

$LIB/mfbseg.o:		mfbline.c
			$CC $COPTS $DEF -D POLYSEGMENT $INC mfbline.c

$LIB/mfbhrzvert.o:	mfbhrzvert.c
			$CC $COPTS $DEF $INC mfbhrzvert.c

$LIB/mfbbres.o:		mfbbres.c
			$CC $COPTS $DEF $INC mfbbres.c

$LIB/mfbbresd.o:	mfbbresd.c
			$CC $COPTS $DEF $INC mfbbresd.c

$LIB/mfbfillarc.o:	mfbfillarc.c
			$CC $COPTS $DEF $INC mfbfillarc.c

$LIB/mfbzerarc.o:	mfbzerarc.c
			$CC $COPTS $DEF $INC mfbzerarc.c

$LIB/vgamodule.o:	vgamodule.c
			$CC $COPTS $DEF $INC vgamodule.c
