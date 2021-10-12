#*
#****************************************************************
#	Makefile para o cliente "xclock"			*
#****************************************************************
#*
PGM	=	xclock
DIR	=	/usr/xwin/bin
MAN	=	xwin
PROT	=	a=rx,u+w

.virtual	cp cmp cmpq cmpqcp unld lint man modif clean rmobj

$PGM:		$PGM.c
		cc -O -us -o $PGM $PGM.c -lx.o
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

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
