#*
#****************************************************************
#	Makefile para o diretório "hw/vgahw"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/Xext				\
			-I $xc/sys/cfb				\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/vgahw

$LIB/.LIBSYM:		$LIB					\
#			$LIB/vgaHWmodule.o			\
			$LIB/vgaHW.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/vgaHW.o:		vgaHW.c
			$CC $COPTS $INC $DEF vgaHW.c

$LIB/vgaHWmodule.o:	vgaHWmodule.c
			$CC $COPTS $INC $DEF vgaHWmodule.c
