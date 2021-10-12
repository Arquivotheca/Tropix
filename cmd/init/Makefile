#*
#****************************************************************
#*	Makefile para o programa "init"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= init
PROT	= u=rwx
MAN	= cmd
CC	= cc

.virtual	unld lint cp cmp cmpq cmpqcp man modif prman pento penfrom pensave

$PGM:		$PGM.c
		$CC -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

unld:
		-rm -f $PGM

lint:
		$CC -T $PGM.c

cp:		$PGM
		-mv -f $DIR/$PGM $DIR/old.$PGM
		cp -f $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/*$PGM

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen

