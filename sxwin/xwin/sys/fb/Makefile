#*
#****************************************************************
#	Makefile para o diretório "hw/fb"			*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/mi				\
			-I $xc/sys/include			\
			-I $xc/sys/hw/common			\
			-I $xc/sys/render			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF
#			-D IN_MODULE				\
#			-D XFree86Module

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/fb

$LIB/.LIBSYM:		$LIB					\
#			$LIB/fbmodule.o				\
			$LIB/fbarc.o				\
			$LIB/fballpriv.o			\
			$LIB/fbbits.o				\
			$LIB/fbblt.o				\
			$LIB/fbbltone.o				\
			$LIB/fbbstore.o				\
			$LIB/fbcmap.o				\
			$LIB/fbcompose.o			\
			$LIB/fbcopy.o				\
			$LIB/fbfill.o				\
			$LIB/fbfillrect.o			\
			$LIB/fbfillsp.o				\
			$LIB/fbgc.o				\
			$LIB/fbgetsp.o				\
			$LIB/fbglyph.o				\
			$LIB/fbimage.o				\
			$LIB/fbline.o				\
			$LIB/fbseg.o				\
			$LIB/fboverlay.o			\
			$LIB/fbpixmap.o				\
			$LIB/fbpoint.o				\
			$LIB/fbpush.o				\
			$LIB/fbscreen.o				\
			$LIB/fbsetsp.o				\
			$LIB/fbsolid.o				\
			$LIB/fbstipple.o			\
			$LIB/fbtile.o				\
			$LIB/fbtrap.o				\
			$LIB/fbutil.o				\
			$LIB/fbwindow.o				\
			$LIB/fb24_32.o				\
			$LIB/fbpict.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/fbmodule.o:	fbmodule.c
			$CC $COPTS $INC $DEF fbmodule.c

$LIB/fbarc.o:		fbarc.c
			$CC $COPTS $INC $DEF fbarc.c

$LIB/fballpriv.o:	fballpriv.c
			$CC $COPTS $INC $DEF fballpriv.c

$LIB/fbbits.o:		fbbits.c
			$CC $COPTS $INC $DEF fbbits.c

$LIB/fbblt.o:		fbblt.c
			$CC $COPTS $INC $DEF fbblt.c

$LIB/fbbltone.o:	fbbltone.c
			$CC $COPTS $INC $DEF fbbltone.c

$LIB/fbbstore.o:	fbbstore.c
			$CC $COPTS $INC $DEF fbbstore.c

$LIB/fbcmap.o:		fbcmap.c
			$CC $COPTS $INC $DEF fbcmap.c

$LIB/fbcompose.o:	fbcompose.c
			$CC $COPTS $INC $DEF fbcompose.c

$LIB/fbcopy.o:		fbcopy.c
			$CC $COPTS $INC $DEF fbcopy.c

$LIB/fbfill.o:		fbfill.c
			$CC $COPTS $INC $DEF fbfill.c

$LIB/fbfillrect.o:	fbfillrect.c
			$CC $COPTS $INC $DEF fbfillrect.c

$LIB/fbfillsp.o:	fbfillsp.c
			$CC $COPTS $INC $DEF fbfillsp.c

$LIB/fbgc.o:		fbgc.c
			$CC $COPTS $INC $DEF fbgc.c

$LIB/fbgetsp.o:		fbgetsp.c
			$CC $COPTS $INC $DEF fbgetsp.c

$LIB/fbglyph.o:		fbglyph.c
			$CC $COPTS $INC $DEF fbglyph.c

$LIB/fbimage.o:		fbimage.c
			$CC $COPTS $INC $DEF fbimage.c

$LIB/fbline.o:		fbline.c
			$CC $COPTS $INC $DEF fbline.c

$LIB/fbseg.o:		fbseg.c
			$CC $COPTS $INC $DEF fbseg.c

$LIB/fboverlay.o:	fboverlay.c
			$CC $COPTS $INC $DEF fboverlay.c

$LIB/fbpixmap.o:	fbpixmap.c
			$CC $COPTS $INC $DEF fbpixmap.c

$LIB/fbpoint.o:		fbpoint.c
			$CC $COPTS $INC $DEF fbpoint.c

$LIB/fbpush.o:		fbpush.c
			$CC $COPTS $INC $DEF fbpush.c

$LIB/fbscreen.o:	fbscreen.c
			$CC $COPTS $INC $DEF fbscreen.c

$LIB/fbsetsp.o:		fbsetsp.c
			$CC $COPTS $INC $DEF fbsetsp.c

$LIB/fbsolid.o:		fbsolid.c
			$CC $COPTS $INC $DEF fbsolid.c

$LIB/fbstipple.o:	fbstipple.c
			$CC $COPTS $INC $DEF fbstipple.c

$LIB/fbtile.o:		fbtile.c
			$CC $COPTS $INC $DEF fbtile.c

$LIB/fbtrap.o:		fbtrap.c
			$CC $COPTS $INC $DEF fbtrap.c

$LIB/fbutil.o:		fbutil.c
			$CC $COPTS $INC $DEF fbutil.c

$LIB/fbwindow.o:	fbwindow.c
			$CC $COPTS $INC $DEF fbwindow.c

$LIB/fb24_32.o:		fb24_32.c
			$CC $COPTS $INC $DEF fb24_32.c

$LIB/fbpict.o:		fbpict.c
			$CC $COPTS $INC $DEF fbpict.c
