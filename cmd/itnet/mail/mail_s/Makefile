#*
#****************************************************************
#*	Makefile para o programa "mail_s"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= mail_s
PGMDIR	= mail_s
PROT	= u=rwx
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara
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

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/mail/$PGMDIR
		-forldiff /pen$itnet/mail/$PGMDIR
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/mail/$PGMDIR &&		\
		cmptree -sg . $itnet/mail/$PGMDIR;	\
		forldiff $itnet/mail/$PGMDIR;		\
		rm -f /pen$itnet/mail/$PGMDIR/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/mail/$PGMDIR
		-umount /pen
