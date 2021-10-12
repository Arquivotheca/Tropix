#*
#****************************************************************
#*	Makefile para o header "curses.h"			*
#****************************************************************
#*
INC	= /usr/include
PGM	= curses
MAN	= curses

.virtual	cp cmp man prman modif

cp:		$PGM.h
		-cmp $PGM.h $INC || cp $PGM.h $INC
		-chmod -v u=rw,a+r $INC/$PGM.h

cmp:
		-cmp -l $PGM.h $INC/$PGM.h

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

prman:
		lpr $mandir/$MAN/$PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

