#*
#****************************************************************
#*	Makefile para o programa "inomap"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/etc
PGM	= inomap
PROT	= a+x
MAN	= cmd

$PGM:		$PGM.c
		cc -f -s -O -o $PGM $PGM.c -lcurses
		size $PGM
		ls -l $PGM

lint:		$PGM.c
		cc -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -l $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs8 $PGM
		cd ..; gar -icv8
