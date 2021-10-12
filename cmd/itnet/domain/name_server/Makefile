#*
#****************************************************************
#*	Makefile para o programa "name_s"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= name_s
PROT	= a+x,u+m
MAN	= cmd
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:		$PGM.c
		$CC $DEF -T $PGM.c

cp:		$PGM
		-mv $DIR/$PGM $DIR/old.$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		rm $DIR/old.$PGM
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
		cd ..; gar -avs0 $PGM/$PGM.c
		cd ..; gar -icv0
