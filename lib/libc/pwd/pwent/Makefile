#*
#****************************************************************
#*	Makefile para a função "pwent"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= pwd
PGM	= pwent
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
		-cmp -l pwd.h $inc
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getpwent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/setpwent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/endpwent
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getpwuid
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getpwnam
		-ln $mandir/$MAN/$PGM $mandir/$MAN/pwcache
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getpwpos
		-ln $mandir/$MAN/$PGM $mandir/$MAN/getpwslot
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
