#*
#****************************************************************
#*	Makefile para a chamada ao sistema "signal"		*
#****************************************************************
#*
DIR	= sys
PGM	= signal
MAN	= sys

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

p:		../p.c $PGM.o
		cc -o p ../p.c $PGM.o

q:		../q.c $PGM.o
		cc -o q ../q.c $PGM.o

r:		../r.c $PGM.o
		cc -o r ../r.c $PGM.o

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v "u=rw,a+r" /lib/libc/$DIR/$PGM.o

cmp:
		-cmp -l ../signal.h /usr/include
		-cmpobj $PGM.o /lib/libc/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
