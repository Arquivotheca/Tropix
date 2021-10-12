#*
#****************************************************************
#	Makefile para o diretório "hw/ddc"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/i2c			\
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
LIB	=		$xc/sys/lib/ddc

$LIB/.LIBSYM:		$LIB					\
			$LIB/xf86DDC.o				\
			$LIB/edid.o				\
			$LIB/interpret_edid.o			\
			$LIB/print_edid.o			\
			$LIB/interpret_vdif.o			\
			$LIB/print_vdif.o			\
			$LIB/ddcProperty.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xf86DDC.o:		xf86DDC.c
			$CC $COPTS $INC $DEF xf86DDC.c

$LIB/edid.o:		edid.c
			$CC $COPTS $INC $DEF edid.c

$LIB/interpret_edid.o:	interpret_edid.c
			$CC $COPTS $INC $DEF interpret_edid.c

$LIB/print_edid.o:	print_edid.c
			$CC $COPTS $INC $DEF print_edid.c

$LIB/interpret_vdif.o:	interpret_vdif.c
			$CC $COPTS $INC $DEF interpret_vdif.c

$LIB/print_vdif.o:	print_vdif.c
			$CC $COPTS $INC $DEF print_vdif.c

$LIB/ddcProperty.o:	ddcProperty.c
			$CC $COPTS $INC $DEF ddcProperty.c
