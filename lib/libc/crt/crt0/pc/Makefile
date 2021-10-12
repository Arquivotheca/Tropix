#*
#****************************************************************
#*	Makefile para a chamada ao sistema "crt0"		*
#****************************************************************
#*
DIR	= /lib
PGM	= crt0
MAN	= libc

.virtual	cp cmp man prman modif

$PGM.o:		$PGM.s
		as -o $PGM.o -l $PGM.l $PGM.s
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o

cp:		$PGM.o
		cp $PGM.o $DIR
		chmod -v u=rw,a+r $DIR/$PGM.o

cmp:
		-cmpobj $PGM.o $DIR

man:		../man.r
		sroff ../man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff ../man.r | lpr
