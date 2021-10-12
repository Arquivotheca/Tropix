#*
#****************************************************************
#*	Makefile para a função "crt0"				*
#****************************************************************
#*
DIR	= csu
PGM	= crt0
MAN	= libc

$PGM.o:		$PGM.s
		as -o crt0.o -l crt0.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

t:		t.o $PGM.o
		ld -o t $PGM.o t.o -lc

t.o:		t.c
		cc -c t.c

s:		s.c
		cc -o s s.c

q:		q.c
		cc -o q q.c

cp:		$PGM.o
		cp crt0.o /lib
		chmod -v a+r /lib/crt0.o

cmp:		crt0.o
		-cmpobj $PGM.o /lib

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		man $PGM

prman:
		sroff ../man.r | lpr

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
