#*
#****************************************************************
#*	Makefile para o programa "mail"				*
#****************************************************************
#*
MAN	= cmd
PGM	= mail
man_exec = DUMMY
ROOT	=
CDIR	= $ROOT/usr/bin
SDIR	= $ROOT/usr/lib/itnet
MDIR	= $ROOT/usr/lib

.virtual	mail lint n cp cmp cmpq cmpqcp m size ls unld man modif
.virtual	pento penfrom pensave

mail:
		cd $(PGM)_c;		make
		cd $(PGM)_s;		make
		cd send_$(PGM);		make
		cd $(PGM)_daemon;	make

b:
		cd $(PGM)_c;		make -b
		cd $(PGM)_s;		make -b
		cd send_$(PGM);		make -b
		cd $(PGM)_daemon;	make -b

lint:
		cd $(PGM)_c;		make lint
		cd $(PGM)_s;		make lint
		cd send_$(PGM);		make lint
		cd $(PGM)_daemon;	make lint

n:
		cd $(PGM)_c;		make -n
		cd $(PGM)_s;		make -n
		cd send_$(PGM);		make -n
		cd $(PGM)_daemon;	make -n

cp:
		cd $(PGM)_c;		make cp
		cd $(PGM)_s;		make cp
		cd send_$(PGM);		make cp
		cd $(PGM)_daemon;	make cp

cmp:
		cd $(PGM)_c;		make cmp
		cd $(PGM)_s;		make cmp
		cd send_$(PGM);		make cmp
		cd $(PGM)_daemon;	make cmp

cmpq:
		cd $(PGM)_c;		make cmpq
		cd $(PGM)_s;		make cmpq
		cd send_$(PGM);		make cmpq
		cd $(PGM)_daemon;	make cmpq

cmpqcp:
		cd $(PGM)_c;		make cmpqcp
		cd $(PGM)_s;		make cmpqcp
		cd send_$(PGM);		make cmpqcp
		cd $(PGM)_daemon;	make cmpqcp

#cmpq:
#		-cmpobj -q $(PGM)_c/$(PGM)   $CDIR/$(PGM)
#		-cmpobj -q $(PGM)_s/$(PGM)_s $SDIR/$(PGM)_s
#		-cmpobj -q send_$(PGM)/send_$(PGM) $MDIR/send_$(PGM)
#
#cmpqcp:
#		-cmpobj -q $(PGM)_c/$(PGM)   $CDIR/$(PGM)   || (cd $(PGM)_c; m cp)
#		-cmpobj -q $(PGM)_s/$(PGM)_s $SDIR/$(PGM)_s || (cd $(PGM)_s; m cp)
#		-cmpobj -q send_$(PGM)/send_$(PGM) $MDIR/send_$(PGM) || (cd send_$(PGM); make cp)

m:
		chmod +m	$CDIR/$(PGM) $SDIR/$(PGM)_s
		ls -ll 		$CDIR/$(PGM) $SDIR/$(PGM)_s

size:
		size $(PGM)_c/$(PGM) $(PGM)_s/$(PGM)_s send_$(PGM)/send_$(PGM)

ls:
		ls -L $(PGM)_c/$(PGM) $(PGM)_s/$(PGM)_s send_$(PGM)/send_$(PGM)

unld:
		cd $(PGM)_c;		make unld
		cd $(PGM)_s;		make unld
		cd send_$(PGM);		make unld
		cd $(PGM)_daemon;	make unld

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

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
