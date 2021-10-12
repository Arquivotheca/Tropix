#*
#****************************************************************
#*	Makefile para a biblioteca "libm/conv"			*
#****************************************************************
#*
LIB	= /lib/libm
DIR	= conv
PROT	= a+xr

man_exec = DUMMY
.putenv man_exec

.virtual	conv n lint cp cmp man clr gar

conv:
	cd fcvt;	make
	cd pottb;	make
	cd strtod;	make

n:
	cd fcvt;	make -n
	cd pottb;	make -n
	cd strtod;	make -n

lint:
	cd fcvt;	make lint
	cd pottb;	make lint
	cd strtod;	make lint

cp:	$LIB $LIB/$DIR
	cd fcvt;	make cp
	cd pottb;	make cp
	cd strtod;	make cp

$LIB:	.exist
	mkdir $LIB
	chmod $PROT $LIB

$LIB/$DIR: .exist
	mkdir $LIB/$DIR
	chmod $PROT $LIB/$DIR

cmp:
	cd fcvt;	make cmp
	cd pottb;	make cmp
	cd strtod;	make cmp

man:
	cd strtod;	make man

clr:
	-rm -i */?.o
	-rm -i */?
	-rm -i bobo
	-rm -i */bobo
	-rm -i garfield
	-rm -i */garfield

gar:
	cd ..; gar -ovs8 conv
	cd ..; gar -icv8
