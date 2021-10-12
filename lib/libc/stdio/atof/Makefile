
#*
#****************************************************************
#*	Makefile para a função "atof"				*
#****************************************************************
#*
DIR	= stdio
PGM	= atof
MAN	= libc
CC	= cc
LIBC	= /lib/libc

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		cc -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		
		$CC -T $PGM.c

t:		t.o $PGM.o
		cc -o t t.o $PGM.o

t.o:		t.c
		cc -c t.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:
		@echo Veja ../strtod

modif:
		@echo Veja ../strtod

prman:
		@echo Veja ../strtod

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
