#*
#****************************************************************
#*	Makefile para o programa "rlpr"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= rlpr
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	=	src/rlpr.c	\
		src/tcp.c

OBJ	=	obj/rlpr.o	\
		obj/tcp.o

.virtual	c lint cp cmp cmpq cmpqcp unld man modif

$PGM:		obj $OBJ
		$CC $DEF -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir -f obj

obj/$PGM.o:	src/$PGM.c	\
		h/tcp.h
		$CC -O -c -o obj src/$PGM.c

obj/tcp.o:	src/tcp.c
		cc -D CLIENT -O -c -o obj src/tcp.c

c:		obj $OBJ

lint:
		$CC $DEF -T -D CLIENT -D SERVER $SRC

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
