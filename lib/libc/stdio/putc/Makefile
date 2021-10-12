#*
#****************************************************************
#*	Makefile para a macro "putc"				*
#****************************************************************
#*
DIR	= stdio
PGM	= putc
MAN	= libc

.virtual	man prman gar

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		-ln $mandir/$MAN/$PGM $mandir/$MAN/putchar
		-ln $mandir/$MAN/$PGM $mandir/$MAN/fputc
		-ln $mandir/$MAN/$PGM $mandir/$MAN/putw
		man $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
