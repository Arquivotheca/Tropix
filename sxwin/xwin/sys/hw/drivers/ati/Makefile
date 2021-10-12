#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/ati"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/hw 				\
			-I $xc/sys/hw/common 			\
			-I $xc/sys/hw/os-support 		\
			-I $xc/sys/hw/vbe	 		\
#			-I $xc/sys/hw/os-support/vbe 		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/int10            		\
			-I $xc/sys/hw/ddc 			\
			-I $xc/sys/hw/i2c       		\
			-I $xc/sys/hw/rac 			\
			-I $xc/sys/hw/ramdac    		\
			-I $xc/sys/hw/shadowfb 			\
			-I $xc/sys/hw/xaa 			\
			-I $xc/sys/hw/xf24_32bpp		\
			-I $xc/sys/hw/xf4bpp 			\
			-I $xc/sys/hw/xf1bpp    		\
			-I $xc/sys/hw/vgahw 			\
			-I $xc/sys/hw/fbdevhw   		\
			-I $xc/sys/cfb 				\
			-I $xc/sys/mfb  			\
			-I $xc/sys/fb 				\
			-I $xc/sys/mi   			\
			-I $xc/sys/render 			\
			-I $xc/sys/Xext 			\
			-I $xc/sys/include      		\
#			-I $xc/sys/GL/dri 			\
#			-I $xc/lib/GL/dri			\
			-I $xc/include 				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions		\
			-I $xc

DEF	=		$STDDEF					\
#			-D IN_MODULE				\
#			-D XFree86Module			\
#			-D GLXEXT				\
#			-D XF86DRI 				\
#			-D GLX_DIRECT_RENDERING			\
#			-D GLX_USE_DLOPEN			\
#			-D GLX_USE_MESA				\
			-D __FUNCTION__='"função corrente"'

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/ati

$LIB/.LIBSYM:		$LIB					\
			$LIB/ati.o				\
			$LIB/atiendian.o			\
			$LIB/atiaccel.o				\
			$LIB/atiadapter.o			\
			$LIB/atibus.o				\
			$LIB/atichip.o				\
			$LIB/aticursor.o			\
			$LIB/atiident.o				\
			$LIB/atioption.o			\
			$LIB/atiprobe.o				\
			$LIB/atividmem.o			\
#			$LIB/atimodule.o			\
#			$LIB/atibios.o				\
			$LIB/ativgaio.o				\
			$LIB/atii2c.o				\
			$LIB/radeon_probe.o			\
			$LIB/r128_probe.o			\
			$LIB/atiadjust.o			\
			$LIB/aticlock.o				\
			$LIB/aticonfig.o			\
			$LIB/aticonsole.o			\
			$LIB/atidac.o				\
			$LIB/atidga.o				\
			$LIB/atidsp.o				\
			$LIB/atilock.o				\
			$LIB/atimach64.o			\
			$LIB/atimach64io.o			\
			$LIB/atimach64xv.o			\
			$LIB/atimach64accel.o			\
			$LIB/atimach64cursor.o			\
			$LIB/atimach64i2c.o			\
			$LIB/atimode.o				\
			$LIB/atipreinit.o			\
			$LIB/atiprint.o				\
			$LIB/atirgb514.o			\
			$LIB/atiscreen.o			\
			$LIB/atiutil.o				\
			$LIB/ativalid.o				\
#			$LIB/atiload.o				\
#			$LIB/atimisc.o				\
			$LIB/atibank.o				\
			$LIB/atituner.o				\
			$LIB/ativga.o				\
			$LIB/atiwonder.o			\
			$LIB/atiwonderio.o			\
			$LIB/atixv.o				\
			$LIB/r128_accel.o			\
			$LIB/r128_cursor.o			\
			$LIB/r128_dga.o				\
			$LIB/r128_driver.o			\
			$LIB/r128_video.o			\
#			$LIB/r128_misc.o			\
#			$LIB/r128_dri.o				\
			$LIB/radeon_accel.o			\
			$LIB/radeon_cursor.o			\
			$LIB/radeon_dga.o			\
			$LIB/radeon_driver.o			\
#			$LIB/radeon_misc.o			\
			$LIB/radeon_video.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/ati.o:		ati.c
			$CC $COPTS $INC $DEF ati.c

$LIB/atiendian.o:	atiendian.c
			$CC $COPTS $INC $DEF atiendian.c

$LIB/atiaccel.o:	atiaccel.c
			$CC $COPTS $INC $DEF atiaccel.c

$LIB/atiadapter.o:	atiadapter.c
			$CC $COPTS $INC $DEF atiadapter.c

$LIB/atibus.o:		atibus.c
			$CC $COPTS $INC $DEF atibus.c

$LIB/aticursor.o:	aticursor.c
			$CC $COPTS $INC $DEF aticursor.c

$LIB/atichip.o:		atichip.c
			$CC $COPTS $INC $DEF atichip.c

$LIB/atiident.o:	atiident.c
			$CC $COPTS $INC $DEF atiident.c

$LIB/atioption.o:	atioption.c
			$CC $COPTS $INC $DEF atioption.c

$LIB/atiprobe.o:	atiprobe.c
			$CC $COPTS $INC $DEF atiprobe.c

$LIB/atividmem.o:	atividmem.c
			$CC $COPTS $INC $DEF atividmem.c

$LIB/atimodule.o:	atimodule.c
			$CC $COPTS $INC $DEF atimodule.c

$LIB/atibios.o:		atibios.c
			$CC $COPTS $INC $DEF atibios.c

$LIB/ativgaio.o:	ativgaio.c
			$CC $COPTS $INC $DEF ativgaio.c

$LIB/atii2c.o:		atii2c.c
			$CC $COPTS $INC $DEF atii2c.c

$LIB/radeon_probe.o:	radeon_probe.c
			$CC $COPTS $INC $DEF radeon_probe.c

$LIB/r128_probe.o:	r128_probe.c
			$CC $COPTS $INC $DEF r128_probe.c

$LIB/atiadjust.o:	atiadjust.c
			$CC $COPTS $INC $DEF atiadjust.c

$LIB/aticlock.o:	aticlock.c
			$CC $COPTS $INC $DEF aticlock.c

$LIB/aticonfig.o:	aticonfig.c
			$CC $COPTS $INC $DEF aticonfig.c

$LIB/aticonsole.o:	aticonsole.c
			$CC $COPTS $INC $DEF aticonsole.c

$LIB/atidac.o:		atidac.c
			$CC $COPTS $INC $DEF atidac.c

$LIB/atidga.o:		atidga.c
			$CC $COPTS $INC $DEF atidga.c

$LIB/atidsp.o:		atidsp.c
			$CC $COPTS $INC $DEF atidsp.c

$LIB/atilock.o:		atilock.c
			$CC $COPTS $INC $DEF atilock.c

$LIB/atimach64.o:	atimach64.c
			$CC $COPTS $INC $DEF atimach64.c

$LIB/atimach64io.o:	atimach64io.c
			$CC $COPTS $INC $DEF atimach64io.c

$LIB/atimach64xv.o:	atimach64xv.c
			$CC $COPTS $INC $DEF atimach64xv.c

$LIB/atimach64accel.o:	atimach64accel.c
			$CC $COPTS $INC $DEF atimach64accel.c

$LIB/atimach64cursor.o:	atimach64cursor.c
			$CC $COPTS $INC $DEF atimach64cursor.c

$LIB/atimach64i2c.o:	atimach64i2c.c
			$CC $COPTS $INC $DEF atimach64i2c.c

$LIB/atimode.o:		atimode.c
			$CC $COPTS $INC $DEF atimode.c

$LIB/atipreinit.o:	atipreinit.c
			$CC $COPTS $INC $DEF atipreinit.c

$LIB/atiprint.o:	atiprint.c
			$CC $COPTS $INC $DEF atiprint.c

$LIB/atirgb514.o:	atirgb514.c
			$CC $COPTS $INC $DEF atirgb514.c

$LIB/atiscreen.o:	atiscreen.c
			$CC $COPTS $INC $DEF atiscreen.c

$LIB/atiutil.o:		atiutil.c
			$CC $COPTS $INC $DEF atiutil.c

$LIB/ativalid.o:	ativalid.c
			$CC $COPTS $INC $DEF ativalid.c

$LIB/atiload.o:		atiload.c
			$CC $COPTS $INC $DEF atiload.c

$LIB/atimisc.o:		atimisc.c
			$CC $COPTS $INC $DEF atimisc.c

$LIB/atibank.o:		atibank.c
			$CC $COPTS $INC $DEF atibank.c

$LIB/atituner.o:	atituner.c
			$CC $COPTS $INC $DEF atituner.c

$LIB/ativga.o:		ativga.c
			$CC $COPTS $INC $DEF ativga.c

$LIB/atiwonder.o:	atiwonder.c
			$CC $COPTS $INC $DEF atiwonder.c

$LIB/atiwonderio.o:	atiwonderio.c
			$CC $COPTS $INC $DEF atiwonderio.c

$LIB/atixv.o:		atixv.c
			$CC $COPTS $INC $DEF atixv.c

$LIB/r128_accel.o:	r128_accel.c
			$CC $COPTS $INC $DEF r128_accel.c

$LIB/r128_cursor.o:	r128_cursor.c
			$CC $COPTS $INC $DEF r128_cursor.c

$LIB/r128_dga.o:	r128_dga.c
			$CC $COPTS $INC $DEF r128_dga.c

$LIB/r128_driver.o:	r128_driver.c
			$CC $COPTS $INC $DEF r128_driver.c

$LIB/r128_video.o:	r128_video.c
			$CC $COPTS $INC $DEF r128_video.c

$LIB/r128_misc.o:	r128_misc.c
			$CC $COPTS $INC $DEF r128_misc.c

$LIB/r128_dri.o:	r128_dri.c
			$CC $COPTS $INC $DEF r128_dri.c

$LIB/radeon_accel.o:	radeon_accel.c
			$CC $COPTS $INC $DEF radeon_accel.c

$LIB/radeon_cursor.o:	radeon_cursor.c
			$CC $COPTS $INC $DEF radeon_cursor.c

$LIB/radeon_dga.o:	radeon_dga.c
			$CC $COPTS $INC $DEF radeon_dga.c

$LIB/radeon_driver.o:	radeon_driver.c
			$CC $COPTS $INC $DEF radeon_driver.c

$LIB/radeon_video.o:	radeon_video.c
			$CC $COPTS $INC $DEF radeon_video.c

$LIB/radeon_misc.o:	radeon_misc.c
			$CC $COPTS $INC $DEF radeon_misc.c
