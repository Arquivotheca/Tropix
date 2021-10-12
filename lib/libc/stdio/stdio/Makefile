#*
#****************************************************************
#*	Makefile para a função "stdio"				*
#****************************************************************
#*
DIR	= stdio
PGM	= stdio
MAN	= libc

.virtual	cp cmp man prman gar

cp:

cmp:

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
