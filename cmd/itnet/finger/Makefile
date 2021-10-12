#*
#****************************************************************
#*	Makefile para o programa "finger"			*
#****************************************************************
#*
MAN	= cmd
PGM	= finger
man_exec = DUMMY
ROOT	=
CDIR	= $ROOT/usr/bin
SDIR	= $ROOT/usr/lib/itnet

.virtual	compile lint n cp cmp cmpq cmpqcp m size unld man modif prman gar gara

compile:
		cd $(PGM)_c;	make
		cd $(PGM)_s;	make

b:
		cd $(PGM)_c;	make -b
		cd $(PGM)_s;	make -b

lint:
		cd $(PGM)_c;	make lint
		cd $(PGM)_s;	make lint

n:
		cd $(PGM)_c;	make -n
		cd $(PGM)_s;	make -n

cp:
		cd $(PGM)_c;	make cp
		cd $(PGM)_s;	make cp

cmp:
		cd $(PGM)_c;	make cmp
		cd $(PGM)_s;	make cmp

cmpq:
		cd $(PGM)_c;	make cmpq
		cd $(PGM)_s;	make cmpq
cmpqcp:
		cd $(PGM)_c;	make cmpqcp
		cd $(PGM)_s;	make cmpqcp

m:
		chmod +m	$CDIR/$(PGM) $SDIR/$(PGM)_s
		ls -ll 		$CDIR/$(PGM) $SDIR/$(PGM)_s

size:
		size $(PGM)_c/$(PGM) $(PGM)_s/$(PGM)_s

unld:
		cd $(PGM)_c;	make unld
		cd $(PGM)_s;	make unld

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
