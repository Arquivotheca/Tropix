#*
#****************************************************************
#	Makefile para o diretório "hw/os-support/tropix"	*
#****************************************************************
#*
xc	=		../../../..

INC	=		-I .					\
			-I $xc/sys/include 			\
			-I $xc/sys/hw	 			\
			-I $xc/sys/hw/common 			\
			-I $xc/sys/hw/vbe			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/ddc      			\
			-I $xc/sys/hw/i2c      			\
			-I $xc/sys/hw/int10			\
			-I $xc/sys/hw         			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/extensions		\
			-I $xc/sys/mi

DEF	=		$STDDEF					\
			-D USESTDRES
#			-D PCCONS_SUPPORT
#			-D SYSCONS_SUPPORT
#			-D PCVT_SUPPORT  
#			-D USE_DEV_IO
#			-D USESTDRES
#			-D HAS_MTRR_SUPPORT

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/tropix

$LIB/.LIBSYM:		$LIB					\
			$LIB/init.o				\
			$LIB/kbd.o				\
			$LIB/mouse.o				\
			$LIB/pci.o				\
			$LIB/pm.o				\
			$LIB/resource.o				\
			$LIB/sigio.o				\
			$LIB/slowbcopy.o			\
			$LIB/time.o				\
			$LIB/tty.o				\
			$LIB/vbe.o				\
			$LIB/video.o				\
			$LIB/vidmem.o				\
			$LIB/vtsw.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/init.o:		init.c
			$CC $COPTS $INC $DEF init.c

$LIB/kbd.o:		kbd.c
			$CC $COPTS $INC $DEF kbd.c

$LIB/mouse.o:		mouse.c
			$CC $COPTS $INC $DEF mouse.c

$LIB/pci.o:		pci.c
			$CC $COPTS $INC $DEF pci.c

$LIB/pm.o:		pm.c
			$CC $COPTS $INC $DEF pm.c

$LIB/resource.o:	resource.c
			$CC $COPTS $INC $DEF resource.c

$LIB/sigio.o:		sigio.c
			$CC $COPTS $INC $DEF sigio.c

$LIB/slowbcopy.o:	slowbcopy.c
			$CC $COPTS $INC $DEF slowbcopy.c

$LIB/time.o:		time.c
			$CC $COPTS $INC $DEF time.c

$LIB/tty.o:		tty.c
			$CC $COPTS $INC $DEF tty.c

$LIB/vbe.o:		vbe.c
			$CC $COPTS $INC $DEF vbe.c

$LIB/video.o:		video.c
			$CC $COPTS $INC $DEF video.c

$LIB/vidmem.o:		vidmem.c
			$CC $COPTS $INC $DEF vidmem.c

$LIB/vtsw.o:		vtsw.c
			$CC $COPTS $INC $DEF vtsw.c
