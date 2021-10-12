#*
#****************************************************************
#*	Makefile para os módulos de  "reduce"			*
#****************************************************************
#*
PGM	= reduce
SRC	= reduce0.c reduce1.c reduce2.c reduce3.c
OBJ	= obj/reduce0.o obj/reduce1.o obj/reduce2.o obj/reduce3.o
CC	= cc
DEF	= -D LITTLE_ENDIAN

.virtual	all lint lint0 lint1 lint2 lint3 gar

all:		$OBJ

obj/reduce0.o:	obj reduce0.c ../reduce.h
		$CC $DEF -c -O -o obj reduce0.c

obj/reduce1.o:	obj reduce1.c ../reduce.h
		$CC $DEF -c -O -o obj reduce1.c

obj/reduce2.o:	obj reduce2.c ../reduce.h
		$CC $DEF -c -O -o obj reduce2.c

obj/reduce3.o:	obj reduce3.c ../reduce.h
		$CC $DEF -c -O -o obj reduce3.c

obj:		.exist
		mkdir obj

lint:
		$CC $DEF -T $SRC

lint0:
		$CC $DEF -T reduce0.c

lint1:
		$CC $DEF -T reduce1.c

lint2:
		$CC $DEF -T reduce2.c

lint3:
		$CC $DEF -T reduce3.c

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
