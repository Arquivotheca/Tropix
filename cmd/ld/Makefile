#*
#****************************************************************
#*	Makefile para o programa "ld"				*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/bin
LDIR	= .
PGM	= ld
PROT	= a+x,u+m
MAN	= cmd
CC	= cc
DEF	=
LIB0	= -lc -lm
LIB1	= -lt.o

SRC	=	src/ld.c	\
		src/pass1.c	\
		src/pass2.c	\
		src/pass3.c	\
		src/pass4.c	\
		src/pass5.c	\
		src/pass6.c	\
		src/pass7.c	\
		src/sub.c

OBJ	=	obj/ld.o	\
		obj/pass1.o	\
		obj/pass2.o	\
		obj/pass3.o	\
		obj/pass4.o	\
		obj/pass5.o	\
		obj/pass6.o	\
		obj/pass7.o	\
		obj/sub.o

NEW_OBJ	=	new.obj/ld.o	\
		new.obj/pass1.o	\
		new.obj/pass2.o	\
		new.obj/pass3.o	\
		new.obj/pass4.o	\
		new.obj/pass5.o	\
		new.obj/pass6.o	\
		new.obj/pass7.o	\
		new.obj/sub.o

.virtual	unld lint lintl lint1 lint2 lint3 lint4 lint5 lint6 lint7 lints
.virtual	m alltst tst tstr prsymcp prsymcmp cp cmp cmpq cmpqcp
.virtual	man modif prman gar gara tst.tst.ld clean

$PGM:		obj $OBJ
		$CC $DEF -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

unld:
		rm -f ld

alltst:		obj $OBJ tst tstr tst.$PGM tst.tst.$PGM

tst.$PGM:	obj $OBJ
		./ld -o tst.$PGM -n /lib/crt0.o $OBJ $(LIB$CC_SHLIB)
		size tst.$PGM
		ls -l tst.$PGM

tst.tst.$PGM:	obj $OBJ
		./tst.ld -o tst.tst.$PGM -n /lib/crt0.o $OBJ $(LIB$CC_SHLIB)
		size tst.tst.$PGM
		ls -l tst.tst.$PGM
		cmpobj tst.tst.$PGM tst.$PGM
		rm -f tst.tst.$PGM

obj:		.exist
		mkdir obj

obj/ld.o:	src/ld.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/ld.c

obj/pass1.o:	src/pass1.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass1.c

obj/pass2.o:	src/pass2.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass2.c

obj/pass3.o:	src/pass3.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass3.c

obj/pass4.o:	src/pass4.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass4.c

obj/pass5.o:	src/pass5.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass5.c

obj/pass6.o:	src/pass6.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass6.c

obj/pass7.o:	src/pass7.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/pass7.c

obj/sub.o:	src/sub.c	\
		h/ld.h
		$CC $DEF -c -O -o obj src/sub.c

lint:
		$CC $DEF -T $SRC

lintl:
		$CC $DEF -T src/ld.c

lint1:
		$CC $DEF -T src/pass1.c

lint2:
		$CC $DEF -T src/pass2.c

lint3:
		$CC $DEF -T src/pass3.c

lint4:
		$CC $DEF -T src/pass4.c

lint5:
		$CC $DEF -T src/pass5.c

lint6:
		$CC $DEF -T src/pass6.c

lint7:
		$CC $DEF -T src/pass7.c

lints:
		$CC $DEF -T src/sub.c

#*
#******	Teste de biblioteca compartilhada ***********************
#*
t:		t.c s
		cc -Oc t.c
		./ld -n -o t /lib/crt0.o t.o -lt.o
		size t

s:		s.c
		cc -Oc s.c
		./ld -n -o s /lib/crt0.o s.o -lt.o
		size s

#*
#******	Teste do "ld" *******************************************
#*
tst:		$OBJ
		./ld -o nld -n -s -u /lib/crt0.o $OBJ $(LIB$CC_SHLIB)
		ls -l ld nld
		size ld nld
		-cmpobj ld nld
		rm -f nld

#*
#******	Teste do "-m" *******************************************
#*
m:		$OBJ
		./ld -m -o nld -n -s -u /lib/crt0.o $OBJ $(LIB$CC_SHLIB)
		ls -l ld nld
		size ld nld
		-cmpobj ld nld
		rm -f nld

#*
#******	Teste do "-r" *******************************************
#*
OBJ1	=	obj/ld.o

OBJ2	=	obj/pass1.o		\
		obj/pass2.o		\
		obj/pass3.o		\
		obj/pass4.o

OBJ3	=	obj/pass5.o		\
		obj/pass6.o		\
		obj/pass7.o

OBJ4	=	obj/sub.o

tstr:		$OBJ1 $OBJ2 $OBJ3 $OBJ4
		./ld -r -o a.out obj/pass3.o
		size -c a.out obj/pass3.o
		-cmpobj a.out obj/pass3.o
		rm -f a.out
#
		./ld -r -o mod1.o $OBJ2
		./ld -r -o mod2.o $OBJ3
		./ld -o nld -n -s -u /lib/crt0.o $OBJ1 mod1.o mod2.o $OBJ4 $(LIB$CC_SHLIB)
		size -c ld nld
		-cmpobj ld nld
		rm -f nld mod?.o

#*
#****** Programa "prsym" ****************************************
#*
prsym:		src/prsym.c
		$CC $DEF -O -u -s -o prsym src/prsym.c

prsymcp:	prsym
		cp -f prsym $ROOT/usr/pub/bin

prsymcmp:		
		-cmpobj prsym $ROOT/usr/pub/bin

#*
#******	Geração das bibliotecas *********************************
#*/
.virtual	libs	cmplibs	cplibs
.virtual	rmlibt	cplibt	cmplibt	cmplibtq shiftlibt
.virtual	rmlibx	cplibx	cmplibx	cmplibxq
.virtual	rmliby	cpliby	cmpliby	cmplibyq

libs: 		libt.o libx.o liby.o

cmplibs: 	cmplibtq cmplibxq cmplibyq

cplibs: 	cplibt cplibx cpliby

rmlibt:
		rm -f libt.o

libt.o:		/lib/libc/.LIBSYM /lib/libm/.LIBSYM /lib/libxti/.LIBSYM /lib/libcurses/.LIBSYM
		$LDIR/$PGM -b -o libt.o /lib/libc /lib/libm /lib/libxti /lib/libcurses
		size libt.o

cplibt:		libt.o
		cp libt.o $ROOT/lib/new.libt.o

cmplibt:	
		-cmpobj libt.o $ROOT/lib/old.libt.o
		-cmpobj libt.o $ROOT/lib/libt.o
		-cmpobj libt.o $ROOT/lib/new.libt.o

cmplibtq:	
		-cmpobj libt.o $ROOT/lib/libt.o

shiftlibt:
		-mv $ROOT/lib/libt.o $ROOT/lib/old.libt.o
		-mv $ROOT/lib/new.libt.o $ROOT/lib/libt.o

rmlibx:
		rm -f libx.o

libx.o:		/usr/xwin/lib/libX11/.LIBSYM		\
		/usr/xwin/lib/libaux/.LIBSYM		\
		/usr/xwin/lib/libxaux/.LIBSYM		\
		/usr/xwin/lib/libxpm/.LIBSYM
		$LDIR/$PGM -b -o libx.o			\
			/usr/xwin/lib/libX11		\
			/usr/xwin/lib/libaux		\
			/usr/xwin/lib/libxaux		\
			/usr/xwin/lib/libxpm
		size libx.o

cplibx:		libx.o 
		-mv $ROOT/usr/xwin/lib/libx.o $ROOT/usr/xwin/lib/old.libx.o
		cp libx.o $ROOT/usr/xwin/lib

cmplibx:	
		-cmpobj libx.o $ROOT/usr/xwin/lib/old.libx.o
		-cmpobj libx.o $ROOT/usr/xwin/lib/libx.o

cmplibxq:	
		-cmpobj libx.o $ROOT/usr/xwin/lib/libx.o

rmliby:
		rm -f liby.o

liby.o: 		/usr/xwin/lib/libICE/.LIBSYM	\
			/usr/xwin/lib/libXaw/.LIBSYM	\
			/usr/xwin/lib/libXext/.LIBSYM	\
			/usr/xwin/lib/libXmu/.LIBSYM	\
			/usr/xwin/lib/libXt/.LIBSYM	\
			/usr/xwin/lib/libjpeg/.LIBSYM
		$LDIR/$PGM -b -o liby.o			\
			/usr/xwin/lib/libICE		\
			/usr/xwin/lib/libXaw		\
			/usr/xwin/lib/libXext		\
			/usr/xwin/lib/libXmu		\
			/usr/xwin/lib/libXt		\
			/usr/xwin/lib/libjpeg
		size liby.o

cpliby:		liby.o 
		-mv $ROOT/usr/xwin/lib/liby.o $ROOT/usr/xwin/lib/old.liby.o
		cp liby.o $ROOT/usr/xwin/lib

cmpliby:	
		-cmpobj liby.o $ROOT/usr/xwin/lib/old.liby.o
		-cmpobj liby.o $ROOT/usr/xwin/lib/liby.o

cmplibyq:	
		-cmpobj liby.o $ROOT/usr/xwin/lib/liby.o

#*
#******	Cópia, comparações, ... *********************************
#*
cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
#		$DIR/$PGM -M
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		chmod a+r $mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0

clean:
		rm -f mod.o nld s s.o t t.o
