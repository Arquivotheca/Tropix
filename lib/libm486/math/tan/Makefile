#*
#****************************************************************
#*	Makefile para a função "tan"				*
#****************************************************************
#*
PGM	= tan
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

man:
		@echo Veja $libm/math/$PGM

prman:
		@echo Veja $libm/math/$PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs8 $PGM
		cd ..; gar -icv8
