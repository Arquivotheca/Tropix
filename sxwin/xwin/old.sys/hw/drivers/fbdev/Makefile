#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/fbdev"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/miext/shadow			\
			-I $xc/sys/afb				\
			-I $xc/sys/fb				\
			-I $xc/sys/hw/xaa			\
			-I $xc/sys/hw/fbdevhw			\
			-I $xc/sys/hw/ramdac			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/Xext				\
			-I $xc/sys/hw/xf24_32bpp		\
			-I $xc/sys/include			\
			-I $xc/sys/hw/shadowfb			\
			-I $xc/sys/render			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE
#			-D XFree86Module
#			-D USE_AFB

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/fbdev

$LIB/.LIBSYM:		$LIB					\
			$LIB/fbdev.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/fbdev.o:		fbdev.c
			$CC $COPTS $INC $DEF fbdev.c
