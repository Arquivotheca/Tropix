#*
#****************************************************************
#	Makefile para o diretório "hw/common"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/os-support 		\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/hw/os-support/bus 		\
			-I $xc/sys/hw/parser 			\
			-I $xc/sys/hw/loader 			\
			-I $xc/sys/hw/vgahw 			\
			-I $xc/sys/hw/rac 	        	\
			-I $xc/sys/hw/scanpci 			\
			-I $xc/sys/hw/os-support/vbe 		\
			-I $xc/sys/hw/int10 			\
			-I $xc/sys/hw/ddc 			\
			-I $xc/sys/hw/ddc 			\
			-I $xc/sys/hw/i2c 			\
			-I $xc/sys/hw/vgahw			\
			-I $xc/sys/mfb 				\
			-I $xc/sys/mi 				\
			-I $xc/sys/include			\
			-I $xc/sys/os				\
			-I $xc/sys/Xext 			\
			-I $xc/sys/Xi				\
			-I $xc/sys/render			\
			-I $xc/include	 			\
			-I $xc/include/X11 			\
			-I $xc/include/X11/extensions		\
			-I $xc/include/X11/fonts		\
			-I $xc/lib/font/include

DEF	=		$STDDEF $EXTDEF				\
			-D PROJECTROOT='"/usr/xwin"'		\
			-D OSNAME='" Tropix 4.9.0"'

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/common

$LIB/.LIBSYM:		$LIB					\
			$LIB/drvConf.o				\
			$LIB/xf86AutoConfig.o			\
			$LIB/xf86Init.o				\
#			$LIB/xf86IniExt.o			\
			$LIB/xf86Configure.o			\
			$LIB/xf86Config.o			\
			$LIB/xf86Bus.o				\
			$LIB/xf86isaBus.o			\
			$LIB/xf86pciBus.o			\
			$LIB/xf86fbBus.o			\
			$LIB/xf86Bus.o				\
			$LIB/xf86Cursor.o			\
			$LIB/xf86DGA.o				\
			$LIB/xf86DPMS.o				\
			$LIB/xf86DefModes.o			\
			$LIB/xf86DoProbe.o			\
			$LIB/xf86DoScanPci.o			\
			$LIB/xf86Events.o			\
			$LIB/xf86Globals.o			\
			$LIB/xf86Helper.o			\
			$LIB/xf86Io.o				\
			$LIB/xf86MiscExt.o			\
			$LIB/xf86Mode.o				\
			$LIB/xf86Option.o			\
			$LIB/xf86Versions.o			\
			$LIB/xf86VidMode.o			\
			$LIB/xf86fbman.o			\
			$LIB/xf86xv.o				\
			$LIB/xf86cmap.o				\
			$LIB/xf86PM.o				\
			$LIB/xf86Debug.o			\
			$LIB/xf86Xinput.o			\
#			$LIB/xf86Switch.o			\
			$LIB/xisb.o				\
			$LIB/xf86XKB.o				\
			$LIB/xf86KbdBSD.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/drvConf.o:		drvConf.c
			$CC $COPTS $INC $DEF drvConf.c

$LIB/xf86AutoConfig.o:	xf86AutoConfig.c
			$CC $COPTS $INC $DEF xf86AutoConfig.c

$LIB/xf86Init.o:	xf86Init.c
			$CC $COPTS $INC $DEF				\
				-D OSVENDOR='" NCE/UFRJ"'		\
				-D BUILDERADDR='"tropix@nce.ufrj.br"'  	\
				-D XORG_RELEASE='"4.4.0"'		\
				xf86Init.c

#$LIB/xf86IniExt.o:	../../mi/miinitext.c
#			$CC $COPTS $INC $DEF ../../mi/miinitext.c

$LIB/xf86Configure.o:	xf86Configure.c
			$CC $COPTS $INC $DEF				\
				-D XF86CONFIGFILE='"xconfig"'		\
				xf86Configure.c

$LIB/xf86Bus.o:		xf86Bus.c
			$CC $COPTS $INC $DEF				\
				-D BUILDERADDR='"tropix@nce.ufrj.br"'  	\
				xf86Bus.c

$LIB/xf86isaBus.o:	xf86isaBus.c
			$CC $COPTS $INC $DEF xf86isaBus.c

$LIB/xf86pciBus.o:	xf86pciBus.c
			$CC $COPTS $INC $DEF xf86pciBus.c

$LIB/xf86fbBus.o:	xf86fbBus.c
			$CC $COPTS $INC $DEF xf86fbBus.c

$LIB/xf86Config.o:	xf86Config.c
			$CC $COPTS $INC $DEF					\
				-D ROOT_CONFIGPATH='"/usr/xwin/conf/xconfig"'	\
				-D USER_CONFIGPATH='"/usr/xwin/conf/xconfig"'	\
				-D DRIVERS='"s3virge trident ati vesa vga"'	\
				-D IDRIVERS='"mouse"'				\
				-D XF86CONFIGFILE='"xconfig"'			\
				xf86Config.c

$LIB/xf86Cursor.o:	xf86Cursor.c
			$CC $COPTS $INC $DEF xf86Cursor.c

$LIB/xf86DGA.o:		xf86DGA.c
			$CC $COPTS $INC $DEF xf86DGA.c

$LIB/xf86DPMS.o:	xf86DPMS.c
			$CC $COPTS $INC $DEF xf86DPMS.c

$LIB/xf86DefModes.o:	xf86DefModes.c
			$CC $COPTS $INC $DEF xf86DefModes.c

$LIB/xf86DoProbe.o:	xf86DoProbe.c
			$CC $COPTS $INC $DEF xf86DoProbe.c

$LIB/xf86DoScanPci.o:	xf86DoScanPci.c
			$CC $COPTS $INC $DEF xf86DoScanPci.c

$LIB/xf86Events.o:	xf86Events.c
			$CC $COPTS $INC $DEF xf86Events.c

$LIB/xf86Globals.o:	xf86Globals.c
			$CC $COPTS $INC $DEF					\
				-D DEFAULT_MODULE_PATH=\"/usr/xwin/modules\"	\
				-D DEFAULT_LOGPREFIX=\"/var/log/xwin/xserver\"	\
				xf86Globals.c

$LIB/xf86Helper.o:	xf86Helper.c
			$CC $COPTS $INC $DEF xf86Helper.c

$LIB/xf86Io.o:		xf86Io.c
			$CC $COPTS $INC $DEF xf86Io.c

$LIB/xf86MiscExt.o:	xf86MiscExt.c
			$CC $COPTS $INC $DEF xf86MiscExt.c

$LIB/xf86Mode.o:	xf86Mode.c
			$CC $COPTS $INC $DEF xf86Mode.c

$LIB/xf86Option.o:	xf86Option.c
			$CC $COPTS $INC $DEF xf86Option.c

$LIB/xf86Versions.o:	xf86Versions.c
			$CC $COPTS $INC $DEF xf86Versions.c

$LIB/xf86VidMode.o:	xf86VidMode.c
			$CC $COPTS $INC $DEF xf86VidMode.c

$LIB/xf86fbman.o:	xf86fbman.c
			$CC $COPTS $INC $DEF xf86fbman.c

$LIB/xf86xv.o:		xf86xv.c
			$CC $COPTS $INC $DEF xf86xv.c

$LIB/xf86cmap.o:	xf86cmap.c
			$CC $COPTS $INC $DEF -D entry=_entry xf86cmap.c

$LIB/xf86PM.o:		xf86PM.c
			$CC $COPTS $INC $DEF xf86PM.c

$LIB/xf86Debug.o:	xf86Debug.c
			$CC $COPTS $INC $DEF xf86Debug.c

$LIB/xf86Xinput.o:	xf86Xinput.c
			$CC $COPTS $INC $DEF xf86Xinput.c

$LIB/xf86Switch.o:	xf86Switch.c
			$CC $COPTS $INC $DEF xf86Switch.c

$LIB/xisb.o:		xisb.c
			$CC $COPTS $INC $DEF xisb.c

$LIB/xf86XKB.o:		xf86XKB.c
			$CC $COPTS $INC $DEF xf86XKB.c

$LIB/xf86KbdBSD.o:	xf86KbdBSD.c
			$CC $COPTS $INC $DEF xf86KbdBSD.c
