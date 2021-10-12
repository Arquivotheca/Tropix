#*
#****************************************************************
#*	Makefile para a chamada ao sistema "getgid"		*
#****************************************************************
#*
DIR	= sys
PGM	= getgid
MAN	= sys

.virtual	cp cmp man prman modif gar

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		../t.c $PGM.o
		cc -o t ../t.c $PGM.o

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v u=rw,a+r /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:
		@echo Veja ../getuid

modif:
		@echo Veja ../getuid

prman:
		@echo Veja ../getuid

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
