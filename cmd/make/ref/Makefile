#*
#****************************************************************
#*	Makefile para o programa "make/ref"			*
#****************************************************************
#*
REF=	$mandir/ref
PGM=	make
SRC=	make.r

.virtual man prman

man:	$SRC
	sroff $SRC > $REF/$PGM
	chmod  a+r $REF/$PGM
	man -r $PGM

prman:
	lpr -e "$PGM - ref" $REF/$PGM
