#*
#****************************************************************
#*	Makefile para a função "data"				*
#****************************************************************
#*
PGM	= data
DIR	= curses
MAN	= curses
LIB	= /lib/libcurses
CC	= cc
DEF	=

.virtual	lint cp cmp man modif prman gar gara

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		$CC $DEF -T $PGM.c

cp:		$PGM.o
		mklib -rv $LIB $DIR/$PGM.o
		chmod -v u=rw,a+r $LIB/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIB/$DIR

man:
		@echo Dados, não possuem manual

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif -w $PGM

prman:
		@echo Dados, não possuem manual

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
