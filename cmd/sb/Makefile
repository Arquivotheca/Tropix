#*
#****************************************************************
#*	Makefile para o conjunto de programas "sb"		*
#****************************************************************
#*
PGM	= sb
MAN	= cmd

PGM1	= sbwave

man_exec = DUMMY
.putenv	man_exec

.virtual	compile lint n cp cmp cmpq cmpqcp unld man gar gara gars garcmp

compile:
	-cd sbwave;	make
	-cd sbvol;	make

b:
	-cd sbwave;	make -b
	-cd sbvol;	make -b

lint:
	-cd sbwave;	make lint
	-cd sbvol;	make lint

n:
	-cd sbwave;	make -n
	-cd sbvol;	make -n

cp:
	-cd sbwave;	make cp
	-cd sbvol;	make cp

cmp:
	-cd sbwave;	make cmp
	-cd sbvol;	make cmp

cmpq:
	-cd sbwave;	make cmpq
	-cd sbvol;	make cmpq

cmpqcp:
	-cd sbwave;	make cmpqcp
	-cd sbvol;	make cmpqcp

man:
	-cd sbwave;	make man
	-cd sbvol;	make man

unld:
	-cd sbwave;	make unld
	-cd sbvol;	make unld

gar:
	cd ..; gar -ovs0 $PGM
	cd ..; gar -icv0

gara:
	cd ..; gar -avs0 $PGM
	cd ..; gar -icv0

gars:
	cd ..; gar -ovs0 $PGM

garcmp:
	cd ..; gar -icv0
