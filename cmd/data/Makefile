#*
#****************************************************************
#*	Makefile para o programa "data"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= data
PGM1	= date
PGM2	= datagmt
PROT	= a+x
MAN	= cmd
CC	= cc

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		$CC -u -s -O -o $PGM $PGM.c
		ln -f $PGM $PGM1
		ln -f $PGM $PGM2
		size $PGM
		ls -l $PGM

lint:
		$CC -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ln -f $DIR/$PGM $DIR/$PGM1
		ln -f $DIR/$PGM $DIR/$PGM2
		ls -L $DIR/$PGM $DIR/$PGM1 $DIR/$PGM2

cmp:
		-cmpobj $PGM $DIR/$PGM
		-cmpobj $DIR/$PGM $DIR/$PGM1
		-cmpobj $DIR/$PGM $DIR/$PGM2

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM1
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
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
