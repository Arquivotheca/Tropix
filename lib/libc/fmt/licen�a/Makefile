#*
#****************************************************************
#*	Makefile para o formato "licença"			*
#****************************************************************
#*
PGM 	= licença
PGM2	= licenca
PGM3	= lpggnu
MAN	= fmt

.virtual	man prman lint gar

man:		$PGM.r
		sroff $PGM.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM3
		man -f $PGM

prman:		
		sroff $PGM.r | lpr

lint:		
		sroff $PGM.r >/dev/null

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
