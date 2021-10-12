#*
#****************************************************************
#*	Makefile para a função "getset"				*
#****************************************************************
#*
PGM	= getset
MAN	= curses

.virtual	man prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/w$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wgeton
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/wgetoff
		man $PGM

prman:
		sroff $PGM.r | lpr
