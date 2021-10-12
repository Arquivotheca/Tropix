#*
#****************************************************************
#*	Makefile para o programa "xmandel"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= xmandel
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	= src/$PGM.c	\
	  src/compute.c	\
	  src/gif.c	\
	  src/msg.c

OBJ	= obj/$PGM.o	\
	  obj/compute.o	\
	  obj/gif.o	\
	  obj/msg.o

.virtual	c lint
.virtual	lintx lintc lintg lintm
.virtual	zipto zipfrom
.virtual	cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC -u -o $PGM $OBJ -lx.o
#		$CC -u -s -o $PGM $OBJ -lx.o
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c h/xmandel.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/compute.o:	src/compute.c h/xmandel.h
		$CC $DEF -c -O -o obj src/compute.c

obj/gif.o:	src/gif.c h/xmandel.h
		$CC $DEF -c -O -o obj src/gif.c

obj/msg.o:	src/msg.c h/xmandel.h
		$CC $DEF -c -O -o obj src/msg.c

lint:
		$CC $DEF -T $SRC

lintx:
		$CC $DEF -T src/$PGM.c

lintc:
		$CC $DEF -T src/compute.c

lintg:
		$CC $DEF -T src/gif.c

lintm:
		$CC $DEF -T src/msg.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield
