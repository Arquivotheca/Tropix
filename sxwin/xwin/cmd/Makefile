#*
#****************************************************************
#	Makefile para o diretório "xc/cmd"			*
#****************************************************************
#*
man_exec = DUMMY
.putenv	man_exec

.virtual	all cp cmp cmpq cmpqcp unld clean rmobj man modif
.virtual	pento penfrom pensave

all:
	-cd xinit	&& make
	-cd xterm	&& make
	-cd fvwm	&& make
	-cd xsetroot	&& make
	-cd xclock	&& make
	-cd superprobe	&& make
	-cd xredir	&& make
	-cd xtset	&& make
	-cd xpaint	&& make
	-cd xedit	&& make

cp:
	cd xinit	&& make cp
	cd xterm	&& make cp
	cd fvwm		&& make cp
	cd xsetroot	&& make cp
	cd xclock	&& make cp
	cd superprobe	&& make cp
	cd xredir	&& make cp
	cd xtset	&& make cp
	cd xpaint	&& make cp
	cd xedit	&& make cp

cmp:
	cd xinit	&& make cmp
	cd xterm	&& make cmp
	cd fvwm		&& make cmp
	cd xsetroot	&& make cmp
	cd xclock	&& make cmp
	cd superprobe	&& make cmp
	cd xredir	&& make cmp
	cd xtset	&& make cmp
	cd xpaint	&& make cmp
	cd xedit	&& make cmp

cmpq:
	cd xinit	&& make cmpq
	cd xterm	&& make cmpq
	cd fvwm		&& make cmpq
	cd xsetroot	&& make cmpq
	cd xclock	&& make cmpq
	cd superprobe	&& make cmpq
	cd xredir	&& make cmpq
	cd xtset	&& make cmpq
	cd xpaint	&& make cmpq
	cd xedit	&& make cmpq

cmpqcp:
	cd xinit	&& make cmpqcp
	cd xterm	&& make cmpqcp
	cd fvwm		&& make cmpqcp
	cd xsetroot	&& make cmpqcp
	cd xclock	&& make cmpqcp
	cd superprobe	&& make cmpqcp
	cd xredir	&& make cmpqcp
	cd xtset	&& make cmpqcp
	cd xpaint	&& make cmpqcp
	cd xedit	&& make cmpqcp

unld:
	cd xinit	&& make unld
	cd xterm	&& make unld
	cd fvwm		&& make unld
	cd xsetroot	&& make unld
	cd xclock	&& make unld
	cd superprobe	&& make unld
	cd xredir	&& make unld
	cd xtset	&& make unld
	cd xpaint	&& make unld
	cd xedit	&& make unld

clean \
rmobj:
	cd xinit	&& make rmobj
	cd xterm	&& make rmobj
	cd fvwm		&& make rmobj
	cd xsetroot	&& make rmobj
	cd xclock	&& make rmobj
	cd superprobe	&& make rmobj
	cd xredir	&& make rmobj
	cd xtset	&& make rmobj
	cd xpaint	&& make rmobj
	cd xedit	&& make rmobj

man modif:
	cd fvwm		&& make man modif
	cd xclock	&& make man modif
	cd xinit	&& make man modif
	cd xpaint	&& make man modif
#	cd xsetroot	&& make man modif
	cd xterm	&& make man modif
	cd xtset	&& make man modif
	cd superprobe	&& make man modif
#	cd xredir	&& make man modif
	cd xedit	&& make man modif

pento:
		-mount /pen
		-cmptree -sg . /pen/src/xwin/cmd/$PGM
		-forldiff /pen/src/xwin/cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen/src/xwin/cmd/$PGM &&		\
		cmptree -sg . /src/xwin/cmd/$PGM;	\
		forldiff /src/xwin/cmd/$PGM;		\
		rm -f /pen/src/xwin/cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento /src/xwin/cmd/$PGM
		-umount /pen

