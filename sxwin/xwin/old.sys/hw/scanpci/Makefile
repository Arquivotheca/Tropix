#*
#****************************************************************
#	Makefile para o diretório "hw/scanpci"			*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/hw				\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os-support		\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/scanpci			\
			-I $xc/sys/include			\
			-I $xc/include				\
			-I $xc/include/X11

DEF	=		$STDDEF

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/scanpci

$LIB/.LIBSYM:		$LIB					\
#			$LIB/xf86ScanPci.o			\
			$LIB/xf86PciData.o			\
			$LIB/xf86PciTable.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/xf86ScanPci.o:	xf86ScanPci.c
			$CC $COPTS $INC $DEF xf86ScanPci.c

$LIB/xf86PciData.o:	xf86ScanPci.c
			$CC $COPTS $INC $DEF -D PCIDATA xf86ScanPci.c

$LIB/xf86PciTable.o:	xf86PciTable.c
			$CC $COPTS $INC $DEF -D PCIDATA xf86PciTable.c

