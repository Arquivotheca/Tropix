#*
#****************************************************************
#*	Makefile para a biblioteca "libc"			*
#****************************************************************
#*
.virtual	libc n lint cp cmp cmpqcp man cmpinc lib clean rmobj
.virtual	pento penfrom pensave

libc:
	cd crt;		make
	cd etc;		make
#	cd sid;		make
#	cd id;		make
#	cd lid;		make
	cd mem;		make
	cd pwd;		make
	cd stdio;	make
	cd str;		make
	cd sys;		make
	cd tb;		make

n:
	cd crt;		make n
	cd etc;		make n
#	cd sid;		make n
	cd fmt;		make n
#	cd id;		make n
#	cd lid;		make n
	cd mem;		make n
	cd pwd;		make n
	cd stdio;	make n
	cd str;		make n
	cd sys;		make n
	cd tb;		make n

lint:
	cd crt;		make lint
	cd etc;		make lint
#	cd sid;		make lint
	cd fmt;		make lint
#	cd id;		make lint
#	cd lid;		make lint
	cd mem;		make lint
	cd pwd;		make lint
	cd stdio;	make lint
	cd str;		make lint
	cd sys;		make lint
	cd tb;		make lint

cp:
	-mkdir /lib/libc
	cd crt;		make cp
	cd etc;		make cp
#	cd sid;		make cp
#	cd id;		make cp
#	cd lid;		make cp
	cd mem;		make cp
	cd pwd;		make cp
	cd stdio;	make cp
	cd str;		make cp
	cd sys;		make cp
	cd tb;		make cp
	mklib /lib/libc

cmp:
	cd crt;		make cmp
	cd etc;		make cmp
#	cd sid;		make cmp
#	cd id;		make cmp
#	cd lid;		make cmp
	cd mem;		make cmp
	cd pwd;		make cmp
	cd stdio;	make cmp
	cd str;		make cmp
	cd sys;		make cmp
	cd tb;		make cmp
	mklib -k /lib/libc

cmpqcp:
	cd crt;		make cmpqcp
	cd etc;		make cmpqcp
#	cd sid;		make cmpqcp
#	cd id;		make cmpqcp
#	cd lid;		make cmpqcp
	cd mem;		make cmpqcp
	cd pwd;		make cmpqcp
	cd stdio;	make cmpqcp
	cd str;		make cmpqcp
	cd sys;		make cmpqcp
	cd tb;		make cmpqcp
	mklib -k /lib/libc

man:
	cd crt;		make man
	cd etc;		make man
#	cd sid;		make man
	cd fmt;		make man
#	cd id;		make man
#	cd lid;		make man
	cd mem;		make man
	cd pwd;		make man
	cd stdio;	make man
	cd str;		make man
	cd sys;		make man
	cd tb;		make man

cmpinc:
	-cmp -l etc/assert/assert.h		$inc
	-cmp -l etc/ctype/ctype.h		$inc
	-cmp -l etc/dirent/dirent.h		$inc
	-cmp -l etc/ftw/ftw.h			$inc
	-cmp -l etc/locale/locale.h		$inc
	-cmp -l etc/mkasmtb/mkasmtb.h		$inc
	-cmp -l etc/nlist/nlist.h		$inc
	-cmp -l etc/setjmp/setjmp.h		$inc
	-cmp -l mem/smalloc/malloc.h		$inc
	-cmp -l pwd/grent/grp.h			$inc
	-cmp -l pwd/pwent/pwd.h			$inc
	-cmp -l stdio/stdio/stdio.h		$inc
	-cmp -l stdio/vprintf/stdarg.h		$inc
	-cmp -l sys/fcntl/fcntl.h		$inc
	-cmp -l sys/lseek/unistd.h		$inc
	-cmp -l sys/pcntl/pcntl.h		$inc/sys
	-cmp -l sys/signal/signal.h		$inc
	-cmp -l sys/stat/stat.h			$inc/sys

lib:
	mklib /lib/libc

clean:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

rmobj:
	walk -p "*.o" . | rm

pento:
		-mount /pen
		-cmptree -sg . /pen$libc
		-forldiff /pen$libc
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$libc &&		\
		cmptree -sg . $libc;	\
		forldiff $libc;		\
		rm -f /pen$libc/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $libc
		-umount /pen
