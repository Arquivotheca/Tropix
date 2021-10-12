#*
#****************************************************************
#*	Makefile para o programa "mkfst1"			*
#****************************************************************
#*
DIR	= /etc
PGM	= mkfst1
APGM	= mkfs
PROT	= u+x
MAN	= cmd
CC	= cc
DEF	=

.virtual	lint cp cmp cmpq cmpqcp unld man modif

SRC	=	src/mkfst1.c	\
		src/args.c	\
	 	src/block.c	\
	 	src/dir.c	\
		src/inode.c	\
		src/map.c

OBJ	=	obj/mkfst1.o	\
		obj/args.o	\
	 	obj/block.o	\
	 	obj/dir.o	\
		obj/inode.o	\
		obj/map.o

$PGM:		obj $OBJ
		$CC $DEF -u -s -o $PGM $OBJ
		ln -f $PGM $APGM
		size $PGM $APGM
		ls -l $PGM $APGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/mkfst1.o:	src/mkfst1.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/mkfst1.c

obj/args.o:	src/args.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/args.c

obj/block.o:	src/block.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/block.c

obj/dir.o:	src/dir.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/dir.c

obj/inode.o:	src/inode.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/inode.c

obj/map.o:	src/map.c h/mkfst1.h
		$CC $DEF -c -O -o obj src/map.c

lint:
		$CC $DEF -T $SRC

cp:		$PGM
		cp -f $PGM $APGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM $DIR/$APGM

cmp:
		-cmpobj $PGM $APGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

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
