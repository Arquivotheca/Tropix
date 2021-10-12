#*
#****************************************************************
#*	Makefile para o programa "pop3"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= pop3
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	= -D DIR_ENDIAN_CV

SRC	= src/base64.c src/login.c src/mbox.c src/$(PGM)_c.c src/scan.c
OBJ	= obj/base64.o obj/login.o obj/mbox.o obj/$(PGM)_c.o obj/scan.o

.virtual	c lint lintp lintl lintm cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM: 		obj $OBJ
		$CC -u -o $PGM $OBJ
#		$CC -u -s -o $PGM $OBJ
		size $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/base64.o:	src/base64.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/base64.c

obj/login.o:	src/login.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/login.c

obj/mbox.o:	src/mbox.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/mbox.c

obj/$(PGM)_c.o:	src/$(PGM)_c.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/$(PGM)_c.c

obj/scan.o:	src/scan.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/scan.c

lint:
		$CC $DEF -T $SRC

lint64:
		$CC $DEF -T src/base64.c

lintl:
		$CC $DEF -T src/login.c

lintm:
		$CC $DEF -T src/mbox.c

lintp:
		$CC $DEF -T src/$(PGM)_c.c

lints:
		$CC $DEF -T src/scan.c

cp:		$PGM
		cp -f $PGM $DIR/$PGM
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
