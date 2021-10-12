#*
#****************************************************************
#*	Makefile para a função "patmatch"			*
#****************************************************************
#*
DIR	= etc
PGM	= patmatch
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	=

.virtual	lint cp cmp man modif prman gar

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

l:		l.o $PGM.o
		$CC -o l l.o $PGM.o

n.o:		n.c
		$CC -c n.c

n:		n.o $PGM.o
		$CC -o n n.o $PGM.o

l.o:		l.c
		$CC -c l.c

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

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
