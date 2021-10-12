#*
#****************************************************************
#	Makefile para o diretório "hw/i2c"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/Xext				\
			-I $xc/sys/cfb				\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/i2c

$LIB/.LIBSYM:		$LIB					\
#			$LIB/xf86i2cmodule.o			\
			$LIB/xf86i2c.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xf86i2c.o:		xf86i2c.c
			$CC $COPTS $INC $DEF xf86i2c.c

$LIB/xf86i2cmodule.o:	xf86i2cmodule.c
			$CC $COPTS $INC $DEF xf86i2cmodule.c
