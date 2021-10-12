#*
#****************************************************************
#*	Makefile para a função "crc16s"				*
#****************************************************************
#*
DIR	= etc
PGM1	= crc16s
PGM2	= crc1641s
MAN	= libc
CC	= cc
DEF	=

.virtual	all lint cp cmp man prman modif

all:		$PGM1.o $PGM2.o

lint:
		$CC $DEF -T $PGM1.c $PGM2.c mkcrcstb.c 

$PGM1.o:	.$PGM1.s .crcstb.s
		as -o $PGM1.o .$PGM1.s .crcstb.s
		size $PGM1.o
		ls -l $PGM1.o
		nm $PGM1.o | c

$PGM2.o:	.$PGM2.s .crcstb.s
		as -o $PGM2.o .$PGM2.s .crcstb.s
		size $PGM2.o
		ls -l $PGM2.o
		nm $PGM2.o | c

.$PGM1.s:	$PGM1.c
		$CC $DEF -S -O $PGM1.c
		mv -f $PGM1.s .$PGM1.s

.$PGM2.s:	$PGM2.c
		$CC $DEF -S -O $PGM2.c
		mv -f $PGM2.s .$PGM2.s

.crcstb.s:	mkcrcstb.c
		$CC $DEF -o mkcrcstb mkcrcstb.c 
		mkcrcstb
		rm -f mkcrcstb

t:		t.c $PGM1.o $PGM2.o
		$CC -o t t.c $PGM1.o $PGM2.o

q:		q.c $PGM1.o $PGM2.o
		$CC -o q q.c $PGM1.o $PGM2.o

cp:		$PGM1.o $PGM2.o
		mklib -crv $DIR/$PGM1.o $DIR/$PGM2.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM1.o /lib/libc/$DIR/$PGM2.o

cmp:
		-cmpobj $PGM1.o $PGM2.o /lib/libc/$DIR

man:
		@echo Veja ../crc16

prman:
		@echo Veja ../crc16

modif:
		@echo Veja ../crc16
