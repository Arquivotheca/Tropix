#*
#****************************************************************
#*	Makefile para a função "strtok"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= str
PGM	= strtok
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
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strchr
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strrchr
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strend
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strpbrk
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strspn
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strcspn
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strfield
		-ln $mandir/$MAN/$PGM $mandir/$MAN/strstr
		man $PGM

prman:
		sroff ../man.r | lpr

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
