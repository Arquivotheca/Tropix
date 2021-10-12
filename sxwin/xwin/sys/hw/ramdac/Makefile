#*
#****************************************************************
#	Makefile para o diretório "hw/ramdac"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/Xext				\
			-I $xc/sys/cfb				\
			-I $xc/sys/mfb				\
			-I $xc/sys/mi				\
			-I $xc/sys/render			\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/ramdac

$LIB/.LIBSYM:		$LIB					\
			$LIB/xf86RamDacMod.o			\
			$LIB/xf86RamDac.o			\
			$LIB/xf86RamDacCmap.o			\
			$LIB/xf86Cursor.o			\
			$LIB/xf86HWCurs.o			\
			$LIB/xf86BitOrder.o			\
			$LIB/IBM.o				\
			$LIB/BT.o				\
			$LIB/TI.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xf86RamDacMod.o:	xf86RamDacMod.c
			$CC $COPTS $INC $DEF xf86RamDacMod.c

$LIB/xf86RamDac.o:	xf86RamDac.c
			$CC $COPTS $INC $DEF xf86RamDac.c

$LIB/xf86RamDacCmap.o:	xf86RamDacCmap.c
			$CC $COPTS $INC $DEF xf86RamDacCmap.c

$LIB/xf86Cursor.o:	xf86Cursor.c
			$CC $COPTS $INC $DEF xf86Cursor.c

$LIB/xf86HWCurs.o:	xf86HWCurs.c
			$CC $COPTS $INC $DEF xf86HWCurs.c

$LIB/xf86BitOrder.o:	../xaa/xaaBitOrder.c
			$CC $COPTS $INC $DEF				\
			-D XAAReverseBitOrder=xf86ReverseBitOrder	\
#			-D RAMDAC_MODULE				\
			../xaa/xaaBitOrder.c

$LIB/IBM.o:		IBM.c
			$CC $COPTS $INC $DEF IBM.c

$LIB/BT.o:		BT.c
			$CC $COPTS $INC $DEF BT.c

$LIB/TI.o:		TI.c
			$CC $COPTS $INC $DEF TI.c
