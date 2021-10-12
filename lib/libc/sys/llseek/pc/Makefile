#*
#****************************************************************
#*	Makefile para a chamada ao sistema "llseek"		*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= sys
PGM	= llseek
MAN	= sys

.virtual	cp cmp man modif

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		../t.c $PGM.o
		$CC -o t ../t.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		
		@echo veja ../lseek

modif:
		@echo veja ../lseek
