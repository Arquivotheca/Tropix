#*
#****************************************************************
#	Makefile para o programa "xredir"			*
#****************************************************************
#*
PGM	=	xredir
DIR	=	/usr/xwin/bin
PROT	=	u=srwx,g+srx,o+rx

.virtual	cp cmp cmpq cmpqcp unld lint clean rmobj

$PGM:		$PGM.c
		cc -us -O -o $PGM $PGM.c
		size $PGM

cp:		$PGM $DIR
		cp -f $PGM $DIR/$PGM
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

$DIR:		.exist
		mkdir -f $DIR

cmp:
		-cmpobj $PGM $DIR/$PGM

cmpq:
		-cmpobj -q $PGM $DIR/$PGM

cmpqcp:
		-cmpobj -q $PGM $DIR/$PGM  ||  make cp

unld:
		-rm -f $PGM

lint:		$PGM.c
		cc -T $PGM.c

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
