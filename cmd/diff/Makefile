#*
#****************************************************************
#*	Makefile para o programa "diff"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= diff
PROT	= a+x
MAN	= cmd
CC	= cc
SRC	= $(PGM)0.c $(PGM)1.c $(PGM)2.c $(PGM)3.c 
OBJ	= $(PGM)0.o $(PGM)1.o $(PGM)2.o $(PGM)3.o 

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		$PGM.h $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		$OBJ

$(PGM)0.o:	$(PGM)0.c $PGM.h
		$CC -c -O $(PGM)0.c

$(PGM)1.o:	$(PGM)1.c $PGM.h
		$CC -c -O $(PGM)1.c

$(PGM)2.o:	$(PGM)2.c $PGM.h
		$CC -c -O $(PGM)2.c

$(PGM)3.o:	$(PGM)3.c $PGM.h
		$CC -c -O $(PGM)3.c

lint:
		$CC -T $SRC

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
