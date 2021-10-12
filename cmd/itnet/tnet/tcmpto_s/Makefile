#*
#****************************************************************
#*	Makefile para o programa "tcmpto_s"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= tcmpto
PROT	= u=rwxm
MAN	= cmd
CC	= cc
RED	= ../reduce
REDOBJ	= $RED/obj
DEF	= -D LITTLE_ENDIAN

.virtual	c lint lintt linta lintc linte lintf lintl lintto lintw	\
		cp cmpq cmpqcp unld cmp man modif prman gar gara

OBJ	=	obj/$(PGM)_s.o		\
		obj/aux.o		\
		obj/cmp.o		\
		obj/exec.o		\
		obj/from.o		\
		obj/link.o		\
		obj/to.o		\
		obj/walk.o		\
		$REDOBJ/reduce0.o	\
		$REDOBJ/reduce1.o	\
		$REDOBJ/reduce2.o	\
		$REDOBJ/reduce3.o

SRC	=	src/$(PGM)_s.c		\
		src/aux.c		\
		src/cmp.c		\
		src/exec.c		\
		src/from.c		\
		src/link.c		\
		src/to.c		\
		src/walk.c

$(PGM)_s:	obj $OBJ
		$CC -u -s -o $(PGM)_s $OBJ
		size $(PGM)_s
		ls -L $(PGM)_s

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$(PGM)_s.o:	src/$(PGM)_s.c h/$(PGM)_s.h
		$CC $DEF -c -O -o obj src/$(PGM)_s.c

obj/aux.o:	src/aux.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/aux.c

obj/cmp.o:	src/cmp.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/cmp.c

obj/exec.o:	src/exec.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/exec.c

obj/from.o:	src/from.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/from.c

obj/link.o:	src/link.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/link.c

obj/to.o:	src/to.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/to.c

obj/walk.o:	src/walk.c h/tcmpto_s.h
		$CC $DEF -c -O -o obj src/walk.c

$REDOBJ/reduce0.o: $RED/reduce0.c ../reduce.h
		cd $RED; make obj/reduce0.o

$REDOBJ/reduce1.o: $RED/reduce1.c ../reduce.h
		cd $RED; make obj/reduce1.o

$REDOBJ/reduce2.o: $RED/reduce2.c ../reduce.h
		cd $RED; make obj/reduce2.o

$REDOBJ/reduce3.o: $RED/reduce3.c ../reduce.h
		cd $RED; make obj/reduce3.o

lint:
		$CC $DEF -T $SRC

lintt:
		$CC $DEF -T src/$(PGM)_s.c

linta:
		$CC $DEF -T src/aux.c

lintc:
		$CC $DEF -T src/cmp.c

linte:
		$CC $DEF -T src/exec.c

lintf:
		$CC $DEF -T src/from.c

lintl:
		$CC $DEF -T src/link.c

lintto:
		$CC $DEF -T src/to.c

lintw:
		$CC $DEF -T src/walk.c

cp:		$(PGM)_s
		cp -f $(PGM)_s $DIR
		chmod $PROT $DIR/$(PGM)_s
		ls -L $DIR/$(PGM)_s

cmp:
		-cmpobj $(PGM)_s $DIR

cmpq:
		-cmpobj -q $(PGM)_s $DIR

cmpqcp:
		-cmpobj -q $(PGM)_s $DIR || make cp

unld:
		-rm -f $(PGM)_s

man:
		@echo Veja ..

modif:
		@echo Veja ..

prman:
		@echo Veja ..

gar:
		cd ..; gar -ovs0 $(PGM)_s
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $(PGM)_s
		cd ..; gar -icv0
