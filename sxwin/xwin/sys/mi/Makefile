#*
#****************************************************************
#	Makefile para o diretório "mi"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/include			\
			-I $xc/sys/render			\
			-I $xc					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/mi

$LIB/.LIBSYM:		$LIB					\
			$LIB/mivaltree.o			\
			$LIB/mipolyseg.o			\
			$LIB/mipolyrect.o			\
			$LIB/mipoly.o				\
			$LIB/mipolycon.o			\
			$LIB/mipolygen.o			\
			$LIB/mipolyutil.o			\
			$LIB/mifillrct.o			\
			$LIB/miwideline.o			\
			$LIB/mispans.o				\
			$LIB/miarc.o				\
			$LIB/mizerarc.o				\
			$LIB/mifillarc.o			\
			$LIB/miwindow.o				\
			$LIB/micursor.o				\
			$LIB/miregion.o				\
			$LIB/mipolytext.o			\
			$LIB/mibitblt.o				\
			$LIB/mipolypnt.o			\
			$LIB/mipushpxl.o			\
			$LIB/miexpose.o				\
			$LIB/miglblt.o				\
			$LIB/mizerline.o			\
			$LIB/mifpolycon.o			\
			$LIB/midash.o				\
			$LIB/mibstore.o				\
			$LIB/miinitext.o			\
			$LIB/mieq.o				\
			$LIB/mipointer.o			\
			$LIB/misprite.o				\
			$LIB/midispcur.o			\
			$LIB/miscrinit.o			\
			$LIB/miclipn.o				\
			$LIB/migc.o				\
			$LIB/cbrt.o				\
			$LIB/mizerclip.o			\
			$LIB/mibank.o				\
			$LIB/micmap.o				\
			$LIB/mioverlay.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/mivaltree.o:	mivaltree.c	
			$CC $COPTS $INC $STDDEF mivaltree.c

$LIB/mipolyseg.o:	mipolyseg.c	
			$CC $COPTS $INC $STDDEF mipolyseg.c

$LIB/mipolyrect.o:	mipolyrect.c	
			$CC $COPTS $INC $STDDEF mipolyrect.c

$LIB/mipoly.o:		mipoly.c	
			$CC $COPTS $INC $STDDEF mipoly.c

$LIB/mipolycon.o:	mipolycon.c	
			$CC $COPTS $INC $STDDEF mipolycon.c

$LIB/mipolygen.o:	mipolygen.c	
			$CC $COPTS $INC $STDDEF mipolygen.c

$LIB/mipolyutil.o:	mipolyutil.c	
			$CC $COPTS $INC $STDDEF mipolyutil.c

$LIB/mifillrct.o:	mifillrct.c	
			$CC $COPTS $INC $STDDEF mifillrct.c

$LIB/miwideline.o:	miwideline.c	
			$CC $COPTS $INC $STDDEF miwideline.c

$LIB/mispans.o:		mispans.c	
			$CC $COPTS $INC $STDDEF mispans.c

$LIB/miarc.o:		miarc.c		
			$CC $COPTS $INC $STDDEF miarc.c

$LIB/mizerarc.o:	mizerarc.c	
			$CC $COPTS $INC $STDDEF mizerarc.c

$LIB/mifillarc.o:	mifillarc.c	
			$CC $COPTS $INC $STDDEF mifillarc.c

$LIB/miwindow.o:	miwindow.c	
			$CC $COPTS $INC $STDDEF miwindow.c

$LIB/micursor.o:	micursor.c	
			$CC $COPTS $INC $STDDEF micursor.c

$LIB/miregion.o:	miregion.c	
			$CC $COPTS $INC $STDDEF miregion.c

$LIB/mipolytext.o:	mipolytext.c	
			$CC $COPTS $INC $STDDEF mipolytext.c

$LIB/mibitblt.o:	mibitblt.c	
			$CC $COPTS $INC $STDDEF mibitblt.c

$LIB/mipolypnt.o:	mipolypnt.c	
			$CC $COPTS $INC $STDDEF mipolypnt.c

$LIB/mipushpxl.o:	mipushpxl.c	
			$CC $COPTS $INC $STDDEF mipushpxl.c

$LIB/miexpose.o:	miexpose.c	
			$CC $COPTS $INC $STDDEF miexpose.c

$LIB/miglblt.o:		miglblt.c	
			$CC $COPTS $INC $STDDEF miglblt.c

$LIB/mizerline.o:	mizerline.c	
			$CC $COPTS $INC $STDDEF mizerline.c

$LIB/mifpolycon.o:	mifpolycon.c
			$CC $COPTS $INC $STDDEF mifpolycon.c

$LIB/midash.o:		midash.c	
			$CC $COPTS $INC $STDDEF midash.c

$LIB/mibstore.o:	mibstore.c	
			$CC $COPTS $INC $STDDEF mibstore.c

$LIB/miinitext.o:	miinitext.c	
			$CC $COPTS $INC $STDDEF $EXTDEF miinitext.c

$LIB/mieq.o:		mieq.c		
			$CC $COPTS $INC $STDDEF mieq.c

$LIB/mipointer.o:	mipointer.c	
			$CC $COPTS $INC $STDDEF mipointer.c

$LIB/misprite.o:	misprite.c	
			$CC $COPTS $INC $STDDEF misprite.c

$LIB/midispcur.o:	midispcur.c	
			$CC $COPTS $INC $STDDEF midispcur.c

$LIB/miscrinit.o:	miscrinit.c	
			$CC $COPTS $INC $STDDEF $EXTDEF miscrinit.c

$LIB/miclipn.o:		miclipn.c	
			$CC $COPTS $INC $STDDEF miclipn.c

$LIB/migc.o:		migc.c		
			$CC $COPTS $INC $STDDEF migc.c

$LIB/cbrt.o:		cbrt.c
			$CC $COPTS cbrt.c

$LIB/mizerclip.o:	mizerclip.c	
			$CC $COPTS $INC $STDDEF mizerclip.c

$LIB/mibank.o:		mibank.c
			$CC $COPTS $INC $STDDEF mibank.c

$LIB/micmap.o:		micmap.c
			$CC $COPTS $INC $STDDEF micmap.c

$LIB/mioverlay.o:	mioverlay.c
			$CC $COPTS $INC $STDDEF mioverlay.c
