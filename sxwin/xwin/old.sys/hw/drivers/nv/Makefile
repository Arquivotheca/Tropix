#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/nv"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/cfb				\
			-I $xc/sys/hw/xaa			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/hw/fbdevhw			\
			-I $xc/sys/hw/ramdac			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/Xext				\
			-I $xc/sys/hw/int10			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions		\
			-I $xc/sys/include			\
			-I $xc/sys/hw/shadowfb			\
			-I $xc/sys/fb				\
			-I $xc/sys/render			\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/hw/os-support/bus

DEF	=		$STDDEF					\
			-D PSZ=8
#			-D IN_MODULE 				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/nv

$LIB/.LIBSYM:		$LIB					\
			$LIB/nv_driver.o			\
			$LIB/nv_dac.o				\
			$LIB/nv_setup.o				\
			$LIB/nv_cursor.o			\
			$LIB/nv_hw.o				\
			$LIB/nv_xaa.o				\
			$LIB/nv_dga.o				\
			$LIB/nv_shadow.o			\
			$LIB/nv_video.o				\
			$LIB/riva_dac.o				\
			$LIB/riva_cursor.o			\
			$LIB/riva_dga.o				\
			$LIB/riva_driver.o			\
			$LIB/riva_hw.o				\
			$LIB/riva_setup.o			\
			$LIB/riva_shadow.o			\
			$LIB/riva_xaa.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/nv_driver.o:	nv_driver.c
			$CC $COPTS $INC $DEF nv_driver.c

$LIB/nv_dac.o:		nv_dac.c
			$CC $COPTS $INC $DEF nv_dac.c

$LIB/nv_setup.o:	nv_setup.c
			$CC $COPTS $INC $DEF nv_setup.c

$LIB/nv_cursor.o:	nv_cursor.c
			$CC $COPTS $INC $DEF nv_cursor.c

$LIB/nv_hw.o:		nv_hw.c
			$CC $COPTS $INC $DEF nv_hw.c

$LIB/nv_xaa.o:		nv_xaa.c
			$CC $COPTS $INC $DEF nv_xaa.c

$LIB/nv_dga.o:		nv_dga.c
			$CC $COPTS $INC $DEF nv_dga.c

$LIB/nv_shadow.o:	nv_shadow.c
			$CC $COPTS $INC $DEF nv_shadow.c

$LIB/nv_video.o:	nv_video.c
			$CC $COPTS $INC $DEF nv_video.c

$LIB/riva_dac.o:	riva_dac.c
			$CC $COPTS $INC $DEF riva_dac.c

$LIB/riva_cursor.o:	riva_cursor.c
			$CC $COPTS $INC $DEF riva_cursor.c

$LIB/riva_dga.o:	riva_dga.c
			$CC $COPTS $INC $DEF riva_dga.c

$LIB/riva_driver.o:	riva_driver.c
			$CC $COPTS $INC $DEF riva_driver.c

$LIB/riva_hw.o:		riva_hw.c
			$CC $COPTS $INC $DEF riva_hw.c

$LIB/riva_setup.o:	riva_setup.c
			$CC $COPTS $INC $DEF riva_setup.c

$LIB/riva_shadow.o:	riva_shadow.c
			$CC $COPTS $INC $DEF riva_shadow.c

$LIB/riva_xaa.o:	riva_xaa.c
			$CC $COPTS $INC $DEF riva_xaa.c
