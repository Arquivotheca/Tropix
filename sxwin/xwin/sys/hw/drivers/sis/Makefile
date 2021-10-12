#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/sis"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/int10			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/shadowfb			\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/Xext				\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/fb				\
			-I $xc/sys/cfb				\
			-I $xc/sys/render			\
			-I $xc/sys/hw/xaa			\
			-I $xc/sys/hw/xf1bpp			\
			-I $xc/sys/hw/xf4bpp			\
			-I $xc/sys/hw/xf24_32bpp		\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/hw/ramdac			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/GL/dri			\
			-I $xc/sys/include			\
#			-I ../../../../../../lib/GL/dri		\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D GLXEXT				\
#			-D XF86DRI 				\
#			-D GLX_DIRECT_RENDERING			\
#			-D GLX_USE_DLOPEN			\
#			-D GLX_USE_MESA

#			-D IN_MODULE				\
#			-D XFree86Module   

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/sis

$LIB/.LIBSYM:		$LIB					\
			$LIB/sis_driver.o			\
			$LIB/init.o				\
			$LIB/sis_dac.o				\
			$LIB/sis_cursor.o			\
			$LIB/sis_accel.o			\
			$LIB/sis_setup.o			\
			$LIB/sis6326_video.o			\
			$LIB/sis300_accel.o			\
			$LIB/sis310_accel.o			\
			$LIB/sis_vga.o				\
			$LIB/sis_vb.o				\
			$LIB/sis_video.o			\
			$LIB/sis_opt.o				\
			$LIB/init301.o				\
			$LIB/sis_dga.o				\
			$LIB/sis_shadow.o
#			$LIB/sis_dri.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/sis_driver.o:	sis_driver.c
			$CC $COPTS $INC $DEF sis_driver.c

$LIB/init.o:		init.c
			$CC $COPTS $INC $DEF init.c

$LIB/sis_dac.o:		sis_dac.c
			$CC $COPTS $INC $DEF sis_dac.c

$LIB/sis_cursor.o:	sis_cursor.c
			$CC $COPTS $INC $DEF sis_cursor.c

$LIB/sis_accel.o:	sis_accel.c
			$CC $COPTS $INC $DEF sis_accel.c

$LIB/sis_setup.o:	sis_setup.c
			$CC $COPTS $INC $DEF sis_setup.c

$LIB/sis6326_video.o:	sis6326_video.c
			$CC $COPTS $INC $DEF sis6326_video.c

$LIB/sis300_accel.o:	sis300_accel.c
			$CC $COPTS $INC $DEF sis300_accel.c

$LIB/sis310_accel.o:	sis310_accel.c
			$CC $COPTS $INC $DEF sis310_accel.c

$LIB/sis_vga.o:		sis_vga.c
			$CC $COPTS $INC $DEF sis_vga.c

$LIB/sis_vb.o:		sis_vb.c
			$CC $COPTS $INC $DEF sis_vb.c

$LIB/sis_video.o:	sis_video.c
			$CC $COPTS $INC $DEF sis_video.c

$LIB/sis_opt.o:		sis_opt.c
			$CC $COPTS $INC $DEF sis_opt.c

$LIB/init301.o:		init301.c
			$CC $COPTS $INC $DEF init301.c

$LIB/sis_dga.o:		sis_dga.c
			$CC $COPTS $INC $DEF sis_dga.c

$LIB/sis_shadow.o:	sis_shadow.c
			$CC $COPTS $INC $DEF sis_shadow.c

#$LIB/sis_dri.o:	sis_dri.c
#			$CC $COPTS $INC $DEF sis_dri.c
