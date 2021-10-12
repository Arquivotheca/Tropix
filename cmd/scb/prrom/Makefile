#*
#****************************************************************
#*	Makefile para o programa "prrom"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/etc
PGM	= prrom
PROT	= a+x
MAN	= cmd

$PGM:		$PGM.c
		cc $DEF -f -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:		$PGM.c
		cc $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

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
