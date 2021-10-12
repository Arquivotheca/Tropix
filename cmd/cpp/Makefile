#*
#****************************************************************
#*	Makefile para o programa "cpp"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/lib
PGM	= cpp
PROT	= a+x,u+m
MAN	= cmd
CC	= cc
DEF	=

OBJ	=	obj/cpp.o	\
		obj/scan.o	\
		obj/error.o	\
		obj/parse.o	\
		obj/sym.o	\
		obj/define.o	\
		obj/include.o	\
		obj/expand.o	\
		obj/expr.o	\
		obj/pragma.o	\
		obj/directive.o

TBL	=	obj/chartb.o	\
		obj/nametb.o	\
		obj/symtb.o	\
		obj/hashtb.o	\
		obj/macdef.o

SRC	=	src/cpp.c	\
		src/scan.c	\
		src/mktb.c	\
		src/error.c	\
		src/parse.c	\
		src/sym.c	\
		src/define.c	\
		src/include.c	\
		src/expand.c	\
		src/pragma.c	\
		src/directive.c

.virtual	unld c lint tst cp cmp cmpq cmpqcp man modif prman
.virtual	pento penfrom pensave

#*
#******	Link-Edição Final ***************************************
#*
$PGM:		obj $OBJ $TBL
		$CC $DEF -u -s -o $PGM $OBJ $TBL
		size $PGM
		ls -l $PGM

unld:
		-rm -f $PGM

c:		obj $OBJ $TBL

obj:		.exist
		mkdir obj

#*
#******	Dependências e Compilação dos Módulos *******************
#*
obj/cpp.o:	src/cpp.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h
		$CC $DEF -c -O -o obj src/cpp.c

obj/scan.o:	src/scan.c		\
		h/scan.h		\
		h/lex.h			\
		h/error.h		\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/scan.c

obj/error.o:	src/error.c		\
		h/scan.h		\
		h/error.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/error.c

obj/parse.o:	src/parse.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/parse.c

obj/directive.o: src/directive.c	\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/directive.c

obj/sym.o:	src/sym.c		\
		h/scan.h		\
		h/error.h		\
		h/symtb.h		\
		h/cpp.h			\
		h/hash.h
		$CC $DEF -c -O -o obj src/sym.c

obj/define.o:	src/define.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/define.c

obj/include.o:	src/include.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/include.c

obj/expand.o:	src/expand.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/expand.c

obj/expr.o:	src/expr.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/expr.c

obj/pragma.o:	src/pragma.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/cpp.h
		$CC $DEF -c -O -o obj src/pragma.c

obj/chartb.o:	src/chartb.c		\
		h/scan.h		\
		h/lex.h
		$CC $DEF -c -O -o obj src/chartb.c

#*
#******	Geração das Tabelas ("mkasmtb") *************************
#*
obj/nametb.o	\
obj/symtb.o	\
obj/macdef.o	\
obj/hashtb.o: 	src/mktb.c		\
		h/scan.h		\
		h/error.h		\
		h/lex.h			\
		h/symtb.h		\
		h/hash.h
		$CC $DEF -s -u -o obj/mktb src/mktb.c
		obj/mktb
		as -o obj/nametb.o nametb.s
		as -o obj/symtb.o  symtb.s
		as -o obj/hashtb.o hashtb.s
		as -o obj/macdef.o macdef.s
		rm -f nametb.s symtb.s hashtb.s macdef.s obj/mktb

lint:
		$CC $DEF -T $SRC

#*
#******	Teste ***************************************************
#*
tst:
		$PGM src/scan.c scan.i
		/lib/gcc.486 -ftropix -O -o scan.s scan.i
		as scan.s
		cmpobj a.out obj/scan.o
		rm -f scan.i scan.s a.out 

#*
#******	Cópia, manuais, ... *************************************
#*
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

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
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

