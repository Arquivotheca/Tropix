#*
#****************************************************************
#*	Makefile para o programa "xdefrag"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= xdefrag
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	= src/$PGM.c src/devtb.c src/examine.c src/method_file.c src/method_ftw.c src/aux.c
OBJ	= obj/$PGM.o obj/devtb.o obj/examine.o obj/method_file.o obj/method_ftw.o obj/aux.o

.virtual	c lint lintx lintd linte lintm lintf linta zipto zipfrom
.virtual	cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC -u -o $PGM $OBJ -lx.o
#		$CC -u -s -o $PGM $OBJ -lx.o
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/devtb.o:	src/devtb.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/devtb.c

obj/examine.o:	src/examine.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/examine.c

obj/method_file.o: src/method_file.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/method_file.c

obj/method_ftw.o: src/method_ftw.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/method_ftw.c

obj/aux.o: src/aux.c h/xdefrag.h
		$CC $DEF -c -O -o obj src/aux.c

lint:
		$CC $DEF -T $SRC

lintx:
		$CC $DEF -T src/$PGM.c

lintd:
		$CC $DEF -T src/devtb.c

linte:
		$CC $DEF -T src/examine.c

lintm:
		$CC $DEF -T src/method_file.c

lintf:
		$CC $DEF -T src/method_ftw.c

linta:
		$CC $DEF -T src/aux.c

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
