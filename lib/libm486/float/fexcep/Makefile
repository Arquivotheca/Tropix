#*
#****************************************************************
#*	Makefile para a função "fexcep"				*
#****************************************************************
#*
LIBC	= /lib/libm
DIR	= float
PGM	= fexcep
MAN	= libc

.virtual	cp cmp man prman modif

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

t:		t.c $PGM.o
		cc -o t t.c $PGM.o

cp:		$PGM.o
		mklib -rv $LIBC $DIR/$PGM.o
		chmod -v u=rw,a+r $LIBC/$DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $LIBC/$DIR

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/fgcr
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/fgiar
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/fgsr
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/fscr
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/fssr
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff man.r | lpr
