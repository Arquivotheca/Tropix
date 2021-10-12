#*
#****************************************************************
#*	Makefile para o programa "ntfs"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/...
PGM	= ntfs
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif
.virtual	tst zipto zipfrom

$PGM:		.$PGM.c unicode.o ../h/nt.h
		$CC $DEF -u -O -o $PGM .$PGM.c unicode.o
		size $PGM
		ls -l $PGM

tst:		$PGM
		rm -f new; ./ntfs >new; diff -c old new | show

unicode.o:	unicode.s
		as -o unicode.o unicode.s

lint:
		$CC $DEF -T .$PGM.c

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

zipto:
		-cmptree -sg . /zip$ker/$PGM
		-forldiff /zip$ker/$PGM
		-rm garfield
		ldiff ../h/nt.h /zip$ker/h

zipfrom:
		ldiff /zip$ker/h/nt.h ../h/nt.h
		cd /zip$ker/$PGM &&		\
		cmptree -sg . $ker/$PGM;	\
		forldiff $ker/$PGM;		\
		rm garfield
