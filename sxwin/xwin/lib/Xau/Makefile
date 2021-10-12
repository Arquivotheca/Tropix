#*
#****************************************************************
#	Makefile para a biblioteca "lib/Xau"			*
#****************************************************************
#*
INC	=		-I .					\
			-I ../../include

DEF	=		-D CSRG_BASED				\
			-D TROPIX				\
			-D entry=__entry

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		lib

$LIB/.LIBSYM:		$LIB					\
			$LIB/AuDispose.o			\
			$LIB/AuFileName.o			\
			$LIB/AuGetAddr.o			\
			$LIB/AuGetBest.o			\
			$LIB/AuLock.o				\
			$LIB/AuRead.o				\
			$LIB/AuUnlock.o				\
			$LIB/AuWrite.o
			mklib $LIB

.virtual		links

links:
			ln -f Xauth.h ../../include/X11

$LIB:			.exist
			mkdir -f $LIB

$LIB/AuDispose.o:	AuDispose.c
			$CC $COPTS $INC $DEF AuDispose.c

$LIB/AuFileName.o:	AuFileName.c
			$CC $COPTS $INC $DEF AuFileName.c

$LIB/AuGetAddr.o:	AuGetAddr.c
			$CC $COPTS $INC $DEF AuGetAddr.c

$LIB/AuGetBest.o:	AuGetBest.c
			$CC $COPTS $INC $DEF AuGetBest.c

$LIB/AuLock.o:		AuLock.c
			$CC $COPTS $INC $DEF AuLock.c

$LIB/AuRead.o:		AuRead.c
			$CC $COPTS $INC $DEF AuRead.c

$LIB/AuUnlock.o:	AuUnlock.c
			$CC $COPTS $INC $DEF AuUnlock.c

$LIB/AuWrite.o:		AuWrite.c
			$CC $COPTS $INC $DEF AuWrite.c
