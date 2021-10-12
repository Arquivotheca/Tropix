#*
#****************************************************************
#*	Makefile para a função "end"				*
#****************************************************************
#*
PGM	= end
MAN	= libc

.virtual	man prman modif

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/etext
		-ln $mandir/$MAN/$PGM $mandir/$MAN/edata
		man $PGM

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM
