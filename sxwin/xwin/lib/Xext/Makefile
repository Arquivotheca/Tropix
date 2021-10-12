#*
#****************************************************************
#*	Makefile para a biblioteca Xext				*
#****************************************************************
#*
LIB	=	lib

DEF	=	-D CSRG_BASED		\
		-D entry=_entry_	\
		-D TROPIX

INC	=	-I .			\
		-I ../X11		\
		-I ../../include

CC	=	cc
COPTS	=	-c -O -o $@

#*
#****************************************************************
#*	Geração da biblioteca					*
#****************************************************************
#*
OBJ	=	$LIB/globals.o		\
		$LIB/extutil.o		\
		$LIB/XMultibuf.o	\
		$LIB/XShape.o		\
		$LIB/XShm.o		\
		$LIB/MITMisc.o		\
		$LIB/XTestExt1.o	\
		$LIB/XSync.o

$LIB/.LIBSYM:	$LIB $OBJ
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

#*
#****************************************************************
#*	Compilação dos Diversos Módulos				*
#****************************************************************
#*
$LIB/globals.o:		globals.c
			$CC $COPTS $INC $DEF globals.c

$LIB/extutil.o:		extutil.c
			$CC $COPTS $INC $DEF extutil.c

$LIB/XMultibuf.o:	XMultibuf.c
			$CC $COPTS $INC $DEF XMultibuf.c

$LIB/XShape.o:		XShape.c
			$CC $COPTS $INC $DEF XShape.c

$LIB/XShm.o:		XShm.c
			$CC $COPTS $INC $DEF XShm.c

$LIB/MITMisc.o:		MITMisc.c
			$CC $COPTS $INC $DEF MITMisc.c

$LIB/XTestExt1.o:	XTestExt1.c
			$CC $COPTS $INC $DEF XTestExt1.c

$LIB/XSync.o:		XSync.c
			$CC $COPTS $INC $DEF XSync.c
