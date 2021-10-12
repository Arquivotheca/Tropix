#*
#****************************************************************
#	makefile para o programa "fvwm"				*
#****************************************************************
#*
PGM	=		fvwm
DIR	=		/usr/xwin/bin
LIBDIR	=		/usr/xwin/lib
MAN	=		xwin
PROT	=		a=rx,u+w

CC	=		cc
COPTS	=		-c -O -o $@

INC	=		-I .					\
			-I h

SRC	=		src/add_window.c			\
			src/bindings.c				\
			src/borders.c				\
			src/builtins.c				\
			src/colormaps.c				\
			src/colors.c				\
			src/complex.c				\
			src/decorations.c			\
			src/events.c				\
			src/focus.c				\
			src/functions.c				\
			src/fvwm.c				\
			src/icons.c				\
			src/menus.c				\
			src/misc.c				\
			src/module.c				\
			src/move.c				\
			src/placement.c				\
			src/read.c				\
			src/resize.c				\
			src/style.c				\
			src/virtual.c				\
			src/windows.c

OBJ	=		obj/add_window.o			\
			obj/bindings.o				\
			obj/borders.o				\
			obj/builtins.o				\
			obj/colormaps.o				\
			obj/colors.o				\
			obj/complex.o				\
			obj/decorations.o			\
			obj/events.o				\
			obj/focus.o				\
			obj/functions.o				\
			obj/fvwm.o				\
			obj/icons.o				\
			obj/menus.o				\
			obj/misc.o				\
			obj/module.o				\
			obj/move.o				\
			obj/placement.o				\
			obj/read.o				\
			obj/resize.o				\
			obj/style.o				\
			obj/virtual.o				\
			obj/windows.o

.virtual		cp cmp cmpq cmpqcp unld lint man modif

$PGM:			obj $OBJ
			cc -us -o $PGM $OBJ ../../../lib/Xmu/lib ../lib -lx.o
			size $PGM

#				$LIBDIR/libXmu/AllCmap.o	\
#				$LIBDIR/libXmu/VisCmap.o	\
#				$LIBDIR/libXmu/LookupCmap.o	\
#				$LIBDIR/libXmu/DelCmap.o	\
#				$LIBDIR/libXmu/StdCmap.o	\
#				$LIBDIR/libXmu/CrCmap.o		\
#				$LIBDIR/libXmu/CmapAlloc.o	\
#				../lib				\
#				-lx.o
#			size $PGM

obj:			.exist
			mkdir -f obj
#*
#****************************************************************
#	Compilação dos Diversos Módulos				*
#****************************************************************
#*
obj/add_window.o:	src/add_window.c			\
			../global.h				\
			h/fvwm.h				\
			h/misc.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/add_window.c

obj/bindings.o:		src/bindings.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/bindings.c

obj/borders.o:		src/borders.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/borders.c

obj/builtins.o:		src/builtins.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/builtins.c

obj/colormaps.o:	src/colormaps.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/colormaps.c

obj/colors.o:		src/colors.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h
			$CC $INC $COPTS src/colors.c

obj/complex.o:		src/complex.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/complex.c

obj/decorations.o:	src/decorations.c			\
			../global.h				\
			h/fvwm.h				\
			h/misc.h				\
			h/screen.h				\
			h/parse.h				\
			h/menus.h
			$CC $INC $COPTS src/decorations.c

obj/events.o:		src/events.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/events.c

obj/focus.o:		src/focus.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/focus.c

obj/functions.o:	src/functions.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/functions.c

obj/fvwm.o:		src/fvwm.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/screen.h				\
			h/parse.h				\
			h/module.h
			$CC $INC $COPTS src/fvwm.c

obj/icons.o:		src/icons.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/icons.c

obj/menus.o:		src/menus.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h
			$CC $INC $COPTS src/menus.c

obj/misc.o:		src/misc.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/misc.c

obj/module.o:		src/module.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/module.c

obj/move.o:		src/move.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/move.c

obj/placement.o:	src/placement.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h
			$CC $INC $COPTS src/placement.c

obj/read.o:		src/read.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/read.c

obj/resize.o:		src/resize.c				\
			../global.h				\
			h/fvwm.h				\
			h/misc.h				\
			h/screen.h				\
			h/parse.h
			$CC $INC $COPTS src/resize.c

obj/style.o:		src/style.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h
			$CC $INC $COPTS src/style.c

obj/virtual.o:		src/virtual.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h				\
			h/module.h
			$CC $INC $COPTS src/virtual.c

obj/windows.o:		src/windows.c				\
			../global.h				\
			h/fvwm.h				\
			h/menus.h				\
			h/misc.h				\
			h/parse.h				\
			h/screen.h
			$CC $INC $COPTS src/windows.c

#*
#****************************************************************
#	Cópia/Comparação/Manuais				*
#****************************************************************
#*
cp:			$PGM $DIR
			cp -f $PGM $DIR/$PGM
			chmod $PROT $DIR/$PGM
			ls -L $DIR/$PGM

$DIR:			.exist
			mkdir -f $DIR

cmp:
			-cmpobj $PGM $DIR/$PGM

cmpq:
			-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
			-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
			-rm -f $PGM

lint:			$SRC
			$CC -T $INC $SRC

man:			man.r
			sroff man.r >$mandir/$MAN/$PGM
			man $PGM

modif:			modif.r
			sroff modif.r >$mandir/modif/$MAN/$PGM
			modif $PGM
