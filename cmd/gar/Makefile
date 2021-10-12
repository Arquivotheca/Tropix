#*
#****************************************************************
#*	Makefile para o programa "gar"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= gar
PROT	= a+rx
MAN	= cmd
CC	= cc
DEF	= -D PC
MATCH	= pc.match

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman g gara
.virtual	pento penfrom pensave

SRC	=	src/gar.c		\
		src/in.c		\
		src/tok.c		\
		src/out.c		\
		src/append.c		\
		src/aux.c		\
#		reduce/reduce0.c	\
#		reduce/reduce1.c	\
#		reduce/reduce2.c 	\
#		reduce/reduce3.c	\
		deflate/deflate.c	\
		deflate/inflate.c	\
		deflate/util.c		\
		deflate/trees.c		\
		deflate/bits.c		\
		deflate/$MATCH.s

OBJ	= 	obj/gar.o		\
		obj/in.o 		\
		obj/tok.o 		\
		obj/out.o 		\
		obj/append.o 		\
		obj/aux.o 		\
#		obj/reduce0.o 		\
#		obj/reduce1.o 		\
#		obj/reduce2.o 		\
#		obj/reduce3.o		\
		obj/deflate.o		\
		obj/inflate.o		\
		obj/util.o		\
		obj/trees.o		\
		obj/bits.o		\
		obj/$MATCH.o

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

$PGM\s:		obj $OBJ
		$CC -u -o $PGM\s $OBJ
		size $PGM\s
		ls -l $PGM\s

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/snblock.o:	$sys/sn/psnset/snblock.c
		cp $sys/sn/psnset/snblock.o obj

obj/chksn.o:	$sys/sn/psnset/chksn.c
		cp $sys/sn/psnset/chksn.o obj

obj/gar.o:	src/gar.c h/common.h h/gar.h
		$CC $DEF -c -O -o obj src/gar.c

obj/in.o:	src/in.c h/common.h h/gar.h
		$CC $DEF -c -O -o obj src/in.c

obj/tok.o:	src/tok.c h/common.h h/gar.h h/tar.h h/cpio.h
		$CC $DEF -c -O -o obj src/tok.c

obj/out.o:	src/out.c h/common.h h/gar.h h/tar.h
		$CC $DEF -c -O -o obj src/out.c

obj/append.o:	src/append.c h/common.h h/gar.h
		$CC $DEF -c -O -o obj src/append.c

obj/aux.o:	src/aux.c h/common.h h/gar.h h/tar.h
		$CC $DEF -c -O -o obj src/aux.c

#obj/reduce0.o:	reduce/reduce0.c h/reduce.h
#		$CC $DEF -c -O -o obj reduce/reduce0.c
#
#obj/reduce1.o:	reduce/reduce1.c h/common.h h/reduce.h
#		$CC $DEF -c -O -o obj reduce/reduce1.c
#
#obj/reduce2.o:	reduce/reduce2.c h/common.h h/gar.h h/reduce.h
#		$CC $DEF -c -O -o obj reduce/reduce2.c
#
#obj/reduce3.o:	reduce/reduce3.c h/reduce.h
#		$CC $DEF -c -O -o obj reduce/reduce3.c

obj/deflate.o:	deflate/deflate.c h/common.h h/deflate.h
		$CC $DEF -c -O -o obj deflate/deflate.c

obj/inflate.o:	deflate/inflate.c h/common.h h/deflate.h
		$CC $DEF -c -O -o obj deflate/inflate.c

obj/util.o:	deflate/util.c h/common.h h/deflate.h
		$CC $DEF -c -O -o obj deflate/util.c

obj/trees.o:	deflate/trees.c h/common.h h/deflate.h
		$CC $DEF -c -O -o obj deflate/trees.c

obj/bits.o:	deflate/bits.c	h/common.h h/deflate.h
		$CC $DEF -c -O -o obj deflate/bits.c

obj/$MATCH.o:	deflate/$MATCH.s
		$CC $DEF -c -o obj deflate/$MATCH.s

lint:		$SRC
		$CC $DEF -T $SRC

cp:		$PGM
		cp -f $PGM $DIR/gar
		chmod $PROT $DIR/gar
		ls -L $DIR/gar

cmp:
		-cmpobj $PGM $DIR/gar

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

g:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0

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
