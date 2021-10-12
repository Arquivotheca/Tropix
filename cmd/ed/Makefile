#*
#****************************************************************
#*	Makefile para o programa "ed"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= ed
PROT	= a+rx
MAN	= cmd
CC	= cc

SRC	=	src/main.c	src/msg.c	src/io.c	\
		src/list.c	src/add.c	src/com.c	\
		src/scan.c  	src/error.c	src/re.c	\
		src/mark.c	src/line.c  	src/sub.c

OBJ	=	obj/main.o	obj/msg.o	obj/io.o	\
		obj/list.o	obj/add.o	obj/com.o	\
		obj/scan.o  	obj/error.o	obj/re.o	\
		obj/mark.o	obj/line.o  	obj/sub.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

obj/main.o:	src/main.c	\
		h/ed.h		\
		h/txtio.h
		$CC -c -O -o obj src/main.c

obj/io.o:	src/io.c	\
		h/ed.h		\
		h/txtio.h
		$CC -c -O -o obj src/io.c

obj/msg.o:	src/msg.c	\
		h/ed.h
		$CC -c -O -o obj src/msg.c

obj/list.o:	src/list.c	\
		h/ed.h
		$CC -c -O -o obj src/list.c

obj/add.o:	src/add.c	\
		h/ed.h		\
		h/scan.h
		$CC -c -O -o obj src/add.c

obj/com.o:	src/com.c	\
		h/ed.h		\
		h/scan.h	\
		h/txtio.h
		$CC -c -O -o obj src/com.c

obj/scan.o:	src/scan.c	\
		h/ed.h		\
		h/scan.h
		$CC -c -O -o obj src/scan.c

obj/error.o:	src/error.c	\
		h/ed.h		\
		h/scan.h
		$CC -c -O -o obj src/error.c

obj/re.o:	src/re.c	\
		h/ed.h
		$CC -c -O -o obj src/re.c

obj/mark.o:	src/mark.c	\
		h/ed.h
		$CC -c -O -o obj src/mark.c

obj/line.o:	src/line.c	\
		h/ed.h
		$CC -c -O -o obj src/line.c

obj/sub.o:	src/sub.c	\
		h/ed.h
		$CC -c -O -o obj src/sub.c

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
