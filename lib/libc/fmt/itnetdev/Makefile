#*
#****************************************************************
#*	Makefile para o formato "itnetdev"			*
#****************************************************************
#*
PGM	= itnetdev
MAN	= fmt

.virtual	man prman gar

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man -f $PGM

prman:		
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
