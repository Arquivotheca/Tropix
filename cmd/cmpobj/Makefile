#*
#****************************************************************
#*	Makefile para o programa "cmpobj"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
LDIR	= /usr/bin
PGM	= cmpobj
PROT	= a+x
MAN	= cmd
CC	= cc

.virtual	lint cp cmp cmpq cmpqcp unld man modif zipto zipfrom

$PGM:		$PGM.c
		$CC -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:
		$CC -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-$LDIR/$PGM $PGM $DIR
		-./$PGM $PGM $DIR

cmpq:
		-$LDIR/$PGM -q $PGM $DIR

cmpqcp:
		-$LDIR/$PGM -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield
