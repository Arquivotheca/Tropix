REF	= $mandir/ref
PGM	= sroff
SRC	= sroff.r
.virtual	man

man:	$(SRC)
	sroff $(SRC) > $(REF)/$(PGM)
	chmod  u=rw,go=r $(REF)/$(PGM)
	man -r $(PGM)

prman:
	lpr -e "$(PGM) - ref" $(REF)/$(PGM)
