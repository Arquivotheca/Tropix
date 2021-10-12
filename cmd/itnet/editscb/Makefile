#*
#****************************************************************
#*	Makefile para o programa "editscb"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= editscb
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman
.virtual	pento penfrom pensave

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

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/$PGM
		-forldiff /pen$itnet/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/$PGM &&		\
		cmptree -sg . $itnet/$PGM;	\
		forldiff $itnet/$PGM;		\
		rm -f /pen$itnet/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/$PGM
		-umount /pen

