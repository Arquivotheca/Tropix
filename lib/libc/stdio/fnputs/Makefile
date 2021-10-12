#*
#****************************************************************
#*	Makefile para a função "fnputs"				*
#****************************************************************
#*
DIR	= stdio
PGM	= fnputs
MAN	= libc

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		cc -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:
		cc -T $PGM.c

t:		t.o $PGM.o
		cc -o t t.o $PGM.o

t.o:		t.c
		cc -c t.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

man:
		@echo Veja ../puts

prman:
		@echo Veja ../puts

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
