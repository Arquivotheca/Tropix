#*
#****************************************************************
#*	Makefile para a função "getcwd"				*
#****************************************************************
#*
DIR	= etc
PGM	= getcwd
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	lint cp cmp man modif prman
.virtual	pento penfrom pensave

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		$PGM.c
		$CC $DEF -T $PGM.c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c
		$CC -c t.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man -l $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

pento:
		-mount /pen
		-cmptree -sg . /pen$libc/$DIR/$PGM
		-forldiff /pen$libc/$DIR/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$libc/$DIR/$PGM &&		\
		cmptree -sg . $libc/$DIR/$PGM;		\
		forldiff $libc/$DIR/$PGM;		\
		rm -f /pen$libc/$DIR/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $libc/$DIR/$PGM
		-umount /pen
