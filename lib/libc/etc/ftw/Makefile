#*
#****************************************************************
#*	Makefile para a função "ftw"				*
#****************************************************************
#*
DIR	= etc
PGM	= ftw
MAN	= libc
LIBC	= /lib/libc
CC	= cc
DEF	= -D DIR_ENDIAN_CV

.virtual	lint cp cmp man modif prman gar gara

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		$CC $DEF -T $PGM.c

t:		t.o $PGM.o
		$CC -u -o t t.o $PGM.o

sh.t:		$PGM.o
		$CC -u -o sh.t t.o

t.o:		t.c
		$CC -c -O t.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR
		-cmp -l ftw.h $inc

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

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
