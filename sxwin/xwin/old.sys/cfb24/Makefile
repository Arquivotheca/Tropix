#*
#****************************************************************
#	Makefile para o diretório "cfb24"			*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/cfb				\
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
			-D PSZ=24 -D XFREE86

LIB	=		$xc/sys/lib/cfb24
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
#			$LIB/cfbcmap.o				\
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
			$LIB/cfbcppl.o				\
			$LIB/cfbply1rctC.o			\
			$LIB/cfbply1rctG.o
			mklib $LIB

#			$LIB/cfb8bit.o				\
#			$LIB/cfbteblt8.o			\
#			$LIB/cfbglrop8.o			\
#			$LIB/cfbpush8.o				\
#			$LIB/cfbrctstp8.o			\
#			$LIB/cfb8cppl.o

$LIB:			.exist
			mkdir -f $LIB

$LIB/cfbgc.o:		../cfb/cfbgc.c	
			$CC $COPTS $INC $DEF ../cfb/cfbgc.c

$LIB/cfbrrop.o:		../cfb/cfbrrop.c
			$CC $COPTS $INC $DEF ../cfb/cfbrrop.c

$LIB/cfbwindow.o:	../cfb/cfbwindow.c	
			$CC $COPTS $INC $DEF ../cfb/cfbwindow.c

$LIB/cfbgetsp.o:	../cfb/cfbgetsp.c
			$CC $COPTS $INC $DEF ../cfb/cfbgetsp.c

$LIB/cfbfillrct.o:	../cfb/cfbfillrct.c
			$CC $COPTS $INC $DEF ../cfb/cfbfillrct.c

$LIB/cfbsolidC.o:	../cfb/cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXcopy ../cfb/cfbsolid.c

$LIB/cfbsolidX.o:	../cfb/cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXxor ../cfb/cfbsolid.c

$LIB/cfbsolidG.o:	../cfb/cfbsolid.c	
			$CC $COPTS $INC $DEF -D RROP=GXset ../cfb/cfbsolid.c

$LIB/cfbtileoddC.o:	../cfb/cfbtileodd.c	
			$CC $COPTS $INC $DEF -D MROP=Mcopy ../cfb/cfbtileodd.c

$LIB/cfbtileoddG.o:	../cfb/cfbtileodd.c	
			$CC $COPTS $INC $DEF -D MROP=0 ../cfb/cfbtileodd.c

$LIB/cfbtile32C.o:	../cfb/cfbtile32.c	
			$CC $COPTS $INC $DEF -D MROP=Mcopy ../cfb/cfbtile32.c

$LIB/cfbtile32G.o:	../cfb/cfbtile32.c	
			$CC $COPTS $INC $DEF -D MROP=0 ../cfb/cfbtile32.c

$LIB/cfbfillsp.o:	../cfb/cfbfillsp.c
			$CC $COPTS $INC $DEF ../cfb/cfbfillsp.c

$LIB/cfbsetsp.o:	../cfb/cfbsetsp.c
			$CC $COPTS $INC $DEF ../cfb/cfbsetsp.c

$LIB/cfbscrinit.o:	../cfb/cfbscrinit.c
			$CC $COPTS $INC $DEF ../cfb/cfbscrinit.c

$LIB/cfballpriv.o:	../cfb/cfballpriv.c
			$CC $COPTS $INC $DEF ../cfb/cfballpriv.c

$LIB/cfbpntwin.o:	../cfb/cfbpntwin.c
			$CC $COPTS $INC $DEF ../cfb/cfbpntwin.c

$LIB/cfbmskbits.o:	../cfb/cfbmskbits.c
			$CC $COPTS $INC $DEF ../cfb/cfbmskbits.c

$LIB/cfbpixmap.o:	../cfb/cfbpixmap.c
			$CC $COPTS $INC $DEF ../cfb/cfbpixmap.c

#$LIB/cfbcmap.o:	../cfb/cfbcmap.c
#			$CC $COPTS $INC $DEF $EXTDEF ../cfb/cfbcmap.c

$LIB/cfbzerarcC.o:	../cfb/cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXcopy ../cfb/cfbzerarc.c

$LIB/cfbzerarcX.o:	../cfb/cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXxor ../cfb/cfbzerarc.c

$LIB/cfbzerarcG.o:	../cfb/cfbzerarc.c	
			$CC $COPTS $INC $DEF -D RROP=GXset ../cfb/cfbzerarc.c

$LIB/cfbfillarcC.o:	../cfb/cfbfillarc.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy ../cfb/cfbfillarc.c

$LIB/cfbfillarcG.o:	../cfb/cfbfillarc.c
			$CC $COPTS $INC $DEF -D RROP=GXset ../cfb/cfbfillarc.c

$LIB/cfbigblt8.o:	../cfb/cfbigblt8.c
			$CC $COPTS $INC $DEF ../cfb/cfbigblt8.c

$LIB/cfbglblt8.o:	../cfb/cfbglblt8.c
			$CC $COPTS $INC $DEF ../cfb/cfbglblt8.c

$LIB/cfbtegblt.o:	../cfb/cfbtegblt.c	
			$CC $COPTS $INC $DEF ../cfb/cfbtegblt.c

$LIB/cfbbstore.o:	../cfb/cfbbstore.c
			$CC $COPTS $INC $DEF ../cfb/cfbbstore.c

$LIB/cfbpolypnt.o:	../cfb/cfbpolypnt.c
			$CC $COPTS $INC $DEF ../cfb/cfbpolypnt.c

$LIB/cfbbres.o:		../cfb/cfbbres.c
			$CC $COPTS $INC $DEF ../cfb/cfbbres.c

$LIB/cfbline.o:		../cfb/cfbline.c
			$CC $COPTS $INC $DEF ../cfb/cfbline.c

$LIB/cfbhrzvert.o:	../cfb/cfbhrzvert.c
			$CC $COPTS $INC $DEF ../cfb/cfbhrzvert.c

$LIB/cfbbresd.o:	../cfb/cfbbresd.c
			$CC $COPTS $INC $DEF ../cfb/cfbbresd.c

$LIB/cfbimage.o:	../cfb/cfbimage.c
			$CC $COPTS $INC $DEF ../cfb/cfbimage.c

$LIB/cfbseg.o:		../cfb/cfbline.c
			$CC $COPTS $INC $DEF -D POLYSEGMENT ../cfb/cfbline.c

$LIB/cfb8lineCO.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy ../cfb/cfb8line.c

$LIB/cfb8lineX.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXxor ../cfb/cfb8line.c

$LIB/cfb8lineG.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXset ../cfb/cfb8line.c

$LIB/cfb8lineCP.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D PREVIOUS ../cfb/cfb8line.c

$LIB/cfb8segCS.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D POLYSEGMENT	\
				-D WIDTH_SHIFT ../cfb/cfb8line.c

$LIB/cfb8segX.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXxor -D POLYSEGMENT ../cfb/cfb8line.c

$LIB/cfb8segG.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXset -D POLYSEGMENT ../cfb/cfb8line.c

$LIB/cfb8segC.o:	../cfb/cfb8line.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy -D POLYSEGMENT ../cfb/cfb8line.c

$LIB/cfbbitblt.o:	../cfb/cfbbitblt.c
			$CC $COPTS $INC $DEF ../cfb/cfbbitblt.c

$LIB/cfbbltC.o:		../cfb/cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mcopy ../cfb/cfbblt.c

$LIB/cfbbltX.o:		../cfb/cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mxor ../cfb/cfbblt.c

$LIB/cfbbltO.o:		../cfb/cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=Mor ../cfb/cfbblt.c

$LIB/cfbbltG.o:		../cfb/cfbblt.c
			$CC $COPTS $INC $DEF -D MROP=0 ../cfb/cfbblt.c

$LIB/cfbply1rctC.o:	../cfb/cfbply1rct.c
			$CC $COPTS $INC $DEF -D RROP=GXcopy ../cfb/cfbply1rct.c

$LIB/cfbply1rctG.o:	../cfb/cfbply1rct.c
			$CC $COPTS $INC $DEF -D RROP=GXset ../cfb/cfbply1rct.c

$LIB/cfb8bit.o:		../cfb/cfb8bit.c	
			$CC $COPTS $INC $DEF ../cfb/cfb8bit.c

$LIB/cfbteblt8.o:	../cfb/cfbteblt8.c	
			$CC $COPTS $INC $DEF ../cfb/cfbteblt8.c

$LIB/cfbglrop8.o:	../cfb/cfbglblt8.c
			$CC $COPTS $INC $DEF -D GLYPHROP ../cfb/cfbglblt8.c

$LIB/cfbpush8.o:	../cfb/cfbpush8.c
			$CC $COPTS $INC $DEF ../cfb/cfbpush8.c

$LIB/cfbrctstp8.o:	../cfb/cfbrctstp8.c
			$CC $COPTS $INC $DEF ../cfb/cfbrctstp8.c

$LIB/cfb8cppl.o:	../cfb/cfb8cppl.c
			$CC $COPTS $INC $DEF ../cfb/cfb8cppl.c

$LIB/cfbcppl.o:		../cfb/cfbcppl.c
			$CC $COPTS $INC $DEF ../cfb/cfbcppl.c
