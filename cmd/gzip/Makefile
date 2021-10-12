#*
#****************************************************************
#*	Makefile para o programa "gzip"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= gzip
PGM2	= gunzip
PROT	= a+x
MAN	= cmd

MATCH	= match.486

.virtual	asm.tst c cmp cp gar garS lint lintbits lintdeflate \
		lintgzip lintinflate lintmethod linttreat linttrees \
		lintunlzw lintunpack lintunzip lintutil lintzip unld \
		man modif nm oldtst prman tst0 tst1 tstall cmpq cmpqcp 

SRC	=	src/gzip.c	\
		src/treat.c	\
		src/method.c	\
		src/zip.c	\
		src/deflate.c	\
		src/trees.c	\
		src/bits.c	\
		src/unzip.c	\
		src/inflate.c	\
		src/util.c	\
		src/unlzw.c	\
		src/unpack.c

OBJ	=	obj/gzip.o	\
		obj/treat.o	\
		obj/method.o	\
		obj/zip.o	\
 		obj/deflate.o	\
		obj/trees.o	\
		obj/bits.o	\
 		obj/match.o	\
		obj/unzip.o	\
		obj/inflate.o	\
		obj/util.o	\
		obj/unlzw.o	\
		obj/unpack.o

$PGM:		obj $OBJ
		cc -s -u -o $PGM $OBJ
		size $PGM
		ln $PGM $PGM2
		ls -l $PGM

c:		obj $OBJ
		@echo Fim de c

obj:		.exist
		mkdir obj

obj/gzip.o:	src/gzip.c
		cc -c -O -o obj src/gzip.c

obj/treat.o:	src/treat.c
		cc -c -O -o obj src/treat.c

obj/method.o:	src/method.c
		cc -c -O -o obj src/method.c

obj/zip.o:	src/zip.c
		cc -c -O -o obj src/zip.c

obj/deflate.o:	src/deflate.c
		cc -D ASMV -c -O -o obj src/deflate.c

obj/trees.o:	src/trees.c
		cc -c -O -o obj src/trees.c

obj/bits.o:	src/bits.c
		cc -c -O -o obj src/bits.c

obj/unzip.o:	src/unzip.c
		cc -c -O -o obj src/unzip.c

obj/inflate.o:	src/inflate.c
		cc -c -O -o obj src/inflate.c

obj/util.o:	src/util.c
		cc -c -O -o obj src/util.c

obj/unlzw.o:	src/unlzw.c
		cc -c -O -o obj src/unlzw.c

obj/unpack.o:	src/unpack.c
		cc -c -O -o obj src/unpack.c

obj/match.o:	l src/$MATCH.s
		as -l l/$MATCH.l -o obj/match.o src/$MATCH.s

l:		.exist
		mkdir l

lint: 		lintgzip linttreat lintmethod lintzip lintdeflate	\
		linttrees lintbits lintunzip lintinflate lintutil	\
		lintunlzw lintunpack

lintgzip:
		cc -T src/gzip.c

linttreat:
		cc -T src/treat.c

lintmethod:
		cc -T src/method.c

lintzip:
		cc -T src/zip.c

lintdeflate:
		cc -D ASMV -T src/deflate.c

linttrees:
		cc -T src/trees.c

lintbits:
		cc -T src/bits.c

lintunzip:
		cc -T src/unzip.c

lintinflate:
		cc -T src/inflate.c

lintutil:
		cc -T src/util.c

lintunlzw:
		cc -T src/unlzw.c

lintunpack:
		cc -T src/unpack.c


tstall:		tst0 tst1

tst0:
		cp .tst/old.trees.c .tst/trees.c
		gzip -v .tst/trees.c
#		-cmp .tst/trees.c.gz .tst/old.trees.c.gz
		gunzip -v .tst/trees.c.gz
		-cmp .tst/trees.c .tst/old.trees.c
		rm  .tst/trees.c

tst1:
		cp h/*.h /tmp
		gzip -vr /tmp
		gunzip -vr /tmp
		-cmptree /tmp h
		rm -f /tmp/*.h

asm.tst:
		time asm.gzip -v .tst/trees.c
		-cmp .tst/trees.c.z .tst/old.trees.c.z
		time asm.gzip -d .tst/trees.c.z
		-cmp .tst/trees.c  trees.c

oldtst:
		time /usr/bin/gzip -v .tst/trees.c
		-cmp .tst/trees.c.z .tst/old.trees.c.z
		time /usr/bin/gunzip .tst/trees.c.z
		-cmp .tst/trees.c  trees.c

nm:
		nm -tn obj/*.o | show

cp:		$PGM
		cp -f $PGM $DIR
		ln $DIR/$PGM $DIR/$PGM2
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$PGM2

cmp:
		-cmpobj $PGM $DIR
		-cmpobj $DIR/$PGM $DIR/$PGM2

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM $PGM2

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

garS:
		gar -avs0S .
		gar -icv0
