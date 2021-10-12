#*
#****************************************************************
#	Makefile para o diretório "os"				*
#****************************************************************
#*
xc	=		../..

INC	=		-I .					\
			-I $xc/sys/include			\
			-I $xc/sys/Xext				\
			-I $xc/sys/render			\
			-I $xc/include				\
			-I $xc/include/X11			\
			-I $xc/include/X11/fonts		\
			-I $xc/include/X11/extensions		\
			-I $xc/lib/Xau

DEF	=		$STDDEF $EXTDEF				\
			-D XSERV_t				\
			-D TRANS_SERVER				\
			-D DDXOSINIT				\
			-D SERVER_LOCK				\
			-D DDXOSFATALERROR 			\
			-D DDXOSVERRORF				\
			-D USE_RGB_TXT				\
			-D entry=__entry__
#			-D DDXTIME				\
#			-D TCPCONN				\
#			-D UNIXCONN				\
#			-D HAS_STICKY_DIR_BIT 			\
#			-D HAS_FCHOWN				\
#			-D DBE					\

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		$xc/sys/lib/os

$LIB/.LIBSYM:		$LIB					\
			$LIB/WaitFor.o				\
			$LIB/access.o				\
			$LIB/connection.o			\
			$LIB/io.o				\
			$LIB/log.o				\
			$LIB/oscolor.o				\
			$LIB/osinit.o				\
			$LIB/utils.o				\
#			$LIB/auth.o				\
#			$LIB/mitauth.o				\
			$LIB/xtrans.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/WaitFor.o:		WaitFor.c
			$CC $COPTS $INC $DEF WaitFor.c

$LIB/access.o:		access.c
			$CC $COPTS $INC $DEF access.c

$LIB/connection.o:	connection.c
			$CC $COPTS $INC $DEF connection.c

$LIB/io.o:		io.c
			$CC $COPTS $INC $DEF io.c

$LIB/log.o:		log.c
			$CC $COPTS $INC $DEF log.c

$LIB/oscolor.o:		oscolor.c
			$CC $COPTS $INC $DEF oscolor.c

$LIB/osinit.o:		osinit.c
			$CC $COPTS $INC $DEF osinit.c

$LIB/utils.o:		utils.c
			$CC $COPTS $INC $DEF -D USE_CHMOD utils.c

$LIB/xtrans.o:		$xc/lib/xtrans/xtrans.c
			$CC $COPTS $INC -I $xc/lib/xtrans $DEF $xc/lib/xtrans/xtrans.c
#
#$LIB/auth.o:		auth.c
#			$CC $COPTS $INC $DEF auth.c
#
#$LIB/mitauth.o:	mitauth.c
#			$CC $COPTS $INC $DEF mitauth.c
#
#$LIB/secauth.o:	secauth.c
#			$CC $COPTS $INC $DEF secauth.c
#
#$LIB/xdmcp.o:		xdmcp.c
#			$CC $COPTS $INC $DEF xdmcp.c
