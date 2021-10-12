#*
#****************************************************************
#*	Makefile para o programa "xview"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= xview
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	= src/$PGM.c	\
	  src/gif.c	\
	  src/jpeg.c	\
	  src/image.c	\
	  src/load.c	\
	  src/msg.c

OBJ	= obj/$PGM.o	\
	  obj/gif.o	\
	  obj/jpeg.o	\
	  obj/image.o	\
	  obj/load.o	\
	  obj/msg.o

.virtual	c lint
.virtual	lintx linti lintg lintm lintl
.virtual	zipto zipfrom
.virtual	pento penfrom pensave
.virtual	cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC -u -o $PGM $OBJ -lx.o -ly.o
#		$CC -u -s -o $PGM $OBJ -lx.o -ly.o
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c h/xview.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/gif.o:	src/gif.c h/xview.h
		$CC $DEF -c -O -o obj src/gif.c

obj/jpeg.o:	src/jpeg.c h/xview.h
		$CC $DEF -c -O -o obj src/jpeg.c

obj/load.o:	src/load.c h/xview.h
		$CC $DEF -c -O -o obj src/load.c

obj/image.o:	src/image.c h/xview.h
		$CC $DEF -c -O -o obj src/image.c

obj/msg.o:	src/msg.c h/xview.h
		$CC $DEF -c -O -o obj src/msg.c

lint:
		$CC $DEF -T $SRC

lintx:
		$CC $DEF -T src/$PGM.c

linti:
		$CC $DEF -T src/image.c

lintg:
		$CC $DEF -T src/gif.c

lintj:
		$CC $DEF -T src/jpeg.c

lintm:
		$CC $DEF -T src/msg.c

lintl:
		$CC $DEF -T src/load.c

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


pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen
