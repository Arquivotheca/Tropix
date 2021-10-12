#*
#****************************************************************
#	Makefile para o cliente "xpaint"			*
#****************************************************************
#*
PGM	=	xpaint
DIR	=	/usr/pub/bin
#DIR	=	/usr/xwin/bin
MAN	=	xwin
PROT	=	a=rx,u+w

CC	=	cc
COPTS	=	-c -O -o $@

OBJ	=	obj/$PGM.o		\
		obj/bmp.o		\
		obj/colors.o		\
		obj/gif.o		\
		obj/jpeg.o		\
		obj/xpm.o		\
		obj/new.gif.o

SRC	=	src/$PGM.c		\
		src/colors.c		\
		src/bmp.c		\
		src/gif.c		\
		src/jpeg.c		\
		src/xpm.c		\
		src/new.gif.c

.virtual	cp cmp cmpq cmpqcp unld lint lintg man modif

$PGM:		obj $OBJ
		cc -u -o $PGM $OBJ -ly.o -lx.o
		size $PGM

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c		\
		h/xpaint.h
		$CC $COPTS src/$PGM.c

obj/bmp.o:	src/bmp.c		\
		h/xpaint.h
		$CC $COPTS src/bmp.c

obj/colors.o:	src/colors.c		\
		h/xpaint.h
		$CC $COPTS src/colors.c

obj/gif.o:	src/gif.c		\
		h/xpaint.h
		$CC $COPTS src/gif.c

obj/new.gif.o:	src/new.gif.c		\
		h/xpaint.h
		$CC $COPTS src/new.gif.c

obj/jpeg.o:	src/jpeg.c		\
		h/xpaint.h
		$CC $COPTS src/jpeg.c

obj/xpm.o:	src/xpm.c		\
		h/xpaint.h
		$CC $COPTS src/xpm.c

cp:		$PGM $DIR
		cp -f $PGM $DIR/debug.$PGM
		chmod $PROT $DIR/debug.$PGM
		ls -L $DIR/debug.$PGM
#		cp -f $PGM $DIR/$PGM
#		chmod $PROT $DIR/$PGM
#		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

cmp:
		-cmpobj $PGM $DIR/debug.$PGM
#		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
		-rm -f $PGM

lint:		$SRC
		cc -T $SRC

lintg:
		cc -T src/new.gif.c

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
