#*
#****************************************************************
#*	Makefile para a função "memcpy"				*
#****************************************************************
#*
DIR	= mem
PGM	= memcpy
MAN	= libc

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		t.o $PGM.o
		cc -o t t.o $PGM.o

t.o:		../t.c
		cc -c ../t.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memmove
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memccpy
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memeq
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memtcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memttcmp
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memchr
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memset
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memsetw
		-ln $mandir/$MAN/$PGM $mandir/$MAN/memsetl
		-ln $mandir/$MAN/$PGM $mandir/$MAN/mem
		man $PGM

prman:
		sroff ../man.r | lpr

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/mem
		modif mem

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
