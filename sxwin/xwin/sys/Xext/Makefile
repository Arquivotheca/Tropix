#*
#****************************************************************
#	Makefile para o diretório "Xext"			*
#****************************************************************
#*
xc	=		../..

INC	=		-I extmod				\
			-I $xc/sys/include			\
			-I $xc/sys/mi 				\
			-I $xc/sys/hw 				\
			-I $xc/sys/hw/os-support/bus		\
			-I $xc/sys/hw/common			\
			-I $xc/sys/hw/os			\
			-I $xc 					\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/extensions		\
			-I $xc/include/X11/fonts

DEF	=		$STDDEF $EXTDEF

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/Xext

$LIB/.LIBSYM:		$LIB					\
			$LIB/shape.o				\
			$LIB/mitmisc.o				\
			$LIB/xtest.o				\
			$LIB/xtest1di.o				\
			$LIB/xtest1dd.o				\
			$LIB/sleepuntil.o			\
			$LIB/bigreq.o				\
			$LIB/sync.o				\
			$LIB/saver.o				\
			$LIB/xcmisc.o				\
			$LIB/xf86vmode.o			\
			$LIB/dpms.o				\
			$LIB/xf86misc.o				\
			$LIB/fontcache.o			\
			$LIB/xf86bigfont.o			\
			$LIB/xvmain.o				\
			$LIB/xvdisp.o				\
			$LIB/xvmc.o				\
			$LIB/xf86dga.o				\
			$LIB/xf86dga2.o				\
			$LIB/EVI.o				\
			$LIB/sampleEVI.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/shape.o:		shape.c
			$CC $COPTS $INC $DEF shape.c

$LIB/shm.o:		shm.c
			$CC $COPTS $INC $DEF shm.c

$LIB/mitmisc.o:		mitmisc.c
			$CC $COPTS $INC $DEF mitmisc.c

$LIB/xtest.o:		xtest.c
			$CC $COPTS $INC $DEF xtest.c

$LIB/xtest1di.o:	xtest1di.c
			$CC $COPTS $INC $DEF xtest1di.c

$LIB/xtest1dd.o:	xtest1dd.c
			$CC $COPTS $INC $DEF xtest1dd.c

$LIB/sleepuntil.o:	sleepuntil.c
			$CC $COPTS $INC $DEF sleepuntil.c

$LIB/bigreq.o:		bigreq.c
			$CC $COPTS $INC $DEF bigreq.c

$LIB/sync.o:		sync.c
			$CC $COPTS $INC $DEF sync.c

$LIB/saver.o:		saver.c
			$CC $COPTS $INC $DEF saver.c

$LIB/xcmisc.o:		xcmisc.c
			$CC $COPTS $INC $DEF xcmisc.c

$LIB/xf86vmode.o:	xf86vmode.c
			$CC $COPTS $INC $DEF xf86vmode.c

$LIB/xf86misc.o:	xf86misc.c
			$CC $COPTS $INC $DEF xf86misc.c

$LIB/xf86bigfont.o:	xf86bigfont.c
			$CC $COPTS $INC $DEF xf86bigfont.c

$LIB/xf86dga.o:		xf86dga.c
			$CC $COPTS $INC $DEF xf86dga.c

$LIB/xf86dga2.o:	xf86dga2.c
			$CC $COPTS $INC $DEF xf86dga2.c

$LIB/security.o:	security.c
			$CC $COPTS $INC $DEF security.c

$LIB/appgroup.o:	appgroup.c
			$CC $COPTS $INC $DEF appgroup.c

$LIB/xprint.o:		xprint.c
			$CC $COPTS $INC $DEF xprint.c

$LIB/cup.o:		cup.c
			$CC $COPTS $INC $DEF cup.c

$LIB/panoramiX.o:	panoramiX.c
			$CC $COPTS $INC $DEF panoramiX.c

$LIB/panoramiXSwap.o:	panoramiXSwap.c
			$CC $COPTS $INC $DEF panoramiXSwap.c

$LIB/panoramiXprocs.o:	panoramiXprocs.c
			$CC $COPTS $INC $DEF panoramiXprocs.c

$LIB/dpms.o:		dpms.c
			$CC $COPTS $INC $DEF dpms.c

$LIB/EVI.o:		EVI.c
			$CC $COPTS $INC $DEF EVI.c

$LIB/sampleEVI.o:	sampleEVI.c
			$CC $COPTS $INC $DEF sampleEVI.c

$LIB/xvmain.o:		xvmain.c
			$CC $COPTS $INC $DEF xvmain.c

$LIB/xvdisp.o:		xvdisp.c
			$CC $COPTS $INC $DEF xvdisp.c

$LIB/xvmc.o:		xvmc.c
			$CC $COPTS $INC $DEF xvmc.c

$LIB/fontcache.o:	fontcache.c
			$CC $COPTS $INC $DEF fontcache.c
