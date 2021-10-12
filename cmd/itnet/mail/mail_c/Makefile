#*
#****************************************************************
#*	Makefile para o programa "mail" (cliente)		*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= mail
PGMDIR	= mail_c
PROT	= a+x
MAN	= cmd
CC	= cc
DEF	=

SRC	=	src/$PGM.c		\
		src/aux.c		\
		src/base64.c		\
	 	src/chartb.c		\
		src/letter.c		\
	 	src/mbox.c		\
	 	src/mem_alloc.c		\
	 	src/scan.c		\
	 	src/show_direc.c	\
	 	src/show_part.c		\
	 	src/show_line.c

OBJ	=	obj/$PGM.o		\
		obj/aux.o		\
		obj/base64.o		\
		obj/chartb.o		\
		obj/letter.o		\
		obj/mbox.o		\
	 	obj/mem_alloc.o		\
		obj/scan.o		\
		obj/show_direc.o	\
		obj/show_part.o		\
		obj/show_line.o

.virtual	lint			\
		lint$PGM		\
		lintaux			\
		lint64			\
	 	lintchartb		\
	 	lintletter		\
	 	lintmbox		\
	 	lintmem			\
	 	lintshowd		\
	 	lintshowp		\
	 	lintshowl		\
	 	lintscan		\
		sz

.virtual	c cp cmp cmpq cmpqcp unld man modif
.virtual	pento penfrom pensave

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir -f obj

obj/$PGM.o:	src/$PGM.c h/mail.h
		$CC $DEF -c -O -o obj src/$PGM.c

obj/aux.o:	src/aux.c h/mail.h
		$CC $DEF -c -O -o obj src/aux.c

obj/base64.o:	src/base64.c h/mail.h
		$CC $DEF -c -O -o obj src/base64.c

obj/chartb.o:	src/chartb.c h/mail.h
		$CC $DEF -c -O -o obj src/chartb.c
		size obj/chartb.o

obj/letter.o:	src/letter.c h/mail.h
		$CC $DEF -c -O -o obj src/letter.c

obj/mbox.o:	src/mbox.c h/mail.h
		$CC $DEF -c -O -o obj src/mbox.c

obj/mem_alloc.o: src/mem_alloc.c
		$CC $DEF -c -O -o obj src/mem_alloc.c

obj/scan.o:	src/scan.c h/mail.h
		$CC $DEF -c -O -o obj src/scan.c

obj/show_direc.o: src/show_direc.c h/mail.h
		$CC $DEF -c -O -o obj src/show_direc.c

obj/show_part.o: src/show_part.c h/mail.h
		$CC $DEF -c -O -o obj src/show_part.c

obj/show_line.o: src/show_line.c h/mail.h
		$CC $DEF -c -O -o obj src/show_line.c

lint:
		$CC $DEF -T $SRC

lint$PGM:
		$CC $DEF -T src/$PGM.c

lintaux:
		$CC $DEF -T src/aux.c

lint64:
		$CC $DEF -T src/base64.c

lintchartb:
		$CC $DEF -T src/chartb.c

lintletter:
		$CC $DEF -T src/letter.c

lintmbox:
		$CC $DEF -T src/mbox.c

lintmem:
		$CC $DEF -T src/mem_alloc.c

lintscan:
		$CC $DEF -T src/scan.c

lintshowd:
		$CC $DEF -T src/show_direc.c

lintshowp:
		$CC $DEF -T src/show_part.c

lintshowl:
		$CC $DEF -T src/show_line.c

sz:		prsz
		prsz

prsz:		prsz.c h/mail.h
		cc -O -o prsz prsz.c

cp:		$PGM
		cp -f $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

pento:
		-mount /pen
		-cmptree -sg . /pen$itnet/mail/$PGMDIR
		-forldiff /pen$itnet/mail/$PGMDIR
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$itnet/mail/$PGMDIR &&		\
		cmptree -sg . $itnet/mail/$PGMDIR;	\
		forldiff $itnet/mail/$PGMDIR;		\
		rm -f /pen$itnet/mail/$PGMDIR/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $itnet/mail/$PGMDIR
		-umount /pen
