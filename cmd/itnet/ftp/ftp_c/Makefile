#*
#****************************************************************
#*	Makefile para o programa "ftp"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= ftp
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	= -D LITTLE_ENDIAN

SRC	= $(PGM)_c.c login.c cmd.c
OBJ	= $(PGM)_c.o login.o cmd.o

.virtual	cp cmp cmpq cmpqcp unld man modif prman gar gara
.virtual	lint lintf lintc lintl

$PGM: 		$OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM

c:		$OBJ

$(PGM)_c.o:	$(PGM)_c.c $(PGM)_c.h
		$CC $DEF -c -O $(PGM)_c.c

cmd.o:		cmd.c $(PGM)_c.h
		$CC $DEF -c -O cmd.c

login.o:	login.c $(PGM)_c.h
		$CC $DEF -c -O login.c

lint:
		$CC $DEF -T $SRC

lintf:
		$CC $DEF -T $(PGM)_c.c

lintc:
		$CC $DEF -T cmd.c

lintl:
		$CC $DEF -T login.c

cp:		$PGM
		cp -f $PGM $DIR/$PGM
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

man:
		@echo Veja ..

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $(PGM)_c
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $(PGM)_c
		cd ..; gar -icv0
