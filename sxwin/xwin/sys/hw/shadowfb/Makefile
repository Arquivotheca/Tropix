#*
#****************************************************************
#	Makefile para o diretório "hw/shadowfb"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/mi				\
			-I $xc/sys/render			\
			-I $xc/sys/include			\
			-I $xc					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/shadowfb

$LIB/.LIBSYM:		$LIB					\
#			$LIB/sfbmodule.o
			$LIB/shadow.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/shadow.o:		shadow.c
			$CC $COPTS $DEF $INC shadow.c

$LIB/sfbmodule.o:	sfbmodule.c
			$CC $COPTS $DEF $INC sfbmodule.c
