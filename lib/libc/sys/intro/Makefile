#*
#****************************************************************
#*	Makefile para a chamada ao sistema "intro"		*
#****************************************************************
#*
DIR	= sys
PGM	= intro
MAN	= sys

.virtual	man modif prman

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:		
		sroff man.r | lpr
