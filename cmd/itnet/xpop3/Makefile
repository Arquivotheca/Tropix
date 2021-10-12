#*
#****************************************************************
#*	Makefile para o programa "xpop3"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= xpop3
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	= src/$PGM.c src/connect.c src/uidl.c src/mbox.c src/scan.c src/rcv.c src/del.c
OBJ	= obj/$PGM.o obj/connect.o obj/uidl.o obj/mbox.o obj/scan.o obj/rcv.o obj/del.o

.virtual	c lint lintx lintc lintu lintm lints lintr lintd
.virtual	cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ -lx.o
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c h/xpop3.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/connect.o:	src/connect.c h/xpop3.h
		$CC $DEF -c -O -o obj src/connect.c

obj/uidl.o:	src/uidl.c h/xpop3.h
		$CC $DEF -c -O -o obj src/uidl.c

obj/mbox.o:	src/mbox.c h/xpop3.h
		$CC $DEF -c -O -o obj src/mbox.c

obj/scan.o:	src/scan.c h/xpop3.h
		$CC $DEF -c -O -o obj src/scan.c

obj/rcv.o:	src/rcv.c h/xpop3.h
		$CC $DEF -c -O -o obj src/rcv.c

obj/del.o:	src/del.c h/xpop3.h
		$CC $DEF -c -O -o obj src/del.c

lint:
		$CC $DEF -T $SRC

lintx:
		$CC $DEF -T src/$PGM.c

lintc:
		$CC $DEF -T src/connect.c

lintu:
		$CC $DEF -T src/uidl.c

lintm:
		$CC $DEF -T src/mbox.c

lints:
		$CC $DEF -T src/scan.c

lintr:
		$CC $DEF -T src/rcv.c

lintd:
		$CC $DEF -T src/del.c

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
