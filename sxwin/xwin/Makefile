#*
#****************************************************************
#	Makefile para o Sistema Gráfico X-Window		*
#****************************************************************
#*
INCDIR	=	/usr/xwin/include
BINDIR	=	/usr/xwin/bin

man_exec = DUMMY
.putenv	man_exec

.virtual	xwin cp cpinclude cmpinclude cmp cmpq cmpqcp unld clean rmobj man

xwin:
		cd lib && make
		cd sys && make
		cd cmd && make

cp:		$BINDIR 
		cd lib && make
		cd sys && make cp
		cd cmd && make cp

cpinclude:	$INCDIR
		cptree -fvm. include $INCDIR
		walk    $INCDIR | chmod -N 0644
		walk -d $INCDIR | chmod -N 0755

cmpinclude:
		-cmptree -u include $INCDIR

cmp:
		cd sys && make cmp
		cd cmd && make cmp

cmpq:
		cd sys && make cmpq
		cd cmd && make cmpq

cmpqcp:		$BINDIR
		cd lib && make
		cd sys && make cmpqcp
		cd cmd && make cmpqcp

unld:
		cd sys && make unld
		cd cmd && make unld

install:
		cd lib; make libs; make cplibs
		make cp
		make cpinclude

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
		rm -f lib/Xt/makestrs
		cd sys && make unld
		cd cmd && make unld

man:
		cd sys && make man
		cd cmd && make man

$INCDIR:	.exist
		mkdir -f $INCDIR

$BINDIR:	.exist
		mkdir -f $BINDIR
