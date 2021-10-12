#*
#****************************************************************
#*	Makefile para o formato "environ"			*
#****************************************************************
#*
PGM	= environ
MAN	= fmt

.virtual	man prman gar

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/ambiente
		man -f $PGM

prman:		
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
