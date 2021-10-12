#*
#****************************************************************
#	Makefile para o diretório "hw/rac"			*
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
			-I $xc/sys/render			\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/rac

$LIB/.LIBSYM:		$LIB					\
#			$LIB/xf86RACmodule.o			\
			$LIB/xf86RAC.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xf86RAC.o:		xf86RAC.c
			$CC $COPTS $INC $DEF xf86RAC.c

$LIB/xf86RACmodule.o:	xf86RACmodule.o
			$CC $COPTS $INC $DEF xf86RACmodule.c
