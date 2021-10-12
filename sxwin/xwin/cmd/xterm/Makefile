#*
#****************************************************************
#	Makefile para o cliente "xterm"				*
#****************************************************************
#*
PGM	=	xterm
DIR	=	/usr/xwin/bin
MAN	=	xwin
PROT	=	a=srx,u+w

SRC	=	src/xterm.c			\
		src/command.c			\
		src/misc.c			\
		src/screen.c			\
		src/scrollbar.c			\
		src/xdefaults.c

OBJ	=	obj/xterm.o			\
		obj/command.o			\
		obj/misc.o			\
		obj/screen.o			\
		obj/scrollbar.o			\
		obj/xdefaults.o

CC	=	cc
COPTS	=	-c -O -o $@

.virtual	cp cmp cmpq cmpqcp unld lint man modif clean rmobj
.virtual	pento penfrom pensave

$PGM:		obj $OBJ
		cc -us -o $PGM $OBJ -lx.o
		size $PGM

obj:		.exist
		mkdir obj

obj/xterm.o:	src/xterm.c			\
		h/feature.h			\
		h/command.h			\
		h/misc.h			\
		h/main.h			\
		h/screen.h			\
		h/scrollbar.h			\
		h/xdefaults.h	
		$CC $COPTS src/xterm.c

obj/command.o:	src/command.c			\
		h/feature.h			\
		h/command.h			\
		h/misc.h			\
		h/main.h			\
		h/screen.h			\
		h/scrollbar.h			\
		h/xdefaults.h	
		$CC $COPTS src/command.c

obj/misc.o:	src/misc.c			\
		h/misc.h			\
		h/main.h
		$CC $COPTS src/misc.c

obj/screen.o:	src/screen.c			\
		h/feature.h			\
		h/command.h			\
		h/misc.h			\
		h/main.h			\
		h/screen.h
		$CC $COPTS src/screen.c

obj/scrollbar.o:src/scrollbar.c			\
		h/feature.h			\
		h/misc.h			\
		h/main.h			\
		h/scrollbar.h
		$CC $COPTS src/scrollbar.c

obj/xdefaults.o:src/xdefaults.c			\
		h/feature.h			\
		h/command.h			\
		h/misc.h			\
		h/main.h			\
		h/screen.h			\
		h/scrollbar.h			\
		h/xdefaults.h	
		$CC $COPTS src/xdefaults.c

cp:		$PGM $DIR
		cp -f $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
		-rm -f $PGM

lint:		$SRC
		cc -T $SRC

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f

pento:
		-mount /pen
		-cmptree -sg . /pen/src/xwin/cmd/$PGM
		-forldiff /pen/src/xwin/cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen/src/xwin/cmd/$PGM &&		\
		cmptree -sg . /src/xwin/cmd/$PGM;	\
		forldiff /src/xwin/cmd/$PGM;		\
		rm -f /pen/src/xwin/cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento /src/xwin/cmd/$PGM
		-umount /pen

