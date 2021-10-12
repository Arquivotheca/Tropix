#*
#****************************************************************
#*	Makefile para o programa "sbvol"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= sbvol
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		ln -f $PGM $PGM+
		ln -f $PGM $PGM-
		size $PGM
		ls -l $PGM

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		ln -f $DIR/$PGM $DIR/$PGM+
		ln -f $DIR/$PGM $DIR/$PGM-
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR
		-cmpobj $DIR/$PGM $DIR/$PGM+
		-cmpobj $DIR/$PGM $DIR/$PGM-

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
