#*
#****************************************************************
#*	Makefile para a função "t_getstate"			*
#****************************************************************
#*
PGM	= t_getstate
PROT	= u=rw,a+r
MAN	= xti
DEF	= -D PC -D LITTLE_ENDIAN

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		cc $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | /usr/bin/c

lint:		$PGM.c
		cc $DEF -T $PGM.c

t:		t.o $PGM.o
		cc -o t t.o $PGM.o

t.o:		t.c
		cc -c t.c

cp:		$PGM.o
		mklib -rv /lib/libxti $PGM.o
		chmod -v $PROT /lib/libxti/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libxti

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs9 $PGM
		cd ..; gar -icv9
