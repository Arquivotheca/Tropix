#*
#****************************************************************
#*	Makefile para o programa "itnet"			*
#****************************************************************
#*
PGM	= itnet
MAN	= cmd
ROOT	=
DIR	= $ROOT/usr/lib/itnet

man_exec = DUMMY
.putenv	man_exec

.virtual	compile lint n cp cmp cmpq cmpqcp unld man mani
.virtual	pento penfrom pensave

compile:
	-cd daemon;	make
#	-cd domain;	make
#	-cd echomon;	make
	-cd editscb;	make
	-cd finger;	make
	-cd ftp;	make
	-cd mail;	make
	-cd nettime;	make
	-cd ping;	make
	-cd pop3;	make
	-cd ppp;	make
	-cd rlogin;	make
	-cd rlpr;	make
	-cd telnet;	make
	-cd tnet;	make
	-cd www;	make
	-cd xpop3;	make

b:
	-cd daemon;	make b
#	-cd domain;	make b
#	-cd echomon;	make -b
	-cd editscb;	make -b
	-cd finger;	make b
	-cd ftp;	make b
	-cd mail;	make b
	-cd nettime;	make b
	-cd ping;	make -b
	-cd pop3;	make b
	-cd ppp;	make -b
	-cd rlogin;	make b
	-cd rlpr;	make -b
	-cd telnet;	make b
	-cd tnet;	make b
	-cd www;	make b
	-cd xpop3;	make -b

lint:
	-cd daemon;	make lint
#	-cd domain;	make lint
#	-cd echomon;	make lint
	-cd editscb;	make lint
	-cd finger;	make lint
	-cd ftp;	make lint
	-cd mail;	make lint
	-cd nettime;	make lint
	-cd ping;	make lint
	-cd pop3;	make lint
	-cd ppp;	make lint
	-cd rlogin;	make lint
	-cd rlpr;	make lint
	-cd telnet;	make lint
	-cd tnet;	make lint
	-cd www;	make lint
	-cd xpop3;	make lint

n:
	-cd daemon;	make -n
#	-cd domain;	make -n
#	-cd echomon;	make -n
	-cd editscb;	make -n
	-cd finger;	make -n
	-cd ftp;	make -n
	-cd mail;	make -n
	-cd nettime;	make -n
	-cd ping;	make -n
	-cd pop3;	make -n
	-cd ppp;	make -n
	-cd rlogin;	make -n
	-cd rlpr;	make -n
	-cd telnet;	make -n
	-cd tnet;	make -n
	-cd www;	make -n
	-cd xpop3;	make -n

cp:	$DIR
	-cd daemon;	make cp
#	-cd domain;	make cp
#	-cd echomon;	make cp
	-cd editscb;	make cp
	-cd finger;	make cp
	-cd ftp;	make cp
	-cd mail;	make cp
	-cd nettime;	make cp
	-cd ping;	make cp
	-cd pop3;	make cp
	-cd ppp;	make cp
	-cd rlogin;	make cp
	-cd rlpr;	make cp
	-cd telnet;	make cp
	-cd tnet;	make cp
	-cd www;	make cp
	-cd xpop3;	make cp

cmp:
	-cd daemon;	make cmp
#	-cd domain;	make cmp
#	-cd echomon;	make cmp
	-cd editscb;	make cmp
	-cd finger;	make cmp
	-cd ftp;	make cmp
	-cd mail;	make cmp
	-cd nettime;	make cmp
	-cd ping;	make cmp
	-cd pop3;	make cmp
	-cd ppp;	make cmp
	-cd rlogin;	make cmp
	-cd rlpr;	make cmp
	-cd telnet;	make cmp
	-cd tnet;	make cmp
	-cd www;	make cmp
	-cd xpop3;	make cmp

cmpq:
	-cd daemon;	make cmpq
#	-cd domain;	make cmpq
#	-cd echomon;	make cmpq
	-cd editscb;	make cmpq
	-cd finger;	make cmpq
	-cd ftp;	make cmpq
	-cd mail;	make cmpq
	-cd nettime;	make cmpq
	-cd ping;	make cmpq
	-cd pop3;	make cmpq
	-cd ppp;	make cmpq
	-cd rlogin;	make cmpq
	-cd rlpr;	make cmpq
	-cd telnet;	make cmpq
	-cd tnet;	make cmpq
	-cd www;	make cmpq
	-cd xpop3;	make cmpq

cmpqcp:
	-cd daemon;	make cmpqcp
#	-cd domain;	make cmpqcp
#	-cd echomon;	make cmpqcp
	-cd editscb;	make cmpqcp
	-cd finger;	make cmpqcp
	-cd ftp;	make cmpqcp
	-cd mail;	make cmpqcp
	-cd nettime;	make cmpqcp
	-cd ping;	make cmpqcp
	-cd pop3;	make cmpqcp
	-cd ppp;	make cmpqcp
	-cd rlogin;	make cmpqcp
	-cd rlpr;	make cmpqcp
	-cd telnet;	make cmpqcp
	-cd tnet;	make cmpqcp
	-cd www;	make cmpqcp
	-cd xpop3;	make cmpqcp

unld:
	-cd daemon;	make unld
#	-cd domain;	make unld
#	-cd echomon;	make unld
	-cd editscb;	make unld
	-cd finger;	make unld
	-cd ftp;	make unld
	-cd mail;	make unld
	-cd nettime;	make unld
	-cd ping;	make unld
	-cd pop3;	make unld
	-cd ppp;	make unld
	-cd rlogin;	make unld
	-cd rlpr;	make unld
	-cd telnet;	make unld
	-cd tnet;	make unld
	-cd www;	make unld
	-cd xpop3;	make unld

man:
	sroff man.r >$mandir/$MAN/$PGM
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/internet
	chmod a+r $mandir/$MAN/$PGM
	-cd daemon;	make man
#	-cd domain;	make man
#	-cd echomon;	make man
	-cd editscb;	make man
	-cd finger;	make man
	-cd ftp;	make man
	-cd mail;	make man
	-cd nettime;	make man
	-cd ping;	make man
	-cd pop3;	make man
	-cd ppp;	make man
	-cd rlogin;	make man
	-cd rlpr;	make man
	-cd telnet;	make man
	-cd tnet;	make man
	-cd www;	make man
	-cd xpop3;	make man

$DIR:	.exist
	mkdir -f $DIR

mani:	man.r
	sroff man.r >$mandir/$MAN/$PGM
	ln -f $mandir/$MAN/$PGM $mandir/$MAN/internet
	chmod a+r $mandir/$MAN/$PGM
	man_exec=REAL	 man $PGM

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
