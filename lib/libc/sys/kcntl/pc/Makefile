#*
#****************************************************************
#*	Makefile para a chamada ao sistema "kcntl"		*
#****************************************************************
#*
LIBC	= /lib/libc
DIR	= sys
PGM	= kcntl
MAN	= sys

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

a:		../a.c $PGM.o
		cc -o a ../a.c $PGM.o

b:		../b.c $PGM.o
		cc -o b ../b.c $PGM.o

c:		../c.c $PGM.o
		cc -o c ../c.c $PGM.o

d:		../d.c $PGM.o
		cc -o d ../d.c $PGM.o

g:		../g.c $PGM.o
		cc -o g ../g.c $PGM.o

z:		../z.c $PGM.o
		cc -o z ../z.c $PGM.o

preemption:	../preemption.c $PGM.o
		cc -o preemption ../preemption.c $PGM.o

j:		../j.c $PGM.o
		cc -o j ../j.c $PGM.o

k:		../k.c $PGM.o
		cc -o k ../k.c $PGM.o

mmulimit:	../mmulimit.c $PGM.o
		cc -O -o mmulimit ../mmulimit.c $PGM.o

debug:		../debug.c $PGM.o
		cc -O -o debug ../debug.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

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
