#*
#****************************************************************
#*	Makefile para a função "strcmp"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= str
PGM	= strcmp
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

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/streq
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strrcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/stricmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strircmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strncmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strtcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strntcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strttcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strnttcmp
		man $PGM

prman:
		sroff ../man.r | lpr

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
