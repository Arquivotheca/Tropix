#*
#****************************************************************
#	Makefile para o diretório "hw/os-support/bus"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw				\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		$STDDEF					\
			-U min

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/bus

$LIB/.LIBSYM:		$LIB					\
			$LIB/Pci.o				\
			$LIB/ix86Pci.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/Pci.o:		Pci.c
			$CC $COPTS $INC $DEF Pci.c

$LIB/ix86Pci.o:		ix86Pci.c
			$CC $COPTS $INC $DEF ix86Pci.c
