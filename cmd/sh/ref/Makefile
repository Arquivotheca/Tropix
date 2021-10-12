REF=	$mandir/ref
PGM=	sh
SRC=	cap0.r cap1.r cap2.r cap3.r cap4.r cap5.r cap6.r

.virtual man prman

man:	$(SRC)
	sroff $(SRC) > $(REF)/$(PGM)
	chmod  u=rw,go=r $(REF)/$(PGM)
	man -r $(PGM)

prman:
	sroff po.r $(SRC) | lpr -e "Guia - $(PGM)"
