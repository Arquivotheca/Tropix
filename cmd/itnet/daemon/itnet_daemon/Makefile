#*
#****************************************************************
#*	Makefile para o programa "itnet_daemon"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= itnet_daemon
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	=

.putenv	DIR
.virtual	i lint cp cmp cmpq cmpqcp unld man modif prman
.virtual	pento penfrom pensave

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

i:
		$DIR/$PGM

lint:
		$CC $DEF -T $PGM.c

cp:		$DIR $PGM
		-mv -f $DIR/$PGM $DIR/old.$PGM
		cp -f $PGM $DIR 
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

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
		-cmptree -sg . /pen$itnet/daemon/$PGM
		-forldiff /pen$itnet/daemon/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/daemon/$PGM &&		\
		cmptree -sg . $itnet/daemon/$PGM;	\
		forldiff $itnet/daemon/$PGM;		\
		rm -f /pen$itnet/daemon/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/daemon/$PGM
		-umount /pen
