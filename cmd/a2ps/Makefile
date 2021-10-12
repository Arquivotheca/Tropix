
#****************************************************************
#*	Makefile para o programa "a2ps"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= a2ps
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	=	src/$PGM.c		\
		src/prologue.c

OBJ	=	obj/$PGM.o		\
		obj/prologue.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC -us -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir -f obj

obj/$PGM.o:	src/$PGM.c
		$CC $DEF -O -c -o obj src/$PGM.c

obj/prologue.o:	src/prologue.c
		$CC $DEF -O -c -o obj src/prologue.c

lint:
		$CC $DEF -T $SRC

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

unld:		$PGM
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
