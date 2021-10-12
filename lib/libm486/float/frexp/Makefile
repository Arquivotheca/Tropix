#*
#****************************************************************
#*	Makefile para a função "frexp"				*
#****************************************************************
#*
LIB	= libm
DIR	= float
PGM	= frexp
MAN	= libc

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

t:		t.c $PGM.o
		cc -o t t.c $PGM.o

cp:		$PGM.o
		mklib -rv /lib/$LIB $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/$LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/$LIB/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/ldexp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/modf
		man $PGM

prman:		
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs9 $PGM
		cd ..; gar -icv9
