#*
#****************************************************************
#	Makefile para o cliente "xsetroot"			*
#****************************************************************
#*
PGM	=	xsetroot
DIR	=	/usr/xwin/bin
PROT	=	a=rx,u+w

.virtual	cp cmp cmpq cmpqcp unld lint clean rmobj

$PGM:		$PGM.c
		cc -us -O -o $PGM $PGM.c -lx.o
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
