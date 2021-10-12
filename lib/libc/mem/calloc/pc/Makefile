#*
#****************************************************************
#*	Makefile para a função "calloc"				*
#****************************************************************
#*
DIR	= mem
PGM	= calloc
PROT	= u=rw,a+r
MAN	= libc

.virtual	cp cmp man prman modif gar

$PGM.o:		../$PGM.c
		cc -O -c ../$PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | c

lint:		../$PGM.c
		cc -T ../$PGM.c

t:		t.o $PGM.o
		cc -u  -o t t.o $PGM.o

t.o:		../t.c
		cc -c ../t.c

cp:		$PGM.o
		mklib -crv $DIR/$PGM.o
		chmod -v $PROT /lib/libc/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libc/$DIR

man:
		@echo Veja ../malloc

prman:
		@echo Veja ../malloc

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
