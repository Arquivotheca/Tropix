#*
#****************************************************************
#*	Makefile para a função "lidcan"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= lid
PGM	= lidcan
MAN	= libc

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

t:		../t.c $PGM.o
		$CC -o t ../t.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lid
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lidclr
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lidcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lideq
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lidcpy
		-ln $mandir/$MAN/$PGM $mandir/$MAN/lidhash
		man $PGM

prman:		
		sroff ../man.r | lpr

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
