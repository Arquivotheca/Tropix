#*
#****************************************************************
#*	Makefile para o programa "fortune"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= fortune
PROT	= a+x
MAN	= cmd
DATADIR = /usr/pub/lib

.virtual	lint unld cp cmp cmpq cmpqcp cpdata cmpdata cmpqcpdata man modif prman gar

$PGM:		$PGM.c
		cc -D DATADIR=$DATADIR -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

unld:
		rm -f $PGM

lint:		$PGM.c
		cc -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -l $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

gfd:		gfd.c
		cc -s -o gfd gfd.c
		size gfd
		ls -l gfd

ffc:		ffc.c
		cc -s -o ffc ffc.c
		size ffc
		ls -l ffc

fortune.data:	gfd set.r
		gfd set.r fortune.data

cpdata:		fortune.data $DATADIR
		cp fortune.data $DATADIR/fortune.data

cmpdata:
		-cmp -l fortune.data $DATADIR/fortune.data

cmpqcpdata:
		-cmp -l fortune.data $DATADIR/fortune.data || make cpdata

$DATADIR:	.exist
		mkdir -f $DATADIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 -p "*.data" $PGM
		cd ..; gar -icv0
