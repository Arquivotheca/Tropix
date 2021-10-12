#*
#****************************************************************
#	makefile para o programa "FvwmPager"			*
#****************************************************************
#*
PGM	=	FvwmPager
DIR	=	/usr/xwin/bin
PROT	=	a=rx,u+w

.virtual	cp cmp cmpq cmpqcp unld lint

$PGM:		$PGM.o x_pager.o
		cc -s -o $PGM $PGM.o x_pager.o ../lib -lx.o
		size $PGM

$PGM.o:		$PGM.c					\
		../global.h				\
		../fvwm/h/module.h			\
		../libs/fvwmlib.h			\
		FvwmPager.h				\
		../fvwm/h/fvwm.h	
		cc -O -c $PGM.c

x_pager.o:	x_pager.c				\
		../global.h				\
		../fvwm/h/fvwm.h			\
		FvwmPager.h	
		cc -O -c x_pager.c

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

lint:		$PGM.c x_pager.c
		cc -T $PGM.c x_pager.c
