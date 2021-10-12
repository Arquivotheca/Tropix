#*
#****************************************************************
#*	Makefile para a função "crc16"				*
#****************************************************************
#*
DIR	= etc
PGM1	= crc16
PGM2	= crc1641
PROT	= u=rw,a+r
MAN	= libc
LIBC	= /lib/libc
CC	= cc
DEF	=

.virtual	all lint cp cmp man modif prman gar

all:		$PGM1.o $PGM2.o

lint:
		cc -T $PGM1.c $PGM2.c mkcrctb.c t.c

$PGM1.o:	.$PGM1.s .crctb.s
		as -o $PGM1.o .$PGM1.s .crctb.s
		size $PGM1.o
		ls -l $PGM1.o
		nm $PGM1.o | c

$PGM2.o:	.$PGM2.s .crctb.s
		as -o $PGM2.o .$PGM2.s .crctb.s
		size $PGM2.o
		ls -l $PGM2.o
		nm $PGM2.o | c

.$PGM1.s:	$PGM1.c
		$CC $DEF -S -O $PGM1.c
		mv -f $PGM1.s .$PGM1.s

.$PGM2.s:	$PGM2.c
		$CC $DEF -S -O $PGM2.c
		mv -f $PGM2.s .$PGM2.s

.crctb.s:	mkcrctb.c
		cc $DEF -o mkcrctb mkcrctb.c 
		mkcrctb
		rm -f mkcrctb

t:		t.c $PGM1.o $PGM2.o
		cc -o t t.c $PGM1.o $PGM2.o

cp:		$PGM1.o $PGM2.o
		mklib -rv $LIBC $DIR/$PGM1.o $DIR/$PGM2.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM1.o $LIBC/$DIR/$PGM2.o

cmp:
		-cmpobj $PGM1.o $PGM2.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM1
		ln  $mandir/$MAN/$PGM1 $mandir/$MAN/$(PGM1)s
		ln  $mandir/$MAN/$PGM1 $mandir/$MAN/$(PGM2)
		ln  $mandir/$MAN/$PGM1 $mandir/$MAN/$(PGM2)s
		ln  $mandir/$MAN/$PGM1 $mandir/$MAN/crc
		man $PGM1

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM1
		modif $PGM1

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

