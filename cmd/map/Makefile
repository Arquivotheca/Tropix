#*
#****************************************************************
#*	Makefile para o conjunto de programas "map"		*
#****************************************************************
#*
PGM	= map
MAN	= cmd

man_exec = DUMMY
.putenv	man_exec

.virtual	compile lint n cp cmp cmpq cmpqcp unld man
.virtual	pento penfrom pensave

compile:
#	-cd bufmap;	make
	-cd coremap;	make
#	-cd inomap;	make
#	-cd swapmap;	make
	-cd sysmap;	make
	-cd textmap;	make
	-cd xcoremap;	make

b:
#	-cd bufmap;	make -b
	-cd coremap;	make -b
#	-cd inomap;	make -b
#	-cd swapmap;	make -b
	-cd sysmap;	make -b
	-cd textmap;	make -b
	-cd xcoremap;	make -b

lint:
#	-cd bufmap;	make lint
	-cd coremap;	make lint
#	-cd inomap;	make lint
#	-cd swapmap;	make lint
	-cd sysmap;	make lint
	-cd textmap;	make lint
	-cd xcoremap;	make lint

n:
#	-cd bufmap;	make -n
	-cd coremap;	make -n
#	-cd inomap;	make -n
#	-cd swapmap;	make -n
	-cd sysmap;	make -n
	-cd textmap;	make -n
	-cd xcoremap;	make -n

cp:
#	-cd bufmap;	make cp
	-cd coremap;	make cp
#	-cd inomap;	make cp
#	-cd swapmap;	make cp
	-cd sysmap;	make cp
	-cd textmap;	make cp
	-cd xcoremap;	make cp

cmp:
#	-cd bufmap;	make cmp
	-cd coremap;	make cmp
#	-cd inomap;	make cmp
#	-cd swapmap;	make cmp
	-cd sysmap;	make cmp
	-cd textmap;	make cmp
	-cd xcoremap;	make cmp

cmpq:
#	-cd bufmap;	make cmpq
	-cd coremap;	make cmpq
#	-cd inomap;	make cmpq
#	-cd swapmap;	make cmpq
	-cd sysmap;	make cmpq
	-cd textmap;	make cmpq
	-cd xcoremap;	make cmpq

cmpqcp:
#	-cd bufmap;	make cmpqcp
	-cd coremap;	make cmpqcp
#	-cd inomap;	make cmpqcp
#	-cd swapmap;	make cmpqcp
	-cd sysmap;	make cmpqcp
	-cd textmap;	make cmpqcp
	-cd xcoremap;	make cmpqcp

unld:
#	-cd bufmap;	make unld
	-cd coremap;	make unld
#	-cd inomap;	make unld
#	-cd swapmap;	make unld
	-cd sysmap;	make unld
	-cd textmap;	make unld
	-cd xcoremap;	make unld

man:
#	-cd bufmap;	make man
	-cd coremap;	make man
#	-cd inomap;	make man
#	-cd swapmap;	make man
	-cd sysmap;	make man
	-cd textmap;	make man
	-cd xcoremap;	make man

pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen

