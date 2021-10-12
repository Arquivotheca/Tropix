#*
#****************************************************************
#*	Makefile para a função "vfprintf"			*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= stdio
PGM	= vfprintf
VA	= va_list
PROT	= u=rw,a+r
MAN	= libc

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		$CC -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		
		$CC -T $PGM.c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c
		$CC -c t.c

t2:		t2.o $PGM.o
		$CC -o t2 t2.o $PGM.o

t2.o:		t2.c
		$CC -c t2.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$VA
		-ln $mandir/$MAN/$VA $mandir/$MAN/va_start
		-ln $mandir/$MAN/$VA $mandir/$MAN/va_arg
		-ln $mandir/$MAN/$VA $mandir/$MAN/va_end
		-ln $mandir/$MAN/$VA $mandir/$MAN/va_first
		man $VA

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
