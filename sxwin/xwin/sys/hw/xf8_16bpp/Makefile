#*
#****************************************************************
#	Makefile para o diretório "hw/xf8_16bpp"		*
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
LIB	=		$xc/sys/lib/xf8_16bpp

$LIB/.LIBSYM:		$LIB					\
#			$LIB/cfb8_32module.o			\
			$LIB/cfbscrinit.o			\
			$LIB/cfbwindow.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/cfb8_16module.o:	cfb8_16module.c
			$CC $COPTS $DEF $INC cfb8_16module.c

$LIB/cfbscrinit.o:	cfbscrinit.c
			$CC $COPTS $DEF $INC cfbscrinit.c

$LIB/cfbwindow.o:	cfbwindow.c
			$CC $COPTS $DEF $INC cfbwindow.c
