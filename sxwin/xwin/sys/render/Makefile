#*
#****************************************************************
#	Makefile para o diretório "render"			*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/include			\
			-I $xc/sys/mi				\
	    		-I $xc/sys/fb				\
			-I $xc/sys/hw/kdrive			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions

DEF	=		$STDDEF

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/render

$LIB/.LIBSYM:		$LIB					\
			$LIB/animcur.o				\
			$LIB/filter.o				\
			$LIB/glyph.o				\
			$LIB/miglyph.o				\
			$LIB/miindex.o				\
			$LIB/mipict.o				\
			$LIB/mirect.o				\
			$LIB/mitrap.o				\
			$LIB/mitri.o				\
			$LIB/picture.o				\
			$LIB/render.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/animcur.o:		animcur.c
			$CC $COPTS $INC $DEF animcur.c

$LIB/filter.o:		filter.c
			$CC $COPTS $INC $DEF filter.c

$LIB/glyph.o:		glyph.c
			$CC $COPTS $INC $DEF glyph.c

$LIB/miglyph.o:		miglyph.c
			$CC $COPTS $INC $DEF miglyph.c

$LIB/miindex.o:		miindex.c
			$CC $COPTS $INC $DEF miindex.c

$LIB/mipict.o:		mipict.c
			$CC $COPTS $INC $DEF mipict.c

$LIB/mirect.o:		mirect.c
			$CC $COPTS $INC $DEF mirect.c

$LIB/mitrap.o:		mitrap.c
			$CC $COPTS $INC $DEF mitrap.c

$LIB/mitri.o:		mitri.c
			$CC $COPTS $INC $DEF mitri.c

$LIB/picture.o:		picture.c
			$CC $COPTS $INC $DEF picture.c

$LIB/render.o:		render.c
			$CC $COPTS $INC $DEF render.c
