#*
#****************************************************************
#	Makefile para o diretório "miext/shadow"		*
#****************************************************************
#*
xc	=		../../..

INC	=		-I .					\
			-I $xc/sys/mi				\
			-I $xc/sys/fb				\
			-I $xc/sys/render			\
			-I $xc/sys/include			\
			-I $xc/sys/hw/common			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/miextshadow

$LIB/.LIBSYM:		$LIB					\
#			$LIB/shmodule.o				\
			$LIB/shadow.o				\
			$LIB/shalloc.o				\
			$LIB/shplanar.o				\
			$LIB/shplanar8.o			\
			$LIB/shpacked.o				\
			$LIB/shrotate.o				\
			$LIB/shrot8pack.o			\
			$LIB/shrot16pack.o			\
			$LIB/shrot32pack.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/shmodule.o:	shmodule.c
			$CC $COPTS $INC $DEF shmodule.c

$LIB/shadow.o:		shadow.c
			$CC $COPTS $INC $DEF shadow.c

$LIB/shalloc.o:		shalloc.c
			$CC $COPTS $INC $DEF shalloc.c

$LIB/shplanar.o:	shplanar.c
			$CC $COPTS $INC $DEF shplanar.c

$LIB/shplanar8.o:	shplanar8.c
			$CC $COPTS $INC $DEF shplanar8.c

$LIB/shpacked.o:	shpacked.c
			$CC $COPTS $INC $DEF shpacked.c

$LIB/shrotate.o:	shrotate.c
			$CC $COPTS $INC $DEF shrotate.c

$LIB/shrot8pack.o:	shrot8pack.c
			$CC $COPTS $INC $DEF shrot8pack.c

$LIB/shrot16pack.o:	shrot16pack.c
			$CC $COPTS $INC $DEF shrot16pack.c

$LIB/shrot32pack.o:	shrot32pack.c
			$CC $COPTS $INC $DEF shrot32pack.c
