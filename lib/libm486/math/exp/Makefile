#*
#****************************************************************
#*	Makefile para a função "exp"				*
#****************************************************************
#*
PGM	= exp
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

tt:		tt.c $PGM.o
		cc -o tt tt.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIB $DIR/$PGM.o
		chmod -v $PROT $LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIB/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/exp2
		-ln $mandir/$MAN/$PGM $mandir/$MAN/exp10
		-ln $mandir/$MAN/$PGM $mandir/$MAN/expb
		-ln $mandir/$MAN/$PGM $mandir/$MAN/pow
		man $PGM

prman:
		sroff man.r | lpr

		@echo Veja $libm/math/$PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs8 $PGM
		cd ..; gar -icv8
