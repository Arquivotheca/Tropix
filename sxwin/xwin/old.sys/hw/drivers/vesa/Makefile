#*
#****************************************************************
#	Makefile para o diretório "hw/drivers/vesa"		*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/fb 				\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/xf24_32bpp		\
			-I $xc/sys/hw/xf4bpp 			\
			-I $xc/sys/hw/xf1bpp 	   		\
			-I $xc/sys/mfb 				\
			-I $xc/sys/afb 				\
			-I $xc/sys/mi 	   			\
			-I $xc/sys/miext/shadow 		\
			-I $xc/sys/render 	   		\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/hw/vgahw 	   		\
			-I $xc/sys/hw/common 			\
			-I $xc/sys/hw/os-support 		\
			-I $xc/sys/hw/os-support/bus 		\
			-I $xc/sys/Xext 	   		\
			-I $xc/sys/include 			\
			-I $xc/sys/hw/rac 			\
			-I $xc/sys/hw/ddc 			\
			-I $xc/sys/hw/i2c 	   		\
			-I $xc/sys/hw/os-support/vbe 		\
			-I $xc/sys/hw/int10 	   		\
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
LIB	=		$xc/sys/lib/vesa

$LIB/.LIBSYM:		$LIB					\
			$LIB/vesa.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/vesa.o:		vesa.c
			$CC $COPTS $INC $DEF vesa.c
