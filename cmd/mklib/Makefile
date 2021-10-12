#*
#****************************************************************
#*	Makefile para o programa "mklib"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= mklib
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	= -D PC

SRC	=	src/main.c		\
		src/do_table.c		\
		src/do_consist.c	\
		src/do_create.c		\
		src/do_replace.c	\
		src/do_update.c		\
		src/do_delete.c		\
		src/sinop_io.c		\
		src/sort.c		\
		src/aux.c	

OBJ	=	obj/main.o		\
		obj/do_table.o		\
		obj/do_consist.o	\
		obj/do_create.o		\
		obj/do_replace.o	\
		obj/do_update.o		\
		obj/do_delete.o		\
		obj/sinop_io.o		\
		obj/sort.o		\
		obj/aux.o

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman gar gara
.virtual	lintmain lintm linttable lintt lintconsist lintk
.virtual	lintcreate lintc lintreplace lintr lintupdate lintu
.virtual	lintdelete lintd lintsinop lintio lintsort lints
.virtual	lintaux linta

$PGM:		obj $OBJ
		$CC $DEF -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/main.o:	src/main.c h/mklib.h
		$CC $DEF -c -O -o obj src/main.c

obj/do_table.o:	src/do_table.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_table.c

obj/do_consist.o: src/do_consist.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_consist.c

obj/do_create.o: src/do_create.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_create.c

obj/do_replace.o: src/do_replace.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_replace.c

obj/do_update.o: src/do_update.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_update.c

obj/do_delete.o: src/do_delete.c h/mklib.h
		$CC $DEF -c -O -o obj src/do_delete.c

obj/sinop_io.o:	src/sinop_io.c h/mklib.h
		$CC $DEF -c -O -o obj src/sinop_io.c

obj/sort.o:	src/sort.c h/mklib.h
		$CC $DEF -c -O -o obj src/sort.c

obj/aux.o:	src/aux.c h/mklib.h
		$CC $DEF -c -O -o obj src/aux.c

lint:	
		$CC $DEF -T $SRC

lintmain:	
lintm:	
		$CC $DEF -T src/main.c

linttable:	
lintt:	
		$CC $DEF -T src/do_table.c

lintconsist:	
lintk:	
		$CC $DEF -T src/do_consist.c

lintcreate:	
lintc:	
		$CC $DEF -T src/do_create.c

lintreplace:	
lintr:	
		$CC $DEF -T src/do_replace.c

lintupdate:	
lintu:	
		$CC $DEF -T src/do_update.c

lintdelete:	
lintd:	
		$CC $DEF -T src/do_delete.c

lintsinop:	
lintio:	
		$CC $DEF -T src/sinop_io.c

lintsort:	
lints:	
		$CC $DEF -T src/sort.c

lintaux:	
linta:	
		$CC $DEF -T src/aux.c

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
