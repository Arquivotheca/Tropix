#*
#****************************************************************
#*	Makefile para o programa "rlogin"			*
#****************************************************************
#*
MAN	= cmd
PGM	= rlogin
man_exec = DUMMY
ROOT	=
CDIR	= $ROOT/usr/bin
SDIR	= $ROOT/usr/lib/itnet

.virtual	compile lint n cp cmp cmpq cmpqcp m size unld man modif prman gar gara

compile:
		cd rlogin_c;	make
		cd rlogin_s;	make

b:
		cd rlogin_c;	make -b
		cd rlogin_s;	make -b

lint:
		cd rlogin_c;	make lint
		cd rlogin_s;	make lint

n:
		cd rlogin_c;	make -n
		cd rlogin_s;	make -n

cp:
		cd rlogin_c;	make cp
		cd rlogin_s;	make cp

cmp:
		cd rlogin_c;	make cmp
		cd rlogin_s;	make cmp

cmpq:
		cd rlogin_c;	make cmpq
		cd rlogin_s;	make cmpq

cmpqcp:
		cd rlogin_c;	make cmpqcp
		cd rlogin_s;	make cmpqcp

#cmpq:
#		-cmpobj -q rlogin_c/rlogin   $CDIR/rlogin
#		-cmpobj -q rlogin_s/rlogin_s $SDIR/rlogin_s
#
#cmpqcp:
#		-cmpobj -q rlogin_c/rlogin   $CDIR/rlogin   || (cd rlogin_c; m cp)
#		-cmpobj -q rlogin_s/rlogin_s $SDIR/rlogin_s || (cd rlogin_s; m cp)

m:
		chmod +m	$CDIR/rlogin $SDIR/rlogin_s
		ls -ll 		$CDIR/rlogin $SDIR/rlogin_s

size:
		size rlogin_c/rlogin rlogin_s/rlogin_s

unld:
		cd rlogin_c;	make unld
		cd rlogin_s;	make unld

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
