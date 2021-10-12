#*
#****************************************************************
#*	Makefile para a função "grent"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= pwd
PGM	= grent
MAN	= libc

.virtual	lint cp cmp man prman modif gar

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
		-cmp -l grp.h $inc 
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getgrent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/setgrent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/endgrent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getgrgid
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getgrnam
		-ln $mandir/$MAN/$PGM $mandir/$MAN/grcache
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
