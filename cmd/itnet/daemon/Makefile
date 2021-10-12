#*
#****************************************************************
#*	Makefile para os "daemons" da ITNET			*
#****************************************************************
#*
PGM	= itnet/daemon

.virtual	compile lint n cp cmp cmpq cmpqcp unld man
.virtual	pento penfrom pensave

compile:
	cd itnet_daemon;	make
	cd name_daemon;		make
	cd serv_daemon;		make

b:
	cd itnet_daemon;	make -b
	cd name_daemon;		make -b
	cd serv_daemon;		make -b

lint:
	cd itnet_daemon;	make lint
	cd name_daemon;		make lint
	cd serv_daemon;		make lint

n:
	cd itnet_daemon;	make -n
	cd name_daemon;		make -n
	cd serv_daemon;		make -n

cp:
	cd itnet_daemon;	make cp
	cd name_daemon;		make cp
	cd serv_daemon;		make cp

cmp:
	cd itnet_daemon;	make cmp
	cd name_daemon;		make cmp
	cd serv_daemon;		make cmp

cmpq:
	cd itnet_daemon;	make cmpq
	cd name_daemon;		make cmpq
	cd serv_daemon;		make cmpq

cmpqcp:
	cd itnet_daemon;	make cmpqcp
	cd name_daemon;		make cmpqcp
	cd serv_daemon;		make cmpqcp

unld:
	cd itnet_daemon;	make unld
	cd name_daemon;		make unld
	cd serv_daemon;		make unld

man:
#	cd itnet_daemon;	make man
#	cd name_daemon;		make man
	cd serv_daemon;		make man

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

