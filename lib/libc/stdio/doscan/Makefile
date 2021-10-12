#*
#****************************************************************
#*	Makefile para a função "doscan"				*
#****************************************************************
#*
DIR	= stdio
PGM	= _doscan
MAN	= libc

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		cc -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		cc -T $PGM.c

all:		i f s ansi

i:		i.pgm
		i.pgm <i.in.v | diff -c i.out.v - | show

i.pgm:		i.c $PGM.o
		cc -o i.pgm i.c $PGM.o

f:		f.pgm
		f.pgm <f.in.v | diff -c f.out.v - | show

f.pgm:		f.c $PGM.o
		cc -o f.pgm f.c $PGM.o

s:		s.pgm
		s.pgm <s.in.v | diff -c s.out.v - | show

s.pgm:		s.c $PGM.o
		cc -o s.pgm s.c $PGM.o

ansi:		ansi.a ansi.b ansi.c

ansi.a:		ansi.a.pgm
		ansi.a.pgm <ansi.a.in.v | diff -c ansi.a.out.v - | show

ansi.a.pgm:	ansi.a.c $PGM.o
		cc -o ansi.a.pgm ansi.a.c $PGM.o

ansi.b:		ansi.b.pgm
		ansi.b.pgm <ansi.b.in.v | diff -c ansi.b.out.v - | show

ansi.b.pgm:	ansi.b.c $PGM.o
		cc -o ansi.b.pgm ansi.b.c $PGM.o

ansi.c:		ansi.c.pgm
		ansi.c.pgm <ansi.c.in.v | diff -c ansi.c.out.v - | show

ansi.c.pgm:	ansi.c.c $PGM.o
		cc -o ansi.c.pgm ansi.c.c $PGM.o

t:		t.o $PGM.o
		cc -o t t.o $PGM.o

t.o:		t.c
		cc -c t.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:
		@echo Veja ../scanf

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		@echo Veja ../scanf

gar:
		cd ..; gar -ovs0 doscan
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 doscan
		cd ..; gar -icv0
