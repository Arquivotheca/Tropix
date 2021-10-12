#*
#****************************************************************
#*	Makefile para o programa "POP3_S"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/lib/itnet
MANDIR	= $ROOT/usr/man/cmd
PGM	= pop3
PROT	= u=rwx
CC	= cc
DEF	= -D LITTLE_ENDIAN -D DIR_ENDIAN_CV

SRC	= src/$(PGM)_s.c src/login.c src/cmd.c src/mbox.c
OBJ	= obj/$(PGM)_s.o obj/login.o obj/cmd.o obj/mbox.o

.virtual	lint lintp lintl lintc lintb cp cmp cmpq cmpqcp unld man gar gara

$(PGM)_s:	obj $OBJ
		$CC -u -s -o $(PGM)_s $OBJ
		size $(PGM)_s

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/$(PGM)_s.o: src/$(PGM)_s.c h/pop3_s.h
		$CC $DEF -c -O -o obj src/$(PGM)_s.c

obj/login.o:	src/login.c h/pop3_s.h
		$CC $DEF -c -O -o obj src/login.c

obj/cmd.o:	src/cmd.c h/pop3_s.h
		$CC $DEF -c -O -o obj src/cmd.c

obj/mbox.o:	src/mbox.c h/pop3_s.h
		$CC $DEF -c -O -o obj src/mbox.c

lint:
		$CC $DEF -T $SRC

lintp:
		$CC $DEF -T src/$(PGM)_s.c

lintl:
		$CC $DEF -T src/login.c

lintc:
		$CC $DEF -T src/cmd.c

lintb:
		$CC $DEF -T src/mbox.c

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

man:		man.r
		sroff man.r > $MANDIR/$PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
