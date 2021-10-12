#*
#****************************************************************
#*	Makefile para o programa "shutdown"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= shutdown
APGM	= reboot
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		ln -f $PGM $APGM
		size $PGM $APGM
		ls -l $PGM $APGM

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $APGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$APGM

cmp:
		-cmpobj $PGM $APGM $DIR

cmpq:
		-cmpobj -q $PGM $APGM $DIR

cmpqcp:
		-cmpobj -q $PGM $APGM $DIR || make cp

unld:
		-rm -f $PGM $APGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$APGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr
