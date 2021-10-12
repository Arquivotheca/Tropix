#*
#****************************************************************
#*	Makefile para o programa "cc"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= cc
PROT	= u+m,a+x
MAN	= cmd
CC	= /bin/cc
DEF	=

.virtual	unld tst lint cp cmp cmpq cmpqcp man modif prman gar

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

unld:
		-rm -f $PGM

tst:
		cc $DEF -u -s -O -o n$PGM $PGM.c
		size $PGM n$PGM
		cmpobj $PGM n$PGM
		rm -f n$PGM

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod -v $PROT $DIR/$PGM
#		$DIR/$PGM -M

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

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
