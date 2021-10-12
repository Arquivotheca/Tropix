#*
#****************************************************************
#*	Makefile para a função "strpbrk"			*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= str
PGM	= strpbrk
MAN	= libc

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		../t.c
		$CC -c ../t.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:
		@echo Veja ../strtok

prman:
		@echo Veja ../strtok

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
