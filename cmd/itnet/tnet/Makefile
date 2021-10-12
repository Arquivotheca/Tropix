#*
#****************************************************************
#*	Makefile para a biblioteca de programas "tnet"		*
#****************************************************************
#*
MAN	= cmd
PGM	= tnet
PGM1	= tcmpto
PGM2	= tcmpfrom
PGM3	= tcpto
PGM4	= tcpfrom
PGM5	= tpipe
man_exec = DUMMY
ROOT	=
CDIR	= $ROOT/usr/bin
SDIR	= $ROOT/usr/lib/itnet

.virtual	compile lint n cp cmp cmpq cmpqcp m size unld man modif prman gar

compile:
	-cd tcmpto_c;	make
	-cd tcmpto_s;	make

b:
	-cd tcmpto_c;	make -b
	-cd tcmpto_s;	make -b

lint:
	-cd tcmpto_c;	make lint
	-cd tcmpto_s;	make lint

n:
	-cd tcmpto_c;	make -n
	-cd tcmpto_s;	make -n

cp:
	-cd tcmpto_c;	make cp
	-cd tcmpto_s;	make cp

cmp:
	-cd tcmpto_c;	make cmp
	-cd tcmpto_s;	make cmp

cmpq:
	-cd tcmpto_c;	make cmpq
	-cd tcmpto_s;	make cmpq

cmpqcp:
	-cd tcmpto_c;	make cmpqcp
	-cd tcmpto_s;	make cmpqcp

#cmpq:
#	-cmpobj -q	tcmpto_c/tcmpto 	$CDIR/tcmpto
#	-cmpobj -q	tcmpto_s/tcmpto_s	$SDIR/tcmpto_s
#
#cmpqcp:
#	-cmpobj -q	tcmpto_c/tcmpto 	$CDIR/tcmpto 	|| (cd tcmpto_c; m cp)
#	-cmpobj -q	tcmpto_s/tcmpto_s	$SDIR/tcmpto_s	|| (cd tcmpto_s; m cp)

m:
	chmod +m	$CDIR/tcmpto $SDIR/tcmpto_s
	ls -ll 		$CDIR/tcmpto $SDIR/tcmpto_s

size:
	size tcmpto_c/tcmpto tcmpto_s/tcmpto_s

unld:
	-cd tcmpto_c;	make unld
	-cd tcmpto_s;	make unld

man:	man.r
	sroff man.r >$mandir/$MAN/$PGM
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM1
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM3
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM4
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM5
	man $PGM

modif:	modif.r
	sroff modif.r >$mandir/modif/$MAN/$PGM
	modif $PGM

prman:
	sroff man.r | lpr

gar:
	cd ..; gar -ovs0		\
		-p $CDIR/$PGM1		\
		-p $CDIR/$PGM2		\
		-p $CDIR/$PGM3		\
		-p $CDIR/$PGM4		\
		-p $CDIR/$PGM5		\
		-p $SDIR/tcmpto_s	\
		tnet			\
		$CDIR/$PGM1		\
		$CDIR/$PGM2		\
		$CDIR/$PGM3		\
		$CDIR/$PGM4		\
		$CDIR/$PGM5		\
		$SDIR/tcmpto_s
	cd ..; gar -icv0
