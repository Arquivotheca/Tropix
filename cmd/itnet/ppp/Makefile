#*
#****************************************************************
#*	Makefile para o programa "ppp"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= ppp
PGM2	= prot
PROT	= a+x
MAN	= cmd
DEF	= -D LITTLE_ENDIAN

.virtual	lint lintp cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.c
		cc $DEF -u -s -O -o $PGM $PGM.c
		size $PGM
		ls -l $PGM

$PGM2:		$PGM2.c
		cc -u $DEF -s -O -o $PGM2 $PGM2.c
		size $PGM2
		ls -l $PGM2

lint:		$PGM.c
		cc $DEF -T $PGM.c

lintp:		$PGM2.c
		cc $DEF -T $PGM2.c

dialnce:
		ppp -t ats6=5pd5983240
#		ppp -s 38.4 -t ats6=5pd5983240 -d

dialrnp:
		ppp -s 38.4 -t ats6=5pd2747445 -d

dialrnp2:
		ppp -s 38.4 -t ats6=5pd2392031

dialrnp3:
		ppp -s 38.4 -t ats6=5pd2392610

dialrnp4:
		ppp -s 38.4 -t ats6=5pd2397263

dialrnp5:
		ppp -s 38.4 -t ats6=5pd2942717

dialrnp6:
		ppp -s 38.4 -t ats6=6pd5112984

dialrnp7:
		ppp -s 38.4 -t ats6=7pd5110695

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		cmpobj -q $PGM $DIR/$PGM || make cp

unld:
		rm -f $PGM

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
