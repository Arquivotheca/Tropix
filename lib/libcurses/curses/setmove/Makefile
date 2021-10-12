#*
#****************************************************************
#*	Makefile para a função "setmove"			*
#****************************************************************
#*
PGM	= setmove
DIR	= curses
MAN	= curses
LIB	= /lib/libcurses
CC	= cc
DEF	=

.virtual	lint cp cmp man modif prman gar gara

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		$CC $DEF -T $PGM.c

t:		t.o
		$CC -o t t.o -lcurses
		t

t.o:		t.c
		$CC -c -O t.c

cp:		$PGM.o
		mklib -rv $LIB $DIR/$PGM.o
		chmod -v u=rw,a+r $LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIB/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif -w $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
