#*
#****************************************************************
#*	Makefile para o programa "sh"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
PGM	= sh
PROT	= u+m,a+rx
MAN	= cmd
CC	= cc
AS	= as

OBJ	=	obj/categtb.o	\
		obj/error.o	\
		obj/exec.o	\
		obj/expand.o	\
		obj/fnrgets.o	\
		obj/func.o	\
		obj/hist.o	\
		obj/parse.o	\
		obj/salloc.o	\
		obj/scan.o	\
		obj/sh.o	\
		obj/sym.o	\
		obj/symtb.o

SRC	=	tbl/categtb.c	\
		tbl/mksym.c	\
				\
		src/error.c	\
		src/exec.c	\
		src/expand.c	\
		src/fnrgets.c	\
		src/func.c	\
		src/hist.c	\
		src/parse.c	\
		src/salloc.c	\
		src/scan.c	\
		src/sh.c	\
		src/sym.c


.virtual	unld c lint cp cmp cmpq cmpqcp man modif prman zipto zipfrom

#
#****************************************************************
#	Link-edição final					*
#****************************************************************
#
$PGM:		obj $OBJ
		$CC -su -o $PGM $OBJ
		size $PGM
		ls -l $PGM

unld:
		-rm -f $PGM

obj:		.exist
		mkdir obj

c:		$OBJ

lint:
		$CC -T $SRC

#
#****************************************************************
#	Módulos funcionais					*
#****************************************************************
#
obj/categtb.o:	tbl/categtb.c	\
		h/categ.h
		$CC -c -O -o obj tbl/categtb.c

obj/error.o:	src/error.c	\
		h/common.h	\
		h/error.h	\
		h/sym.h		\
		h/scan.h	\
		h/categ.h
		$CC -c -O -o obj src/error.c

obj/exec.o:	src/exec.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/command.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/exec.c

obj/expand.o:	src/expand.c	\
		h/common.h	\
		h/sym.h		\
		h/command.h	\
		h/scan.h	\
		h/proto.h
		$CC -c -O -o obj src/expand.c

obj/fnrgets.o:	src/fnrgets.c	\
		h/categ.h
		$CC -c -O -o obj src/fnrgets.c

obj/func.o:	src/func.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/command.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/func.c

obj/hist.o:	src/hist.c	\
		h/common.h
		$CC -c -O -o obj src/hist.c

obj/parse.o:	src/parse.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/command.h	\
		h/error.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/parse.c

obj/salloc.o:	src/salloc.c
		$CC -c -O -o obj src/salloc.c

obj/scan.o:	src/scan.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/categ.h	\
		h/error.h	\
		h/command.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/scan.c

obj/sh.o:	src/sh.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/command.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/sh.c

obj/sym.o:	src/sym.c	\
		h/common.h	\
		h/sym.h		\
		h/scan.h	\
		h/command.h	\
		h/extern.h	\
		h/proto.h
		$CC -c -O -o obj src/sym.c

obj/symtb.o:	tbl/mksym.c	\
		h/sym.h
		$CC -O -o mksym tbl/mksym.c
		mksym
		$AS -o obj/symtb.o hashtb.s symtb.s hashsz.s
		rm -f mksym hashtb.s symtb.s hashsz.s

#
#****************************************************************
#	Cópia, comparação, etc...				*
#****************************************************************
#
cp:		$PGM
		-mv -f $DIR/$PGM $DIR/old.$PGM
		cp $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR/old.$PGM
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp


#
#****************************************************************
#	Manuais 						*
#****************************************************************
#
man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

#
#****************************************************************
#	Atualiza o ZIP 						*
#****************************************************************
#
zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield
