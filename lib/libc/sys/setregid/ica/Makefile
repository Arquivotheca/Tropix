#*
#****************************************************************
#*  Makefile para as chamadas ao sistema "setrgid" e "setegid"	*
#****************************************************************
#*
DIR	= sys
PGM1	= setrgid
PGM2	= setegid
MAN	= sys

.virtual	c cp cmp man modif gar

c:		$PGM1.o $PGM2.o

$PGM1.o:	$PGM1.s
		as -o $PGM1.o -l $PGM1.l $PGM1.s
		size $PGM1.o
		ls -l $PGM1.o
		nm $PGM1.o

$PGM2.o:	$PGM2.s
		as -o $PGM2.o -l $PGM2.l $PGM2.s
		size $PGM2.o
		ls -l $PGM2.o
		nm $PGM2.o

t:		../t.c $PGM1.o $PGM2.o
		cc -o t ../t.c $PGM1.o $PGM2.o

lint:
		cc -T ../t.c

cp:		$PGM1.o $PGM2.o
		mklib -crv $DIR/$PGM1.o $DIR/$PGM2.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM1.o /lib/libc/$DIR/$PGM2.o

cmp:
		-cmpobj $PGM1.o $PGM2.o /lib/libc/$DIR

man:
		@echo veja ../setuid

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM1
		modif $PGM1

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
