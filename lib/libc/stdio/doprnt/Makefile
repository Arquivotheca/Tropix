#*
#****************************************************************
#*	Makefile para a função "doprnt"				*
#****************************************************************
#*
LIBC	= /lib/libc
CC	= cc
DIR	= stdio
PGM	= _doprnt
MAN	= libc
DEF	= -D PC

.virtual	lint cp cmp clr man prman modif gar
.virtual	b.cmp c.cmp d.cmp e.cmp f.cmp g.cmp n.cmp	\
		o.cmp p.cmp q.cmp s.cmp u.cmp x.cmp all.cmp

$PGM.o:		$PGM.c
		$CC $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		
		$CC $DEF -T $PGM.c

all.cmp:	b.cmp c.cmp d.cmp e.cmp f.cmp g.cmp n.cmp	\
		o.cmp p.cmp q.cmp s.cmp u.cmp x.cmp

b:		b.c $PGM.o
		$CC -O -o b b.c $PGM.o ../printf/printf.o

b.cmp:		b
		b | diff -c b.v - | show

char:		c.c $PGM.o
		$CC -O -o char c.c $PGM.o ../printf/printf.o

c.cmp:		char
		char | diff -c c.v - | show

d:		d.c $PGM.o
		$CC -O -o d d.c $PGM.o ../printf/printf.o

d.cmp:		d
		d | diff -c d.v - | show

e:		e.c $PGM.o
		$CC -O -o e e.c $PGM.o ../printf/printf.o

e.cmp:		e
		e | diff -c e.v - | show

f:		f.c $PGM.o
		$CC -O -o f f.c $PGM.o ../printf/printf.o

f.cmp:		f
		f | diff -c f.v - | show

g:		g.c $PGM.o
		$CC -O -o g g.c $PGM.o ../printf/printf.o

g.cmp:		g
		g | diff -c g.v - | show

n:		n.c $PGM.o
		$CC -O -o n n.c $PGM.o ../printf/printf.o

n.cmp:		n
		n | diff -c n.v - | show

o:		o.c $PGM.o
		$CC -O -o o o.c $PGM.o ../printf/printf.o

o.cmp:		o
		o | diff -c o.v - | show

p:		p.c $PGM.o
		$CC -O -o p p.c $PGM.o ../printf/printf.o

p.cmp:		p
		p | diff -c p.v - | show

q:		q.c $PGM.o
		$CC -O -o q q.c $PGM.o ../printf/printf.o

q.cmp:		q
		q | diff -c q.v - | show


s:		s.c $PGM.o
		$CC -O -o s s.c $PGM.o ../printf/printf.o

s.cmp:		s
		s | diff -c s.v - | show

u:		u.c $PGM.o
		$CC -O -o u u.c $PGM.o ../printf/printf.o

u.cmp:		u
		u | diff -c u.v - | show

x:		x.c $PGM.o
		$CC -O -o x x.c $PGM.o ../printf/printf.o

x.cmp:		x
		x | diff -c x.v - | show

clr:
		-rm b char d e f g n o p q s u x

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:
		@echo Veja ../printf

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		@echo Veja ../printf

gar:
		cd ..; gar -ovs0 doprnt
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 doprnt
		cd ..; gar -icv0
