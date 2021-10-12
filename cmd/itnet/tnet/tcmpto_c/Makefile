#*
#****************************************************************
#*	Makefile para o programa "tcmpto"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= tcmpto
PGM2	= tcmpfrom
PGM3	= tcpto
PGM4	= tcpfrom
PGM5	= tpipe
PROT	= u=rwxm,a+x
MAN	= cmd
RED	= ../reduce
REDOBJ	= $RED/obj
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	c lint lintt lintto linta lintc linte lintf lintl lintw	\
		cp cmp cmpq cmpqcp unld man modif prman gar gara

OBJ	=	obj/$(PGM)_c.o		\
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

SRC	=	src/$(PGM)_c.c		\
		src/aux.c		\
		src/cmp.c		\
		src/exec.c		\
		src/from.c		\
		src/link.c		\
		src/to.c		\
		src/walk.c

$PGM: 		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		ln -f $PGM $PGM2
		ln -f $PGM $PGM3
		ln -f $PGM $PGM4
		ln -f $PGM $PGM5
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$(PGM)_c.o:	src/$(PGM)_c.c h/$(PGM)_c.h
		$CC $DEF -c -O -o obj src/$(PGM)_c.c

obj/aux.o:	src/aux.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/aux.c

obj/cmp.o:	src/cmp.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/cmp.c

obj/exec.o:	src/exec.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/exec.c

obj/from.o:	src/from.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/from.c

obj/link.o:	src/link.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/link.c

obj/to.o:	src/to.c h/tcmpto_c.h
		$CC $DEF -c -O -o obj src/to.c

obj/walk.o:	src/walk.c h/tcmpto_c.h
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
		$CC $DEF -T src/$(PGM)_c.c

lintto:
		$CC $DEF -T src/to.c

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

lintw:
		$CC $DEF -T src/walk.c

cp:		$PGM
		cp -f $PGM $DIR/$PGM
		ln -f $DIR/$PGM $DIR/$PGM2
		ln -f $DIR/$PGM $DIR/$PGM3
		ln -f $DIR/$PGM $DIR/$PGM4
		ln -f $DIR/$PGM $DIR/$PGM5
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$PGM2 $DIR/$PGM3 $DIR/$PGM4 $DIR/$PGM5

cmp:
		-cmpobj $PGM $DIR/$PGM
		-cmpobj $DIR/$PGM $DIR/$PGM2
		-cmpobj $DIR/$PGM $DIR/$PGM3
		-cmpobj $DIR/$PGM $DIR/$PGM4
		-cmpobj $DIR/$PGM $DIR/$PGM5

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM $PGM2 $PGM3 $PGM4 $PGM5 

man:
		@echo Veja ..

modif:
		@echo Veja ..

prman:
		@echo Veja ..

gar:
		cd ..; gar -ovs0 $(PGM)_c
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $(PGM)_c
		cd ..; gar -icv0
