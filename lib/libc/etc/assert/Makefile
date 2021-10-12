#*
#****************************************************************
#*	Makefile para a função "assert"				*
#****************************************************************
#*
DIR	= etc
PGM	= assert
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

t:		t.c $PGM.o
		$CC -o t t.c $PGM.o

Nt:		t.c $PGM.o
		$CC -D NDEBUG -o Nt t.c $PGM.o

t.i:		t.c
		$CC -P t.c; show t.i
		rm t.i

Nt.i:		t.c
		$CC -D NDEBUG -P -o Nt.i t.c; show Nt.i
		rm Nt.i

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmp $PGM.h $inc
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
