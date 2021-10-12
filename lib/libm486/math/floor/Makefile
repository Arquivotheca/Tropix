#*
#****************************************************************
#*	Makefile para a função "floor"				*
#****************************************************************
#*
PGM	= floor
LIB	= /lib/libm
DIR	= math
PROT	= u=rw,a+r
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
		mklib -rv $LIB $DIR/$PGM.o
		chmod -v $PROT $LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIB/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/ceil
		-ln $mandir/$MAN/$PGM $mandir/$MAN/fmod
		-ln $mandir/$MAN/$PGM $mandir/$MAN/fabs
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs8 $PGM
		cd ..; gar -icv8
