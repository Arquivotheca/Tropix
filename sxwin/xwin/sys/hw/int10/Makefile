#*
#****************************************************************
#	Makefile para o diretório "hw/int10"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		$STDDEF

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/int10

$LIB/.LIBSYM:		$LIB					\
			$LIB/stub.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/stub.o:		stub.c
			$CC $COPTS $INC $DEF stub.c

