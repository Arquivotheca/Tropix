#*
#****************************************************************
#	Makefile para a biblioteca "lib/os"			*
#****************************************************************
#*
INC	=	-I ../../include				\
		-I ../../include/X11				\
		-I ../../sys/include

DEF	=	-D TROPIX

CC	=	cc
COPTS	=	-c -O -o $@
LIB	=	lib

#*
#****************************************************************
#	Geração da Biblioteca					*
#****************************************************************
#*
$LIB/.LIBSYM:	$LIB			\
		$LIB/barrier.o		\
		$LIB/basename.o		\
		$LIB/delay.o		\
		$LIB/ffs.o		\
		$LIB/fp.o		\
		$LIB/fulltime.o		\
		$LIB/gethostname.o	\
#		$LIB/index.o		\
		$LIB/inet.o		\
		$LIB/inout.o		\
		$LIB/ldl.o		\
		$LIB/msg.o		\
		$LIB/pty.o		\
		$LIB/statetostr.o	\
		$LIB/strcasecmp.o	\
		$LIB/strlcpy.o		\
		$LIB/sym.o		\
		$LIB/usleep.o		\
		$LIB/waitpid.o
		mklib $LIB

$LIB:		.exist
		mkdir -f $LIB

#*
#****************************************************************
#	Os diversos módulos da bilioteca			*
#****************************************************************
#*
$LIB/barrier.o:		barrier.c
			$CC $COPTS $DEF barrier.c

$LIB/basename.o:	basename.c
			$CC $COPTS $DEF basename.c

$LIB/delay.o:		delay.s
			as -o $@ delay.s

$LIB/ffs.o:		ffs.s
			$CC $COPTS $DEF ffs.s

$LIB/fp.o:		fp.s
			as -o $@ fp.s

$LIB/fulltime.o:	fulltime.c
			$CC $COPTS $DEF fulltime.c

$LIB/gethostname.o:	gethostname.c
			$CC $COPTS $DEF gethostname.c

#$LIB/index.o:		index.c
#			$CC $COPTS $DEF index.c

$LIB/inet.o:		inet.c
			$CC $COPTS $DEF $INC inet.c

$LIB/inout.o:		inout.s
			as -o $@ inout.s

$LIB/ldl.o:		ldl.c
			$CC $COPTS $DEF ldl.c

$LIB/msg.o:		msg.c
			$CC $COPTS $DEF msg.c

$LIB/pty.o:		pty.c
			$CC $COPTS $DEF pty.c

$LIB/statetostr.o:	statetostr.c
			$CC $COPTS $DEF statetostr.c

$LIB/strcasecmp.o:	strcasecmp.c
			$CC $COPTS $DEF strcasecmp.c

$LIB/strlcpy.o:		strlcpy.c
			$CC $COPTS $DEF strlcpy.c

$LIB/sym.o:		sym.c
			$CC $COPTS $DEF sym.c

$LIB/usleep.o:		usleep.c
			$CC $COPTS $DEF usleep.c

$LIB/waitpid.o:		waitpid.c
			$CC $COPTS $DEF waitpid.c
