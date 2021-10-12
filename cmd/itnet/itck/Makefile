#*
#****************************************************************
#*	Makefile para o programa "itck"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/etc
PGM	= itck
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	lint cp cmp cmpq cmpqcp man modif prman gar gara

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

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR  || m cp

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
