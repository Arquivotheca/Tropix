#*
#****************************************************************
#*	Makefile para a função "caphash"			*
#****************************************************************
#*
PGM	= caphash
DIR	= terminfo
MAN	= curses
LIB	= /lib/libcurses
CC	= cc
DEF	=

.virtual	lint cp cmp man modif prman gar gara

$(PGM)tb.o:	$PGM.o mkhashtb 
		mkhashtb
		as -o $(PGM)tb.o $(PGM)tb.s boolno.s numno.s strno.s
		rm -f $(PGM)tb.s boolno.s numno.s strno.s
		size $PGM.o
		size $(PGM)tb.o
		ls -l $PGM.o
		ls -l $(PGM)tb.o
		nm $PGM.o | c
		nm $(PGM)tb.o | c

mkhashtb:	mkhashtb.o $PGM.o
		$CC $DEF -o mkhashtb mkhashtb.o $PGM.o

$PGM.o:		$PGM.c hash.h
		$CC $DEF -c -O $PGM.c

mkhashtb.o:	mkhashtb.c hash.h
		$CC $DEF -c -O mkhashtb.c

lint:
		$CC $DEF -T $PGM.c mkhashtb.c

t:		t.o
		$CC -o t t.o -lcurses
		t

t.o:		t.c
		$CC $DEF -c -O t.c

cp:		$(PGM)tb.o
		mklib -rv $LIB $DIR/$PGM.o $DIR/$(PGM)tb.o
		chmod -v u=rw,a+r $LIB/$DIR/$PGM.o $LIB/$DIR/$(PGM)tb.o

cmp:
		-cmpobj $PGM.o $(PGM)tb.o $LIB/$DIR

man:
		@echo Não possui manual

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif -w $PGM

prman:
		@echo Não possui manual

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
