#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/s3virge"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/cfb				\
			-I $xc/sys/hw/xaa			\
			-I $xc/sys/fb				\
			-I $xc/sys/hw/xf1bpp			\
			-I $xc/sys/hw/xf4bpp			\
			-I $xc/sys/hw/xf24_32bpp		\
			-I $xc/sys/Xext				\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/hw/ramdac			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/hw/int10			\
			-I $xc/sys/hw/shadowfb 	   		\
			-I $xc/sys/include			\
			-I $xc/sys/render			\
			-I $xc					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF					\
			-D PSZ=8
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/s3virge

$LIB/.LIBSYM:		$LIB					\
			$LIB/s3v_driver.o			\
			$LIB/s3v_accel.o			\
			$LIB/s3v_dac.o				\
			$LIB/s3v_hwcurs.o			\
			$LIB/s3v_dga.o				\
			$LIB/s3v_i2c.o				\
			$LIB/s3v_shadow.o			\
			$LIB/s3v_xv.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/s3v_driver.o:	s3v_driver.c
			$CC $COPTS $INC $DEF s3v_driver.c

$LIB/s3v_accel.o:	s3v_accel.c
			$CC $COPTS $INC $DEF s3v_accel.c

$LIB/s3v_dac.o:		s3v_dac.c
			$CC $COPTS $INC $DEF s3v_dac.c

$LIB/s3v_hwcurs.o:	s3v_hwcurs.c
			$CC $COPTS $INC $DEF s3v_hwcurs.c

$LIB/s3v_dga.o:		s3v_dga.c
			$CC $COPTS $INC $DEF s3v_dga.c

$LIB/s3v_i2c.o:		s3v_i2c.c
			$CC $COPTS $INC $DEF s3v_i2c.c

$LIB/s3v_shadow.o:	s3v_shadow.c
			$CC $COPTS $INC $DEF s3v_shadow.c

$LIB/s3v_xv.o:		s3v_xv.c
			$CC $COPTS $INC $DEF s3v_xv.c
