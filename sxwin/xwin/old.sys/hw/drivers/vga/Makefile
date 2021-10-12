#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/vga"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/cfb 				\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/int10			\
			-I $xc/sys/hw/xf4bpp			\
			-I $xc/sys/hw/xf1bpp			\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/fb				\
			-I $xc/sys/render			\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/include			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/shadowfb			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/vga

$LIB/.LIBSYM:		$LIB					\
			$LIB/generic.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/generic.o:		generic.c
			$CC $COPTS $INC $DEF generic.c
