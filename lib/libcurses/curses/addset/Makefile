#*
#****************************************************************
#*	Makefile para a função "addset"				*
#****************************************************************
#*
PGM	= addset
MAN	= curses

.virtual	man prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/w$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/waddon
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/waddoff
		man $PGM

prman:
		sroff man.r | lpr
