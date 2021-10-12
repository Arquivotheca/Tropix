#*
#****************************************************************
#	Makefile para a biblioteca "lib/zlib"			*
#****************************************************************
#*
INC	=

DEF	=		-D TROPIX

CC	=		cc
COPTS	=		-c -O -o $@
LIB	=		lib

$LIB/.LIBSYM:		$LIB					\
			$LIB/adler32.o				\
			$LIB/compress.o				\
			$LIB/crc32.o				\
			$LIB/deflate.o				\
			$LIB/gzio.o				\
			$LIB/infblock.o				\
			$LIB/infcodes.o				\
			$LIB/inffast.o				\
			$LIB/inflate.o				\
			$LIB/inftrees.o				\
			$LIB/infutil.o				\
			$LIB/trees.o				\
			$LIB/uncompr.o				\
			$LIB/zutil.o
			mklib $LIB

$LIB:			.exist
			mkdir -f $LIB

$LIB/adler32.o:		adler32.c
			$CC $COPTS $INC $DEF adler32.c

$LIB/compress.o:	compress.c
			$CC $COPTS $INC $DEF compress.c

$LIB/crc32.o:		crc32.c
			$CC $COPTS $INC $DEF crc32.c

$LIB/deflate.o:		deflate.c
			$CC $COPTS $INC $DEF deflate.c

$LIB/gzio.o:		gzio.c
			$CC $COPTS $INC $DEF gzio.c

$LIB/infblock.o:	infblock.c
			$CC $COPTS $INC $DEF infblock.c

$LIB/infcodes.o:	infcodes.c
			$CC $COPTS $INC $DEF infcodes.c

$LIB/inffast.o:		inffast.c
			$CC $COPTS $INC $DEF inffast.c

$LIB/inflate.o:		inflate.c
			$CC $COPTS $INC $DEF inflate.c

$LIB/inftrees.o:	inftrees.c
			$CC $COPTS $INC $DEF inftrees.c

$LIB/infutil.o:		infutil.c
			$CC $COPTS $INC $DEF infutil.c

$LIB/trees.o:		trees.c
			$CC $COPTS $INC $DEF trees.c

$LIB/uncompr.o:		uncompr.c
			$CC $COPTS $INC $DEF uncompr.c

$LIB/zutil.o:		zutil.c
			$CC $COPTS $INC $DEF zutil.c
