#*
#****************************************************************
#*	Makefile para a função "fixdfsi"			*
#****************************************************************
#*
LIBC	= /lib/libm
DIR	= float
PGM	= fixdfsi
MAN	= libc

.virtual	cp cmp man prman modif

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		t.c $PGM.o
		cc -o t t.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff man.r | lpr
