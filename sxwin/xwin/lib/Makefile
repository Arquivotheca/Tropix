#*
#****************************************************************
#	Makefile para as Bibliotecas do X-Window		*
#****************************************************************
#*
.virtual	all links clean rmobj

all:
	cd os   && make
	cd font	&& make
	cd zlib && make
	cd xpm	&& make
	cd jpeg && make
	cd X11	&& make
	cd Xmu  && make
	cd Xt   && make
	cd Xaw  && make
	cd ICE  && make
	cd Xext && make

#	cd Xau	&& make

links:
	cd ICE	&& make links
	cd SM	&& make links
	cd X11	&& make links
	cd Xau	&& make links
	cd Xaw	&& make links
	cd Xmu	&& make links
	cd Xt	&& make links
	cd jpeg	&& make links
	cd xpm	&& make links

clean \
rmobj:
	walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f

#*
#******	Geração das bibliotecas compartilhadas ******************
#*/
LIBDIR	=	/usr/xwin/lib

.virtual	libs	cmplibs	cplibs
.virtual	rmlibx	cplibx	cmplibx	cmplibxq
.virtual	rmliby	cpliby	cmpliby	cmplibyq

libs: 		libx.o liby.o

cmplibs: 	cmplibxq cmplibyq

cplibs: 	cplibx cpliby

rmlibx:
		rm -f libx.o

libx.o:		X11/lib/.LIBSYM			\
		os/lib/.LIBSYM			\
		xpm/lib/.LIBSYM
		ld -b -o libx.o			\
			X11/lib			\
			os/lib			\
			xpm/lib
		size libx.o

cplibx:		$LIBDIR libx.o 
		-mv $LIBDIR/libx.o $LIBDIR/old.libx.o
		cp libx.o $LIBDIR

cmplibx:	
		-cmpobj libx.o $LIBDIR/old.libx.o
		-cmpobj libx.o $LIBDIR/libx.o

cmplibxq:	
		-cmpobj libx.o $LIBDIR/libx.o

rmliby:
		rm -f liby.o

liby.o: 	ICE/lib/.LIBSYM			\
		Xaw/lib/.LIBSYM			\
		Xext/lib/.LIBSYM		\
		Xmu/lib/.LIBSYM			\
		Xt/lib/.LIBSYM			\
		jpeg/lib/.LIBSYM
		ld -b -o liby.o			\
			ICE/lib			\
			Xaw/lib			\
			Xext/lib		\
			Xmu/lib			\
			Xt/lib			\
			jpeg/lib
		size liby.o

cpliby:		$LIBDIR liby.o 
		-mv $LIBDIR/liby.o $LIBDIR/old.liby.o
		cp liby.o $LIBDIR

cmpliby:	
		-cmpobj liby.o $LIBDIR/old.liby.o
		-cmpobj liby.o $LIBDIR/liby.o

cmplibyq:	
		-cmpobj liby.o $LIBDIR/liby.o

$LIBDIR:	.exist
		mkdir -f $LIBDIR
