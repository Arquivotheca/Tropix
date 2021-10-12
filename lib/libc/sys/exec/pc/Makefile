#*
#****************************************************************
#*	Makefile para a chamada ao sistema "exec"		*
#****************************************************************
#*
LIBC	= /lib/libc
DIR	= sys
PGM	= exec
MAN	= sys

.virtual	all allt cp cmp man prman modif gar

all:		execl.o execv.o execle.o execve.o

allt:		t l v L V		

execl.o:	execl.s
		as -o execl.o -l execl.l execl.s
		size execl.o
		ls -l execl.o
		nm execl.o

execv.o:	execv.s
		as -o execv.o -l execv.l execv.s
		size execv.o
		ls -l execv.o
		nm execv.o

execle.o:	execle.s
		as -o execle.o -l execle.l execle.s
		size execle.o
		ls -l execle.o
		nm execle.o

execve.o:	execve.s
		as -o execve.o -l execve.l execve.s
		size execve.o
		ls -l execve.o
		nm execve.o

t:		../t.c
		cc -o t ../t.c

l:		../l.c execl.o
		cc -o l ../l.c execl.o

v:		../v.c execv.o
		cc -o v ../v.c execv.o

L:		../L.c execle.o
		cc -o L ../L.c execle.o

V:		../V.c execve.o
		cc -o V ../V.c execve.o

cp:		execl.o execv.o execle.o execve.o
		mklib -rv $LIBC			\
			$DIR/execl.o		\
			$DIR/execv.o		\
			$DIR/execle.o		\
			$DIR/execve.o
		chmod -v u=rw,a+r		\
			$LIBC/$DIR/execl.o	\
			$LIBC/$DIR/execle.o	\
			$LIBC/$DIR/execv.o	\
			$LIBC/$DIR/execve.o

cmp:
		-cmpobj execl.o execv.o execle.o execve.o $LIBC/$DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/execl
		-ln $mandir/$MAN/$PGM $mandir/$MAN/execv
		-ln $mandir/$MAN/$PGM $mandir/$MAN/execle
		-ln $mandir/$MAN/$PGM $mandir/$MAN/execve
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
