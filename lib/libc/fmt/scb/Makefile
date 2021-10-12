#*
#****************************************************************
#*	Makefile para o programa "scb"				*
#****************************************************************
#*
PGM	= scb
MAN	= fmt

.virtual	man prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

prman:
		sroff man.r | lpr
