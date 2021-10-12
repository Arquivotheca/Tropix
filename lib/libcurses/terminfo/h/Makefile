#*
#****************************************************************
#*	Makefile para os includes de "terminfo"			*
#****************************************************************
#*
INC	= $inc
PGM	= terminfo.h

.virtual	cp cmp

cp:	$INC
	-cmp $PGM $INC || cp $PGM $INC
	-chmod a+r $INC/*.h

$INC:	.exist
	mkdir $INC

cmp:
	cmp -l $PGM $INC
