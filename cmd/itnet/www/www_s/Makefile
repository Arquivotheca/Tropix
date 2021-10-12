#*
#****************************************************************
#*	Makefile para o programa "www_s"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= www_s
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman
.virtual	pento penfrom pensave

$PGM:		$PGM.c
		$CC $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

lint:
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

man:
		@echo Veja ..

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/www/$PGM
		-forldiff /pen$itnet/www/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/www/$PGM &&		\
		cmptree -sg . $itnet/www/$PGM;		\
		forldiff $itnet/www/$PGM;		\
		rm -f /pen$itnet/www/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/www/$PGM
		-umount /pen

