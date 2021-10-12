#*
#****************************************************************
#	Makefile para o diretório "hw/xf8_32bpp"		*
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
			-I $xc/sys/Xext 			\
			-I $xc/sys/include 			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/extensions		\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xf8_32bpp

$LIB/.LIBSYM:		$LIB					\
			$LIB/cfbbstore.o			\
			$LIB/cfbcpyarea.o			\
			$LIB/cfbcpyplane.o			\
			$LIB/cfbgcmisc.o			\
			$LIB/cfbimage.o				\
			$LIB/cfbpntwin.o			\
			$LIB/cfbscrinit.o			\
			$LIB/cfbwindow.o			\
			$LIB/xf86overlay.o			\
#			$LIB/cfb8_32module.o			\
			$LIB/cfbgc8.o				\
			$LIB/cfbgc32.o				\
			$LIB/cfbgcunder.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB


$LIB/cfbbstore.o:	cfbbstore.c
			$CC $COPTS $DEF $INC cfbbstore.c

$LIB/cfbcpyarea.o:	cfbcpyarea.c
			$CC $COPTS $DEF $INC cfbcpyarea.c

$LIB/cfbcpyplane.o:	cfbcpyplane.c
			$CC $COPTS $DEF $INC cfbcpyplane.c

$LIB/cfbgcmisc.o:	cfbgcmisc.c
			$CC $COPTS $DEF $INC cfbgcmisc.c

$LIB/cfbimage.o:	cfbimage.c
			$CC $COPTS $DEF $INC cfbimage.c

$LIB/cfbpntwin.o:	cfbpntwin.c
			$CC $COPTS $DEF $INC cfbpntwin.c

$LIB/cfbscrinit.o:	cfbscrinit.c
			$CC $COPTS $DEF $INC cfbscrinit.c

$LIB/cfbwindow.o:	cfbwindow.c
			$CC $COPTS $DEF $INC cfbwindow.c

$LIB/xf86overlay.o:	xf86overlay.c
			$CC $COPTS $DEF $INC xf86overlay.c

$LIB/cfb8_32module.o:	cfb8_32module.c
			$CC $COPTS $DEF $INC cfb8_32module.c

$LIB/cfbgc8.o:		cfbgc.c
			$CC $COPTS $DEF -D PSZ=8 $INC cfbgc.c

$LIB/cfbgc32.o:		cfbgc.c
			$CC $COPTS $DEF -D PSZ=32 $INC cfbgc.c

$LIB/cfbgcunder.o:	cfbgcunder.c
			$CC $COPTS $DEF $INC cfbgcunder.c
