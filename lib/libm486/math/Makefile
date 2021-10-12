#*
#****************************************************************
#*	Makefile para a biblioteca "libm/math"			*
#****************************************************************
#*
LIB	= /lib/libm
DIR	= math
PROT	= a+xr

man_exec = DUMMY
.putenv man_exec

.virtual	math n lint cp cmp man clr gar

math:
	cd asin;	make
	cd atan;	make
	cd exp;		make
	cd floor;	make
	cd hypot;	make
	cd log;		make
	cd sin;		make
	cd sinh;	make
	cd sqrt;	make
	cd tan;		make

n:
	cd asin;	make -n
	cd atan;	make -n
	cd exp;		make -n
	cd floor;	make -n
	cd hypot;	make -n
	cd log;		make -n
	cd sin;		make -n
	cd sinh;	make -n
	cd sqrt;	make -n
	cd tan;		make -n

lint:
	@echo Não tem módulos em \"C\"

cp:	$LIB $LIB/$DIR
	cd asin;	make cp
	cd atan;	make cp
	cd exp;		make cp
	cd floor;	make cp
	cd hypot;	make cp
	cd log;		make cp
	cd sin;		make cp
	cd sinh;	make cp
	cd sqrt;	make cp
	cd tan;		make cp

$LIB:	.exist
	mkdir $LIB
	chmod $PROT $LIB

$LIB/$DIR: .exist
	mkdir $LIB/$DIR
	chmod $PROT $LIB/$DIR


cmp:
	cd asin;	make cmp
	cd atan;	make cmp
	cd exp;		make cmp
	cd floor;	make cmp
	cd hypot;	make cmp
	cd log;		make cmp
	cd sin;		make cmp
	cd sinh;	make cmp
	cd sqrt;	make cmp
	cd tan;		make cmp

man:
	cd asin;	make man
	cd exp;		make man
	cd floor;	make man
	cd hypot;	make man
	cd log;		make man
	cd sin;		make man
	cd sinh;	make man
	cd sqrt;	make man

clr:
	-rm -i */?.o
	-rm -i */?
	-rm -i bobo
	-rm -i */bobo
	-rm -i garfield
	-rm -i */garfield

gar:
	cd ..; gar -ovs0 math
	cd ..; gar -icv0
