#*
#****************************************************************
#*	Makefile para o programa "echomon_pr"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= echomon_pr
PROT	= a+x
MAN	= cmd
CC	= cc

$PGM:		$PGM.c
		$CC $DEF -u -s -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:		$PGM.c
		$CC $DEF -T $PGM.c

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
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
