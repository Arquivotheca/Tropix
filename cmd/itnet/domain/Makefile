#*
#****************************************************************
#*	Makefile para os "daemons" da ITNET			*
#****************************************************************
#*
PGM1	= domain
PGM2	= name_server
ROOT	=
SDIR	= $ROOT/usr/lib/itnet

compile:
# 	cd $PGM1;	make
	cd $PGM2;	make

b:
# 	cd $PGM1;	make -b
	cd $PGM2;	make -b

n:
# 	cd $PGM1;	make -n
	cd $PGM2;	make -n

cp:
# 	cd $PGM1;	make cp
	cd $PGM2;	make cp

cmp:
# 	cd $PGM1;	make cmp
	cd $PGM2;	make cmp
cmpq:
# 	-cmpobj -q	$PGM1/$PGM1	$SDIR
	-cmpobj -q	$PGM2/$PGM2	$SDIR

cmpqcp:
# 	-cmpobj -q	$PGM1/$PGM1	$SDIR || (cd $PGM1; m cp)
	-cmpobj -q	$PGM2/$PGM2	$SDIR || (cd $PGM2; m cp)
