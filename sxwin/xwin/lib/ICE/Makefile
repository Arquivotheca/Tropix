#*
#****************************************************************
#*	Makefile para a biblioteca ICE				*
#****************************************************************
#*
LIB	=	lib

DEF	=	-D CSRG_BASED		\
		-D asm=__asm		\
		-D entry=_entry_	\
		-D TROPIX

INC	=	-I .			\
		-I ../../include	\
		-I ../../include/X11	\
		-I ../../include/X11/ICE

CC	=	cc -c -O -o $@

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
OBJ	=	$LIB/accept.o		\
		$LIB/authutil.o		\
		$LIB/connect.o		\
		$LIB/error.o		\
		$LIB/getauth.o		\
		$LIB/iceauth.o		\
		$LIB/listen.o		\
		$LIB/locking.o		\
		$LIB/misc.o		\
		$LIB/ping.o		\
		$LIB/process.o		\
		$LIB/protosetup.o	\
		$LIB/register.o		\
		$LIB/replywait.o	\
		$LIB/setauth.o		\
		$LIB/shutdown.o		\
		$LIB/transport.o	\
		$LIB/watch.o

$LIB/.LIBSYM:	$LIB $OBJ
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

.virtual	links

links: 		../../include/X11/ICE
		ln -f			\
			ICE.h		\
			ICEconn.h	\
			ICElib.h	\
			ICElibint.h	\
			ICEmsg.h	\
			ICEproto.h	\
			ICEutil.h	\
		../../include/X11/ICE

../../include/X11/ICE:	.exist
			mkdir -f ../../include/X11/ICE

#*
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#*
$LIB/accept.o:		accept.c
			$CC $INC $DEF $INC accept.c

$LIB/authutil.o:	authutil.c
			$CC $INC $DEF  authutil.c

$LIB/connect.o:		connect.c
			$CC $INC $DEF $INC connect.c

$LIB/error.o:		error.c
			$CC $INC $DEF  error.c

$LIB/getauth.o:		getauth.c
			$CC $INC $DEF  getauth.c

$LIB/iceauth.o:		iceauth.c
			$CC $INC $DEF  iceauth.c

$LIB/listen.o:		listen.c
			$CC $INC $DEF $INC listen.c

$LIB/locking.o:		locking.c
			$CC $INC $DEF  locking.c

$LIB/misc.o:		misc.c
			$CC $INC $DEF $INC misc.c

$LIB/ping.o:		ping.c
			$CC $INC $DEF  ping.c

$LIB/process.o:		process.c
			$CC $INC $DEF  process.c

$LIB/protosetup.o:	protosetup.c
			$CC $INC $DEF  protosetup.c

$LIB/register.o:	register.c
			$CC $INC $DEF  register.c

$LIB/replywait.o:	replywait.c
			$CC $INC $DEF  replywait.c

$LIB/setauth.o:		setauth.c
			$CC $INC $DEF  setauth.c

$LIB/shutdown.o:	shutdown.c
			$CC $INC $DEF  shutdown.c

$LIB/transport.o:	../xtrans/xtrans.c
			$CC $INC $DEF $INC/include/X11	\
				-D ICE_t -D TRANS_CLIENT	\
				-D TRANS_SERVER ../xtrans/xtrans.c

$LIB/watch.o:		watch.c
			$CC $INC $DEF  watch.c
