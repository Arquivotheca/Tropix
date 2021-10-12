#*
#****************************************************************
#*	Makefile para o programa "tic"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= tic
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=
SRC	= src/tic.c src/scan.c src/error.c
OBJ	= obj/tic.o obj/scan.o obj/error.o

.virtual	lint tst cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/tic.o:	src/tic.c h/error.h
		$CC $DEF -c -O -o obj src/tic.c

obj/scan.o:	src/scan.c h/scan.h h/error.h
		$CC $DEF -c -O -o obj src/scan.c

obj/error.o:	src/error.c h/error.h
		$CC $DEF -c -O -o obj src/error.c

lint:
		$CC $DEF -T $SRC

tst:
		$PGM terminfo/vt100.v
		-cmp /usr/lib/terminfo/test /usr/lib/terminfo/vt100
		rm -f /usr/lib/terminfo/test

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

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
