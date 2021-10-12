#*
#****************************************************************
#*	Makefile para o programa "fsckt1"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= fsckt1
APGM	= fsck
PROT	= u+x
MAN	= cmd
CC	= cc

SRC	=	src/fsckt1.c	\
		src/phase1.c	\
		src/phase2.c	\
		src/phase3.c	\
		src/phase4.c	\
		src/phase5.c	\
		src/phase6.c	\
		src/check.c	\
		src/block.c	\
		src/sub.c

OBJ	=	obj/fsckt1.o	\
		obj/phase1.o	\
		obj/phase2.o	\
		obj/phase3.o	\
		obj/phase4.o	\
		obj/phase5.o	\
		obj/phase6.o	\
		obj/check.o	\
		obj/block.o	\
		obj/sub.o

.virtual	c lint cp cmp cmpq cmpqcp unld man modif prman
.virtual	lintb lintc lints lint1 lint2 lint3 lint4 lint5 lint6
.virtual	pento penfrom pensave

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		ln -f $PGM $APGM
		size $PGM $APGM
		ls -l $PGM $APGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/fsckt1.o:	src/fsckt1.c h/fsckt1.h
		$CC -c -O -o obj src/fsckt1.c

obj/block.o:	src/block.c h/fsckt1.h
		$CC -c -O -o obj src/block.c

obj/check.o:	src/check.c h/fsckt1.h
		$CC -c -O -o obj src/check.c

obj/phase1.o:	src/phase1.c h/fsckt1.h
		$CC -c -O -o obj src/phase1.c

obj/phase2.o:	src/phase2.c h/fsckt1.h
		$CC -c -O -o obj src/phase2.c

obj/phase3.o:	src/phase3.c h/fsckt1.h
		$CC -c -O -o obj src/phase3.c

obj/phase4.o:	src/phase4.c h/fsckt1.h
		$CC -c -O -o obj src/phase4.c

obj/phase5.o:	src/phase5.c h/fsckt1.h
		$CC -c -O -o obj src/phase5.c

obj/phase6.o:	src/phase6.c h/fsckt1.h
		$CC -c -O -o obj src/phase6.c

obj/sub.o:	src/sub.c h/fsckt1.h
		$CC -c -O -o obj src/sub.c

lint:
		$CC -T $SRC

lintb:
		$CC -T src/block.c

lintc:
		$CC -T src/check.c

lint1:
		$CC -T src/phase1.c

lint2:
		$CC -T src/phase2.c

lint3:
		$CC -T src/phase3.c

lint4:
		$CC -T src/phase4.c

lint5:
		$CC -T src/phase5.c

lint6:
		$CC -T src/phase6.c

lints:
		$CC -T src/sub.c

eddir:		src/eddir.c
		$CC -O -o eddir src/eddir.c

edino:		src/edino.c
		$CC -O -o edino src/edino.c

cp:		$PGM
		cp -f $PGM $APGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$APGM

cmp:
		-cmpobj $PGM $APGM $DIR

cmpq:
		-cmpobj -q $PGM $APGM $DIR

cmpqcp:
		-cmpobj -q $PGM $APGM $DIR || make cp

unld:
		-rm -f $PGM $APGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$APGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		ln -f $mandir/modif/$MAN/$PGM $mandir/modif/$MAN/$APGM
		modif $PGM

prman:
		sroff man.r | lpr

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
