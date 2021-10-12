#*
#****************************************************************
#*	Makefile para o programa "ping"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= ping
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	= -D PC -D LITTLE_ENDIAN
CHECKSUM = checksum.486

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.o checksum.o
		$CC $DEF -u -s -o $PGM $PGM.o checksum.o
		size $PGM
		ls -l $PGM

$PGM.o:		$PGM.c
		$CC $DEF -c -O $PGM.c

checksum.o:	$CHECKSUM.s
		as -o checksum.o $CHECKSUM.s

lint:		$PGM.c
		$CC $DEF -T $PGM.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
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
		chmod a+r $mandir/$MAN/$PGM
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
