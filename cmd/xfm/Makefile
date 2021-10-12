#*
#****************************************************************
#*	Makefile para o programa "xfm"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= xfm
PROT	= u+s,g+s,a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	= src/cmp.c		\
	  src/cp.c		\
	  src/df.c		\
	  src/dir.c		\
	  src/du.c		\
	  src/filemenu.c	\
	  src/fstab.c		\
	  src/key.c		\
	  src/mkdir.c		\
	  src/mount.c		\
	  src/msg.c		\
	  src/mv.c		\
	  src/node.c		\
	  src/rename.c		\
	  src/rm.c		\
	  src/scroll.c		\
	  src/tree.c		\
	  src/$PGM.c		\
	  src/xpm.c		\
	  src/zip.c

OBJ	= obj/cmp.o		\
	  obj/cp.o		\
	  obj/df.o		\
	  obj/dir.o		\
	  obj/du.o		\
	  obj/filemenu.o	\
	  obj/fstab.o		\
	  obj/key.o		\
	  obj/mkdir.o		\
	  obj/mount.o		\
	  obj/msg.o		\
	  obj/mv.o		\
	  obj/node.o		\
	  obj/rename.o		\
	  obj/rm.o		\
	  obj/scroll.o		\
	  obj/tree.o		\
	  obj/$PGM.o		\
	  obj/xpm.o		\
	  obj/zip.o


.virtual	c lint
.virtual	lintx lintt lintp lintm lintf lintk lintc lintcp lintmv lintfs lintd
.virtual	lintmnt lintr lintmkdir lintrename lintdu lints lintdir lintnode lintz
.virtual	zipto zipfrom
.virtual	cp cmp cmpq cmpqcp unld man modif
.virtual	pento penfrom pensave

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ -lx.o
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/cmp.o:	src/cmp.c h/xfm.h
		$CC $DEF -c -O -o obj src/cmp.c

obj/cp.o:	src/cp.c h/xfm.h
		$CC $DEF -c -O -o obj src/cp.c

obj/df.o:	src/df.c h/xfm.h
		$CC $DEF -c -O -o obj src/df.c

obj/dir.o:	src/dir.c h/xfm.h
		$CC $DEF -c -O -o obj src/dir.c

obj/du.o:	src/du.c h/xfm.h
		$CC $DEF -c -O -o obj src/du.c

obj/filemenu.o:	src/filemenu.c h/xfm.h
		$CC $DEF -c -O -o obj src/filemenu.c

obj/fstab.o:	src/fstab.c h/xfm.h
		$CC $DEF -c -O -o obj src/fstab.c

obj/key.o:	src/key.c h/xfm.h
		$CC $DEF -c -O -o obj src/key.c

obj/mkdir.o:	src/mkdir.c h/xfm.h
		$CC $DEF -c -O -o obj src/mkdir.c

obj/mount.o:	src/mount.c h/xfm.h
		$CC $DEF -c -O -o obj src/mount.c

obj/msg.o:	src/msg.c h/xfm.h
		$CC $DEF -c -O -o obj src/msg.c

obj/mv.o:	src/mv.c h/xfm.h
		$CC $DEF -c -O -o obj src/mv.c

obj/node.o:	src/node.c h/xfm.h
		$CC $DEF -c -O -o obj src/node.c

obj/rename.o:	src/rename.c h/xfm.h
		$CC $DEF -c -O -o obj src/rename.c

obj/rm.o:	src/rm.c h/xfm.h
		$CC $DEF -c -O -o obj src/rm.c

obj/scroll.o:	src/scroll.c h/xfm.h
		$CC $DEF -c -O -o obj src/scroll.c

obj/tree.o:	src/tree.c h/xfm.h
		$CC $DEF -c -O -o obj src/tree.c

obj/$PGM.o:	src/$PGM.c h/xfm.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/xpm.o:	src/xpm.c h/xfm.h
		$CC $DEF -c -O -o obj src/xpm.c

obj/zip.o:	src/zip.c h/xfm.h
		$CC $DEF -c -O -o obj src/zip.c

lint:
		$CC $DEF -T $SRC

lintx:
		$CC $DEF -T src/$PGM.c

lintt:
		$CC $DEF -T src/tree.c

lintp:
		$CC $DEF -T src/xpm.c

lintf:
		$CC $DEF -T src/filemenu.c

lintk:
		$CC $DEF -T src/key.c

lintm:
		$CC $DEF -T src/msg.c

lintc:
		$CC $DEF -T src/cmp.c

lintcp:
		$CC $DEF -T src/cp.c

lintmv:
		$CC $DEF -T src/mv.c

lintfs:
		$CC $DEF -T src/fstab.c

lintd:
		$CC $DEF -T src/df.c

lintmnt:
		$CC $DEF -T src/mount.c

lintr:
		$CC $DEF -T src/rm.c

lintmkdir:
		$CC $DEF -T src/mkdir.c

lintrename:
		$CC $DEF -T src/rename.c

lintdu:
		$CC $DEF -T src/du.c

lints:
		$CC $DEF -T src/scroll.c

lintdir:
		$CC $DEF -T src/dir.c

lintnode:
		$CC $DEF -T src/node.c

lintz:
		$CC $DEF -T src/zip.c

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

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

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield

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
