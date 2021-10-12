#*
#****************************************************************
#*	Makefile para a chamada ao sistema "sema"		*
#****************************************************************
#*
DIR	= sys
PGM	= sema
MAN	= sys

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

a:		a.c $PGM.o
		cc -o a a.c $PGM.o

b:		b.c $PGM.o
		cc -o b b.c $PGM.o

c:		c.c $PGM.o
		cc -o c c.c $PGM.o

d:		d.c $PGM.o
		cc -o d d.c $PGM.o

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/semactl
		-ln $mandir/$MAN/$PGM $mandir/$MAN/semalock
		-ln $mandir/$MAN/$PGM $mandir/$MAN/semafree
		-ln $mandir/$MAN/$PGM $mandir/$MAN/sematestl
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
