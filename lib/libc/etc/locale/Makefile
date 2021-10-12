#*
#****************************************************************
#*	Makefile para a função "locale"				*
#****************************************************************
#*
DIR	= etc
LDIR	= /etc/locale
PGM	= locale
PGM2	= lc_vector
PGM3	= lc_lconv
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	=

.virtual	all lint cp cmp man modif prman gar

all:		$PGM.o $PGM2.o $PGM3.o

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

$PGM2.o:	$PGM2.c
		$CC $DEF -O -c $PGM2.c
		size $PGM2.o
		ls -l $PGM2.o
		nm $PGM2.o | c

$PGM3.o:	$PGM3.c
		$CC $DEF -O -c $PGM3.c
		size $PGM3.o
		ls -l $PGM3.o
		nm $PGM3.o | c

lint:		$PGM.c $PGM2.c $PGM3.c
		$CC $DEF -T $PGM.c $PGM2.c $PGM3.c

t:		t.o $PGM.o $PGM2.o $PGM3.o
		$CC -o t t.o $PGM.o $PGM2.o $PGM3.o

t.o:		t.c
		$CC -c t.c

cp:		$PGM.o $LDIR
		mklib -crv $DIR/$PGM.o $DIR/$PGM2.o $DIR/$PGM3.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o /lib/libc/$DIR/$PGM2.o /lib/libc/$DIR/$PGM3.o
		-cmp time_P.v $LDIR/time_P || cp time_P.v $LDIR/time_P
		-cmp time_E.v $LDIR/time_E || cp time_E.v $LDIR/time_E
		chmod -v $PROT $LDIR/time_P $LDIR/time_E

cmp:
		-cmp time_P.v $LDIR/time_P
		-cmp time_E.v $LDIR/time_E
		-cmp $PGM.h /usr/include/$PGM.h
		-cmpobj $PGM.o $PGM2.o $PGM3.o /lib/libc/$DIR

$LDIR:		.exist
		mkdir $LDIR
		chmod 0755 $LDIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/setlocale
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/localeconv
		man -l $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
