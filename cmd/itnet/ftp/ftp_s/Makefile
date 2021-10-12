#*
#****************************************************************
#*	Makefile para o programa "FTP_S"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
PGM	= ftp
PROT	= u=rwx
CC	= cc
DEF	= -D LITTLE_ENDIAN

SRC	= main.c login.c cmd.c ls.c
OBJ	= main.o login.o cmd.o ls.o

.virtual	lint lintc lintl lintm lintls
.virtual	cp cmp cmpq cmpqcp unld man modif gar gara

$(PGM)_s:	$OBJ
		$CC -u -s -o $(PGM)_s $OBJ
		size $(PGM)_s

c:		$OBJ

main.o:		main.c ftp_s.h
		$CC $DEF -c -O main.c

login.o:	login.c ftp_s.h
		$CC $DEF -c -O login.c

cmd.o:		cmd.c ftp_s.h
		$CC $DEF -c -O cmd.c

ls.o:		ls.c ftp_s.h
		$CC $DEF -c -O ls.c

lint:
		$CC $DEF -T $SRC

lintc:
		$CC $DEF -T cmd.c

lintl:
		$CC $DEF -T login.c

lintm:
		$CC $DEF -T main.c

lintls:
		$CC $DEF -T ls.c

cp:		$(PGM)_s
		cp -f $(PGM)_s $DIR
		chmod $PROT $DIR/$(PGM)_s
		ls -L $DIR/$(PGM)_s

cmp:
		-cmpobj $(PGM)_s $DIR

cmpq:
		-cmpobj -q $(PGM)_s $DIR

cmpqcp:
		-cmpobj -q $(PGM)_s $DIR || make cp

unld:
		-rm -f $(PGM)_s

man:
		@echo Veja ..

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
