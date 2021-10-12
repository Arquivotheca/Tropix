#*
#****************************************************************
#*	Makefile para o programa "echomon_gen"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= echomon_gen
PROT	= a+x
MAN	= cmd
CC	= cc
CHECKSUM_SRC = ../../ping/checksum.486.s
CHECKSUM_OBJ = ../../ping/checksum.o

$PGM:		$PGM.o $CHECKSUM_OBJ
		$CC $DEF -u -s -o $PGM $PGM.o $CHECKSUM_OBJ
		size $PGM
		ls -l $PGM

$PGM.o:		$PGM.c
		$CC $DEF -c -O $PGM.c

$CHECKSUM_OBJ:	$CHECKSUM_SRC
		as -o $CHECKSUM_OBJ $CHECKSUM_SRC

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
