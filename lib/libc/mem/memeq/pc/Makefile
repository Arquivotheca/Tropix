#*
#****************************************************************
#*	Makefile para a função "memeq"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= mem
PGM	= memeq
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
		@echo Veja ../memcpy

prman:
		@echo Veja ../memcpy

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/mem
		modif mem

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
