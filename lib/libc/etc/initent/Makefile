#*
#****************************************************************
#*	Makefile para a função "initent"			*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= etc
PGM	= initent
MAN	= libc

.virtual	lint cp cmp man prman modif

$PGM.o:		$PGM.c
		$CC -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		
		$CC -T $PGM.c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c
		$CC -c t.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getinitent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/setinitent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/endinitent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getinitnam
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
