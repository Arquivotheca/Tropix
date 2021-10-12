#*
#****************************************************************
#*	Makefile para o programa "sort"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= sort
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

OBJ	= 	obj/$PGM.o	\
		obj/getcomp.o	\
		obj/compara.o	\
		obj/merge.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

obj/sort.o:	src/sort.c h/sort.h
		$CC -c -O -o obj src/sort.c

obj/getcomp.o:	src/getcomp.c h/sort.h
		$CC -c -O -o obj src/getcomp.c

obj/compara.o:	src/compara.c h/sort.h
		$CC -c -O -o obj src/compara.c

obj/merge.o:	src/merge.c h/sort.h
		$CC -c -O -o obj src/merge.c

lint:
		$CC $DEF -T src/$PGM.c src/getcomp.c src/compara.c src/merge.c

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
