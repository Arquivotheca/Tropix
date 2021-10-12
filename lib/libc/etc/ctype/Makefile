#*
#****************************************************************
#*	Makefile para a função "ctype"				*
#****************************************************************
#*
DIR	= etc
PGM	= ctype
PROT	= u=rw,a+r
MAN	= libc
CC	= cc
DEF	=

.virtual	lint cp cmp man man0 man1 modif prman gar

$PGM.o:		$PGM.c ctype.h
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		$PGM.c
		$CC $DEF -T $PGM.c

t:		t.o $PGM.o
		$CC -o t -f t.o $PGM.o

t.o:		t.c
		$CC -c t.c

q:		q.o $PGM.o
		$CC -o q -f q.o $PGM.o

q.o:		q.c
		$CC -c q.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmp ctype.h /usr/include
		-cmpobj $PGM.o /lib/libc/$DIR

man:		man0 man1

man0:		man0.r
		sroff man0.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isalpha
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isupper
		-ln $mandir/$MAN/$PGM $mandir/$MAN/islower
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isdigit
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isxdigit
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isalnum
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isspace
		-ln $mandir/$MAN/$PGM $mandir/$MAN/ispunct
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isprint
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isgraph
		-ln $mandir/$MAN/$PGM $mandir/$MAN/iscntrl
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isascii
		-ln $mandir/$MAN/$PGM $mandir/$MAN/isiso
		man -l $PGM

man1:		man1.r
		sroff man1.r >$mandir/$MAN/conv
		-ln $mandir/$MAN/conv $mandir/$MAN/toupper
		-ln $mandir/$MAN/conv $mandir/$MAN/_toupper
		-ln $mandir/$MAN/conv $mandir/$MAN/tolower
		-ln $mandir/$MAN/conv $mandir/$MAN/_tolower
		-ln $mandir/$MAN/conv $mandir/$MAN/toascii
		-ln $mandir/$MAN/conv $mandir/$MAN/toiso
		man -l conv
prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
