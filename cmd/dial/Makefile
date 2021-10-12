#*
#****************************************************************
#*	Makefile para o programa "phone"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= dial
PROT	= a+x
MAN	= cmd
CC	= cc

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		$CC -Osu -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:		
		$CC -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -l $DIR/$PGM

cmp:		$PGM
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
