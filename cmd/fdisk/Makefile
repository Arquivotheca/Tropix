#*
#****************************************************************
#*	Makefile para o programa "fdisk"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/etc
PGM	= fdisk
PROT	= u+rx
MAN	= cmd
CC	= cc
DEF	= -D SB_ENDIAN_CV

.virtual	lint lintf lintg cp cmp cmpq cmpqcp 
.virtual	unld man modif prman zipto zipfrom pento penfrom pensave

$PGM:		obj obj/$PGM.o obj/geo.o
		$CC $DEF -u -s -o $PGM obj/$PGM.o obj/geo.o
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

obj/$PGM.o:	src/$PGM.c
		$CC $DEF -O -c -o obj src/$PGM.c

obj/geo.o:	src/geo.c
		$CC $DEF -O -c -o obj src/geo.c

lint:
		$CC $DEF -T src/$PGM.c src/geo.c

lintf:
		$CC $DEF -T src/$PGM.c

lintg:
		$CC $DEF -T src/geo.c

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
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
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

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield
