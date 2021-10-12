#*
#****************************************************************
#*	Makefile para a função "getmntent"			*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= etc
PGM	= getmntent
MAN	= libc

.virtual	lint cp cmp man prman modif
.virtual	pento penfrom pensave

$PGM.o:		$PGM.c
		$CC -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		
		$CC -T $PGM.c

t:		t.o $PGM.o
		$CC -o t t.o $PGM.o

t.o:		t.c fstab
		$CC -O -c t.c

fstab:		.exist
		ln fstab.v fstab

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/mntent
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/getmntopt
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/defmntent
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/editmntopt
		man $PGM

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

