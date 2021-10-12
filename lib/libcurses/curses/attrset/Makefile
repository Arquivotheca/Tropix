#*
#****************************************************************
#*	Makefile para a função "attrset"			*
#****************************************************************
#*
PGM	= attrset
MAN	= curses

.virtual	man prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/w$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wattron
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wattroff
		man $PGM

prman:
		sroff $PGM.r |lpr
