#*
#****************************************************************
#*	Makefile para o programa "sroff"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= sroff
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

OBJ	=	obj/altera.o 	\
		obj/altvar.o	\
		obj/centra.o	\
		obj/citacao.o	\
		obj/lista.o	\
		obj/literal.o	\
		obj/parag.o	\
		obj/poepalvr.o	\
		obj/procarq.o	\
		obj/secnum.o	\
		obj/sroff.o	\
		obj/tbldata.o	\
		obj/tblfmt.o	\
		obj/tblinic.o	\
		obj/tblmt.o	\
		obj/tblopcao.o	\
		obj/titpag.o	\
		obj/trtblc.o	\
		obj/trtcarac.o	\
		obj/trtlin.o	\
		obj/trtpag.o	\
		obj/trtrod.o	\
		obj/trttit.o	\
		obj/trtwo.o

SRC	=	src/altera.c 	\
		src/altvar.c	\
		src/centra.c	\
		src/citacao.c	\
		src/lista.c	\
		src/literal.c	\
		src/parag.c	\
		src/poepalvr.c	\
		src/procarq.c	\
		src/secnum.c	\
		src/sroff.c	\
		src/tbldata.c	\
		src/tblfmt.c	\
		src/tblinic.c	\
		src/tblmt.c	\
		src/tblopcao.c	\
		src/titpag.c	\
		src/trtblc.c	\
		src/trtcarac.c	\
		src/trtlin.c	\
		src/trtpag.c	\
		src/trtrod.c	\
		src/trttit.c	\
		src/trtwo.c

.virtual	c lint cp cmp cmpq cmpqcp tst unld man modif prman gar gara

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

c:		$OBJ

obj/altera.o: 	h/sroff.h src/altera.c
		$CC -c -O -o obj src/altera.c

obj/altvar.o:	h/sroff.h src/altvar.c
		$CC -c -O -o obj src/altvar.c

obj/centra.o:	h/sroff.h src/centra.c
		$CC -c -O -o obj src/centra.c

obj/citacao.o:	h/sroff.h src/citacao.c
		$CC -c -O -o obj src/citacao.c

obj/lista.o: 	h/sroff.h src/lista.c
		$CC -c -O -o obj src/lista.c

obj/literal.o: 	h/sroff.h src/literal.c
		$CC -c -O -o obj src/literal.c 

obj/parag.o: 	h/sroff.h src/parag.c
		$CC -c -O -o obj src/parag.c

obj/poepalvr.o:	h/sroff.h src/poepalvr.c
		$CC -c -O -o obj src/poepalvr.c

obj/procarq.o:	h/sroff.h h/tbl.h src/procarq.c
		$CC -c -O -o obj src/procarq.c

obj/secnum.o: 	h/sroff.h src/secnum.c
		$CC -c -O -o obj src/secnum.c

obj/sroff.o: 	h/sroff.h h/tbl.h src/sroff.c
		$CC -c -O -o obj src/sroff.c

obj/tbldata.o: 	h/sroff.h h/tbl.h src/tbldata.c
		$CC -c -O -o obj src/tbldata.c

obj/tblfmt.o: 	h/sroff.h h/tbl.h src/tblfmt.c
		$CC -c -O -o obj src/tblfmt.c

obj/tblinic.o: 	h/sroff.h h/tbl.h src/tblinic.c
		$CC -c -O -o obj src/tblinic.c

obj/tblmt.o: 	h/sroff.h h/tbl.h src/tblmt.c
		$CC -c -O -o obj src/tblmt.c

obj/tblopcao.o:	h/sroff.h h/tbl.h src/tblopcao.c
		$CC -c -O -o obj src/tblopcao.c

obj/titpag.o: 	h/sroff.h src/titpag.c
		$CC -c -O -o obj src/titpag.c

obj/trtblc.o: 	h/sroff.h src/trtblc.c
		$CC -c -O -o obj src/trtblc.c

obj/trtcarac.o:	h/sroff.h src/trtcarac.c
		$CC -c -O -o obj src/trtcarac.c

obj/trtlin.o: 	h/sroff.h src/trtlin.c
		$CC -c -O -o obj src/trtlin.c

obj/trtpag.o:  	h/sroff.h src/trtpag.c
		$CC -c -O -o obj src/trtpag.c

obj/trtrod.o: 	h/sroff.h src/trtrod.c
		$CC -c -O -o obj src/trtrod.c

obj/trttit.o:	h/sroff.h src/trttit.c
		$CC -c -O -o obj src/trttit.c

obj/trtwo.o: 	h/sroff.h src/trtwo.c
		$CC -c -O -o obj src/trtwo.c

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

tst:		
		$DIR/$PGM man.r >/tmp/old.man
		$PGM	  man.r >/tmp/new.man
		diff -c /tmp/old.man /tmp/new.man
		rm /tmp/old.man /tmp/new.man

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
