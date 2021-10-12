#*
#****************************************************************
#	Makefile para o diretório "hw/input/mouse"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw				\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/extensions
			

DEF	=		$STDDEF					\
			-D PNP_MOUSE

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/input

$LIB/.LIBSYM:		$LIB					\
			$LIB/mouse.o				\
			$LIB/pnp.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/mouse.o:		mouse.c
			$CC $COPTS $INC $DEF mouse.c

$LIB/pnp.o:		pnp.c
			$CC $COPTS $INC $DEF pnp.c
