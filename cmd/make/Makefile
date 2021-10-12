#*
#****************************************************************
#*	Makefile para o programa "make"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
LDIR	= /usr/bin
PGM	= make
PROT	= a+x,+m
MAN	= cmd
NAME	= m
CFLAGS	= -O
CC	= cc
AS	= as
DEF	= -D PC -D LITTLE_ENDIAN

INC	=	h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/categ.h	\
		h/extern.h	\
		h/proto.h

SRC	=	tbl/chartb.c	\
		tbl/mksym.c	\
		src/error.c	\
		src/hash.c	\
		src/scan.c	\
		src/parse.c	\
		src/update.c	\
		src/make.c

OBJ	=	obj/chartb.o	\
		obj/sym.o	\
		obj/error.o	\
		obj/hash.o	\
		obj/scan.o	\
		obj/parse.o	\
		obj/update.o	\
		obj/make.o

.virtual	lint cp cmp cmpq cmpqcp unld man modif prman
.virtual	pento penfrom pensave

$PGM:		obj $OBJ
		$CC -su -o $@ $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir -f obj

obj/chartb.o:	tbl/chartb.c	\
		h/categ.h
		$CC $DEF $CFLAGS -c -o obj tbl/chartb.c

obj/sym.o:	tbl/mksym.c	\
		obj/hash.o	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/proto.h
		$CC $DEF $CFLAGS -o mksym tbl/mksym.c obj/hash.o
		mksym
		$AS -o obj/sym.o nullstr.s symtb.s hashtb.s \
			at_macro.s star_macro.s ask_macro.s minor_macro.s
		rm -f mksym nullstr.s symtb.s hashtb.s \
			at_macro.s star_macro.s ask_macro.s minor_macro.s

obj/error.o:	src/error.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/extern.h	\
		h/proto.h
		$CC $DEF $CFLAGS -c -o obj src/error.c

obj/hash.o:	src/hash.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h
		$CC $DEF $CFLAGS -c -o obj src/hash.c

obj/scan.o:	src/scan.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/categ.h	\
		h/extern.h	\
		h/proto.h
		$CC $DEF $CFLAGS -c -o obj src/scan.c

obj/parse.o:	src/parse.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/extern.h	\
		h/proto.h
		$CC $DEF $CFLAGS -c -o obj src/parse.c

obj/update.o:	src/update.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/extern.h	\
		h/proto.h
		$CC $DEF $CFLAGS -c -o obj src/update.c

obj/make.o:	src/make.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/extern.h	\
		h/proto.h
		$CC $DEF $CFLAGS -c -o obj src/make.c

lint:		
		$CC -T $SRC

#lint:		$SRC
#		$CC -T $?
#		touch $@

cp:		$PGM
		mv -f $DIR/$PGM $DIR/old.$PGM
		mv -f $DIR/$NAME $DIR/old.$NAME
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ln -f $DIR/$PGM $DIR/$NAME
		ls -L $DIR/$PGM $DIR/$NAME
		$DIR/$PGM -M

cmp:
		-cmpobj $PGM $DIR
		-cmpobj $DIR/$PGM $DIR/$NAME

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || $LDIR/make cp

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

pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen
