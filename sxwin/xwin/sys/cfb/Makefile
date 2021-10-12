#*
#****************************************************************
#	Makefile para o diretório "cfb"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF					\
			-D PSZ=8 -D XFREE86

LIB	=		$xc/sys/lib/cfb
CC	=		cc
COPTS	=		-c -O -o $@

$LIB/.LIBSYM:		$LIB					\
			$LIB/cfbgc.o				\
			$LIB/cfbrrop.o				\
			$LIB/cfbwindow.o			\
			$LIB/cfbgetsp.o				\
			$LIB/cfbfillrct.o			\
			$LIB/cfbsolidC.o			\
			$LIB/cfbsolidX.o			\
			$LIB/cfbsolidG.o			\
			$LIB/cfbtileoddC.o			\
			$LIB/cfbtileoddG.o			\
			$LIB/cfbtile32C.o			\
			$LIB/cfbtile32G.o			\
			$LIB/cfbfillsp.o			\
			$LIB/cfbsetsp.o				\
			$LIB/cfbscrinit.o			\
			$LIB/cfballpriv.o			\
			$LIB/cfbpntwin.o			\
			$LIB/cfbmskbits.o			\
			$LIB/cfbpixmap.o			\
			$LIB/cfbcmap.o				\
			$LIB/cfbzerarcC.o			\
			$LIB/cfbzerarcX.o			\
			$LIB/cfbzerarcG.o			\
			$LIB/cfbfillarcC.o			\
			$LIB/cfbfillarcG.o			\
			$LIB/cfbigblt8.o			\
			$LIB/cfbglblt8.o			\
			$LIB/cfbtegblt.o			\
			$LIB/cfbbstore.o			\
			$LIB/cfbpolypnt.o			\
			$LIB/cfbbres.o				\
			$LIB/cfbline.o				\
			$LIB/cfbhrzvert.o			\
			$LIB/cfbbresd.o				\
			$LIB/cfbimage.o				\
			$LIB/cfbseg.o				\
			$LIB/cfb8lineCO.o			\
			$LIB/cfb8lineX.o			\
			$LIB/cfb8lineG.o			\
			$LIB/cfb8lineCP.o			\
#			$LIB/cfb8segCS.o			\
			$LIB/cfb8segX.o				\
			$LIB/cfb8segG.o				\
			$LIB/cfb8segC.o				\
			$LIB/cfbbitblt.o			\
			$LIB/cfbbltC.o				\
			$LIB/cfbbltX.o				\
			$LIB/cfbbltO.o				\
			$LIB/cfbbltG.o				\
			$LIB/cfbply1rctC.o			\
			$LIB/cfbply1rctG.o			\
			$LIB/cfb8bit.o				\
			$LIB/cfbteblt8.o			\
			$LIB/cfbglrop8.o			\
			$LIB/cfbpush8.o				\
			$LIB/cfbrctstp8.o			\
			$LIB/cfbcppl.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/cfbgc.o:		cfbgc.c
			$CC $COPTS $INC $DEF cfbgc.c

$LIB/cfbrrop.o:		cfbrrop.c
			$CC $COPTS $INC $DEF cfbrrop.c

$LIB/cfbwindow.o:	cfbwindow.c	
			$CC $COPTS $INC $DEF cfbwindow.c

$LIB/cfbgetsp.o:	cfbgetsp.c
			$CC $COPTS $INC $DEF cfbgetsp.c

$LIB/cfbfillrct.o:	cfbfillrct.c
			$CC $COPTS $INC $DEF cfbfillrct.c

$LIB/cfbsolidC.o:	cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXcopy cfbsolid.c

$LIB/cfbsolidX.o:	cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXxor cfbsolid.c

$LIB/cfbsolidG.o:	cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXset cfbsolid.c

$LIB/cfbtileoddC.o:	cfbtileodd.c	
			$CC $COPTS $INC $DEF -D MROP=Mcopy cfbtileodd.c

$LIB/cfbtileoddG.o:	cfbtileodd.c	
			$CC $COPTS $INC $DEF -D MROP=0 cfbtileodd.c

$LIB/cfbtile32C.o:	cfbtile32.c	
			$CC $COPTS $INC $DEF -D MROP=Mcopy cfbtile32.c

$LIB/cfbtile32G.o:	cfbtile32.c	
			$CC $COPTS $INC $DEF -D MROP=0 cfbtile32.c

$LIB/cfbfillsp.o:	cfbfillsp.c
			$CC $COPTS $INC $DEF cfbfillsp.c

$LIB/cfbsetsp.o:	cfbsetsp.c
			$CC $COPTS $INC $DEF cfbsetsp.c

$LIB/cfbscrinit.o:	cfbscrinit.c
			$CC $COPTS $INC $DEF cfbscrinit.c

$LIB/cfballpriv.o:	cfballpriv.c
			$CC $COPTS $INC $DEF cfballpriv.c

$LIB/cfbpntwin.o:	cfbpntwin.c
			$CC $COPTS $INC $DEF cfbpntwin.c

$LIB/cfbmskbits.o:	cfbmskbits.c
			$CC $COPTS $INC $DEF cfbmskbits.c

$LIB/cfbpixmap.o:	cfbpixmap.c
			$CC $COPTS $INC $DEF cfbpixmap.c

$LIB/cfbcmap.o:		cfbcmap.c
			$CC $COPTS $INC $DEF $EXTDEF cfbcmap.c

$LIB/cfbzerarcC.o:	cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXcopy cfbzerarc.c

$LIB/cfbzerarcX.o:	cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXxor cfbzerarc.c

$LIB/cfbzerarcG.o:	cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXset cfbzerarc.c

$LIB/cfbfillarcC.o:	cfbfillarc.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy cfbfillarc.c

$LIB/cfbfillarcG.o:	cfbfillarc.c
			$CC $COPTS $INC $DEF -D RROP=GXset cfbfillarc.c

$LIB/cfbigblt8.o:	cfbigblt8.c
			$CC $COPTS $INC $DEF cfbigblt8.c

$LIB/cfbglblt8.o:	cfbglblt8.c
			$CC $COPTS $INC $DEF cfbglblt8.c

$LIB/cfbtegblt.o:	cfbtegblt.c	
			$CC $COPTS $INC $DEF cfbtegblt.c

$LIB/cfbbstore.o:	cfbbstore.c
			$CC $COPTS $INC $DEF cfbbstore.c

$LIB/cfbpolypnt.o:	cfbpolypnt.c
			$CC $COPTS $INC $DEF cfbpolypnt.c

$LIB/cfbbres.o:		cfbbres.c
			$CC $COPTS $INC $DEF cfbbres.c

$LIB/cfbline.o:		cfbline.c
			$CC $COPTS $INC $DEF cfbline.c

$LIB/cfbhrzvert.o:	cfbhrzvert.c
			$CC $COPTS $INC $DEF cfbhrzvert.c

$LIB/cfbbresd.o:	cfbbresd.c
			$CC $COPTS $INC $DEF cfbbresd.c

$LIB/cfbimage.o:	cfbimage.c
			$CC $COPTS $INC $DEF cfbimage.c

$LIB/cfbseg.o:		cfbline.c
			$CC $COPTS $INC $DEF -D POLYSEGMENT cfbline.c

$LIB/cfb8lineCO.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy cfb8line.c

$LIB/cfb8lineX.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXxor cfb8line.c

$LIB/cfb8lineG.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXset cfb8line.c

$LIB/cfb8lineCP.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D PREVIOUS cfb8line.c

$LIB/cfb8segCS.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D POLYSEGMENT	\
				-D WIDTH_SHIFT cfb8line.c

$LIB/cfb8segX.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXxor -D POLYSEGMENT cfb8line.c

$LIB/cfb8segG.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXset -D POLYSEGMENT cfb8line.c

$LIB/cfb8segC.o:	cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D POLYSEGMENT cfb8line.c

$LIB/cfbbitblt.o:	cfbbitblt.c
			$CC $COPTS $INC $DEF cfbbitblt.c

$LIB/cfbbltC.o:		cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy cfbblt.c

$LIB/cfbbltX.o:		cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mxor cfbblt.c

$LIB/cfbbltO.o:		cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mor cfbblt.c

$LIB/cfbbltG.o:		cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=0 cfbblt.c

$LIB/cfbply1rctC.o:	cfbply1rct.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy cfbply1rct.c

$LIB/cfbply1rctG.o:	cfbply1rct.c
			$CC $COPTS $INC $DEF -D RROP=GXset cfbply1rct.c

$LIB/cfb8bit.o:		cfb8bit.c	
			$CC $COPTS $INC $DEF cfb8bit.c

$LIB/cfbteblt8.o:	cfbteblt8.c	
			$CC $COPTS $INC $DEF cfbteblt8.c

$LIB/cfbglrop8.o:	cfbglblt8.c
			$CC $COPTS $INC $DEF -D GLYPHROP cfbglblt8.c

$LIB/cfbpush8.o:	cfbpush8.c
			$CC $COPTS $INC $DEF cfbpush8.c

$LIB/cfbrctstp8.o:	cfbrctstp8.c
			$CC $COPTS $INC $DEF cfbrctstp8.c

$LIB/cfbcppl.o:		cfbcppl.c
			$CC $COPTS $INC $DEF cfbcppl.c
