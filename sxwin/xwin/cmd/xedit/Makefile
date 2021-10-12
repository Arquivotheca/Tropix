#*
#****************************************************************
#	Makefile para o cliente "xedit"				*
#****************************************************************
#*
PGM	=	xedit
DIR	=	/usr/xwin/bin
LIBDIR	=	/usr/xwin/lib
CONF	=	/usr/xwin/conf/Xt
MAN	=	xwin
PROT	=	a=rx,u+w

SRC	=	src/xedit.c		\
		src/commands.c		\
		src/util.c

OBJ	=	obj/xedit.o		\
		obj/commands.o		\
		obj/util.o

CC	=	cc
COPTS	=	-c -O -o $@

.virtual	lint cp cmp cmpq cmpqcp unld man modif clean rmobj

$PGM:		obj $OBJ
		cc -s -o $PGM $OBJ -ly.o -lx.o
		size $PGM

obj:		.exist
		mkdir obj

obj/xedit.o:	src/xedit.c			\
		h/xedit.h	
		$CC $COPTS src/xedit.c

obj/commands.o:	src/commands.c			\
		h/xedit.h	
		$CC $COPTS  src/commands.c

obj/util.o:	src/util.c			\
		h/xedit.h	
		$CC $COPTS  src/util.c

cp:		$CONF $PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		cp -f Xedit.ad.v $CONF/Xedit
		chmod a=r $CONF/Xedit
		ls -L $DIR/$PGM
		ls -L $CONF/Xedit

$DIR:		.exist
		mkdir -f $DIR

$CONF:		.exist
		mkdir -f $CONF

cmp:
		-cmpobj $PGM $DIR/$PGM
		-cmp Xedit.ad.v $CONF/Xedit

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
		-rm -f $PGM

lint:		$SRC
		cc -T $SRC

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
