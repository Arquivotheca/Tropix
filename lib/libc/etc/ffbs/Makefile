#*
#****************************************************************
#*	Makefile para a função "ffbs"				*
#****************************************************************
#*
DIR	= etc
PGM	= ffbs
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	=

.virtual	cp cmp man modif prman gar

$PGM.o:		$PGM.s
		as -l $PGM.l -o $PGM.o $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c
		$CC -c t.c

r:		r.o $PGM.o
		$CC -o r r.o $PGM.o

r.o:		r.c
		$CC -c r.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
