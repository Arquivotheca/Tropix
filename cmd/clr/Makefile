#*
#****************************************************************
#*	Makefile para o programa "clr"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= clr
PGM2	= cls
PGM3	= clear
PROT	= a+x
MAN	= cmd
CC	= cc

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		$CC -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:
		$CC -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		ln -f $DIR/$PGM $DIR/$PGM2
		ln -f $DIR/$PGM $DIR/$PGM3
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$PGM2 $DIR/$PGM3

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
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM3
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
