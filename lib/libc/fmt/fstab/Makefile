#*
#****************************************************************
#*	Makefile para o formato "fstab"			*
#****************************************************************
#*
PGM	= fstab
MAN	= fmt

.virtual	man

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man -f $PGM
