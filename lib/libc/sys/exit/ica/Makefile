#*
#****************************************************************
#*	Makefile para a chamada ao sistema "exit"		*
#****************************************************************
#*
DIR	= sys
PGM	= exit
MAN	= sys

.virtual	all cp cmp man prman modif gar

all:		$PGM.o _$PGM.o

_$PGM.o:	_$PGM.s
		as -o _$PGM.o -l _$PGM.l _$PGM.s
		size _$PGM.o
		ls -l _$PGM.o
		nm _$PGM.o

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		../t.c $PGM.o
		cc -o t ../t.c $PGM.o

q:		../q.c _$PGM.o
		cc -o q ../q.c _$PGM.o

cp:		$PGM.o _$PGM.o
		mklib -crv $DIR/$PGM.o $DIR/_$PGM.o
		chmod -v u=rw,a+r		\
			/lib/libc/$DIR/$PGM.o	\
			/lib/libc/$DIR/_$PGM.o

cmp:
		-cmpobj $PGM.o _$PGM.o /lib/libc/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/_exit
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
