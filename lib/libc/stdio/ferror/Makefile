#*
#****************************************************************
#*	Makefile para a função "ferror"				*
#****************************************************************
#*
DIR	= stdio
PGM	= ferror
MAN	= libc

.virtual	man prman modif gar

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/feof
		-ln $mandir/$MAN/$PGM $mandir/$MAN/clearerr
		-ln $mandir/$MAN/$PGM $mandir/$MAN/fileno
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
