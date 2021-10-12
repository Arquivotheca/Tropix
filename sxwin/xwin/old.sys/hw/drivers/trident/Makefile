#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/trident"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/include			\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/Xext				\
			-I $xc/sys/mi				\
			-I $xc/sys/hw/xaa			\
			-I $xc/sys/fb				\
			-I $xc/sys/hw/xf1bpp			\
			-I $xc/sys/miext/shadow			\
			-I $xc/sys/render			\
			-I $xc/sys/hw/shadowfb			\
			-I $xc/sys/hw/xf4bpp			\
			-I $xc/sys/mfb				\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/hw/ramdac			\
			-I $xc/sys/hw/rac			\
			-I $xc/sys/hw/int10			\
			-I $xc/sys/hw/ddc			\
			-I $xc/sys/hw/i2c			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module   

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/trident

$LIB/.LIBSYM:		$LIB					\
			$LIB/trident_driver.o			\
			$LIB/trident_dac.o			\
			$LIB/tridenthelper.o			\
			$LIB/trident_accel.o			\
			$LIB/trident_i2c.o			\
			$LIB/trident_bank.o			\
			$LIB/image_accel.o			\
			$LIB/blade_accel.o			\
			$LIB/tvga_dac.o				\
			$LIB/trident_dga.o			\
			$LIB/trident_shadow.o			\
			$LIB/trident_video.o			\
			$LIB/trident_tv.o			\
			$LIB/xp_accel.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/trident_driver.o:	trident_driver.c
			$CC $COPTS $INC $DEF trident_driver.c

$LIB/trident_dac.o:	trident_dac.c
			$CC $COPTS $INC $DEF trident_dac.c

$LIB/tridenthelper.o:	tridenthelper.c
			$CC $COPTS $INC $DEF tridenthelper.c

$LIB/trident_accel.o:	trident_accel.c
			$CC $COPTS $INC $DEF trident_accel.c

$LIB/trident_i2c.o:	trident_i2c.c
			$CC $COPTS $INC $DEF trident_i2c.c

$LIB/trident_bank.o:	trident_bank.c
			$CC $COPTS $INC $DEF trident_bank.c

$LIB/image_accel.o:	image_accel.c
			$CC $COPTS $INC $DEF image_accel.c

$LIB/blade_accel.o:	blade_accel.c
			$CC $COPTS $INC $DEF blade_accel.c

$LIB/tvga_dac.o:	tvga_dac.c
			$CC $COPTS $INC $DEF tvga_dac.c

$LIB/trident_dga.o:	trident_dga.c
			$CC $COPTS $INC $DEF trident_dga.c

$LIB/trident_shadow.o:	trident_shadow.c
			$CC $COPTS $INC $DEF trident_shadow.c

$LIB/trident_tv.o:	trident_tv.c
			$CC $COPTS $INC $DEF trident_tv.c

$LIB/trident_video.o:	trident_video.c
			$CC $COPTS $INC $DEF trident_video.c

$LIB/xp_accel.o:	xp_accel.c
			$CC $COPTS $INC $DEF xp_accel.c
