#*
#****************************************************************
#*	Makefile para a função "refresh"			*
#****************************************************************
#*
PGM	= refresh
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

cp:		$PGM.o
		mklib -rv $LIB $DIR/$PGM.o
		chmod -v u=rw,a+r $LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIB/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/w$PGM
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

t:		t1	t2	t3	t4

t1:		t1.o	$PGM.o
		$CC -o t1 $PGM.o t1.o -lcurses
		TERM=test; t1

t1.o:		../t1.c
		$CC -c ../t1.c

t2:		t2.o	$PGM.o
		$CC -o t2 $PGM.o t2.o -lcurses
		TERM=test; t2

t2.o:		../t2.c
		$CC -c ../t2.c

t3:		t3.o	$PGM.o
		$CC -o t3 $PGM.o t3.o -lcurses
		TERM=test; t3

t3.o:		../t3.c
		$CC -c ../t3.c

t4:		t4.o	$PGM.o
		$CC -o t4 $PGM.o t4.o -lcurses
		TERM=test; t4

t4.o:		../t4.c
		$CC -c ../t4.c

ex:		cp
		TERM=test; cd $cmd/ex; $CC -o ex obj/*.o; ex ../com/com.c
