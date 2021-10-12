#*
#****************************************************************
#*	Makefile para a chamada ao sistema "event"		*
#****************************************************************
#*
LIBC	= /lib/libc
DIR	= sys
PGM	= event
PGM1	= eventctl
PGM2	= eventdone
PGM3	= eventtest
PGM4	= eventwait
MAN	= sys

.virtual	all cp cmp man prman modif gar

all:		$PGM1.o	$PGM2.o	$PGM3.o	$PGM4.o

$PGM1.o:	$PGM1.s
		as -o $PGM1.o -l $PGM1.l $PGM1.s
		size $PGM1.o
		ls -l $PGM1.o
		nm $PGM1.o

$PGM2.o:	$PGM2.s
		as -o $PGM2.o -l $PGM2.l $PGM2.s
		size $PGM2.o
		ls -l $PGM2.o
		nm $PGM2.o

$PGM3.o:	$PGM3.s
		as -o $PGM3.o -l $PGM3.l $PGM3.s
		size $PGM3.o
		ls -l $PGM3.o
		nm $PGM3.o

$PGM4.o:	$PGM4.s
		as -o $PGM4.o -l $PGM4.l $PGM4.s
		size $PGM4.o
		ls -l $PGM4.o
		nm $PGM4.o

t:		../t.c $PGM1.o $PGM2.o $PGM3.o $PGM4.o
		cc -o t ../t.c $PGM1.o $PGM2.o $PGM3.o $PGM4.o

cp:		$PGM1.o
		mklib -rv $LIBC					\
			$DIR/$PGM1.o $DIR/$PGM2.o		\
			$DIR/$PGM3.o $DIR/$PGM4.o
		chmod -v u=rw,a+r				\
			$LIBC/$DIR/$PGM1.o $LIBC/$DIR/$PGM2.o	\
			$LIBC/$DIR/$PGM3.o $LIBC/$DIR/$PGM4.o

cmp:
		-cmpobj $PGM1.o $PGM2.o $PGM3.o $PGM4.o $LIBC/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM1
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM3
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM4
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
