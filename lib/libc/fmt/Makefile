#*
#****************************************************************
#*	Makefile para a biblioteca "fmt"			*
#****************************************************************
#*
man_exec = DUMMY
PGM	 = fmt

.putenv man_exec

.virtual	n man clr pento penfrom pensave

n:
	cd a.out;	make -n
	cd cmd;		make -n
	cd environ;	make -n
	cd exports;	make -n
	cd gar;		make -n
	cd group;	make -n
	cd initab;	make -n
	cd iso;		make -n
	cd itnetdev;	make -n
	cd itnetserv;	make -n
	cd lib;		make -n
	cd nfs;		make -n
	cd passwd;	make -n
	cd rc;		make -n
	cd scb;		make -n
	cd termio;	make -n

man:
	cd a.out;	make man
	cd cmd;		make man
	cd environ;	make man
	cd exports;	make man
	cd gar;		make man
	cd group;	make man
	cd initab;	make man
	cd iso;		make man
	cd itnetdev;	make man
	cd itnetserv;	make man
	cd lib;		make man
	cd nfs;		make man
	cd passwd;	make man
	cd rc;		make man
	cd scb;		make man
	cd termio;	make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

pento:
		-mount /pen
		-cmptree -sg . /pen$libc/$PGM
		-forldiff /pen$libc/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$libc/$PGM &&		\
		cmptree -sg . $libc/$PGM;	\
		forldiff $libc/$PGM;		\
		rm -f /pen$libc/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $libc/$PGM
		-umount /pen
