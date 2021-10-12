#*
#****************************************************************
#*	Makefile para o programa "bison"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PDIR	= $ROOT/usr/pub/lib/bison
PGM	= bison
PROT	= a+x
FPROT	= a+r
MAN	= cmd

PFILE	= bison.simple.c
PFILE1	= bison.hairy.c

SRC	= 	src/LR0.c		\
		src/allocate.c		\
		src/closure.c		\
		src/conflicts.c		\
		src/derives.c		\
		src/files.c		\
		src/gram.c		\
		src/lalr.c		\
		src/lex.c		\
		src/main.c		\
		src/nullable.c		\
		src/output.c		\
		src/print.c		\
		src/reader.c		\
		src/reduce.c		\
		src/symtab.c		\
		src/warshall.c

OBJ	= 	obj/LR0.o		\
		obj/allocate.o		\
		obj/closure.o		\
		obj/conflicts.o		\
		obj/derives.o		\
		obj/files.o		\
		obj/gram.o		\
		obj/lalr.o		\
		obj/lex.o		\
		obj/main.o		\
		obj/nullable.o		\
		obj/output.o		\
		obj/print.o		\
		obj/reader.o		\
		obj/reduce.o		\
		obj/symtab.o		\
		obj/warshall.o

.virtual	unld cmp cp cmpq cmpqcp gar lint lintfiles man modif nm prman

$PGM: 		obj $OBJ
		cc -u -s -O -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

obj/LR0.o:	src/LR0.c		\
		h/common.h		\
		h/machine.h		\
		h/new.h			\
		h/gram.h		\
		h/state.h	
		cc -c -O -o obj src/LR0.c

obj/allocate.o:	src/allocate.c		\
		h/common.h	
		cc -c -O -o obj src/allocate.c

obj/closure.o:	src/closure.c		\
		h/common.h		\
		h/machine.h		\
		h/new.h			\
		h/gram.h	
		cc -c -O -o obj src/closure.c

obj/conflicts.o: src/conflicts.c	\
		h/common.h		\
		h/machine.h		\
		h/new.h			\
		h/files.h		\
		h/gram.h		\
		h/state.h	
		cc -c -O -o obj src/conflicts.c

obj/derives.o:	src/derives.c		\
		h/common.h		\
		h/new.h			\
		h/types.h		\
		h/gram.h	
		cc -c -O -o obj src/derives.c

obj/files.o:	src/files.c		\
		h/common.h		\
		h/files.h		\
		h/new.h			\
		h/gram.h	
		cc -c -O -D XPFILE=$PDIR/$PFILE -D XPFILE1=$PDIR/$PFILE1 -o obj src/files.c

obj/gram.o:	src/gram.c
		cc -c -O -o obj src/gram.c

obj/lalr.o:	src/lalr.c		\
		h/common.h		\
		h/machine.h		\
		h/types.h		\
		h/state.h		\
		h/new.h			\
		h/gram.h	
		cc -c -O -o obj src/lalr.c

obj/lex.o:	src/lex.c		\
		h/common.h		\
		h/files.h		\
		h/symtab.h		\
		h/lex.h			\
		h/new.h	
		cc -c -O -o obj src/lex.c

obj/main.o:	src/main.c		\
		h/common.h		\
		h/machine.h		\
		h/files.h	
		cc -c -O -o obj src/main.c

obj/nullable.o:	src/nullable.c		\
		h/common.h		\
		h/types.h		\
		h/gram.h		\
		h/new.h	
		cc -c -O -o obj src/nullable.c

obj/output.o:	src/output.c		\
		h/common.h		\
		h/machine.h		\
		h/new.h			\
		h/files.h		\
		h/gram.h		\
		h/state.h	
		cc -c -O -o obj src/output.c

obj/print.o:	src/print.c		\
		h/common.h		\
		h/machine.h		\
		h/new.h			\
		h/files.h		\
		h/gram.h		\
		h/state.h	
		cc -c -O -o obj src/print.c

obj/reader.o:	src/reader.c		\
		h/common.h		\
		h/files.h		\
		h/new.h			\
		h/symtab.h		\
		h/lex.h			\
		h/gram.h		\
		h/machine.h	
		cc -c -O -o obj src/reader.c

obj/reduce.o:	src/reduce.c		\
		h/common.h		\
		h/files.h		\
		h/gram.h		\
		h/machine.h		\
		h/new.h	
		cc -c -O -o obj src/reduce.c

obj/symtab.o:	src/symtab.c		\
		h/common.h		\
		h/new.h			\
		h/symtab.h		\
		h/gram.h	
		cc -c -O -o obj src/symtab.c

obj/warshall.o:	src/warshall.c		\
		h/common.h		\
		h/machine.h	
		cc -c -O -o obj src/warshall.c

unld:
		-rm -f $PGM
lintfiles:
		cc -D XPFILE=$PFILE -D XPFILE1=$PFILE1 -T src/files.c

lint:
		cc -T $SRC

nm:
		nm -tn obj/*.o

tst:
		-rm -f tst/c-parse.tab.c
		cd tst; bison -d c-parse.y
		diff -c tst/old.tab.c tst/c-parse.tab.c | show
		diff -c tst/old.tab.h tst/c-parse.tab.h | show

cp:		$PDIR $PGM
		-cmp bison.simple.c $PDIR || cp bison.simple.c $PDIR
		-cmp bison.hairy.c $PDIR  || cp bison.hairy.c $PDIR
		cp -f $PGM $DIR
		chmod $FPROT $PDIR/bison.simple.c $PDIR/bison.hairy.c
		chmod $PROT $DIR/$PGM
		ls -L $PDIR $DIR/$PGM

cmp:		$PDIR
		-cmp bison.simple.c bison.hairy.c $PDIR
		-cmpobj $PGM $DIR

cmpq:
		-cmp bison.simple.c bison.hairy.c $PDIR
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

$PDIR:		.exist
		mkdir -f $PDIR

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
