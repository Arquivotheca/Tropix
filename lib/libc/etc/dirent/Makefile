#*
#****************************************************************
#*	Makefile para a função "dirent"				*
#****************************************************************
#*
DIR	= etc
PGM	= dirent
MAN	= libc
LIBC	= /lib/libc
CC	= cc
DEF	= -D DIR_ENDIAN_CV

.virtual	lint cp cmp man modif prman gar gara

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		$CC $DEF -T $PGM.c

t:		t.o $PGM.o
		$CC -u -o t t.o $PGM.o
#		$CC -u -o t t.o $PGM.o $libc/sys/getdirent/pc/getdirent.o

t.o:		t.c
		$CC -c -O t.c

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/opendir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/inopendir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/readdir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/telldir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/seekdir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/rewinddir
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/closedir
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
