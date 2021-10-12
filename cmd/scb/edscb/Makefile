#*
#****************************************************************
#*	Makefile para o programa "edscb"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= edscb
PROT	= a+x
MAN	= cmd
DEF	=
CC	= cc

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman pento penfrom pensave

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM || make cp

unld:
		-rm -f $PGM

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
		-cmptree -sg . /pen$cmd/scb/$PGM
		-forldiff /pen$cmd/scb/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/scb/$PGM &&		\
		cmptree -sg . $cmd/scb/$PGM;	\
		forldiff $cmd/scb/$PGM;		\
		rm -f /pen$cmd/scb/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/scb/$PGM
		-umount /pen

