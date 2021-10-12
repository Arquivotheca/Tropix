#*
#****************************************************************
#*	Makefile para o programa "www"				*
#****************************************************************
#*
MAN	= cmd
PGM	= www
man_exec = DUMMY
ROOT	=
CDIR	= $ROOT/usr/pub/bin
SDIR	= $ROOT/usr/lib/itnet

.virtual	compile lint n cp cmp cmpq cmpqcp m size unld man modif prman
.virtual	pento penfrom pensave

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

#cmpq:
#		-cmpobj -q $(PGM)_c/$(PGM)   $CDIR/$(PGM)
#		-cmpobj -q $(PGM)_s/$(PGM)_s $SDIR/$(PGM)_s
#
#cmpqcp:
#		-cmpobj -q $(PGM)_c/$(PGM)   $CDIR/$(PGM)   || (cd $(PGM)_c; m cp)
#		-cmpobj -q $(PGM)_s/$(PGM)_s $SDIR/$(PGM)_s || (cd $(PGM)_s; m cp)

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

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/$PGM
		-forldiff /pen$itnet/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/$PGM &&		\
		cmptree -sg . $itnet/$PGM;	\
		forldiff $itnet/$PGM;		\
		rm -f /pen$itnet/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/$PGM
		-umount /pen

