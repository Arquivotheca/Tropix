#*
#****************************************************************
#	Makefile para o diretório "hw/parser"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		-D CSRG_BASED				\
			-D PROJECTROOT='"/usr/xwin"'		\
			-D TROPIX

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/xf86config

#.virtual		all
#all:			$LIB/.LIBSYM cpconfig

$LIB/.LIBSYM:		$LIB					\
			$LIB/Device.o				\
			$LIB/Files.o				\
			$LIB/Flags.o				\
			$LIB/Input.o				\
			$LIB/Keyboard.o				\
			$LIB/Layout.o				\
			$LIB/Module.o				\
			$LIB/Video.o				\
			$LIB/Monitor.o				\
			$LIB/Pointer.o				\
			$LIB/Screen.o				\
			$LIB/Vendor.o				\
			$LIB/read.o				\
			$LIB/scan.o				\
			$LIB/write.o				\
			$LIB/DRI.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/Device.o:		Device.c
			$CC $COPTS $INC $DEF Device.c

$LIB/Files.o:		Files.c
			$CC $COPTS $INC $DEF Files.c

$LIB/Flags.o:		Flags.c
			$CC $COPTS $INC $DEF Flags.c

$LIB/Input.o:		Input.c
			$CC $COPTS $INC $DEF Input.c

$LIB/Keyboard.o:	Keyboard.c
			$CC $COPTS $INC $DEF Keyboard.c

$LIB/Layout.o:		Layout.c
			$CC $COPTS $INC $DEF Layout.c

$LIB/Module.o:		Module.c
			$CC $COPTS $INC $DEF Module.c

$LIB/Video.o:		Video.c
			$CC $COPTS $INC $DEF Video.c

$LIB/Monitor.o:		Monitor.c
			$CC $COPTS $INC $DEF Monitor.c

$LIB/Pointer.o:		Pointer.c
			$CC $COPTS $INC $DEF Pointer.c

$LIB/Screen.o:		Screen.c
			$CC $COPTS $INC $DEF Screen.c

$LIB/Vendor.o:		Vendor.c
			$CC $COPTS $INC $DEF Vendor.c

$LIB/read.o:		read.c
			$CC $COPTS $INC $DEF read.c

$LIB/scan.o:		scan.c
			$CC $COPTS $INC $DEF			\
			-D XCONFIGDIR='"/usr/xwin/conf"' 	\
			-D XCONFIGFILE='"xconfig"' 		\
			-D XVERSION=4002			\
			scan.c

$LIB/write.o:		write.c
			$CC $COPTS $INC $DEF write.c

$LIB/DRI.o:		DRI.c
			$CC $COPTS $INC $DEF DRI.c

cpconfig.o:		cpconfig.c
			$CC $COPTS $INC $DEF cpconfig.c

cpconfig:		cpconfig.o $LIB/.LIBSYM
			cc -s -o cpconfig cpconfig.o $LIB $xc/lib/os/lib
