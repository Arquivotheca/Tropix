#*
#****************************************************************
#*	Makefile para a função "refset"				*
#****************************************************************
#*
PGM	= refset
MAN	= curses

.virtual	man prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/w$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wrefon
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wrefoff
		man $PGM

prman:
		sroff $PGM.r | lpr
