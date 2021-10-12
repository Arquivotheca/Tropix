#*
#****************************************************************
#*	Makefile para o programa "edusr"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/etc
PGM	= edusr
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	=	src/aux.c	\
		src/cmd.c	\
		src/do_cota.c	\
		src/do_expir.c \
		src/do_home.c	\
		src/do_mkgrp.c	\
		src/do_mkusr.c	\
		src/do_prgrp.c	\
		src/do_prusr.c	\
		src/do_quit.c	\
		src/do_rmgrp.c	\
		src/do_rmusr.c	\
		src/do_set.c	\
		src/do_shell.c	\
		src/edusr.c	\
		src/getopt.c

OBJ	=	obj/aux.o	\
		obj/cmd.o	\
		obj/do_cota.o	\
		obj/do_expir.o	\
		obj/do_home.o	\
		obj/do_mkgrp.o	\
		obj/do_mkusr.o	\
		obj/do_prgrp.o	\
		obj/do_prusr.o	\
		obj/do_quit.o	\
		obj/do_rmgrp.o	\
		obj/do_rmusr.o	\
		obj/do_set.o	\
		obj/do_shell.o	\
		obj/edusr.o	\
		obj/getopt.o

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman gar gara

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/aux.o:	src/aux.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/aux.c

obj/cmd.o:	src/cmd.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/cmd.c

obj/do_cota.o: src/do_cota.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_cota.c

obj/do_expir.o: src/do_expir.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_expir.c

obj/do_home.o: src/do_home.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_home.c

obj/do_mkgrp.o: src/do_mkgrp.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_mkgrp.c

obj/do_mkusr.o: src/do_mkusr.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_mkusr.c

obj/do_prusr.o:	src/do_prusr.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_prusr.c

obj/do_prgrp.o:	src/do_prgrp.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_prgrp.c

obj/do_quit.o:	src/do_quit.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_quit.c

obj/do_rmgrp.o:	src/do_rmgrp.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_rmgrp.c

obj/do_rmusr.o:	src/do_rmusr.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_rmusr.c

obj/do_set.o:	src/do_set.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_set.c

obj/do_shell.o:	src/do_shell.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/do_shell.c

obj/edusr.o:	src/edusr.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/edusr.c

obj/getopt.o:	src/getopt.c		\
		h/edusr.h
		$CC $DEF -c -O -o obj src/getopt.c

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
