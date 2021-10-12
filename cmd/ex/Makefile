#*
#****************************************************************
#*	Makefile para o programa "ex"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= ex
PGM1	= vi
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

#
#******	Módulos fonte *******************************************
#
SRC	=	com/add.c	\
		com/com.c	\
		com/error.c	\
		com/scan.c	\
		com/sub.c	\
		gbl/buffer.c	\
		gbl/main.c	\
		gbl/mark.c	\
		gbl/re.c	\
		gbl/str.c	\
		txt/changes.c	\
		txt/io.c	\
		txt/list.c	\
		vis/edit.c	\
		vis/move.c	\
		vis/obj.c	\
		vis/page.c	\
		vis/tab.c	\
		vis/user.c	\
		vis/visual.c

#
#******	Módulos objeto ******************************************
#
OBJ	=	obj/add.o	\
		obj/com.o	\
		obj/error.o	\
		obj/scan.o	\
		obj/sub.o	\
		obj/buffer.o	\
		obj/main.o	\
		obj/mark.o	\
		obj/re.o	\
		obj/str.o	\
		obj/changes.o	\
		obj/io.o	\
		obj/list.o	\
		obj/edit.o	\
		obj/move.o	\
		obj/obj.o	\
		obj/page.o	\
		obj/tab.o	\
		obj/user.o	\
		obj/visual.o

#
#****** Capítulos do guia ***************************************
#
REF	=	ref/cap0.r	\
		ref/cap1.r	\
		ref/cap2.r	\
		ref/cap3.r	\
		ref/cap4.r	\
		ref/cap5.r

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman guia gar gara

#
#******	Geração do programa *************************************
#
$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		ln -f $PGM $PGM1
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

#
#******	Compilação dos módulos **********************************
#
obj/add.o:	com/add.c	\
		h/global.h	\
		h/text.h	\
		h/commands.h	\
		h/scan.h
		$CC $DEF -c -O -o obj com/add.c

obj/com.o:	com/com.c	\
		h/global.h	\
		h/text.h	\
		h/commands.h	\
		h/scan.h
		$CC $DEF -c -O -o obj com/com.c

obj/error.o:	com/error.c	\
		h/global.h	\
		h/scan.h
		$CC $DEF -c -O -o obj com/error.c

obj/scan.o:	com/scan.c	\
		h/global.h	\
		h/scan.h
		$CC $DEF -c -O -o obj com/scan.c

obj/sub.o:	com/sub.c	\
		h/global.h	\
		h/text.h	\
		h/commands.h
		$CC $DEF -c -O -o obj com/sub.c

obj/buffer.o:	gbl/buffer.c	\
		h/global.h	\
		h/text.h	\
		h/obj.h
		$CC $DEF -c -O -o obj gbl/buffer.c

obj/main.o:	gbl/main.c	\
		h/global.h	\
		h/text.h	\
		h/commands.h	\
		h/visual.h
		$CC $DEF -c -O -o obj gbl/main.c

obj/mark.o:	gbl/mark.c	\
		h/global.h	\
		h/text.h
		$CC $DEF -c -O -o obj gbl/mark.c

obj/re.o:	gbl/re.c	\
		h/global.h
		$CC $DEF -c -O -o obj gbl/re.c

obj/str.o:	gbl/str.c	\
		h/global.h
		$CC $DEF -c -O -o obj gbl/str.c

obj/changes.o:	txt/changes.c	\
		h/global.h	\
		h/text.h
		$CC $DEF -c -O -o obj txt/changes.c

obj/io.o:	txt/io.c	\
		h/global.h	\
		h/text.h
		$CC $DEF -c -O -o obj txt/io.c

obj/list.o:	txt/list.c	\
		h/global.h	\
		h/text.h
		$CC $DEF -c -O -o obj txt/list.c

obj/edit.o:	vis/edit.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h	\
		h/obj.h
		$CC $DEF -c -O -o obj vis/edit.c

obj/move.o:	vis/move.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h	\
		h/obj.h		\
		h/char.h
		$CC $DEF -c -O -o obj vis/move.c

obj/obj.o:	vis/obj.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h	\
		h/obj.h		\
		h/char.h
		$CC $DEF -c -O -o obj vis/obj.c

obj/page.o:	vis/page.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h
		$CC $DEF -c -O -o obj vis/page.c

obj/tab.o:	vis/tab.c	\
		h/global.h	\
		h/obj.h
		$CC $DEF -c -O -o obj vis/tab.c

obj/user.o:	vis/user.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h
		$CC $DEF -c -O -o obj vis/user.c

obj/visual.o:	vis/visual.c	\
		h/global.h	\
		h/text.h	\
		h/visual.h	\
		h/obj.h		\
		h/char.h
		$CC $DEF -c -O -o obj vis/visual.c


#
#******	Compilação dos módulos **********************************
#
lint:
		$CC -T $SRC

cp:		$PGM
		cp -f $PGM $DIR
		ln -f $DIR/$PGM $DIR/$PGM1
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$PGM1

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM $PGM1

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM1
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		ln -f $mandir/modif/$MAN/$PGM $mandir/modif/$MAN/$PGM1
		modif $PGM

prman:
		sroff man.r | lpr

guia:		$REF
		sroff $REF >$mandir/ref/$PGM
		ln -f $mandir/ref/$PGM $mandir/ref/$PGM1
		chmod u=rw,go=r $mandir/ref/$PGM
		man -g $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
