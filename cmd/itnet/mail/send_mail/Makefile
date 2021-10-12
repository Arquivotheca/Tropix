#*
#****************************************************************
#*	Makefile para o programa "send_mail"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib
PGM	= send_mail
MOD1	= mbox
MOD2	= show
PROT	= a+rx,u+s,g+s
MAN	= cmd
CC	= cc
DEF	= 
SRC	= $PGM.c
OBJ	= $PGM.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara
.virtual	pento penfrom pensave


$PGM:		$PGM.c
		$CC $DEF -O -u -s -o $PGM $PGM.c

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

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/mail/$PGM
		-forldiff /pen$itnet/mail/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/mail/$PGM &&		\
		cmptree -sg . $itnet/mail/$PGM;		\
		forldiff $itnet/mail/$PGM;		\
		rm -f /pen$itnet/$PGM/mail/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/mail/$PGM
		-umount /pen
