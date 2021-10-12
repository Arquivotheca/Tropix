#*
#****************************************************************
#*	Makefile para a função "modecolor"			*
#****************************************************************
#*
DIR	= etc
PGM	= modecolor
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	=

.virtual	lint tst cp cmp man modif prman

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		$PGM.c
		$CC $DEF -T $PGM.c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c
		$CC -c t.c

tst:		t
		t dir=1,reg=2,lnk=3,blk=4,chr=5,fifo=6
		t dir=9,reg=10,lnk=11,blk=12,chr=13,fifo=14

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man -l $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
