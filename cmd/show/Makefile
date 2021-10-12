#*
#****************************************************************
#*	Makefile para o programa "show"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= show
PGM1	= man
PGM2	= modif
PROT	= a+x,u+m
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld manual modif

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		ln -f $PGM $PGM1
		ln -f $PGM $PGM2
		size $PGM
		ls -l $PGM

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		ln -f $DIR/$PGM $DIR/$PGM1
		ln -f $DIR/$PGM $DIR/$PGM2
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $PGM1 $PGM2 $DIR

cmpq:
		-cmpobj -q $PGM $PGM1 $PGM2 $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

manual:		$PGM.man.r $PGM1.man.r $PGM2.man.r
		sroff $PGM.man.r >$mandir/$MAN/$PGM
		sroff $PGM1.man.r >$mandir/$MAN/$PGM1
		sroff $PGM2.man.r >$mandir/$MAN/$PGM2
		chmod a+r $mandir/$MAN/$PGM $mandir/$MAN/$PGM1 $mandir/$MAN/$PGM2
		man $PGM
		man $PGM1
		man $PGM2

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
