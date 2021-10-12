#*
#****************************************************************
#*	Makefile para o programa "chknet"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= chknet
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif
.virtual	pento penfrom pensave

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
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/$PGM
		-forldiff /pen$itnet/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/$PGM &&		\
		cmptree -sg . $itnet/$PGM;	\
		forldiff $itnet/$PGM;		\
		rm -f /pen$itnet/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/$PGM
		-umount /pen

