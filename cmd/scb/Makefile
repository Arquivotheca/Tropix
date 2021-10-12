#*
#****************************************************************
#*	Makefile para o conjunto de programas "scb"		*
#****************************************************************
#*
PGM	= scb
MAN	= cmd

man_exec = DUMMY
.putenv	man_exec

.virtual	compile lint n cp cmp cmpq cmpqcp unld man modif gar gara gars garcmp

compile:
	cd edscb;	make
#	cd prscb;	make

b:
	cd edscb;	make -b
#	cd prscb;	make -b

lint:
	cd edscb;	make lint
#	cd prscb;	make lint

n:
	cd edscb;	make -n
#	cd prscb;	make -n

cp:
	cd edscb;	make cp
#	cd prscb;	make cp

cmp:
	cd edscb;	make cmp
#	cd prscb;	make cmp

cmpq:
	cd edscb;	make cmpq
#	cd prscb;	make cmpq

cmpqcp:
	cd edscb;	make cmpqcp
#	cd prscb;	make cmpqcp

unld:
	cd edscb;	make unld
#	cd prscb;	make unld

man:
	cd edscb;	make man
#	cd prscb;	make man

modif:
	cd edscb;	make modif
#	cd prscb;	make modif

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
