#*
#****************************************************************
#	Makefile para o diretório "hw/xf24_32bpp"		*
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

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xf24_32bpp

$LIB/.LIBSYM:		$LIB					\
			$LIB/cfbcpyarea.o			\
			$LIB/cfbgcmisc.o			\
			$LIB/cfbpixmap.o			\
			$LIB/cfbimage.o				\
			$LIB/cfbscrinit.o			\
			$LIB/cfbwindow.o			\
#			$LIB/cfb24_32module.o			\
			$LIB/cfbgc24.o				\
			$LIB/cfbgc32.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/cfbcpyarea.o:	cfbcpyarea.c
			$CC $COPTS $DEF $INC cfbcpyarea.c

$LIB/cfbgcmisc.o:	cfbgcmisc.c
			$CC $COPTS $DEF $INC cfbgcmisc.c

$LIB/cfbpixmap.o:	cfbpixmap.c
			$CC $COPTS $DEF $INC cfbpixmap.c

$LIB/cfbimage.o:	cfbimage.c
			$CC $COPTS $DEF $INC cfbimage.c

$LIB/cfbscrinit.o:	cfbscrinit.c
			$CC $COPTS $DEF $INC cfbscrinit.c

$LIB/cfbwindow.o:	cfbwindow.c
			$CC $COPTS $DEF $INC cfbwindow.c

$LIB/cfb24_32module.o:	cfb24_32module.c
			$CC $COPTS $DEF $INC cfb24_32module.c

$LIB/cfbgc24.o:		cfbgc.c
			$CC $COPTS $DEF -D PSZ=24 $INC cfbgc.c

$LIB/cfbgc32.o:		cfbgc.c
			$CC $COPTS $DEF -D PSZ=32 $INC cfbgc.c

