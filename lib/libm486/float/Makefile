#*
#****************************************************************
#*	Makefile para a biblioteca "libm/float"			*
#****************************************************************
#*
LIB	= /lib/libm
DIR	= float
PROT	= a+xr

man_exec = DUMMY
.putenv man_exec

.virtual	float n lint cp cmp man clr gar

float:
	cd fexcep;	make
	cd fgman;	make
	cd fixdfsi;	make
	cd fixunsdfsi;	make
	cd frexp;	make

n:
	cd fexcep;	make -n
	cd fgman;	make -n
	cd fixdfsi;	make -n
	cd fixunsdfsi;	make -n
	cd frexp;	make -n

lint:
	@echo Não tem módulos em \"C\"

cp:	$LIB $LIB/$DIR
	cd fexcep;	make cp
	cd fgman;	make cp
	cd fixdfsi;	make cp
	cd fixunsdfsi;	make cp
	cd frexp;	make cp

$LIB:	.exist
	mkdir $LIB
	chmod $PROT $LIB

$LIB/$DIR: .exist
	mkdir $LIB/$DIR
	chmod $PROT $LIB/$DIR


cmp:
	cd fexcep;	make cmp
	cd fgman;	make cmp
	cd fixdfsi;	make cmp
	cd fixunsdfsi;	make cmp
	cd frexp;	make cmp

man:
	cd fexcep;	make man
	cd fgman;	make man
	cd frexp;	make man

clr:
	-rm -i */?.o
	-rm -i */?
	-rm -i bobo
	-rm -i */bobo
	-rm -i garfield
	-rm -i */garfield

gar:
	cd ..; gar -ovs8 float
	cd ..; gar -icv8
