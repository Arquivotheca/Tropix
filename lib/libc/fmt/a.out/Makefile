#*
#****************************************************************
#*	Makefile para o formato "a.out"				*
#****************************************************************
#*
PGM	= a.out
MAN	= fmt

.virtual	man prman gar

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

prman:		
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
